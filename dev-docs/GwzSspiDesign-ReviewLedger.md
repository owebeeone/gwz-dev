# SSPI design review ledger

2026-10-03. Review object: root `377c5e29882e353e42dbe75031b175374b213f7f`,
core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`, evidence
`1beb1d204c824701ddbd033c7f89df9a3561f5e5`. DRAFT only, no product activation.

Three fresh GPT-6.1 Sol reviewers were dispatched on independent Consistency,
Safety and Surface axes. The canonical prompts are filed alongside the object.
A common supplemental sentence mistakenly carried Surface-only language into
the Consistency/Safety prompts. Before either full report arrived, the lane owner
corrected those two prompts and messaged both reviewers to read the full graph
on their assigned axis at the unchanged tuple. The initial prompts are preserved
with `-Initial` suffixes. No peer findings were supplied in the correction. The
Surface prompt/scope was unchanged. This is a brief correction, not a remediation
round or evidence of a completed review.

Validation before dispatch: new package local links resolve; core merge/local-
clone document guards pass; archive verifier (including staged bytes) passes;
549 imported raw evidence files match original bytes and modes. Production code
is unchanged, so prior macOS product checks were not rerun.

Round 1: Consistency and full Safety NO-GO on the same Digest initial-challenge
omission. Surface NO-GO on mechanism result contract; its subsequent scope
clarification permits explicit permissive Negotiate semantics without adding a
policy knob. Full Safety withdrew its two initial caller-only findings after
reading the graph. Those initial findings are not silently converted into new
requirements. All raw reports/clarification are filed verbatim.

Merged remediation 1 addresses both blockers and the four P3 wording/gate issues
in one document patch. Publication/containment/clock architecture remains as
reviewed; the changed Digest/Token contract gets explicit focused retracing.
Original reviewers continue under the operator's preference to use old reviewers.
No design acceptance yet; full Windows parity remains NO-GO.
