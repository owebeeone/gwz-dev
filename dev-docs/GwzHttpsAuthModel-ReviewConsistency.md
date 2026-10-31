# HTTPS authentication model (DRAFT r2) — Consistency-AXIS REVIEW
**Review object:** dev-docs/GwzHttpsAuthModel-DRAFT.md at root 0a0545a3 (code at gwz-core a7b965c5, gwz-transport 0a253639)
**Baseline:** I read every root document with `git show 0a0545a3:<path>`, and code with `git -C gwz-core show a7b965c5:<path>` and `git -C gwz-transport show 0a253639:<path>`. I took the gwz-py capture site (`native/src/route/transport.rs`, `client_host.rs`) from the gwz-py working tree, as reference only. At start and at end, `git diff 0a0545a3 HEAD -- dev-docs/GwzHttpsAuthModel-DRAFT.md` was empty (HEAD was 0a0545a3 at start and 8d0837bc at end). I made no writes to the workspace, ran no builds and used no model.
**Date:** 2026-10-31
**Axis:** Consistency: the draft against its controlling graph (contract, plan, 1.8 remediation plan, p1c-https reviews, code at the pinned SHAs). Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO** — 0 P0, 0 P1, 3 P2, 11 P3. I pre-commit to GO on a revision that resolves P2-1, P2-2 and P2-3 as specified below and dispositions every P3 (fixed, or recorded with an owner and a closure test).

## 0. Evidence base

- **Object:** the 399 lines of the draft, read in full.
- **Contract `GwzConnectionApiContract.md`:**
  - §4.1 (L160-208), §6 (L285-300) and §7 (L461-505, table at L489-496);
  - §7.1 (L507-532) and §7.2 (L534-570);
  - §8 (L592-630), §9 (L632-666), §10 (L668-713);
  - §15 rows 1.3c/C2(b) (L881), 1.5 (L882), 1.6 (L883), the integration fix-up (L884) and 1.7 (L887).
- **Plan `GwzTransportReleaseConvergencePlan.md`:**
  - §1 (L35-79, threat model L55-62, placement L63-79) and §3 (R1, R2 and R4 at L166-169);
  - §4 steps 2.4 (L681-690) and 3.1/3.2 (L721-735, Prerequisite L724-728);
  - the §10 decision list (L907-1001).
- **Remediation plan `GwzPhase1-RemPlan.md`:** whole (dispositions L28-51, the "HTTPS model" row L35, steps L57-66, waves L68-73).
- **Code review `GwzPhase1-ReviewCode.md`:** item 3 (L117-120) and P2-2 (L160-180).
- **p1c-https reviews:**
  - `history/GwzP1cHttps-ReviewState.md` (PR5, L63 and L94-102);
  - `history/GwzP1cHttps-ReviewState-2.md` (PR6, L34 and L84-98);
  - `history/GwzP1cHttps-ReviewCode.md` P2-1 (L58).
- **Uncited documents:**
  - `GwzSplitCommandChannel.md` §3.4 (L135-166, served walkthrough L156-166), §5 admission (L313-344) and the facts table (L188-189);
  - `GwzServerCore-Step2.3-Plan.md` (ownership, L84-238);
  - `GwzSspiDesign.md` L66-84.
- **Code checked line by line against the draft's citations:**
  - gwz-core:
    - `https_adapter.rs` (L90-96, L98-183, L276-325);
    - `https_adapter/{exchange,auth,pipe,worker}.rs`;
    - `https_auth.rs` (L27, L36-87), `https_auth/secret.rs`, `https_auth/lookup.rs`;
    - `https_policy.rs` L41-130, `https_destination.rs` L32-145;
    - `transport_host/endpoint_environment.rs` L100-230;
    - `https_worker/{credentials,serve,prepare,native,native/connection,native/exchange}.rs`;
    - `connection_host/https_auth_tests.rs` (12 rows, L42-650) and `https_tests.rs` L117-122.
  - gwz-transport:
    - `src/pool/{allocation.rs,mod.rs,machine.rs}`;
    - `crates/gwz-connection/src/{ports,instance,exchange,operation,binding,host}.rs` and `remote/served.rs:503`;
    - `crates/gwz-connection/Cargo.toml`.
- **Citations confirmed accurate:**
  - `https_adapter.rs:93`, `:119-121`, `:123-132` and `:308-325`;
  - `exchange.rs:54`, `:86-96`, `:206`, `:219-231`, `:239-240`, `:257`, `:264`, `:269` and `:290`;
  - `auth.rs:41-68`, `:67`, `:69-80` and `:81`;
  - `https_policy.rs:48` and `:55-58`; `worker.rs:395-406`; `pool/allocation.rs:22-28` (a `BTreeMap` scan, `machine.rs:111`);
  - `pool/mod.rs:214`; `ports.rs:206`; `instance.rs:349` and `:567`; `endpoint_environment.rs:207`; `https_auth.rs:27` and `:48`;
  - `native.rs:134-139`; `native/connection.rs:119-122`; `https_tests.rs:118`;
  - `GwzSspiDesign.md:66-67` and `:72-84`;
  - every contract line the draft quotes, which I checked verbatim, including L494, L563, L565, L663, L686 and L881.
