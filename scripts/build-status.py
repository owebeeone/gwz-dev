#!/usr/bin/env python3
"""Bazel workspace status; SHA-256 framing matches core build_support/provenance.rs."""
import hashlib
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
EXCLUDED = {"target", ".git", "__pycache__", ".venv", ".regen-venv"}
INPUTS = ["Cargo.toml", "Cargo.lock", "build.rs", "src", "crates", "build_support", "protocol/gwz.taut.py"]


def git(root, *args):
    try:
        result = subprocess.run(["git", "-C", str(root), *args], capture_output=True, text=True)
        return result.stdout.strip() if result.returncode == 0 else None
    except OSError:
        return None


def source_digest(root):
    inputs = []
    def walk(path):
        if path.is_symlink():
            inputs.append(path)
        elif path.is_dir():
            for child in path.iterdir():
                if child.name not in EXCLUDED:
                    walk(child)
        elif path.is_file():
            inputs.append(path)
    for name in INPUTS:
        walk(root / name)
    digest = hashlib.sha256()
    for path in sorted(inputs, key=lambda item: item.relative_to(root).as_posix()):
        key = path.relative_to(root).as_posix().encode()
        link = path.is_symlink()
        data = str(path.readlink()).encode() if link else path.read_bytes()
        digest.update(len(key).to_bytes(8, "big"))
        digest.update(key)
        digest.update(bytes([link]))
        digest.update(len(data).to_bytes(8, "big"))
        digest.update(data)
    return digest.hexdigest()


def identity(root):
    toplevel = git(root, "rev-parse", "--show-toplevel")
    own = toplevel is not None and Path(toplevel).resolve() == root.resolve()
    revision = git(root, "rev-parse", "HEAD") if own else None
    diff = git(root, "diff", "--name-only", "HEAD", "--") if own else None
    untracked = git(root, "ls-files", "--others", "--exclude-standard") if own else None
    dirty = "unknown" if diff is None or untracked is None else str(bool(diff or untracked)).lower()
    return f"revision={revision or 'unavailable'} dirty={dirty} source-sha256={source_digest(root)} build=bazel"


if __name__ == "__main__":
    for member, key in [("gwz-core", "CORE"), ("gwz-cli", "CLI")]:
        print(f"STABLE_GWZ_{key}_PROVENANCE {identity(ROOT / member)}")
