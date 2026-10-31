# HTTPS authentication model (DRAFT r3): Safety-axis re-verdict, round 2

**Review object:** dev-docs/GwzHttpsAuthModel-DRAFT.md revision 3 at root 5de7d71b (593 lines). Code is still at gwz-core a7b965c5 and gwz-transport 0a253639.

**Inputs:**
- `dev-docs/GwzHttpsAuthModel-RemPlan.md` at 5de7d71b;
- my round-1 report;
- the restated R3 and R4 rows of `GwzPhase1-RemPlan.md` in the same commit.

**Baseline:**
- Every document was read with `git show 5de7d71b:<path>`.
- Code was re-read with `git show` at the pinned member SHAs.
- `git diff 5de7d71b HEAD -- dev-docs/GwzHttpsAuthModel-DRAFT.md` was empty at the start and at the end (HEAD = 5de7d71b both times).
- Nothing was written, built or run.

**Date:** 2026-10-31

**Axis:** Safety, round 2. Independent, adversarial, read-only. Filed verbatim by the lane owner.

**Verdict: NO-GO.** 0 P0, 0 P1, 1 P2, 4 P3.
- All ten round-1 findings are closed.
- The one P2 is the U7 regression that r3's own fix for P2-2 introduces. The drafter found it and left it unresolved.
- **I pre-commit to GO on r4 if:**
  - R2-P2-1 is resolved as its correction states (the drafter's U7 refinement, made precise below);
  - R2-P3-1 to R2-P3-4 are dispositioned;
  - nothing else changes beyond those.

## Prior-finding closure table

| ID | Status | Evidence in r3 (re-run sequence) |
|---|---|---|
| P2-1 (scope-record writes) | **Closed** | W1-W4 (L260-266). Sequence 1 re-run: the re-authentication reports `LogonSession` in `Ended` (L346-348, W1). The connection is recorded before it can idle, so binding B's later lease is `Unprovable`. H3c-5 is red under the mutation "re-authentication reports `Anonymous`". Sequence 2: W3 means the record only rises and `LogonSession` is sticky (H3c-6). Sequence 3: `prior` stays (W4, H1b file list L455). |
| P2-2 (learning rule) | **Closed, with a regression → R2-P2-1** | G2 (L224) and Learning (L294-299) learn `ConnectionBound` for a key served unchallenged on a `LogonSession` connection. `connection_bound()` follows the scope after the exchange (L301-304). Rows H3a-2 and H3d-1 cover the /a, /b sequence. The regression for a public key is U7 (L589-593). |
| P2-3 (provider lifetime) | **Closed; residual accepted (U3)** | Single admission (L168-172, A8, H1e). Every holder is listed with its bound (L177-183). The `closed` flag and the clearing `Drop` are gone. Retained native cleanup clones `NativeCaller`, never the context (L183). Rows H1e 1-5. Sequence 1 (one capture fanned out to N admissions) is now refused `InvalidInput`. Sequence 2 (a cached capture) is refused at the second admission. The residual is a clone kept past the end of the operation (U3); judged below. |
| P2-4 (overflow) | **Closed** | The bounds table (L153-163) ends the *advertisement* with `Capacity` before any body byte, so an overflowing key never produces an anonymous POST. The per-credential bound is 16 KiB and the total is 256 KiB. P8 refuses a POST for an `Unretained` key before any byte. Rows H1d 1-4, including a 5 KiB credential. `ErrorCode::Capacity` exists (wire 5, `gwz-transport/src/protocol.rs:247,263`). |
| P3-1 (rejection) | **Closed** | Rejection is eager under the lock and scoped to the origin (L196-202). Fingerprints are an HMAC under a per-operation key. G6 and G7 state the fresh-lookup rule. Old row 5's contradiction is replaced by H1c-2. Rows H1a 4-6, H1c 2-6. |
| P3-2 (branches) | **Closed; minor gaps → R2-P3-1** | The `Need` table (L221-256): RA2 (no challenge), RA3 (Basic only), RA4, P6 (POST 401 after re-authentication), and the G8 kind change. Rows H3b-3, H3b-4, H3c-1, H3c-2. |
| P3-3 (key) | **Closed** | 3(a) L131-141 keys over the full pool `Key` (scheme, username, host, port), the path and the class, plus a separate origin digest. Row H1a-1. |
| P3-4 (charge) | **Closed** | A formula that is an upper bound (L164-167). H1a-8 asserts it is at least the byte sum with every bound full. |
| P3-5 (residuals) | **Closed** | A1 (L392-398) names engine and TLS buffers, process dumps and swap. Retiring idle per-binding connections is recorded as not built (A11, §6). |
| P3-6 (re-authentication conditions) | **Closed** | Conditions 1-5 (L321-335), A4 and A6. The body cap is max(8 MiB, 1.25 x the observed length). Rows H3d-2 and H3d-3. |

## Changed-range analysis

**No new architectural root cause.** Single admission is a remedy for P2-3, not a defect class. It is one generic claim on `AuthContext` plus `AdmissionError::InvalidInput`, and the H1e row keeps plain providers shareable.

**The 30-row transition table (L221-256).** I walked every row for one property: wherever authentication is not established, the exchange ends before a POST byte, and no credential goes to a key that did not challenge.
- It holds for G4-G7, G11, G14, P2, P5 to RA1, RA2-RA7, P7 and P8.
- P3 (unrecorded POST) sends no credential.
- P4 carries only the binding's Windows identity, which the connection already has (recorded in §6).
- G7 and H1c-5 close the "re-send after rejection" path.
- `Rejected` is terminal and wins over late inserts, which is the account-lockout trade-off I asked for.
- The gaps are small and listed in R2-P3-1. The U7 interaction of G2 with RA2 is R2-P2-1.

**The bounded settling wait (L306-319, A10, H2a).**
- It stays within one binding: the affinity table is binding state, and the lease still passes `authorize_reuse` (`instance.rs:601-630`).
- The table records only `connection_bound()` connections, which are `LogonSession` and therefore this binding's only.
- During the wait the request opens and evicts nothing, the wait is capped by the request's own deadline (H2a-7), and FIFO is kept (H2a-9).
- No deadlock: the settling exchange is driven by the host driver, and its cleanup bound (5 s) ends it `Discarded` at worst.
- **But** the lapse of the wait is enforced only if the driver wakes for it. That is exactly the Code P2-1 defect (caller pumps do not re-arm the driver), which R1 fixes → R2-P3-2.

**The eleven-sub-step split (L452-475).** Lane A lands H1b (retention) before H1c (eager rejection and fingerprints), H1d (overflow) and H1e's gwz-core part (single admission). A tree in between retains credentials without the round-1 safety corrections. It has no production caller until 3.1 (contract L884), so nothing is exposed, but the text does not forbid consuming such a tree → R2-P3-3.

**Scope provenance.**
- The (+) items (H1e, H2b, the contract sections) trace to my P2-3 and the Consistency findings, which are demonstrated necessity for operator decision 1 and the RemPlan dispositions.
- The R4 row was restated by the lane owner in the same commit (`GwzPhase1-RemPlan.md` R4).
- No (+) item conflicts with an operator decision. Whether the row amendment needs the operator's confirmation is the lane owner's call (U1). This is recorded, not a finding.

**Re-checked code facts r3 relies on:**
- `Scopes::bound` is last-writer-wins today (`https_adapter.rs:162-179`); W3 changes it.
- `worker.rs:405`, then `:417`, are the write-then-idle order.
- `AdmissionError` has `InputBudget` (`host.rs:529,538`) but not `InvalidInput`, as A9 states.
- `Pending.auth` is at `instance.rs:77` and `ConnectCommand.auth` at `ports.rs:143`, as the holder table states.

## Findings

### R2-P2-1: A key learned by G2 (unchallenged on a `LogonSession` connection) is made terminally `Rejected` by RA2, so a public repository's POST on any other connection fails where today it succeeds

- **Root cause.** G2 records `ConnectionBound` without recording *why* (L224, L294-298). RA2 (L247) then treats an unchallenged re-authentication GET as proof of server misbehaviour and moves the key to terminal `Rejected` (L254). For a G2-learned key, an unchallenged GET is the expected answer when the repository is public.
- **Location:**
  - draft §3(d) G2 L224, RA2 L247 and the `Rejected` terminal rule L254;
  - §3.1 Learning L294-298;
  - U7 L589-593;
  - A4 L417-426: "A re-authentication that does not authenticate the connection ends the exchange before any POST byte".
- **Violated invariant:**
  - Code P2-1 of p1c-https, kept by 3(a) L137-138: a request the server serves to anyone must not depend on authentication;
  - parity with a7b965c5, where an unrecorded key's POST goes anonymously and works (`https_auth_tests.rs:558`, kept green by H1b-5).
- **Sequence** (one binding; IIS-style server with Windows authentication on /priv and anonymous access on /pub):
  1. GET /priv on c1: 401, NTLM, 200, so c1 is `LogonSession` (G8).
  2. GET /pub on c1 gets 200 with no challenge, so /pub is learned `ConnectionBound` (G2).
  3. POST /pub is placed on c2 (c1 busy, or a hint miss).
  4. P5 runs the re-authentication GET on c2: 200 with no challenge (RA2). /pub becomes `Rejected` and the POST ends `Authentication`.
  5. Every later POST of /pub in the operation, on c1 too, ends at P7 with no request.
- **Impact.** A fail-closed but terminal regression for mixed public and private repositories on one connection-bound server. It is the R1 shape on such servers, and the error reads "Authentication" for a repository that needs none.
- **Required text correction** (the drafter's refinement, made precise):
  - `ConnectionBound { observed, evidence }` with `evidence = Challenged` (G8) or `OnBound` (G2).
  - **RA2 with `OnBound`:** the POST is sent anonymously on that connection after the GET is drained, exactly like P3. A 401 then ends it `Authentication`, never answered and never replayed. The key stays `ConnectionBound { OnBound }`, not `Rejected`.
  - **RA2 with `Challenged`:** `Rejected`, as now.
  - **RA3 and RA4:** unchanged (`Rejected`).
  - Amend A4's sentence to "...ends the exchange before any POST byte, except that a key learned only on a bound connection whose re-authentication GET is served unchallenged sends its POST as an unrecorded key's".
  - Safety check of the refinement: the POST carries no credential and goes to the key's own path, after one bodiless GET the conditions already allow. It sends nothing an unrecorded key does not send today.
- **Closure rows:**
  - The mixed fixture above: POST /pub forced onto c2 meets RA2 with `OnBound`, the POST goes anonymously and succeeds, and the key is not `Rejected`.
  - H3b-4 is relabelled to `Challenged` evidence and stays as is.
  - A mutation that makes `OnBound` behave as `Challenged` turns the first row red.

### R2-P3-1: Small gaps in the transition table

- **Location:** §3(d), L221-256.
- **Gaps:**
  - (a) RA7 lists only 403, 404 and 5xx. A 3xx or a wrong content type on the re-authentication GET is unlisted. G13 covers these for an ordinary GET.
  - (b) G3 and G1 do not name an `Unretained` prior state.
  - (c) G8's "a replaced `Basic` credential is wiped" is imprecise. The entry drops its reference, and the bytes are wiped only when no other key of the origin holds them. That is consistent with the shared `Arc` of L148-149, but the sentence reads as an immediate wipe.
  - (d) G5 does not state precedence when a challenge offers both Negotiate and Basic for a `Basic`-recorded key. The code prefers the logon session (`auth.rs:41-68`), which makes it G8.
- **Consequence.** An implementer must guess these branches. Each plausible guess fails closed except (c), which only overstates the wipe.
- **Correction.**
  - RA7: "another status or content type: `classify`/`Protocol`, `Effect::None`".
  - Add `Unretained` to G3 ("unchanged").
  - Reword G8: "the entry's reference drops; the bytes are wiped when no key of the origin holds them".
  - G5: "when the logon session is also offered and the native route is available, G8 applies".
- **Closure.** A re-read of the table. Optionally, a row for a re-authentication GET answered with a 3xx: `Effect::None`, zero POST bytes.

### R2-P3-2: The settling wait's lapse depends on R1's driver re-arm, but H2a names R1 only in the pin order

- **Location:** §3.1 Placement L312-317; §5 Order L467-472; H2a-6.
- **Root cause.** The wait ends at `prefer_wait_ms` only when the driver wakes at `next_deadline`. Before R1, caller-driven pumps do not re-arm the driver (Phase 1 Code P2-1). A quiet driver could leave a waiting POST parked past its 5 s wait until its 30 s allocation deadline, or longer, since that deadline is not enforced either.
- **Consequence.** The new bounded wait inherits an unbounded wait on exactly the paths Code P2-1 named.
- **Correction:**
  - make H2a's dependency on R1's re-arm explicit;
  - H2a-6 runs with the driver parked first (the PROBE-ALLOC shape) and asserts the lapse at its bound.
- **Closure.** That row, red on 0a253639.

### R2-P3-3: The text does not forbid a consumer of a tree between H1b and H1c, H1d and H1e

- **Location:** §5 Order, L468-472.
- **Root cause.** H1b lands retention before eager rejection and fingerprints (H1c), overflow handling (H1d) and single admission (H1e's gwz-core part).
- **Consequence.** A tree at H1b keeps a credential the server has 401'd and sends it on each later POST of the key: the "sent after a rejection" and lockout case P3-1 closed.
- **Correction.** One sentence: "No production caller, release tag or 3.1 lane consumes gwz-core between H1b and the completion of H1c, H1d and H1e; the R4 group is adopted whole." Alternatively, move "a 401 to a credentialed request drops the entry" into H1b.
- **Closure.** The sentence.

### R2-P3-4: Single admission forces one fresh capture per bind on the served `cli` path, which A12 states only for 3.1 and 3.2

- **Location:** §3(b) Lifetime L168-175; §3(f) L271-279; A12 L436-443; plan L72-74 ("the client's owner builds the `PreparedOperation` ... from its own invocation and capture").
- **Root cause.** The claim turns a composition shape into a hard `InvalidInput`. A client owner that captures once per invocation and admits one operation per core bind refuses its second bind.
- **Consequence.** A served command with more than one operation fails at admission. This is fail-closed and typed, but it is a functional break rather than a safety one, and nothing in r3 tells the served path's owner.
- **Correction.** A12 also states: "the client owner of a served core builds a fresh capture (a fresh `OperationAuth`) per bind." The Phase 2 served path is named as a consumer of the rule.
- **Closure.** The sentence. Optionally, a row in which two binds in one served invocation both register.

## Judgement on U1-U7 and the caller-identity dissent

- **U1 (R4's size, eleven sub-steps).** No safety objection to the split. Each sub-step is a coherent goal, provided R2-P3-3's group-adoption sentence is added. The size growth is honest, sized from real rows.
- **U2 (single admission against clones). Accept H1e as designed.** Of the three options it is the only mechanical guarantee:
  - option (B), a per-capture convention, leaves the guarantee to discipline;
  - the guard-and-`Weak` option needs an end-of-operation hook the crate does not have.

  H1e costs about 60 product lines, keeps plain providers shareable, and frees the claim on every never-registered path, including a revoked permit's `NeverRegistered`. One addition: H1e-1 should also assert that a second admission *while the first is still waiting* is refused, and that a `QueueFull` on the first frees the claim.
- **U3 (a clone kept past the operation). Accept as a stated residual.** With single admission, the only remaining holder past the end is the composition's own capture: gwz code under the lane's control, not a library path. A12 binds 3.1 and 3.2 to drop it. Recommend that 3.1 carry a row in the CLI composition: after the command's operation finishes, the provider's strong count is 0 (the H1e-3 shape, at the composition).
- **U4 (H3c-2 wording). Agree with the drafter.** The first POST's body was sent by definition (P6, `PossiblySent`). The zero-byte assertion is for the next POST of the key.
- **U5 (the counter's route to `TransportObservation`). Agree:** composition work for 3.1/3.4. The counter is non-secret.
- **U6 (R3 owning the caller move). Agree.** One owner, which the restated R3 row records. H3a-H3d consume only `native()`. The ordering R3 → R4 is load-bearing for safety: without it the connection-bound path is the one place an operation could authenticate as another's caller (L362-363).
- **U7 (a public key learned on a bound connection).** A real regression introduced by r3's fix for my P2-2. The drafter's refinement is correct and safe as specified in R2-P2-1: it sends no credential and nothing an unrecorded key does not already send. **Adopt it**; this is the P2 above.
- **Caller-identity dissent (3.2 item 3; §6 item 4). I withdraw my round-1 objection, conditionally.**
  - The cost I named, partitioning anonymous connections that carry no identity, is close to zero in practice: one process almost always has one caller. The gain is defence in depth for any future relaxation of `authorize_reuse`.
  - **Condition:** when the capture cannot read the caller's identity, the scope must take a value unique to that capture, so that it shares with nothing. It must never take a constant that would merge distinct callers.
  - Only a non-secret identifier (a digest of SID, LUID or logon session) enters the digest. R3's row should include the unreadable-identity case.

## Risks

**Origin-wide rejection** (L196-202, G6, accepted trade-off). This is a **hypothesis** about server behaviour: some servers answer 401 to a valid credential that lacks access to one repository. On those, one member's 401 rejects every key of that origin holding the same credential, and the fingerprint blocks the same bytes for the rest of the operation, so a 16-member command can fail on one inaccessible member. This is the lockout-protection choice I asked for in round 1. Measure it in the 3.4 sweep.

**Accepted in r3, not re-raised:**
- token expiry inside a long operation (`password_expiry_utc` ignored, L558-560);
- the hint's efficacy until gwz-git has a production caller (L555-557);
- per-request Kerberos servers failing closed (L552-554).

**The budget code** is `ErrorCode::Capacity`, which other capacity refusals also use. The "never `Authentication`" goal is met, but a reader cannot tell the credential record's bound from a mux or pool limit without facts. H1d's `stream_error` arm should carry a distinct message.

**Next action.** r4 needs:
- R2-P2-1: the U7 refinement, precisely;
- the R2-P3-1 table edits;
- the R2-P3-2 dependency and parked-driver row;
- the R2-P3-3 and R2-P3-4 sentences;
- the U2 waiting-admission row.

The re-review need only re-run the R2-P2-1 sequence and re-read those edits. The pre-commitment above stands.
