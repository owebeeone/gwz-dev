# Core session plan CS1.4 + CS1.5 (the gate and context freeze) — first remediation plan

Date: 2026-09-28. Status: **remediation plan for [the first verdict](GwzCoreSessionCS1.4-CS1.5-Verdict.md); applied to the step's files in one patch.**

Every finding of the two reports gets exactly one disposition: [Consistency](GwzCoreSessionCS1.4-CS1.5-ReviewConsistency.md) C-P2-1, and [Safety](GwzCoreSessionCS1.4-CS1.5-ReviewSafety.md) S-P2-1 and S-P3-1 to S-P3-6.
- All findings are accepted. None is disputed.
- The two blocking findings are corrected as their reviewers specified.
- The six P3s change the frozen interface's validation and its internal behaviour, so they are fixed now rather than carried: a later fix would reopen the freeze.
- Where a reviewer left a choice open, the choice is stated. Two change something a reviewer attacked in round 1, and the re-verdict must check each against the old counterexamples:
  - S-P3-3's re-entry rule covers every gate, not only the same gate;
  - S-P3-2's quarantined job stops being polled but is not dropped while the host context lives.
- Files: the nine of the object, plus gwz-core's `Cargo.toml`, which gains one `windows-sys` feature for S-P3-6. No other file changes, and no contract, plan or checker changes.

## 1. Blocking findings

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| C-P2-1, S-P3-1 | Blind convergence; **choice: the read leaves core.** Remove `EnvironmentSnapshot::capture()` and its allowlist entry, so gwz-core holds no environment read for the snapshot and §5.6 holds literally. Add `EnvironmentSnapshot::from_os_pairs(pairs: impl IntoIterator<Item = (OsString, OsString)>) -> ModelResult<Self>`: a read-free constructor for Rust drivers, which call `std::env::vars_os()` in their own crate at their edge. It applies `from_byte_pairs`'s refusals (an empty name, a NUL, `=` after a name's first character) with the same index-only error, and wipes the rest of its input on refusal. It is fallible because an arbitrary caller's pairs can carry what the platform never yields. Its rustdoc and `docs/RustApi.md` show the driver's call, and the module table's first row and the `capture` sentences go. The CS5.1 example host binary and any stdio host call it the same way. | `environment.rs`, `environment/tests.rs`, `process_globals_allowlist.json`, `docs/RustApi.md` | (a) The allowlist has no entry for `src/session_host/environment.rs`, and `rg 'env::(var|var_os|vars|vars_os)' gwz-core/src/session_host` matches only in test code. `check_process_globals.py` over gwz-core passes with no `STALE` and the pre-step entry count. (b) `from_os_pairs(std::env::vars_os())` succeeds, and its entries equal a `from_byte_pairs` snapshot of the same pairs' encoded bytes, first occurrence winning. The comparison obeys S-P2-1's rule. (c) Synthetic pairs: a duplicate keeps its first value, and each refused form is refused, naming only its index. |
| S-P2-1 | No test prints an environment entry. Factor the snapshot tests' set comparison into `mismatch_report(seen, wanted) -> String`. It reports only the two sizes and how many entries are in one set and not the other: never a name, a value, or an encoding of either. The live-environment test from C-P2-1 (b) fails through it, never through `assert_eq!` on entries. | `environment/tests.rs` | A unit test plants an entry such as `GH_TOKEN=s3cr3t` in one of two sets. The report contains neither the name nor the value, in plain text or in hex, and still reports the difference. |