- **Citation discrepancies:** listed in P3-2.

## Problem, requirement provenance, and scope

- **PR5 (placement, Basic).** Solved by design. The credential is held on the operation's provider and sent on whichever connection the pool leases (§3(b)), and row H1b-1 pins it.
- **PR6 (single slot).** Solved. The slot is removed, and row H1b-2 pins it.
- **Phase 3 concurrency, Basic.** Solved, in both the in-process and the served `cli` placement:
  - the record is written before the advertisement body is delivered (`exchange.rs:264-269`; §3(h));
  - the provider exists only client-side (plan L72-74; Split L136-137).
- **Phase 3 concurrency, NTLM/Negotiate.** Solved for correctness by the re-authentication fallback. The hint, the operator-decided placement mechanism, is structurally late (P3-4).
- **Traceability of each new mechanism:**
  - `Needs`/`Need::Basic`: decision 1 (b2), PR5 and PR6.
  - The class in `NeedKey`: decision 4.
  - `ConnectionBound`, `Request.prefer`, `ExchangeIo::connection_bound` and the affinity table: decision 2.
  - The re-authentication GET: decision 2 (correctness fallback).
  - Credential dedupe and the bounds: retention charge (contract L686, L692-694).
  - Shared-credential poisoning: Git-like behaviour; it is a design choice, not an operator decision.
  - The `closed` flag and the clearing `Drop`: trace to nothing reachable (P3-6).
- **Provenance.**
  - The task boundary (draft L8-11) is attributed to the operator, but it is wider than the operator-recorded R4 row (P3-7).
  - The draft treats the four operator decisions as decided and realizes them. Decision 2's realization is weaker than its wording (P3-4).
- **Confirmed code facts and hypotheses.**
  - Confirmed at the pinned SHAs:
    - the PR5 and PR6 mechanics;
    - lowest-id leasing;
    - the scope written after the task returns;
    - the per-request `challenging` fixture;
    - the absence of an unsolicited native token;
    - helper output limited to 16 KiB;
    - `Secret` wiped on drop;
    - `pub(super)` fields reachable from an `https_auth` child module.
  - Labelled as hypotheses by the draft, correctly: Git's `http.c` and `credential.c` behaviour, and a server accepting a header-less POST on a GET-authenticated connection.
  - Asserted without evidence:
    - the hint hits in the consumer's real order (P3-4);
    - one capture per admitted operation (P3-10);
    - "trailing empty segment popped" for the key (P3-2: true of the request URL, not of `Destination`).

## 1. Findings

### P2-1 — The re-authentication GET ends a POST with `Effect::None`, contradicting contract §9's effect boundary, which the amendment list never touches

- **Location:**
  - Draft §3.1 L239-240: "If it fails, the exchange ends before any POST byte (`Effect::None`)".
  - Draft §4.4 L311-312.
  - Draft §4.6 L322: "Retained: no replay".
  - Contract §9 L660-663: "Before an SSH exec is submitted, or an HTTP request is handed to the HTTP engine, a stale idle connection may be replaced ... After that boundary the effect is `PossiblySent` even if no byte acknowledgment arrived".
- **Root cause:** the design adds a second HTTP request inside one exchange. It reasons about the effect of the POST's body, but contract §9 defines the effect by when the exchange's first request is handed to the HTTP engine.
- **Violated invariant:** §9's typed `Effect` boundary, which R2a is building now (RemPlan L33: "`Effect` on local `OpenFailure`/`ExchangeEnd`"). §15 is "the bounded amendment list" (L855).
- **Sequence:**
  1. A POST for a `ConnectionBound` key is leased an anonymous connection.
  2. The adapter hands the re-authentication GET to Hyper.
  3. The server answers 401 to the native token, or resets the connection mid-advertisement.
  4. The draft reports `Effect::None`. Contract §9, read literally, requires `PossiblySent`, because a request crossed the engine boundary.
- **Impact:** R2a and the gwz-core consumers implement §9 as written, or follow the draft. Either way the contract becomes self-contradictory:
  - If §9 wins, a push whose pack never left the client is reported as a possible remote effect. The Split design L231 renders exactly that to the user.
  - If the draft wins, §9 is silently amended.
- **Required correction:**
  - Add a §9 amendment row to §4.6. The `Effect` of an exchange describes its own request.
  - A re-authentication advertisement GET sent inside a POST exchange does not move the boundary. The POST is handed to the engine only after the GET's 200 body has been drained.
  - A failure during the re-authentication is `Effect::None`, and the stale-idle replacement of L660-661 does not apply to it ("at most once per exchange").
  - State the same in the §4.4 text.
