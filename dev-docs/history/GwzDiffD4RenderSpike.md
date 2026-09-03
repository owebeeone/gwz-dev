# GWZ diff D4 render spike — findings

Status: spike complete, 2026-07-04.

This document records the outcome of the **GwzDiffPlan.md phase D4** decision
point:

> "libgit2 prefix first; spike extended-header rewriting; hand renderer if
> needed."

It is a **tests-only** spike. All evidence lives in
`gwz-core/tests/diff_render_spike.rs`, which drives the `git2` crate (the same
libgit2 `1.9.4` the crate already links, via `git2 0.21`) against constructed
temporary Git repositories and, where a golden reference clarifies the verdict,
compares against the real `git` binary (`git 2.52.0`).

No `gwz-core/src/` code was touched. The spike does not add any dependency
(no `tempfile`); it mints and cleans its own scratch repositories.

## Verdict table

| # | Question | Verdict | Evidence test(s) |
| - | -------- | ------- | ---------------- |
| Q1 | Workspace-relative path rewriting in **all** header positions | **hybrid** — libgit2-native for `diff --git`, `---`, `+++`; **needs-hand-renderer** for `rename from`/`rename to` (and `copy from`/`copy to`) | `q1a_*`, `q1b_*`, `q1c_*` |
| Q2 | Extended headers survive rewriting (similarity index, mode changes, new/deleted modes) | **libgit2-native** for emission; the path-free headers need no rewriting. Caveat: the `similarity index NN%` **value** may differ from git | `q2a_*`, `q2b_*`, `q2c_*` |
| Q3 | Binary patches: byte-correct emission; `--binary` vs placeholder | **libgit2-native** (byte-correct, round-trips), but **not git-byte-identical** — the compressed literal stream differs | `q3a_*`, `q3b_*` |
| Q4 | `line-prefix` and `-z`/NUL name records | `line-prefix`: **needs-hand-renderer**. `-z` name records: **hybrid** (libgit2 gives the path set/status; renderer builds NUL framing and rename pairing) | `q4a_*`, `q4b_*` |

## Cross-cutting structural finding (drives the whole D4 renderer shape)

libgit2's `Diff::print(DiffFormat::Patch, cb)` delivers the **entire
extended-header block as a single `origin == 'F'` content chunk** with embedded
newlines:

```
origin='F' content="diff --git a/gwz-core/old.txt b/gwz-core/new.txt\n\
                     similarity index 90%\n\
                     rename from old.txt\n\
                     rename to new.txt\n\
                     index 600d48a..a3fb829 100644\n\
                     --- a/gwz-core/old.txt\n\
                     +++ b/gwz-core/new.txt\n"
```

Consequences for any renderer:

- To rewrite the rename/copy header lines (Q1) or to apply a per-line prefix
  (Q4a), the renderer **must split the `'F'` chunk on `\n`** and act on physical
  lines. Prepending once per callback invocation is wrong.
- The renderer does **not** need to reconstruct the header block from scratch: it
  can forward libgit2's `'F'` block and edit only the two path-bearing lines,
  using the structured `DiffDelta` old/new paths it already has in hand.

This is why the recommendation below is a **bounded post-pass over
`Diff::print` output**, not a full from-scratch patch renderer.

## Q1 — path rewriting across all header positions

### Q1a — simple positions are libgit2-native

Setting `DiffOptions::old_prefix("a/<member>")` / `new_prefix("b/<member>")`
correctly member-prefixes the `diff --git`, `---`, and `+++` lines for
add/modify/delete deltas, matching real `git diff --src-prefix/--dst-prefix`.

### Q1b — the refutation (central finding)

For a **rename** delta, the prefix options rewrite `diff --git`, `---`, `+++`,
but leave the extended headers as **bare repo-relative paths**:

```
diff --git a/gwz-core/old.txt b/gwz-core/new.txt
similarity index 90%
rename from old.txt          <-- NOT prefixed
rename to new.txt            <-- NOT prefixed
index 600d48a..a3fb829 100644
--- a/gwz-core/old.txt
+++ b/gwz-core/new.txt
```

This is **not** a libgit2 limitation that a flag can fix: the golden cross-check
proves **real `git diff --src-prefix=a/<m>/ --dst-prefix=b/<m>/` behaves
identically** — the `a/`,`b/` prefix is by design applied only to the
`diff --git`/`---`/`+++` positions, never to `rename from`/`rename to`. The
prefix option is a virtual `a`/`b` directory, not a general path-rewrite hook.

Therefore, to make `rename from`/`rename to` (and by symmetry `copy from`/`copy
to`) workspace-relative, GWZ must rewrite those lines itself. Prefix options
alone are insufficient. **needs-hand-renderer** for these lines.

### Q1c — the remedy is bounded

A targeted rewrite of only the two path-bearing header lines — splitting the
`'F'` block and substituting `rename from <member>/<old>` / `rename to
<member>/<new>` using the structured delta paths — produces fully
workspace-relative output while leaving every already-correct line untouched.
This confirms the fix is small and well-defined, not a rewrite of the whole
patch formatter.

## Q2 — extended headers survive rewriting

