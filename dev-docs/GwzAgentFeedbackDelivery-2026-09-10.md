# Agent feedback fixes: delivery checkpoint

Implementation and scoped regression checks are complete; release is starting.
This is not a release completion record.
Scope: [the fix plan](GwzAgentFeedbackFixPlan-2026-09-10.md).
The operator authorized integration and the next patch release after validation.

## Lanes

| Family name | Owner/model | Scope |
| --- | --- | --- |
| `fixpaths` | paths / GPT-5.6 Terra | Serialized invocation context, shared path resolution, absolute listings, routing |
| `fixpython` | Python / GPT-5.6 Terra | Native/client context propagation, submission capture and driver parity |
| `fixoutcomes` | outcomes / GPT-5.6 Terra | Shared lock comparisons/reasons, sync and no-change results, both renderers |
| `fixhelp` | help / GPT-5.6 Luna | Compact and structured CLI help, focused tests and generated reference |

All are siblings of `/Users/owebeeone/limbo/gwz-dev`, named `gwz-dev-NAME`.
The unrelated `fsbench` lane belongs to another agent. Earlier `svc*` lanes
were detached, not deleted; see the lane disposal audit.

Source checkpoint: root `b3aecba`, core `b4445e0`, CLI `43b42ab`, Python
`b928e0e`. Before cloning, source CLI binary/library tests and core library
tests were compiled, and the Python extension was built and installed using
Maturin. All builds used `CARGO_PROFILE_DEV_DEBUG=0`,
`CARGO_PROFILE_TEST_DEBUG=0`, `CARGO_INCREMENTAL=0`; lane builds retain these
settings and at most two Cargo jobs.

Each clone reported 183,645 files copied natively, zero ordinary copies,
49,715,992,389 logical bytes and nine verified repositories. This reports the
copy primitive, not an independent extent-allocation measurement. Free space
after the four clones was about 26.8 GiB. Build output is not evidence to commit.
Copied virtual-environment launchers and editable installs may retain source
paths: agents must use lane-local Python imports and rebuild their own binding.

The integrated lanes remain on disk. An unforced disposal of `fixhelp` refused
because copied ignored files, stashes and protected reflog history were not
certified disposable/preserved. Nothing was removed or force-waived. This is
cleanup follow-up, not missing integration; leave the unrelated `fsbench` lane
and earlier detached `svc*` directories alone.

## Integration and release

1. Each agent commits its completed lane through GWZ; no independent pushes.
2. Integrate complete lanes serially into the receiving root with
   `gwz --root ROOT --target @all merge --remote NAME`.
3. Reconcile taut schema generation once both additive protocol changes land;
   validate generated Rust/Python bindings together.
4. Run the bounded regression matrix in the fix plan, including real driver
   construction/decoding, caller/executor cwd separation and Windows paths.
   Reuse the completed NTFS/ReFS capability validation; do not rerun model trials
   or compiler-mutation tests as a release gate.
5. Commit the integrated state and use each repository's release script. Core
   must succeed first; CLI and Python then run in parallel with a matching core
   version. At this checkpoint all three latest published releases are v1.0.8;
   verify the next unused tag before cutting (expected v1.0.9).
6. Verify actual GitHub release/workflow state after the scripts. The inspected
   scripts tag and push; the workflows trigger on a published release or manual
   dispatch, so tag existence alone is not proof that builds started. Avoid
   dispatching a duplicate run.

## Integrated implementation and focused validation

All four primary lanes were integrated through serial whole-family GWZ merges.
Core path checkpoint `309b41a`, outcome checkpoint `b5e79a5`, Python native/client
checkpoint `a910877`, CLI help checkpoint `8f6dd66`, and final help wording
`9ca1063` are included. The combined taut generator's additive compatibility
check passes with RequestMeta tag 9 (invocation) and MemberResponse tag 11
(lock difference reasons); both driver bindings were regenerated together.

Central additions:
- Core `9b1b025`: rejected repository operands and PathEscape report caller,
  resolved candidate and workspace facts. Selector-like directory hints apply
  only after a failed manifest read; valid literal `@root`/`@all` roots remain
  supported. Both new regressions failed on the original diagnostics, then
  passed after the implementation (2/2).
- Python `9281a9a`: actual Rust CLI/Python parity regression covers four root
  forms, member/outside callers, absolute listing paths, add/diff routing,
  human/JSON/JSONL error facts, dirty registration/status/sync and no-commit
  outcomes. Eight cases passed. With native serialized-caller sync/submit and
  client checks: 45 passed in 3.06 seconds.
- Core `e3b9967`: integration exposed an index-order recovery defect. Disk and
  in-memory indexes with identical path/stage/mode/OID entries can enumerate
  differently. Sorting exact comparison rows repairs recovery without folding
  case or suppressing content drift. A regression passed, and the real open
  Python-lane merge recovered to ExpectedConflict and then completed after its
  source-comment conflict was staged. The private evidence archive retains the
  before/after status and read-only diagnostic.