- **Closure test:**
  - Row H3-4 asserts `Effect::None` on the `ExchangeEnd` or `OpenFailure` the client reads, plus zero POST bytes at the server.
  - Add a row H3-4b: a reset during the re-authentication body, with `Effect::None` and no second re-authentication.
  - §15 carries the §9 row.

### P2-2 — Rejection is specified two incompatible ways: the contract text says the credential is wiped, but the design keeps it in memory until every sharing key is next used or the operation ends

- **Location:**
  - Draft §3(b) L163-165: "every key sharing that `Arc` becomes `Rejected` at its next use without a request, and the secret is wiped when its last key drops it".
  - Draft §4.2 L295-296: "marks the key and every key sharing that credential rejected for the rest of the operation, and wipes the credential".
  - Draft §5 row H1b-6 L355: "the secret buffers read zero afterwards".
- **Root cause:** poisoning is lazy, applied at each key's next use, over an `Arc<Secret>` that every sharing key holds.
- **Violated invariant:** the proposed contract clause itself, together with contract L504-505: "Ports ... must pass credential redaction/retention conformance".
- **Sequence:**
  1. One host credential serves the 16 private members (`useHttpPath=false`), deduplicated to one `Arc`.
  2. A POST for member 1 gets a 401. Its key becomes `Rejected`.
  3. Members 2 to 16 are never used again in this operation, or are used much later.
  4. The `Secret` stays allocated until the operation ends. Any exchange that cloned the `Arc` to build a header also delays the wipe.
- **Impact:**
  - The amended contract claims a retention property that the design does not provide.
  - Row H1b-6 passes only because it touches every sharing key first. A retention conformance row derived from §4.2 (reject, then check without touching) is red by design.
- **Required correction (either):**
  - **(a) Eager.** Under the `Needs` mutex, at the moment the 401 is classified:
    - replace every entry holding that `Arc` with `Rejected`;
    - state that headers are built under the lock, or that any `Arc` clone is dropped immediately after the header is built.
    The secret is then wiped as soon as no in-flight request holds it.
  - **(b)** Change §4.2 to say exactly what the lazy design does.
  - I recommend (a): 512 entries make the scan trivial, and the text stays true.
- **Closure test:** row H1b-6 becomes "after a 401 on key K1, with K2..K16 sharing the credential and never used afterwards, the credential's username and password allocations read zero before the operation finishes". This needs a test hook on `Secret`, like the existing `SecretHeader` records hook (`secret.rs:18-34`).

### P2-3 — Bound overflow silently drops the credential and reproduces the misleading `Authentication` refusal the design exists to remove

- **Location:**
  - Draft §3(b) L141-142: "A credential over 4 KiB or a 65th distinct one is not retained: the key is not recorded, and its POST is refused as an unrecorded key (typed, fail closed)".
  - Draft §4.2 L297-298: "what does not fit is not recorded".
  - Row H1b-8 L358.
  - Draft §1 L50-51: "the refusal reads 'Authentication' although the helper had a credential".
  - Contract §10 L691-692: "it does not silently omit environment/credential inputs".
- **Root cause:** the advertisement that cannot be recorded still succeeds. Overflow is detected at the GET and reported at the POST.
- **Violated invariant:**
  - Contract §10 L691-692's no-silent-omission principle, applied here to a runtime bound.
  - Contract §9 L665-666, in spirit: no anonymous attempt of a mutating body that is known to be doomed.
  - Diagnosability: the problem statement's own L50-51.
- **Sequence:**
  1. A 65th distinct credential, or a 4,097-byte credential, answers GET `/repo` (receive-pack) with 200. Nothing is recorded.
  2. gwz-git POSTs `/repo` anonymously, from its first byte.
  3. The server answers 401, and the exchange ends `Authentication` with `PossiblySent`.
- **Impact:**
  - The exact misdiagnosis of PR5 and PR6 returns for every key over the bound.
  - The pack body is put on the wire anonymously.
  - Row H1b-8 asserts this misleading behaviour as correct.
- **Required correction:** an exchange that would record a key or credential the bounds cannot hold ends before delivering any advertisement byte. It ends with a distinct typed code, a budget refusal (for example the `InputBudget` class of §11 L719, or a new named code), never `Authentication`. Then no POST for that key follows. Record the four bounds in contract §10's table (see P3-1).
- **Closure test:** row H1b-8 becomes:
  - the 513th key's advertisement and the 65th credential's advertisement each end with the budget code, with no body delivered;
  - no POST for either key reaches the server;
  - earlier keys still work.

### P3-1 — The superseded-clause list is incomplete, and one proposed edit is wrong