- **q2a (similarity index):** `similarity index NN%` is path-free, so prefixing
  never touches it. **Finding/caveat:** libgit2's computed similarity value is
  not guaranteed to equal git's (observed libgit2 90–94% where git reported
  88% on the same fixture — different similarity heuristics). A renderer must
  **forward libgit2's value verbatim** and must not try to reproduce git's
  percentage. If exact git parity for `similarity index`/`--stat`-adjacent
  numbers is ever required, that is a separate, larger effort.
- **q2b (mode change):** `old mode`/`new mode` are path-free; emitted correctly;
  `diff --git` is prefixed. Nothing to rewrite. (unix-only test.)
- **q2c (new/deleted):** `new file mode`/`deleted file mode` are path-free;
  `/dev/null` is used for the absent side; the real path side is member-prefixed
  by the prefix options. The whole new/deleted header block is workspace-correct
  with prefixes alone — no hand rewriting needed.

## Q3 — binary patches

- **q3a (placeholder):** Without `show_binary`, libgit2 emits
  `Binary files a/<member>/blob.bin and b/<member>/blob.bin differ`, and the
  placeholder paths **are** member-prefixed by the prefix options. Correct as-is.
- **q3b (literal `--binary`):** With `show_binary(true)`, libgit2 emits a
  well-formed `GIT binary patch` / `literal N` block. **Finding:** the encoded
  stream is **not byte-identical** to `git diff --binary` — both use zlib+base85
  over the same content, but the compressed representation differs (observed
  libgit2 `zc$@)H...` vs git `zcmV-W...` for the same `literal 80`). Both are
  format-valid. The spike proves libgit2's emission is **byte-correct** by
  feeding libgit2's own patch to `git apply --binary` and confirming the file is
  reconstructed to the exact expected bytes. A renderer must forward libgit2's
  literal verbatim and must **not** assert byte-equality with the `git` binary.

## Q4 — line-prefix and `-z`/NUL name records

- **q4a (`line-prefix`):** `git2 0.21` `DiffOptions` has **no** line-prefix
  setter. The renderer must prepend the prefix to every physical output line
  itself — and, per the cross-cutting finding, must split the `'F'` header block
  on `\n` to prefix each header line. **needs-hand-renderer.**
- **q4b (`-z` name records):** `DiffFormat::NameOnly` is **newline-separated,
  not NUL-separated**, and emits one record per path (a rename surfaces only the
  new name). The `-z` framing and the two-field `<status>\0<old>\0<new>\0`
  rename record of `git diff -z --name-status` must be **assembled from the
  structured deltas** (status + old/new paths), with the member prefix applied.
  libgit2 supplies the raw material; the renderer builds the NUL framing and
  rename pairing. **hybrid.**

## Recommendation for D4-proper

1. **Do not create a full from-scratch patch renderer.** The libgit2 prefix
   options already produce correct `diff --git`, `---`, `+++`, mode/new/deleted
   headers, `/dev/null`, binary placeholders, and byte-correct binary literals.
   A ground-up renderer would be high-risk for little gain.

2. **Add a bounded post-pass over `Diff::print` output.** Drive
   `Diff::print(DiffFormat::Patch, cb)`; in the callback:
   - split the `'F'` header chunk on `\n`;
   - for rename/copy deltas, substitute the `rename from`/`rename to` (and
     `copy from`/`copy to`) lines with `<member>/<path>` using the structured
     `DiffDelta` old/new paths;
   - forward every other physical line unchanged (already member-prefixed);
   - apply `--line-prefix`, if requested, per physical line at this same point.
   This satisfies the Q1 acceptance criterion (workspace-relative paths in
   `diff --git`, `---`, `+++`, rename/copy headers) without a hand renderer for
   the body or the byte literals.

3. **Build `--stat`/`--numstat`/`--shortstat`/`--summary`/`--name-only`/
   `--name-status` from manifest entries** (as the plan already directs), not by
   forwarding repo-local formatted output — this is also what Q4b requires for
   `-z` framing and rename pairing, and it side-steps the Q2a similarity-value
   divergence for stat-style output.

4. **Forward libgit2 binary literals verbatim; never diff them against git.**
   Byte-correctness is guaranteed by round-trip (`git apply`), not by matching
   git's compressed stream. `--binary` vs the `Binary files ... differ`
   placeholder is selected via `DiffOptions::show_binary`.

5. **Treat the `similarity index` value as libgit2's, not git's.** Do not
   promise git-identical similarity percentages. If exact parity is ever a
   requirement, scope it as a separate item.

6. **A `gwz-diff-render` workspace crate is NOT justified by this spike.** The
   plan says to split one out only "if the spike proves libgit2 cannot rewrite
   every required header safely." It can — via the bounded post-pass above. The
   two path-bearing header lines and the line-prefix are the only hand-rendered
   parts, and they are small, deterministic, and covered by the spike tests.
   Keep the renderer in `gwz-core/src/diff/render.rs` (or equivalent) as the
   plan's touchpoint table already anticipates.

## Reproducing the evidence

```
cargo test -p gwz-core --test diff_render_spike
```

10 tests, all passing. Each `q*` test carries the verdict in its name and
doc-comment and asserts against expected git-format output (with real-`git`
golden cross-checks in q1a, q1b, q2a, q2b, q2c, q3a, q3b, q4b).
