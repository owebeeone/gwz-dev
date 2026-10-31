# Steps 2.3b and 2.3c (carried connection fixes; member lock manager) — remediation plan, round 1

**Status (2026-10-31):** round 2, fresh reviewers: `GwzServerCore-Step2.3c-ReviewState-2.md` and `-ReviewCode-2.md` both report **GO** at lane p2-3bc2 root `57279067` / gwz-core `fc27ccd6` / gwz-transport `281a28d` / gwz-core-evidence `447592e4`; WSL matrix `p2-3bc2-20261031-b` PASS (it settles deviation 7: the four Linux rows pass on ext4). Every round-1 row is closed. New P3s only, none architectural (the architectural count stays at one). Blind convergences: State R2S-1 = Code P3-N1 (path key, not filesystem identity) and State R2S-2 = Code P3-N2 (guard lifetime unpinned); State §4's Windows `canonicalize` hypothesis = Code P3-N4. Dispositions: P3-N1 and P3-N4 carried into 2.3f, P3-N2, P3-N3 and P3-N5 into 2.3h (`GwzServerCore-Step2.3-Plan.md`). P3-B1 and P3-B2 recorded in contract §15 at this landing. Windows receipt `p2-3bc2-20261031-a` FAILED on one row outside the change (`https_auth::runner::process_tests::cancel_kills_the_helper_and_its_descendants`, failing before in p1-6-20261025-b and -d; findings F9); the receipt of record is the re-run `p2-3bc2-20261031-b` on the same tree. Acceptance is conditional on that receipt.

**Objects:**
- 2.3b: gwz-transport `a3f9ed0` and gwz-core `76507c49`.
- 2.3c: gwz-core `3d4cff0a`.
- All in lane p2-3bc.

**Reviews:**
- `GwzServerCore-Step2.3bc-ReviewState.md` (single State review of both steps):
  - 2.3b GO, P3-B1 to P3-B4;
  - 2.3c NO-GO, P2 S-1, P3-C1 to P3-C7.
- `GwzServerCore-Step2.3c-ReviewCode.md`: the second axis, escalated by S-1. NO-GO, P2-1, P3-1 to P3-6.

**Blind convergence** (the Code reviewer had no access to the State report):
- State S-1 = Code P2-1: the lock key is the request's lexical spelling of the workspace root, not its filesystem identity. Two spellings (a symlink, a case variant, a Windows short name) of one workspace never exclude each other. Both reproduced it at the served level.
- State P3-C6 = Code P3-2, labelled ARCHITECTURAL by both: `workspace_ops` now imports `session_host::members`, closing a cycle with `session_host::served` → `workspace_ops`.
- State P3-C2 = Code P3-3: `push_member` and `push_root` keep unlocked, dead non-dry-run `backend.push` branches.
- State P3-C1 = Code P3-1: the module docs still say tag fetch is not plumbed.
- State P3-C7 = Code P3-6: public API removal recorded for the release.

**Architectural root causes on 2.3c:** one, the layering cycle.

**Re-landing.** Root moved while the lane was under review:
- 2.3a landed;
- gwz-transport `21726ca` (R2a) is now a sibling of the lane's `a3f9ed0`, and both change `exchange.rs` and `state_rows.rs`.

Following the 2026-10-25 finding (gwz merge refuses resolution edits outside conflict paths), the three objects are re-applied as patches in a fresh lane p2-3bc2 cloned from root. The pin then moves to the re-landed gwz-transport commit, and the remediation is one further patch.

**Next round:** the remediation moves a module boundary and changes the call graph, so round 2 goes to fresh reviewers on both axes, not continued ones.

## Dispositions

