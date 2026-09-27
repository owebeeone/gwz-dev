# GWZ core session contract — revision 4 verdict

Date: 2026-09-27. Status: **accepted as a design contract at revision 4 after [Consistency-4](GwzCoreSessionDesign-ReviewConsistency-4.md) and [Safety-4](GwzCoreSessionDesign-ReviewSafety-4.md) reported GO; this accepts the design contract only**. It authorizes no implementation, activation, platform proof or release. It closes step TR1.1 of the [transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md).

Revision 4 applied [RemPlan-2](GwzCoreSessionDesign-RemPlan-2.md) as one patch to revision 3 (root `5a5d6cf`).
- **Operator decisions.** On 2026-09-27 the operator decided RemPlan-2 §4 as it recommended: apply it; include all ten nonblocking P3 corrections; set the initial `reconciled_commit` to `46e65a9a888fbd4a5bbeace946996581dcf23333`.
- **How it was reviewed.** Revision 4 was applied in the working tree, uncommitted. It was reviewed as sixteen files identified by SHA-256 against root `9a65306`, gwz-core `b13bbadb`, gwz-py `4ad2b07` and gwz-transport `a7a36ae`.
- **The reviewers.** The same two continued with their context intact. Each verified all sixteen digests at the start and the end. Neither read the other's round-4 report. Each ran the named unit tests and the process-global checker.

| Axis | Verdict | Prior findings | New |
| --- | --- | --- | --- |
| Consistency | GO | P3-28, P3-29, P3-30, P3-32 and P3-33 closed; P3-31 closed as B11 | P3-34 |
| Safety | GO | P2-9 (B11) closed, with all three parts of its required correction; P3-28 to P3-31 closed | P3-32 |

B11, the one blocking root of [Verdict-3](GwzCoreSessionDesign-Verdict-3.md), is closed on both axes:
- `run_tests.py` fails closed;
- gwz-core's boundary job checks gwz-transport at the pinned commit and cannot skip it;
- the text states both, with the bump rule.

Both reviewers judged the implementer's five changes beyond RemPlan-2 sound. No finding in this pass is architectural, so RemPlan-2 §5's stop was not triggered. This was the fourth review pass, confined to non-architectural corrections, as GwzProcessOptimization §4.1 permits.

## The object

These are the SHA-256 digests reviewed and accepted, before the corrections below.

| SHA-256 | File |
| --- | --- |
| `007c948a74cecab72cb36d86c3e2df15ce57c829bbba54e5614ef8b7dd2d63da` | `dev-docs/GwzCoreSessionDesign.md` |
| `20baf1cfaeeaf6eff7baedff0ddb4dfb18a9bb128afb83d0aa678878e947733d` | `dev-docs/GwzCoreSessionDesign-RemPlan-2.md` |
| `e680a78a2c53232508d53d3b7739b389b6324345facfca77baaa3642989d8250` | `gwz-core/scripts/run_tests.py` |
| `174f53e43af601b99ed9c7087c7bf99c25fdf708b040d4e6da2d23b63a605303` | `gwz-core/scripts/release.py` |
| `db02057203e966a6484b76ce19b986f0c2f70f1d6b26a7a7fb20e9c4b4e4a440` | `gwz-core/scripts/test_release_bump.py` |
| `f7cca2548aa72f8982af9f290937beca31aa1ac3daf2508037ae1b34e9e80f17` | `gwz-core/scripts/checks/check_process_globals.py` |
| `c313f080eedc098d49f18136e40c772fad6f923de9e2ea721d163c8cde4383c2` | `gwz-core/scripts/checks/process_globals_allowlist_gwz_transport.json` |
| `2e12ddc4a4d49bcf98731983497cda10f10c9b2e72067dd285f251a74fdd44ea` | `gwz-core/scripts/checks/test_check_process_globals.py` |
| `a737cc2fdfd3900c73649a17e59cb93dff3fbe1021f1baef620cdf674c9fcb85` | `gwz-core/scripts/checks/test_release_boundary.py` |
| `f37193ca4773855f7f151648a4538debda027c06931d5cd1384ded403f6ef62d` | `gwz-core/scripts/checks/test_run_tests_transport_globals.py` |
| `0f9f31c3c54ea404eccc2ff7215cdb6589ad3de53d49b834096614572b25e434` | `gwz-core/.github/workflows/checked-artifact-boundary.yml` |
| `5feabd8341ac576ebde2bf87dd6b7c5dd799cbf28f51a2e0217217a0cec06f2d` | `gwz-core/.github/workflows/platform-matrix.yml` |
| `9408908ddb3888dfcdba33b94cdb17f5c03e2ef83124024a3b09559ebb08b69b` | `gwz-core/.github/workflows/release.yml` |
| `9e1998d4301246571d9016976938db944a26cd2587ab6201fa89c0d3cdaea3ba` | `gwz-core/.github/workflows/windows-matrix.yml` |
| `4a3b5404197558dba85e0d9500bcc7858ddab64a0a81d4b957a79a3c465c7757` | `gwz-core/dev-docs/GWZDesign.md` |
| `2c9b154e0bca704ba6142a3e028a2925533f6cdb5aa8b803e977317627e97dc9` | `gwz-core/dev-docs/GWZRequirements.md` |

