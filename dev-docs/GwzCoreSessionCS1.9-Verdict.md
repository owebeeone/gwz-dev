# Core session plan CS1.9 (host-context shutdown, the off switch's attribute and the snapshot's zeroization) — verdict

Date: 2026-09-28. Status: **accepted at the six gwz-core files listed below, on root `eba8c8c`, gwz-core `3f99e49c`, gwz-cli `ebbea902`, gwz-py `728e17e` and gwz-transport `a7a36aec`, after [Consistency](GwzCoreSessionCS1.9-ReviewConsistency.md) and [Safety](GwzCoreSessionCS1.9-ReviewSafety.md) reported GO; this accepts step CS1.9 as implemented only**. The step is uncommitted. Acceptance authorizes no commit, push or release: the operator decides those.

CS1.9 is the additive re-freeze of CS1.4 with CS1.5, so its review was dual (plan §2.2, §3.0).
- The implementer worked on gwz-core `3f99e49c`, which commits CS1.4 and CS1.5 as their [Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md) accepted them.
- An API spend limit stopped the implementer once, part-way through. It resumed with its context intact, checked that its last write had landed whole, and finished. The reviewed object is its final state.
- Both reviewers verified the six files and the HEADs at the start and the end. Each ran the step's tests (50 pass), both lexical checks (nothing new), and the Windows and Linux type checks, with and without tests (clean).
- The Consistency report was held outside `dev-docs` until the Safety report had finished.

| Axis | Verdict | Findings |
| --- | --- | --- |
| Consistency | GO | none: 0 P0, 0 P1, 0 P2, 0 P3 |
| Safety | GO | none: 0 P0, 0 P1, 0 P2, 0 P3 |

## The accepted object

| File | SHA-256 |
| --- | --- |
| `src/session_host/context.rs` | `504ef99e3e139fbd23b531e7945c3419b9a097b181fc7d6a7e00502b6f4e07d1` |
| `src/session_host/context/tests.rs` | `e233fb1dca05a80a40807a342e0f08ac72e0662ada190c35cfd77fcf1aff9d47` |
| `src/session_host/environment.rs` | `0df22620d81f314dc8029019b6fa1d22cbded08b8f90f9318f86583ebccfc5f8` |
| `src/session_host/environment/tests.rs` | `38c37b67916780aacbd27190d9fb4ccd60772da272c2ce5a861d508978f0a866` |
| `src/session_host/mod.rs` | `dddbdcb1c5479d0b98bbc560f2bb4d202fb825ef2a5015f3d2c1ddb9f59cce01` |
| `docs/RustApi.md` | `3b6906c08af7ba1efc26546ee3454cff1a87f5b27f13366e96a53b02d7fa0e59` |

What it adds:
- **`HostContext::shutdown()`** disposes what the host context holds within one cleanup bound of 5 s (reuse §7) and returns a new `ShutdownReport`.
  - The report is `{ pending_local_work: u32, peer_cleanup_confirmed: bool }`. Pending counts the supervisor's jobs still running at the bound and its quarantined jobs. `peer_cleanup_confirmed` stays false until CS3.7 gives the host context a peer, following §8's `(0, false)` convention.
  - A later call, from any handle, returns the same report. A concurrent call waits for the first.
  - After it, `supervise` refuses a job and `open` refuses the host context with `invalid_request`, both before any effect. A drop afterwards disposes nothing more.
- **`SessionOptions::transport_off`** is false by default and goes only into the session context. Nothing derives it from the snapshot, the process environment or configuration (server §5).
- **Zeroization.** CS1.5 already overwrote each name and value as it dropped.
  - CS1.9 proves it on a live allocation, spare capacity included, and shows that the session's end drops the snapshot.
  - It pins that the snapshot cannot be cloned.
  - It closes one Windows gap: decoding no longer frees outgrown buffers that held part of a value.

## Recorded at acceptance