## 2. Nonblocking findings, Safety

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| S-P3-2 | **Choice: quarantine, as the SSH reaper already does** (`agent_job.rs` `Retained::reap`: a poisoned entry is kept, never polled again, and never claims disposal). A job whose `poll` panicked is kept but never polled again, so its finish is never assumed while the host context lives. With only quarantined jobs left, the supervisor waits without a timeout, as with no job. Once the host context is released and every remaining job is quarantined, the thread ends and the quarantined jobs drop, running their `Drop`. One panic yields one panic report, never a stream. | `context.rs` | A job whose `poll` always panics is polled exactly once. While the context lives the supervisor does not wake every 20 ms, observed as no further polls over 200 ms. After the context drops, `wait_ended` returns true within one second. The existing `PanicsOnce` behaviour is unchanged for a job that panics once and later finishes, if a test keeps it. |
| S-P3-3 | **Choice: no crossing inside a crossing, on any gate.** A thread-local flag marks a thread running a crossing's closure. An `effect`, `append` or `report` on any gate, or a `revoke`, from that thread panics with a message naming the rule, instead of deadlocking. The panic unwinds through the outer crossing, whose guard releases the lock, so `revoke` then proceeds. `state()` never waits on the crossing lock, so it is safe anywhere, inside a closure included. The module notes and the callback constraint (`gate.rs:17-20, 69-74`) name the rule: a closure or callback never crosses a gate, and takes no session lock. | `gate.rs` | Each runs on a spawned thread with a bounded join, so a regression fails instead of hanging. A nested `effect` inside an `effect` on the same gate panics, and a crossing of a second gate inside the first's closure panics. `revoke` on the first gate then returns. `state()` called inside a closure returns the gate's state. |
| S-P3-4 | `validate` refuses a `read_bytes` above half the frame size, 32 MiB, with `invalid_request` naming `read_bytes`. The field's doc and RustApi.md state it: CS2.10 counts each record's encoded size against `read_bytes`, so a reply is at most `read_bytes` plus its envelope, which the other half of the frame bounds. The claim at `limits.rs:82-83` is corrected. | `limits.rs`, `docs/RustApi.md` | `MAX_FRAME_BYTES / 2` is accepted. `MAX_FRAME_BYTES / 2 + 1` and `MAX_FRAME_BYTES - 1` are refused, each naming `read_bytes`. |
| S-P3-5 | `validate` refuses a `close_wait` above one hour with `invalid_request` naming `close_wait`, so a deadline of `Instant::now() + close_wait` cannot overflow. The field's doc and RustApi.md state the maximum, and that zero means close detaches every running worker at once. `Limits`' doc states that a consumer never pre-allocates by a limit, since the counts are bounded only by the rules above. | `limits.rs`, `docs/RustApi.md` | `Duration::MAX` and one hour plus one nanosecond are refused, naming `close_wait`; one hour and zero are accepted. |
| S-P3-6 | **Choice: compare as the OS and std do.** On Windows, names compare with `CompareStringOrdinal(…, TRUE)` through `windows-sys`, whose `Win32_Globalization` feature gwz-core's `Cargo.toml` gains. That is the comparison std's `Command` makes for its environment map (`EnvKey`), so `get`, the first-wins rule and the child agree. The single-unit uppercase fold goes, with its tests. The POSIX rule is unchanged. The Windows arm must build: check it with clippy for a Windows target, as the implementer did for round 1. | `environment.rs`, `environment/tests.rs`, `Cargo.toml` | Windows CI: for a snapshot holding case variants of one name, `apply_to` yields `command.get_envs().count() == snapshot.len()`, and `get` of each variant returns the value the child sees through the probe. Every other platform: the existing tests pass unchanged, except the fold's. |

## 3. Residual notes taken

- RustApi.md lists "bytes that are not WTF-8" among `from_byte_pairs`'s refusals; that refusal applies on Windows only, so the sentence says "on Windows" (Consistency §3).
- RustApi.md and the snapshot's rustdoc qualify the zeroization: the snapshot overwrites its own buffers. Copies that std's `Command` and the OS make for a child are outside it (Safety §2).

## 4. Re-verdict

The implementer applies all of the above as one patch and reports:
- the new SHA-256 of every file;
- `cargo check --lib`, `cargo test --lib session_host` and `cargo clippy --lib` results;
- a `check_process_globals.py` run over gwz-core;
- the Windows arm's clippy result.

The same two reviewers then re-verdict with their context intact:
- **Consistency** checks C-P2-1 against its counterexample, and every changed range against the contract and the plan.
- **Safety** checks S-P2-1 and S-P3-1 to S-P3-6 against theirs, and the two choices this plan flags.

Each files its report as `-ReviewConsistency-1.md` or `-ReviewSafety-1.md`, with a table closing each prior finding.