| ID | Disposition | Closure test |
|---|---|---|
| S-1 = P2-1 (P2) | **Fix.** `MemberLockManager::lock` keys the workspace by `std::fs::canonicalize` of the root (on Windows the verbatim on-disk form). If canonicalization fails, the step is refused with that I/O error; it never falls back to the lexical spelling. The module table states the key. | Three served rows through `CoreDispatch` with real git, each answering `cancelled` while another holder holds the member: (1) the resolved spelling (control); (2) a symlink to the root (Unix, inside `cfg_if!`); (3) a case variant of the root, skipped with a printed reason on a case-sensitive volume (it runs on macOS APFS, `/Volumes/gwztmp` and Windows). A manager row: a symlink spelling waits. A control row: two genuinely different workspaces do not wait. The mutation that drops the canonicalization turns rows (2) and (3) and the manager row red. |
| P3-C6 = P3-2 (P3, ARCH) | **Fix.** Move `MemberLockManager`, `BoundMemberLocks` and the per-call manager into `operation`, beside `MemberSteps` and `MemberMutationGuard`. `HostContext` owns one instance and serves it; `workspace_ops` names only `operation`. | `git grep -n session_host -- src/workspace_ops` finds nothing outside `tests/`. A new source check (below) enforces it. |
| P3-C2 = P3-3 (P3) | **Fix.** Remove the proven-dead `dry_run` parameter and the non-dry-run `backend.push` branches of `push_member` and `push_root` (standing rule: dead code is removed in the change that proves it dead). Add a fast syntax-aware source check, `scripts/checks/check_member_step_boundary.py` with a unit test, that refuses (a) any `session_host` path in `workspace_ops` production files, and (b) any `backend.fetch(`, `.push(`, `.push_prepared(`, `.tag_fetch(` call in `workspace_ops` production files that does not follow a member-step entry (`.enter(`) in the same function body. A short allow-list names any read-only exception with its reason. Wire it into the source checks the gates run. | The check passes on the tree and fails on mutations I (fetch step unlocked), J (push member unlocked) and R (tag fetch unlocked), and on an added `use crate::session_host` line in a `workspace_ops` file. The existing push rows stay green. |
| P3-C1 = P3-1 (P3) | **Fix.** Rewrite the module tables in `members.rs` (moved), `served/dispatch.rs` and `membermutationguard.rs`: tag's push and fetch are locked; the only exclusion is the dry-run plans; who takes the port. | Text review; a grep finds no "not plumbed". |
| P3-C3 (P3) | **Fix.** `tag --remote … delete` (`handle_tag.rs`, the `:refs/tags/<name>` push) takes the member step like tag push. | A row: the delete waits behind another holder of the member, then succeeds. Mutation: unlocked, red. |
| P3-C4 (P3) | **Fix.** The pull preflights read the remote-tracking ref (`read_ref(remote_ref)`) inside the same guard scope as their `backend.fetch`, as `fetch_one` does. | Review of the three sites. The pull rows stay green. |
| P3-C5 (P3) | **Fix.** Add a test-only waiter count on the manager (like `held_count`, inside the existing `cfg_if!` test arm), and make each "it is waiting" row assert that the count reached one, not only that a `recv_timeout` elapsed. | The rows fail under the lock-less mutation A deterministically. A spot check of three rows with the 150 ms window raised to 2 s shows the same verdicts. |
| P3-4 (P3, Code) | **Fix.** `Session::reply` in `served/tests.rs` waits with `recv_timeout` against its deadline. In `a_handler_port_without_a_token_waits_and_with_one_is_cancellable`, take the two per-call entries in sequence, not nested. | Re-run mutations D and G: every red row fails within its own bound, with no harness kill. |
| P3-5 (P3, Code) | **Fix.** Tag push carries the member id with its plan (zip `selected`, as the fetch arm does). The `@root` fallback search is removed. | The tag rows stay green, and no fallback exists. |
| P3-C7 = P3-6 (P3) | **Fix and record.** `MemberMutationGuard` and the manager become `pub(crate)` in this same breaking change; no caller outside gwz-core exists (grepped by the Code reviewer). Record both breaks (`operation::MemberLockManager` removed; `MemberMutationGuard` lifetime dropped and made crate-private) in gwz-core's unreleased notes, if the repository keeps them, or else in the step evidence README for the 1.1.0 release checklist. | The record exists and names both. |
| P3-B1 (2.3b) | **Record.** The capacity wait after an abort has no bound of its own; it ends on the route's terminal, a cancel or carrier loss. The lane owner records this in contract §15 at landing. | Text. |
| P3-B2 (2.3b) | **Lane owner at landing.** Contract §15: the `OpenFailure::Fault` sentence now says that an open at the bound after an abort waits; `streams_per_binding` is held to 64; the 65th open is `BindingLimit`. | Text. |
| P3-B3 (2.3b) | **Fix.** One sentence on the `Wait` enum doc (gwz-connection `remote/core.rs`): `Opening` and `Ending` each have exactly one owning future, so one waiter per `Wait` suffices. | Text. |
| P3-B4 (2.3b) | **Record.** `streams_per_binding` above 64 is clamped silently; this is documented in the field doc and README. A visible signal at composition is left for 2.3h, which composes the per-call transport. | — |
| Code §3 risk | **Record for 2.3g:** any second `ServedSession` constructor (the byte-stream session) must take `host.members()`. Mutation C covers only `serve_in_process`. | Carried into the 2.3g brief. |
| Code §4 | **Record for 2.3i** (C20, the workspace registry): W-class clones (`materialize`, `clone_repo_member`) write a member's refs from the network with no member lock. §5.1 relies on the class rules and the registry, which do not exist yet. | Carried into the 2.3i brief. |

**Not changed:**
- the manager's blocking, poison and cancel behaviour, which both axes verified;
- the `MemberSteps` port's one-argument shape;
- the public handler signatures;
- 2.3b's logic.

**Gates:**
- Mac and Linux container rows for `session_host`, `operation`, `workspace_ops::tests`, `g26`, `g29` and `remote_lane`, ordinary and candidate shapes;
- every closure mutation red, then restored;
- the source checks, including the new one; fmt; clippy; the candidate clippy ratchet;
- `run_boundary_job.py`;
- WSL and Windows receipt `p2-3bc2-20261031-a`. The rows include a Windows case-variant path, so the receipt is required and `ci-only` no longer applies to 2.3c.
