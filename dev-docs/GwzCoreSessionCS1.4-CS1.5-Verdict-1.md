# Core session plan CS1.4 + CS1.5 (the gate and context freeze) — second review verdict

Date: 2026-09-28. Status: **accepted at the twelve gwz-core files listed below, on gwz-core `bd538656`, after [Consistency-1](GwzCoreSessionCS1.4-CS1.5-ReviewConsistency-1.md) and [Safety-1](GwzCoreSessionCS1.4-CS1.5-ReviewSafety-1.md) reported GO; this accepts steps CS1.4 and CS1.5 as implemented (the gate and context interface freeze) only**. The step is uncommitted. Acceptance authorizes no commit, push or release: the operator decides those.

The object was the implementation after the [first remediation plan](GwzCoreSessionCS1.4-CS1.5-RemPlan.md), which answered the [first verdict](GwzCoreSessionCS1.4-CS1.5-Verdict.md)'s NO-GO in one patch. Both round-1 reviewers re-verdicted it with their context intact.
- This was round 2 of the two-round cap.
- Each reviewer verified the twelve files and the five HEADs at the start and the end.
- Each ran `cargo check --lib`, `cargo test --lib session_host` (39 passed) and `check_process_globals.py` ("31 allowlisted items (18 debt, 13 permanent); nothing new").
- The Safety report was held outside `dev-docs` until the Consistency report had finished.

| Axis | Verdict | Prior findings closed | New findings |
| --- | --- | --- | --- |
| Consistency | GO | C-P2-1 | none |
| Safety | GO | S-P2-1, S-P3-1 to S-P3-6 | none |

## The accepted object

| File | SHA-256 |
| --- | --- |
| `src/session_host/mod.rs` | `42e7ef8b76de76d3d46439775c854ed5fa83570f2b197f2f1c61ea490902ac0c` |
| `src/session_host/limits.rs` | `2cb479650d1b87c4d1f4105e297c043efdba92e0e8b0399bcb495d23826839d4` |
| `src/session_host/gate.rs` | `68343faee3624b72e3f9920c6ec44b472481fe24504c71c068299394cad3b260` |
| `src/session_host/gate/tests.rs` | `d4bbf919612e8eef4c0bb46e8a9b4646d4f8b1bf8b11335d850782ab2afe747b` |
| `src/session_host/context.rs` | `fa6749598cc4f936577fb19ee1afeb828c1455e4ebecb1906d186ec9c0ff2251` |
| `src/session_host/context/tests.rs` | `fbd2b732332b97c7fea37966ef211c619460f73b9648aee751ada15e29bd0697` |
| `src/session_host/environment.rs` | `939bd57636beaa96cdab0fed56e0553fbd41066304bf5d65f99e95b15e6de349` |
| `src/session_host/environment/tests.rs` | `3f41c9b63ff2c6a1a8a3b2aea252c77f46818faed41dc64267d12bd1315d4505` |
| `src/lib.rs` | `c5cf47982ef4d767aeb3517f0dc5194910d11350bfebb3da10a5bd8acb6e0e49` |
| `docs/RustApi.md` | `ca0a8a050ef393cb80872f2ded90f3a1fd0713feb76020c08832642ce984b122` |
| `scripts/checks/process_globals_allowlist.json` | `881a81156bbf391d5b00e13250b85285d9a13150d613e68ef39c18f18c2e8a86` |
| `Cargo.toml` | `19e7803d66e8966d479db120b39776e5feff53497742a52653b640297001dc9c` |

`Cargo.lock` is unchanged. The Windows arm (`CompareStringOrdinal`, WTF-8 equality, case-insensitive names, the child probe) was accepted by reading. It is clippy-clean for `x86_64-pc-windows-msvc` in a scratch crate, and its tests first run in Windows CI at the Phase 1 exit.

## What the reviewers confirmed on the revision

