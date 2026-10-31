# HTTPS authentication model (DRAFT r4): Safety-axis re-verdict, round 3

**Review object:** dev-docs/GwzHttpsAuthModel-DRAFT.md revision 4 at root 56186ad1 (637 lines). Code is still at gwz-core a7b965c5 and gwz-transport 0a253639.

**Inputs:**
- the round-2 report (`GwzHttpsAuthModel-ReviewSafety-2.md`);
- the r3→r4 changes table (L38-55);
- the R3 and R4 rows of `GwzPhase1-RemPlan.md`, amended in the same commit.

**Baseline:**
- The draft was read with `git show 56186ad1:<path>`.
- The changed range was read as `git diff 5de7d71b 56186ad1` over the draft and `GwzPhase1-RemPlan.md`.
- I re-checked one code claim at a7b965c5: `setup_retry/limit/capacity.rs:181-187`, where a `Capacity` failure without a detail has no worded report.
- `git diff 56186ad1 HEAD -- dev-docs/GwzHttpsAuthModel-DRAFT.md` was empty at the start and at the end (HEAD = 56186ad1).
- Nothing was written, built or run.

**Date:** 2026-10-31

**Axis:** Safety, round 3, limited to non-architectural text corrections under the cap. Independent, adversarial, read-only. Filed verbatim by the lane owner.

**Verdict: GO.** 0 P0, 0 P1, 0 P2, 4 P3.
- R2-P2-1 is closed, and every round-2 P3 and condition is closed.
- The four new P3s are bounded, fail-closed text corrections in the newly added text. None is architectural.
- The lane owner should fold them into the acceptance edit or record them with their consequences. They do not hold the verdict.

## Prior-finding closure table

| ID | Status | Evidence in r4 |
|---|---|---|
| R2-P2-1 (U7) | **Closed** | See "R2-P2-1 sequence re-run" below. |
| R2-P3-1 (table gaps) | **Closed** | G3 now names `Unretained`. G5 states the precedence (G8 when the logon session is also offered and the native route is available). G8's wording is corrected: the reference drops, and the bytes are wiped when no key of the origin holds them. RA7 covers "another status (3xx included) or content type" with `Effect::None`. Row H3c-3 adds the 3xx case. |
| R2-P3-2 (wait lapse depends on R1) | **Closed** | 3.1 Placement (L349): "H2a depends on R1". The Order section names R1 and R1b. H2a-6 runs "with the driver parked first (the PROBE-ALLOC shape; red on `0a253639`)". |
| R2-P3-3 (intermediate trees) | **Closed** | §5 Order: "No production caller, release tag or 3.1 lane consumes gwz-core between H1b and the completion of H1c, H1d and H1e; the R4 group is adopted whole." |
| R2-P3-4 (served path) | **Closed** | A12: "the client owner of a served core builds a fresh capture (a fresh `OperationAuth`) per bind". 3(b) Lifetime names the Phase 2 served path. Row H1e-7 has two binds, two captures, both registering. |
| Round-2 U2 addition (waiting admission) | **Closed** | 3(b) Lifetime and A8: "while one admission holds or has consumed the claim, `admit` of any clone is refused `InvalidInput` (`register` stays infallible)". H1e-1 covers "after the first registered or while it is still waiting". H1e-2 covers a `QueueFull` that frees the claim. H1e-4 covers concurrent admissions. |
| Caller-identity condition | **Closed** | 3.2 item 3: only a non-secret identifier digest enters, and an unreadable identity gives a scope "unique to that capture, never a constant", with a row. The R3 row in `GwzPhase1-RemPlan.md` carries the same text. §6 item 4 records the withdrawn objection. |
| Budget-code message (round-2 risk) | **Closed** | 3(b): `io::ErrorKind::QuotaExceeded`, with the message "operation credential record budget exceeded". A9 defines what a `Capacity` without a detail means on an HTTPS exchange. H1d-1 asserts the kind and wording. The cited gwz-core reader (`capacity.rs:181-187`) gives a detail-less `Capacity` no worded report, so nothing words it as the pool's limit. |

