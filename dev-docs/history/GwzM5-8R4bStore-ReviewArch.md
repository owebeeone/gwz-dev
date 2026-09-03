# R4b-S checked-store architecture review

## Scope and conclusion

This is an independent, read-only review of the current uncommitted R4b-S
checkpoint against `GwzM5-8R4bTransitionDesign.md` §§9–13 and the I2 §8
unknown-field retirement contract. I reviewed the checked record, store,
transition effect/footprint integration, record-wire unknown identity support,
and focused tests. Concurrent R4b-A acceptance implementation was treated as
out of scope except where the store consumes its first-acceptance result.

**NO-GO.** P0: none; P1: two; P2: two; P3: none. The store's ordinary rewrite
protocol is strong and all executed gates pass, but exact participant-drift
occurrence ownership is not preserved and the required transition-wide
unknown-manifest matrix is absent. Dependants must not consume this interface
until the P1 findings are closed and independently re-reviewed.

## Findings

### P1 — clearing one duplicate drift identity can rebind unknowns to another occurrence

I2 makes the participant-drift identity `(kind, every expected/live field,
occurrence index among identical keys)` and forbids unknowns from moving to a
different sequence row. The manifest extractor and overlay correctly assign
that occurrence index
(`record_wire/unknown_fields/extract/common.rs:245-274` and
`record_wire/unknown_fields/overlay.rs:101-117`), but the transition/store
boundary loses it:

- `transition/reduce/mod.rs:260-279,378-383` clears by the first fully equal
  typed `ParticipantDrift` row; the authority/effect carries no occurrence;
- `transition/effect.rs:15-18,218-222` identifies retirement only by member and
  drift kind; and
- `store/unknown.rs:59-65` consequently authorizes every disappeared locator
  for that member/kind, without comparing the expected/live fields or
  occurrence.

This is observable with two drift rows that have equal I2 identity fields but
different diagnostic messages and different unknown extensions. Clearing the
first exact typed row leaves the second row, which shifts from occurrence 1 to
occurrence 0. Overlay then attaches the cleared row's occurrence-0 unknown to
the survivor and authorizes loss of the survivor's occurrence-1 unknown. The
existing lower-level regression at
`record_wire/unknown_fields/tests/overlay_regressions.rs:102-123` demonstrates
the occurrence-based overlay behavior but does not cross it with the reducer's
choice of row. This violates I2's exact identity, survivor, retirement, and
no-rebinding requirements.

Remediation must carry/select the exact occurrence identity through the bound
clear proof, `TransitionEffect`, and `RetiredContainer`, then prove by a checked
store test that clearing either duplicate preserves the other row's own
unknowns. The record/replace path at `transition/reduce/mod.rs:237-258` also
needs the same audit because it replaces by kind rather than full I2 identity.

### P1 — the mandatory per-transition unknown survivor/retirement gate is missing

Design §13 requires unknown survivor and retirement-manifest checks after every
transition. The focused store suite has ten tests, but only three transitions
cross the real store unknown overlay: participant outcome
(`tests/store.rs:31-58`), participant rollback completion
(`tests/store.rs:60-135`), and first acceptance
(`tests/store.rs:137-205`). The 53-variant predecessor test only captures that
each reducer emitted a known-field effect; it does not seed unknowns or call
`CheckedV1Store::commit` (`tests/predecessor_matrix.rs:41-68`). The small
effect retirement test samples only outcome and participant drift
(`tests/effect.rs:99-122`).

There is therefore no store-level evidence for recovery retirement,
preservation progress/completion, rollback phases other than participant
completion, publication survivors, operation drift, participant drift,
same-identity error message changes, or different-identity error replacement.
The gap directly allowed the preceding drift defect to remain green. Add an
executable matrix over every transition effect that seeds all applicable live
and retiring containers, commits through the checked store, and compares the
exact expected manifest.

### P2 — archive publication is a hard-link/unlink sequence, not the frozen atomic rename

