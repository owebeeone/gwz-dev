# R4b-P fourth remediation filesystem-interface review

Date: 2026-08-12

Reviewed independently:

- `GwzM5-8R4bP1P2-RemPlan-4.md`, especially sections 3.1-3.6, R1/R2,
  R5, and exit criteria 1-5;
- the frozen checked-artifact/restart contracts in
  `GwzM5-8R4bP1P2-RemPlan-3.md` and `GwzM5-8Refactor.md`;
- the settled-tree filesystem findings in
  `GwzM5-8R4bP1P2-ReviewFaults-4.md`; and
- the platform API premises needed to assess feasibility.

I did not read or coordinate with the concurrent state-machine reviewer. This
was a read-only design review; I did not modify production or test code.

## Verdict

**NO-GO for R1/R2 implementation.** There is no P0 finding. One P1 restart-
closure defect and four P2 interface/evidence gaps remain.

The redesign is nevertheless a material improvement. Component-wise path
equivalence and the bounded global catalog close the family-hiding shape from
the settled tree. The external-filesystem-UUID plus persistent-handle model is
the right Linux identity shape. Separating protocol records from streamed
payloads closes the one-MiB self-stranding defect. A retained, flushed leaf
handle plus a real namespace barrier is also the right durability primitive,
and write-through retirement is a coherent replacement for the Windows
delete/no-op sequence.

The blockers are in the physical states that those interfaces can leave. In
particular, the new Windows anchor is no longer confined to its discoverable
private directory. It can be durably moved into an arbitrary managed parent
during an authority-free proof, with no durable record saying where it went.
That state is not restart-closed.

## Findings

### [P1-1] A managed-parent anchor can be stranded outside every discoverable recovery namespace

**Where:** RemPlan-4 lines 183-209 and 211-260, especially lines 244-250.

**Violated contract.** Section 3.5 requires authority-free `After` and
proof-only paths to use the durable-leaf observer. Section 3.6 says a crash at
either anchor edge has a closed recovery form. Exit criteria 4 and 5 require
the durable observation and Windows barrier to remain closed across every
restart boundary.

**Failure proof.** Let `H` be the anchor's private-home name, `P/R` the
collision-checked reserved name in a managed parent, and consider a proof-only
or authority-free `After` row, so there is no checked-artifact family
authority:

| Boundary | `H` | `P/R` | Durable locator/binding |
| --- | --- | --- | --- |
| before outbound | exact anchor | absent | none |
| after successful write-through outbound, before reobservation | absent | exact anchor | none |

A process crash in the second row leaves the only anchor in a user-managed
parent outside the private catalog. The design persists neither the target
parent's durable identity/path profile nor `R`, and `NamespaceProtocol::barrier`
accepts only the current parent. Retrying the same parent could search one
rederived slot, but another checked action sees only a missing home anchor and
has no safe way to discover the old one. Creating a replacement leaves two
exact anchors and can later collide with the stranded alias; returning
`Ambiguous` forever is not the promised closed recovery. Depending on an
unstated outer action to replay exactly the same parent also makes the platform
primitive non-self-contained and does not cover authority-free checked-
artifact callers.

This is materially different from RemPlan-3's accepted private-directory
round trip: both names there were in one known, always-enumerated directory.

**Required correction.** Freeze one of these complete protocols before R1:

1. Prefer a bounded, durable `BarrierIntent` in the private catalog, binding
   anchor identity, private-home identity/name, target-parent durable identity
   and path profile, reserved target name, and the caller/action binding. Make
   the intent durable with the discoverable private-home round trip before
   moving the anchor out. After return and reobservation, retire the intent
   durably. On restart, catalog recovery must resolve exactly the home-only,
   target-only, and same-identity duplicate forms before any other action.
2. Alternatively, use a persistent per-parent barrier object whose complete
   lifetime, visibility, collision, and GC rules are frozen. A roaming anchor
   without a durable locator is not sufficient.

The intent's scratch/publication/retirement, anchor outbound/return, both
reobservations, and successor edge must all be iterable fault boundaries. A
fault after outbound during an authority-free proof must restart, rediscover
the exact anchor, return it home, retain the managed leaf, and then reach the
same durable result. A second action attempted at that state must recover the
outstanding barrier first rather than create another anchor.

### [P2-1] The global catalog has no closed grammar for GWZ's own transient entries

**Where:** RemPlan-4 lines 92-117, 153-181, 237-260, and 498-510.

