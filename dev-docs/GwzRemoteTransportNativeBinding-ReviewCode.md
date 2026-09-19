# Remote transport native binding qualification — Code review

## Object

- **Date:** 2026-09-20
- **Axis:** Code
- **Root:** `d7b1b04d35acd73dd85ec553a68d4e498c120f21`  
  Tree `52eabb1c0e8a9a06bd502199ac3b8df346e59de5`
- **gwz-core:** `fe815856291a93fa4ecdf0ab5879984d7b5ba1ee`  
  Tree `2234265b10cf44a32f8152aa37bda064419e1b72`
- **gwz-transport:** `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`
- **taut:** `733e8a78897a90f017f4726e4331aed95e8cb977`
- **Scope:** `GwzRemoteTransportNativeBinding.md`, `tests/transport_native`, and the root Phase 3a checkpoint section.

The exact tuple matched at review start and end. Member repositories were clean; root contained only authorized current-round prompts and the Surface report, none of which was read.

## Verdict

**GO**

| Severity | Count |
|---|---:|
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

No concrete Code-axis defect was found.

## Evidence

I inspected the complete core changed range, controlling native-binding document, Phase 3 plan/design requirements, pinned two-file git2 patch, proof runner, dependency lock, provenance tests, public fixture documentation, Rust integration tests, upstream git2 0.21.0 sources, libgit2-sys 0.18.8/libgit2 1.9.7 callback layout, and libgit2 transport construction/free paths.

Executed evidence:

- Pinned archive proof passed all **7 integration tests** with Rust 1.95.
- Archive SHA-256 verified as `ddddbf932745a6be37109b6112d3ee09696106f848449069d3a57bba937ab82e`.
- Patch SHA-256 verified as `51586ea398130dbb36e3c9f59a3117b5c19b3a5c223d5bb62e266077a8d6c3e6`.
- Original and patched source-file hashes matched the pin.
- The offline patched lock graph differed only in git2 source/checksum provenance.
- Both Python provenance tests passed.
- Rust 1.95 formatting check passed.
- The stock archive lacks `RemoteCallbacks::smart_transport`, consistent with the recorded compile-red claim.
- `git diff --check` reported six whitespace-only context-marker lines inside the stored unified patch. They are patch syntax context lines and do not introduce whitespace into applied Rust source.

## Findings

None.

## Invariant analysis

**Safe ownership:** `transport_cb` initializes the output slot, wraps libgit2’s actual `git_remote` owner in a `ManuallyDrop<Remote>`, and passes only a temporary borrow to the factory. The `SmartSubtransport` trait already requires `Send + 'static`, preventing returned subtransports and streams from retaining the borrowed Remote.

**Exact owner construction:** The safe API accepts a subtransport rather than an arbitrary `Transport`. The binding itself invokes `Transport::smart` with the callback’s native owner, preventing safe callers from substituting a transport constructed for another Remote.

**Transfer and destruction:** `Transport::into_raw` disables Rust-side ownership exactly when the pointer is returned to libgit2. libgit2 frees a newly constructed transport on connect failure and retains a successful transport on the owning Remote until that Remote is freed. Tests verify one-time owned-context destruction and retained transport reuse after disconnect and callback destruction.

**Error and panic boundaries:** Factory errors call git2’s existing raw error setter and preserve message, code, and class. The callback uses git2’s established panic wrapper; the enclosing Rust operation resumes the captured panic. Tests exercise both paths.

**Callback compatibility:** Existing callback payload construction remains shared. Transfer-progress coverage proves an ordinary callback continues to operate alongside the transport callback. Named, anonymous, and clone-created remotes receive the expected identity.

**Routing isolation:** Nested and concurrent factories retain separate state. Native file transport remains operational before, during, and after per-remote use. The foreign registry test runs in its own process, registers before work, and demonstrates coexistence without any registry mutation by the patch.

**Reproducibility:** The runner verifies archive, patch, original files, patched files, and lockfile before testing. It extracts only safe regular paths into temporary storage, patches there, uses locked offline resolution, and leaves production dependencies and Cargo’s source cache unchanged.

**Claim fidelity:** Fetch/clone verify exact seeded commit identity; push verifies the exact destination commit. Stateful discovery/exchange, retained transport behavior, factory failure/panic, ordinary callback behavior, concurrent/nested routes, and foreign/native coexistence are exercised. No SSH authentication, pooling, production activation, publication, or platform-parity outcome is claimed.

## Residual risks and next action

This GO qualifies the proposed safe per-remote binding boundary and its reproducible local proof only. Production dependency selection, the SSH adapter, authentication/trust behavior, pooling integration, every network entry, cancellation, native-platform parity, distribution, and activation remain deferred.

Proceed to the separately reviewed dependency/adapter integration checkpoint without treating this test-only patch as an available production dependency.
