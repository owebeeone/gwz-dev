<!-- Draft: GWZ v0.9.1 announcement. Venue-neutral (blog / HN / forum post). -->

# GWZ — Git Workspace Zone

Dissatisfied with the existing multi-repo Git tools, I made one that works much
like using git itself — hence `gwz`. The similarities don't end at gwz being
three letters starting with "g": I wanted the gwz workflow to feel like a git
workflow.

v0.9.2 is out —
[installers on GitHub](https://github.com/owebeeone/gwz-cli/releases/latest),
`pip install gwz` for the Python bindings and CLI, and
[documentation](https://owebeeone.github.io/gwz-cli/) on GitHub Pages.

## Why another one

I tried what's out there, and every tool lost me somewhere. vcstool is
deliberately stateless — it fans commands out over whatever working copies it
finds on disk. Export a YAML if you want a record, remember to commit that
YAML somewhere, and don't ask for a stash. Google's repo and Zephyr's west
sync you onto detached HEADs and are shaped around the ecosystems they grew
up in. Submodules do record composition — that part they got right — but the
day-to-day you already know.

And the thing I wanted most, nobody had: attempt a merge across every
repository at once, see exactly which members conflict, and keep a recorded
state to walk back to when the answer is ugly. In gwz that's `gwz snapshot`,
then `gwz branch --merge <ref>` — clean members merge, conflicted members are
reported with their conflicting paths (native merge state left in place, like
git), and the snapshot is there to `gwz materialize` back to.

## What it is

- Member repositories remain ordinary Git repositories. No submodules
  unexpectedly detaching (unless you configured it that way).
- The root is a small tracked Git repository that records composition and
  reproducible state: manifest and lock under `gwz.conf/`, snapshots and
  commit markers to recover workspace state later.
- `gwz status | diff | add | commit | pull | push | tag | branch | stash |
  clone` play roles similar (not identical) to their git namesakes — across
  the workspace, with per-member results.
- Membership is explicit and has a lifecycle:
  `gwz repo create | clone | add | sync | detach | attach`. Detach keeps the
  member's designation; attach verifies recorded commit evidence instead of
  guessing from directory names.

The everyday loop is the point:

```sh
gwz clone https://github.com/owebeeone/gwz-dev.git gwz-dev
cd gwz-dev                        # gwz's own dev tree is a gwz workspace
gwz status
gwz snapshot before-change
gwz branch --create feature/x --switch
# ...edit across members...
gwz add -A
gwz commit -m "One change, several repositories"
gwz --dry-run pull --head         # plan it before anything moves
gwz stash push -u -m half-done    # one stash bundle, not per-repo archaeology
```

## Fan-out tools, submodules, and the difference

Fan-out tools treat the workspace as whatever is on disk right now. gwz treats
it as a durable, versioned object: composition changes are commits in the
root, reviewed alongside the source changes that need them; members keep
stable identities across detach and re-attach; operations share one selection
and policy model (`--dry-run`, `--partial`, fast-forward-only by default) and
report typed per-member results — `--json`/`--jsonl` when a machine is
reading. Submodules pin composition too; the difference is that gwz members
stay normal checkouts on normal branches, with the record beside them rather
than wrapped around them.

The longer, fairer version of this comparison is
[Why GWZ](https://github.com/owebeeone/gwz-core/blob/main/docs/WhyGwz.md).

## The core is a message service

The engine, [gwz-core](https://github.com/owebeeone/gwz-core), is not fused to
the CLI. Every operation is a named request/response message on a
[Taut](https://github.com/owebeeone/taut)-defined service, with deterministic
CBOR encoding and a schema-driven JSON form. The Rust CLI is just a client.
The [Python bindings](https://pypi.org/project/gwz/) are another client of the
same engine. A UI, an agent, or CI can embed the core in-process — or host it
beside the checkout and drive it from another process or machine. gwz-core
supplies the contracts, not a daemon; transport and auth are the embedder's
choice, on purpose.

Taut is another of my projects: a take on protobuf that is lighter and less
opinionated — the schema is a few lines of typed Python, the wire is
deterministic CBOR you can read in an afternoon, no plugin toolchain, nine
languages verified against a shared golden corpus.

## Status

v0.9.2 is functional and self-hosting — gwz develops gwz in a gwz workspace
(the `gwz clone` above fetches it). It is pre-1.0: the protocol and parts of
the command surface may still move. GPL-2.0-only, the same license family as
git.

## The feedback I want

- Point it at your own multi-repo pile — `gwz init`, then `gwz repo add` each
  existing checkout; members aren't modified — and tell me where the model
  doesn't fit.
- The first missing command that would stop you adopting it.
- Whether the member lifecycle rules (detach/attach with commit evidence) read
  as reassuring or as bureaucracy.
- If the embeddable core interests you: what would you put in front of it?

Issues: <https://github.com/owebeeone/gwz-cli/issues> — or reply here.