- **The environment.** Core no longer reads the process environment. Drivers read it in their own crate and pass it through `from_os_pairs`, and no test prints an environment entry.
- **Nesting.** A crossing, or a `revoke`, from inside a crossing's closure or a cancel callback panics with the rule's words instead of deadlocking. `state()` takes no lock and reports `Revoked` exactly when revocation is final.
- **The supervisor.** A panicking job is quarantined: it is polled once, keeps what it owes while the host context lives, and drops when the supervisor ends.
- **Limits.** `read_bytes` is at most half the frame, and `close_wait` at most one hour, with zero documented.
- **Windows names.** They compare as std's `Command` does.
- **The new allowlist entry.** `thread_local CROSSING` meets O9's definition of `permanent`: it carries no session-relevant state. Consistency judged `debt` wrong for it, because a debt entry with no removing step on the session path would leave O9 unclosable.
- **Additivity.** TR1.4b's endpoint registry, its bounded `shutdown` and the server design's `transport_off` can still be added without breaking a caller.

## Plan-text impacts, for the session plan after TR1.4b's review

The session plan is the object of TR1.4b's dual review, which is running, so these land with TR1.4b's remediation or its post-GO corrections:
1. **File lists** (L1-06). CS1.4 gains `gate/tests.rs` and `context/tests.rs`. CS1.5 gains `environment/tests.rs` and gwz-core's `Cargo.toml`, for the `windows-sys` feature.
2. **§5.4's list of what stays `permanent`** gains `thread_local CROSSING` (`gate.rs`). gwz-core's allowlist now holds 31 entries: 18 debt and 13 permanent.
3. **§3.0's ratchet sentence** says what it means. State that carries session-relevant content is `debt`, naming the step that removes it. State that carries none may be `permanent`, with a reason, under review (Consistency-1 §2, §3).
4. **CS1.4's budget** came in at 505 production lines by the reviewer's count, against the aspirational < 450. The excess is the two Safety remediations.

## Carried to later steps

Merged from both rounds, for the owners of the steps named. They are not findings against this object.

| Step | Obligation | Source |
| --- | --- | --- |
| CS1.2 | Channel queues are bounded by counters, never pre-allocated with `with_capacity(limit)`. | Safety, both rounds |
| CS1.6 | Record whether handlers get the token through `OperationServices` or through `HandlerContext`. | Consistency, both rounds |
| CS2.2, CS2.12 | A session context never drops with a live, uncancelled token. | Safety, both rounds |
| CS2.4, CS2.5, CS2.9 | Guards whose `Drop` could cross a gate stay in the worker's frame, outside every closure. A nesting panic raised from a destructor during unwinding aborts the process. The gate's module doc gains that sentence when these steps add such guards. | Safety-1 §3 |
| CS2.10 | Count each record's encoded size against `read_bytes`; that is what makes the half-frame bound sufficient. | Both |
| CS2.11, CS3.7, CS3.9 | A closure or cancel callback never crosses or revokes any gate, and a wait happens after its crossing, waking on the token. A closure must not block on another thread's crossing either: the thread-local rule cannot detect that. | Both, round 2 |
| CS2.12 | Measure close latency while a worker emits events in a loop, since the mutex is not fair. | Safety, round 1 |
| CS3.5 | A supervised job releases its permit in `Drop`, not only in `poll`, so the release when the supervisor ends is real. | Safety-1 §3 |
| CS3.7 | Cancel callbacks are non-blocking, and cannot report through a gate. The request registration signals its runtime without a crossing, and the worker reports the cancel at its next crossing. | Both, round 2 |
| TR1.4b's `shutdown` | Count quarantined jobs in its pending report, since they hold their resources for the host context's lifetime. | Safety-1 §3 |
| Socket host | Keep the server design's entry bound, since `insert` is quadratic in a snapshot's entries. | Safety, round 1 |
| Phase 1 exit | The Windows-only tests run for the first time. `case_variants_of_a_name_are_one_entry_for_get_apply_to_and_the_child` shows any disagreement between `CompareStringOrdinal` and std. | Both |

## Round count

| Round | Consistency | Safety | Blocking findings |
| --- | --- | --- | --- |
| 1 | NO-GO (1 P2) | NO-GO (1 P2, 6 P3) | C-P2-1 and S-P3-1 converged blind; S-P2-1 |
| 2 | GO | GO | none |

No finding was classified as architectural.
