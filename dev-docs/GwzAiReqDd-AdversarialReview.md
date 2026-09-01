# Adversarial Review — GWZ AI Execution Layer Requirements and Design

Date: 2026-09-01

Review object: [GwzAiReqDd.md](GwzAiReqDd.md)

Status: Original adversarial response preserved; revised design regrade in
section 9

## 1. Verdict

**Original verdict: GO as a direction memo. NO-GO as an implementation
charter.**

Original finding count: **0 P0 / 4 P1 / 6 P2 / 3 P3**.

The central thesis is right: GWZ can become a strong execution boundary between
an unreliable AI controller and a workspace of independent repositories. The
existing product has unusually good raw material for that role: typed semantics,
real repository independence, checked artifacts, coordinated identity,
structured degradations, streaming output, and cross-client protocol discipline.

The draft is also too broad to implement safely as one program. It combines a
durable workflow engine, an authorization system, a task-clone manager, an audit
store, an instruction trust model, remote review/CI coordination, and future AI
transport adapters. Starting from this document without first collapsing the
scope would repeat the process failure that the document says it wants to avoid.

## 2. Blocking findings

### P1-1 — The first deliverable is not actually bounded

Sections 6, 8, and 12 describe several independently difficult products. AI-1
still includes task-clone identity, immutable planning, authorization, durable
operations, effect fences, retry, cancellation, receipts, and two public client
surfaces. That is not a minimal slice; it is the center of the whole system.

The document needs one vertical operation with a hard production-LOC and public
surface cap. It should name the exact existing GWZ operation being wrapped, the
effects it may perform, the crash cuts it must survive, and everything it is
forbidden to generalize. Otherwise the coordinator will become a speculative
framework before its invariants are proven.

**Required disposition:** replace AI-1 with one local-only operation, one storage
mechanism, one capability shape, one receipt, and no new agent protocol. Give it
a two-round terminal review cap.

### P1-2 — Idempotency is a slogan until identity and drift semantics are exact

The draft says that same identity plus same intent converges, but it does not
define who is authoritative for the identity or what constitutes the same
intent. It binds refs, manifests, locks, policy, instructions, tool versions,
and capabilities into planning, yet does not define which changes:

- make the original operation safe to resume;
- require a new plan under the same operation;
- require a new operation identity; or
- make all further automation unsafe.

There is also no exact rule for an accepted request whose durable journal write
succeeds but whose caller never receives the operation identifier. This is the
first retry problem an AI host will produce.

**Required disposition:** specify identity allocation, canonical intent bytes,
collision behavior, lost-response recovery, plan supersession, and a drift table
before implementation.

### P1-3 — A full clone is not a security boundary

The draft correctly says this once, but the rest of the architecture still
risks making “task isolation” sound like safe execution of repository code. A
full clone isolates Git working state. It does not stop hooks, build scripts,
test runners, compiler plugins, or prompt-injected commands from reading the
user's files, using inherited credentials, opening the network, or modifying
other clones.

The capability table is not yet an enforcement design. “Declare the effect” is
not enough if a child process can perform undeclared effects after launch.

**Required disposition:** publish a separate threat model that distinguishes
GWZ-enforced checks from host-enforced sandboxing. The first mutating slice must
either execute no repository code or require a named sandbox profile whose
enforcement is tested.

### P1-4 — Remote workspace changes have no defined source of truth

The draft rejects the fiction of atomic remote publication, but its
`WorkspaceChange` model does not answer the hard questions. If two repositories
push, one pull request opens, another provider times out, a human edits a branch,
and the AI host disappears:

- which local or remote record owns the change identity;
- which observation is authoritative;
- who may retry or close remote objects;
- how rewritten branches are proven to belong to the change; and
- when reconciliation is terminal rather than indefinitely pending?

This is a separate milestone and perhaps a separate design document.

**Required disposition:** remove remote review and CI from the first execution
layer implementation. Before AI-3, specify a provider-neutral reconciliation
state machine with exact identity proofs and human takeover.

## 3. Major nonblocking findings

