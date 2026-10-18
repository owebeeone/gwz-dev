# Remote transport adapter foundation — Code review

## Object

- **Date:** 2026-09-20
- **Axis:** Code
- **Root:** `687e2d3c21a5fca0cfff81216eff4bdac9f855ca`  
  Tree `44a453682f34d8c0400fd385aa84604adce53e81`
- **gwz-core:** `46bbc932ac25d9b1762c77351293ea1c0ac7dcbb`  
  Tree `c6e6adc500b52453b0b9a0dffca58ffba08f74b3`
- **gwz-transport:** `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`
- **taut:** `733e8a78897a90f017f4726e4331aed95e8cb977`
- **Scope:** Package/distribution candidate and preactivation blocking stream bridge only.

The tuple matched at review start and end. Member repositories were clean. Root contained only authorized current-round prompts/reports; peer material was not read.

## Verdict

**GO**

| Severity | Count |
|---|---:|
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

No concrete Code-axis finding was identified.

## Evidence

I inspected the complete core changed range, controlling foundation document, root checkpoint, `BlockingStream`, its isolated tests and source-copy path, package proof, fork transformer, manifests, locks, pins, provenance guards, and prior accepted native patch interactions. The changed range passed `git diff --check`.

Executed gates passed:

- Exact `gwz-git2` staging and extracted-package qualification: **7 native tests each**.
- Generated candidate identity: `gwz-git2` `0.21.0-gwz.1`.
- Candidate archive SHA-256: `2c0544413ee18231fb9185ad29cb82ffa34c223245cd044523be1515897f68af`.
- Exact transport archive consumer: **21 tests**, including three blocking-I/O cases.
- Copied bridge SHA-256: `3450bdf09ce6fc76fed7e6285bcd0a0e2d013f69a8e18cd43edc485c56104e39`.
- Distribution transformer tests: **3 passed**.
- Archive provenance/admission tests: **9 passed**.
- Rust 1.95 formatting: passed.

## Findings

None.

## Invariant analysis

**Blocking and wake protocol:** Each operation owns its `Signal`, mutex-protected wake bit, condition variable, and waker. Polling occurs outside the mutex. A wake between `Pending` and lock acquisition leaves the bit set; spurious notifications loop on the predicate. The bridge does not consume the thread park token.

**I/O behavior:** Reads, writes, flush, half-close, and close directly forward to the accepted stream futures. Partial writes remain visible to `std::io::write_all`. Tests exercise bounded backpressure, irregular reads, independent timer-driven coalescing, and cleanup completion with a separate host thread.

**Failure semantics:** Cancellation maps to `ConnectionAborted`, avoiding `write_all`’s `Interrupted` retry behavior. Timeout, carrier loss, protocol failure, peer failure, and caller misuse retain distinct kinds. The original transport `Error`, including peer code/effect, is retained as the `io::Error` source. EOF remains distinct from failure.

**Ownership and placement:** `BlockingStream` owns only a stream handle. Final-handle drop retains stream cancellation behavior and cannot assert connection reuse. Host pumping and timers remain external. The source sits in core and is absent from active production module/dependency wiring; no transport-runtime owner or private protocol was added.

**Distribution identity:** The transformer changes only the package name/version and distribution metadata. `[lib] name = "git2"` and the upstream `libgit2-sys` identity remain intact. The native fixture uses an explicit Cargo package alias, and locked-graph comparison rejects unrelated dependency changes.

**Provenance:** Upstream archive, accepted patch, original/patched files, fixture lock, packaging lock, staged source, and extracted package are checked. Native tests run against both staged and exact packaged bytes. The consumer proof copies the committed core bridge into an isolated core-shaped tree and reports its digest before compilation.

**Claim boundaries:** Production manifests and locks remain stock. No publication, remote fork, SSH session, authentication, trust, pooling, network-entry coverage, carrier, or cross-platform qualification is claimed.

## Remaining risks and next action

This GO accepts only the package and bridge foundation. Publication or another distributable dependency source must be separately established before coordinated dependency activation. Proceed to the nonblocking SSH session owner, trust/authentication, pool lifecycle, platform, and complete network-entry gates without advertising active SSH support.
