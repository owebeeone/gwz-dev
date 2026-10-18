Merged remediation is complete; editing stopped. Ready for owner settlement and original-reviewer verification.

Concrete RED → GREEN:

- Public owned-future probe initially failed with E0597/E0505. `start`/`shutdown` now use precise `use<>` capture; compiled callers retain `Future + Send + 'static` values after dropping Supervisor.
- Publication regression initially returned Protocol instead of saved Cancelled.
- Retained oversized-challenge regression initially recorded no wipe. Every failed Step now disposes its challenge before returning.

Coverage now includes cancellation/expiry with both token ownership orders, all four failed-challenge cases, synchronous captured-origin disposal, production fake-port lifecycle/I/O loops, failed observations, panics, unfinished owners and seeded reaping schedules. Native drivers use the same bounded iterations; production owners retain actual JoinHandles.

Gates pass on Darwin:

- **60 unit +3 integration +3 examples +16 compile-fail tests**
- Strict Darwin/MSVC/GNU Clippy; complete fmt
- **16 schema artifacts +13 Python tests**
- Verified **96-file standalone archive**
- Owner’s disabled-branch scan: **61 Rust files**

Size: **8,382 Rust LOC**, including **7,306 handwritten /1,076 generated**. Largest file: **444 LOC**. Dependencies and fingerprinted wire/IR/contract bytes are unchanged.

Member docs now identify trusted host packaging’s exact `build_fingerprint` source and document synchronous pre-registration handle disposal. No finding is self-closed. Windows runtime/provider qualification remains deferred; no Git mutation, campaign, release or activation performed.
