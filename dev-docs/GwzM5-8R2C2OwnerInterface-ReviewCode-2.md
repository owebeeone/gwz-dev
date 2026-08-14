# R2-C2 physical catalog-owner critical-interface code re-review

Date: 2026-08-15 (Australia/Sydney)

## Verdict

**NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 1 |
| P3 | 1 |

The corrected checkpoint closes the first code review's two P2 findings and
one P3 finding, and it also supplies the complete physical owner and retained
completion capability missing from the first state/filesystem review. A new
deterministic exact-SHA probe nevertheless shows that the last source-object
check is not bound to the subsequent namespace publication: a replacement
installed after the last verification is moved into a reserved destination
before the owner rejects it. That is a blocking P2 under the requested gate.

## Reviewed immutable tuple and scope

- workspace root: `411775d5916e0d14c9dcc45f04add80144765db9`
- `gwz-core`: `a02d7d6a4f92e506b48320b235b0bc38b6eef4e3`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`
- prior root checkpoint: `04d4fc0bc9e203db83d15f13e73e3a7a26a72556`
- prior core checkpoint: `1ef95b4ade6fa402cf1f2101a84af493af6a7beb`
- controlling amendment SHA-256:
  `ac804e3b51592a005464dcaab38267b87f5d7a77b6b8d7c8f869fab77c32f972`

The root and all GWZ member repositories were clean at the initial check. I
read `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`, the complete current catalog
amendment and R2-C2 plan checkpoint, the first code review, and the first
state/filesystem review. I inspected the complete 21-file core correction
delta (3,058 insertions, 190 deletions) and the complete root correction delta.

The only workspace file written by this review is this report. The behavioral
and compiler counterexamples used the disposable exact-SHA export
`/tmp/gwz-r2c2-code11.412P2W/root`; no implementation, controlling document,
lock, marker, or peer-review file was modified.

## Finding

### P2 — verification and publication are separate pathname operations, so a post-check replacement is published

The correction properly retains and verifies the exact scratch source before
active publication, but it drops the verified file handle and then performs a
new pathname operation:

- `capability/pre_catalog/provider/mutation.rs:188-196` opens the observed
  scratch, proves identity/bytes, flushes it, and drops that handle;
- `:199-204` reopens the source name for the last identity/byte check and drops
  that temporary handle when `verify_named_file` returns; and
- `:205-214` calls `platform::rename_relative` with the source pathname.

On Linux and macOS the platform operation is `renameat_with` over the pathname
(`checked_artifact/platform.rs:22-46`). On Windows it opens the pathname again
at `platform.rs:94-96` and renames that newly opened handle, but does not compare
its identity with the owner-observed source. Thus none of the three platform
branches binds the object that passed the last check to the object that is
actually published. The post-publication destination check detects a mismatch
only after the foreign object has already been moved from its old name into a
reserved catalog role.

This is not a theoretical scheduling observation. In the disposable exact-SHA
export I added one test-only hook immediately after the committed last
`verify_named_file` and before `rename_relative`. The hook removed the exact
scratch and wrote a foreign replacement at the same canonical name. The owner
returned an error, but the desired read-only invariant failed with:

```text
foreign source must remain at scratch and must not be published;
scratch_exists=false,
active_bytes=Some([114, 101, 112, 108, 97, 99, 101, 109, 101, 110, 116,
45, 97, 102, 116, 101, 114, 45, 115, 111, 117, 114, 99, 101, 45, 118,
101, 114, 105, 102, 105, 99, 97, 116, 105, 111, 110, 10])
```

Those bytes are exactly `replacement-after-source-verification\n`. The
replacement was absent from scratch and present as
`checked-artifacts-catalog-bootstrap-v1.active` before rejection.

The same unchecked interval exists in the remaining catalog publications:

- staging is verified/reobserved through a retained handle at
  `directory_mutation.rs:190-214`, then the staging pathname is renamed to
  final at `:215-221`;
- active is verified by name at `:288-293`, then its pathname is renamed into
  the final directory at `:294-300`; and
- the anchor checks a name and then separately renames that name at
  `:528-542` for B-to-A, A-to-B, and B-to-A.

The committed substitution hooks run before those last checks, so they prove
that visible pre-check drift rejects but do not exercise the interval that the
counterexample reaches. The result contradicts the accepted remediation text
requiring source-object association and read-only rejection for a substituted
source. The amendment's same-user non-authentication boundary permits a
process deliberately forging exact GWZ protocol state; it does not make it
safe to move a nonmatching foreign object after the owner has classified it as
non-owned.

Required correction: make each publication primitive consume the already
verified source capability and atomically bind the moved source to its expected
identity, including directory publication and the anchor exercise. The Windows
implementation can compare the identity of the exact handle used by
`SetFileInformationByHandle`; the Unix implementation needs an admitted
platform primitive with an equivalent atomic object/name guarantee or must
reject the capability before mutation. If the intended trust contract instead
excludes post-verification same-user namespace substitution, that must be an
explicit controlling-design change and the prior claim of read-only source
substitution rejection must be narrowed. Add deterministic hooks after the
last source verification and before every publication and assert that a
replacement remains at its original name and never appears at the reserved
destination.

## Test-quality finding

### P3 — the declared fault-key “matrix” is an inventory assertion, not a convergence matrix

`catalog/bootstrap/tests.rs:324-375` constructs 21 pairs of
`(CheckedArtifactFaultKeyV1, nonempty string)`, compares their sorted stable
keys with the enum inventory, and stops. It does not install any of those fault
keys, execute a physical edge, simulate process loss, reacquire a lease, or
assert convergence/read-only ambiguity. The test therefore cannot prove
amendment exit gate 9 (“every declared fault edge converges”), despite its name
`restart_and_substitution_matrix_covers_every_catalog_bootstrap_fault_key`.

The other focused tests provide useful but narrower evidence: ten whole-edge
reacquisitions, zero/partial next-file repair, three intermediate anchor
process-stop states, and selected source/content substitutions. They do not
turn the 21-key string table into executable per-key fault evidence.

Required correction: connect every declared catalog-bootstrap key to a real
test fault point and run a parameterized recovery test that interrupts at that
point, drops the live lease/process state, reacquires a new full target lease,
and proves convergence to the same retained completion or a typed read-only
ambiguity. Keep the inventory equality assertion as a guard, but do not use a
nonempty evidence label as the behavioral proof.

## Prior findings rechecked

- **Sealed owner authority — closed.** `CatalogOwnerEdgeV1` contains a private
  `CatalogOwnerEdgeKindV1`, and every constructor remains private to
  `catalog/bootstrap.rs`. Permit execution consumes that edge; fresh-token
  generation is owner-private and the old caller-token method is gone. A
  disposable sibling constructor failed compilation with `E0451` (private
  `kind` field/private edge kind), and the same sibling reference made the
  structural checker reject the exact reference set. The pristine direct gate
  and all 58 adversarial boundary cases pass.
- **Windows durability contract — closed at the C2 contract boundary.** The
  amendment no longer treats a successful directory no-op as proof that an
  empty parent/staging/retired directory is durable. Those directories are
  explicitly provisional and authority-free. Authority-carrying files use
  `FILE_FLAG_WRITE_THROUGH` plus `sync_all`, and Windows publication uses a
  write-through source handle. Native Windows fault/power-loss evidence remains
  explicitly deferred to R2-F. A local Windows cross-target check could not
  pass the host C dependency build (`libz-sys` lacked Windows SDK/C headers), so
  it yielded no product-code result; static contract inspection and the
  committed all-target evidence were used here.
- **Amendment provenance — closed.** The preamble now identifies the ten-slot,
  retained-completion, and durability additions as C2-controlled changes after
  the C1-reviewed hash and names both first C2 NO-GO reviews as the remediation
  gate.
- **Complete owner path and completion retention — closed.** The owner now
  handles missing Git parent, scratch, active, all staging prefixes, final
  publication, active retirement, and exact completion. Completion retains the
  final directory plus format, anchor, roaming, retired root/descriptor, and
  retired bootstrap handles under the target lease and revalidates current
  named associations and exact bytes/interior. The focused completion,
  byte-identical replacement, every-edge reacquisition, and partial-prefix
  tests pass.
- **Scratch rewrite/source flush — closed.** Existing scratch repair opens
  without truncation, checks retained identity and prefix bytes plus current
  named association before truncation, writes/flushed/reobserves the same
  handle, and checks the final name. Active publication opens/verifies and
  flushes the exact observed scratch. The remaining P2 is the later atomic
  check-to-rename gap, not the corrected rewrite or missing source flush.

## Other reviewed properties

- The first-catalog interior reader charges native names before classification,
  uses ten entries plus one overflow probe, rejects aliases/duplicates/extras/
  wrong kinds, and recognizes only the frozen ordered staging prefixes.
- Infrastructure records are derived from retained physical staging, anchor,
  roaming, and retired-root identities; the taut
  `staging_directory_identity == catalog_root_identity` binding and identical
  format/retired-descriptor bytes are preserved.
- Every normal owner edge consumes its permit and returns only a target witness
  for full preflight re-entry. The only compound edge is the frozen anchor
  exercise plus descriptor publication, and all three intermediate process
  restart states converge in the committed tests.
- The opaque completion interface exposes no raw handle, path, provider,
  expected record, token, callback, or independent lease. Its retained fields
  are sufficient for C3 to add bounded global enumeration without widening the
  production entry.
- The current C2 exact first-bootstrap observer deliberately treats action
  interiors as outside its grammar. C3 must replace that initial exact view
  with the accepted 74-entry global root grammar before action admission; that
  planned extension is not itself a C2 defect.

## Commands and results

All pristine commands below ran in the requested tree unless a disposable
export is stated.

```text
cat AGENTS_GWZ.md
cat gwz-core/AGENTS.md
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD
git status --short
git -C gwz-core status --short
git -C gwz-cli status --short
gwz status --json
  PASS: exact requested tuple; root and all members initially clean

