# SSPI caller values and secret codec implementation checkpoint

2026-10-03. **DRAFT implementation checkpoint**, not a new process design.
Accepted authority: GwzSspiMessagesAcceptance.md (schema/design and TokenLimit),
GwzSspiDesign.md revision 2 plus §8, GwzSspiPlan.md step 1, caller guide, and
member docs/WireProtocol.md exact fingerprinted bytes. Secret handling and wire
format mandate dual Code/State review before supervision. Caller documentation
must describe implemented values without claiming a working Supervisor.

## Scope

Implement owned SecretBytes/SecretText, checked TokenLimit, Package/Identity/
AuthRequest/Digest and token/error values needed by the accepted caller contract.
Do not expose private IPC messages publicly or add success-shaped Supervisor,
Conversation, worker, native API or process stubs. A byte/string constructor may
copy a borrowed source only with explicit documentation that the caller still
owns and must wipe its original; no ordinary temporary owned secret copy.
No Clone/Debug/serde on secret-bearing types. Native UTF-16 secret storage, actual
provider memory and live process cleanup remain step 3, not claimed in this gate.

Generate private typed wire records, enum tags and direct encode/decode walks from
the exported taut IR, not a handwritten second schema. A generated borrowed
projection tied to held zeroizing input permits allocation-free profile admission
before materializing any owned secret fields. No generic CBOR value tree. The
handwritten semantic layer owns bounds, package/body/identity/mechanism relations;
field tags/types remain generated. Decode rejects closed/canonical profile drift.
Encode validates the same profile and computes size before any secret write.
Framing uses bounded owned zeroizing memory and exact partial-I/O accounting;
no growing queue. Pure state/round admission may be tested here, but no new native
record or runtime registration is implemented.

Fixed-size allocated secret owners must allocate/initialize before copying any
secret, wipe all owned initialized storage on Drop/error, and never grow/reallocate
with live payload. Every frame remains owned until its reader/writer work completes.
No mutable global/thread-local test observer. Test probes own per-test audit state
and inspect zeroization before deallocation, never read freed memory. Synthetic
fixtures only. Seeded chunk/random tests print seed/case/trace on failure.

## Dependency review before addition

Lane-owner explicit source review completed before implementation: zeroize 1.9.0
local crates.io source Cargo.toml and src/lib.rs (Box<[Z]> Zeroize, Zeroizing Drop,
volatile zeroization/compiler fence implementation). License Apache-2.0 OR MIT,
MSRV 1.85 ≤ package 1.95. Default features disabled; alloc only; no derive/serde/
std feature, no transitive crate, no runtime callbacks. Approved sole dependency
for owned wiping: zeroize = '=1.9.0', default-features=false, features=['alloc'].
This is a scoped addition to member docs/Architecture.md's empty scaffold allowlist;
final independent reviewers must attack storage/reallocation/error paths and this
choice. Do not substitute new cryptographic or CBOR/runtime dependencies.
Zeroization is normal owned-buffer cleanup, not physical erasure/LSASS cancellation.

## Gates and settlement

TDD: rejected input/traits/storage + generated wire round trips and malformed
cases, then implementation. Golden synthetic CBOR from pinned reference taut
must match actual Rust output; reference is tooling only, never production secrets.
Bounds±1, missing/duplicate/unknown/noncanonical/type/depth/trailing bodies, numeric
limits, identity/Digest/CBT/mechanism, exact cap transfer, frames split at arbitrary
boundaries, cancellation/drop and safe wipe observation are required. Native
provider caps use a supplied checked number in fake admission; no provider lookup
or authentication is faked as successful. Actual native qualification remains open.

Run standalone tests, strict Clippy/fmt, pinned regeneration, generated drift and
crate archive tests with external target caches. Record exact root/member/core
reference tuple, preserve unrelated files, generate canonical Code/State prompts,
commit intended checkpoint via GWZ, obtain dual peer-blind verdicts. Review loop
and two-remediation cap apply. Surface reviews any new public value ergonomics
through caller docs only; prior approved signatures cannot silently change.
No actual credentials, remote, push, tag, release, application envelope or endpoint
activation. Next after GO is the supervision kernel under the existing plan.

## Implemented checkpoint and local verification

The implementation supplies all listed caller values and private generated
records. Schema, IR, fingerprinted WireProtocol bytes and contract fingerprint
remain unchanged. Generation now pins secret-rust-v1 and Rust 1.95 rustfmt;
16 artifacts include per-message Rust modules and fourteen synthetic reference
vectors. Python is generation tooling only; Cargo is standalone and Python-free.
The pure codec checks supplied expectations and profile relations. Conversation
phase, allowed message kind by phase, Error.phase and terminal arbitration are
supervisor obligations, not evidence established here. Native UTF-16/provider
storage and process disposal remain explicitly deferred.

Owner verification on 2026-10-03 passed 29 Rust unit tests, caller integration,
unchanged worker refusal, twelve negative Clone/Debug doctests, strict all-target
Clippy, fmt including four include!-owned test modules, pinned regeneration and
thirteen Python tests. The syntax-aware cfg-boundary check inspected every member
Rust file, including disabled test branches, with zero findings. Seeded framing
uses 128 schedules for each of fourteen independently generated fixtures and
prints seed/case/fixture/chunk trace on failure. Wipe audit observes live initialized
storage after zeroizing and before deallocation, not freed memory. Remote CI is
authored but unexecuted; no Windows runtime qualification is implied.

Review tier is mandatory dual Code/State for this secret/wire boundary, with an
additional cold Surface review of the new owned-value constructors/documentation.
Acceptance and exact settled tuple are recorded separately after verdicts.

Drafter recorded actual RED runs for missing caller exports and missing codec
modules/types before implementation, followed by passing GREEN runs. Standalone
Cargo package verification and all 43 Rust checks passed from the extracted
61-file crate archive; owner inspected packaged caller/code/tooling/status paths.
Handwritten source/tests are 2,427 lines across thirteen cohesive files (largest
444), generated Rust 953 across thirteen files, vectors 123, generator tools 244.
This is one cohesive secret-boundary checkpoint, larger than the aspirational
500-line chunk so that admission, storage, framing and conformance share one gate.
