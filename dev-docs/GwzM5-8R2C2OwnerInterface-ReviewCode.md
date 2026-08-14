# R2-C2 physical catalog-owner critical-interface code review

Date: 2026-08-14 (Australia/Sydney)

## Verdict

**NO-GO**

Severity counts:

- P0: 0
- P1: 0
- P2: 2
- P3: 1

The lease/retry skeleton and the newly frozen first-catalog interior grammar
are implementable, and the three committed physical-edge tests pass. The
checkpoint is not yet a sealed or power-loss-closed owner interface, however.
Its ready and missing-parent permits expose production mutation methods across
the entire `checked_artifact` module, including a fresh-scratch edge that
accepts a caller-selected ownership token. An unrelated sibling can compile
that bypass and the current structural gate still passes. Separately, the
admitted Windows implementation reports successful namespace flushes through
a function whose Windows branch is explicitly a no-op. Both defects must be
closed before staging/final mutation is added.

## Reviewed checkpoint and scope

The review began from the requested immutable tuple:

- workspace root: `04d4fc0bc9e203db83d15f13e73e3a7a26a72556`
- `gwz-core`: `1ef95b4ade6fa402cf1f2101a84af493af6a7beb`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Root and both member trees were clean at the initial check. I read
`AGENTS_GWZ.md`, `gwz-core/AGENTS.md`, the current
`GwzM5-8R2CCatalogBootstrapAmendment.md`, and the R2-C2 package contract in
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`. I reviewed the complete core delta
from `f416e6d7bf8b582087b3dd1512dacc91db066369` to the requested core SHA and
the complete root checkpoint commit.

Read-only committed-tree tests used the pristine exact-SHA export
`/tmp/gwz-r2c2-pristine11.PEoLIe/root`. The authority counterexample used only
the disposable exact-SHA export `/tmp/gwz-r2c2-code11.eLQLLJ/root`. No
implementation, controlling document, lock, marker, or peer-review file in the
workspace was modified.

## Findings

### P2 — permit-visible edge methods bypass the claimed sealed owner and accept a caller token

The public facade is appropriately narrow: `catalog/bootstrap.rs:23-28`
accepts only `CatalogMutationLeaseV1` and returns an opaque retained catalog.
That is not the only production mutation surface, though:

- `capability/pre_catalog.rs:154-187` exposes
  `CatalogPermitV1::execute_write_or_rewrite_scratch` as
  `pub(in crate::checked_artifact)` and accepts a
  `CatalogBootstrapOwnershipTokenV1` chosen by its caller;
- `:190-215` exposes active publication at the same broad visibility; and
- `:314-324` exposes the missing-parent physical edge at the same visibility.

The lower raw-handle functions are correctly restricted to
`capability::pre_catalog`, but these permit methods are mutation-capability
wrappers available to every sibling below `checked_artifact`. The fresh branch
has no expected record, so lines 159-186 construct the scratch name and record
from the supplied nonzero token and authorize the physical edge. The check
against an expected record constrains recovery, but it does not constrain a
fresh caller token.

This directly contradicts amendment section 7's statement that one sealed
`CatalogOwnerV1` owns namespace mutation and exit-gate item 10's rule that no
production signature accepts token bytes or token state. It also weakens the
one-edge/retry invariant: a sibling can consume a permit, perform an edge, and
retain or discard the returned witness without following the private
`CatalogOwnerStepV1::Retry` loop.

The counterexample is compiler-confirmed. In the disposable exact export I
added a review-only production helper to the unrelated existing
`checked_artifact/authority.rs`. It accepted a
`Box<CatalogPermitV1<'lease>>` and `[u8; 32]`, constructed the nonzero token,
called `execute_write_or_rewrite_scratch`, and discarded the returned witness.
`cargo +1.95.0 check -p gwz-core --lib` succeeded. Because the helper did not
need to name a lease or witness type, the checked-artifact boundary checker
also returned its normal **OK** result. Thus the source gate does not turn the
broad Rust visibility into a sealed type boundary.

The committed interface test at
`interface_tests/capability_permit.rs:100-157` misses this counterexample: it
searches only `catalog.rs` and `catalog/bootstrap.rs` for forbidden token or
provider text. It never examines the production permit methods that actually
expose the mutation.

Required correction:

