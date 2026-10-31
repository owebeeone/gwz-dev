# HTTPS authentication model (DRAFT r3) — remediation plan, round 2

**Object:** `GwzHttpsAuthModel-DRAFT.md` r3 at root `5de7d71b`.

**Re-verdicts:**
- `GwzHttpsAuthModel-ReviewSafety-2.md`: NO-GO, 1 P2 and 4 P3. All ten round-1 findings are closed.
- `GwzHttpsAuthModel-ReviewConsistency-2.md`: NO-GO, 1 P2 and 9 P3. Every round-1 ID is closed.

Both axes pre-commit to GO on r4.

**Blind convergence:** both name the same single P2, the U7 regression (Safety R2-P2-1 = Consistency N-P2-1). Neither labels any root cause architectural, and every remaining item is a text correction. This is therefore a third round confined to non-architectural corrections, which the cap permits.

**Lane owner (done in the same commit as this plan):** the `GwzPhase1-RemPlan.md` R3 row now owns `https_adapter/auth.rs` (N-P3-1(c)); the R4 row lists contract §7, §8 and §11 and the `connection_host.rs` mod lines (N-P3-3).

## Dispositions (all: fix in r4)

| ID(s) | Correction |
|---|---|
| R2-P2-1 = N-P2-1 (U7) | `ConnectionBound { observed, evidence }`, with `evidence` = `Challenged` (G8) or `OnBound` (G2). **RA2 + `OnBound`:** after the GET is drained, the POST is sent anonymously on that connection, as P3. A 401 ends it `Authentication`, never answered, never replayed, `PossiblySent`. The entry stays `ConnectionBound { OnBound }`. **RA2 + `Challenged`:** `Rejected`. RA3 and RA4 are unchanged. Amend A4's sentence. Rows: mixed `/priv` + `/pub` fixture, POST `/pub` forced onto c2, one unchallenged re-authentication GET, then an anonymous POST that succeeds; entry unchanged; next `/pub` exchange works. A mutation (`OnBound` treated as `Challenged`) turns it red. H3b-4 is re-scoped to `Challenged` |
| R2-P3-1 | Table edits: RA7 covers "another status or content type: `classify`/`Protocol`, `Effect::None`"; G3 adds `Unretained` (unchanged); G8 is reworded ("the entry's reference drops; the bytes are wiped when no key of the origin holds them"); G5 adds "when the logon session is also offered and the native route is available, G8 applies". Optional 3xx re-authentication row |
| R2-P3-2 | H2a depends explicitly on R1's driver re-arm. H2a-6 runs with the driver parked first and asserts the lapse at its bound |
| R2-P3-3 | Sentence: no production caller, release tag or 3.1 lane consumes gwz-core between H1b and the completion of H1c, H1d and H1e; the R4 group is adopted whole |
| R2-P3-4 | A12: the client owner of a served core builds a fresh capture per bind; the Phase 2 served path is named as a consumer. Optional two-binds row |
| Safety U2 addition | H1e-1 also asserts that a second admission while the first is still waiting is refused, and that `QueueFull` on the first frees the claim |
| Safety caller-identity condition | R3's row: an unreadable caller identity gives a scope value unique to the capture, never a constant; only a non-secret identifier digest enters |
| Safety risk | H1d's `stream_error` arm carries a distinct message for the record bound |
| N-P3-1(a),(b) | H1e lists `admission.rs` and `binding.rs`; H3a lists the `connection_host.rs` mod line |
| N-P3-2 | Pin rule instead of a fixed order: each lane that moves the pin rebases on the latest pushed gwz-transport commit and moves the pin and the allowlist `reconciled_commit` together; R1b is named |
| N-P3-3 | The task boundary cites the amended R4 row; the (+) markers are removed. U1 and U6 are closed by `5de7d71b`; U2 records H1e and its authority |
| N-P3-4 | A8: "while one admission holds or has consumed the claim, `admit` of any clone is refused `InvalidInput`". H1e row with two concurrent admissions, neither registered |
| N-P3-5 | §3(d) gains `Exhausted` (and poisoned-lock) rows for G4, G5, P1, P4 and P5; H1d-5 asserts one |
| N-P3-6 | The budget refusal end to end: the `io::ErrorKind` (for example `QuotaExceeded`) and the `cli` form; either a detail cause naming the credential record, or A9 stating the meaning of a detail-less HTTPS-adapter `Capacity`. H1d-1 asserts the kind and the absence of any pool-limit wording |
| N-P3-7 | A12: "captures once per admitted operation, on the original entry, admits that capture once and drops it with the operation" |
| N-P3-8 | A6: "except the gwz-core HTTPS adapter's re-authentication of a connection-bound key"; A10 adds "the owner not full" |
| N-P3-9 | A12 adds a 3.4 sentence naming the route by which the sweep reads the re-authentication count, or the counter is exposed like `scopes()` with a composition row |
