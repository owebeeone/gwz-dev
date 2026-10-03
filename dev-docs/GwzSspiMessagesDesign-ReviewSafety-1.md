# SSPI taut messages — SAFETY-AXIS REVIEW

**Review object:** Private SSPI taut schema/design/tooling, remediation 1; controlling `dev-docs/GwzSspiMessagesDesign.md`, DRAFT dated 2026-10-03.
**Baseline:** root `05403cea8018cf14a793977ddec2c0c7136ccc54`; gwz-sspi `7ea900e77f272dd6e0d64f566c59fb29322f5738`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Committed sources read using `git show exactSHA:path`; all three HEADs matched at start and end.
**Date:** 2026-10-03
**Axis:** Safety — attack what the contract permits to go wrong. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. This accepts the corrected schema/design shape on the Safety axis only.

---

## 0. Evidence base

Continued the original review using the generated remediation prompt, merged `GwzSspiMessagesDesign-RemPlan-1.md` and legitimate prior-round reports.

Inspected complete remediation diffs:

- Root `de941b21b0b0e694ae529a817eb75b128f292a98..05403cea8018cf14a793977ddec2c0c7136ccc54`.
- Member `8379af881c04bd59e1e470c853e828a0037f7944..7ea900e77f272dd6e0d64f566c59fb29322f5738`.

Read corrected caller guide lines 1–113, particularly 27–46 and 87–92; SSPI design §§4–8, particularly amendment lines 284–297; plan step 1; checkpoint and remediation status. Read corrected member WireProtocol lines 1–204, cap model lines 1–37, tests lines 111–138, semantic manifest and Testing/Implementation changes. Prior evidence covers unchanged schema, generator, CI, architecture, core §8 and process authority.

Executed the two authorized checks with the specified external interpreter and `-B`:

- Regeneration `--check`: **2 schema artifacts verified**.
- Schema unittest discovery: **12 tests passed**, 0.003 seconds.

No writes, builds, Git mutations, native experiments or current peer-report reads occurred.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Prior Safety findings: none | Preserve established safety boundaries while adding the cap carrier | Retraced cap transfer, refusal, deadline, terminal-publication and secret-boundary requirements | No Safety findings outstanding |
| Prior Consistency P2-1 | Required checked TokenLimit in AuthRequest; exact Begin transfer; explicit API amendment and separate Surface gate | Original counterexample retraced: otherwise identical requests now supply 64 and 128 through the declared caller field; executed model/reference round trips preserve the corresponding Begin values. Missing cap and invalid ranges refuse | Corrected in the reviewed design/model; original Consistency reviewer’s closure remains separate |

## Changed-range analysis

The semantic change is confined to the missing caller-to-wire cap bridge. Root documents declare `TokenLimit::new(raw_bytes: u32)`, required ownership, range, no default, immutable transfer and error boundaries. Design §8 explicitly supersedes the omitted carrier; previous Surface GO is not grandfathered onto this amendment.

Member changes add matching semantics, two synthetic model tests and evidence-limit wording. DSL/tags/exported IR remain unchanged; semantic and contract fingerprints change. Regeneration verifies those bytes.

Remaining changes record prior reviews, remediation status and the updated member pin. No runtime code, process architecture, native implementation, dependency, clock, retry or application protocol change appears. No **new architectural root cause** was established beyond the prior missing-input bridge.

## 2. Invariant analysis

**Cap ownership and narrowing.** A host can no longer follow the declared API while omitting the HTTP-owned raw-token allowance. There is no implicit 65,536 fallback. The parent retains the immutable value and copies it exactly into Begin. Worker provider metadata can narrow the allowance before credential/context initialization; it cannot enlarge it.

**Boundary failures.** Tests execute invalid 0/65,537, negative and bool cases; cap−1/cap/cap+1 at 1, 64 and 65,536; and a narrower provider maximum. Caller excess is InvalidRequest, provider overproduction ProviderRejected, and oversized received Token Protocol before publication. The model is explicitly limited to lengths and transfer; it does not replace nonempty-challenge, package, framing or production-secret admission.

**Cancellation and retained ownership.** Cap failures are terminal and inherit existing cleanup rules. They introduce neither a fresh allowance nor recovery/retry. The original immutable deadline still bounds launch, steps and Finish. Late output cannot override terminal arbitration; Error/EOF/Finished cannot independently release a live or quarantined slot.

**Disclosure and compatibility.** Hello identity/fingerprint checks still precede Begin. Changed semantics produce a changed contract fingerprint, with no degraded compatibility path. Secret storage, partial decode/encode wiping and strict production admission retain the mandatory dual Code/State stop. Synthetic tests grant no native or zeroization assurance.

## 3. Risks and next action

Production cap enforcement, secret codecs, native behavior and installed artifact matching remain subsequent gates.

Next: combine the exact-tuple Consistency/Safety re-verdicts with the separately required revised Surface verdict before accepting this amendment. Keep the codec/API secret-boundary stop open before supervision work.
