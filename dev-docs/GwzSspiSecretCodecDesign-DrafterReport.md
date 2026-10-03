Draft complete; stopped editing and ready for dual Code/State review.

Implemented owned caller values, fixed zeroizing storage, IR-generated borrowed/owned records and strict CBOR walks, semantic admission before secret copies, two-pass encoding, pure partial framing and explicit adapters. Public API documentation is in [CallerValues.md](/Volumes/projects/limbo/gwz-dev/gwz-sspi/docs/CallerValues.md).

Validation passed:

- **43 Rust checks:** 29 unit tests, caller integration, bootstrap refusal and 12 compile-fail doctests.
- **13 Python tests**, generation check covering 16 artifacts, strict Clippy and both formatting checks.
- **Standalone archive:** 61 packaged files; Cargo verification and all 43 Rust checks passed from the extracted archive.
- **Independent fixtures:** 14 pinned-taut vectors; 128 seeded framing schedules per fixture.
- **Size:** handwritten source/tests 2,427 LOC across 13 files; generated Rust 953 LOC across 13 files; golden vectors 123 LOC; generator tools 244 LOC. Largest Rust file: 444 lines.

Actual RED runs failed on missing caller exports and missing codec modules/types before implementation; subsequent GREEN runs passed. Wipe audits inspect initialized live buffers before deallocation, using per-test Arc/Mutex metadata.

WireProtocol.md, authored schema, exported IR and contract bytes remain unchanged. Only approved zeroize `=1.9.0` was added. CI/tooling now records the projection and rustfmt pin.

Supervisor/Conversation, phase and terminal arbitration, actual IPC, native UTF-16/provider disposal and process cleanup remain unimplemented. The original worker still refuses. No Git operations, external edits, commit, push or release performed.
