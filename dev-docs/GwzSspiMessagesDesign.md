# SSPI taut messages — schema/design checkpoint

2026-10-03. **DRAFT remediation 1, pending Consistency/Safety and Surface GO.** Operator directive:
create taut messages, write their design and perform review-loop review.
This is a private wire-format freeze, so dual review is mandatory. Initial review
found the missing HTTP-cap input bridge. Remediation 1 adds a required typed caller
TokenLimit; the revised caller guide needs Surface review on this amendment.
The earlier Surface verdict covers the unchanged baseline only.

## Object and authority

The standalone design is [gwz-sspi/docs/WireProtocol.md](../gwz-sspi/docs/WireProtocol.md).
It and [sspi.taut.py](../gwz-sspi/protocol/sspi.taut.py) define the proposed v1
contract. Exported IR, semantic fingerprint manifest and generator pin are in
that member's protocol directory. The manifest covers exact schema IR AND wire
semantics, so semantic changes cannot silently retain a matching fingerprint.
The root record carries acceptance status separately from those hashed bytes.

Controlling graph: [SSPI design revision 2](GwzSspiDesign.md) §§2–7,
[implementation plan](GwzSspiPlan.md) step 1,
[caller guide](GwzSspiCallerGuide-DRAFT.md),
[accepted mechanism](GwzSspiAcceptance.md), and the member's architecture/testing
policy. Existing Windows parity §8 binding representation and SSPI requirements
are preserved. No whole-Windows acceptance or endpoint activation is implied.

The seven bodies are Hello, Begin, Challenge, Token, Finish, Finished and fixed
Error. Hello checks actual primary identity and exact contract/build before Begin.
Begin includes zeroization-sensitive identity/binding and complete Digest input.
Begin.token_limit carries the existing raw HTTP token cap; it is not a new knob.
Round correlation detects repeated/skipped responses on ordered private pipes.
One contained conversation has one outstanding command; dropping a pending step
cancels the conversation rather than inserting a concurrent Finish.
Finished acknowledges disposal but never proves held-handle exit/Job/thread cleanup.
No cancellation message, worker timer, network carrier, multiplexing or retry.

## This gate's limits

This package authors/exports schema and specifies codec rules. It DOES NOT
implement Rust secret types, caller API, codecs, framing, state machine,
supervision or Windows SSPI. No ordinary reference Rust binding is generated:
its Clone/Debug and nonzeroizing string/byte owners violate the accepted design.
Reference Python codecs see synthetic fixtures only. The production codec must
be IR-driven, zeroizing and stricter than reference extension preservation.

Schema/design GO leaves the existing step-1 dual Code/State secret-boundary stop
open. The next coherent implementation is caller secret values + IR-driven codec
with malformed/frame/state/admission/zeroization tests. No supervision before
that review, and no real credentials in the tooling/tests in this checkpoint.
The optional worker remains a fixed refusing scaffold, Cargo publish=false,
no new Rust dependency, remote, push, tag, release or application envelope change.

## Verification and provenance

Local 2026-10-03: ten synthetic schema/tooling tests pass (0.003 s warm), exact
regeneration --check passes, existing real worker-refusal test passes, strict
Clippy/fmt pass, and cargo package --locked --offline --allow-dirty verifies the
31-file archive, including schema, IR, fingerprint, public design and tests.
No remote CI or Windows native execution is claimed. CI now selects Python 3.12
and pinned taut-proto==0.10.0 for schema drift/tests, independently of Cargo.

TDD tooling started RED with missing regen_schema module, then GREEN after its
implementation. An initial uv overlay was correctly refused by release-location
validation; a dedicated external venv passes. No global Python installation
changed. A local Cargo invocation initially used the ignored member target;
that output was immediately relocated, retaining its contents outside the repo.
Tools/builds now live under /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/
(schema-tools and schema-validation-target). No loose parent artifacts created.
No campaign/native experiment ran, so no new private evidence is needed.

Reproduction from the member: install taut-proto==0.10.0 into a dedicated Python
environment, `python -B scripts/regen_schema.py --check`, then
`python -B -m unittest discover -s tests/schema -v`.
Existing Rust commands are in README; set external CARGO_TARGET_DIR in gwz-dev.
Public CI and tests depend on no private evidence or sibling checkout.

## Review ledger

Pending exact root/member tuple after settlement. Review prompts are generated
from the canonical review-loop template. Axes: Consistency (schema/design/graph
agreement, reproducibility, scope) and Safety (secret handling permitted by text,
closed framing, phase/round legality, terminal and cleanup invariants).
The required TokenLimit caller field/constructor is a bounded API amendment;
Surface now reviews its guide independently. Private IPC shapes are reviewed on
both axes. Full report outputs and prompt files will be explicitly permitted
review-time noise. Unrelated root SSH prompts/route draft, core bug report and
old evidence timeout run remain out of scope and untouched.

Metrics: initial review completed; remediation 1 pending; implementation-contact tooling
failures above do not constitute discovered protocol defects; escaped defects 0.
Initial: Safety GO, Consistency P2-1 NO-GO; one architectural root cause, no
blind convergence. The [merged remediation](GwzSspiMessagesDesign-RemPlan-1.md)
adds the missing caller cap and executable synthetic input-to-wire/bound models.
The corrected suite passes 12 tests; schema tags/IR remain unchanged, while the
semantic fingerprint is regenerated. These models are not production proof.
Next: obtain Consistency/Safety re-verdicts and added Surface GO, up to
two rounds; accept only the same exact tuple with both GO. Preserve reports verbatim.
