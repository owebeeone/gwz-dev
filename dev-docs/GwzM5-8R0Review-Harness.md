# R0 retained-reader harness review

## Verdict

The foundation is directionally strong: raw artifacts are SHA-256 checked on
every cache read, downloads are staged and atomically installed, archive
extraction rejects traversal and links, command templates are passed as argv
rather than through a shell, Python wheels are materialized under their real
distribution names, fixtures are copied per invocation, and each direct reader
process has closed stdin and a timeout.

It is not yet an R0 acceptance gate. There are no P0 or P1 findings, but the
P2 findings below permit required tuples, assertions, fixture identity, output
semantics, mutations, or executed cached code to escape the claimed proof.
Under the documented `R0 -> R1` sequence, these P2s block declaring R0 complete
and therefore block the R1 dispatch gate. They do not imply a defect in the
v0.9.2/v0.10.x release binaries themselves.

## Findings

### [P2-1] A reader can omit a platform tuple and still be gate-ready

**Where:** `gwz-core/scripts/retained_readers/retained_reader_harness.py:191-201,
293-304`; `gwz-core/scripts/retained_readers/test_retained_reader_harness.py:85-95`;
`gwz-core/.github/workflows/retained-readers.yml:45-56`

`validate_manifest` rejects duplicate tuples but never requires the full
`reader x platform` cross-product. `gate_readiness_errors` only visits tuples
that happen to exist. The unit assertion deliberately enumerates only five
platforms and omits `windows-aarch64`; the workflow likewise has no job for
that historically unsupported platform. Removing
`rust-cli:v0.9.2:windows-aarch64` therefore leaves both validation and
`gate-ready` green:

```text
validate: PASS
gate errors: []
tuple present: False
```

This violates the explicit-unsupported/fail-not-skip contract. Require exactly
one artifact declaration for every reader/platform pair. A required tuple must
be verified; an unsupported tuple must remain present with its reason and
substitute evidence. Also pin the known support classification so changing a
distributed tuple from `required` to `unsupported` cannot silently turn a
required lane into a passing skip.

### [P2-2] The supposedly frozen fixtures have no canonical identity

**Where:** `gwz-core/scripts/retained_readers/test_retained_reader_cases.py:74-88`;
`gwz-core/scripts/retained_readers/retained_reader_evidence.py:91-100`;
`gwz-core/scripts/retained_readers/cases.json`; `gwz-core/scripts/retained_readers/evidence-macos-aarch64.json`

The determinism test generates the fixtures twice with the same current
generator and compares those two results. It has no reviewed golden digest.
The normalized evidence records only manifest and cases digests; it omits the
fixture tree digests, generator/harness revision, and the before/after snapshot
digests already present in the raw matrix result. None of the checked cases
pins `before_sha256` or `after_sha256`.

Consequently a generator edit can replace the custom-message or legacy
`no_ff` fixture and still produce byte-identical normalized evidence whenever
the coarse outcome/postconditions remain the same. Regenerating and `cmp`-ing
the macOS evidence does not catch that change because the evidence contains no
fixture identity. This does not satisfy the R0 requirement to freeze those
fixtures.

Check in a reviewed, platform-portable logical snapshot digest for every
fixture (or immutable fixture archive digest), require it before execution,
and carry the fixture-set digest plus evaluator/generator identity into
evidence. Retain before/after semantic snapshot digests in normalized evidence
where the case is mutating.

### [P2-3] The runtime validators do not enforce the checked schemas, so assertions can be ignored

**Where:** `gwz-core/scripts/retained_readers/retained_reader_matrix.py:207-233`;
`gwz-core/scripts/retained_readers/retained_reader_harness.py:81-201`;
`gwz-core/scripts/retained_readers/cases.schema.json`;
`gwz-core/scripts/retained_readers/manifest.schema.json`;
`gwz-core/.github/workflows/retained-readers.yml:33-37`

The JSON Schemas correctly use `additionalProperties: false` and define the
expectation/postcondition variants, but CI never validates with those schemas.
The handwritten validators cover only a subset. For example, a case containing
misspelled `postcondition` instead of `postconditions` passes
`validate_cases`; the intended semantic assertion is then ignored. The same
class of drift exists for unrecognized manifest properties and several nested
case fields.

