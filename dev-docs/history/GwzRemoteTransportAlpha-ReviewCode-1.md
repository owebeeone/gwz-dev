# Local alpha correction 1 — CODE RE-VERDICT

**Tuple:** root `84961962fb4879f591c7195fb035d8c34d920b38`; core `3b79fb26d731cce565a2319a2ae2d512d4d54c51`; CLI `ab59011db0ee00ab0c032fc23fc06b2578bf7b68`; evidence `73370827da996b8bdfdcc817e81edc0286b6abe1`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. All seven HEADs matched at both boundaries.

**Verdict: GO** — prior Code GO is retained; no changed-range finding.

The two-line dispatch correction removes `Snapshot` and local-only tag operations from alpha transport construction. It retains transport ownership for `TagOp::{Fetch,Push}` and for `List`/`Delete` when `remote` is present, matching the actual handler call graph: local create/list/delete and snapshots perform no remote exchange, while the retained variants call `ls_remote`, `push`, or `tag_fetch`.

The archived CLI source fingerprint matches the committed blob. Its actual-binary red reproduces local tag failure from unrelated CA configuration; the corrected run proves local snapshot/tag isolation, refusal for all four remote tag variants under invalid proxy configuration, and continued HTTPS clone/fetch/push behavior.

No core, protocol, cleanup, authentication, or fallback behavior changed.