git -C gwz-core diff --stat 1ef95b4..a02d7d6
  21 files changed, 3058 insertions(+), 190 deletions(-)
git diff --stat 04d4fc0..411775d
  6 files changed, 553 insertions(+), 14 deletions(-)

.venv/bin/python gwz-core/scripts/checks/check_checked_artifact_boundaries.py
  PASS: checked-artifact boundary ok (15 visible entries, 5 classified modules)
.venv/bin/python -m unittest gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py
  PASS: 58 tests in 426.888 seconds

cargo +1.95.0 test -p gwz-core --lib checked_artifact::catalog::bootstrap::tests
  PASS: 11 passed
cargo +1.95.0 test -p gwz-core --lib checked_artifact::capability::pre_catalog::provider::directory_mutation_tests
  PASS: 7 passed
cargo +1.95.0 test -p gwz-core --lib checked_artifact::capability::pre_catalog::provider::mutation_tests
  PASS: 3 passed
cargo +1.95.0 test -p gwz-core --lib checked_artifact::interface_tests::capability_permit
  PASS: 7 passed
cargo +1.95.0 test -p gwz-core --lib checked_artifact::capability::pre_catalog::provider::catalog_tests
  PASS: 14 passed
cargo +1.95.0 test -p gwz-core --lib checked_artifact::interface_tests::catalog_recovery
  PASS: 5 passed
