# GWZ M5–M8 R0 Retained-Reader Compatibility Re-review

Date: 2026-08-01

Scope:

- every finding in `dev-docs/GwzM5-8R0Review-Compat.md`;
- the remediated retained-reader fixtures, cases, evaluator, runtime isolation,
  evidence, and adversarial tests under
  `gwz-core/scripts/retained_readers/**`; and
- `gwz-core/.github/workflows/retained-readers.yml`.

## Verdict

There are **no P0, P1, or P2 implementation findings** in the remediated R0
retained-reader checkpoint. The checkpoint is ready to commit. The local
macOS arm64 lane is reproducible with the exact recorded runtime: 37 executable
reader/case results pass, the undistributed `gwz-py` v0.10.0 tuple is explicitly
reported as unsupported, and the newly generated normalized evidence is
byte-for-byte identical to `evidence-macos-aarch64.json`.

R1 is **not yet ready to dispatch**, for a different reason: the mandatory
platform evidence cannot exist until this checkpoint has a committed SHA. Run
the committed workflow and require passing evidence plus a digest-bound
commit/run attestation from Linux x86_64, Windows x86_64, Linux arm64, macOS
x86_64, and macOS arm64. That is the remaining R0 acceptance gate, not a defect
that should prevent committing the code needed to run it.

Two P3 assurance gaps remain. Neither blocks the checkpoint commit or R1 on
its own, but the first should be inexpensive to correct before calling R0
permanently closed, and the second should be closed before M5a activation.

## Findings

### [P3-1] The checked-evidence freshness test does not enforce all provenance bindings it claims

**Where:**
`gwz-core/scripts/retained_readers/test_retained_reader_cases.py:34-65` and
`gwz-core/scripts/retained_readers/README.md:152-158`.

The checked evidence now contains the required fixture-set, per-fixture,
generator, evaluator, Git, platform, and exact Python-runtime identities. I
independently recomputed the portable identities and reproduced the complete
evidence byte-for-byte with the recorded CPython 3.10.15 executable, so the
current evidence is valid.

The automated freshness test, however, compares only the current manifest and
cases hashes. It checks that some provenance fields exist, but does not compare
the checked fixture mapping/set with `fixture-contract.json`, recompute the
current generator hash, or recompute the evaluator hash. A later generator or
evaluator edit that does not change fixture bytes or case data can therefore
leave stale checked evidence while this test remains green. That is weaker
than the README's statement that unit tests bind the checked evidence to the
current fixtures, generator, and evaluator.

**Recommended correction:** extend the checked-evidence test to compare its
fixture mapping and set digest with `fixture-contract.json`, and compare its
generator/evaluator digests with values recomputed from the same centrally
declared input set used by `collect_provenance`. Keep host-specific runtime and
workflow identity in generated evidence/attestation rather than making a
portable unit test depend on the local host.

### [P3-2] Archive status and GC still do not use a custom-message archive

**Where:**
`gwz-core/scripts/retained_readers/generate_retained_reader_fixtures.py:286-316`
and `gwz-core/scripts/retained_readers/cases.json:232-267`.

The new preserve case does use the executed custom-message fixture and passes
through all three durable-v0 readers. Archive-only status and explicit GC are
also now exercised for all three readers, with exact no-mutation/deletion
policies. The archived fixture is nevertheless copied from
`no-ff-fast-forwardable`; it contains `mode: no_ff` and `frozen no-ff message`,
not the custom-message record requested by the original P3 correction.

This does not undermine the observed command behavior, and the custom-message
abort cases do prove that old readers can produce the archive. It leaves the
strict §15.1.3 cross-product incomplete: no retained-reader case subsequently
loads or deletes that custom-message archive.

**Recommended correction:** add or substitute an archived fixture derived
from the custom-message record and run archive status and explicit GC through
v0.10.0 Rust and v0.10.2 Rust/Python. Retain the present archived no-ff case if
its dormant-field coverage is independently valuable.

## Disposition of the original findings

| Original finding | Re-review disposition |
| --- | --- |
| P2-1: required cross-platform evidence absent | Workflow implementation is ready and now emits evidence plus a separate SHA/run attestation on all five supported lanes. Execution remains a mandatory post-commit R0 gate. |
| P2-2: abort never reconciles an executed custom-message commit | Resolved. The exact two-parent commit fixture is adopted and rolled back; its wrong-message twin returns typed `MergeDrift` with no mutation. |
| P2-3: v0.10.0 rewrites do not prove unknown-field preservation | Resolved. Every applicable v0.10.0 rewrite/archive case compares all four baseline fields exactly, with adversarial drop/alter tests. |
| P2-4: evidence does not bind fixtures or runtime | Substantively resolved. Evidence records canonical fixture/generator/evaluator identities, Git SHA-1/version, actual platform, and exact CPython identity/binary digest; runtime derivation is fresh and interpreter-keyed. The residual automated freshness assertion is P3-1. |
| P3-1: preserve/GC applicability and mutation bounds are weak | Mostly resolved. Required command coverage is frozen, preserve and GC execute, mutation policies are exact/bounded, and semantic postconditions are recorded. The custom-message archive cross-product remains P3-2. |

## High-risk behavior verified

- **Executed-commit abort:** v0.10.0 Rust, v0.10.2 Rust, and v0.10.2 Python
  each return `Aborted`/`RolledBack`, restore the exact first parent, remove the
  feature path, and archive the operation.
- **Wrong-message fail-closed behavior:** all three readers return
  `MergeDrift`; the two-parent commit and wrong message remain, the open record
  remains, no archive appears, and the tree snapshot is unchanged.
- **Unknown additive fields:** all applicable v0.10.0 archive-producing cases
  preserve exact `lock_yaml`, `manifest_yaml`, `lock_commit_sha256`, and
  `manifest_commit_sha256` values. The adversarial test fails both field removal
  and value alteration.
- **Preserve and GC applicability:** every durable-v0 reader has an applicable
  preserve case and explicit GC case; case validation fails if a frozen command
  loses coverage. Mutations are exact or class-bounded rather than broadly
  allowed.
- **Fixture/runtime provenance:** the generated fixture contract, checked
  evidence, current generator hash, current evaluator hash, Git 2.52.0 SHA-1
  format, macOS arm64 identity, and CPython 3.10.15 executable digest all agree.
  Derived reader trees and Python environments are fresh per run; only verified
  raw artifacts persist.
- **Separate attestation:** normalized evidence remains self-reference-free.
  CI requires non-null `GITHUB_SHA` and `GITHUB_RUN_ID`, writes an attestation
  containing the evidence digest/platform/commit/run, and uploads both files
  for every platform lane.

## Verification performed

- `python3 -m unittest discover -s scripts/retained_readers -p 'test_*.py' -v`:
  **57 passed**.
- `retained_reader_harness.py gate-ready`: **manifest-ready**, 36 declared
  surface/release/platform tuples.
- Full offline macOS arm64 retained-reader matrix using the exact recorded
  `/Users/owebeeone/.asdf/installs/python/3.10.15/bin/python3.10` runtime:
  **37 passed, one declared unsupported, zero failures**.
- Generated evidence compared with the checked
  `evidence-macos-aarch64.json`: **byte-identical**.

## Gate decision

- **Commit the R0 checkpoint:** approved; no P0/P1/P2 implementation blocker.
- **Dispatch R1 immediately after that commit:** not approved yet.
- **Dispatch R1 after all five committed-SHA workflow lanes pass and their
  evidence/attestations are retained:** approved from the compatibility side;
  the two P3 items do not block R1 extraction.