Windows path normalization now treats DOS and verbatim DOS representations
consistently without canonicalizing deleted operands. Native Windows validation
on Dabeest passed the diagnostic case, three path-routing cases and two generated
metadata merge cases against core `90ffa104`, CLI `0555d340`, Python `7b5722c5`.
The Python parity matrix passed 8/8 in 5.95 seconds after rebuilding the native
binding against that same source snapshot. Its later test-only overlay uses
Python `e83a507` plus JSON decoding before comparing Windows diagnostic paths.
Fixtures and reused build caches are on D:, outside the archive.

Core `90ffa104` also repairs generated metadata conflicts during lane integration:
when manifests agree and every conflict is in generated lock/marker files, keep
the receiving metadata while retaining the ordinary merged tree. A real complete
lane merge then succeeded with its root documentation changes preserved. Ordinary
file conflicts and manifest disagreements retain normal conflict handling.

Broader testing exposed stale test constructors/expectations and an HTTP fixture
that could reset a socket after a partial request read. Core `a88ed90` updates
those fixtures and removes a brittle source-text/count guard; behavioral filter
tests remain. Python `e83a507` updates optional protocol constructor fields and
keeps the original wire compatibility baseline by projecting out the two known
additive extensions. Compiler-mutation tests have not been run.

The complete Mac Python suite passed 816 tests in 86.02 seconds. Core `470aab2`
then repairs physical path aliases in diff/log classification and routing:
existing root/caller bases resolve to physical locations, while operands remain
lexical so deleted paths still work. CLI diff integration passed 26/26; Python
native diff/log and driver parity passed another 55 cases in 34.82 seconds.
CLI library tests passed 183/183. Core's first broader pass had four failures;
the corrected fixture/filter subset passed 10/10, the renamed unmaterialized
status test passed separately, and all 58 core integration tests passed.
All 75 CLI integration cases passed across the original run and targeted
reruns. The last local-family fixture now commits its source before cloning;
merge assertions verify actual commits and import refs rather than obsolete
message wording. Unmaterialized locked members report Unknown with their
materialization hint retained.

Final Windows delta rebuilt both drivers against core `470aab2`: Python parity
passed 8/8 in 8.23 seconds and Windows routing passed 3/3. Exported CLI production
source remained `0555d340`; later local CLI commits change only test fixtures.
Python was `278b959`. Raw outputs and failed attempts are retained in the private
`gwz-core-evidence/campaigns/agent-feedback-fixes/runs/2026-09-10-release-validation/`
archive. No full suite was repeated after unrelated test-only corrections;
targeted reruns cover each observed failure and the final diff/log change.

Wire compatibility here means retaining established field numbers and enum
values. Taut optional fields are nullable but still required map keys; an old
binary request omitting new keys is not supported. The original compatibility
baseline is unchanged. Drivers/core ship together; the direct Rust legacy entry
points require an explicit absolute start. See the invocation path contract.

The actual scripts prepare/tag/push but do not create GitHub releases. Python's
script additionally requires the same-version CLI tag to be remotely visible.
Therefore tag preparation is core → CLI → Python; after the core release passes,
CLI and Python hosted builds can overlap once those tags exist. v1.0.9 was
verified unused in all three remotes before launch. Release URLs and final
outcomes remain to be recorded after execution.

## Release execution

Core v1.0.9 is tagged and pushed at `e3c5dd17f131f13366d529b7ffb9a565c6b96815`.
Its release script passed with `--push --no-test`, reusing the recorded test
runs and retaining all mandatory protocol/format/boundary/Clippy gates. The
first attempt stopped before tagging on two lint findings; a braced let-chain
and moving the existing test module to the end resolved them without behavior
changes. The tag was never moved.

[Core GitHub release](https://github.com/owebeeone/gwz-core/releases/tag/v1.0.9)
is published; [release verification](https://github.com/owebeeone/gwz-core/actions/runs/34392227877)
finished with Linux passing and Windows failing six test fixtures: five compare
normalized Windows paths with the old verbatim spelling, and one supplies a
rooted path without a drive as the invocation's absolute caller directory.
CLI and Python release scripts have not started. Correct these fixtures, run
the affected cases on Windows, and cut v1.0.10; leave v1.0.9 untouched.

The separate [candidate validation](https://github.com/owebeeone/gwz-core/actions/runs/34392089509)
also failed: its reviewed workspace baseline was still `089a4cc`, with old
sibling source revisions and a Cargo lock that did not match the new core
version. The repair updates `scripts/workspace-tuple.py` to exclude private
members, retain exact public sibling revisions, and derive an explicitly
recorded candidate Cargo lock. Baseline runs retain their committed Cargo lock.
The workflow installs the pinned Rust toolchain before assembly and checks
locked metadata before expensive tests. Six focused helper tests pass. Publish
a compatible root checkpoint and update each product repository's
`GWZ_WORKSPACE_REVISION` before treating candidate CI as repaired.

The operator identified the additional `fsbench` lane before publication. Its
two evidence commits (`3e1c9ac`, `90c3081`) and root metadata were integrated
through a whole-family merge: seven participants unchanged, two merged, no
conflicts. All product heads stayed unchanged, including the exact core tag.
The archive verifier passed. The benchmark helper is retained evidence, not a
new product dependency or a newly authorized privileged execution.
