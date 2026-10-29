# Step 1.6 cross-binding reuse authorization (gwz-transport 9f8f091, gwz-core 726e9062) — STATE-AXIS REVIEW

**Review object:** 1.6's part of gwz-transport `70516c0e..9f8f091b1c` (`a21e972` plus the merge resolution of `exchange.rs`, `instance.rs` and `ports.rs`), gwz-core `2900809f..726e90628a`, and evidence run `2026-10-25-p1-6`.
**Baseline:** lane `/Volumes/projects/limbo/gwz-dev-p1-reuse`. The tuple was identical at start and end: gwz-core `726e90628a`, gwz-transport `9f8f091b1c`, gwz-cli `91dab04522`, gwz-py `3b7af0d9fb`, gwz-core-evidence `d3b930a81f`, gwz-git `e3bc2a41f6`, gwz-sspi `a2978a8f3f`, gwz-credential `6d78c593a4`, git2-rs `d13951f7e0`, libgit2 `b172e3d187`, taut `913e0dc35c`, taut-shape `89916c4d01`, taut-shape-rs `959f0f048a`, taut-shape-py `da79bfb743`, root `fc5e7b73e4`.
**Date:** 2026-10-25
**Axis:** State
**Verdict:** **NO-GO.** P0 0, P1 0, P2 2, P3 3. I commit in advance to GO once P2-1 and P2-2 are closed by the bounded remedies below, with their closure rows and no new P0–P2.

## 0. Evidence base

- **Source read:** every hunk of the object.
  - Crate: `ports.rs` (`ReuseCommand`, `ReuseVerdict`, `Completion::Reuse`, the default `authorize_reuse`), `instance.rs` as merged (`Setup`, `Authorizing`, `openings`, `serve`, `ask_again`, `reused`, `abandon`, `fail_in_flight`), `exchange.rs`, `fakes.rs`, `reuse_tests.rs`.
  - gwz-core: `agent_auth.rs`, `agent_keys.rs`, `ssh_pool.rs` (`Lessee`, `Proofs`, `lessee`, `record_lessee`, `evidence`, `retire_idle`, the `revoked` phase flag), `ssh_worker/runner.rs` (`screen`, `SharedProof`, `settle_proofs`, `fail_waiters`), `held.rs`, `ssh_local.rs`, `ssh_setup.rs`, `ssh_network.rs::recheck_trust`, `https_pool.rs::serves_binding`, `shared_reservation.rs`, `ssh_tests/reuse_auth.rs`, `https_worker/reuse_tests.rs`, `windows_parity/3.3.json`.
- **Reference read (not in the object):** gwz-transport pool `allocation.rs::schedule`/`open_for`/`connected`, `lifecycle.rs::release`/`fail_request`/`scope_https`, and `clock.rs` idle expiry; gwz-core `https_worker/credentials.rs` (scope = `ids.unique()`), `https_policy.rs::RouteKey`, `transport_host/https_endpoint.rs:324` and `session.rs:90` (operation names = `placement-{pid}-{n}`), `ssh_worker/open_request.rs` (expiry, `WouldBlock → Capacity`), and `ssh_worker.rs::attach`.
- **Commands run** (load 4.7–9.4):
  - `cargo test -p gwz-connection` with `CARGO_TARGET_DIR=…/review-p1-6-state`: 63 passed. The target directory was deleted afterwards.
  - `RUSTFLAGS='--cfg gwz_transport_candidate' cargo test --lib p1_6_` in gwz-core, without `SSH_AUTH_SOCK`: 12 passed.
  - Recomputed the evidence's content digests. gwz-core `69091a1c…` and gwz-transport `bca8ea82…` equal `raw/digests/digests-final-after-merge.txt`, so the committed trees are byte-equal to the tested worktrees.
- **Evidence read:**
  - README and checkpoint.
  - `red-probe.diff` and the red log (6 FAILED, at the stated assertions).
  - The green logs, the mutation log names, the Linux `run2-final` and `e-after-merge` summaries (OVERALL PASS, candidate leg 3817/9).
  - Receipt `d` JSON: PASS on `9a9a659` plus the worktree; 4 named host gaps.
  - The digest start/end diff (equal).