- Make physical edge execution uncallable from an unrelated
  `checked_artifact` descendant. Co-locate the owner with the permit/provider
  transition or require an unforgeable owner authority whose constructor and
  field are private to the actual `CatalogOwnerV1` module; broad
  `pub(in crate::checked_artifact)` mutation methods are not sufficient.
- Generate a fresh token inside that sealed transition. No production-visible
  mutation method may accept a token or token bytes. Recovery must continue to
  use only the token extracted from its owner-classified expected record.
- Apply the same owner-only barrier to parent creation and active publication,
  while preserving permit consumption and witness-only retry internally.
- Add a compile-fail or structural adversarial test that injects the above
  helper into an unrelated existing sibling. The gate must reject it even when
  the helper discards the result and never names a lease/witness type.

### P2 — the admitted Windows missing-parent namespace flush is a successful no-op

The missing Git-parent edge creates `gwz`, reopens it no-follow, observes
directory metadata, and calls `sync_directory` to “flush created Git GWZ
parent” (`provider/mutation.rs:16-30`). Scratch creation similarly syncs the
file and then calls `sync_directory` for the namespace edge (`:39-92`). The
helper at `:122-127` delegates both operations to
`checked_artifact::platform::sync_parent`.

On Windows, `platform.rs:145-149` implements `sync_parent` as unconditional
`Ok(())`; its comment explicitly says a normal directory handle cannot be
flushed portably. The Windows pre-catalog provider nevertheless admits local
NTFS as `WindowsNtfsFileId128V1`; these C2 operations neither perform another
namespace durability primitive nor return `Unsupported` before mutation.

This is not merely missing future native fault evidence. Amendment section 6
requires the owner to flush the created parent edge, and the R2-C2 plan requires
each classified create/write edge to be flushed before retry. Returning success
from a no-op cannot establish that durability proof. In particular, the
ancestor `gwz` link can remain unflushed while later descendant catalog edges
advance, so the eventual opaque completion cannot rely on the promised
power-loss ordering.

The Windows active-publication rename does request `FILE_FLAG_WRITE_THROUGH`
through the platform rename implementation, so this finding is specifically
the missing-parent create and scratch namespace barrier, not the no-replace
rename itself.

Required correction:

- Introduce a platform-owned namespace-edge durability operation whose Windows
  implementation actually proves the admitted NTFS edge durable. Use it for
  the fixed parent creation and scratch namespace edge.
- If the fixed-name/no-extra-object grammar cannot be implemented durably on a
  supported Windows host, reject `PlatformCapability::NamespaceDurability`
  before the first mutation rather than returning success.
- Keep the exact outer and ten-slot grammars: the fix must not introduce an
  unlisted durability marker. Add a Windows-native fault/power-loss test and a
  structural regression that prevents the C2 flush path from resolving to the
  Windows no-op.

### P3 — amendment provenance again claims no semantic change after its reviewed hash

The amendment preamble says the post-native-equivalence bytes at SHA-256
`5168f728e86ecb7d06b79ea4033dc80e5dacbf9e519a06f782a2808d63c1992f` were
reviewed and that only the expanded provenance preamble was added afterward.
The current amendment has SHA-256
`8f1e76314fb09a598c67d1b7a2dfdd410df85c918bdcae9a0f88da440a58b51e`, and
this checkpoint added the substantive ten-slot and staging-prefix grammar in
section 5. The “only ... preamble” claim is therefore stale.

Required correction text: after the settled C2 interface reviews, state that
the exact ten-slot/prefix amendment was added after the C1-reviewed semantic
hash, identify the reviews controlling that addition, and record the newly
reviewed semantic SHA-256. Do not describe the post-hash change as
provenance-only.

## Other reviewed properties

- **Lease-only entry and lifetime:** the facade takes no path, provider,
  expected record, role rows, or callback. `CatalogMutationLeaseV1` is consumed
  into one lease-owned witness. The opaque result's lifetime remains tied to
  the held final lease.
- **Retry discipline on the intended path:** missing-parent, scratch, and
  active-publication methods consume their permit and return only a target
  witness. `CatalogOwnerV1` immediately re-enters full preflight, so its normal
  path revalidates live target, related Git facts, named lock, collision state,
  retained parent, and aggregate roles between edges. Randomness is requested
  only for a Ready fresh decision; recovery reuses the observed token.
