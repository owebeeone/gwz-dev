# Step 1.7 driver-only construction — remediation plan, round 2 (last round under the cap)

Object: gwz-core `43f4ef0c` (lane p1-7). Re-verdicts: `GwzDriverOnly-Step1.7-ReviewCode-2.md` (GO; new P3-1, P3-2)
and `GwzDriverOnly-Step1.7-ReviewState-2.md` (NO-GO on new P2-1; new P3-1). Both reviewers classify every new finding
as a residual of round-1 root causes, not a new architectural root cause. State pre-committed to GO on a revision
resolving its P2-1 as specified.

**Blind convergence (round 2):** Code P3-2 = State P3-1 (the clone judge predicts the rewrite from the default
configuration; `includeIf "gitdir:"` in the new repository's configuration diverges; fails closed, unnamed). Code
P3-1 = State §4 item 1 (test rows move libgit2's process-wide global search path in the shared test binary).
**Disagreement to settle with evidence:** State P2-1 says `--identity` is dropped for `[::1]:repo` and the endpoint
authenticates with its agent; Code measured every bracketed-IPv6 SCP spelling refused core-side at the SSH destination
parse ("invalid SSH destination", no connection), making the identity split moot today. The fix below removes the split
either way; the new row records the real end-to-end outcome.

One merged patch on top of `43f4ef0c`; the lane commit is amended (unpushed); receipt label `p1-7-20261030-c`.

| ID | Disposition | Closure test |
|---|---|---|
| State P2-1 — identity selection uses `scp_like`, a third classifier | Fix. `resolve_remote`'s SSH decision uses `bound_route(url) == Bound::Ssh` (keeping `ssh_scheme` for scheme forms) in cli context. Local placement is left unchanged and the split there is recorded as a finding (no behaviour change outside cli in this step). | Route-table row: for every URL in `the_check_refuses_exactly_what_the_binding_does_not_bind`'s list, `bound_route == Ssh` implies identity selection treats it as SSH under cli. Harness row for `[::1]:repo` with a per-remote override and with an invocation identity: assert the end-to-end outcome actually observed (refused by name with zero connections, or the selected identity reaching `HostRoute`), and that no path drops a selected identity silently. |
| Code P3-2 = State P3-1 — clone/read judge misses conditional includes | Fix by observing: in `clone_repo_named`'s `remote_create` callback compare the created remote's `url()` with the judged URL and refuse with the created URL's route name on mismatch or unbound (libgit2 removes its target). For `read_remote_file`, the same observation if its remote is created the same way; otherwise state why its scratch path cannot match a `gitdir` rule and correct the comment. | The reviewers' includeIf probe as a row: named `file://` and `http://` refusals, zero connections, no target. |
| Code P3-1 — global search path moved in the shared test binary | Fix. The global-configuration rows run in a re-executed child of the test binary (as the scratch row does), with `HOME`/`XDG_CONFIG_HOME` (or `GIT_CONFIG_GLOBAL` if the fork honours it) pointed at the row's directory; remove `set_search_path`/`reset_search_path` and the static mutex. | No `opts::set_search_path` under `src/` outside a child-only path; the rows still kill M11. |
| Wording (Code §3) | The IPv6 comment and test name say what is measured; `gwz_git_boundary.json` `transport.rs` entry gets `dated` 2026-10-30 and its `responsibility` names effective-URL resolution. | Read. |

The contract §15 row text is finalised by the lane owner after this round from the implementer's corrected text.