- **Grep scan of the diffs:** no `#[cfg]` or `cfg_attr` on individual items (only `cfg_if!`), no new statics or thread-locals, and no production code under `tests/`.

## Problem, requirement provenance, and scope

- **Requested outcome** (plan §4 step 1.6; contract §7.1/§7.2; reuse design §4; D-B):
  - Before another binding's idle connection is leased: a fresh SSHSIG possession proof (ambient key) or explicit-key equality, plus a known_hosts re-check.
  - HTTPS helper-token and logon-session connections are never reused across bindings.
  - Red rows: key removed → fresh connection; known_hosts changed → refusal; expired helper token → no reuse.
- **Provenance of the findings:**
  - P2-1: contract §4.1 ("All completions include the endpoint, binding, connection and work generation"), §7.1, and design §12's claim.
  - P2-2: design §4, first sentence ("Within one operation, reuse proceeds exactly as today"), and "Why retire".
  - P3s: design §4's shared-job rule, contract §7.1, and the README's own claims.
- **Component authorization:**
  - The crate seam (`a21e972`) and the gwz-core files outside the brief's literal list (`ssh_local.rs`, `ssh_setup.rs`, `ssh_network.rs`, `shared_reservation.rs`, `agent_keys.rs`, `ring` in `Cargo.toml`, the parity JSON) are what recording and verifying the evidence needs. They are within the step's need and the lane owner's acceptance.
  - Nothing in them blocks delivery.
  - Closing P2-2 likely needs a narrow gwz-transport pool change. That is a lane-owner decision (see the correction).