## Corrections applied after the GO

Both reviewers cleared their new finding to land without a further round.
- Safety offered to fold P3-32 into the implementation plan, or to accept its recipe change in the status-only edit.
- Consistency called P3-34 "a one-clause text correction for the lane owner to fold into the commit or the implementation plan; no further Consistency round is needed".

Each was applied as specified. Two residual notes were taken with them.

1. **Safety P3-32.** §5.8's `git credential fill` recipe adds `-c core.askPass=`, an empty override that disables a configured askpass program on every git version. The text now states that git itself never prompts on any version. §15.8's recorder test also sets `core.askPass`, and runs on a git older than 2.40.
2. **Consistency P3-34.** O9, §5.6, the §5.7 row, the paired GWZDesign paragraph and the GWZRequirements O9 bullet each now say:
   - a child gets nothing from the live environment beyond the snapshot;
   - a spawn may remove prompt hooks and add prompt-disabling settings, as §5.8 states and the `gh` helper already does.

   §15.8 adds the assertion that no live-environment variable absent from the snapshot reaches a session-path child.
3. **Consistency residual.** §5.7 says a named `GWZ_TRANSPORT_CHECKOUT` that is missing fails, and is never a reason to use the sibling. That is what `run_tests.py` already does.
4. **Consistency residual.** §15.8's recorder test says "HTTP fetch", matching the line before it, so it reaches the native path in both builds.

The contract then carries the accepted status and hashes `f9199ddb706a5e92cc6c51e98bb65f40c17e4e761a58bd20e79d4b162d143a23`. GWZDesign hashes `136524d163428506e15f2148f570c9edbebe85199db45a4761f79880b8d2d785`, and GWZRequirements hashes `e5bdfcd9538a57e1078eff48a4a020b67461301f4df8bc8949090b3cdf7e22a8`. RemPlan-2 changes only its status line, which now records this acceptance; it hashes `9a5c2a6dbb0ada2365f36f426406dceb79d1d16be049aa74032fd730b9093ffa`. The other twelve files are unchanged from the reviewed digests.

## Recorded

- **The pin moves only in a gwz-core commit that changes the allowlist.** A push of gwz-transport does not move it. RemPlan-2 §4 item 3's wording ("the pin stays behind local gwz-transport until gwz-transport is pushed") is read with that rule.
- **The test baseline.** Two failures in `scripts/test_release_bump.py` and `scripts/test_publish_crates.py` reproduce on a clean export of gwz-core HEAD. They come from the half-landed crate rename, which transport release step TR3.1 finishes, and predate revision 4.
- **Not run:** the cargo test suite, clippy and the GitHub workflows themselves.
- **Residual risks carried** from the two reports:
  - `release.yml`'s verify job skips the gwz-transport leg by design;
  - a standalone gwz-core clone now needs `GWZ_TRANSPORT_CHECKOUT` for a release, and the crates.io runbook should say so;
  - the pin may move backwards on `main` without failing;
  - the ancestry check names `origin/main` literally;
  - the `SKIPPED GATE` wording assumes no checkout;
  - RemPlan-2 §3's recorded items.

## Next actions

1. **The paired paragraphs.** They are flipped from DRAFT to the accepted revision before the session plan's CS1.1, as its G0 requires.
2. **Commit.** Revision 4 and these records are uncommitted. They are committed when the operator asks.
3. **Unblocked steps.** The transport release plan's TR1.4a, the session plan's first revision, can start, and so can TR1.2's review of the [reuse design](GwzConnectionReuseDesign.md).
