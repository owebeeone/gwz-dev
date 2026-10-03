# SSPI taut messages — CONSISTENCY-AXIS REVIEW

**Review object:** DRAFT `dev-docs/GwzSspiMessagesDesign.md` at root `de941b21b0b0e694ae529a817eb75b128f292a98`, dated 2026-10-03; private taut schema, semantics and tooling.
**Baseline:** root `de941b21b0b0e694ae529a817eb75b128f292a98`; gwz-sspi `8379af881c04bd59e1e470c853e828a0037f7944`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Committed sources read with `git show` at these SHAs.
**Date:** 2026-10-03
**Axis:** Internal consistency and agreement with the controlling contract graph. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; no P0, P1 or P3 findings. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

Read:

- Workspace instructions, member `AGENTS.md`, generated Consistency prompt, review-loop skill and canonical prompt template.
- Root message checkpoint, lines 1–88; accepted SSPI design revision 2 §§2–7; implementation plan step 1; caller guide, particularly lines 17–68; acceptance record; current-checkpoint SSPI entries.
- `AgentProcessRules.md` review/amendment requirements and `GwzProcessOptimization.md` §§3–4, 8.
- Member `docs/WireProtocol.md`, lines 1–190; architecture/testing policies; README and implementation status; protocol README, DSL, generator pin and fingerprint manifest; generator, lines 1–87; synthetic tests, lines 1–126; CI workflow and Cargo package inclusion rules.
- Core Windows parity SSPI amendment, lines 15–43, and §8, lines 267–333.
- Exact root and member review-range diffs.

Executed only the two authorized checks with the external schema-tools interpreter and `-B`:

- `scripts/regen_schema.py --check`: exit 0, “2 schema artifacts verified”.
- Schema unittest discovery: exit 0, ten tests passed in 0.004 seconds.

All three `rev-parse HEAD` values matched the prescribed tuple at both start and end. No writes, builds or Git mutations occurred. Cargo/archive/native claims were inspected as recorded claims, not independently rerun.

## 1. Findings

### [P2-1] HTTP token cap has a wire carrier but no declared caller input

**Location:** Root checkpoint lines 5–6 and 27; member `WireProtocol.md` lines 4–5, 63–67 and 78–85; accepted caller guide lines 24–26 and 33–42.

**Invariant:** The independent library must receive the HTTP-owned token bound through its declared caller contract, while this checkpoint expressly preserves that accepted API.

The wire contract requires that “core supplies its existing HTTP token limit” as `Begin.token_limit`. Both parent and worker must enforce it before native authentication. However, the accepted `AuthRequest` description supplies package, target, identity, binding and Digest input; no HTTP/raw-token-limit input is declared. `start` additionally receives only deadline and cancellation. Neither `Options` nor `step` provides this missing input.

**Reproduction:** Follow the documented caller sequence: construct the listed `AuthRequest` values, call `start`, then `step(None)`. The parent must now encode required `Begin.token_limit`. The core-owned HTTP bound has never crossed the API. The parent can guess a constant, introduce an undeclared request field, or depend on HTTP policy outside its boundary; none implements the stated composition unchanged. For two otherwise identical requests subject to different host HTTP bounds, the declared library inputs cannot distinguish the required wire limits.

**Impact:** The freeze promises an unenforceable input path. Implementers must silently change the accepted API or weaken the requirement that native input/output remains within the host’s HTTP bound. This is a concrete interface/composition defect, despite production adapters being deferred.

**Required correction:** Explicitly declare the typed caller carrier for the raw token cap, its ownership, units, required/default behavior and valid range; document its exact transfer into Begin and subsequent enforcement. Amend the caller guide/design and the “no caller API change” claims with explicit precedence. If this changes the accepted public surface, obtain the applicable Surface review rather than carrying forward its previous verdict. It need not become a user configuration knob.

**Closure test:** A contract fixture or model must demonstrate two identical authentication requests differing only in the declared cap producing correspondingly different Begin limits; cap±1 challenges/output and 0/65,537 must refuse at the specified boundaries. Verify the corrected caller-to-wire path across the documents. This is one new architectural root cause: a missing input bridge between the frozen caller surface and private IPC.

## 2. Invariant analysis

Other attacks did not establish defects:

- DSL/export agreement and semantic fingerprint regeneration held. Semantics bytes participate in the digest; acceptance status resides separately.
- Digest’s full initial challenge, method and URI reach the first step; other packages prohibit them. Actual SID/LUID/session and exact fingerprints precede Begin.
- Binding prefix and bounded digest representation agree with Windows §8.
- Seven bodies, closed envelopes, round limits, phase-specific replies and serialized Finish form a coherent ordered protocol. Dropping an outstanding step cancels instead of inserting Finish.
- Finished proves acknowledged disposal only; process/Job/thread confirmation remains necessary for cleanup and capacity release.
- Secret projections and strict admission remain explicit later gates. Synthetic reference-codec results are not presented as production zeroization or lifecycle evidence.
- No worker clock, retry, application envelope or Rust dependency expansion appeared. CI inputs and packaged tooling are standalone.

## 3. Risks and next action

Production codec, installed artifact matching and native qualification remain deferred; this review supplies no implementation assurance.

Next action: resolve P2-1 in the merged remediation package and submit the corrected tuple for verification of the caller-to-wire cap path.
