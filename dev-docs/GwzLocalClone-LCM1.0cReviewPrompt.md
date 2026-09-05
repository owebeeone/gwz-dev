# GwzLocalClone-LCM1.0c — review prompt (plan stage 1.0d, dual peer-blind)

Generated 2026-09-05 by the lane owner from the review-loop canonical
template (`~/.claude/skills/review-loop/references/review-prompt-template.md`).
One body, two axis sections. Axis A `Code` dispatched to Fable 5.1 (medium
effort budget); Axis B `State` dispatched to Opus 5, per operator instruction.
Reports are filed verbatim as `GwzLocalClone-LCM1.0c-ReviewCode.md` and
`GwzLocalClone-LCM1.0c-ReviewState.md`.

---

## Prompt body (common to both axes)

```text
You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: {AXIS}
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone.
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as {REPORT FILENAME} — write it as a
  standalone document a later auditor can read without this conversation.
- The question this review answers (plan LCM1.0d): may parallel feature
  lanes open on this skeleton? GO/NO-GO on that question.

READ-ONLY RULES
- Modify nothing: no file writes anywhere (not even a scratchpad — your report
  is your final message), no git mutations, no `cargo clean`/`cargo update`.
  Inspection commands only, from the COMMANDS list. Foreground only; never
  start a background job; every command must finish within 10 minutes.
- Verify the tuple at start AND at end; if it moved, stop and report the
  discrepancy instead of a verdict.
- Shell cwd resets between tool calls: always `cd` with absolute paths.

EXACT TUPLE (the object under review — nothing else is in scope)
- gwz-dev (root, /Users/owebeeone/limbo/gwz-dev): fc8b18966f24c14f422147d16f61a0dd19159670
- gwz-core: 52468cac728b7b5a877071187de2cb04196821ec — lane C commits on
  87207c2: 7faf475, a69f460, f6b1283, 06d7a00, e169877, 52468ca
- gwz-cli: 86840f67e23a9ef04312df0dd7c8f781cc476e04 — one commit on c04b488
- gwz-py: afcd5a396ccaf5e64884f8f1cec2f66273ac08bb — one commit on 65fc667
- Object: the LCM1.0c skeleton = diff ranges gwz-core 87207c2..52468ca,
  gwz-cli c04b488..86840f6, gwz-py 65fc667..afcd5a3, PLUS lane C's record
  dev-docs/GwzLocalClone-LCM1.0c-Checkpoint.md at gwz-dev fc8b189 — its
  claims (gates, timings, isolation evidence, stubs) are part of the object
  and may be refuted.
- Controlling documents (all at gwz-dev fc8b189, unchanged since 0c9f715):
  dev-docs/GwzLocalCloneLibraryBoundaries.md rev 1 (§2 packages/edges, §3
  interface checkpoint, §4 ownership, §5 tiers, §6 gate);
  dev-docs/GwzLocalClonePlan.md rev 4 (§2 seams, §3 "LCM1.0" 1.0c exit and
  1.0d); dev-docs/GwzLocalCloneImplementationArchitecture.md rev 3 (§2–§8);
  dev-docs/GwzLocalCloneDesign.md rev 8 (§3, §4.0, §6, §7).
- Supporting inputs: dev-docs/GwzLocalClone-ReviewPlanArch-Adoption.md;
  dev-docs/GwzLocalClone-ReviewPlanArch-F51.md §2 (P2-1..P2-4 and their
  closure criteria — this skeleton exists to close them);
  /Users/owebeeone/limbo/glade-wz/dev-docs/LibraryBoundaryAndTestingPolicy.md
  (LBT-001..012, adopted by the boundaries document).
- Out of scope (do not report as findings): lane T's `local-testrepo` crate
  (not yet built); feature behaviour (every new operation refuses by design);
  Bazel (not run by lane C — a residual risk, not a finding); the record's §7
  operator questions 1–8 (already logged — report only if a §7 item hides a
  defect the record misclassifies); the record §2.5 heading's stale gwz-py SHA
  `571ca97` (known nit); the boundaries §5 wording `cargo test -p` versus the
  landed manifest-path CI form (known, record §7.6) — judge the landed form's
  correctness, not the wording gap.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by
  dev-docs/GwzProcessOptimization.md (severity/verdict contract L1-19,
  finding form L1-20). Read AGENTS_GWZ.md and gwz-core/AGENTS.md.
- Best-effort scope is controlling (design rev 8 header, plan §1): the object
  must NOT add catalog, identity, journal, replay or lifecycle machinery —
  report any such addition; do not demand any.

COMMANDS (the only commands you may run)
- Tuple: `git -C /Users/owebeeone/limbo/gwz-dev rev-parse HEAD` and the same
  for `gwz-core`, `gwz-cli`, `gwz-py`; `git -C <repo> status --short` must be
  empty (only `.gitignore`d build output may exist).
- Reading history: `git -C <repo> log --stat <base>..<tip>`, `git show`,
  `git diff <base>..<tip> -- <path>`; plain file reads and grep.
- Cheap gates, from /Users/owebeeone/limbo/gwz-dev/gwz-core:
  `python3.13 scripts/checks/check_checked_artifact_boundaries.py` (~10 s);
  `python3.13 scripts/checks/check_local_clone_boundaries.py` (<1 s);
  `python3.13 -m unittest scripts/checks/test_check_local_clone_boundaries.py` (~2 s);
  `python3.13 protocol/regen.py --check`;
  `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 87207c2 HEAD` (~50 s).
- Targeted cargo, from /Users/owebeeone/limbo/gwz-dev (warm; builds write only
  to target/): `cargo test -p gwz-<name> --lib` for any of the 13 packages
  under gwz-core/crates/ (~1 s each); `cargo test -p gwz-core --lib local_clone`;
  `cargo test -p gwz-core --lib workspace_ops::merge::validate`;
  `cargo test -p gwz-core --lib workspace_ops::tests::g01::tracking_backend`;
  `cargo test -p gwz-core --lib operation::`; `cargo test -p gwz-core --test protocol`;
  `cargo test -p gwz-core --lib -- --list` (census only);
  `cargo metadata --format-version 1 --no-deps`; `cargo tree -p <pkg> -e normal,dev,build`.
- gwz-py, from /Users/owebeeone/limbo/gwz-dev/gwz-py:
  `.venv/bin/python scripts/check_protocol_drift.py`;
  `.venv/bin/python -m pytest src/tests/test_protocol.py src/tests/test_codec.py -q`.
- FORBIDDEN: any unfiltered `cargo test -p gwz-core` (the whole suite, ~16 min);
  `test_check_checked_artifact_boundaries.py` (~16 min);
  `run_r4bg_aggregate_gates.py` (~20 min); `test_v1_lifecycle_privacy_probe.py`;
  `gwz-py/run_tests.py`; `cargo clean`/`cargo update`/`--locked` from any
  standalone checkout; creating checkouts or worktrees; any `git` mutation;
  any file write; any background job.

SEVERITY AND VERDICT CONTRACT
- Findings use IDs P0-n / P1-n / P2-n / P3-n:
  P0 = active corruption, data loss, credential exposure, or false composition.
  P1 = likely destructive or unrecoverable release blocker.
  P2 = concrete correctness, recovery, compatibility, parity, or
       diagnosability defect.
  P3 = bounded robustness, coverage, maintainability, or documentation defect
       with a concrete consequence.
- Verdict is GO or NO-GO. NO-GO while any P0, P1, or P2 is open.
- Each finding: ONE root cause, exact location (file:line at the tuple),
  violated invariant, credible reproduction or state/interleaving sequence,
  impact, required correction, and a closure/regression test. Separate
  independent root causes.
- Style preferences and speculative unease are not defects. Do not pad.
  Attacks that FAILED are part of the result — record them in §2.
- If your verdict is NO-GO but every blocking finding has a bounded,
  text-or-code-fixable remedy, you may pre-commit: "I pre-commit to GO on a
  revision that resolves {IDs} as specified."
```

