# Private evidence repository split

2026-09-10. Private-member flag implemented and validated; campaign evidence migration completed locally.

The private remote is `owebeeone/gwz-core-evidence`. The approved split moves
raw evidence, benchmark results and evidence-specific runners there. Product
tests, CI checks, authoritative design documents and concise result summaries
remain public.

## Campaign archive layout (settled before migration)

Evidence belongs to the project under test, regardless of where it was collected.
GWZ evidence goes to the private `gwz-core-evidence` member of `gwz-dev`.
SDAX evidence goes to the private `sdax-core-evidence` member of `sdax-wz`.
The SDAX attribution in GWZ's agent runner is provenance, not SDAX results.
Neither archive is a product/build workspace member (Cargo, packaging, release CI).

Each archive has a catalog README, `campaigns/<stable-topic>/`, and
`provenance/migrations/<date>.json`. Campaigns contain their own runners/inputs
and named or dated run directories. A campaign can contain multiple experiments
and hosts. Preserve historical run names and internal layouts; do not rewrite
frozen protocols, runner snapshots, result bytes or their recorded absolute paths.
Small shared archive utilities live in `tools/` only when actually shared.
New runs always get a new directory, never overwrite an archived run.

GWZ initial campaigns:

- `filesystem-comparison/runs/<original-run-name>/`: the three filesystem
  comparison runs; helper in `filesystem-comparison/tools/fs-bench-helper/`.
- `filesystem-capability/runs/capability-gate-20260910/`.
- `filesystem-interface/runs/filesystem-interface-20260908/`.
- `test-performance/runs/<original-run-name>/`: both profiling campaigns.
- `release-census/runs/release-census-20260907/`.
- `agent-usability/runner/`, `inputs/`, `runs/imported-summaries/`: opt-in
  evaluation scripts, candidate skills, frozen baseline inputs, JSON results.
- `linux-identity/runs/imported-history/`: two historical raw JSON reports.
- `private-members/runs/20260910-windows/`: focused Windows logs and CLI probe.

SDAX initial campaigns:

- `authoring-evaluation/evaluation-harness/` (retained original layout, including
  its runs and validation), `evaluation-results/`: independent authoring trials.
- `performance/performance-harness/`, `scripts/`, `performance-results/`:
  measurement harness, exclusively measurement-related helpers and all runs.
- `remediation/runs/remediation-20260909/`: integrated checks, freeze, native
  performance validation and structural evaluation review with provenance.

SDAX design iteration `sdax-v1`, plans, normative documents and concise public
reports stay public. GWZ retained-reader fixtures and Linux identity probe
scripts stay public because public CI directly requires them. Generic public
CI/test/release helpers remain public. Build caches, binaries, Python caches and
nested Git stores are not evidence to publish: move existing caches outside every repository without copying or deleting them.
Do not keep build caches in the archive, even ignored.
Local historical recovery copies and other source checkouts are not migration
inputs. Remote execution directories remain untouched; import only bounded
completed logs/runners, never build trees or live campaign state.

Before moving each file, record its source repository/HEAD/path, destination,
SHA-256, size, mode, and tracked/untracked/ignored status. Rename on this disk,
then verify bytes and mode at the destination; fail if any input changed since
inventory. Preserve unrelated staged/unstaged changes. Update public references
to label the private archive. Already-public Git history is unchanged.

Historical runners with source-relative paths are preserved byte-for-byte.
Document a fresh replay checkout with the original harness layout, explicit
source revision and new output location, rather than changing frozen runner
bytes and invalidating their evidence hashes. Windows replay fixtures use
`D:/gwz-tests/<unique-name>/`; existing historical E: paths remain provenance.

## Selected feature: explicit private flag

The operator selected the simple manifest policy on 2026-09-10. A member can
declare `private: true`. Workspace clone and lock materialization quietly skip
remote access refusals for that member while preserving its manifest/lock entry.
Unmarked members and non-access errors keep their existing behavior. The flag
is managed with `gwz repo sync <member> --private` / `--public`.

Private names and URLs remain visible in the pushed manifest. Encryption, group
keys, external services and hidden membership overlays are not part of this
feature. Earlier proposals for those mechanisms are superseded.

Implementation and focused validation are complete in the working tree. The
evidence repositories now contain the campaigns described above. Product source
changes and public migration edits are separate from the private archive commits.

Validation covered:

- Red/green native HTTP access-refusal tests (401, 403, 404), public refusal,
  private server failure, accessible private members, policy set/preserve/clear,
  root failure, and pre-existing destination preservation.
- Windows final clone/materialization suite: 19 passed, including an all-private
  inaccessible workspace; additional private/authentication tests passed.
- Windows CLI: private/public flag parsing and actual human/JSON clone output
  passed. The probe also checked unchanged manifest/lock bytes and ordinary
  failure after clearing the flag. Fixtures, runner and logs are retained under
  `D:/gwz-tests/private-members/` on Dabeest.
- Mac clone/materialization, repo-sync and artifact suites passed (18, 34 and
  496 tests respectively), plus CLI flag and generated-help checks. The final
  Windows run additionally covers its platform-specific HTTP refusal messages.
- Python client/CLI/protocol tests: 60 passed. Rust protocol regeneration and
  Python drift checks passed with the historical wire fingerprint unchanged
  after removing exactly the two new optional boolean fields.
- Rust formatting/parsing, whitespace and the filesystem source-boundary check
  passed. No Linux native compilation was performed.

The destination test exposed and fixed an existing rollback defect: a failed
clone must not remove a pre-existing non-repository directory. Rollback now
tracks only destinations absent at operation entry. Regenerable Mac incremental
build caches were cleared to recover disk space; remaining builds ran on D:.

## Migration validation

Local evidence was renamed and verified byte-for-byte against its pre-move inventory. GWZ remote logs and transcripts were imported as a read-only snapshot with per-file hashes. No model inference or filesystem benchmark was run. SDAX public checks passed after removing the harnesses, including packaging, public tests, consumer checks and CI script tests. Archived authoring runner checks passed (23); GWZ synthetic runner checks passed (6, with 4 native Windows cases skipped on Mac). Replay preparation used external directories and preserved frozen archive bytes. Full cache locations are recorded privately; no target directory is in either evidence repository.

The final Windows replay used `D:/gwz-tests/evidence-replay-20260910`; all 10 native/protocol tests passed. Both archive AGENTS.md files document campaigns and default ignored scratch/temp/tmp directories. Ignore behavior was checked against both disposable paths and durable log/JSON/CSV/source-bundle paths.
