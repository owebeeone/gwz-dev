# taut 0.10.0 adoption — remediation plan after round 1

Date: 2026-09-30. This merges round 1, which reviewed the bundle at manifest `ecaf0ce9…` (61 entries):
- [Consistency](GwzTaut010Adoption-ReviewConsistency.md): GO, with four P3s.
- [Safety](GwzTaut010Adoption-ReviewSafety.md): GO, with two P3s.

The merged verdict is **GO**; nothing blocks. The lane owner takes every P3 into this step instead of recording it as a follow-up:
- one is dead configuration, which the remove-dead-code rule does not let wait;
- one is a provenance hole that the text claimed was closed;
- the rest are small text corrections.

The two axes partly converge, from opposite sides, on one gap: the `decode` removal's accounting is incomplete. Consistency found the brief's "only in-tree user" claim and the undocumented new root items. Safety found a contract comment that still names the removed function.

## Dispositions

| ID | Disposition | Closure test |
|---|---|---|
| C-P3-1 | Accepted, correction (b). The brief states that the lock records taut-shape-rs at `5026715`: its `origin/main`, one CI-only commit past `v0.10.0` (`8f86a9c`), with crate source identical to the tag's. The ritual's third leg governs the taut checkout, and another session owns taut-shape-rs's position, so this step does not move it. | The brief names `5026715` and its difference from the tag. |
| C-P3-2 | Accepted. The brief's §2 and §3 name `gwz_core::MAX_DEPTH` and `gwz_core::MAX_ENCODED_LEN` as new root items, and `gwz_core::cbor`'s removed accessors and changed variants as source-level breaks. They list the three prior in-tree users of `decode`. §3.3 says `TooLarge` cannot occur while no gwz message declares a length bound. `docs/RustApi.md`'s CBOR section names the two constants and what they bound, and its stale "v0.3.0" sentence goes. | `grep -c MAX_ENCODED_LEN gwz-core/docs/RustApi.md` is at least 1, and claim 4 lists all three users. |
| C-P3-3 | Accepted (remove-dead-code rule). The `codec` key goes from all three generator pins, `forward_compat` from gwz-transport's, and the consumer generator's inert `codec` guard goes with them. | None of the three pin files has a `codec` key; the consumer's tests pass. |
| C-P3-4 | Accepted. The brief gains §6, the records this step writes at commit: the checkpoint entry (release pins replace commit pins, and the checkpoint's "pin taut's `src/` tree" idea is moot) and the plan text it invalidates for the plan's next revision (`GwzCoreSessionPlan.md` line 109, CS1.1's regeneration inputs). | The checkpoint entry filed with the commit names the release-pin regime and plan line 109. |
| S-P3-1 | Accepted, corrections (a) and (c). Each of the three generators trusts a `taut-proto` distribution only when its metadata lies in one of this interpreter's site directories (`sysconfig`'s purelib and platlib, `site.getsitepackages()`, the user site); otherwise it refuses before importing taut. The READMEs, the docstrings and the brief state exactly that rule. | A new test in each of the three suites stages a copy of taut *with* its own `taut_proto-0.10.0.dist-info` on `PYTHONPATH` and asserts the refusal; the existing shadow tests stay green. |
| S-P3-2 | Accepted. The D17 contract comment in `gwz-core/src/diff/output.rs` names `DiffOutputRecord::decode` as the reader. | No `decode(&p)` naming the removed function remains in `output.rs`. |

## Safety residuals the lane owner adopts

- **The landing order.** Commit gwz-core, gwz-py and gwz-transport first; then `gwz capture` the root lock from the observed state; then commit the root. A root commit whose lock names the members' pre-step heads never happens.
- **Checking `effective` against the defaults.** `ir_version_1` drops `effective` without comparing it to taut's defaults. That check belongs to the step that adopts taut options, and is recorded there.

## Round 2

The corrections land as one patch. Each reviewer, continued with its context, verifies that its own findings are closed on the corrected tree and checks the delta for anything new. Any P0 to P2 after round 2 goes to the operator.
