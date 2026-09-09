# Agent feedback fixes: delivery checkpoint

Implementation is in progress; this is not a release completion record.
Scope: [the fix plan](GwzAgentFeedbackFixPlan-2026-09-10.md).
The operator authorized integration and the next patch release after validation.

## Lanes

| Family name | Owner/model | Scope |
| --- | --- | --- |
| `fixpaths` | paths / GPT-5.6 Terra | Serialized invocation context, shared path resolution, absolute listings, routing and diagnostics |
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

Completion, test results, integrated commits and release URLs remain to be
recorded here after execution.
