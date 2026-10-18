# SSPI caller values and secret codec — STATE-AXIS REVIEW

**Review object:** Root `85b4bcc2d8230c0f28672a02bb99997b8a8e79c3..e9f80c697acc5860ad90dbf5acd5888ccf2bd586`; gwz-sspi `2e3b646411645d0bbe1081e4dcc15d1e0b971a4e..e3851768da8d58140d92590bf61575f6edbe333c`. Controlling document: `dev-docs/GwzSspiSecretCodecDesign.md`, DRAFT implementation checkpoint dated 2026-10-03.

**Baseline:** Root `e9f80c697acc5860ad90dbf5acd5888ccf2bd586`; gwz-sspi `e3851768da8d58140d92590bf61575f6edbe333c`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`, unchanged reference only. Exact committed implementation and checkpoint sources were read with `git show`; checkout documents, generated modules and tests were inspected with `cat`, `sed`, `nl` and `rg`, alongside the specified committed diffs. All three HEADs matched at both the start and end.

**Date:** 2026-10-03.

**Axis:** State — ownership transitions, partial-transfer accounting, failure direction, interruption boundaries and bounded retained storage. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings in the reviewed scope.

---

## 0. Evidence base

Read the workspace `AGENTS.md`, `AGENTS_GWZ.md`, member `AGENTS.md`, and canonical State prompt. Applied `AgentProcessRules.md`, particularly L1-13 through L1-22, L2 ownership/generated-code/evidence rules and the State review mandate, as amended by `GwzProcessOptimization.md`.

Checked these controlling documents:

- `GwzSspiSecretCodecDesign.md`, lines 1–105.
- `GwzSspiMessagesAcceptance.md`, lines 1–63.
- `GwzSspiMessagesDesign.md`, lines 1–102.
- `GwzSspiDesign.md`, revision 2, §§1–8.
- `GwzSspiPlan.md`, lines 1–46, particularly the step-1 secret-boundary stop.
- `GwzSspiCallerGuide-DRAFT.md`, lines 1–122.
- Member `docs/Architecture.md`, lines 1–38; `docs/Testing.md`, lines 1–56; `docs/CallerValues.md`, lines 1–60; `docs/WireProtocol.md`, lines 1–204.

Inspected the complete implementation of `src/secret.rs:1–112`, `src/values.rs:1–208`, `src/protocol/cbor.rs:1–162`, `profile.rs:1–262`, `adapters.rs:1–133`, `framing.rs:1–149` and module boundaries. Inspected all thirteen generated Rust modules, the taut declarations, generator pin, regeneration script, Rust projection and reference-vector generator.

Read all four Rust test modules: `tests.rs:1–301`, `bounds_tests.rs:1–444`, `identity_adapter_tests.rs:1–289` and `framing_tests.rs:1–316`, plus the caller integration test and schema test source. Inspected Cargo manifests/lock, toolchain pin and public CI workflow.

Independently checked the local zeroize 1.9.0 source for enabled features, `Box<[Z]>` wiping, `Zeroizing` Drop and volatile writes. Its alloc feature introduces no enabled transitive dependency. Secret ownership wrappers do not expose its Clone or Debug implementations.

Executed only permitted checks:

| Command | Result |
|---|---|
| Root/member/core `git rev-parse HEAD`, at start and end | Exact requested tuple both times |
| Supplied pure unit binary `/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/secret-codec/debug/deps/gwz_sspi-94c665645127b05f` | Exit 0; 29 passed, zero failed/ignored |
| Pinned external Python `-B scripts/regen_schema.py --check`, from gwz-sspi | Exit 0; 16 artifacts verified |
| Same Python `-B -m unittest discover -s tests/schema -v` | Exit 0; 13 tests passed |
| `cargo metadata --no-deps --offline`, from gwz-sspi | Exit 0; standalone single-member workspace, sole declared dependency zeroize `=1.9.0`, defaults disabled, alloc enabled |

The specified member diff contains no change to the taut schema, exported IR, contract manifest, fingerprinted WireProtocol document or worker executable source.

No build, source mutation, temporary harness, worker execution, native authentication or filesystem fault campaign was performed. The Rust execution evidence is the permitted existing binary, not a fresh reviewer compilation. Caller integration, compile-fail doctests, Clippy, formatting and archive verification were inspected or recorded in the owner checkpoint; they were not independently rerun here. No current peer report or prompt was read.

## 2. Invariant analysis

### Fixed storage and cleanup ordering

The allocation path initializes storage before copying a secret. `Storage::zeroed` constructs a zero-filled boxed slice; `Storage::copy` copies only after that construction finishes. Any allocation adjustment during conversion to a box therefore occurs before secret bytes enter it. Subsequent access exposes slices, with no resizing operation.

`Storage::drop` wipes the complete initialized allocation before invoking the audit probe. The production probe is empty; test probes retain only allocation length and an all-zero result. The underlying `Zeroizing` wrapper also wipes during field destruction. The audit examines live storage, not freed memory.

The generated owned projection stores all text and byte fields in these owners, including SID/LUID, fingerprints and whole frames. Secret-bearing public and generated structures have no ordinary Clone/Debug path. Borrowed constructor sources remain caller-owned, explicitly documented. An adapter refusal retains the caller request until its owner drops; the executed test verifies that ordering and subsequent wiping.

### Admission precedes owned field copies

The decoder first walks fixed generated records using references into the held zeroizing frame. It checks complete consumption and semantic/context admission before `decode_owned` invokes the generated ownership walk. Structural or semantic refusal therefore creates no owned decoded secret fields.

The strict reader rejects wrong major types, negative integers, nonminimal heads, indefinite encodings, missing/unknown/reordered/duplicate fields, malformed UTF-8, NUL, truncated lengths and trailing data. Length arithmetic is checked. Generated map walks have fixed schemas and explicit depth accounting; hostile input cannot induce arbitrary recursive value-tree allocation.

Encoding performs semantic validation and a complete bounded size walk before allocating its destination or writing secret payload. The destination remains fixed afterward. Tests exercise every short destination for the reference fixtures and observe wiping after partial-write failure.

### Profile and supplied expectations

Envelope validation requires exactly one body matching the kind and version 1. Hello requires the embedded contract digest, supplied build expectation and supplied primary identity. SID revision/count/exact length, LUID length and unsigned session range are checked before materialization.

Begin enforces identity nullability, byte limits, package/Digest relations, binding prefix and accepted digest lengths. Digest retains the actual challenge/method/URI and checks nonempty challenge, ASCII method grammar, URI percent escapes and cap intersection. Target handling excludes ports/paths and bracketed IPv6; core canonicalization remains the documented host responsibility.

The caller cap crosses the Begin adapter exactly. Token and Challenge admission require supplied cap/provider/round context, reject missing required expectations, and never enlarge a bound. Challenge rounds are 2–8; Token rounds are 1–8; continuing round 8 refuses. Numeric attributes/status preserve unsigned 32-bit limits. Mechanism admission refuses direct-provider mismatches and unresolved completion. Error projection retains fixed classifications and permits numeric native status only for ProviderRejected.

These checks establish value admission against supplied expectations. They do not establish actual native identity, conversation phase or permission to publish.

### Reader interruption and terminal states

The reader owns at most one body allocation, created only after a complete admitted length header. Partial pushes account separately for header and body bytes. A push stops at the end of its frame and returns the exact consumed count, leaving subsequent bytes with the caller.

Successful `take` transfers the single allocation and makes the reader terminal. Premature take, EOF and abort fail terminally and release any body through its wiping owner. Subsequent pushes cannot revive the reader. EOF is not converted into Finished or cleanup success, including after complete body reception.

The executed tests cover every cut across a representative header/body for EOF, abort, Drop and take; rejected and boundary lengths; and concatenated frames with exact leftover preservation. No counterexample to the reader’s terminal grammar was found.

### Writer accounting and retained ownership

The writer holds the body while header/body progress is incomplete. `remaining` exposes only the current unwritten segment. `advance` accepts a positive count no greater than that exposed segment; it cannot skip from an incomplete header into the body or claim bytes beyond the current segment.

Zero or excessive advancement aborts and wipes. `finish` succeeds only after all four header bytes and the complete body are accounted for; premature finish fails and wipes. Drop also retains ordinary owner cleanup. Safe Rust borrowing prevents mutation or destruction while an exposed slice is still borrowed.

Every partial offset is exercised for abort, Drop, premature finish, zero writes and over-accounted writes. The seeded test executes 128 schedules for each of fourteen reference fixtures, checking exact output/reassembly with seed, case, fixture and chunk trace available on failure.

These are pure byte-accounting owners. Their abort behavior is not evidence that live native I/O has stopped; no such I/O exists in this checkpoint.

### State, durability and dependency boundaries

The production implementation introduces no durable record, filesystem mutation, process launch, native call or restart procedure. There is consequently no new durable-write edge requiring a crash/restart classifier here. The relevant interruption states are owned in-memory framing states, which remain bounded and terminal on failure.

Mutable access to framing/storage is exclusive; no production mutable global or thread-local state is introduced. Conditional test/production sections use enclosing braced modules, including the disabled branches inspected in source.

Generation derives tags/types/walks from the authoritative IR. The executed checks verify all expected generated bytes and test that a changed semantic document supplies its fresh digest to the Rust projection in the same generation. Cargo needs no Python, sibling repository or private evidence dependency. Public CI declares the standalone checks but is explicitly unexecuted evidence.

## 3. Risks and next action

The permitted existing binary and local generation checks provide pure-codec evidence. This review adds no independent fresh compilation, remote CI or Windows runtime qualification.

The next supervision kernel must preserve frame ownership through actual launch/read/write-thread completion and enforce phase, terminal arbitration and publication eligibility. A decoded Complete token, Finished frame, EOF or successful fake writer finish must not become native-success or process-disposal evidence. Those obligations remain explicitly deferred and are not claimed by this GO.

The next action is to combine the independently filed gate verdicts and record acceptance of this exact secret-codec checkpoint if the required reviews pass, then proceed to `GwzSspiPlan.md` step 2. This verdict authorizes no release, activation or native qualification.