## Axis A role section — CODE (Fable 5.1, medium effort budget)

```text
AXIS: CODE — architecture, interfaces, call graphs, and compatibility reality.
Attack: interface contracts vs. actual call sites; ownership and visibility;
API/wire compatibility with retained readers and older writers; error paths
and hidden panic/allocation paths; whether the diff does what its DRAFT doc
claims and nothing it forbids.

EFFORT BUDGET: medium — about 90 minutes; verify by reading plus the listed
cheap commands; report only what you verified.

REVIEW AREAS (attack these for THIS object)
1. Contracts vs authority. The three contract crates (gwz-core/crates/
   copy-contract, repo-contract, family-store-contract): public types,
   errors, partial-effect and cancellation semantics vs boundaries §3 and
   architecture §3–§8; conformance suites are real behaviour tests, not
   success stubs; fakes are faithful; no gwz-core type leaks into a leaf;
   `cfg(any(test, feature = "contract-tests"))` gating (e169877) does not hide
   a conformance module from the consumers that must run it.
2. Package graph. Every crate's normal/dev/build/optional/target dependencies
   (read each Cargo.toml; `cargo tree`; `cargo metadata`) vs the boundaries §2
   allowlist; the machine inventory read by
   scripts/checks/check_local_clone_boundaries.py matches §2 exactly; the gate
   checks RESOLVED edges, renamed deps and dev closures as §6 requires; its
   negative fixtures (test_check_local_clone_boundaries.py) cover §6's list.
3. Core skeleton call graph. gwz-core/src/local_clone/*,
   src/workspace_ops/handle_local.rs, src/operation/push_event.rs variants,
   src/lib.rs and src/workspace_ops/mod.rs edits (minimal; merge/mod.rs
   untouched); ActionKind 27/28 dispatch in gwz-cli (src/globalargs/dispatch.rs
   and clirequest) and gwz-py (native/src/dispatch/local_family.rs); the
   family-merge wrapper path to the public handle_merge_with_events; the
   engine guard `validate_no_local_source_name` in workspace_ops/merge/validate.rs
   (every op; no existing behaviour changed); refusal ordering: shape
   validation → family observation → `unsupported_operation`, with dry-run
   refused before any write.
4. Transport ports. `fetch_anonymous`/`push_anonymous` in
   git/gitbackend/contract.rs and transport.rs, the delegate! lines in
   git/gitbackend.rs, and TrackingBackend in workspace_ops/tests/g01/
   tracking_backend.rs: local paths only, explicit refspecs, no credential
   helpers, no persisted remote, error mapping onto existing codes only,
   receiver-checked-out-branch observation, non-bare push refusal observed.
5. Protocol. gwz-core/protocol/gwz.taut.py allocations exactly per design §7
   (27/28, MergeRequest F(9), LocalCloneMode, LocalFamilyRequest, tag 6 held);
   generated Rust (src/protocol/generated.rs, src/cbor.rs) and Python
   (gwz-py/src/gwz/protocol/generated) byte-current; message/error catalogs
   and the gwz-py drift pin moved correctly; old binaries: an old core
   refuses 27/28 typed; retained readers unaffected; tests/protocol.rs
   projection test still meaningful.
6. Pins. Re-pins in scripts/checks/check_checked_artifact_boundaries.py carry
   dated reasons for exactly the files changed, nothing more;
   scripts/checks/run_r4bg_aggregate_gates.py remainder 1018 darwin /
   1019 linux — reproduce the census with `cargo test -p gwz-core --lib -- --list`
   and the record §3/§7.8 arithmetic.
7. Resolver. gwz-core/crates/family-model's verb-specific token resolution vs
   design §6 (lines 416–436) and the §7 CLI→message table (lines 555–575):
   every state × verb row present in its table test; merge non-ready →
   UnknownLocal-with-state, pull/push non-ready → lifecycle refusal, absent →
   git-remote fallback left to core.
8. CI. .github/workflows/checked-artifact-boundary.yml (e169877) Tier A loop
   form: all 13 packages, manifest-path form runs from the standalone
   checkout, a failure fails the job; the boundary gate job is wired.
9. Closure of F51 P2-1..P2-4: state per finding whether the skeleton supplies
   the closure evidence F51 §2 named, or what is missing.
```

