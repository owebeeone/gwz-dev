# HTTPS SSPI composition proposal — Surface-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md` at `fe40ba9b23e31da95358441ae5214a7cadb31b31`, dated 2026-10-04. Committed DRAFT proposal only; proposed APIs are unavailable in the current package. Timeout-zero disposition remains pending.

**Baseline:**

| Repository | Committed HEAD |
|---|---|
| gwz-dev | `fe40ba9b23e31da95358441ae5214a7cadb31b31` |
| gwz-core | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-sspi | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |

Both caller guides were read using `git show fe40ba9b23e31da95358441ae5214a7cadb31b31:<path>` with numbered lines. All six HEADs matched the canonical prompt at the start and end.

**Date:** 2026-10-04

**Axis:** Surface — cold proposed signatures, ownership, sharing, defaults, errors, lifecycle and caller recipe. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. The proposed caller contract is coherent within this review’s evidence boundary. This is not final freeze or implementation authorization before the pending operator decision.

---

## 0. Evidence base

Read in full:

| Document | Lines | Evidence examined |
|---|---:|---|
| `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md` | 1–168 | Proposed capture/start signatures, ownership, concurrent sharing, deadline source, availability, errors and disposal |
| `dev-docs/GwzSspiCallerGuide-DRAFT.md` | 1–146 | Existing conversation API, defaults, mechanism acceptance, cancellation and retained cleanup |

Also read the complete canonical `GwzSspiHttpsComposition-PromptSurface.md`. No linked design, plan, checkpoint or review document was followed.

Commands were limited to `cat`, committed `git show` reads, numbered output, and `git rev-parse HEAD` / `git status --short` for the root and five named members.

SSPI, CLI, Python and transport were clean at both checks. Root and core contained the prompt-declared out-of-scope untracked items; their status lists were unchanged.

No source code, implementation tests, drafter narrative or current peer contents were read. No writes, builds, tests, execution, remote commands or Git mutations occurred. Hypothetical signatures were assessed as proposals and were not compiled.

## 2. Invariant analysis

**Names and placement:** `Supervisor::capture_caller` and `Supervisor::start_captured` sit beside the existing Supervisor start operation. Their names distinguish original-caller capture from starting with that capture. `CallerCapture` is expressly opaque and exposes no identity, credential or raw-handle getters.

**Exact ownership:** The proposed signature takes `&CallerCapture`, while the synchronous call retains its own private origin reference. The returned future is explicitly Send + `'static` and borrows neither capture nor Supervisor. The recipe’s `use<>` return matches that stated independence. Dropping the caller-owned capture therefore does not invalidate a previously constructed Start.

**Concurrent sharing:** The capture is Send + Sync without Clone or Debug. The guide gives `Arc<CallerCapture>` as the sharing mechanism for concurrent member Opens. Each Start gets its own one-use admission ticket and uses the same Supervisor capacity. No cloning of a native owner or reuse of a failed Conversation is prescribed.

**Supervisor association:** A capture belongs to its issuing Supervisor. A foreign capture has a stated InvalidRequest result before registration. Closing admission during capture returns Closed; dropping a capture cannot reopen admission or release another conversation’s permit.

**Original-caller continuity:** Capture occurs before thread handoff. Later Start creation performs no metadata query or native-handle duplication. Launch rechecks the original thread’s liveness, impersonation and primary identity; an exited origin refuses. Retry creates another Start from the same capture rather than substituting the executor’s identity.

**Defaults and deadline:** Capacity remains default eight, range 1–64. TokenLimit remains required with no default. The library chooses no timeout. The proposed core deadline is one logical HTTPS Open setup deadline captured before first checkout/adoption, normally from the positive existing 30-second aggregate. Reuse, redirects, helper work, capacity, launch and native rounds keep that timestamp. The guide expressly prohibits resetting it at each Start or challenge.

**Pending zero decision:** Both the opening status and deadline section mark timeout-zero native refusal as pending. The page does not present that outcome as an approved default or implemented behavior. Expired positive deadlines have a stated Timeout result.

**Native availability:** Descriptor, Supervisor and capture refusal is retained for later native selection. Anonymous, existing Basic and SSH paths do not require successful native setup. The capture recipe retains its Result rather than immediately propagating a capture error into every transport path.

**Errors:** The table assigns fixed classifications for closed admission, capture identity failures, foreign captures, invalid requests, cancellation, expiry, launch recheck and worker mismatch. Pre-registration refusals have no RecordId and report Confirmed cleanup for that Start without disposing the caller-owned capture. Registered failures preserve existing record and cleanup semantics.

**Lifecycle pairs:** Capture is paired with Drop disposal. Start retains independent origin ownership; Conversation has the existing finish/cancel/Drop behavior. Shutdown uses a separate explicit cleanup deadline, and dropping its future preserves retained cleanup. The synchronous last-reference handle-close exception is disclosed without claiming a hard OS bound.

**Cold walkthrough:** From the two permitted pages, a caller can obtain the trusted Supervisor, capture on the original operation thread, retain the availability Result, share a successful Arc, construct and transfer an owned Start, await verified Hello, begin once, pass bounded challenges on the same leased origin connection, distinguish native Complete from remote acceptance, and finish/cancel/shutdown. No missing lifecycle command or ownership handoff required a guess.

**Success and replay boundaries:** The proposal keeps the exclusive lease and origin through all rounds, limits the conversation to eight steps, separates remote acceptance from token generation, discards a challenged lease on failure/cancel, and prohibits authentication replay of a partial or complete POST and switching credentials after rejection.

These attacks did not establish a concrete Surface defect.

## 3. Risks and next action

This review establishes coherence of the proposed caller-facing contract. It does not validate implementation feasibility beyond the documented ownership shape, native qualification, HTTP state-machine behavior or physical transport operation. Prior installed-host acceptance does not cover this proposal.

The next action is to obtain the pending operator disposition for timeout-zero behavior before final freeze or implementation authorization, while retaining this Surface GO for the reviewed proposal.