### P2-1 — Most requirements are reliable automation requirements, not AI requirements

Durability, policy, idempotency, leases, receipts, and recovery are necessary for
CI and ordinary automation too. That is a strength, but the document should say
which delta is specifically caused by AI:

- probabilistic intent and repeated delivery;
- untrusted natural-language context;
- unattended authority boundaries;
- explanation suitable for a controller that may have lost context; and
- resistance to prompt injection through repository and remote content.

Without that distinction, “AI execution layer” risks becoming branding for a
general workflow engine.

### P2-2 — Capability policy is underspecified

There is no grammar, precedence lattice, delegation model, revocation behavior,
or non-interactive approval mechanism. A closed vocabulary is recommended, but
the first vocabulary is not proposed. File paths, Git ref mutation, hook
execution, child processes, network destinations, credentials, and remote
publication do not all have the same containment properties.

The implementation must not begin by inventing an open-ended policy language.
Define a small algebra that can be reviewed exhaustively.

### P2-3 — Cancellation and effect fences are not exact enough

“Unstarted effects do not begin” is necessary but not testable until effect
boundaries are identified. Git commit, filesystem rename, branch push, pull
request creation, and a hook-launched child process all have different points of
no return. A process may be cancelled after the external system accepted an
effect but before GWZ observed it.

The state machine needs per-effect prepare/attempt/observe/reconcile rules, not
only operation-level states.

### P2-4 — Exact evidence conflicts with privacy and retention

The receipt is asked to be exact, portable, tamper-evident, redacted, bounded,
and partially deletable. Those properties can conflict. A digest of a low-
entropy secret may disclose it. A redacted command may be insufficient to prove
what executed. Deleting evidence may make safe replay impossible.

The draft needs evidence classes: recovery-critical, audit, diagnostic, secret
reference, and disposable payload. Their retention and hashing rules must differ.

### P2-5 — Clone-per-task economics are missing

Rejecting worktrees is a legitimate product decision, but full clones across
many large repositories and concurrent agents have substantial disk, network,
indexing, and cleanup costs. “Local object reuse” is only a suggestion. The
design needs measured targets for creation latency, shared object safety, disk
ceilings, garbage collection, and orphan discovery.

Otherwise the execution layer may be correct but operationally unusable for the
very multi-agent workloads it targets.

### P2-6 — Transport neutrality is asserted, not demonstrated

CLI, Python, streaming records, MCP, and JSON-RPC have different cancellation,
backpressure, lifetime, authentication, and error boundaries. A generated data
schema does not by itself produce semantic parity.

Do not select an AI transport in advance. First prove the coordinator through
the existing in-process/Python boundary, then require an actual host integration
to justify another adapter.

## 4. Minor findings

### P3-1 — “Execution layer” needs a user-visible boundary

The architecture has a coordinator but does not say whether it is a library, a
long-lived service, or a per-command object. That decision materially changes
leases, journal access, cancellation, and authentication.

### P3-2 — The naming risks making GWZ appear AI-dependent

A public `gwz ai` namespace would imply that safe planning, authority, receipts,
and recovery are special AI modes. They should become ordinary GWZ properties.
AI adapters may be named for AI hosts; the semantic layer should not be.

### P3-3 — Usability has no acceptance bar

The design can pass every durability test and still be too difficult for a
human to operate. The plan should include bounded user tests for understanding a
refusal, approving a plan, finding an interrupted operation, and reconciling a
partial workspace change without the original agent.

## 5. What the draft gets right

The review does not recommend abandoning the direction. The strongest choices
are:

- treating the AI host as fallible and outside the semantic trust boundary;
- keeping models and prompts out of the core;
- using the same semantics for humans, Python, and future adapters;
- refusing silent replanning after bound state changes;
- making partial success a typed state;
- rejecting Git worktrees while explicitly retaining full repositories;
- separating instruction provenance from authority;
- admitting that remote multi-repository delivery is reconciliation, not a
  transaction;
- requiring machine-derived explanations rather than generated advice; and
- protecting M5 from scope expansion.

