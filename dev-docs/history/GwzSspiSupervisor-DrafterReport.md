Implemented and stopped editing. Ready for owner settlement and Code/State/Surface review.

- Parent async lifecycle, owned support values, FIFO capacity, immutable deadlines, phase/round/terminal arbitration, checked IDs and 256 tombstones.
- Separate control/dispatch threads; charged launch/reaper/I/O ownership. Actual child handles and pending payloads retire before launch completion.
- Windows identity provenance, creation-time Job/HANDLE_LIST, suspended membership verification, private pipes, advisory cancellation and held exit/Job observations.
- Public documentation, compiled async recipe and standalone CI updated. Production worker still refuses.

Actual GREEN on Darwin/macOS:

- `cargo test --all-features --locked`: **50 unit, 3 integration, 2 examples, 16 compile-fail tests**.
- Strict all-target/all-feature Clippy: Darwin, Windows MSVC and Windows GNU.
- Cargo fmt and all-source rustfmt checks.
- Pinned generation: **16 unchanged artifacts; 13 Python tests**.
- Verified **91-file archive**, then repeated full Rust and schema tests from its extracted standalone directory.
- Owner independently reported syntax-aware disabled-branch scope scan passing.

RED disclosure: initial test-first compilation failed because the required API/kernel did not exist. Later test construction produced three fixture failures—invalid binding data and incorrect expected wipe length—which were corrected. These were not native behavioral failures.

Size: **56 Rust files / 7,388 LOC**; handwritten **6,312**, generated **1,076**. Supervisor: **24 files / 3,492 LOC**. Largest source: **444 LOC**. Fingerprinted wire/schema/IR/contract bytes remain unchanged.

New public surface is documented in [Supervision.md](/Volumes/projects/limbo/gwz-dev/gwz-sspi/docs/Supervision.md): Supervisor, Conversation, WorkerExecutable, Options, Deadline, Cancellation, RecordId, CleanupStatus, Failure, CancellationReceipt and ShutdownReport.

Limits: deterministic fake tests and cross-compilation do not qualify Windows runtime identity changes, actual inheritance/Job containment, stalled I/O cancellation, native handle cleanup or provider disposal. Native SSPI, worker_entry, installed-worker composition and Windows campaigns remain deferred. No Git mutation, release or activation performed.
