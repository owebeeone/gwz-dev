# SSPI caller values and secret codec implementation checkpoint — CODE-AXIS REVIEW

**Review object:** Root `85b4bcc2d8230c0f28672a02bb99997b8a8e79c3..e9f80c697acc5860ad90dbf5acd5888ccf2bd586`; gwz-sspi `2e3b646411645d0bbe1081e4dcc15d1e0b971a4e..e3851768da8d58140d92590bf61575f6edbe333c`. Controlling document: `dev-docs/GwzSspiSecretCodecDesign.md` at root `e9f80c697acc5860ad90dbf5acd5888ccf2bd586`, DRAFT implementation checkpoint dated 2026-10-03.

**Baseline:** Root `e9f80c697acc5860ad90dbf5acd5888ccf2bd586`; gwz-sspi `e3851768da8d58140d92590bf61575f6edbe333c`; unchanged gwz-core reference `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. All three HEADs were verified independently at the beginning and end and remained unchanged. Sources were inspected with `cat`, `nl`, `sed`, `rg`, exact-SHA `git show`, and the specified committed-range `git diff` commands.

**Date:** 2026-10-03

**Axis:** Architecture, interfaces, ownership, actual call paths, compatibility, and implementation agreement with the accepted authority. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings. This verdict covers the specified pure caller-value, generated codec, admission, and framing implementation only.

---

## 0. Evidence base

Read the workspace instructions supplied for this task, root `AGENTS_GWZ.md`, and member `AGENTS.md`. Read the controlling process rules, particularly `AgentProcessRules.md` L1-16–24 and L2-01–07/L2-15, together with `GwzProcessOptimization.md`, including its adopted review-granularity ruling. No current peer report or peer prompt was read.

Checked the implementation against:

- `GwzSspiSecretCodecDesign.md`, all sections, read from the exact root SHA.
- `GwzSspiMessagesAcceptance.md` and `GwzSspiMessagesDesign.md`, acceptance scope, TokenLimit amendment, protocol authority, and remaining secret-boundary gate.
- `GwzSspiDesign.md` revision 2, §§2–8.
- `GwzSspiPlan.md`, particularly step 1 and the stop before supervision.
- `GwzSspiCallerGuide-DRAFT.md`, implemented-value status, public shapes, required raw cap, error boundaries, and deferred runtime APIs.
- Member `docs/Architecture.md`, `docs/Testing.md`, `docs/WireProtocol.md` §§1–5, and `docs/CallerValues.md`.
- Member and root committed-range diffs, including dependency changes, generation changes, documentation, CI, and exports.

Inspected implementation and tests:

| Files | Inspected area |
|---|---|
| `src/secret.rs:1–112` | Initialized allocation, copying, borrowed access, Drop, production/test audit scopes |
| `src/values.rs:1–208`; `src/lib.rs:1–15` | Public exports, fixed errors, checked cap, owned request/token shapes and trait restrictions |
| `src/protocol/mod.rs:1–30` | Private module boundary and enclosing test modules |
| `src/protocol/cbor.rs:1–162` | Canonical scalar parsing, closed maps, checked slicing, bounded size pass and fixed writer |
| `src/protocol/profile.rs:1–262` | Structural and contextual admission, relations, encode/decode ownership order |
| `src/protocol/adapters.rs:1–133` | Caller Begin transfer, token/error projection, cap intersection and safe error classes |
| `src/protocol/framing.rs:1–149` | Header/body accounting, leftovers, terminal behavior and live storage ownership |
| `src/protocol/generated/*` | All thirteen modules: field/tag/type projection, enum closure, borrowed/owned walks, visibility and storage |
| `scripts/rust_projection.py:1–101`; `scripts/regen_schema.py:1–96` | IR authority, deterministic projection, fresh digest transfer and provenance checks |
| `scripts/reference_vectors.py:1–46`; `src/protocol/test_vectors.rs:1–123` | Independent synthetic reference encoding and fourteen fixtures |
| `src/protocol/tests.rs:1–301` | Reference equivalence and malformed structural admission |
| `src/protocol/bounds_tests.rs:1–444` | Bounds, package/identity/Digest/CBT relations, mechanism matrix and maximum frame |
| `src/protocol/identity_adapter_tests.rs:1–289` | Fingerprints/identity, body cardinality, status bits and exact cap transfer |
| `src/protocol/framing_tests.rs:1–316` | Partial transfer, abandoned storage, live wipe observation and seeded chunking |
| `tests/caller_values.rs:1–22` | Checked cap and constructor-source ownership |
| `Cargo.toml`, `Cargo.lock`, `rust-toolchain.toml`, `.github/workflows/ci.yml` | Standalone package/dependency boundary and public checks |

Reviewed the installed `zeroize-1.9.0` source under `/Users/owebeeone/.cargo/registry/src/index.crates.io-1949cf8c6b5b557f/zeroize-1.9.0/`: `Cargo.toml`, the scalar/slice/Box/Zeroizing implementations in `src/lib.rs`, volatile writes, and `src/barrier.rs`. The selected Box implementation wipes initialized elements without reallocating. Its optimization barrier uses assembly on supported architectures and a black-box/volatile fallback elsewhere.

Executed only authorized commands:

| Command | Result |
|---|---|
| `git rev-parse HEAD` in root, member, and core, at start and end | Exact requested tuple, unchanged |
| Authorized direct unit binary `.../secret-codec/debug/deps/gwz_sspi-94c665645127b05f` | Exit 0; 29 passed, zero failed |
| Pinned Python `-B scripts/regen_schema.py --check` | Exit 0; 16 artifacts verified |
| Pinned Python `-B -m unittest discover -s tests/schema -v` | Exit 0; 13 tests passed |
| `cargo metadata --no-deps --offline` | Exit 0; standalone member, sole dependency `zeroize =1.9.0`, defaults disabled, `alloc` enabled |
| Specified member diff for schema, IR, contract manifest, WireProtocol and toolchain | Empty; accepted contract bytes and toolchain unchanged |

The unit execution used the explicitly permitted prebuilt binary; this reviewer did not rebuild it. No Cargo build/test, worker execution, source mutation, native authentication, temporary harness, or Git mutation occurred. Owner-reported Clippy, formatting, doctest, caller integration, archive, and platform results were not rerun and are not presented as independent execution evidence.

## 2. Invariant analysis

**The generated projection preserves the accepted wire contract.** Field tags, map counts, optional nulls, enum discriminants and storage-neutral types come from the exported IR. The generator does not introduce a second handwritten field schema. Every generated reader requires the exact map count and ordered integer keys; unknown, duplicate, missing and reordered fields therefore refuse. Enum readers reject values outside their declared members. Independent taut fixtures cover every message kind and reproduce identical bytes through both borrowed and owned Rust walks. The unchanged schema/IR/semantic manifest diff and successful exact regeneration support compatibility with the accepted checkpoint.

The fresh contract digest is passed directly into both Rust projection and reference-vector generation in the same run. Projection does not read an obsolete on-disk manifest. The pinned formatter and generator provenance checks make the checked outputs reproducible within the declared tooling boundary.

**Secrets enter fixed initialized storage without an ordinary owned secret intermediate.** `Storage::copy` allocates and boxes zero-filled storage before copying source bytes. Any Vec-to-Box adjustment happens while the allocation contains zeros. Once populated, the owner exposes slices rather than capacity-changing operations. Generated owned strings and byte fields use `SecretText` and `SecretBytes`; no generated secret-bearing record derives Clone or Debug.

`Storage::drop` zeroizes before the per-instance audit observes the live allocation. The underlying `Zeroizing` wrapper also retains Drop wiping. Test probes record only length and whether all bytes are zero, before deallocation. They do not inspect freed memory or retain secret payloads. Production and test observers occupy enclosing conditional modules; no mutable global or thread-local observer is introduced.

Constructor-source ownership is stated explicitly. `SecretText::new` rejects NUL before allocation; the input is already valid UTF-8. The internal admitted constructor receives UTF-8 references, and the current owned-decoding call path reaches it after profile validation. I found no current path that invalidates `as_str`’s UTF-8 invariant or formats a rejected secret into an error.

**Admission precedes owned decoding and secret encoding.** The CBOR reader borrows from held frame storage, uses checked arithmetic and slice access, and rejects noncanonical widths, wrong majors, invalid UTF-8, NUL and trailing bytes. It does not allocate a generic value tree. `decode_owned` performs structural decoding and complete profile/context validation before invoking generated ownership conversion.

Encoding validates the complete profile and performs a bounded size walk before allocating the destination. The second walk writes into a fixed zeroizing slice. Checked writer errors drop that destination; the short-destination tests exercise partial writes and observe wiping.

**Semantic checks retain caps and reject incompatible relations.** The profile checks one matching body, version, exact contract/build expectations, valid SID shape, LUID length, session and supplied primary-identity equality. Begin checks identity presence, package/Digest relation, explicit Digest identity, target shape, CBT prefix and lengths, method/URI constraints, and supplied provider narrowing.

`TokenLimit` has a private checked representation and no Default. The Begin adapter copies its exact raw number. Token/challenge admission requires the supplied cap, a nonzero provider maximum, and expected round; it never enlarges either bound. Tests attack minimum/maximum caps, provider intersections, differing request caps, oversized inputs, round mismatch, numeric status/attribute limits, and mechanism combinations. Unresolved observations are restricted to Negotiate; Complete requires authoritative Selected.

Private Internal errors map to public Protocol. Native status is admitted as a u32 bit pattern and belongs only to ProviderRejected. Errors contain fixed classifications and optional numbers, without provider or identity text.

**Framing preserves one bounded owner and exact progress.** The reader admits the four-byte length before allocating its body, rejects zero and values above 100,000, and consumes only bytes belonging to its single frame. A following frame remains with the caller. Partial header/body counts remain within their slices. Premature take, EOF and abort become terminal and release owned body storage.

The writer exposes the remaining header or body slice, rejects zero and over-accounted progress, and permits successful finish only after both counts complete. Its body remains owned until finish, abort or Drop. Exhaustive partial-offset tests and 128 seeded schedules per reference fixture passed. These are synchronous fake transfers, not evidence about blocking-thread cancellation or real pipe ownership.

**Visibility and scope remain bounded.** Private records and codecs are not caller exports. Public values carry neither native handles nor transport/core types. Metadata and manifests show no sibling, private-evidence, Python, runtime or CBOR production dependency. Public CI uses packaged synthetic fixtures and declared tooling.

The current token adapter converts an admitted frame into an owned value; it performs no terminal arbitration. Documentation consistently assigns phase, publication eligibility, native identity verification and cleanup to subsequent supervision/native gates. This review does not treat codec admission, EOF, Finished bytes, or fake writer completion as proof of those outcomes.

## 3. Risks and next action

The permitted execution did not independently repeat compilation, negative-trait doctests, Clippy, formatting, archive extraction, remote CI or Windows qualification. Source inspection and the authorized pure checks support this bounded verdict; they do not replace those later platform/runtime gates. Ordinary allocation failure behavior and normal owned Drop also provide no physical-erasure guarantee.

The next action is to record this Code GO alongside the independently formed required verdicts for the same tuple, then begin the supervision kernel under the accepted plan once that gate is complete. Authentication, native UTF-16/provider storage, actual IPC, terminal publication arbitration, containment, host integration, release and Windows activation remain separately gated.
