# GWZ Remote Transport Native Binding — State Review

**Date:** 2026-09-20  
**Axis:** State  
**Verdict:** **GO**  
**Findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Object and tuple

Phase 3a prerequisite review of the proposed safe per-remote git2 binding and its isolated qualification fixture. Production dependency activation, SSH implementation and platform parity remain deferred.

| Repository | Commit | Tree |
|---|---|---|
| root | `d7b1b04d35acd73dd85ec553a68d4e498c120f21` | `52eabb1c0e8a9a06bd502199ac3b8df346e59de5` |
| `gwz-core` | `fe815856291a93fa4ecdf0ab5879984d7b5ba1ee` | `2234265b10cf44a32f8152aa37bda064419e1b72` |
| `gwz-transport` | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| `taut` | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

The exact commits and trees matched at review start and end. Member repositories remained clean. Root acquired only authorized current-round prompt/report files; no peer prompt or report was opened.

## Evidence

I inspected core diff `9303eb86914aa5770b4f951613270b14b2108f73..fe815856291a93fa4ecdf0ab5879984d7b5ba1ee`, the controlling native-binding document, the two-file git2 patch, all Rust fixtures, provenance tests, runner pins, and relevant pinned git2/libgit2 ownership and callback paths.

Executed:

- Exact isolated proof against `git2-0.21.0.crate` SHA-256 `ddddbf932745a6be37109b6112d3ee09696106f848449069d3a57bba937ab82e`: **7 integration tests passed**.
- Patch SHA-256 `51586ea398130dbb36e3c9f59a3117b5c19b3a5c223d5bb62e266077a8d6c3e6` and original/patched source digests verified.
- Locked graph verification accepted only the git2 registry-to-local source change.
- Python provenance suite: **2 passed**.
- Rust 1.95 formatting check: **passed**.

## Findings

None.

## Invariant analysis

- **Native ownership:** The callback receives libgit2’s actual owning remote. Its temporary Rust `Remote` wrapper is held in `ManuallyDrop`, so it cannot free that owner. `Transport::smart` constructs against the same owner; `into_raw` relinquishes Rust ownership exactly once and libgit2 frees the transport.
- **Returned lifetime:** `SmartSubtransport` requires `Send + 'static`. Callback captures may borrow only for the options lifetime; returned transport and stream state cannot retain the borrowed `Remote`.
- **Retained transport:** Destroying callbacks/options after the first fetch leaves the owned subtransport alive. Disconnect/reconnect on the same Remote reuses it and ignores replacement callbacks, while dropping the Remote drops the endpoint once. Documentation correctly requires a fresh Remote for a new route or operation context.
- **Error and panic cleanup:** The callback initializes its output pointer to null. Factory errors preserve message, class and code. Panics are caught before the C boundary and resumed by git2’s existing Rust-boundary panic check. Failed construction leaves no installed transport.
- **State continuity and exact effects:** Stateful discovery/exchange uses one owned endpoint. Fetch and clone verify the exact seed commit; push verifies the destination branch’s exact new commit. No test infers success merely from bytes or callback completion.
- **Isolation:** Nested factories and two concurrent worker routes retain distinct names and state. The patch adds no thread-local routing, synthetic scheme, private-layout cast or registry mutation.
- **Coexistence:** Native file transport succeeds before, during and after custom transport use. A separately isolated process-global foreign registration continues to work before, inside and after per-remote binding activity.
- **Subprocess cleanup:** Fixture streams own their child process. Drop attempts termination and always waits, preventing unreaped upload-pack/receive-pack children on normal error, panic and teardown paths.
- **Replay integrity:** The runner verifies archive, patch, lock and source digests before applying in a temporary directory. It rejects unsafe archive entries, compares normalized locked graphs, runs offline/locked, and leaves only ignored build outputs.
- **Durability scope:** All repository mutations are controlled temporary fixtures. The candidate introduces no production persistent state, restart grammar or recovery claim.

## Residual risks and next action

This qualification is host-local macOS evidence for the safe binding boundary. It does not establish production dependency availability, real SSH transport/authentication, cancellation and timeout integration, Windows/Linux parity, remote publication, or Phase 3 network-entry coverage.

The State axis supports accepting this Phase 3a prerequisite. Combine it with the independent Code and Surface verdicts before recording acceptance or beginning adapter activation.
