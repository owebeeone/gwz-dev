# GWZ core session contract — revision 5 verdict

Date: 2026-09-27. Status: **accepted as a design contract at revision 5, SHA-256 `6d12f03e52ae5969a665cc45b7c066a6715366b35685dae57cc55bbcc5857e27`, after [Consistency-5](GwzCoreSessionDesign-ReviewConsistency-5.md) and [Safety-5](GwzCoreSessionDesign-ReviewSafety-5.md) reported GO; this accepts the design contract only**. It authorizes no implementation, activation, platform proof or release.

Revision 5 changes one wire number in revision 4, which [Verdict-4](GwzCoreSessionDesign-Verdict-4.md) accepted. §13 appended `cancellation` to `TransportCapabilitiesResponse` as field 3. The candidate build's protocol projection already numbers fields 3–7 of that message, and its composition refuses a colliding tag. The session plan recorded the collision as its contract ambiguity C8. On 2026-09-27 the operator decided that `cancellation` takes field 8, leaving 3–7 where they are.
- Two fresh reviewers ran in parallel, and neither read the other's report.
- Both verified the object and the five HEADs at the start and the end, and read controlling documents at HEAD. They read the session plan from its frozen copy.
- Neither wrote a file or ran a build or test.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | GO | 0 | 0 | 0 | 2 |
| Safety | GO | 0 | 0 | 0 | 1 |

What both reviewers confirmed:
- Field 8 is free in the production schema (tags 1–2) and in the candidate projection (tags 1–7).
- A gap in field numbers is legal under Taut's validator and the gwz-core schema tooling, and every encoder sorts map keys.
- No reader or writer of a `cancellation` capability exists yet, and no text anywhere else depends on field 3.

## Blind convergence

The Safety axis's P3-1 and a Consistency residual land on the same clause. Plain `optional` is not absence-tolerant under Taut. A reader built from the new schema fails to decode a reply from a writer that lacks the field, where it should read "not supported". The flaw was carried from revision 4's wording. It had no concrete consequence in any same-build pair.

## Corrections applied after the GO

Both reviewers cleared their findings to land without a further round. The contract then hashes `40ca7f7d250d9e4c63d8eddb45ee956d6043d52ffc1c90ae493ec1264a057f7c`.

1. **Safety P3-1: absence.** §13 declares `cancellation` "optional and may be absent" (Taut `missing_ok`); absent or null means not supported, the same as false. §5.8 says the same. §15.2 gains the closure test: the field round-trips at tag 8, a reader without the field ignores it, and a reply without key 8 or with it null decodes as unsupported on both projections.
2. **Consistency P3-1: the authority for 3–7.** §13 cites gwz-core's accepted placement design §3, which reserves tags 3–7 for the client-placement capability fields, and names them. The candidate projection is cited as the file that carries them, not as the authority.
3. **Consistency P3-2: the record.** The revision-history paragraph names revision 5 and this verdict. The status names the commit that holds revision 4. §15.2 names the test that proves the wire number.

## Recorded

- **The candidate's pins are both behind HEAD.** `candidate-generator.json` pins the core schema at gwz-core `54618449` (`551fe930…`) and the gwz-transport owner schema at `10179189…`. The regenerator refuses both at today's HEADs. CS1.1 re-pins both deliberately when it regenerates, and its harness check should run in CI or `run_tests.py`. The session plan's [Verdict-1](GwzCoreSessionPlan-Verdict-1.md) records the core pin; this adds the owner pin.
- **C1 stays open.** Revision 5 does not state what `cancellation` means for native-path remotes and routes in a transport build. The session plan's C1 assigns that to TR1.5's, TR1.6's and S7.5's reviews.
- **The DRAFT flip names revision 5.** The paired GWZDesign and GWZRequirements paragraphs flip to the accepted contract before CS1.1. That is now revision 5, not the revision 4 that the session plan's G0 names. The paragraphs carry no field number, so their content is unaffected.

## Next action

The paired paragraphs flip to revision 5 together with the reuse design's "On GO" edits. CS1.1 starts once TR3.1 lands.