These choices are more important than adding an MCP server or an “agent mode.”
If enforced, they would make GWZ a credible AI execution substrate rather than
another shell-command wrapper.

## 6. Recommended minimal charter

Before any broad execution-layer program, charter one vertical experiment with
all of these constraints:

1. **Local only.** No pushes, pull requests, CI providers, or network effects.
2. **One existing operation.** Use a bounded operation whose core semantics are
   already reviewed; do not generalize unrelated GWZ handlers.
3. **No repository code execution.** No hooks or arbitrary child processes in
   the first slice.
4. **One durable store.** Choose and document its ownership, atomicity,
   corruption behavior, and cleanup.
5. **One identity contract.** Define canonical intent, caller key, GWZ operation
   id, lost-response recovery, collision, and retention.
6. **One closed capability set.** Read workspace, mutate named repositories and
   named path classes, or refuse. No generic policy language.
7. **One exact state machine.** Include per-effect fences and every crash cut.
8. **One receipt.** It must be sufficient for a human to inspect and safely
   retry without the AI host.
9. **One task-clone identity path.** Inspection is required; automatic creation
   and retirement may be a later slice if they threaten the cap.
10. **Hard scope control.** Set a production-LOC budget, a frozen-surface list,
    an independent reviewer, and a two-round terminal cap.

The experiment succeeds only if it proves these three statements under process
death, duplicate delivery, and concurrent invocation:

> No effect occurs outside the authorized plan.

> No externally visible effect occurs without a durable intent/effect fence.

> Every accepted invocation returns a receipt or a recoverable operation id.

## 7. Required dispositions before implementation

| Review item | Recommended disposition |
|---|---|
| P1-1 first-slice breadth | Accept; replace AI-1 with the minimal charter in section 6. |
| P1-2 identity and drift | Accept; add canonical bytes, allocation, lost-response, supersession, and drift tables. |
| P1-3 clone versus sandbox | Accept; publish the enforcement boundary and exclude repository execution initially. |
| P1-4 remote source of truth | Accept; defer AI-3 and require a separate reconciliation design. |
| P2-1 AI-specific delta | Accept editorially; distinguish AI threat drivers from general reliability mechanisms. |
| P2-2 capability grammar | Accept; freeze a small closed vocabulary for the first operation only. |
| P2-3 effect-level cancellation | Accept; specify effect state transitions and observation ambiguity. |
| P2-4 evidence privacy | Accept; define evidence classes before storing real secrets or commands. |
| P2-5 clone economics | Accept as a measured prerequisite for task-environment automation. |
| P2-6 adapters | Accept; defer MCP/JSON-RPC until a real integration requires one. |
| P3 usability and naming | Carry into product design; do not create a public `gwz ai` command by default. |

## 8. Final response

GWZ is already closer to a trustworthy multi-repository AI execution substrate
than most competing tools because it owns semantic coordination rather than
merely launching the same command in several directories. That advantage will
be lost if this draft turns into a large orchestration framework before the
identity and recovery kernel is proven.

The next design step should therefore be smaller, not larger: finish M5 as
already chartered, resolve the blocking identity/storage/security decisions,
and prove one local durable operation end to end.

Do not call “CLI plus JSON” an execution layer. The layer exists when authority,
idempotency, durable recovery, and receipts are enforced even when the AI host
is wrong, duplicated, interrupted, or gone.

## 9. Re-review of the revised design

### 9.1 Revised verdict

**GO for AI-0 and the bounded AI-1S design.**

Revised finding count within AI-1S: **0 P0 / 0 P1 / 0 P2 / 0 P3**.

This is not a GO for the target architecture, AI-1P through AI-4, or immediate
code. AI-1S becomes launchable only after its section 15 adoption conditions and
AI-0 fixtures are recorded. Each deferred milestone still requires a new
charter and adversarial review.

The revision makes the critical distinction the original draft lacked: it
separates a broad product direction from one implementation-sized proof. The
proof is local snapshot creation, not a generic workflow engine. It has one
effect, two closed capabilities, no repository code, no remote system, no
public adapter, an exact identity contract, and a terminal review cap.