- **Location:** draft §4.3 L302 and §4.6 L318-323; contract L486-487, L564-565, §10 table L675-687, and §15 L881 (the "Open for Phase 3" sentence); plan L724-728.
- **Root cause:** the amendment set was assembled from the mechanisms the draft adds, not from a clause-by-clause sweep of what they contradict.
- **Items:**
  - **(a) Wrong edit at §4.3.** "Challenged POST refusal for an unrecorded key" implies that a challenged POST for a recorded key is not refused. But §3(d) L183-185 and §9 still refuse it: a 401 to a credentialed POST ends `Authentication` and is never replayed. Correct wording: "challenged POST refusal (a challenge to a POST is never answered; a recorded key's POST carries its credential from its first byte)".
  - **(b) L564-565.** "Redirects or challenge retries are requested by the existing protocol profile/policy" is the clause the re-authentication GET contradicts: the GET is adapter-initiated, not requested by policy. The draft names only the idempotence phrase, and only as a Safety question (L244, L391-393), and §4.6's row does not list L564-565. The §4.4 rationale must also reconcile with draft §2 L97-98, which rejects a probe POST as "a Git-service decision for gwz-git, not the generic adapter". The answer is that the advertisement GET is bodiless and safe, and that the gwz-core adapter already owns the GitService-to-path table (`https_adapter.rs:9-11`, `exchange.rs:98-127`). The text has to say so.
  - **(c) §15 L881.** The "Open for Phase 3" sentence ("the proposed remedy keys 'this repository needed a credential' on the operation ... and keeps connection binding for NTLM/Negotiate only") describes option b1 and plain binding. It must be marked closed or superseded, alongside the POST-rule sentence.
  - **(d) Plan L724-728.** The descriptive sentence ("credential need recorded per operation ... NTLM/Negotiate connection-bound") goes stale. §4.7 changes only the step name.
  - **(e) §10.**
    - The four new finite defaults (512 keys, 64 credentials of 4 KiB, 256 affinity entries, 8 MiB re-authentication cap) are not added to §10's table, whose L689-691 says the table is where explicit bounded choices live.
    - The operator decisions of 2026-10-31 have no plan §10 decision entry. D-P and D-R are the precedent; the RemPlan alone records them.
  - **(f) Contract L486-487.** "A new auth framework is not introduced". The draft should state why an operation-held credential record is not one. One sentence suffices.
  - **(g) The §7 table replacement (§4.1).**
    - It silently drops "opaque" from "opaque request-scoped authorization".
    - "endpoint instances ... keep no credential" is literally false while openings pend: `instance.rs` `Pending { auth }` and `Work.auth` hold references to the provider. Say "own or retain no credential beyond the operation's `AuthContext` they carry for its openings and exchanges".
- **Impact:** after adoption the contract contradicts itself in five places, and later reviews will cite live text against the design.
- **Closure test:** §4.6 gains rows or sentences for (b) to (f), §4.3 uses the corrected wording, and a re-read finds no contract or plan sentence describing the slot, b1, or an unlisted bound.

### P3-2 — The insertion points and some code citations are wrong

- **Location:**
  - Draft §4.2 L285: "new paragraph after L557".
  - Draft §4.4 L304: "(L553-557)".
  - The contract paragraph actually runs L553-559, and L557 ends mid-sentence ("... generation; only equal").
- **Root cause:** the anchors are line numbers, not paragraph boundaries.
- **Impact:** an editor applying §4.2 splits a sentence and puts the Basic paragraph inside the NTLM paragraph. §4.2 and §4.4 target the same point in an undefined order.
- **Further discrepancies:**
  - L117 "trailing empty segment popped" holds for `Destination::request`/`selected_request` (`https_destination.rs:116-134`, `pop_if_empty`), not for `Destination::url`. A key hashed from `destination.url.path()` separates `/repo` from `/repo/`. This fails closed, but the key's definition must name the function.
  - L157 "`exchange.rs:122`" (`set_sensitive`) is at `:123`.
  - L112 cites contract L483-485 for "where a helper environment lives". L483-485 says the helper environment is not a profile field; the placement in `AuthContext` is the §15 1.5 row (L882).
  - L243 "the one request the adapter sends that the operation did not ask for" is inaccurate. The adapter already sends challenge answers: the Basic answer at `auth.rs:76-78` and up to eight native rounds. Say "the only request of a different service than its exchange's own".
- **Closure test:** anchors name paragraphs ("after the paragraph ending '...No cross-account downgrade to generic reuse is allowed.' (L559)") in a stated order (4.4, then 4.2). The key is "SHA-256 over host, port, the path of `destination.request(service)` without the service suffix, and class", or equivalent wording.

### P3-3 — The sub-step file lists do not match the edits each mechanism needs