Make one validator authoritative and schema-complete. Either run Draft 2020-12
validation in the gate and keep only referential/semantic checks in Python, or
make the stdlib validator reject all unknown/missing properties and validate
every union exactly. Add negative tests proving a misspelled postcondition,
invalid stream mode/value, malformed mutation policy, duplicate reader, and
unknown manifest field fail before any reader runs.

### [P2-4] `--json` compatibility cases can pass on invalid JSON, and evidence accepts a null outcome

**Where:** `gwz-core/scripts/retained_readers/cases.json:5-15,18-31,42-55,66-76,
83-97,106-119,127-157`; `gwz-core/scripts/retained_readers/retained_reader_fixture.py:93-110`;
`gwz-core/scripts/retained_readers/retained_reader_evidence.py:16-36,71-100`

Every checked invocation requests `--json`, but every stdout expectation uses
substring containment. A fake result containing only
`merge_retained Halted mem_member` passes the status expectation despite not
being JSON. `build_evidence` then records `outcome: null` and still returns
passing evidence. The adversarial check produced:

```text
expectation errors: []
normalized outcome: None
```

Parse every JSON/JSONL projection and assert the required typed fields and
values. Evidence creation must reject a missing/unparseable required outcome,
not normalize it to null. The expected shape may deliberately account for
Rust/Python casing differences in the pre-record lane, but it must do so as a
typed contract rather than a substring search.

### [P2-5] Broad allowlists can hide unrelated destructive mutation

**Where:** `gwz-core/scripts/retained_readers/cases.json:27-38,51-62,93-102,
115-123,153-162`; `gwz-core/scripts/retained_readers/retained_reader_fixture.py:126-151`

The mutating continue/legacy cases allow all of `text:member/*`, most root Git
state, all merge state, and sometimes all workspace configuration. The
postconditions prove the expected HEAD/parent/message and a few paths, but an
old reader could also remove another ref, corrupt repository config, rewrite
unasserted archive evidence, or alter another workspace file and still pass.
The snapshot machinery detects these changes; the case policy then permits
them.

Freeze exact changed-path sets plus exact semantic digests for durable records,
accepted lock/manifest bytes, relevant refs/config, and archives. Where Git
implementation details require an allowlist, exclude immutable/unrelated
subtrees explicitly and add postconditions covering every allowed durable
output. Do not use `member/*` or `.git/*` as the final release-gate boundary.

### [P2-6] Content-addressed raw objects can execute unverified derived cache contents

**Where:** `gwz-core/scripts/retained_readers/retained_reader_matrix.py:115-197,
243-279`; `gwz-core/scripts/retained_readers/retained_reader_harness.py:321-360`;
`gwz-core/.github/workflows/retained-readers.yml:64-68`

The raw archive/wheel object is rehashed correctly. Once a Rust tree exists,
however, `_prepare_reader` skips extraction and executes the existing entry
point without checking it against the archive. A cached Python environment is
accepted from a JSON identity marker and entry-point existence alone; its
installed files are not verified. The Actions cache persists these derived
trees and runtimes together with the verified objects.

A stale, partially altered, or cache-poisoned derived tree therefore runs under
the name of the verified artifact. Re-extract/rebuild derived state into a new
atomic temporary directory for each job, or store and verify a complete tree
manifest tied to the raw digest before every execution. Include the exact
Python interpreter identity in the runtime key and verify the installed
distribution versions/files before accepting a cached venv. Prefer caching
only immutable raw `objects/sha256` in CI.

### [P3-1] The immutable-URL check accepts arbitrary mutable HTTPS locations

**Where:** `gwz-core/scripts/retained_readers/retained_reader_harness.py:260-268`

Only GitHub and Python-hosted URLs receive provider-specific checks. Any other
HTTPS URL is accepted when its path ends in the artifact name, even if it is a
mutable `current` endpoint. The digest still protects artifact bytes, so this
is not a code-execution finding, but it contradicts the immutable-source
inventory contract. Restrict hosts to reviewed providers and validate their
immutable URL form, or rename the property to checksum-pinned acquisition and
document that URL immutability is not guaranteed.