These go to the plan's next revision. The plan is accepted, and its §7 allows only a status edit after GO, so this verdict does not edit it.
- **File list.** CS1.9 also changes `src/session_host/mod.rs`: `ShutdownReport` joins its `pub use`, and a CS1.9 line joins its module doc. Without the re-export, a public method would return a type callers cannot name. It also changes the two test files CS1.4 and CS1.5 created.
  - Both reviewers found the deviation disclosed, necessary and harmless.
  - The plan's CS1.9 file list, and §4's entry for `session_host/mod.rs`, should name CS1.9.
  - CS1.2, the file's next owner, starts from CS1.9's commit: a handoff under L1-06.
- **Budget.** 246 production lines added by the implementer's count, 254 by the Consistency reviewer's, against the aspirational < 250. The difference is how comment lines were classified; 52 or 54 lines were removed. `context.rs` is 500 lines.
- **Quarantined jobs.** The plan's CS1.9 bullet says they hold their resources "for the host context's lifetime". The object drops them when the supervisor stops, which after `shutdown` comes before the host context's drop.
  - Both reviewers found this safe.
  - Both found it required by the step's own row, "a drop after `shutdown` disposes nothing more".
  - The plan's wording should follow the object.

## Carried to later steps

Both reports record these as risks, not findings. The first was raised by both axes independently.

| To | Obligation | Raised by |
| --- | --- | --- |
| CS6.6, CS6.7 | The report's false `peer_cleanup_confirmed` means "no peer took part", and after CS3.7 it will also mean "a peer did not confirm". Combining it with a session's close report by AND, as `transport_host/local_command.rs` combines reports today, would turn a confirmed session into an unconfirmed command. Combine so that a host context with no peer leaves the session's confirmation alone, and do not show a cleanup notice on this false alone. | Consistency R5, Safety 1 |
| CS3.7 | Run the endpoint registry's disposal before the supervisor's, inside the same deadline. Never hand the supervisor a job after `dispose` has closed it, since such a job is dropped unpolled. Do not count the registry's leftover work a second time. | Consistency R3, Safety 2 |
| CS3.7, CS3.8, CS7.12 | `context.rs` is at 500 lines and each of these grows it. Plan the movement-only split (rust-split) before the members fill. | Consistency R1 |
| CS3.5, CS3.10 | Refusal after `shutdown` fails closed only if callers register a job before starting the work it owns (`context.rs`, `supervise`'s doc). | Safety 3 |
| Drivers (CS4.5, CS6.6, CS8.10) | An `open` that reads the shutdown flag just before it is set still opens; that session runs local work and is refused network work. Call `shutdown` after the last `open`. | Safety 4 |
| Any step whose workers might call `shutdown` | The gate's nesting rule does not detect `shutdown` inside a crossing or a cancel callback, where it would hold the gate for up to the bound. No planned caller does this; add the check if one appears. | Safety 5 |
| CS6.6 | On Windows, a driver that captures with `from_os_pairs(std::env::vars_os())` gets strings that std built by growing, freeing outgrown buffers before core sees them. That is outside the snapshot's claim. Say so in gwz-cli's capture. The byte-stream path uses `from_byte_pairs` and is covered. | Safety 6 |
| CS7.13 | `shutdown`'s bound is process monotonic time, not an instance clock. A machine suspended during shutdown lengthens the wait. | Safety 8 |
| Phase 1 exit | The shutdown tests use a 300 ms bound with 700 ms slack; watch them on loaded CI runners. The Windows arm, including `a_decoded_string_is_built_at_its_final_size_and_matches_std`, is type-checked only and first runs in Windows CI, beside CS1.5's Windows-only tests. | Consistency R2, R4 |

## Round count

| Round | Consistency | Safety | Blocking findings |
| --- | --- | --- | --- |
| 1 | GO (no findings) | GO (no findings) | none |

No finding was classified as architectural.