- **Location:** the draft §5 table L334-337; draft §3.2 items 1-2 L265-269; RemPlan R3 "Owns" (L63); the 2.3 plan, 2.3b "Owns" (L94-96).
- **Root cause:** files are assigned per concept, not per edit.
- **Items:**
  - **(a) `auth.rs` in H1b.** "Challenge from record" and recording the Basic credential need `https_adapter/auth.rs`. `answer` obtains the secret by calling `lookup` and drops it inside (`auth.rs:69-80`). H1b does not list `auth.rs`; only H3 does.
  - **(b) H1b removes what H3 needs.** H1b removes `prior` and `authenticated` from `worker.rs:259-261` and `pipe.rs:131-132`. H3's check "if the leased connection's scope is `LogonSession`" needs exactly that field, or access to `Scopes`, and H3 lists neither `worker.rs` nor `pipe.rs`.
  - **(c) `connection_bound()` plumbing.** `connection_bound()` is called on the client half after `Reusable`, so the binding must travel from the worker to the pipe. `Settle` is `ssh_adapter::Settle` (`https_adapter.rs:46`), so either `ssh_adapter.rs`, outside R4's "Owns", or `pipe.rs` must change. H3 lists neither.
  - **(d) The caller move has two owners.** §3.2 item 2 assigns removing `HttpsInputs.native` and `Common.native`, plus the `native()` accessor, to the P2-2 fix (R3, "not counted in section 5"). Those are `https_adapter.rs`, `exchange.rs` and `https_auth.rs`, which RemPlan R3 does not own and R4 does. H3 also lists "caller from context". The security-relevant move therefore has two owners, and §3.2 itself says this move prevents cross-operation caller use (L268-269).
  - **(e) H2's list misses `exchange.rs`.** `Operation::open` and the `Request` construction are in `crates/gwz-connection/src/exchange.rs:98` and `:382-396`, not `operation.rs`. `Instance::settle(io, lease)` (`instance.rs:389`) carries no binding, connection or path, so `Settling` and `Exchange::finish` (`exchange.rs:627-647`) must change too.
  - **(f) The pin file.** "The gwz-core pin of gwz-transport moves with H2" collides with R1 ("the gwz-core pin", RemPlan L59) and 2.3b (`.github/gwz-transport.commit`, 2.3 plan L96). No serial order is stated.
- **Impact:** parallel lanes collide, and the caller move may be done twice or skipped.
- **Closure test:** the §5 table lists, per sub-step, every file each named edit touches; §3.2 names one owner (R3 or H3) for each caller edit; and the order states who moves the pin and when.

### P3-4 — The placement hint is recorded only when the advertisement exchange settles, so it misses the consumer's normal order of advertisement then POST

- **Location:**
  - Draft §3.1 L223-226: "`Instance::settle` records after a `Reusable` end with `connection_bound()`".
  - The prefer rule (L217-218): a preferred connection is taken only "if ... idle".
  - Row 1 (L248-249).
  - Compare §3(h) L198-203, which rejects end-of-exchange recording for the `Needs` record for this very reason.
- **Root cause:** an affinity written at settlement, plus a soft preference that ignores a leased connection.
- **Facts confirmed in code:**
  - `poll_finish` stays `Pending` until the worker's task has ended (`pipe.rs:201-211`).
  - The worker marks the connection Idle only after `pipe.settled()` and `sender.ready()` (`exchange.rs:291-299`, `worker.rs:392-414`).
  - The lease is released inside the driver's pump (`instance.rs:847-864`).
- **Sequence:**
  1. gwz-git finishes the receive-pack GET and opens the POST at once. libgit2 frees the previous smart stream when it requests the next one; I mark that as a hypothesis about the future gwz-git adapter.
  2. The POST's `Request` is built before the GET's settlement. There is no affinity entry yet, and the GET's connection is still leased.
  3. The pool picks another idle connection, or opens one.
  4. The fallback re-authenticates.
- **Impact:**
  - Decision 2's placement hint rarely engages.
  - Every Windows NTLM push pays a full second advertisement plus a three-leg conversation.
  - Row 1 passes only because fixture rows wait for idle (`case.wait("the connection is idle", ...)`, `https_auth_tests.rs:582-588`).
- **Required correction:** either:
  - record the affinity when the advertisement is accepted (the learning point, in the binding, keyed to the connection), and let the pool treat a preferred connection that is settling (finish requested, still leased) as worth a bounded wait up to the cleanup bound. This is distinct from a busy one (see §3 item 2);
  - or state that the hint covers only consumers that wait for release, and size the fallback cost as the expected path.
- **Closure test:** Row 1b: GET finish, then POST open with no wait for idle. The POST lands on the advertisement's connection, with no second GET, in 100 of 100 iterations.

### P3-5 — The record's transition rules are stated inconsistently, and one row cannot be satisfied

