# GWZ structural findings — 2026-09-06

Status: **DRAFT 2026-09-06 revision 1.** A lane-owner survey, written after
the target-selection report and at the operator's request to "consider other
structural messes". Every finding below was measured at gwz-dev `5e07e4d` /
gwz-core `bea84c1`, not inferred. Each carries a recommendation; none is
implemented. Target selection has its own document
(`GwzTargetSelectionDesign.md`) and is not repeated here.

The common thread: **conventions that were never encoded survived only while
one person held them.** Six agents working in parallel this week did not
break any rule that a gate enforced. They diverged on every rule that lived
in prose or in a reviewer's head.

## F1 — Gates exist that CI never runs

**Measured.** `run_r4bg_aggregate_gates.py` has four batteries: `privacy`,
`call-graph`, `compatibility`, `fault`. CI invokes `privacy call-graph` only.
The `compatibility` battery is where **both document-consistency gates live**
— the merge one (13 sources, 183 assertions) and the local-clone one added
today (12 sources, 114 assertions). Neither has ever run in continuous
integration. `release.yml` carries a comment explaining the omission: the
battery's commands need sibling repositories a single-repo runner does not
have.

The Bazel pin-drift gate is in the boundary workflow but deliberately not
invoked for the same reason, and is documented as a "landing gate set"
command — meaning a human or agent must remember it.

**Why it matters.** A gate that does not run is prose with extra steps, and
it is worse than prose because it reads as enforcement. The local-clone doc
gate was built today precisely so nobody could later claim `--clean` works;
as wired, nobody would find out.

**Recommendation.** Decide where the multi-repo gate set runs, and make that
a real job rather than a habit. The obvious home is a workspace-level
workflow in gwz-dev, which is the only repository that has the siblings by
construction. Until then, every such gate should print, on success, that it
ran outside CI — an honest gate says what it did not cover.

## F2 — Build identity is not trustworthy

**Measured.** Three `gwz` binaries can exist simultaneously:
`gwz-dev/target/release/gwz` (today), `gwz-cli/target/release/gwz` (31
August, predating the whole local-clone feature), and the Bazel output. The
first two both report `gwz 0.2.0-dev`. The Bazel one reports `0.0.0`, because
hand-written targets do not pass Cargo's package version.

The operator lost time today running the August binary and concluding a
shipped verb was missing.

**Why it matters.** `--version` is the one question a user asks when
behaviour surprises them, and here it cannot distinguish a week-old build
from the current one. The stale tree exists because the Option A workspace
split moved Cargo's output directory and nothing removed the old one.

**Recommendation.** Put the commit into the version string for development
builds, delete the stale tree, and either make the Bazel target carry the
real version or stop producing a binary that claims a false one.

## F3 — Test-suite conventions diverged, and file size followed

**Measured.** Of thirteen new crates, seven keep tests in a sibling
`tests.rs` and six inline them. The largest production file in the program is
`crates/local-disposal/src/lib.rs` at 2277 lines, of which roughly 1490 are
inline tests — in the one crate that deletes data. Six test files exceed 1000
lines.

House rule L1-23 asks for a written cohesion review at roughly a thousand
lines and owners under five hundred. No lane brief mentioned it, and
`rust-split`, which the program's own earlier refactor plan calls mandatory
for mechanical Rust moves, is named in no local-clone document.

**Why it matters.** Nobody disobeyed anything; the rule was not in the
documents the lanes read. The cost is not the line count, it is that the
safety-critical crate cannot be read in one sitting.

**Recommendation.** Put the convention and the tool into the boundaries
document, which is the file lanes actually read, and do the cohesion review
for the handful of files over the threshold. Deferred by operator decision;
recorded here so it is not lost.

## F4 — The test-count pin is a tax on every lane

**Measured.** The fault battery pins the lib remainder as a literal
(`1093 passed` darwin, `1094` linux). Every lane that adds a test must
re-measure the census and re-pin it, in the same commit, with a dated reason,
on two platforms — one measured, one derived and confirmed only when CI first
runs on Linux. It moved five times today. One lane recorded a process
deviation for re-pinning a commit late.

**Why it matters.** It does catch a silently deleted test, which is real. But
it costs a measurement and an edit on every single lane, it is the most
frequent instruction in every brief, and its Linux half is a prediction
carried for weeks before anything checks it.

**Recommendation.** Ask what the pin is defending against and whether a
cheaper instrument holds it — a floor rather than an equality, or a
per-partition count derived at run time rather than pinned. If the equality
is genuinely load-bearing, generate the pin rather than hand-maintaining it.

## F5 — Error codes accreted rather than being designed

**Measured.** The catalog holds 65 codes. Ten belong to local clone, and
nine of those were allocated this week, one lane at a time, each justified
individually against "what wrong recovery would the existing code suggest".
That test is a good one and each decision defensible in isolation. Nobody has
looked at the resulting set as a set.

**Why it matters.** Codes are a compatibility surface the moment a release
tags them. A taxonomy chosen incrementally is the one thing that cannot be
fixed incrementally afterwards.

**Recommendation.** Before the release tag, read the ten as a group against
the rest of the catalog and ask whether a driver could render them by class
rather than one at a time. Cheap now; expensive after a tag.

## F6 — Dead build systems leave live traps

**Measured.** `bazel query //...` failed for weeks on a symlink loop through
`.razel-exec`, an artifact of razel, which nothing in this program now uses.
The fix landed today as a `.bazelignore`. Separately, gwz-core's Bazel
`compile_data` glob was `**/*`, sweeping a 28 GB, 128,000-file `target/`
directory into every compile action — the most plausible cause of two disk
exhaustions this session.

**Why it matters.** Both were invisible until someone ran the build system
they belonged to. The second consumed real hours.

**Recommendation.** Decide whether razel is alive. If it is not, remove its
artifacts rather than ignoring them, because an ignored trap is still a trap
for the next tool that walks the tree.

## F7 — Option A left a hand-maintained pin

**Measured.** Because gwz-core became its own Cargo workspace, crate_universe
can no longer splice the outer workspace, so gwz-cli's three direct
dependencies are hand-pinned in `MODULE.bazel`. Cargo and Bazel agree only
while someone keeps them in step.

This one is already handled — `check_bazel_pin_drift.py` fails on version,
feature or dependency-set drift, with eleven negative fixtures. It is
recorded here because it is the clearest example of the pattern this document
is about: a comment became a gate the same day it was noticed, and that is
the behaviour worth generalising. See also F1: this gate does not run in CI
either.

## Not findings

Two things look like messes and are not. The **local-clone family namespace**
is deliberately separate from target selectors and resolved at operation
time; keeping them apart is correct. And the **three on-disk metadata
formats** each carry a schema version from birth, which is what makes the
best-effort position defensible.

## The one recommendation that generalises

Every finding above is an invariant that existed in someone's head. The
program already knows the cure and applied it repeatedly this week: the
boundary gate, the pin-drift gate, the document-consistency gates. The gap is
that **the cure is applied per incident rather than as policy**. A rule worth
adding to the process document: an invariant that a reviewer would check by
reading is a candidate for a gate, and a convention that a lane brief must
repeat is a convention that is not yet encoded.
