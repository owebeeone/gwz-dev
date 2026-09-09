#!/usr/bin/env python3
"""Materialize a locked CI workspace, optionally testing one explicit candidate.

The root manifest/lock are never edited by this script. GWZ owns materialization.
A candidate checkout replaces code at an existing member path; the evidence
records both the lock revision and actual candidate SHA, not a floating branch.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import yaml

SHA = re.compile(r"[0-9a-f]{40}\Z")


def private_member_paths(root):
    manifest = yaml.safe_load((root / "gwz.conf/gwz.yml").read_text())
    return [item["path"] for item in manifest["members"] if item.get("private")]


def resolve(root, member, revision):
    lock = yaml.safe_load((root / "gwz.conf/gwz.lock.yml").read_text())
    manifest = yaml.safe_load((root / "gwz.conf/gwz.yml").read_text())
    members = {item["id"]: item for item in manifest["members"]}
    private_paths = {item["path"] for item in manifest["members"] if item.get("private")}
    if bool(member) != bool(revision):
        raise ValueError("candidate member and exact SHA must be supplied together")
    candidates = {item["path"]: key for key, item in members.items()}
    if member and member not in candidates:
        raise ValueError("candidate is not a recorded workspace member")
    if member in private_paths:
        raise ValueError("candidate cannot be a private workspace member")
    if revision and not SHA.fullmatch(revision):
        raise ValueError("candidate revision must be a full commit SHA")
    rows = []
    for key, state in lock["members"].items():
        item = members[key]
        if item.get("private"):
            continue
        path = Path(state["path"])
        if path.is_absolute() or ".." in path.parts or state["path"] != item["path"]:
            raise ValueError("unsafe or inconsistent member path")
        commit = state.get("commit", "")
        if state.get("source_kind") != "git" or not SHA.fullmatch(commit):
            raise ValueError("every CI member must have an exact Git lock commit")
        rows.append({"id": key, "path": str(path), "locked_revision": commit,
                     "tested_revision": revision if str(path) == member else commit})
    if member and not any(row["path"] == member for row in rows):
        raise ValueError("candidate is missing from the workspace lock")
    return rows


def command(root, *args):
    return subprocess.check_output(args, cwd=root, text=True).strip()


def materialize_public_members(root, bootstrap, private_paths):
    args = [bootstrap]
    for path in private_paths:
        args.extend(["--no-target", path])
    args.extend(["materialize", "--lock"])
    command(root, *args)


def pin_revisions(root, rows):
    # GWZ creates the workspace/member layout. In this ephemeral CI checkout,
    # attach-to-branch materialization may follow upstream beyond the lock;
    # explicitly detach every existing member at the resolved test revision.
    for row in rows:
        member = root / row["path"]
        expected = row["tested_revision"]
        if command(member, "git", "rev-parse", "HEAD") != expected:
            command(member, "git", "fetch", "--no-tags", "origin", expected)
            command(member, "git", "checkout", "--detach", expected)
        if command(member, "git", "rev-parse", "HEAD") != expected:
            raise ValueError(f'{row["path"]}: checkout does not match the resolved tuple')


def cargo_lock_state(root, has_candidate):
    if has_candidate:
        command(root, "cargo", "update", "--workspace")
    lock = (root / "Cargo.lock").read_bytes()
    return {
        "cargo_lock_derived": has_candidate,
        "cargo_lock_sha256": hashlib.sha256(lock).hexdigest(),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bootstrap", required=True)
    parser.add_argument("--candidate-member", default="")
    parser.add_argument("--candidate-sha", default="")
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    if os.environ.get("GITHUB_ACTIONS") != "true":
        raise ValueError("materialization helper is restricted to ephemeral GitHub Actions runners")
    root = Path.cwd()
    rows = resolve(root, args.candidate_member, args.candidate_sha)
    # CI uses public HTTPS for the manifest's GitHub SSH URLs. This only affects
    # the ephemeral runner's Git configuration, never the shared GWZ artifacts.
    command(root, "git", "config", "--global", "url.https://github.com/.insteadOf", "git@github.com:")
    materialize_public_members(root, args.bootstrap, private_member_paths(root))
    pin_revisions(root, rows)
    report = {
        "workspace_revision": command(root, "git", "rev-parse", "HEAD"),
        "members": rows,
        **cargo_lock_state(root, bool(args.candidate_sha)),
    }
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