## Axis B role section — STATE (Opus 5)

```text
AXIS: STATE — durable-state semantics and adversity.
Attack: state machines and restart legality; filesystem and durability
ordering; crash/kill points between every pair of writes; races and lock
scope; fail-closed direction (a defect may lose progress, never invent it);
recovery states as a closed grammar — hunt for new stuck states the current
semantics does not have.

REVIEW AREAS (attack these for THIS object)
1. Fail-closed direction of every new or changed gate. The local boundary
   gate (scripts/checks/check_local_clone_boundaries.py + inventory + its
   unittest): can it pass vacuously (empty or missing inventory, missing
   crates/ dir, renamed/aliased dep, optional/target/dev edge, reversed
   contract edge, unclassified crate)? The privacy-probe fix
   (scripts/checks/test_v1_lifecycle_privacy_probe.py copies crates/): does
   the probe still detect the privacy violations it exists for, or did the
   copy widen the probe tree in a way that masks them? The remainder pin
   (run_r4bg_aggregate_gates.py): executed vs listed counts, ignored rows, the
   darwin/linux one-row offset (record §7.8) — will a future added or removed
   test fail closed on both OSes?
2. Write-before-refuse. For every new core path (src/local_clone/*,
   workspace_ops/handle_local.rs, the family-merge wrapper): prove by reading
   and by the local_clone tests that NO filesystem effect precedes the
   refusal — no family lock file, no .gwz/ write, no index, no marker; dry-run
   refused before any write; malformed family start requests refused before
   any import; the family observation step is read-only.
3. Transport partial effects. transport.rs `fetch_anonymous`: what exists in
   the receiver after a failure between fetch and ref verification (refs
   created? temp files? FETCH_HEAD? remote persisted? origin tracking refs
   touched?); `push_anonymous` into a non-bare receiver and into a checked-out
   branch — refused before any ref update? Verify against the design §6.2
   allowance (retained import refs are accepted; persisted remotes and origin
   ref updates are not) and the TrackingBackend's recorded partial effects.
4. Lock scope and races. The skeleton must take no new lock and hold
   WorkspaceMutationGuard / V1MutationLease exactly as before
   (merge/runtime/dispatch.rs, merge/start.rs untouched?); no partial
   implementation of the family lock creates or leaves a lock file; the
   engine guard in merge/validate.rs cannot be bypassed by an op that skips
   validation.
5. Recovery grammar. Every refusal message names a state that exists; no
   refusal implies a recovery action that does not exist; nothing can write a
   `creating`/`disposing` row (no writer exists) — confirm no reachable path
   leaves new persistent state anywhere under .gwz/ or gwz.conf/.
6. Isolation evidence validity (record §5). Is the §5.2 sibling break
   genuinely outside the passing crates' declared closures, and would a break
   INSIDE a closure be detected? Does the standalone manifest-path form (no
   --locked; per-crate lock/target generated and git-ignored) open a
   resolution-drift path between the outer and standalone builds that a
   future lane could ship through unnoticed?
7. CI semantics. The Tier A job and the boundary job in
   .github/workflows/checked-artifact-boundary.yml: a failure actually fails
   the workflow (no `|| true`, no `continue-on-error`), the per-commit lane
   gate covers the six new commits, and the job runs from the checkout state
   the tests assume.
8. Evidence provenance. Record §3–§5 numbers are labelled measured with
   host/SHA/command; cold and warm separated; the skeleton crates carry none
   of their eventual heavy dependencies (git2), so the ~1 s timings are a
   floor — is that stated, and is any target presented as achieved?
```

