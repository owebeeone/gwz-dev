# Native fix design remediation

Round 1, 2026-09-20. Code P2-1: accepted factual counterexample; disputed
requirement to harden all existing GIT_ENOTFOUND semantics in this minimal fix.
The prior text overstated what unchanged code guarantees. Correct the design
rather than silently widening the upstream-facing correction.

Disposition: narrow the propagation invariant to return codes other than existing
GIT_ENOTFOUND and new EINVALIDSPEC/EPEEL. Explicitly preserve and characterize
both a missing tag target and a parsed tag with mismatched declared target type.
Require syntax-malformed GIT_EINVALID to fail with destination unchanged. Record
type-consistency hardening as unresolved for the later fallback-removal decision.
No production activation or malformed-input safety claim follows from N1/N2.

Closure: retained Code reviewer re-traces original mismatched-type example and
verifies the text no longer promises rejection; implementation adds the passing
stock-behavior characterization and failing syntax-malformed case. State reviewer
checks the narrower guarantee/activation boundary. No budget increase, new owner,
or additional runtime behavior. Both original reports are filed verbatim.
