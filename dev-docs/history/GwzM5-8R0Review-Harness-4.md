# R0 retained-reader post-CI remediation review

Date: 2026-08-02

## Verdict

No P0, P1, P2, or P3 defect remains in the reviewed post-CI remediation.
The changes are narrow, portable, and fail closed:

- raw Git commit-object input is now passed as explicit UTF-8 bytes, so Python
  cannot translate LF to CRLF on Windows;
- temporary-tree cleanup retries only a `PermissionError`, clears the
  read-only bit on the owned path, and propagates all other error classes;
- a failed matrix prints its complete machine-readable summary and returns
  before evidence provenance, evidence output, or attestation output is
  constructed; and
- regenerated checked evidence is bound to the current generator and matrix
  sources without changing fixtures or behavioral results.

The remediation is approved for commit and push, followed by a rerun of the
`Retained merge readers` workflow on that exact commit. It does not yet justify
accepting R0 or dispatching R1: CI run 30684723533 did not expose the underlying
Linux reader failures, and the new diagnostic path must reveal those results
on the rerun. Any genuine behavioral failures reported there remain separate
work.

## Finding classification

- **P0:** none.
- **P1:** none.
- **P2:** none.
- **P3:** none.

## Review of the fixes

### Binary Git stdin is the correct Windows portability fix

`generate_retained_reader_fixtures.py:75-88` no longer invokes `git
hash-object` with `text=True`. It encodes the canonical commit payload once and
passes bytes to `subprocess.run`; stdout is decoded as ASCII because the only
successful value is an object ID, while error output uses replacement-safe
UTF-8 decoding.

This preserves the exact existing LF bytes on POSIX and prevents the Windows
text wrapper from converting header separators to CRLF. It therefore fixes the
`unterminatedHeader` failure without changing the canonical commit payload or
its object identity. The regenerated fixture identities and all checked
behavioral results are unchanged.

The regression at
`test_retained_reader_adversarial.py:149-161` verifies that the subprocess
receives the exact encoded payload and that text mode is absent. This tests the
relevant Python portability boundary directly instead of depending on the host
OS to reproduce newline translation.

### Read-only cleanup is narrow and fail closed

`generate_retained_reader_fixtures.py:91-102,329-354` installs an `onerror`
callback only for deletion of the generator-owned temporary tree. The callback
accepts only `PermissionError`, makes that path owner-writable, and retries the
original removal operation. If the retry fails, its error escapes. If the
original failure is any other exception class, the exact exception is
re-raised without chmod or suppression.

This is portable for the supported Python 3.10 workflow and current Python
runtimes: `shutil.rmtree(..., onerror=...)`, `os.chmod`, `stat.S_IWRITE`, and
the callback signatures are available on Windows and POSIX. On Windows the
chmod clears the read-only attribute that prevented Git object deletion; on
POSIX it grants the owner write permission before retry. It does not use
`ignore_errors`, catch a broad exception, or suppress a persistent sharing or
permission failure.

The regression at
`test_retained_reader_adversarial.py:163-176` proves the write bit is present
before retry and that the path is removed. An independent negative
reproduction also passed the same callback an `OSError` that was not a
`PermissionError`; the exact exception object was re-raised.

### Failed summaries precede and prevent evidence promotion

`retained_reader_matrix.py:271-312` now checks the aggregate matrix status
immediately after `run_matrix`. A failed result is serialized to stdout and
returns exit code 1 at lines 285-287. All evidence and attestation work begins
later, at lines 288-305, and is therefore unreachable on a failed run.

The regression at `test_retained_reader_matrix.py:385-416` runs the real CLI
with an intentionally mutating reader and `--evidence-out`. It asserts exit 1,
a complete JSON failure summary containing the underlying unexpected-mutation
diagnostic, and absence of the evidence file.

I repeated the failure with both `--evidence-out` and `--attestation-out`, with
valid synthetic CI identity environment variables. The command returned 1,
stdout contained the complete failed result, stderr was empty, and neither
output path existed. Evidence and attestation are therefore never written by a
failed run. An already-existing caller-owned output is not deleted, but the
workflow uses fresh runner-temporary paths and the contract is non-promotion,
not destructive cleanup of pre-existing files.