- **Raw handles:** `RetainedPlatformRoot` and the actual mutation helpers remain
  confined to the retained provider. The opaque completion field is private
  and does not expose a directory, token, record, or permit to callers. The P2
  is the higher-level permit mutation capability, not a raw `Dir` leak.
- **First three physical edges:** on the reviewed macOS host, missing-parent
  creation creates only an empty `gwz` directory, fresh scratch produces one
  exact 241-byte name with matching canonical record fields, and the next edge
  publishes it no-replace as active with no scratch remaining. Scratch
  zero/prefix/exact restart behavior follows from create-new or truncate/rewrite
  plus the C1 aggregate classifier. The Windows durability finding remains.
- **Ten-slot grammar:** the newly frozen order is coherent and does not require
  an eleventh ownership object. `catalog-format-v1` is both the canonical
  infrastructure record and marker; the retirement descriptor duplicates the
  same identity-pinned bytes; anchor B-to-A and A-to-B-to-A preserve one object
  identity; and staging-to-final preserves the directory identity. Empty,
  ordered exact prefixes plus one bounded partial next file give every crash a
  deterministic rebuild point. Dynamic action enumeration remains a later C3
  capability after initial catalog completion, not authority to weaken these
  first-bootstrap prefixes.
- **Opaque completion:** the current private layout stores a Complete permit
  only, but the type has no exposed fields or ABI commitment. C2 can privately
  add the exact retained catalog, anchor, roaming home, retirement root, and
  bounded-enumeration capability when staging/final observation is implemented
  without widening the production entry. No separate interface defect was
  found here.
- **Unimplemented later edges:** staging, infrastructure prefixes, final
  publication, and retirement deliberately return the current “later R2-C2
  recovery edge is not implemented” error. No staging/final writer has been
  introduced prematurely.
- **Source boundaries:** the pristine catalog, pre-catalog provider, and lease
  source-tree/reference gates pass and accurately pin the current tree. The
  authority counterexample demonstrates the additional semantic mutation-call
  boundary that must now be frozen.

## Commands and results

- Exact tuple/status: root and member `git rev-parse HEAD` plus porcelain
  status — requested tuple exact; initially clean.
- Exact exports: `git archive <exact-sha> | tar -x` for root, core, CLI, and the
  clean `taut-shape-rs` dependency — succeeded.
- `git -C gwz-core diff --stat
  f416e6d7bf8b582087b3dd1512dacc91db066369..1ef95b4ade6fa402cf1f2101a84af493af6a7beb`
  — 8 files, 541 insertions, 8 deletions.
- Pristine export, `cargo +1.95.0 test -p gwz-core --lib
  checked_artifact::catalog::bootstrap::tests` — **3 passed, 0 failed**.
- Pristine export, `cargo +1.95.0 test -p gwz-core --lib
  checked_artifact::interface_tests::capability_permit` — **6 passed, 0
  failed**.
- Pristine `python3 scripts/checks/check_checked_artifact_boundaries.py` —
  `checked-artifact boundary: ok (15 visible entries, 5 classified modules)`.
- Three selected boundary mutation tests (current inventory, exact lease
  references, and recursive lease tree) — **3 passed** in 10.448 seconds.
- Pristine `cargo +1.95.0 fmt --all -- --check` — passed.
- Root and core committed-range `git diff --check` — passed.
- Adversarial export with the unrelated sibling permit/token helper,
  `cargo +1.95.0 check -p gwz-core --lib` — **passed**.
- The same adversarial export's checked-artifact boundary checker —
  **incorrectly passed** with the normal `15 visible entries, 5 classified
  modules` result, confirming the finding.

The checkpoint's focused pre-review evidence and the above proportional tests
were sufficient to decide the critical interface. I did not duplicate a full
long Rust matrix after the two blocking counterexamples were confirmed.

## Final tuple and workspace status

After writing this report, the tuple was rechecked and remained exact:

- root `04d4fc0bc9e203db83d15f13e73e3a7a26a72556`
- core `1ef95b4ade6fa402cf1f2101a84af493af6a7beb`
- CLI `3cca145c0b32410f250f640730ed7ca18f1da59f`

Both member trees were clean. Root status contained only the peer review's
expected untracked `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewState.md`; I did
not read, modify, or remove it. The only workspace path written by this review
is this report.
