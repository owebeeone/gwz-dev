# GWZ M5–M8 R0 Post-CI Portability Re-review

Date: 2026-08-02

Scope:

- the current portability remediation in
  `gwz-core/scripts/retained_readers/generate_retained_reader_fixtures.py`;
- failed-run handling in
  `gwz-core/scripts/retained_readers/retained_reader_matrix.py`;
- the associated adversarial and matrix tests;
- regenerated `evidence-macos-aarch64.json`; and
- the previously frozen retained-reader fixture, result, and source-provenance
  contracts.

## Verdict

There are **no P0, P1, P2, or P3 findings** in the reviewed portability
remediation.

The changes are approved to commit and push. After pushing, rerunning the
complete `Retained merge readers` workflow on the new exact SHA is approved and
required. The prior CI failure means R0 still needs new platform evidence; this
review establishes implementation readiness, not a substitute for the Windows,
Linux, and macOS executions.

## Findings by severity

- **P0:** none.
- **P1:** none.
- **P2:** none.
- **P3:** none.

## Canonical Git commit bytes

`_git_input` now encodes the intended commit payload once as UTF-8 bytes and
passes those bytes directly to `subprocess.run` without `text=True`. Python
therefore does not insert Windows CRLF sequences before Git hashes the object.
Git stdout remains bytes and is decoded only after execution as the expected
ASCII object ID.

I generated the complete fixture set, reconstructed the prepared commit payload
from its exact tree, parents, identity, timestamp, and message, and compared it
with `git cat-file commit` output from
`custom-message-pending-completed`. The object bytes were exactly equal, had no
CRLF sequences, and produced the frozen commit ID:

```text
7430611636cbebd4ed75e8b889f43e0779cfe38f
```

This is the important portability boundary: Git receives the same LF-delimited
commit bytes on Windows, macOS, and Linux. Existing fixture identities remain
unchanged, so the remediation corrects transport without changing the reviewed
compatibility data.

The new regression test also inspects the actual subprocess arguments and
requires byte input with no text-mode option. Together with the real-object
probe and frozen object ID, this adequately protects against reintroducing
newline translation.

## Temporary-tree cleanup

The cleanup handler is safely bounded:

- it is installed only on `shutil.rmtree` for the generator-created private
  temporary tree after generation fails;
- it checks the actual exception object and handles only `PermissionError`;
- all other errors are re-raised immediately, before chmod or retry;
- for a permission failure it adds the write bit only to the path supplied by
  `rmtree` and retries the original failing operation once; and
- if chmod or the retry does not resolve the failure, that exception propagates
  rather than being ignored.

This is the conventional Windows read-only Git-object cleanup and does not
turn unrelated filesystem failures into successful generation. I independently
injected a non-permission `OSError` and confirmed that the same exception was
re-raised with neither `chmod` nor the removal callback invoked. The checked
read-only-file test confirms that a genuine read-only path becomes writable
before the single retry.

## Failed matrix visibility and non-promotion

When `run_matrix` returns a failed summary, `main` now serializes that complete
summary to stdout and returns exit status 1 before collecting provenance or
constructing/writing evidence. The summary retains the individual reader,
case, status, and exact evaluation errors instead of replacing them with the
generic `build_evidence` rejection.

I invoked the CLI with a deliberately mutating reader and both
`--evidence-out` and `--attestation-out`. The observable result was:

```json
{
  "case": "probe",
  "errors": ["unexpected mutation: text:changed"],
  "reader": "reader",
  "status": "failed"
}
```

The process returned 1, the top-level status was `failed`, and neither the
evidence path nor the attestation path was created. Because the early return
precedes both evidence normalization and attestation construction, no failed
matrix result can be promoted through those output paths.

Invalid manifests or cases still fail through the existing diagnostic error
path; the remediation correctly targets completed matrix runs whose individual
rows failed.

## Checked evidence and source binding

The regenerated checked evidence is consistent with the remediation:

- its generator digest equals the current generator source;
- its matrix digest equals the current matrix source;
- all 14 individual evaluator-source digests equal the current files;
- `source_set_sha256` and `evaluator_sha256` equal the aggregate of that current
  source mapping;
- the fixture mapping and fixture-set digest remain unchanged and match fresh
  generation; and
- the exact 38-row result contract remains 37 passing executable cases plus
  the one declared-undistributed `gwz-py` v0.10.0 row.

I reran the complete macOS arm64 matrix using the runtime recorded by the
checked artifact, CPython 3.14.3. It returned 37 passes, one declared
unsupported result, and zero failures. The fresh evidence passed the portable
comparison and was byte-for-byte identical to the checked file, with SHA-256:

```text
4d59338e472178933933f6bb3cfdb03a69dca6a4c2a4027331654b9f9dcea0e2
```

The checked evidence therefore remains reproducible and cannot silently remain
stale after either of the two evidence-affecting source changes.

## Verification performed

- retained-reader unit/adversarial suite: **65 passed**;
- Python syntax compilation for all retained-reader modules: **passed**;
- manifest validation: **valid, 36 tuples**;
- `gate-ready`: **manifest-ready, 36 tuples**;
- real Git commit payload/OID comparison: **exact bytes, no CRLF, frozen OID**;
- non-`PermissionError` cleanup injection: **exception propagated; no chmod or
  retry**;
- failed matrix with requested evidence and attestation: **exact row error
  emitted; both output paths absent**; and
- full macOS arm64 retained-reader matrix and checked-evidence comparison:
  **37 passed, one declared unsupported, portable-equal, byte-identical**.

## Approval decision

- **Commit:** approved.
- **Push:** approved.
- **Rerun the full retained-reader CI workflow on the pushed SHA:** approved
  and required.
- **Accept R0 or dispatch R1 before that rerun succeeds:** not approved.
- **Accept R0 and dispatch R1 after all required committed-SHA jobs produce
  passing evidence and attestations:** approved from the compatibility-review
  side.