The design names the physical mutation `open-record archive rename` (§6.9) and
requires archive to atomically move exact terminal bytes (§9). The new-record
path instead creates a hard link, syncs the destination directory, removes the
source, and syncs the source directory
(`store/archive.rs:59-69`). Exact bytes and both crash shapes are handled, but
the operation deliberately exposes a two-name interval and is not the frozen
single atomic move used by the existing archive primitive. Either restore an
atomic no-clobber rename implementation with exact destination reconciliation,
or return to design review and explicitly approve the two-step archive
protocol and its platform/filesystem assumptions.

### P2 — focused lineage and file-shape evidence omits two required cases

`tests/store.rs:207-233` proves wrong-root and different-byte contention, but
does not prove the §13 rule that an intervening same-byte replacement retains
identical durable authority. `tests/store.rs:368-396` proves a symlinked source
and invalid merge id, and the new archive test proves a symlinked `done`
directory, but no focused test replaces an open record or archive destination
with a directory/FIFO/other non-regular object. `read_regular` does reject
non-regular objects and canonical-path traversal (`store/rewrite.rs:134-148`),
so this is an evidence gap rather than a demonstrated implementation failure.

## Conforming behavior observed

- Commit binds the lease to the canonical root/location and rewrite digest,
  rereads and fully compares the checked source, reruns the known-field
  footprint, overlays unknowns, writes a unique same-directory temporary,
  fsyncs and decodes it before rename, then fsyncs the parent and exactly
  rereads the published bytes (`store/rewrite.rs:24-98`). Same-byte replacement
  is accepted by construction; any byte difference is rejected.
- The pre-publish fault leaves the original bytes and removes its temporary.
  The post-publish/pre-directory-fsync fault leaves a decodable new record;
  reopen succeeds and a stale retry rejects.
- First acceptance authorizes only derived accepted lock-member extensions;
  the focused test proves they reach the approved audit destination. Record
  survivors and the tested action/conflict/error/rollback retirements behave
  as expected.
- Terminal validation, exact-byte destination-only and both-copy archive
  reconciliation, mismatched destination rejection, open-record symlink
  rejection, and the newly hardened symlinked `done` rejection all pass.
- The complete v1 lifecycle remains behind `#[cfg(test)]` in
  `merge/mod.rs`; the reviewed lifecycle tree contains no v0 persistence call
  or v1-to-v0 serialization route. Normal production dispatch remains
  unchanged.

## Budget and ownership

The post-review owner split closes the earlier 520-line concentration:
`transition/footprint.rs` is now 390 lines and
`transition/footprint/diff.rs` is 139. The four dedicated store owners total
495 lines; the focused store test owner is 459. Using the accepted TR baseline,
S is approximately +602 net production-bearing lines with about 130 moved
lines across 9 production paths, and 461 focused test/wiring lines across 2
test paths. These are within the ≤700 net production, ≤250 moved, ≤1,000 test,
≤10 production-path, and ≤8 test/tool/doc-path ceilings. Every affected owner
is below 500 lines.

## Verification

Executed against the reviewed checkpoint:

- `cargo test -p gwz-core v1_lifecycle::tests::store --lib`: 10 passed on the
  final post-split tree;
- `cargo test -p gwz-core v1_lifecycle --lib`: 60 passed;
- `cargo test -p gwz-core --lib`: 863 passed;
- `cargo fmt --all -- --check`: passed;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed;
- merge document consistency: 8 sources / 87 assertions passed; and
- merge checker tests: 17 passed.

The footprint extraction was a mechanical ownership split after the broad test
run; the final store suite and strict format/Clippy/diff checks were rerun on
the split tree.

## Gate decision

**NO-GO for R4b-S interface acceptance.** Close the exact participant-drift
occurrence bug and the complete per-transition unknown-manifest gate first.
The archive primitive must also match the frozen atomic-rename contract (or
receive an explicit design amendment), and the same-byte/non-regular focused
cases should be added before re-review.
