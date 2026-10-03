# SSPI taut message checkpoint acceptance

2026-10-03. **Accepted at the exact tuple below after all three review files
reported GO; this accepts private schema/design and the required TokenLimit
caller amendment only.** No production secret-codec, supervision, native SSPI,
host integration, publication or Windows qualification acceptance.

| Repository | Reviewed commit |
|---|---|
| root | 05403cea8018cf14a793977ddec2c0c7136ccc54 |
| gwz-sspi | 7ea900e77f272dd6e0d64f566c59fb29322f5738 |
| gwz-core, unchanged reference | 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31 |

- [Consistency GO / P2-1 closed](GwzSspiMessagesDesign-ReviewConsistency-1.md).
- [Safety GO / no findings](GwzSspiMessagesDesign-ReviewSafety-1.md).
- [Surface GO / P3-1](GwzSspiMessagesDesign-ReviewSurface-1.md).

Initial [Consistency NO-GO](GwzSspiMessagesDesign-ReviewConsistency.md) and
[Safety GO](GwzSspiMessagesDesign-ReviewSafety.md) are preserved verbatim.
One [merged remediation](GwzSspiMessagesDesign-RemPlan-1.md) added the missing
caller-to-wire raw cap bridge, exact transfer and executable synthetic models.
Original reviewers continued under the operator's standing preference; Surface
used a fresh isolated context and read the caller guide only. Canonical generated
prompts, including remediation prompts, are filed alongside the reports.
No blind convergence; one reviewer-classified architectural root cause; one
remediation round, zero escaped defects. No P0/P1/P2 remains open.

Surface P3-1 was nonblocking: one inherited caller sentence used encoded HTTP
allowance directly instead of the required supplied raw cap. Acceptance replaces
it with min(provider maximum, supplied raw-byte TokenLimit), explaining existing
ceiling and encoding overhead. For its counterexample HTTP allowance=8192,
TokenLimit=4096, provider≥8192, every current guide statement now preserves 4096.
This is a docs-only P3 correction, not another remediation package or changed
wire/profile. No unreviewed runtime behavior is accepted. Acceptance commits also
record status/ledger and member status only; reviewed schema/IR/semantics bytes
and fingerprint remain unchanged. The WireProtocol.md captured DRAFT banner is
historical and intentionally not rewritten inside its fingerprinted bytes.

## Artifacts and checks

[Schema](../gwz-sspi/protocol/sspi.taut.py),
[standalone wire design](../gwz-sspi/docs/WireProtocol.md),
[exported IR](../gwz-sspi/protocol/sspi.ir.json),
[fingerprint manifest](../gwz-sspi/protocol/contract.json),
[root design checkpoint](GwzSspiMessagesDesign.md).

- Seven bodies: Hello, Begin, Challenge, Token, Finish, Finished, Error.
- Hello matches actual identity, build and exact schema+semantics before secrets.
- Ordered single-conversation private IPC, immutable parent deadline, fixed errors,
  complete Digest input, authoritative mechanism completion, raw cap narrowing.
- Finished/EOF/kill never independently release owned process/Job/thread capacity.
- Zeroizing IR projection is required; reference bindings see synthetic data only.
- Twelve pinned taut-proto 0.10.0 schema/model tests and exact regeneration pass
  both in the member and extracted 32-file Cargo archive. Final cargo package
  --locked --offline verifies compilation. Earlier unchanged scaffold test,
  strict Clippy and fmt pass. Python 3.12 schema CI authored, not run remotely.
- Cargo remains dependency-free/Python-free; publication disabled. No native
  campaign ran or real credentials entered any fixture/tooling.

Next: required caller secret values and IR-driven zeroizing framing/codec with
fake-port admission/state/partial-I/O/zeroization tests, then step-1 dual Code/State
secret review before supervision. Subsequent native and host gates remain in
GwzSspiPlan.md. Full Windows remains NO-GO; no push/tag/release or activation.
