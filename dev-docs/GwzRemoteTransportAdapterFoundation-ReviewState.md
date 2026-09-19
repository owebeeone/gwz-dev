# GWZ Remote Transport Adapter Foundation — State Review

**Date:** 2026-09-20  
**Axis:** State  
**Verdict:** **GO**  
**Findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Object and tuple

Review scope is the Phase 3b package candidate and blocking stream bridge only. It does not activate a production dependency or SSH adapter.

| Repository | Commit | Tree |
|---|---|---|
| root | `687e2d3c21a5fca0cfff81216eff4bdac9f855ca` | `44a453682f34d8c0400fd385aa84604adce53e81` |
| `gwz-core` | `46bbc932ac25d9b1762c77351293ea1c0ac7dcbb` | `c6e6adc500b52453b0b9a0dffca58ffba08f74b3` |
| `gwz-transport` | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| `taut` | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

The tuple matched at review start and end. Member repositories remained clean. Root contained only authorized current-round prompt/report files; no peer content was read.

## Evidence

I inspected core diff `30616010f23a7e4ae7dc96b03268a3d950dfe215..46bbc932ac25d9b1762c77351293ea1c0ac7dcbb`, including the bridge, archive runner, fork transformer, locked graphs, package pins, tests and controlling checkpoint.

Executed evidence:

- Exact archive-backed consumer proof: **21 tests passed**, including all three blocking-I/O cases.
- Bridge source SHA-256: `3450bdf09ce6fc76fed7e6285bcd0a0e2d013f69a8e18cd43edc485c56104e39`.
- Distribution transformer: **3 tests passed**.
- Archive-admission suite: **9 tests passed**.
- Rust 1.95 formatting: **passed**.
- Local `gwz-git2 0.21.0-gwz.1` preparation: seven native tests passed against both staging source and exact packaged contents.
- Produced archive SHA-256 matched the checkpoint: `2c0544413ee18231fb9185ad29cb82ffa34c223245cd044523be1515897f68af`.

## Findings

None.

## Invariant analysis

- `wait` polls without holding the wake mutex. A synchronous wake, a wake between poll and sleep, or a spurious condition-variable return is retained by the guarded flag and causes another poll.
- Every blocking call owns its future and wake token. Cancellation and endpoint loss wake pending calls; dropping the final underlying `Stream` requests cancellation without blocking cleanup.
- The bridge adds no thread, socket, timer, queue or buffering layer. Progress therefore requires an independent host dispatcher and timer, as documented; a blocked caller cannot be the sole pump.
- Partial writes retain standard `Write` behavior, and `write_all` repeats only accepted partial writes. Cancellation maps to `ConnectionAborted`, avoiding `Interrupted` retry behavior.
- EOF alone returns zero. Timeout, carrier loss, protocol failure and cancellation remain distinct `io::ErrorKind` values, with the structured transport error retained as the source. Existing readable-prefix semantics pass through unchanged.
- `end_write` performs an ordered outgoing half-close. `close` waits for endpoint cleanup facts without declaring Git success or connection reuse.
- Small writes are emitted by an independently driven coalescing timer even when the blocking side does not flush.
- Fork preparation verifies upstream, patch, lock and source digests; tests staging and packaged bytes; and publishes nothing. Output is created only after qualification, while failures leave reviewed sources and production manifests untouched.
- The locked native fixture permits only the intentional package identity/source translation. The packaged library remains `git2`, preventing mixed Rust type identities for an adopting consumer.

## Remaining risks and next action

Real SSH session ownership, authentication/trust, pool disposal, cancellation against a live adapter, production dependency activation, platform qualification, publication and network-entry coverage remain deferred.

The State axis supports accepting this adapter-foundation checkpoint. Combine it with the independent Code and Surface verdicts before acceptance or activation.
