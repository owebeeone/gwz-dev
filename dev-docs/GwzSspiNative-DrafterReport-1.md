Stopped editing. The merged remediation is ready for owner execution and review; findings are not self-closed.

Implemented:

- Immediate helper/scratch RAII ownership after spawn, with finite held-handle exit checks before `Child::wait`; failed cleanup remains explicitly unconfirmed.
- Forced failures after spawn, PID publication and observer acquisition/unwind.
- Still-suspended production children for Job closure and parent death, plus a fixture-owned no-kill Job negative control with guarded termination.
- Actual initial Negotiate query/release coverage, including per-instance status/allocation/release receipts.
- Corrected CallerValues/Testing status and a structured PowerShell recipe restoring all four environment values, including absence, and disposing only the fresh owned directory after evidence retention.

Changed 13 files: supervisor `mod.rs`, `fixture_cleanup.rs`, Windows `launch.rs`, `native_tests.rs`, `fixture_support.rs`, `containment_tests.rs`; worker Windows `conversation.rs`, `observation.rs`, `native_owner_tests.rs`; `tests/native/windows_worker.rs`; and `docs/{CallerValues,Testing,NativeFixtures}.md`.

RED/GREEN: two portable guard regressions initially failed because cleanup events and scratch disposal were absent; both now pass. Native counterexamples are implemented but unexecuted here.

Passed on Darwin/macOS, using external target `/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native`:

- `cargo test --all-features --locked`: 77 library, 1 binary, 6 integration, 4 positive and 18 compile-fail doctests.
- Strict all-target/all-feature Clippy on Darwin, Windows MSVC and GNU.
- Cargo fmt and pinned include-file rustfmt checks.
- Pinned generator `--check`: 16 artifacts unchanged.
- `cargo package --locked --offline --allow-dirty`: 124-file standalone archive verified.

Current Rust size: 87 files, 11,703 LOC—10,627 handwritten and 1,076 generated; largest file 448 lines.

Dependencies, public API, wire fingerprints and authentication policy remain unchanged. Digest remains refused. Windows suspended-child/failure/query fixtures and PowerShell restoration/disposal cases require owner campaign receipts. Earlier resumed-child receipts retain their EOF limitation; full authentication qualification remains open.
