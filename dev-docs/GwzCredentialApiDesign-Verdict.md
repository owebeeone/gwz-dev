# Credential + Connection joint API design: merged verdict

Date: 2026-10-19. **GO — design/interface composition only.**

## Reviewed object

Root: `c0fe3cf8185087a61780b8a61e347c95efc83c4f`. Credential revision 1, together with the unchanged accepted
Connection contract and the round-1 remediation plan. The full member tuple is
recorded identically in both final reports and the generated review prompts.

Credential Git blob: `37a4c84bbd0eb37c860f61c8dc1216c18f13a3ee`.
Reviewed Credential SHA-256: `f729ed2b8298782d1648a5da0a2085810acf32ee92aff7361192a87823d28a5d`.

The acceptance commit changes only the status header/links in the reviewed
Credential document, plus review metadata/reports. Its substantive body is
identical to the reviewed object. No source member head changed.

## Independent verdicts and closure

| Axis | Initial result | Round 1 result | Open findings |
| --- | --- | --- | --- |
| Consistency, including Surface | NO-GO, two P2 | [GO](GwzCredentialApiDesign-ReviewConsistency-1.md) | 0 P0–P3 |
| Safety | NO-GO, two P2 | [GO](GwzCredentialApiDesign-ReviewSafety-1.md) | 0 P0–P3 |

Both initial reports independently identified the same two architectural roots:
terminal/retained-resource ownership, and operation/provider composition. Safety
also traced the explicit-file-key handoff within the second root. All four
findings closed through one merged correction and independent counterexample
retraces; no executed implementation tests are claimed.

One of two remediation rounds used. No third architectural root was identified.
The same two prior reviewers were reused under the operator's instruction; they
were blind to current-round peer reports. No model-family diversity is claimed.
All reports are filed verbatim; generated prompts and tuples are preserved in
the [review package](GwzCredentialApiDesign-ReviewPackage.md).

## What is accepted

- A separate proposed Credential library/repository, with no dependency back into
  core, Git, Connection or transport; optional Windows delegation to gwz-sspi.
- One captured operation derives multiple independently cancellable conversations.
- Explicit stage/drop/shutdown semantics, bounded accounting and honest native
  cleanup/physical settlement through the existing retention service.
- Endpoint-local agent signing and the existing explicit-key memory-auth route,
  with scoped key use and pin lifetime instead of arbitrary key export.
- A small core composition bridge consuming both libraries, with original caller,
  origin/generation and protocol security scope kept explicit.
- Portable Credential tests, fake-credential Connection tests, provider checks
  and focused composition/actual-placement qualification as separate layers.

The accepted Connection contract is unchanged. Credential §8 maps the precise
composition refinements to its existing clauses; no dependency cycle, wire,
ordering, endpoint-route or cleanup-domain amendment is selected by this review.
This is not acceptance of arbitrary native adapters or a general identity service.

## Limits and next bounded action

There is no new repository, crate, source implementation, dependency change,
platform experiment, publication, tag, push or release from this package.
Actual test-cost reduction, endpoint credential placement, process launch safety,
provider cleanup and installed-client integration still require execution proof.

After separate implementation authorization: create the selected member and
prove the portable lifecycle/multiple-conversation/key-port cases with fakes;
keep core, Git, Connection runtime and live native I/O out of that fast suite.
Then run the small Credential/Connection composition proof and affected provider
checks. No historical lane or broader release programme restarts implicitly.

Unrelated anti-pattern/Git/convergence drafts and earlier archive work were left
uncommitted and untouched by this review package. Existing oversized-document
debt is not declared resolved; new package documents meet the per-file cap and
the root active set remains below 75.
