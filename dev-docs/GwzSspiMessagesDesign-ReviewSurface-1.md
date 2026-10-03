# GwzSspiMessagesDesign — SURFACE-AXIS REVIEW

**Review object:** DRAFT token-limit amendment in `dev-docs/GwzSspiCallerGuide-DRAFT.md` at root `05403cea8018cf14a793977ddec2c0c7136ccc54`, dated 2026-10-03. Object ranges: root `92d6d5e..05403cea8018cf14a793977ddec2c0c7136ccc54`; gwz-sspi `12f4822..7ea900e77f272dd6e0d64f566c59fb29322f5738`.

**Baseline:** root `05403cea8018cf14a793977ddec2c0c7136ccc54`; gwz-sspi `7ea900e77f272dd6e0d64f566c59fb29322f5738`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Authoritative caller guide read with `git show 05403cea8018cf14a793977ddec2c0c7136ccc54:dev-docs/GwzSspiCallerGuide-DRAFT.md`. All three HEADs matched at start and end.

**Date:** 2026-10-03

**Axis:** Surface: cold library API walkthrough, required inputs, units, defaults, names and lifecycle completeness. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; one P3 documentation finding.

---

## 0. Evidence base

Read:

- `AGENTS_GWZ.md` for workspace instructions.
- `dev-docs/GwzSspiMessagesDesign-PromptSurface-1.md` for this review mandate.
- The complete caller guide, lines 1–113, from the settled root commit. Used `nl -ba dev-docs/GwzSspiCallerGuide-DRAFT.md` for line references; its displayed contents matched the committed source read.

Ran these inspection commands at both review boundaries:

- `git rev-parse HEAD`
- `git -C gwz-sspi rev-parse HEAD`
- `git -C gwz-core rev-parse HEAD`

Both sets returned the exact baseline tuple above.

No design, implementation, plan or current peer report was read. No CLI/help inspection was performed because this object adds a library API and no user commands. No files were modified; no builds, tests, installation, native authentication or worker execution were attempted.

## 1. Findings

### [P3-1] The Digest sizing paragraph retains a conflicting HTTP-header cap formula

**Location:** `dev-docs/GwzSspiCallerGuide-DRAFT.md`, lines 53–55, particularly line 54: “challenge/token cap is min(provider maximum, 65,536 bytes, HTTP header limit).”

**Violated invariant:** Authentication-token limits use raw token bytes and must preserve the caller’s explicit `AuthRequest.token_limit`. An encoded HTTP header allowance is not itself a raw-byte allowance. Lines 27, 36–46 and 87–90 state this correctly, but line 54 supplies a second formula that omits the explicit cap and encoding overhead.

**Reproduction:** A host has an HTTP header allowance of 8,192 bytes and constructs `TokenLimit::new(4_096)`. Its provider maximum is at least 8,192 bytes. Lines 36–46 require the effective cap to remain at most 4,096; line 54 instead calculates 8,192. Independently, taking the HTTP header allowance directly as a raw token allowance leaves no room for base64 expansion and the authentication scheme.

**Impact:** A caller reading the Digest paragraph can derive the wrong initial-challenge or token threshold, then encounter the earlier contract’s rejection or create an HTTP header exceeding its transport allowance. The explicit constructor contract and minimal walkthrough remain clear, so this is a bounded documentation defect rather than a demonstrated API correctness or compatibility blocker.

**Required correction:** Replace the formula with the intersection of the supplied raw-byte `TokenLimit` and provider maximum. Explain that `TokenLimit` already incorporates the absolute 65,536-byte ceiling and the host’s conversion from its HTTP allowance after scheme/base64 overhead.

**Closure/regression test:** Repeat the docs-only walkthrough with an HTTP allowance larger than the explicitly chosen `TokenLimit`. Every sizing statement must preserve the chosen cap, and no statement may use encoded HTTP allowance directly as a raw token cap.

## 2. Invariant analysis

The following attacks did not produce blocking findings:

- **Required input and construction:** Lines 27 and 36–38 provide `TokenLimit::new(raw_bytes: u32)` and the required `AuthRequest.token_limit: TokenLimit`. Both explicitly prohibit a default or implicit 65,536 fallback. A cold caller can find where to construct and attach the value.
- **Units and boundaries:** The constructor specifies raw authentication-token bytes and the inclusive range 1–65,536. The walkthrough explicitly rejects 0 and 65,537. Invalid values fail before start/registration. The isolated conflicting formula is recorded as P3-1.
- **Cap lifetime:** Lines 39–46 specify exact propagation into Begin, checks on initial Digest and subsequent challenges, checks before output publication, provider narrowing without enlargement, and an immutable cap throughout the conversation. Failure categories and cancellation consequences are stated.
- **Defaults:** The only documented Supervisor option has an explicit default and range: `max_workers = 8`, accepted 1–64. TokenLimit and operation/shutdown deadlines explicitly have no defaults. There is no hidden per-round timeout reset or deadline extension.
- **Cold installation and removal:** Lines 12–17 place the matching library and worker within the host application’s installation. CLI self-exec and Python bundled-worker integration are described, including absolute paths and removal/upgrades together. Runtime packaging success is deferred and was not inferred.
- **Normal lifecycle:** A caller can follow Supervisor creation, request construction, `start`, initial `step(None)`, subsequent challenges and `finish`. `finish` is legal before the first step, during negotiation and after Complete. Complete ends native token generation but does not assert server acceptance.
- **Cancellation and cleanup:** `cancel`, cancellation signals and dropping owned conversations/futures cover interrupted paths. Receipts distinguish Pending from Confirmed cleanup. `cleanup_status` provides observation, with explicit bounded tombstones and Unknown after eviction. `shutdown` closes admission and reports outstanding ownership; repeated shutdown is defined.
- **Host ownership:** The walkthrough requires an exclusively leased HTTP connection, prohibits moving a conversation to another connection, and gives cancellation paths for connection loss, redirects and route retirement.
- **Mechanism interpretation:** Negotiate permits either Kerberos or NTLM. Unsupported restrictions must be rejected before start. Provisional observations and authoritative completion are distinguished; callers are instructed not to infer mechanism identity from opaque tokens.
- **Names and placement:** Supervisor methods describe host-wide ownership, Conversation methods describe one negotiation, and TokenLimit names the supplied bound. Internal worker dispatch is identified as an integration API. No new command family or user-facing CLI setting is introduced.

## 3. Risks and next action

This GO establishes the documented library surface only. Rust caller values/codecs, production framing/state/supervision, native SSPI behavior, installed worker/build-manifest implementation and provider qualification remain deferred. No installation, runtime, remote authentication or publication success is claimed.

The next action is to correct the stale sizing formula identified in P3-1 and retain the explicit required raw-byte cap contract when carrying this surface into the subsequent implementation gates.
