# GWZ diff plan — re-review (Review48-4)

Reviewed plan:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md` (revised, two-phase)

Prior reviews: `GwzDiffPlan-Review48.md`, `-2.md`, `-3.md`.

Re-review status: independent design re-review, 2026-06-27. New claims
re-validated against `git2 0.21.0` source. Citations are to verified file:line
locations.

## Verdict

The mandated two-phase, file-granular restructure from Review48-3 is fully
adopted and cleanly executed. The whole-diff spool, `diff.open`/`diff.output`/
`diff.cancel`, `DiffStreamKind`, and `DiffOpenResponse` are gone
(`GwzDiffPlan.md:311`); `diff` (manifest) + `diff.file`/`diff.files` (bounded
patch) are ordinary request/response (`GwzDiffPlan.md:291`–`309`); the
explicit-literal-file fast path skips Phase 1 (`GwzDiffPlan.md:177`); the
previously orphaned `DiffParsedTarget` now lives in `DiffManifestResponse.targets`
(`GwzDiffPlan.md:495`); the `--merge-base` comparison rows are filled in
(`GwzDiffPlan.md:589`, `:591`); and the AD11 root-exclusion unknown is honestly
flagged rather than assumed (`GwzDiffPlan.md:276`–`281`).

The restructure removed the plan's single biggest risk. It introduced exactly
one new correctness hazard — rename/copy rendering under stateless Phase 2 — plus
a few smaller refinements. None are blocking; the rename/copy one must be written
into the plan before D0.

## Findings

### [P1] Stateless Phase 2 cannot reproduce the manifest's rename/copy classification without reconstructing the pair

This is the one real hazard the two-phase split created. Phase 1 runs the full
per-repo `Diff` with `find_similar`, so the manifest can legitimately report a
file as `renamed`/`copied` with a `similarity` (`GwzDiffPlan.md:536`, `:468`).
Phase 2 is "stateless recompute": the client sends operands + `file_id` or
`path`, and core recomputes a single-file diff (`GwzDiffPlan.md:541`–`543`).

But rename/copy detection is a **whole-set** operation, not a per-file one:

- A rename is only detected if both the deleted source and added target are in
  the diff's delta set. If Phase 2 filters to just `new_path`, libgit2 sees an
  **added file** and renders it as a new file — not `similarity index` /
  `rename from` / `rename to` + the content delta. The Phase 2 patch then
  **contradicts** the Phase 1 manifest, which said `renamed`.
- Copies are worse: copy detection can pair against an *unmodified* source
  (`copies_from_unmodified`, confirmed at
  `git2-0.21.0/src/diff.rs:1399`). That source is, by definition, not in any
  single-file pathspec set, so a stateless per-file recompute can never
  reproduce a copy the manifest reported.

The plan half-acknowledges this only for the explicit-`path` fast path
(`GwzDiffPlan.md:552`–`554`), but the **manifest-driven** `file_id` path has the
same problem and is supposed to be faithful. The plan must specify that for a
rename/copy entry, Phase 2:

1. derives both `old_path` and `new_path` from the manifest (the recommended
   `file_id` form already encodes scope+status+old/new paths,
   `GwzDiffPlan.md:458`–`459`), and
2. recomputes with a pathspec set containing **both** paths and rename/copy
   detection enabled, then extracts the single matching delta.

Copy-from-unmodified-source still cannot be reconstructed this way; for copies,
either fall back to rendering the target as an add with a note, or carry enough
context for Phase 2 to include the source. Decide this explicitly. As written,
"stateless recompute" + "manifest reports renames/copies" are inconsistent.

### [P2] The root-exclusion unknown is over-stated — a delta post-filter is simpler than libgit2 pathspec exclusion and always correct

AD11/D2 defer proving how root diff excludes member dirs, `.gwz/`, and
`gwz.conf/.tmp/`, and correctly note the git2 pathspec API is additive
(`GwzDiffPlan.md:279`–`281`, `:899`–`902`, `:1181`–`1183`). I confirmed this:
`DiffOptions::pathspec` only *adds* a pattern (`git2-0.21.0/src/diff.rs:938`) and
the only related negative control is `disable_pathspec_match`
(`diff.rs:775`); there is no exclude-pattern setter in the wrapper, and whether
libgit2 honors `:(exclude)` magic inside the pathspec string is unverified —
exactly the spike's open question.

But the spike does not need to answer it. The simplest correct strategy is a
**post-filter on the `Diff` delta list**: after diffing the root, drop any delta
whose workspace path is under an excluded prefix (member paths, `.gwz/`,
`gwz.conf/.tmp/`). That is pure GWZ Rust over data libgit2 already produced,
works regardless of pathspec-magic support, and matches the manifest projection
pass the plan already performs. List it as the preferred strategy.

Two caveats worth a sentence each: (a) post-filtering means libgit2 still
*walks* those paths, so for very large member worktrees a libgit2-level skip is a
perf optimization, not a correctness need; and (b) for the common
`worktree_vs_index` form, member dirs and `.gwz/` are untracked and a default
diff does not emit untracked content anyway, so exclusion is mostly load-bearing
for `--cached` and `<tree>` forms — the plan treats it as always-required, which
is safe but worth scoping.

### [P3] Phase 1 ↔ Phase 2 is a TOCTOU window for worktree diffs

Stateless recompute means Phase 1 (manifest) and Phase 2 (patch) observe the
worktree at two different instants. For `worktree_vs_index`/`worktree_vs_tree`,
the user can edit or revert a file between the manifest call and the patch fetch,
so a manifest entry may have no patch (or a different one) by Phase 2. Git's
single-process diff has a narrower version of this; the two-phase split widens
it. This is acceptable for v0 but should be documented as a snapshot caveat, and
the client should treat "manifest said changed, Phase 2 found nothing" as a
benign race, not an error. (A future stateful mode keyed by operation id —
already an open decision, `GwzDiffPlan.md:1174`–`1176` — would close it.)

### [P3] `--quiet`/`--exit-code` build the whole manifest; Git short-circuits on first difference

The plan satisfies `--quiet`/`--exit-code` from the Phase 1 manifest
(`GwzDiffPlan.md:538`–`540`, AD8). Correct, but `git diff --quiet` stops at the
*first* detected difference and skips rename detection entirely; building the
full manifest (all repos, all files, `find_similar`) just to answer
"any differences?" is wasteful over many/large repos. Add an early-exit manifest
mode for `summary_only`/`--quiet` that returns as soon as one repo reports a
difference and skips similarity detection. Low severity, pure efficiency.

### [P3] `file_id` becomes a parsed, security-relevant token — specify its encoding

Because Phase 2 correctness now depends on decoding `file_id` back into
scope+status+old/new paths (see P1), `file_id` is no longer an opaque handle —
it is a structured token core must parse from client input. Paths can contain
the delimiter, spaces, and non-UTF-8 bytes. Specify a concrete, unambiguous
encoding (and validate on decode), or keep `file_id` opaque and carry
`old_path`/`new_path`/`status`/`scope` as explicit `DiffFileRequest` fields
instead. The latter is cleaner and sidesteps a parsing/escaping surface.

## Smaller notes

- **Batch per-file errors:** `DiffFileResponse` carries its own
  `response=ResponseEnvelope` (`GwzDiffPlan.md:513`), so `diff.files` can report
  per-file failures without failing the batch — good, but the phase contract
  should state this explicitly so a client knows a partial batch is expected.
- **`files_changed` vs `files_manifested`** (`GwzDiffPlan.md:479`): the
  distinction is undocumented. One sentence on what each counts (e.g. changed in
  repo vs. surfaced after pathspec/exclusion filtering) would prevent client
  drift.

## Resolved since Review48-2/-3

- Streaming spool, `diff.open/output/cancel`, `DiffStreamKind`,
  `DiffOpenResponse` removed (`GwzDiffPlan.md:311`–`313`).
- `DiffParsedTarget`/`DiffComparison` now referenced (`:495`, `:401`–`405`).
- D-1 reduced to a request/response + PyO3 BYTES spike, no backpressure work
  (`GwzDiffPlan.md:786`–`819`).
- `gwz-py` uses the existing `call` bridge; no new streaming binding
  (`GwzDiffPlan.md:213`, `:761`).
- `histogram` stays out (confirmed absent in `git2 0.21`); `--merge-base` forms
  tabulated; root exclusion extended to `.gwz/` and `gwz.conf/.tmp/`
  (`GwzDiffPlan.md:629`–`632`).

## Residual risk

Low. The architecture is sound and the transport risk is gone. The one item that
must not slip into implementation unspecified is **rename/copy fidelity between
the manifest and stateless per-file patches** (P1) — get that contract written
before D0 freezes the schema, because it may influence whether `DiffFileRequest`
carries explicit old/new paths instead of an opaque `file_id` (P3). Everything
else is documentation, an efficiency short-circuit, and choosing the
delta-post-filter exclusion strategy the plan already lists as an option.