**R2-P2-1 sequence re-run.** One binding; a server with Windows authentication on /priv and anonymous access on /pub.
1. GET /priv on c1: 401, NTLM, 200. The key is recorded `ConnectionBound { Challenged }` (G8).
2. GET /pub on c1 gets 200 with no challenge, so it is recorded `ConnectionBound { OnBound }` (G2).
3. POST /pub is forced onto c2. Its re-authentication GET is served without a challenge, which is RA2a (L273). The GET is drained, then the POST goes anonymously, as P3 does.
4. The POST succeeds, and the entry stays unchanged, not `Rejected`.

The same answer for a `Challenged` key is still `Rejected` (RA2, L272). The other parts of the fix:
- A4 carries the exception sentence, and A11 records it.
- Row H3b-5 has the mutation "`OnBound` treated as `Challenged`".
- The safety check holds: the RA2a POST carries no credential, goes to the key's own path and follows one bodiless GET the conditions already allow. Its scope stays unchanged, because no native conversation completed, so W1 writes nothing.

## Changed-range analysis

**No new root cause is architectural.** The changed range is:
- the `evidence` field;
- RA2 and RA2a;
- rows X1-X3;
- the budget-code message and A9;
- the single-admission wording;
- the R1 dependency, the group-adoption sentence and the pin rule;
- the served-path and 3.4 counter sentences in A12;
- the caller-identity condition;
- the size and file-list updates.

I walked every new row for one property: wherever authentication is not established, the exchange ends before any POST byte, and no credential goes to a key that did not challenge.
- RA2, RA2a and RA7 hold.
- X1-X3 hold (they fail closed), but they are overbroad and incomplete (R3-P3-2 and R3-P3-3).
- RA1 on an `OnBound` entry stays `OnBound`. That is safe but keeps the anonymous fallback on a key now proven to need authentication (R3-P3-1).
- The proposed counter route has no bound (R3-P3-4).
- Single admission's new wording ("holds or has consumed") is consistent with an infallible `register`. Both race outcomes are rowed (H1e-1, H1e-4).
- The pin rule has no safety bearing.

## Findings

### R3-P3-1: RA1 on an `OnBound` entry leaves `evidence = OnBound`, although the re-authentication GET was just challenged

- **Location:**
  - §3(d) RA1, L271: "`ConnectionBound` | same; connection scope raised to `LogonSession`";
  - compare G8, L264, and the rule at L276: "A kind or evidence change (G8) replaces".
- **Root cause.** The evidence upgrade is defined for the GET event G8 but not for its re-authentication analogue RA1.
- **Sequence:**
  1. /k is learned `OnBound` on c1.
  2. POST /k lands on c2. The re-authentication GET is challenged, the logon session answers (RA1), and the POST succeeds. /k stays `OnBound`.
  3. Later, POST /k lands on c3, where a backend that misbehaves serves the re-authentication GET without a challenge.
  4. That is RA2a rather than RA2: the POST body goes anonymously, gets a 401, and ends `Authentication` with `PossiblySent`. The entry is unchanged, so the same thing can happen on each later miss.
- **Impact.** No credential is exposed, but a key proven to need authentication keeps the anonymous fallback. Each such miss spends a POST body with `PossiblySent`, where RA2 would refuse with `Effect::None` and zero POST bytes.
- **Correction.** RA1: "`ConnectionBound { any }` → `ConnectionBound { Challenged }`; connection scope raised to `LogonSession`."
- **Closure.** One clause in H3b-5: after an RA1 on an `OnBound` key, a later unchallenged re-authentication is RA2, with zero POST bytes.

### R3-P3-2: The `Exhausted` rows run a helper only to discard its result, leave the native path unspecified, and misreport the 401 that triggers them

- **Location:**
  - §3(d) X1 and X2, L279-280;
  - the bounds table row "Rejection fingerprints".
- **Root cause.** The `Exhausted` state is defined as "refuse results" rather than "make no lookups and no writes".
- **Consequences:**
  - (a) X2 says "a fresh lookup's result is refused", which implies the helper is still spawned. An interactive helper (a GCM prompt) would then ask the user for a credential that gwz discards, and a fresh `Secret` is fetched for nothing.
  - (b) G2 and G8 under `Exhausted` are unstated. Native tokens could be sent and a `ConnectionBound` entry written into a cleared record. This still fails closed, because X3 refuses the POST, but the text is incomplete.
  - (c) X1's "the exchange in hand ends with the budget code" applies to the 401 that produced the 64th fingerprint. That exchange should end `Authentication`, the actual verdict, and only later exchanges should get `Capacity`.
