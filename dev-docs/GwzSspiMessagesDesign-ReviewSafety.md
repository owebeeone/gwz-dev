# SSPI taut messages — SAFETY-AXIS REVIEW

**Review object:** Private SSPI taut schema/design/tooling; controlling root document `dev-docs/GwzSspiMessagesDesign.md`, DRAFT dated 2026-10-03.
**Baseline:** root `de941b21b0b0e694ae529a817eb75b128f292a98`; gwz-sspi `8379af881c04bd59e1e470c853e828a0037f7944`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Committed sources read using `git show exactSHA:path`. All three HEADs matched at review start and end.
**Date:** 2026-10-03
**Axis:** Safety — attack what the contract permits to go wrong. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. This verdict covers the schema/design checkpoint only.

---

## 0. Evidence base

Read workspace/member instructions and the generated Safety prompt. Inspected root range `92d6d5e..de941b21b0b0e694ae529a817eb75b128f292a98` and member range `12f4822..8379af881c04bd59e1e470c853e828a0037f7944`.

Controlling sources read:

- Root `GwzSspiMessagesDesign.md` lines 1–88; `GwzSspiDesign.md` revision 2 §§2–7; `GwzSspiPlan.md`, particularly step 1; complete caller guide and acceptance record; current checkpoint’s SSPI status.
- Process rules’ independent-review, severity, actionable-finding and protocol-authority provisions; `GwzProcessOptimization.md`, including §§4 and 8.
- Member `docs/WireProtocol.md` lines 1–190; complete Architecture/Testing documents; `protocol/sspi.taut.py` lines 1–50; exported IR and manifests; protocol README; generator lines 1–87; schema tests lines 1–126; complete CI workflow and Cargo manifest. Inspected library and refusing worker sources.
- Core Windows parity SSPI amendment and §8, lines 267–333.

Executed only the two permitted tests, using the specified external interpreter with `-B`:

- `gwz-sspi/scripts/regen_schema.py --check`: **2 schema artifacts verified**.
- `-m unittest discover -s gwz-sspi/tests/schema -v`: **10 tests passed**, 0.003 seconds.

No builds, writes, Git mutations, native experiments or peer-report reads occurred. Archive/native results recorded by the author were not independently rerun.

## 2. Invariant analysis

**Mixed versions and identity before disclosure.** Hello is worker-first and exactly once. Its version, contract/build fingerprints and actual SID/LUID/session must match before Begin. The contract expressly separates matching fingerprints from executable trust and creation-time containment. A mismatched worker cannot obtain a degraded mode or secret-bearing Begin. The fingerprint covers exact exported IR and standalone semantics, preventing a semantic-only edit from retaining the checked digest.

**Admission and allocation.** Zero/oversized frame lengths refuse before allocation; malformed/truncated input terminates without resynchronization. Mandatory presence, explicit inactive nulls, canonical scalars, unique known keys, exactly one matching body and bounded depth close extension and type-confusion paths. Reference-codec unknown-field preservation is demonstrated by a test and explicitly prohibited in production. The future decoder must avoid ordinary intermediate value trees and validate declared lengths before secret allocation/copy.

**First input, bindings and mechanisms.** Digest’s initial challenge, method and URI are carried together in Begin and required only for Explicit Digest. They feed the first native step. Binding bytes retain the accepted certificate-digest representation; serialized native pointers are excluded. Parent HTTP limits and worker provider limits intersect before native authentication. Negotiate cannot report Digest; Complete requires authoritative selection rather than inferring mechanism from package choice or attributes.

**Ordering and terminal arbitration.** Begin occurs once; responses correlate to the pending round; repeated/skipped rounds refuse before native processing and parent publication. Continue at round eight is unpublishable. Finish is legal before Begin and between steps, while dropping a pending step cancels the whole conversation. Thus cancellation cannot insert a competing Finish into a blocked write/native step. A token written before cancellation but read afterward remains unpublishable under terminal arbitration.

**Cleanup and bounded retention.** Finished acknowledges normal disposal only. It does not establish process exit, empty Job or completed launch/I/O threads. Error, EOF and termination requests likewise cannot release capacity alone. Stalled partial frames retain charged storage and the existing slot; quarantine does not admit replacement work against that slot. Forced exit makes no physical-erasure or external-provider-cancellation claim.

**Scope and evidence.** Fixed errors exclude freeform secret/provider diagnostics. Synthetic tooling is separated from production secret codecs. CI checks use member-local sources with pinned Python tooling, while Cargo remains Python-free and publication disabled. No application envelope, worker timer, retry allowance or new configuration knob is introduced.

## 3. Risks and next action

Production admission, zeroization, terminal races, native identity/provider behavior and installed artifact matching remain unproved implementation obligations. The text preserves their separate gates and does not claim synthetic tests satisfy them.

The next action is to combine the independent verdicts on this exact tuple. Schema acceptance must leave step 1’s dual Code/State secret-boundary stop open before supervision implementation.