### 9.2 Finding dispositions

| Original finding | Regrade | Revised-design evidence |
|---|---|---|
| P1-1 unbounded first deliverable | CURED | Section 12.2 selects only existing snapshot creation, sets a 600-production-line/eight-file cap, freezes excluded surfaces, and imposes a two-round terminal review. |
| P1-2 incomplete idempotency identity | CURED | Section 9.1 defines caller namespace/key bytes, canonical CBOR intent, SHA-256 digest, GWZ UUIDv7 allocation, durable key mapping, lost-response recovery, collisions, plan revisions, and the post-fence no-replan rule. |
| P1-3 clone mistaken for security | CURED for AI-1S | Section 7.1 divides GWZ and host enforcement. AI-1S launches no repository code and uses no hooks, child process, network, or credential. A sandbox profile is mandatory before any later charter does so. |
| P1-4 remote source of truth | CURED by exclusion | Section 12.5 makes remote delivery a separately reviewed AI-3 design and states the minimum dual-observation reconciliation rule. It is not an AI-1S condition. |
| P2-1 AI-specific delta missing | CURED | Section 2.1 isolates duplicate delivery, probabilistic intent, prompt injection, unattended authority, context loss, and structured explanation from general automation reliability. |
| P2-2 policy grammar missing | CURED for AI-1S | Sections 8.3, 12.2, and 14.1 define exactly two nondelegable capabilities approved for one expiring plan digest; no general language exists. |
| P2-3 effect cancellation vague | CURED | Sections 10.1 and 10.2 define prepare, atomic create, observe, cancellation timing, restart, drift, and ambiguity outcomes for the sole effect. |
| P2-4 evidence/privacy conflict | CURED for AI-1S | Section 8.5 separates recovery, tombstone, audit, diagnostic, and secret-reference classes with concrete retention and forbids raw-secret hashes. |
| P2-5 clone economics missing | CURED as a future gate | Section 12.4 supplies relative time/storage budgets, forbids finished-task alternates, and requires platform, disk, ownership, and cleanup measurements before AI-2. |
| P2-6 transport neutrality unproven | CURED by exclusion | AI-1S has an internal driver only; AI-1P decides ordinary product exposure and AI-4 requires a named host before another transport exists. |
| P3-1 process boundary absent | CURED | Section 8 defines a per-invocation in-process coordinator with journal-owned durability and makes a service a new charter. |
| P3-2 AI-dependent naming | CURED | Sections 2.1, 12.3, and 15 explicitly reject a special `gwz ai` semantic mode or command by default. |
| P3-3 usability acceptance absent | CURED for exposure | Section 13.1 requires four human recovery/refusal tasks before AI-1P. |

### 9.3 Remaining cautions, not findings

- The 600-line cap is credible only if AI-1S reuses existing atomic artifact and
  boundary machinery. If it needs a new general storage framework, the lane
  must stop rather than reinterpret the cap.
- Root Git-directory durability and no-follow behavior must be proven on every
  supported platform during AI-0. The design decision is not implementation
  evidence.
- Canonical CBOR fixtures must be checked in before code. “Deterministic” cannot
  be delegated to whatever a serializer happens to emit.
- The snapshot artifact's `created_at` and creator fields must be plan inputs so
  retry constructs byte-identical output. Any execution-time clock read after
  planning violates the charter.
- A test-only driver is appropriate for the proof, but it must not become a
  permanent shadow API. AI-1P must either expose an ordinary supported surface
  or remove the experiment after extracting the proven primitive.

### 9.4 Final reviewer response

The revised design now does what a requirements/design document should do: it
states a broad destination while making the first authorized move deliberately
small, falsifiable, and recoverable.

The execution layer is still an ambitious product program. AI-1S is not. It is
one exact snapshot artifact protected by durable identity, authority, a fence,
read-back, and a receipt. If GWZ cannot make that small operation survive
duplicates, crashes, cancellation, and lost responses, it should not attempt
remote multi-repository orchestration. If it can, the result will be real
evidence for the next design rather than architecture by aspiration.
