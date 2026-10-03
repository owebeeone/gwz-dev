# SSPI taut messages — CONSISTENCY-AXIS REVIEW

**Review object:** DRAFT remediation 1 of `dev-docs/GwzSspiMessagesDesign.md` at root `05403cea8018cf14a793977ddec2c0c7136ccc54`, dated 2026-10-03; private taut schema, semantics, tooling and bounded TokenLimit amendment.
**Baseline:** root `05403cea8018cf14a793977ddec2c0c7136ccc54`; gwz-sspi `7ea900e77f272dd6e0d64f566c59fb29322f5738`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Sources read with exact-SHA `git show` and remediation-range diffs.
**Date:** 2026-10-03
**Axis:** Internal consistency and agreement with the controlling contract graph. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — P2-1 closed; no open P0, P1, P2 or P3 findings. This is the Consistency verdict, not aggregate acceptance or production implementation approval.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 | Declare required `AuthRequest.token_limit: TokenLimit`, exact transfer to Begin, enforcement boundaries and explicit API amendment. | Retraced the original caller sequence through guide lines 27, 36–46 and 87–92, design §8, and WireProtocol lines 73–84. The caller now supplies the previously missing input. Executed distinct-cap transfer and boundary models successfully. | CLOSED |

## Changed-range analysis

Reviewed root `de941b21b0b0e694ae529a817eb75b128f292a98..05403cea8018cf14a793977ddec2c0c7136ccc54` and member `8379af881c04bd59e1e470c853e828a0037f7944..7ea900e77f272dd6e0d64f566c59fb29322f5738`.

The correction adds the checked caller constructor and required request field, defines immutable raw-byte transfer and terminal error boundaries, and explicitly supplements accepted design §§4 and 6 through a bounded §8 amendment. The checkpoint removes its previous no-API-change claim and requires a separate revised Surface verdict. Member changes add the same semantics, two synthetic tests and their model, regenerated semantic/contract hashes, and accurate testing/status notes.

Schema fields, tags, types and exported IR are unchanged. No production Rust implementation, dependency, clock, retry or cleanup change appeared in the remediation. All substantive changes address P2-1 or its evidence/authority trail. **No new architectural root cause identified**; cumulative count remains one.

## 0. Evidence base

Read the remediation prompt and committed merged remediation plan; revised caller guide, design §8, message checkpoint and current-checkpoint entry; changed member WireProtocol, testing policy, implementation status, fingerprint manifest, `cap_contract_model.py` and schema tests. Rechecked these against the controlling graph read in the initial review, including accepted design §§2–7, plan step 1 and core Windows parity §8. No current peer report was read.

Executed the two authorized checks using the external schema-tools interpreter and `-B`:

- Regeneration `--check`: exit 0; two artifacts verified.
- Schema unittest discovery: exit 0; twelve tests passed in 0.004 seconds.

All three HEADs matched the prescribed tuple at start and end. No writes, builds or Git mutations occurred.

## 2. Invariant analysis

The original counterexample now has a defined input path: the host derives the HTTP allowance, constructs `TokenLimit`, places it in `AuthRequest`, and the parent copies its immutable numeric value exactly into Begin. Otherwise identical requests with caps 64 and 128 retain those respective values through reference encoding/decoding. Missing model input, invalid 0/65,537 values, cap+1 payloads and a narrower provider maximum are exercised.

The constructor has explicit units, range and no default. Caller input, provider overproduction and oversized received output have specified error classes; all preserve terminal cleanup ownership. HTTP policy remains with the host, and provider bounds can only narrow the cap.

The amendment names its affected accepted clauses and does not inherit earlier Surface approval. Semantic fingerprint regeneration reflects the document change while retaining the unchanged schema digest.

The initial review’s remaining invariant analysis continues to hold: Digest first input, identity-before-Begin, binding representation, mechanism observation, ordered rounds, serialized Finish, terminal publication and cleanup proof are unaffected. The models are expressly distinguished from production admission, zeroization and native evidence.

## 3. Risks and next action

Production codec/API enforcement, installed artifact matching and native qualification remain deferred. The passing models establish the reviewed contract’s shape, not future implementation correctness.

Next action: merge the independent Consistency, Safety and revised Surface verdicts on this exact tuple before recording scoped acceptance.