- **Location:**
  - §3(d) L187: "A key is recorded only after an authenticated 200 ... never at the first 401".
  - §3(d) L183: the parenthetical "(a GET answered from the record, or a POST)".
  - §4.2 L294-296: "A 401 to a request that carried a credential ... marks the key ... rejected", which includes a first helper-answered GET.
  - Row H1b-5 L352: "A credential rejected at the advertisement records nothing; the POST fails fast with no helper spawn, no request".
  - §3.1 L212: `ConnectionBound` recorded at `auth.rs:67`, which is before the classify and content-type checks of `exchange.rs:257-266`.
  - §3.1 L239-240: the re-authentication failure leaves the record's state unspecified.
- **Root cause:** there is no single transition table for `Need`.
- **Impact:**
  - Row H1b-5 is contradictory. With nothing recorded, the POST is "sent as today" (L240), which is a request. "Fails fast" needs a `Rejected` entry.
  - A `ConnectionBound` entry for a 403, 404 or wrong-content-type advertisement makes a later POST re-authenticate instead of going anonymous.
  - A refused re-authentication may repeat on every POST of the operation.
- **Closure test:** §3(d) gains a table with columns (event, prior state, new state, bytes sent). It covers:
  - the first helper answer, then 200, then 401;
  - an answer from the record;
  - a credentialed POST 401;
  - the native answer, then 200, then a non-200;
  - re-authentication refused, and re-authentication Io.
  Row H1b-5 is restated to match, and §4.2 and §4.4 agree with the table.

### P3-6 — The `closed` flag cannot be reached, and the clearing `Drop` is redundant