**Contradiction.** Section 3.2 makes an orphan recognized residue globally
ambiguous, while section 3.4 makes unknown or unrecognized entries foreign.
The plan still requires authority and goal scratch publication, Windows
retirement storage, anchor infrastructure, directory-bootstrap alternate
names, and mentions bounded tombstones, but it never states which catalog
category owns each form.

A crash after authority-scratch or goal-scratch creation/flush but before
publication must leave inert scratch under the frozen RemPlan-3 contract. As
written, that entry is either recognized orphan residue and globally
ambiguous or unrecognized and foreign. Either reading permanently blocks a
restart because of GWZ's own pre-publication crash. The same ambiguity applies
to the retirement directory and the new barrier intent required by P1-1.

**Required correction.** Define one exhaustive bounded catalog grammar before
implementation, for example:

- active authority/source/goal entries;
- immutable infrastructure (anchor and retirement-root records);
- published barrier/bootstrap intents and their closed physical forms;
- uniquely named unpublished scratch, which is inert and never authority;
- retired objects, which are not active state but remain bounded/enumerable
  for privacy and GC; and
- malformed, unknown-version, duplicate, and genuinely foreign entries.

State whether infrastructure lives in the scanned directory or a separately
opened retained subtree. Every directory entry must consume the catalog
entry/name/metadata budget even when its payload is not opened. Correctness
must not depend on scratch or retired-object GC. Add a generated restart table
for every fault-created entry; each row must either resume or reject for a
specific foreign-state reason, never because its ownership category was
implicit.

### [P2-2] Linux reboot-stable identity is a safe stop condition, not yet an executable release capability

**Where:** RemPlan-4 lines 119-151, 409-427, and 466-487.

**Feasibility gap.** `FS_IOC_GETFSUUID` is the correct generic UUID shape, but
it entered the upstream kernel in the v6.9 VFS UUID merge. GWZ's Linux release
workflows build on `ubuntu-22.04` labels, and neither a runner label nor a
successful build pins a runtime kernel/filesystem provider. Older supported
kernels therefore require a concrete compatibility provider or must be
explicitly unsupported. The plan's instruction to stop is safe, but it means
the R0 capability decision has not yet been made and R1's shared interface
freeze is premature.

`name_to_handle_at` success is also intentionally insufficient: Linux
documents file handles as filesystem-specific and potentially stale for
filesystem-specific reasons. A same-boot real-host test does not prove the
stated reboot/remount identity promise.

**Required correction.** Insert a lead-owned Linux capability spike before
R1 that freezes:

- the minimum supported kernel/runtime and exact admitted filesystems;
- the UUID provider per admitted filesystem, including any older-kernel
  filesystem-specific ioctl or dependency-free block mapping;
- maximum handle type/length and the filesystem-specific persistence basis;
- the exact typed rejection for overlay, network, zero/no-UUID, unstable
  handle, and unavailable-provider cases; and
- a fail-not-skip release probe recording kernel, filesystem type, provider,
  UUID, handle type/length, and casefold capability.

For every admitted Linux filesystem, persist the tuple, cross at least an
unmount/remount or two-boot boundary, and prove the reacquired tuple is equal
while the ordinary mount ID may change. If the release environment cannot do
that, the plan must narrow the advertised Linux support before production
conversion rather than defer the decision to R5.

Relevant platform evidence:

