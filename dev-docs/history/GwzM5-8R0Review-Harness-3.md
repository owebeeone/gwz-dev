# R0 retained-reader harness final remediation re-review

## Verdict

No P0, P1, P2, or P3 implementation defect remains in the reviewed
retained-reader harness scope. Both residual P2 findings from
`GwzM5-8R0Review-Harness-2.md` are closed.

The harness now binds checked evidence to the exact unique expected result
set, the complete reviewed evaluator/source set, current manifest and cases,
and regenerated canonical fixtures. Its portable comparison excludes only the
three reviewed host-volatile values while retaining results, artifacts,
snapshot identities, semantic mutation identities, and all other provenance.
The workflow invokes that comparison for freshly generated macOS arm64
evidence before upload.

Critical durable outputs are now checked by expected semantic digests rather
than path presence alone. The cases pin accepted lock/member content,
publication marker sets, root publication commits, Git indexes, and complete
archived merge records. The normalized mutation identity also includes the
semantic content of every exact changed path and the observed count plus
content digest for every bounded dynamic class.

From this review's scope, the R0 retained-reader checkpoint is ready to be
committed as a candidate checkpoint. Committing it is not the same as accepting
R0 or dispatching R1: the external workflow must still pass on that exact
committed SHA as described below.

## Finding classification

- **P0:** none.
- **P1:** none.
- **P2:** none.
- **P3:** none.

## Residual P2 disposition

### Harness-2 P2-1 — stale or incomplete checked evidence: closed

`retained_reader_evidence.py` now derives the expected result keys from the
manifest, cases, and platform, including the explicit unsupported tuple, and
requires exact set equality plus uniqueness. A result cannot be duplicated to
hide an omitted reader/case result.

The evidence provenance now carries and validates a complete reviewed source
mapping and aggregate source-set/evaluator digest. This includes the generator,
harness, matrix, evidence, process, runtime, schema, fixture, semantic and YAML
modules plus the checked schemas and fixture contract. Manifest and case bytes
remain independently pinned by their input digests, and regenerated fixture
identities are compared with evidence provenance.

The portable projection removes only Git version, Python version, and Python
executable digest. The CI workflow compares fresh macOS arm64 evidence against
the checked projection before it uploads evidence and attestation. Result,
artifact, input, fixture, source, platform, runtime architecture, snapshot, and
post-mutation identities remain comparison-sensitive.

Relevant implementation and regression coverage:

- `gwz-core/scripts/retained_readers/retained_reader_evidence.py:21-36,
  47-57,116-235,358-384`;
- `gwz-core/scripts/retained_readers/test_retained_reader_evidence_integrity.py:24-49`;
- `gwz-core/scripts/retained_readers/test_retained_reader_cases.py:24-65`; and
- `gwz-core/.github/workflows/retained-readers.yml:97-106`.

Independent reproduction replaced the final checked result with a duplicate of
the first. The workflow comparator rejected it with the missing required
`gwz-py-v0.10.2`/`v0-explicit-archived-record-gc` key. The same comparator
accepted an unmodified checked projection against itself using freshly
generated fixtures.

### Harness-2 P2-2 — critical durable contents were not pinned: closed

Mutation normalization is now content-sensitive. Exact changed paths carry a
portable semantic identity of the resulting file, ref, reflog, index, Git
object graph, YAML document, archive, or absence. Dynamic Git-object and marker
classes retain their observed counts and a digest of normalized contents.

The mutating cases add independent expected semantic postconditions for:

- the complete accepted `gwz.lock.yml` and required member fields;
- the exact normalized publication marker set;
- root branch, parent, message, and normalized commit/tree content;
- root Git index content;
- complete archived merge-record content, including unknown fields and
  publication consistency checks; and
- the v0.9.2 legacy lock rewrite and root index.

These observations are included in `after_invariant_sha256`, so a durable
content change cannot retain the same normalized passing evidence. A malformed
or semantically wrong lock/archive fails the case before evidence can be
promoted.

Relevant implementation and regression coverage:

- `gwz-core/scripts/retained_readers/retained_reader_semantics.py:1-290`;
- `gwz-core/scripts/retained_readers/retained_reader_fixture.py:285-417`;
- `gwz-core/scripts/retained_readers/retained_reader_matrix.py:153-187`;
- `gwz-core/scripts/retained_readers/cases.json`; and
- `gwz-core/scripts/retained_readers/test_retained_reader_evidence_integrity.py:52-131`.

The negative tests prove that changing an otherwise unknown archived-record
field, changing an accepted lock member commit, or changing marker meaning is
detected, while intentionally variable marker UUIDs normalize to the same
semantic identity.

## Other reviewed contracts

The previously closed safety work remains intact:

- complete reader/platform tuples and frozen support classifications;
- fail-closed schema validation and typed JSON outcome evaluation;
- checksum-verified raw-object caching with fresh derived trees and virtual
  environments;
- reviewed immutable artifact URL forms and minimal hostile-Git-resistant
  execution environments;
- descendant process termination on timeout; and
- platform-normalized archive collision and unsafe-entry rejection.

The workflow continues to run harness tests on Ubuntu and Windows and schedules
the Linux x86_64, Windows x86_64, Linux arm64, macOS x86_64, and macOS arm64
reader lanes. Each lane writes normalized evidence and a separate attestation
bound to the evidence bytes and CI commit/run identity.

## Independent verification

Run locally from `gwz-core` on macOS arm64:

- full retained-reader unit suite: **62 passed**;
- manifest validation: **passed**, 36 tuples;
- `gate-ready`: **passed**, 36 tuples;
- checked evidence validation against regenerated fixtures and current source
  digests: **passed**;
- portable checked-evidence comparison through the workflow CLI: **passed**;
- duplicate-result/omitted-result reproduction through that CLI: **rejected**;
- stale source-digest, corrupt accepted-lock, altered unknown archive field,
  and semantically changed dynamic-marker tests: **passed**; and
- hostile Git environment, derived-cache poison, timeout descendant, and
  Windows archive-alias regression tests: **passed**.

I did not reacquire and execute every external release artifact in this local
re-review. That is intentionally the separate committed-SHA CI gate, not an
implementation defect or a reason to withhold the candidate commit.

## Required external committed-SHA gate

After committing and pushing the candidate checkpoint, do not mark R0 accepted
or dispatch R1 until the `Retained merge readers` workflow succeeds on that
exact commit with:

1. `Harness unit tests (ubuntu-24.04)`;
2. `Harness unit tests (windows-2022)`;
3. `linux-x86_64` actual-reader evidence;
4. `windows-x86_64` actual-reader evidence;
5. `linux-aarch64` supported-artifact evidence;
6. `macos-x86_64` supported-artifact evidence; and
7. `macos-aarch64` supported-artifact evidence, including equality with the
   checked portable projection.

Every platform lane must upload both normalized evidence and its attestation,
with the attestation naming the same commit/run and hashing the uploaded
evidence bytes. Windows arm64 remains an explicit unsupported manifest tuple,
not a missing or skipped job. Repository rules must require this workflow, and
the coordinated release gate must use its `workflow_call` entry for relevant
cross-repository Rust/Python driver changes.