- **Location:** draft §3(b) L148-149: "`OperationAuth` also gets a `Drop` that clears the map and a `closed` flag that makes a straggling exchange's late insert a no-op. No hook is added to the host."; row H1a "closed flag" (L334).
- **Root cause:** a straggler that can insert holds a clone of the `Arc` provider, so `Drop` cannot have run. Nothing else sets the flag.
- **Impact:** dead code in a secret-handling module, and an H1a row that cannot fail. Dropping the map already drops every `Arc<Secret>`, and `Secret::drop` wipes (`secret.rs:118-122`).
- **Closure test:** remove both; or name the non-Drop event that sets `closed` (for example the binding's retirement in `host.rs` `retire_if_done`, which is a host hook the draft excludes) and give that event a row.

### P3-7 — The scope and process claims diverge from the operator-recorded remediation plan

- **Location:** draft L8-11, "Components the HTTPS model step may change (operator, 2026-10-31)", against:
  - RemPlan R4 (L64): "gwz-core `https_adapter*`, `https_auth*`; the gwz-transport pool placement API; contract §7 amendment";
  - draft §5 L329-330: about 1,500 lines in four sub-steps, against RemPlan L55: "one lane and one goal, aimed at 500 lines or fewer";
  - draft §5 L364-365: three review objects, including separate Code and State reviews of H1 and of H2/H3, against RemPlan L24 and L75 ("re-reviewed together on one settled tuple"; round 2).
- **Root cause:** the draft's boundary and review plan were not reconciled with the RemPlan row that records the operator's authorization.
- **Impact:** the attribution to the operator overstates the authorization:
  - all of `crates/gwz-connection` (ports, binding, instance, exchange);
  - contract §4.1, §8 and §15;
  - plan text.
  The review structure contradicts the lane owner's own process.
- **Closure test:** either the RemPlan R4 row is amended to H1a..H3 with their files, sizes and review objects, or the draft cites the RemPlan as authority and narrows L8-11 to match it. The attribution names its record.

### P3-8 — The trigger for the busy-wait decision has nothing to measure it

- **Location:**
  - Draft §6 item 2 L381: "trigger is the fallback count in the 3.4 sweep".
  - L391-392: "the sweep should count fallbacks".
  - §3(b) L161: "observations carry typed codes and counts only".
  - Contract §15 L882: "The crate has no observation surface for an event-level R4 row".
- **Root cause:** no H3 deliverable produces a fallback counter or fact.
- **Impact:** the deferred decision can never fire on evidence.
- **Closure test:** H3 names the surface. One option is a re-authentication flag in the exchange's facts, which become `Opened.facts`/`TransportObservation`. Another is a host counter under §11's saturating counters. Give it a row: N re-authentications produce a count of N.

### P3-9 — H1b's size estimate is not credible for its nine fixture-server rows

- **Location:** draft §5 L335 (H1b: rows +260, about 350 in total) and L343-359.
- **Root cause:** the row estimate is not based on the rows in the existing file.
- **Evidence:** the existing rows in `connection_host/https_auth_tests.rs` run 31 to 61 lines each (rows at L358, L371, L402, L437, L496, L557 and L612). Nine rows, including a 100-iteration concurrency row, a 513-key bounds row and a two-context cross-binding row, come to about 400-500 lines before any new handler. H1b is about 500-590 lines, and only H3 has a split contingency (L341).
- **Closure test:** re-estimate from the file. Add an H1b split contingency (for example rows 7 and 8 into H1c).

### P3-10 — The impacts on the documents the draft does not cite are left unstated

- **Location:** draft §3(b) L150-152 and L173-178, §3(f) L192-193, and §6 item 5.
- **Root cause:** the operation-scoped model is not traced into the consumer documents.
- **Items:**
  - **(a) Per-capture premise.** "Operation B has its own provider" and "ends with the operation" both assume one `capture_operation` per admitted operation. `AuthContext` is publicly `Clone` (`operation.rs:163-165`), and contract §10 L692-694 anticipates "Shared provider objects". Today gwz-py captures per call (`route/transport.rs:53-64`) and the served form captures per call (Split L162, D-P). Plan 3.1's dispatch, "one `HostOwner` per command", is unstated. If a command shares one capture across member operations, the record and poisoning span the command.
  - **(b) Served `cli`.** The re-authentication GET is a request inside an admitted `open`, which Split §5 (L313, L328-331) admits per `open`. The draft must say that the GET is covered by that open's admission: same tuple, and a receive-pack advertisement within the push allowance. It must also say what `Opened.facts` (`credential_offered`, `authenticated`, method; Split L188) report for a POST answered from the record and for a re-authenticated POST.
  - **(c) gwz-py (plan 3.2 and R2).**
    - A record per call means at least one helper run per private key per Python call.
    - Under b2 a credentialed request also marks a previously anonymous connection `HelperToken` (as `exchange.rs:227` does today), so mixed public/private workloads lose cross-call reuse.
    - The 3.4 sweep should expect both.
- **Closure test:**
  - A §3(b) sentence: "each admitted operation's provider comes from its own capture; a credential-retaining provider is never shared between admissions".
  - A gwz-core row: two operations admitted with clones of one capture's context share no record. Either they are refused at admission, or the record is bound to its first binding.
  - Sentences for (b) and (c) in §3(f) and §6.

### P3-11 — The re-authentication's deadline rule names a budget the adapter does not have

- **Location:**
  - Draft §3.1 L237-238: "the native deadline D and rounds are minted once from the remaining budget, never reset (contract L704-711)".
  - The code: `HttpsSettings` holds allocation, connect, io (stall), interaction and cleanup only (`https_adapter.rs:72-83`), and today's answer mints `D = now + connect_timeout_ms` per conversation (`auth.rs:48-52`).
- **Root cause:** the text borrows the contract's aggregate-budget wording. An HTTPS exchange has only a stall bound (contract §10 L682: the aggregate is for setup).
- **Impact:** the statement cannot be implemented as written, and each implementer will pick a different budget.
- **Closure test:** state "D is minted once for the re-authentication conversation as the existing answer mints it (`connect_timeout_ms`, refusing at 0), each request within the stall bound". Add a fake-clock row: a re-authentication whose rounds exceed D ends `Timeout` with `Effect::None`.

## 2. Invariant analysis

| Invariant (source) | Holds in the draft? | Note |
|---|---|---|
| The core holds no credential; the carrier carries none (plan §1 L55-62, R7) | Yes | The provider is client-side only (plan L72-74). A served `Open` with a non-empty `auth` is refused (Split L150) |
| A credential goes only to the learned host, port, path and class (decision 4, plan §1) | Yes | The key comes from a server challenge to an admitted exchange. Redirects are not followed (`exchange.rs:260`) |
| No replay of a mutating body (§9 L660-666) | Yes for the POST | The re-authentication GET needs the §9 effect statement (P2-1) |
| Per-binding authorization of authenticated connections (§7.2, `authorize_reuse`) | Yes, unchanged | More connections become `HelperToken` under b2 (P3-10(c)) |
| Operation isolation (R4, plan L169) | Yes, given one capture per admission | The premise is unstated (P3-10(a)) |
| Bounded retained charge (§10 L686, L692) | Satisfiable | About 280 KiB per admission. 72 admissions (8 active plus 64 waiting) come to about 20 MiB plus environments, under 64 MiB per host. The overflow behaviour is wrong (P2-3) |
| Zeroization at rejection (draft §4.2) | No | Lazy poisoning keeps the secret in memory (P2-2) |
| Deadlines never reset (§10 L704-711) | Intended | The budget is misnamed (P3-11) |
| FIFO fairness among eligible requests (§8 L606) | Yes | The hint is soft and scanned in FIFO order. An earlier non-preferring waiter may take the preferred connection, which is acceptable |
| Git semantics stay in gwz-git (§7.2 L536-539) | Pre-existing exception widened | The adapter already maps GitService to paths. The re-authentication GET adds an adapter-chosen request (P3-1(b)) |
| A record is visible before any advertisement byte (draft §3(h)) | Yes | The write happens after `exchange.rs:264` and before `:269`. The affinity table does not get the same treatment (P3-4) |
| Dependency edges (§3 L66-71: gwz-connection depends only on gwz-transport) | At risk | "SHA-256 of spec path" in the affinity table (L223) needs a hash. `crates/gwz-connection/Cargo.toml` has only `cfg-if` and `gwz-transport`. Key by the bounded path string instead (no secret is involved), or state the dependency. This sits under P3-3 and needs no separate ID |

## 3. Judgement on the draft's open items (§6), each with a recommendation

1. **`approve`/`reject`, and pre-authenticated advertisements: out.** Agree. Both are side effects or guesses beyond PR5, PR6 and Phase 3 concurrency. Neither is needed by any decision. Record them as named later rows in §15 (the draft does, at L322).
2. **The busy-wait.** Agree that there should be no wait on a busy preferred connection. A long exchange holding it must not hold a POST. A bounded wait on a settling preferred connection (finish requested, `Reusable` pending, bounded by the cleanup bound of 5 s) is a different case, and it is the one the real consumer hits (P3-4). Recommend: build the settling wait, keep "busy" out, and make the trigger measurable (P3-8).
3. **The bounds.**
   - 512 keys: fine (R1 needs 32).
   - 64 credentials of 4 KiB: fine. A Basic header over 8 KiB is commonly refused, and the helper output limit is 16 KiB.
   - 256 affinity entries: fine.
   - 8 MiB re-authentication body cap: acceptable; it fails closed.
   - Accept the numbers, but put them in contract §10's table (P3-1(e)), and fix the overflow behaviour (P2-3).
4. **Caller identity in `AuthScope`.** Not a prerequisite here, because:
   - `authorize_reuse` refuses `LogonSession` connections across bindings (`https_adapter.rs:308-325`);
   - the affinity table is per binding.
   Contract L556-557 binds the scope tag to the "original caller identity". Once R3 moves the caller into `OperationAuth`, folding it into the scope digest (`https_auth.rs:58-78`) is cheap fail-closed partitioning. Recommend that R3 does it, with a row, so that item 5 can never weaken it.
5. **Sharing helper-token connections across equal-scope bindings.** Reject for 1.8, as the draft says. The legacy route deliberately scoped Basic connections to the credential (`https_worker/credentials.rs:5-16`, `retire_https_scope`), which is evidence that someone judged it necessary. Re-open it only with evidence from the 3.4 sweep and a Safety decision.
6. **The re-authentication GET as an exception to L565.** Acceptable in substance:
   - the GET is bodiless;
   - it repeats an advertisement this operation already had accepted;
   - it runs within the exchange's own deadline;
   - it is the only route to correctness under decision 2 without holding a lease, which the host model does not do.
   Accept it only with the correct clause named (L564-565, "requested by the existing protocol profile/policy"), a §15 row, the §9 effect statement (P2-1), the `cli` admission sentence (P3-10(b)), and the reconciliation with the rejected probe POST (P3-1(b)).

## 4. Risks and next action

- **Highest risk.** The contract amendment is adopted as written, and R2a (Effect) and R4 then implement opposite readings of §9 (P2-1). The rejection-wipe claim then ships false (P2-2).
- **Second risk.** On Windows NTLM pushes the hint is dead weight and the fallback becomes the normal path (P3-4), unmeasured (P3-8).
- **Next action for the lane owner:**
  - Revise to r3, resolving P2-1, P2-2 and P2-3.
  - Fold the P3 corrections into §3(d) (the transition table), §4 (the anchors and the complete amendment list) and §5 (per-edit file lists, the pin order, the H1b estimate).
  - Reconcile RemPlan R4 (P3-7).
  - Re-file r3 to the same two axes. My pre-commitment stands for a revision that resolves the three P2s as specified and dispositions every P3.

## 5. Out-of-scope observations

- `ExchangeSpec` carries `GitService` in code (`exchange.rs:16`, `operation.rs:297`), while contract §7.2 L539-541 still says that the generic API does not expose GitService and that `ExchangeSpec` is `Http { origin, method, target, headers, auth }`. This is pre-existing; the §15 1.3b row (L880) records the step, not the textual divergence. Impact: readers of §7.2 get a wrong shape. Suggest a 1.3b-row sentence. It does not block this object.
- `AuthContext` derives a public `Clone` (`operation.rs:163`). Nothing prevents a consumer from sharing one provider across admissions, which the contract's charge rule (L692-694) even anticipates. This is a limitation, relevant to P3-10(a), not a defect today.
- The p1c-https Code review's "scope drift" note (history `GwzP1cHttps-ReviewCode.md:54`) and this draft's broader tainting under b2 (P3-10(c)) both widen which connections become per-binding. The 3.4 sweep should count `Unprovable` verdicts for HTTPS. This is an optimization opportunity, not a blocker.
- An untracked `dev-docs/GwzHttpsAuthModel-ReviewSafety.md` appeared in the root working tree during this review. I did not open it.
