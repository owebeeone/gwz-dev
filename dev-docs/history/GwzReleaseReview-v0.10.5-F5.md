# GWZ v0.10.5 release review (F5)

Reviewer: Claude (Fable 5), independent post-release review.
Object: the coordinated gwz-core / gwz-cli / gwz-py v0.10.5 release and the
program status it checkpoints. Companion to `GwzM5-8ProgressReviewF5.md`
(pre-release program review) and `GwzMergeCheckpoint-v0.10.5.md` (the
checkpoint's own account, whose claims this review spot-verified).

## 1. Verdict

**v0.10.5 is a sound, honest, well-engineered release, and the
narrow-lineage strategy behind it was the right call.** Every checkpoint-doc
claim I tested verified: lineage purity, absence of the v1 surface, the
fail-closed version registry, pipeline evidence on the exact tag SHAs, a
complete artifact matrix, accurate release notes, and a correctly executed
v0.10.4 supersession that respects tag immutability. The strategy — release
from the last narrow pre-v1 checkpoint rather than the advanced development
line — converted a "98 Windows failures in unreleased code" problem into a
non-event for users.

## 2. What v0.10.5 is

The release is deliberately **not** cut from the development line. It is
`b0781b4` (the last commit before the test-only v1 model landed) plus a
standalone-lock refresh and a version bump — verified: the entire
`b0781b4..v0.10.5` diff touches only `Cargo.lock`/`Cargo.toml`. The −51k
"deletions" in `dev-line..tag` diffs are lineage divergence, not an excision
commit; both histories are preserved by design and the checkpoint doc
explicitly forbids merging them to "make the graphs look linear" — correct.

Shipped delta over v0.10.3 (core: +13,985/−4,771 across 139 files; cli:
+261/−22): the accepted stable prefix of the M5–8 program —

- **Coordinated custom merge messages** (M5a) — the feature release.
- **`diff --tagged` automatic repository selection.**
- **Strict merge-record envelope decoding** with the duplicate-aware YAML
  parser (`yaml-rust2`, the new dependency in the lock refresh) and clearer
  recovery diagnostics (record error context; protocol +129 lines).
- Behavior-preserving internals: R1 policy extraction, R2a integration/message
  seam, R4a acceptance extraction, plus the retained-reader compatibility
  harness tooling (+2.5k, scripts only).

What it does **not** contain — verified at the tag, not assumed: no
`model/v1`, no `v1_lifecycle`, no `MergeRecordProjection`/field-10 protocol
surface, no catalog/checked-artifact code. The `record_wire/` and
`acceptance/` modules present at the tag are the pre-v1 R3-envelope/R4a
pieces, consistent with the program's package ordering.

**Forward compatibility shipped**: the tag's `record_wire/header.rs` carries
the header-only registry for `gwz.merge-operation/v0`–`v4` with typed
`Unsupported` classification and tests (`tests/{decode,header_dispatch}.rs`).
A v0.10.5 binary that later meets a v1 record rejects it cleanly instead of
misreading it — the §15 doctrine's most important promise, kept in the last
release before v1 records can exist.

## 3. Verification record

| Claim | Result |
| --- | --- |
| Release lineage = checkpoint + lock + version | ✔ verified (2 commits, Cargo files only) |
| cli/py same pattern; tree delta Cargo-only | ✔ verified |
| cli pins core by tag; lock resolves exact SHA `4bcf13c` | ✔ verified |
| Release runs on exact tag SHAs | ✔ core `31820283530`, cli `31820277166`, py `31820277012` all success |
| cli artifact matrix | ✔ 5 targets + installers + per-asset sha256 + `sha256.sum` + source tarball |
| Release notes accuracy | ✔ states scope, exclusion, and supersession plainly |
| v0.10.4 handling | ✔ releases renamed "(superseded)", body says do-not-use + why; immutable tags retained |
| Tests at tag (macOS, local) | 711 unit + 10/4/2 suites green; one non-hermetic failure, see F1 |
| Fail-closed on future versions | ✔ registry + typed `Unsupported` + tests at tag |

(Local test evidence in the checkpoint doc is treated as credible per the
maintainer; the runs above were re-executed independently before that
instruction and agree with it.)

## 4. Current program status

Two lines, by design:

- **Released line (v0.10.5)**: stable v0 merge lifecycle + custom messages +
  strict decoding + `diff --tagged`. Users are unexposed to everything below.
- **Development line** (workspace tuple recorded in the checkpoint doc):
  R4b P1/P2 remediation closed; R2-C0 accepted; R2-C1 (catalog aggregate
  classifier, native case-fold correction) accepted with no open P0–P2;
  **R2-C2 is NO-GO** on a real TOCTOU class: after final source-object
  verification, publication drops the verified handle and renames by path —
  a fault hook can substitute the path in that window and move foreign bytes
  into the authoritative destination. The accepted correction (bootstrap
  amendment §4.1) is one sealed, substitution-proof publication primitive for
  all four edges — handle-exact rename on Windows; Unix keeps the checked
  handle across a relative rename under an explicit cooperating-same-user
  trust boundary (no portable compare-object-and-rename exists). Sound, and
  consistent with the program's exactness doctrine; the checkpoint's
  instruction to *not* fix it by stacking more path re-checks is the right
  architectural line.
- **Windows backlog**: the C1-era tree compiles on Windows only with two
  isolated corrections (UTF-8 `String` for native names; unstable metadata
  identity APIs) and then shows 98 v1/checked-artifact test failures. Recorded
  honestly as an implementation/test-design backlog rather than papered over;
  the two corrections deliberately quarantined in the diagnostic clone rather
  than blind-merged. Good discipline on both counts.
- **Resume order** (per checkpoint doc): fix the C2 race with the shared
  primitive → full per-fault interruption/recovery matrix → two settled
  reviews → port the Windows corrections deliberately → classify the Windows
  matrix → only then R4b-G, M5b, A1. This ordering is correct; note it makes
  explicit that the earlier fault-key inventory was not an executed matrix.

Prior-review findings that remain open on the dev line and travel across the
checkpoint: the stale I2 contracts (top P1 from `GwzM5-8ProgressReviewF5.md`),
the operator-escape story, the durable preservation cursor question, and the
panic-style invariant audit before A1.

## 5. Findings (release-scoped)

- **F1 (P2, test design)** — `generated_protocol_is_current` silently uses
  the wrong generator when the expected sibling checkout is absent: it sets
  `PYTHONPATH` to `<parent>/taut/src` without checking existence, so outside
  the workspace layout Python falls back to whatever `taut` is installed and
  the test reports a misleading stale/current verdict in both directions.
  Meanwhile the release workflow pins `taut-proto==0.8.1` from PyPI — two
  generator sources of truth that can drift. Make the test fail loudly when
  the source tree is missing and assert the generator version it actually
  imported.
- **F2 (P3, coverage)** — the core release workflow validates Ubuntu +
  Windows only, while gwz-cli publishes macOS binaries; core's suite runs on
  macOS only informally (the maintainer's local runs, and mine). Add a macOS
  lane to core release CI, or record the local-platform evidence explicitly
  in the checkpoint template.
- **F3 (P3, hygiene)** — three direct `std::fs::write` sites at the tag
  (`git/gitbackend/merge_prepared.rs:238` — the MERGE_HEAD write —
  `workspace_ops/workspace_bootstrap.rs:151,157`). Flagged by clippy's
  disallowed-methods machinery in my environment; regardless of lint
  plumbing, non-durable direct writes are worth migrating to the durable
  write helpers in a codebase whose doctrine is exact durability. (MERGE_HEAD
  matches git's own non-fsync behavior, so this is informational.)
- **Positive pattern worth keeping** — the `b4e83e0` workflow carve-out
  skips the checked-artifact gate for exactly `v0.10.5`, keeps every
  historical gate mandatory, and is itself pinned by a test that asserts the
  workflow text (`tests/publish_workflow.rs`). Tag-exact, self-verifying
  exceptions are how release-gate carve-outs should be done. Likewise the
  disposable `v0.10.5-preflight.*` tag pattern (create, validate, delete
  after the real release) is clean.

## 6. Bottom line

The release engineering matched the quality of the program's design work:
narrow lineage, verified evidence, honest notes, immutable supersession, and
a checkpoint document that records not just what shipped but exactly where
the unreleased work stands and how to resume it. Users got a safe,
incremental release; the program kept its integrity. The open risks are the
ones already on file for the development line — R2-C2's race fix, the
Windows matrix, and the pre-A1 checklist — none of which leak into what was
shipped.