## Evidence and budget integrity

The checked `evidence-macos-aarch64.json` source mapping exactly matches the
current reviewed evidence-source set. Its `source_set_sha256` and
`evaluator_sha256` both equal the recomputed aggregate digest, and its
`generator_sha256` equals the current generator digest. The document remains
canonical JSON.

The evidence diff changes only the two implementation source identities, their
aggregate evaluator/source-set identity, and the intentionally volatile local
Python version/executable identity. All 38 result rows, artifact identities,
fixture identities, input identities, snapshot identities, and semantic
outcomes remain identical to the prior checked evidence. The CI portable
projection deliberately excludes only Python version/executable digest and Git
version, so a fresh macOS arm64 run must still match every durable and
behavioral field.

The retained-reader directory is exactly **6,600 physical lines across 23
files**, matching—not exceeding—the frozen closeout ceiling in
`GwzM5-8ChangeBudget.md`. The 647-line checked evidence is included, and the
largest Python file is 470 lines; every implementation and test module remains
below the 500-line review trigger.

## CI run 30684723533 disposition

The failed run was for commit `bbc710617f17235cd850e8d768b40038e4bb287a`.
Its results divide into three categories:

1. **Already successful and unaffected:** both Ubuntu and Windows harness unit
   jobs passed. The macOS x86_64 and macOS arm64 reader jobs passed; macOS arm64
   also passed the portable checked-evidence comparison and uploaded evidence
   plus attestation.
2. **Windows matrix failure addressed by this remediation:** fixture generation
   failed because Python text-mode stdin changed the raw commit-object payload,
   causing Git to report `unterminatedHeader`. Cleanup then raised a secondary
   `PermissionError` on a read-only loose Git object, obscuring the primary
   failure. Binary stdin fixes the object bytes and the narrow cleanup callback
   fixes only the read-only cleanup case. Actual Windows execution on the new
   commit remains the required platform proof.
3. **Linux diagnostic failure addressed, underlying result unresolved:** Linux
   x86_64 and Linux arm64 reached the actual-reader matrix, which returned a
   failed summary. The old CLI then attempted evidence promotion, replaced the
   useful summary with `only a passing matrix run can become retained-reader
   evidence`, and wrote no artifact. The new early return preserves the
   fail-closed no-evidence behavior while printing the complete reader/case
   failures. It intentionally does not guess at or mask their cause. The rerun
   must be inspected and any reported behavioral incompatibility remediated
   before R0 acceptance.

Thus run 30684723533 remains a failed, non-acceptance run. Its Windows root and
cleanup causes have direct fixes, and its diagnostic-loss cause has a direct
fix. The incomplete Linux diagnosis is a reason to rerun, not a defect that
blocks committing the diagnostic remediation.

## Independent verification

Run locally from `gwz-core` on macOS arm64:

- full retained-reader suite: **65 passed**;
- manifest validation: **passed**, 36 tuples;
- `gate-ready`: **passed**, 36 tuples;
- binary-stdin regression: **passed**;
- read-only cleanup regression: **passed**;
- independent non-`PermissionError` propagation check: **passed**;
- failed-summary/evidence-suppression regression: **passed**;
- independent failed run requesting both evidence and attestation: **exit 1,
  complete JSON summary, neither file written**;
- checked evidence current-source and canonical-format validation: **passed**;
- fixture identities and all 38 result rows versus prior evidence:
  **unchanged**; and
- retained-reader budget: **exactly 6,600 lines / 23 files**.

## Decision

**Approve commit: yes. Approve push: yes. Approve exact-SHA CI rerun: yes.**

**Approve R0 acceptance or R1 dispatch now: no.** First require the rerun to
pass both harness jobs and all five platform lanes, including Windows fixture
generation/reader execution, full Linux diagnostic results, macOS arm64
portable comparison, and evidence-plus-attestation upload for every successful
lane.