cargo +1.95.0 test -p gwz-core --lib checked_artifact::interface_tests::durable_records
  PASS: 8 passed

cargo +1.95.0 fmt --all -- --check
git diff --check
git -C gwz-core diff --check
  PASS

Disposable exact-SHA sibling edge-constructor probe:
cargo +1.95.0 check -p gwz-core --lib
  EXPECTED REJECTION: E0451, private CatalogOwnerEdgeV1.kind/private edge kind
python scripts/checks/check_checked_artifact_boundaries.py
  EXPECTED REJECTION: CatalogOwnerEdgeV1 reference set gained authority.rs

Disposable exact-SHA post-verification source-replacement probe:
cargo +1.95.0 test -p gwz-core --lib replacement_after_source_verification_is_not_published -- --nocapture
  FAIL (counterexample): scratch absent; foreign replacement bytes published as active

cargo +1.95.0 check -p gwz-core --lib --target x86_64-pc-windows-msvc
  NOT A PRODUCT RESULT: host cross-C build stopped in libz-sys because Windows
  SDK/C headers were unavailable; native Windows execution remains R2-F
```

I did not repeat the previously completed 1,338-test full library matrix or
native release matrix; production Rust outside this correction was covered by
the settled checkpoint evidence, and the focused blocking counterexample was
deterministic.

## Final tuple and workspace status

The final tuple was rechecked after writing this report and remained:

- workspace root `411775d5916e0d14c9dcc45f04add80144765db9`
- `gwz-core` `a02d7d6a4f92e506b48320b235b0bc38b6eef4e3`
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f`

`gwz-core` and `gwz-cli` remained clean. Root status contained only this
untracked review report.