### [P3-2] Fixture generation inherits Git-control environment variables

**Where:** `gwz-core/scripts/retained_readers/generate_retained_reader_fixtures.py:28-45`

The generator copies the complete host environment and removes only global and
system Git config. Variables such as `GIT_DEFAULT_HASH`, `GIT_DIR`,
`GIT_WORK_TREE`, `GIT_INDEX_FILE`, object-directory variables, and
`GIT_CONFIG_COUNT` can redirect or change generation. An adversarial run with
`GIT_DEFAULT_HASH=sha256` failed during canonicalization with `Not a valid
object name HEAD`; other inherited variables can alter outputs.

Build a minimal allowlisted environment (or explicitly remove all Git-control
variables), force the supported object format at `git init`, and record/test
the supported Git version/object format. The current twice-in-one-environment
test does not cover this.

### [P3-3] Process and Windows extraction isolation need hardening

**Where:** `gwz-core/scripts/retained_readers/retained_reader_harness.py:378-408`;
`gwz-core/scripts/retained_readers/retained_reader_matrix.py:46-112`;
`gwz-core/.github/workflows/retained-readers.yml:23-37`

The timeout terminates the direct process but does not establish and terminate
a POSIX process group or Windows Job Object, so a descendant can survive the
timeout and continue mutating state. Archive duplicate detection is lexical
and does not reject Windows case-folding, trailing-dot/space, reserved-name,
or alternate-data-stream collisions. The harness unit suite currently runs
only on Ubuntu, so its Windows-conditional extraction and venv behavior are
not exercised as unit tests.

Terminate the complete process tree on timeout, reject platform-normalized
archive collisions before extraction, and run the harness unit suite on both
Ubuntu and Windows. These are hardening items for trusted, checksum-pinned
release artifacts, but they should be closed before treating the harness as a
general retained-reader framework.

## Test and adversarial evidence

Run from `gwz-core` on macOS arm64:

- `python3 -m unittest discover -s scripts/retained_readers -p 'test_*.py' -v`:
  **37 passed**;
- manifest `validate`: **passed**, 36 declared tuples;
- `gate-ready`: **passed**, 36 declared tuples;
- the checked manifest and cases also pass their JSON Schemas when explicitly
  evaluated with Draft 2020-12 tooling;
- deleting one `windows-aarch64` tuple still passed both the handwritten
  validator and `gate-ready`;
- a misspelled `postcondition` field passed `validate_cases` and would be
  ignored;
- a non-JSON stdout string containing the three expected tokens passed a
  `--json` case and normalized to a null outcome; and
- `GIT_DEFAULT_HASH=sha256` changed generation behavior and caused a
  canonicalization failure.

I inspected the checked macOS arm64 evidence but did not reacquire or execute
the network release matrix during this review.

## Required CI gate

`gwz-core/.github/workflows/retained-readers.yml` should become a required
check, not merely a workflow file, after the P2 corrections. The minimum gate
is:

1. schema-complete validation, negative validator tests, deterministic/golden
   fixture verification, and harness unit tests on both Ubuntu x86_64 and
   Windows x86_64;
2. actual checksum-pinned Rust and distributed Python reader behavioral jobs
   on Linux x86_64 and Windows x86_64, with v0.10.0 Python explicitly
   unsupported and v0.9.2 confined to its pre-record lane;
3. full supported-artifact/package execution or smoke evidence on Linux
   arm64, macOS x86_64, and macOS arm64, plus an explicit Windows arm64
   unsupported tuple for every reader;
4. cache only verified raw objects, or reverify all derived trees/venvs before
   execution;
5. exact completeness checks over the expected `(reader, platform, case)` set,
   parsed semantic outcomes, fixture/snapshot identities, and normalized
   evidence; and
6. triggers (or a reusable required invocation from the coordinated release
   gate) covering retained-reader inputs, fixture/evidence code, merge record
   writers/readers, the plan/design/capability documents that define the gate,
   and the coordinated Rust/Python driver release—not just the current core
   path filter.

R0 acceptance additionally requires a successful recorded run of the Linux
and Windows behavioral jobs. A local green unit suite or the existing checked
macOS evidence is not a substitute for those two required lanes.