- the upstream [`vfs-6.9.uuid` merge](https://android.googlesource.com/kernel/common/+/0f1a876682f0979d6a1e5f86861dd562d1758936%5E1..0f1a876682f0979d6a1e5f86861dd562d1758936/)
  introduced the generic ioctl;
- the current Linux UAPI describes
  [`FS_IOC_GETFSUUID`](https://github.com/torvalds/linux/blob/master/include/uapi/linux/fs.h)
  as the external filesystem UUID; and
- the Linux
  [`name_to_handle_at(2)` documentation](https://man7.org/linux/man-pages/man2/open_by_handle_at.2.html)
  explicitly leaves staleness and support filesystem-specific.

### [P2-3] `observe()` still has an infallible-allocation hole despite bounded protocol reads

**Where:** RemPlan-4 lines 153-181.

**Defect.** Streaming source/goal comparison and `take(limit + 1)` protocol
decoding correctly close the settled-tree unbounded-read finding. The ordinary
`observe()` rule, however, says only to convert the statted `u64` length to
`usize`, allocate that exact length, and then read. On 64-bit systems a huge
or sparse hostile file can fit `usize` but exceed `Vec`'s `isize::MAX`
capacity or available memory. A direct exact allocation can panic or abort
instead of returning the required typed operational/ambiguous result.

**Required correction.** Require checked `isize::MAX`/container-capacity
conversion and fallible `try_reserve_exact` (or an equivalent fallible buffer
owner) before reading. Clarify that “no protocol-size cap” means action
recovery streams legitimate payloads without the former one-MiB protocol cap;
it does not promise that the byte-returning public API can materialize an
arbitrary-size value. Callers that need larger values must consume a bounded
streaming proof or receive a typed resource error before allocation.

Add simulated-length and sparse-file cases proving no panic, abort, or read
beyond the selected path. Revalidate handle identity, length, exact EOF, and
content after the fallible read as already specified.

### [P2-4] The exact Windows file-flush handle contract is underspecified

**Where:** RemPlan-4 lines 183-209 and R5 lines 483-487.

**Defect.** The design correctly requires flushing the same handle whose bytes
and identity were accepted, but it does not freeze how that handle is opened
on Windows or what happens when the exact managed file is readable but cannot
be opened for write. `FlushFileBuffers` requires `GENERIC_WRITE`. This matters
most for proof-only and authority-free `After`: the file may have been created
by another writer, and silently skipping the flush would recreate settled-tree
P1-2.

**Required correction.** Specify the exact Windows open access/share/reparse
flags. A successful `ExactDurable` must come from the one no-follow handle
opened with sufficient rights for both exact read and `FlushFileBuffers`.
Failure to obtain or flush that handle must return a typed pre-barrier error;
it must never degrade to ordinary observation or a successful namespace-only
proof. Establish this before mutating the anchor.

Add injected access-denied/flush-failed rows and a real NTFS ACL/read-only
case. Assert no `ExactDurable`, no lifecycle successor, and no stranded anchor.
Microsoft's
[`FlushFileBuffers` contract](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-flushfilebuffers)
states the `GENERIC_WRITE` requirement.

## Platform/contract closure audit

| Area | Review conclusion |
| --- | --- |
| Per-component equivalence | **Closed in architecture.** ASCII-only protocol components plus Linux per-directory casefold, macOS volume mode, and Windows `FileCaseSensitiveInfo` remove the false root-wide default. R1 should statically inventory all callers so an arbitrary persisted/user path cannot enter this API. |
| Global family visibility | **Direction accepted, P2-1 open.** Scanning all authorities makes case-mode changes and old families visible, but the catalog needs an exhaustive grammar for scratch/infrastructure/retirement states. |
| Linux durable identity | **Correct shape, P2-2 open.** External UUID plus a qualified persistent handle is appropriate; the admitted provider/baseline and reboot/remount evidence are not yet frozen. |
| Bounded reads | **Protocol/payload split accepted, P2-3 open.** Authority and streamed payload reads are bounded correctly. The byte-returning observer still needs fallible capacity handling. |
| Exact durable leaf | **Correct shape, P2-4 open.** One retained handle, file flush, parent barrier, and full reobservation close the prior split proof, provided Windows handle rights cannot silently weaken it. |
| Windows publish/retire | **Accepted conditionally.** `MoveFileExW` with no replace and `MOVEFILE_WRITE_THROUGH`, retained-handle pre/post identity checks, and exact NTFS execution are a coherent supported-profile contract. Microsoft's [`MoveFileExW` documentation](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw) says write-through does not return until the move is on disk. No copy/delete fallback may be enabled. |
| Windows anchor/bootstrap | **Anchor NO-GO; bootstrap direction sound.** Fixed-name round-trip bootstrap in an established parent is discoverable, but a roaming managed-parent anchor requires P1-1's durable locator and a literal restart table. |
| Platform evidence | **Open with P2-2/P2-4.** Exact-tree behavioral jobs are required rather than builds, but Linux must prove an admitted provider across remount/reboot and every platform lane must fail rather than skip when its promised profile is absent. |

## Exit decision

Do not begin R1 production edits. Add a durable, catalog-visible Windows
managed-parent barrier intent (or replace the roaming-anchor design), freeze
the catalog's complete transient grammar, complete the Linux capability spike,
and make ordinary allocation and Windows flush rights explicit. Then repeat
this focused filesystem review. These changes remain private protocol and
platform-interface work; none requires a v1 wire field or lifecycle phase.