- **Out-of-scope observations** (not findings):
  - **O1: HTTPS routes keyed by operation name** (the implementer's finding). This is confirmed but latent. Production names are process-unique and credential scopes are unique per lookup or native generation, so equal `HttpsScoped` identities across bindings need a test-fabricated name collision. 1.6's predicate is defense in depth on the connection leg. The route leg (B sends A's token) remains in that fabricated case. It belongs to 1.5/1.3c.
  - **O2: no maximum connection age.** Contract §7.1/§10 requires 10 minutes, checked at release. No code exists in the pool or in `ssh_pool.rs`; it belongs to reuse-design C3. It would cap P2-2's keep-alive at 10 minutes, not close it.
  - **O3: the design's key-type sentence is not met for P-521** (ring lacks P-521). The lane owner accepted this as fail-closed. Its connections fall into P2-2.

## 1. Findings

### P2-1 — Late reuse verdict serves a different binding's lease (crate host)

- **Root cause:** `Completion::Reuse { connection, verdict }` carries neither binding nor generation. `reused()` matches an `Authorizing` entry by connection only. Meanwhile `abandon()` returns a lease whose proof is still running to idle.
- **Location:**
  - `gwz-transport/crates/gwz-connection/src/ports.rs` (`Completion::Reuse`)
  - `instance.rs:265–289` (`abandon`, `release(…, Reusable)` at :284)
  - `instance.rs:512–516` (`reused`, `position(|item| item.lease.connection() == connection)`)
- **Violated invariants:**
  - Contract §4.1: completions carry binding and work generation; a late completion never acts on another generation.
  - Contract §7.1: a fresh proof for *the requesting* binding.
  - Design §12: reuse grants a session nothing its own credentials do not grant.
- **Interleaving:**
  1. A finishes; connection `c` is idle with last lessee A.
  2. B opens. `authorize_reuse{c, B}` starts and is held.
  3. B's opening is dropped. `c` is released `Reusable` to idle.
  4. C (another binding, same profile and scope) opens and takes `c`. The last lessee is A, not C, so `authorize_reuse{c, C}` starts.
  5. B's proof ends `Authorized`, for example because it signed before the key was removed, or because a human confirmed a `-c` key for B.
  6. `reused(c, Authorized)` finds C's entry and `serve`s C, which has had no proof of its own.
- **Impact:**
  - C rides B's proof after a removal or after B's confirmation, which is the replay the step must prevent.
  - C's own later verdict, even `Revoked`, finds nothing.
  - Today it is latent: no production adapter implements `authorize_reuse`; it is the seam 1.3b/1.3c will implement against.
  - The gwz-core worker path is not affected, because there a waiter is owned by its binding's `SharedProof` (attack recorded in §2).
- **Correction** (either option):
  - Carry the `ReuseCommand` (connection + binding) and a per-call proof generation in `Completion::Reuse`, record that generation in `Authorizing`, and have `reused` match all three.
  - Or keep an abandoned lease out of idle until its proof completes.
- **Closure test:** a crate row in which:
  - the fake can end one held proof by command;
  - B holds and is dropped, then C opens and holds;
  - ending B's proof `Authorized` leaves C unserved, with exchanges unchanged;
  - ending C's proof `Revoked` gives C a new connection.
  Add a mutation that ignores the binding in `reused`; the row must kill it.

### P2-2 — An unprovable connection keeps itself alive and defeats other bindings' own reuse (both paths)

- **Root cause:** a cross-binding lease that cannot be proved is released `Disposition::Reusable`, and the opening retries `fresh`. Two pool behaviours combine with that:
  - `release` re-stamps `Idle { since: now }` (`lifecycle.rs:172`);
  - idle selection is lessee-blind and lowest-`ConnectionId`-first (`allocation.rs:22`).
  So every later non-fresh request of any other binding is handed the same connection, bounced, and sent to a new connection. Its own idle connections are skipped.
- **Location:**
  - Unprovable or no-evidence releases: gwz-core `ssh_worker/runner.rs:512–513` (no evidence) and :693 (`unprovable`); `held.rs:57` (`fresh`).
  - Crate: `instance.rs:446` (no proof started, the port's default), :528 (`Unprovable`), and :497 (`fresh`).
- **Violated invariants:**
  - Design §4: "Within one operation, reuse proceeds exactly as today."
  - Design §4 "Why retire" (keeping such a connection only preserves authority nobody else can show) and the §7 idle bound (60 s from release).
- **Reproduction:**
  - **gwz-core:** binding A (SSH URL password, or a P-521 agent key) does one exchange, leaving `c1` idle. Binding B then does two exchanges, under 60 s apart.
    - B1 leases `c1`, gets no evidence or `Unprovable`, releases it, then fresh `c2`.
    - B2 leases `c1` again (lowest id), and fresh `c3` while `c2` sits idle. `reused` is false and there is one handshake per open.
    - `c1`'s idle clock restarts on every bounce, so it never expires while B or any other binding keeps opening.
  - **Crate:** extend `p1_6_an_unprovable_connection_stays_idle_and_the_binding_gets_its_own` with a second open by B. Expect `connects() == 3`; correct behaviour is 2.
  - Because the port's default starts no proof, every multi-binding instance in the crate behaves this way until an adapter proves.
- **Impact:**
  - Within-operation reuse is lost for every binding but the lowest-id connection's owner (handshake and password auth per open).
  - A connection whose authority no other binding can show is kept alive indefinitely, holding a `per_user_host` slot and server resources.
  - The idle-age bound is lost.
  - It is fail-closed for authority: nothing leaks.
- **Correction** (bounded; lane-owner choice):
  - Release a bounced lease without re-stamping `since`, as a pool `release` variant that restores the prior idle stamp.
  - Retry non-fresh with that connection excluded. This needs a request-level exclusion set or last-lessee-preferring selection in the gwz-transport pool, which is a narrow pool addition.
  - Alternatively, document a chosen bound and accept it explicitly as a design amendment.
- **Closure test:**
  - Crate and gwz-core rows: binding B's second open reuses B's own idle connection (`reused: true`, one connect).
  - The unprovable connection expires 60 s after its last real release, despite B's opens; a fake clock is enough.

### P3-1 — RSA keys outside 2048–8192 bits are sent to the agent before being classed Unprovable

- **Location:**
  - `agent_keys.rs:219` (`provable()` excludes only DSA and P-521);
  - the size check after signing at `agent_keys.rs:274`;
  - `agent_auth.rs::prove_possession` (signs, then verifies).
- **Violated claim:** README "The proof": "P-521, DSA and other RSA sizes are `Unprovable`: never sent to the agent".
- **Impact:** for such a key, every cross-binding lease asks the agent to sign; a `-c` key prompts the human each time; and the answer is discarded. P2-2 multiplies this, once per open.
- **Correction:** check the RSA modulus size from the blob before signing (for example `provable_key(blob)`), and fix the README.
- **Closure:** a row in which an RSA-1024 key's proof sends zero sign requests.

### P3-2 — The shared proof job is cancelled when its waiters time out, not only by binding cancellation

- **Location:** `runner.rs:612–614` ("Nobody waits: dropping the job cancels it").
- **Violated invariants:**
  - Contract §7.1: "Only binding cancellation cancels the shared job."
  - Design §4: "A passed proof is recorded for the binding even when the request that started the job has already failed."
- **Impact:** if a confirm-key prompt is answered after the only waiter's deadline, the answer is lost, and the next lease prompts again. That breaks the "one human confirmation per operation and key" cost bound. It is fail-closed.
- **Correction:** keep the job, under its supervisor and cap, until it ends or the binding is cancelled, and record a passing verdict for the binding.
- **Closure:** a row in which a held proof outlives its waiter, then passes; the next lease by the same binding asks the agent nothing.

### P3-3 — Evidence claims not supported by the filed logs

- **Location:** README "Finding probe (`raw/mac/green-gwz-core-rows-1.log`)", and `https_worker/reuse_tests.rs`'s `eprintln!` probe.
- **Problem:** the probe prints only on a failing run, or with `--nocapture`. No filed log contains "carried". Also, the HTTPS red row asserts connection inequality only, while its own probe reports that, in the same fabricated collision, B still sends A's expired token. "Expired helper token → no reuse" is therefore shown for the connection leg only.
- **Correction:** turn the probe into an assertion recorded as the expected 1.5/1.3c finding (or rerun with `--nocapture` and file the output), and word the README row as "connection leg only; route leg = O1".

## 2. Invariant analysis (attacks that failed, with evidence)

**Freshness and replay**
- A fresh 32-byte nonce is drawn per proof job by `getrandom::fill` on the job's stack. Only SHA-512(nonce) inside the SSHSIG blob reaches the agent.
- The 6-byte `SSHSIG` preamble cannot parse as userauth data, which starts with a session-id length.
- The verifier checks the algorithm name (`rsa-sha2-512` for RSA, so no SHA-1 downgrade), trailing bytes, and the key material through ring.
- The OpenSSH `-Y sign` oracle row passes. The exact-bytes row passes: 135 bytes.
- A captured signature cannot match a new digest. Per-binding caching for the binding's life is the design's rule, and is not a finding.

**First lessee and setup provenance**
- In the gwz-transport pool, a connect is bound to its request. A cancelled opening closes the connection (`fail_request` → `start_closing`), so a connection never reaches another request before its first lease. `Lessee::First` is therefore the binding it was set up for.
- In the crate, the tag is `x{binding.serial}-{n}`, unique per host, so a connect cannot borrow another binding's `AuthContext`.

**Shared jobs in gwz-core**
- Jobs are keyed by binding, key and host. A waiter that expires is released `Reusable` and fails alone.
- Another binding re-leasing that connection gets its own `SharedProof`, so no verdict crosses bindings (contrast P2-1).

**Fail-closed fallbacks**
- `screen`'s `lessee()` `Err → Attach` reaches `attach`. There `lost()` (`Disposing{idle_lost}`) and the `allocation_reused`/`resource` phase checks refuse every non-Ready entry. It cannot serve without a proof.

**Retirement**
- `retire_idle` uses the idle-loss path: `Disposing{idle_lost, revoked}`, then `idle_closed`; a leased race becomes `Lost` and is closed by its release. There is one disposal per entry, and the budget is released at actual disposal.
- Scope: the same agent key (possession) or the same host, port and host key (trust).
- A sibling that another waiter has leased becomes `Lost` and its open retries fresh.
- The `p1_6_a_failed_possession_proof…` row plus mutation M6 cover this.

**Known_hosts re-check**
- It re-reads the same path under the same caps and does the same libssh2 match, without the setup's key-type filter. The difference only adds mismatches, so it is fail-closed.
- Hashed entries are handled by libssh2.
- Marker lines fail to load, which gives "untrusted", the same direction as setup.
- The changed-entry row returns `ErrorCode::Trust`, and a restored file gives a new connection, so the refused one was retired.

**Explicit keys**
- The token names exact re-read bytes. Same bytes are reused, which the row confirms. A symlink or content swap gives another token, so the pool identity differs.
- The assertion branch is reachable through `under(requested)` and discards, which is fail-closed.

**Capacity, shutdown and cancellation**
- Capacity: `start_proof` `WouldBlock` releases to idle and fails the open with `Capacity` (`open_request.rs:84,106`). It returns immediately and is not stuck.
- Shutdown: drains `proving` with `Discarded` plus "stopped". The crate's `fail_in_flight` discards `authorizing` leases.
- Opening dropped mid-proof (crate): its lease returns to idle with lessee unchanged (A), so a third binding still needs a proof. The only hole is P2-1's verdict correlation.

**Pool record and 1.5 ordering**
- The crate's lessee record lives in the instance, but every lease passes `openings()`, so there is no bypass.
- A missing `Setup` is treated as a misfit and discarded.
- The merged order is scope fit first, then proof.

**HTTPS**
- `serves_binding` refuses `Lessee::Other` for `HttpsScoped` requests; plain TLS is shared.
- Scoped identities are unique per credential or native generation, so cross-binding equality needs a route-name collision (O1). Redirects look up again.
- The logon-session row passed under real SSPI on Windows (receipt `d`).

**Threat model**
- Every binding on the owner shares the client's own agent and known_hosts. A hostile served core that steers leases across bindings gains nothing it could not request directly.
- Nothing in 1.6 crosses the carrier.

**Evidence and platform parity**
- Red-first: six gwz-core rows failed with the call site disabled. The checkpoint shows the production code was drafted before the rows; red was then shown by disabling the call site, which is acceptable.
- Mutations M1–M6 were each killed.
- Linux run2 and the post-merge run `e` both PASS.
- Receipt `d` predates the merge. The merge changed only crate files with no Windows-specific code, and gwz-core's content is digest-identical apart from the pin. The Windows-receipt-after-merge rule does not trigger, so `d` is acceptable.
- Windows "key removed" is simulated as a refused sign (a disclosed limitation).

**Hygiene**
- `cfg_if!` only, no globals, and no production code under `tests/`.

## 3. Risks and next action

- **Risk:** P2-1 becomes a live authority bypass the moment 1.3b/1.3c ship an adapter that implements `authorize_reuse`. Fix the port shape now, before any adapter codes against it.
- **Risk:** P2-2 silently erodes the reuse gains this design exists for, wherever unprovable connections exist. In the crate that is every multi-binding instance today, and it worsens with P3-1 for unusual RSA keys.
- **Next action:**
  1. Implementer: fix P2-1 (binding and generation in `Completion::Reuse`, plus the closure row and mutation) and P2-2 (no idle re-stamp on a bounce, and a non-fresh retry with exclusion; take the pool addition to the lane owner first), with their rows.
  2. Fold in P3-1 through P3-3, and amend the README.
  3. Re-review on this axis limited to those diffs. GO is pre-committed if the closure rows are green and nothing new at P0–P2 appears.
  4. O1 goes to 1.5/1.3c; O2 to C3.