## Report template (the reviewer's final output)

```markdown
# GwzLocalClone-LCM1.0c — {AXIS}-AXIS REVIEW

**Review object:** LCM1.0c skeleton at gwz-core 52468ca / gwz-cli 86840f6 /
gwz-py afcd5a3 with the record dev-docs/GwzLocalClone-LCM1.0c-Checkpoint.md
at gwz-dev fc8b189 — {date}
**Baseline:** {per-repo SHAs; how sources were read}
**Date:** 2026-09-05
**Axis:** {mandate}. Independent, adversarial, read-only. The other axis runs
in parallel; nothing here relies on it. Filed verbatim by the lane owner.
**Model:** {Fable 5.1 (medium) | Opus 5}

**Verdict: {GO | NO-GO}** — {counts}. {pre-commit clause if honest}

---

## 0. Evidence base
## 1. Findings
### [Px-n] {title}
## 2. Invariant analysis
## 3. Risks and next action
```

---

## Round 2 — single-axis re-verdicts (generated 2026-09-05, after LCM1.0c-rem1)

Same body, same axis sections, plus the template's re-verdict requirements
(prior-finding closure table and changed-range analysis ahead of §0). Fresh
agents carry the round-1 report of their own axis, the remediation plan
(`GwzLocalClone-LCM1.0c-RemPlan.md`, both axes' dispositions) and the diff
as inputs; this session cannot message the round-1 agents (deviation noted
in the plan §4). Axis A `Code` → Fable 5.1 (medium); Axis B `State` → Opus 5.
Reports file as `GwzLocalClone-LCM1.0c-ReviewCode-2.md` and
`GwzLocalClone-LCM1.0c-ReviewState-2.md`.

**Revised tuple.** gwz-dev `fc8b18966f24c14f422147d16f61a0dd19159670` with
these explicitly in-scope uncommitted report outputs: the amended record
`dev-docs/GwzLocalClone-LCM1.0c-Checkpoint.md` (647 lines, SHA-256
`f8b1ae743cf06b2009218b8dc1de5e4c355abeef64b88c6737af5a29bc26c0cb`), the
plan, the two round-1 reviews and this prompt; gwz-core
`a8eae4e428abe296bb105b693443ee47a9caf045` (one commit `LCM1.0c-rem1` on
`52468ca`; 18 files, +1039/−44; no pinned file); gwz-cli
`86840f67e23a9ef04312df0dd7c8f781cc476e04`; gwz-py
`afcd5a396ccaf5e64884f8f1cec2f66273ac08bb`. Reviewed delta: gwz-core
`52468ca..a8eae4e` plus the record's §1/§2.5/§3/§5.2/§6/§7.6/§8/§9 edits.

**Additional allowed commands (beyond round 1):** `git -C gwz-core diff
52468ca..a8eae4e [-- <path>]`; `cargo test -p gwz-core --lib local_clone`
(expect 16); `cargo test -p gwz-core --lib workspace_ops::merge::validate`
(7); `cargo test -p gwz-copy-contract --lib` (6); `cargo test -p
gwz-family-store-contract --lib` (4); `cargo test -p gwz-family-store --lib`;
`python3.13 -m unittest scripts/checks/test_check_local_clone_boundaries.py`
(18); `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 87207c2 HEAD`
(seven commits, ~55 s); `cargo test -p gwz-core --lib -- --list` census for
the re-pin (remainder 1022 darwin executed / 1023 linux derived). The fault
batteries and the 16-minute suites remain forbidden; lane C's executed
`fault:4 → 1022 passed` is a recorded claim (record §9), not the reviewer's run.