- **Correction:**
  - X2: "no answer from the record and **no lookup is started** (no helper spawn): the budget code before any answer byte."
  - Add X4: "G2, G8 under `Exhausted`: the GET is not answered by the logon session and records nothing; a challenged GET ends with the budget code."
  - X1: "the exchange that caused it ends with its own code; later exchanges see `Exhausted`."
- **Closure.** H1d-5 gains three assertions:
  - zero helper spawns after `Exhausted`;
  - no `ConnectionBound` entry is written;
  - the 64th rejecting exchange ends `Authentication`.

### R3-P3-3: X3 refuses unrecorded (anonymous) POSTs, so after `Exhausted` even public fetches and pushes fail

- **Location:**
  - §3(d) X3, L281: "P1, P3, P4, P5: any POST … nothing: the budget code";
  - X1 "entries cleared";
  - H1d-5.
- **Root cause.** Clearing the entries erases the difference between a key the operation learned and one it never learned. X3 then refuses P3, the anonymous POST of a never-learned key, to avoid an anonymous POST for a formerly `Basic` key. This violates Code P2-1 of p1c-https, kept by 3(a): a request the server serves to anyone must not depend on authentication.
- **Sequence.** `getrandom` fails when the record is created, so the record is `Exhausted` from the start (X1). Then:
  1. A fetch of a fully public repository sends its advertisement anonymously, and it is served (G1, unaffected).
  2. Its upload-pack POST, which needs no credential, is refused with `Capacity` (X3).
  3. Every HTTPS fetch in the operation fails.

  The same happens to every public POST after the 64th rejection, or after a poisoned lock.
- **Impact.** Fail-closed and typed, and rare: `getrandom` failure is exotic on the supported platforms, and the other triggers need 64 rejections or a bug. When it happens it is whole-operation, which is out of proportion to any safety gain, since the anonymous POST carries nothing.
- **Correction.** X1 replaces each entry with a secret-free tombstone (the key table is already bounded at 512) instead of clearing it. X3 then becomes: "P1, P4, P5 and any POST of a tombstoned key: the budget code; P3 (a key never learned) is sent anonymously as usual."
- **Closure.** H1d-5 gains two assertions:
  - with `Exhausted` from creation, a public fetch succeeds;
  - after `Exhausted`, a formerly `Basic` key's POST sends no request.

### R3-P3-4: The proposed counter route keeps one handle per built adapter with no bound

- **Location:** A12, the *3.4* sentence (L482-485): "the composition's factory keeps each built adapter's counter handle, as the test factory keeps `scopes()`".
- **Root cause.** The route is copied from a test-only pattern into the production composition without a retention rule.
- **Sequence.** Endpoint instances are bounded at 16 and retired least-recently-used (contract §10), but a long-lived owner keeps building them. A factory that keeps every adapter's handle grows with instance turnover. That is the plan's R6 shape: 10,000 operations on one owner, memory bounded.
- **Impact.** Unbounded growth of non-secret handles. If a handle is wider than an `Arc<AtomicU64>` (for example a clone of `Scopes`), it also retains retired adapters' scope records. Those contain no credential, but they are state kept past disposal.
- **Correction.** "The composition's factory gives every adapter it builds one shared saturating counter (`Arc<AtomicU64>`); the sweep reads the total; no per-adapter handle is retained."
- **Closure.** The 3.1 composition row also asserts the factory holds one counter after N instances are built and retired.

## Risks

- **Still recorded, not re-raised:**
  - origin-wide rejection on servers that answer 401 for "authenticated but no access" (hypothesis);
  - token expiry inside a long operation (`password_expiry_utc` ignored);
  - per-request Kerberos servers failing closed;
  - the hint's efficacy until gwz-git has a production caller.
- **The RA2a anonymous POST** after an unchallenged re-authentication spends one advertisement per miss on public keys learned on a bound connection. The H3d-4 counter counts it, so the 3.4 sweep should read it beside the miss rate.
- **Size.** The R4 row quotes "about 3,725" (r4), while the draft's own §5 notes the row still quotes r3's 3,620 (L276). The two are now inconsistent in the other direction; it is a records nit for the lane owner, not a safety item.

**Next action.** Accept r4. The lane owner then either folds R3-P3-1 to R3-P3-4 into the acceptance edit (all four are table or sentence changes plus row clauses in H1d-5, H3b-5 and the 3.1 composition row), or records them with their consequences before T lands.
