Implemented Negotiate/NTLM native ownership and shared worker bootstrap; editing stopped for owner verification/review.

- Added public `WorkerBootstrap::from_args` and synchronous `worker_entry`, compiled caller documentation, and strict compile-time fingerprint provisioning.
- Added serial SSPI status/completion/mechanism handling, fixed zeroizing UTF-16/CBT storage, provider-buffer wiping/freeing, checked context/credential cleanup and strict worker phases.
- Added production-bridge fake tests and opt-in Windows native/process/EOF/Job/parent-loss fixtures.

Concrete RED: legal pre-Begin Finish and clean EOF both returned Protocol (2 failures). Both are GREEN. Additional fault/seeded tests passed; compiler and fixture-name corrections are not claimed as native RED evidence.

Passed on Darwin:

- `cargo test --all-features --locked`: 75 library tests, binary/caller/bootstrap/entry tests, 4 positive and 18 negative doctests.
- Strict all-target/all-feature Clippy on Darwin, Windows MSVC and GNU.
- Cargo/all-source formatting, schema check (16 artifacts), Python suite (13 tests).
- Package verification: 121 files. Extracted archive full tests passed.

Size: 84 Rust files, 11,197 LOC; 10,121 handwritten and 1,076 generated. Largest file: 448 lines. Wire/IR/contract fingerprints remain unchanged; only the three approved Windows features were added.

Windows execution awaits the owner using [NativeFixtures.md](/Volumes/projects/limbo/gwz-dev/gwz-sspi/docs/NativeFixtures.md). Digest remains ProviderRejected before credential/context work because the accepted input lacks H(Entity). Full step 3, remote authentication, EPA, installed packaging and Windows parity remain open. No Git mutation, campaign execution, activation or self-acceptance.
