# Core session plan CS1.7 (conditional-compilation boundary check) — SAFETY-AXIS REVIEW

**Review object:** the ten uncommitted files of step CS1.7 at the SHA-256 values in `scratchpad/cs17-object.sha256` (new: `scripts/checks/check_cfg_boundaries.py`, `scripts/checks/test_check_cfg_boundaries.py`, `scripts/checks/cfg_boundaries_allowlist.json`; modified: `scripts/run_tests.py`, `scripts/release.py`, `scripts/checks/test_release_boundary.py`, `.github/workflows/release.yml`, `platform-matrix.yml`, `windows-matrix.yml`, `checked-artifact-boundary.yml`); gwz-core HEAD `bd53865690ab4c179babfac15e051b69423a5cff`; status: uncommitted step implementation; date 2026-09-28.

**Baseline:** gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5`, all verified by `git rev-parse HEAD` at the start and the end of the review; `shasum -a 256 -c` reported all ten files OK at both points; the diff `scratchpad/cs17.diff` matched its recorded SHA-256 (`71f65bc0…`, 1173 lines). The object files were read from gwz-core's working tree; the CS1.7 bullet and standing rules from `git -C /Users/owebeeone/limbo/gwz-dev show HEAD:dev-docs/GwzCoreSessionPlan.md` (the plan lives in the workspace-root repo, not gwz-core); the rule from the root `AGENTS.md`; the precedent from `scripts/checks/check_process_globals.py`. Nothing under `scratchpad/held/` or `tasks/` was opened. No file was written, no git mutation or build was run.

**Axis:** SAFETY — what the check permits to go wrong: where a forbidden placement lands unseen, where the gate fails open, what defeats it silently. Independent, adversarial, read-only, and peer-blind to the Consistency report. Filed verbatim by the lane owner.

**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2, 6 P3. I pre-commit to GO on a revision that resolves P2-1 as specified.

## 0. Evidence base (what you read and ran, with outputs)

Read in full: the three new files; `scripts/run_tests.py` (working tree, 172 lines); `release.py` lines 82–100, 119–135, 586–612, 717–748, 836–850; the four workflows (triggers, checkouts, `run:` lines, summarize steps); the CS1.7 bullet, §1.3 and §3.0 of the committed plan; `check_process_globals.py`; `test_check_process_globals.py` (grep for decode/unreadable coverage: none); `check_lane_commits.sh` (grep for allowlist handling: none).

Ran (all read-only):

- `python3.13 -m unittest scripts/checks/test_check_cfg_boundaries.py scripts/checks/test_release_boundary.py` → `Ran 32 tests in 2.122s OK`.
- `python3.13 scripts/checks/check_cfg_boundaries.py` → `conditional-compilation boundary guard: 1197 files, 264 listed occurrences (gwz-core 262, gwz-cli 2, gwz-py 0); nothing new`, exit 0, wall 2.06 s. Two `--list` runs hashed identically (`3b96f0c0…`).
- Exit-code probes: `--root /nonexistent` → 3× MISSING, exit 1; `--root <dir with no Rust>` → 249× STALE + 2× MISSING, exit 1; `--allowlist process_globals_allowlist.json` → "allowlist must name the repositories to scan", exit 1; `--allowlist /dev/null` → `JSONDecodeError`, exit 1; `--allowlist /nonexistent.json` → exit 1; `--skip gwz-cli` → "unrecognized arguments", exit 2; `--list` → exit 0; `--skip-repo gwz-core` → exit 0; `--skip-repo gwz-core --skip-repo gwz-cli --skip-repo gwz-py` → `0 files, 0 listed occurrences (); nothing new`, exit 0.
- `run_tests.py` with patched `subprocess.run`: `['--', '--skip-cfg-siblings']`, `['--skip-cfg-sibling']`, `['--no-fail-fast']` all run the check with no skip args; `['--skip-cfg-siblings=1']` exits 2 before anything runs.
- In-memory `analyze()` on ~60 constructed snippets (cfa14b8 replays at every outer-attribute position; BOM, CRLF, shebang, quotes, raw/byte/C strings, raw identifiers, nested and doc comments, `macro_rules!` bodies, non-ASCII identifiers, generic headers; ratchet key collisions). Outputs are quoted in §1 and §2.
- Exposure scan of all three trees with the check's own lexer plus the precedent's mod-graph for test status: conditional outer attributes the check does not flag, by position (gwz-core): braced statements 42 production + 103 test-only; unbraced `let` 10 + 14; unbraced expression statements 3 + 157; fields/variants 17 + 14; match arms 0 + 8; parameters 4 (test file). gwz-cli: 4 braced statements; gwz-py: 9 braced statements.
- Tree survey: no CRLF or BOM in any `.rs`; all `.rs` decode as UTF-8 (macOS `iconv` misreported `src/filesystem/native/facts/linux.rs`, which holds `§ × — →`); symlinks only under `protocol/.regen-venv/bin`; `.rs` under hidden directories only in `.github/bootstrap-crate/src/lib.rs` (both gwz-core and gwz-cli; a one-line doc-only stub) and the venv; all five `target` directories sit beside a `Cargo.toml`; no `.rs` in gwz-py outside `native/`; no `non_ascii_idents`/`uncommon_codepoints` lint configured; no `check_cfg_boundaries` reference in gwz-cli's or gwz-py's `.github`; the only `pull_request` workflows are `checked-artifact-boundary.yml` and `linux-identity-probe.yml`, both ubuntu — nothing compiles gwz-core for Windows before merge.

## 1. Findings (severity-ordered)

### P2-1 — Unbraced `let` and expression statements are outside the scan; the cfa14b8 hazard passes with no signal at 184 sites in gwz-core

- **Root cause:** `Analysis.unbraced_item` (`scripts/checks/check_cfg_boundaries.py:233-256`) recognises only item forms; for a `let` statement or an unbraced expression statement in a function body it returns `None`, by the scope decision recorded in the docstring at lines 20–23 ("Statements (`let`, expression statements and statement macros) … are out of scope").
- **Violated invariant:** root `AGENTS.md`: "Do not place `#[cfg(...)]` … directly on individual imports or other unbraced declarations … A condition must never silently transfer to the next declaration." In Rust's own grammar a `let` statement *is* a declaration (Reference, Statements: "The two kinds of declaration statements are item declarations and let statements"), and it is unbraced. For an unbraced expression statement the rule's second bullet applies: "Conditional compilation must have an explicit enclosing boundary" — `#[cfg(unix)] apply(mode);` has none. The implementer's argument ("a `let` braced into a block would end its binding there") is about a remedy, not the hazard; `let x = if cfg!(unix) {…} else {…};` or a `cfg_if!`-guarded helper is the braced remedy.
- **Reproduction** (check's own `analyze`, before → after deleting the conditional line but not its attribute):
  ```
  fn f() {                          fn f() {
      #[cfg(unix)]                      #[cfg(unix)]
      let mode = 0o755;                 let path = build(0);
      let path = build(mode);           use_it(path);
      use_it(path);                 }
  }
  before: []                        after: []
  ```
  and `fn f(path: &Path, mode: u32) { #[cfg(unix)] let mode = mode | 0o111; apply(path, mode); }` → `[]` before, `[]` after the `let` is deleted (`apply` is then compiled only on unix). Real production sites with this exact pair shape: `src/filesystem/native/handles.rs:57/59` (`#[cfg(unix)] let executable = metadata…; #[cfg(not(unix))] let executable = false;`), `src/filesystem/native/filesystem_impl.rs:150/153`, `src/git/gitbackend/preservation_image.rs:551/553`. Deleting the `not(unix)` line there and leaving its attribute compiles on macOS and fails only on Windows — the v1.0.5 failure mode.
- **Impact:** a false negative for the hazard class the step exists to prevent, at 24 `let` sites (10 production) and 160 unbraced expression statements (3 production; the rest test-only, where the consequence is an assertion silently compiled out on one platform). None are inventoried, so the ratchet gives no STALE or NEW signal either. On the PR path nothing compiles for Windows, so this lexical check is the only pre-merge guard, and it does not look here.
- **Required correction:** extend `unbraced_item` with a statement class: after the attributes, a `let`, or an expression statement in a block that is not an item list, that ends at `;` with no `{` at depth 0, keyed `let NAME` or the rendered statement (bracket groups stepped over, as the `use` key does). Braced statements (`{ … }`, `if`, `match`, `unsafe`, `loop`, `while`, `for` bodies) keep passing as explicit boundaries. Inventory the 184 existing occurrences as §1.3 migration debt. (If instead the operator amends the root rule and the plan to exclude statements, the 184-site residual must be recorded there; that ruling is outside the step.)
- **Regression test:** `found('fn f() {\n #[cfg(unix)]\n let mode = 0o755;\n let path = build(mode);\n}\n') == [('#[cfg(unix)]', 'let mode')]`; `found('fn f() {\n #[cfg(unix)]\n apply(path, mode);\n}\n')` is one occurrence; `found('fn f() {\n #[cfg(unix)] { let m = 1; }\n #[cfg(unix)] if x { y(); }\n}\n') == []`.

### P3-1 — Fields, variants, match arms and parameters carry the same live hazard, un-inventoried (31 + 8 + 4 sites)

- **Root cause:** the same `unbraced_item` returns `None`; the exclusion is documented at lines 20–23 with the rationale "no `cfg_if!` holds a field or a variant".
- **Violated invariant:** "A condition must never silently transfer to the next declaration." The rule's named remedies do not reach these positions (the remedy is a per-platform type in a platform module), so this is a coverage residual rather than a rule violation — but it is implicit today.
- **Reproduction:** `enum E {\n #[cfg(unix)]\n Unix,\n Other,\n}` → `[]`; after deleting `Unix,` → `[]`. Same for `struct S { #[cfg(unix)] uid: u32, size: u64 }`, `match x { #[cfg(unix)] 0 => a(), 1 => b(), _ => c() }`, `fn f(#[cfg(unix)] mode: u32, path: &Path)`.
- **Impact:** 17 production variant sites (`src/checked_artifact/fault.rs:31-58` under `#[cfg(any(windows, test))]`, `src/git/gitbackend/preservation.rs:616-626`), 14 test-only, 8 match arms, 4 parameters. A deletion that orphans one attribute makes the next variant or arm platform-conditional; the compiler notices only where that variant is referenced on the other platform.
- **Required correction:** either extend the scan to fields, variants and arms (keys `variant NAME`, `field NAME`, `arm <rendered pattern>`) and inventory them, or record the exclusion with these counts in the plan's standing rule (§3.0) so the gap is an explicit, counted residual.
- **Regression test:** if extended, `found('enum E { #[cfg(unix)] Unix, Other }') == [('#[cfg(unix)]', 'variant Unix')]`.

### P3-2 — `--skip-repo` accepts the check's own repository; the gate exits 0 having scanned nothing

- **Root cause:** `check()` line 348 validates only that the name exists in the allowlist; `scan()` lines 305–306 then skip any named repository, including the one at `path: "."`; `main()` lines 393–399 print "nothing new" with an empty `scanned` list.
- **Violated invariant:** the skip exists "for a CI job without that checkout only" (docstring lines 40–41); the check's own tree is by definition checked out.
- **Reproduction:** `python3.13 scripts/checks/check_cfg_boundaries.py --skip-repo gwz-core --skip-repo gwz-cli --skip-repo gwz-py` → three `SKIPPED GATE` lines, then `conditional-compilation boundary guard: 0 files, 0 listed occurrences (); nothing new`, exit 0. `--skip-repo gwz-core` alone → exit 0.
- **Impact:** a workflow or script edit can silence the gate while it still exits 0; the only trace is a stdout line. No current wiring passes gwz-core (`run_tests.py:119` names only the siblings), so this is latent.
- **Required correction:** refuse `--skip-repo` for a repository whose resolved base equals `--root`, and fail when `result.scanned` is empty.
- **Regression test:** `check(root, allowlist, ['core'])[0][0]` starts with `--skip-repo core:`; `main([..., '--skip-repo', 'core'])` returns 1.

### P3-3 — Nothing mechanical enforces "the list only shrinks"

- **Root cause:** the check compares tree to list (NEW/COUNT/STALE); nothing compares the list to its previous version. Docstring line 39 and the allowlist's `rule` text claim "so the list only shrinks". `check_lane_commits.sh` and all workflows contain no allowlist diff.
- **Violated invariant:** plan CS1.7 term "existing occurrences are listed, and the list only shrinks".
- **Reproduction:** by construction of the key match (the step's own `test_listed_occurrence_passes_and_a_new_one_fails`): a change that adds `#[cfg(windows)] use x::Y;` and its entry in the same commit produces `check() == []`.
- **Impact:** the guarantee rests on a reviewer noticing one added line in a 251-entry JSON list; a new forbidden placement plus its entry passes every gate.
- **Required correction:** a `--only-shrinks <base-allowlist>` mode in the check (fail on any key absent from the base or any count larger), run in `checked-artifact-boundary` on `pull_request` against `git show <merge-base>:scripts/checks/cfg_boundaries_allowlist.json` (fetch-depth 0 is already set) and from `release.py`.
- **Regression test:** added key → error; count 14→15 → error; count 14→13 or removed key → `[]`.

### P3-4 — Macro-invocation items are keyed by macro path only, so one placement can be swapped for another unseen

- **Root cause:** lines 250–255 build the key as the rendered macro path plus `!` (plus the name for `macro_rules!`), dropping the argument group.
- **Violated invariant:** docstring line 38 "A new or modified occurrence fails"; plan term "the list only shrinks" at occurrence granularity.
- **Reproduction:** `impl X { #[cfg(test)] delegate!(a() -> u8 => f::a); #[cfg(test)] delegate!(b() -> u8 => f::b); }` versus the same with the second body replaced by `delegate!(zzz_new(repo: &Path) -> Vec<u8> => other::zzz)` → both `[('#[cfg(test)]','delegate!'), ('#[cfg(test)]','delegate!')]`, `equal: True`. The allowlist holds `delegate!` with `count: 14` in `src/git/gitbackend.rs`.
- **Impact:** bounded to the 15 listed macro items; the count cannot grow, but any of the 14 delegates can be replaced by a new one with no NEW/STALE signal. Related, not a defect: `type/const/static/fn/struct/mod` keys are `word name`, so `type FaultHook = Box<…>` → `type FaultHook = Arc<…>` keeps its key; the docstring's "modified occurrence fails" holds only for `use` items and attribute changes and should say so.
- **Required correction:** key macro items by path plus the rendered leading tokens of the first argument group (e.g. `delegate!(probe)`), splitting the count-14 entry.
- **Regression test:** the two texts above yield different keys; a fixture with one swapped body reports NEW and STALE.

### P3-5 — Non-ASCII-initial item names escape the scan

- **Root cause:** `_LEX` identifier class `(?P<id>(?:r\#)?[A-Za-z_]\w*)` (line 63) requires an ASCII first character; a name such as `Ärger` lexes as `punct`, so `self.kind(name) == 'id'` (lines 242–243) fails and `unbraced_item` returns `None`.
- **Violated invariant:** the scanner's token view must not hide a placement rustc accepts; Rust accepts XID_Start identifiers, and no `non_ascii_idents` lint is configured in any of the three trees.
- **Reproduction:** `trait T { #[cfg(unix)] fn ärger(&self) -> u8; }`, `#[cfg(unix)] type Ärger = u8;`, `#[cfg(unix)] mod ärger;`, `#[cfg(unix)] const ÄRGER: u8 = 1;`, `#[cfg(unix)] struct Ärger;` → all `[]`; controls `#[cfg(unix)] type Aärger = u8;` → `[('#[cfg(unix)]', 'type Aärger')]` and `use ärger::X` → flagged.
- **Impact:** bounded; no such identifier exists in the trees today; the false negative is silent.
- **Required correction:** lex identifiers with `[^\W\d]\w*` (one character-class change).
- **Regression test:** `found('#[cfg(unix)]\ntype Ärger = u8;\n') == [('#[cfg(unix)]', 'type Ärger')]`.

### P3-6 — No CI job anywhere checks gwz-cli or gwz-py; the sibling ratchets rest on local runs

- **Root cause:** every CI invocation skips the siblings (`release.yml:92,145`; both matrix workflows; `checked-artifact-boundary.yml:82`), `release.py:133` always passes `--skip-cfg-siblings` (also when `cargo_root == REPO` with the siblings present beside it), and gwz-cli's and gwz-py's `.github` contain no such check. CS1.8's precedent checks gwz-transport out at `reconciled_commit` in the boundary job so its check is skipped nowhere; CS1.7 did not do the analogous thing.
- **Violated invariant:** plan §3.0 "CS1.7's check runs in every step's gate"; the allowlist claims an inventory of gwz-cli at `ebbea902` and gwz-py at `0b535dc5`.
- **Reproduction:** `grep -rln check_cfg_boundaries ../gwz-cli/.github ../gwz-py/.github` → none; all four gwz-core workflows pass the skips.
- **Impact:** a Phase 4/6 gwz-py or gwz-cli change can add a bare cfg import and pass all CI; it is caught only when someone runs `scripts/run_tests.py` in a full workspace (which does fail closed by default). The skip is visible (SKIPPED GATE) but the gap is covered nowhere.
- **Required correction:** follow the transport precedent — check gwz-cli and gwz-py out beside gwz-core in `checked-artifact-boundary` at commits recorded in the allowlist and run the check without skips — or add the check to the siblings' CI with gwz-core checked out beside them.
- **Regression test:** a workflow-text test (as `test_check_process_globals.py` does for `BOUNDARY_WORKFLOW`) asserting the boundary job runs `check_cfg_boundaries.py` without `--skip-repo`.

## 2. Invariant analysis (attacks that held, with evidence)

- **Item-position replays are caught.** Module level: `#[cfg(not(windows))] use crate::filesystem::FileSystem; use std::ffi::OsStr;` → `[('#[cfg(not(windows))]', 'use crate::filesystem::FileSystem', 1)]`; after deleting the import line → `[('#[cfg(not(windows))]', 'use std::ffi::OsStr', 1)]` (NEW plus STALE under the ratchet). Inside a disabled `cfg_if!` else-arm the same replay flags `use std::ffi::OsStr` at line 3. When the successor is braced (`#[cfg(unix)] use a::B; fn helper() {}` → `#[cfg(unix)] fn helper() {}`) or a `let`, the after-text is not flagged, but every existing bare import is listed, so STALE fires and forces a human edit; the resulting text is rule-compliant (a braced item is its own boundary). Nothing lexical can tell a transferred cfg on a braced item from an intended one; this is the rule's boundary, not the check's.
- **Fail-closed paths hold.** Missing root, root without Rust, wrong or malformed or missing allowlist all exit 1 (see §0). Non-UTF-8 or unreadable files raise through `main` (uncaught → exit 1); rustc also requires UTF-8. BOM (`\ufeff#[cfg(unix)]\nuse a::B;`), CRLF and a shebang line are all flagged at the right line. Unterminated block comments or raw strings swallow the rest of a file, but such a file cannot compile in any arm. `--list` exits 0 by design and is wired nowhere.
- **Lexer divergences probed and held:** `'"'`, `b'"'`, `'\\'`, `'\''` before a bare import (all flagged); lifetimes in headers; `c"…"`, `cr#"…"#`, `r##"a"#b…"##` containing attribute text (nothing flagged; the following bare import still is); `r#type` as path, mod name and struct name; nested `/* /* */ */`, `///`, `//!`, `/** */`; `$(#[$m:meta])*` in `macro_rules!`. A `macro_rules!` matcher containing `#[cfg($c:meta)] use $p:path;` yields a false positive (`use$p:path`), never a false negative. Braces inside generic argument lists (`struct S<const N: usize = { 4 }>;`, `impl<const N: usize> Foo<{ N }> { #[cfg(test)] delegate!(…); }`) are false negatives (`end()`/`in_item_list()` treat any `{` as a boundary), exotic, with no exposure; `fn raw(&self) -> [u8; { 3 }];` is flagged because the brace sits inside a matched `[]`.
- **Skipped names:** the hidden `.github/bootstrap-crate/src/lib.rs` (one `#![doc = include_str!(…)]` line, published to crates.io as a stub) and `protocol/.regen-venv`'s generated taut runtime are unscanned; both bounded. All `target` directories sit beside a `Cargo.toml`; `src/target/mod.rs` is scanned (step's own test). `os.walk` does not follow directory symlinks (documented default), so a symlinked source directory pointing outside every root would be unscanned; none exists.
- **Gate wiring holds.** `run_tests.py` runs the check first, unconditionally, with `check=True`; `--`, `--skip-cfg-sibling` and `--no-fail-fast` leave it unskipped, `--skip-cfg-siblings=1` aborts before anything runs. `release.yml` `verify` and `verify-windows` run `run_tests.py` unpiped. `checked-artifact-boundary` runs the check directly in a `run: |` block under GitHub's default `bash -e {0}`. The two matrix workflows pipe through `| tee … || true`, but they are `workflow_dispatch`-only and their summarize step's unguarded `grep -E '^test result'` under `shell: bash` (`-eo pipefail`) fails the job whenever cargo never ran — fail-closed by side effect, with opaque diagnostics.
- **Ratchet mechanics hold where keyed finely:** count increase → COUNT (step's tests); attribute modification → NEW + STALE; render normalisation collides only intentionally (`feature = "a"`/`feature="a"`, trailing commas, spacing) and never across distinct predicates; attribute order is preserved in the key; `cfg(all())`/`cfg(any())` are flagged (conservative); `cfg_attr(not(all()), …)` and `cfg_attr(unix,)` correctly pass.
- **Cost and determinism:** 2.06 s wall for 1197 files; identical `--list` output across runs; all outputs sorted; keys carry no line numbers.
- **Tests:** pinned — missing sibling fails closed, unknown `--skip-repo` fails, entry validation, SKIPPED GATE output, runner flag handling, hidden/`target` skipping, line drift. Not pinned — malformed JSON, missing `repos`, undecodable or unreadable file (all fail closed today only by uncaught exception), own-repo skip (P3-2).

## 3. Risks and next action

- `release.py --no-test` skips `run_test_suite` and therefore this check along with the filesystem-boundary and process-globals checks (`run_checked_boundary_gates` runs only the checked-artifact script and clippy); the tag can be pushed unchecked, and `release.yml`'s `verify` then fails on the tagged tree, blocking crates.io publication but not the tag. Pre-existing pattern; CS1.7 adds one more check to that class.
- Residuals without exposure today: header-brace false negatives inside `<…>`; symlinked source directories; the hidden bootstrap stub.
- The fail-closed exception paths (undecodable, unreadable, malformed JSON, missing `repos`) deserve pinning tests so a later "robustness" edit cannot turn them fail-open unnoticed.
- **Next action:** a remediation round on P2-1 — extend the scan to unbraced statements with inventory, or obtain the operator's explicit ruling amending the rule and plan with the 184-site residual recorded — then P3-1 through P3-6, each with the regression test named above. I pre-commit to GO on a revision that resolves P2-1 as specified.
