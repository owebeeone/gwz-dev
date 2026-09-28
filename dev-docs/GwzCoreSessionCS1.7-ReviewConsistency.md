# Core session plan CS1.7 (conditional-compilation boundary check) — CONSISTENCY-AXIS REVIEW

**Review object:** the ten uncommitted gwz-core files at the SHA-256 values in `scratchpad/cs17-object.sha256` (all ten verified `OK` at start and end of review; `cs17.diff` 1173 lines, SHA-256 `71f65bc0…`): new `scripts/checks/check_cfg_boundaries.py`, `scripts/checks/test_check_cfg_boundaries.py`, `scripts/checks/cfg_boundaries_allowlist.json`; modified `scripts/run_tests.py`, `scripts/release.py`, `scripts/checks/test_release_boundary.py`, `.github/workflows/{release,platform-matrix,windows-matrix,checked-artifact-boundary}.yml`. gwz-core HEAD `bd53865690ab4c179babfac15e051b69423a5cff`. Status: uncommitted step implementation. Date 2026-09-28.

**Baseline:** gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5`, unchanged between the opening and closing verification. gwz-cli and gwz-py working trees clean. Sources read with `cat -n`, `sed -n`, `git show HEAD:…`, `git diff HEAD`; the rule from `/Users/owebeeone/limbo/gwz-dev/AGENTS.md`; CS1.7, CS1.8, §1.3 and the standing rules from `git show HEAD:dev-docs/GwzCoreSessionPlan.md`; the precedent `scripts/checks/check_process_globals.py` and `test_run_tests_transport_globals.py`. The check's functions were exercised on in-memory snippets via `python3.13 -` importing the module. No file was written, no build run, no git mutation.

**Axis:** CONSISTENCY — the implementation against the AGENTS.md rule, the CS1.7 bullet and the CS1.8 precedent. Independent, adversarial, read-only. Filed verbatim by the lane owner.

**Verdict: NO-GO** — 0 P0, 0 P1, **2 P2**, 3 P3. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified below (P2-1 by extending the scan; the alternative, ruling statements out of the rule, is a controlling-document change for the lane owner, not the implementer). The two P2s escalate the step to the Safety axis; I also request that escalation on P2-1's merits (§3).

## 0. Evidence base (what you read and ran, with outputs)

**Tests.** `cd gwz-core && python3.13 -m unittest scripts/checks/test_check_cfg_boundaries.py scripts/checks/test_release_boundary.py scripts/checks/test_run_tests_transport_globals.py scripts/checks/test_run_tests_filesystem_mode.py scripts/checks/test_check_process_globals.py` → `Ran 57 tests in 10.757s  OK`. The plan's three required cases are present (`test_bare_cfg_import_fails_in_a_disabled_arm`, `test_the_same_import_inside_cfg_if_passes`, `test_a_listed_occurrence_that_disappears_fails`), plus NEW/COUNT/STALE, line-drift, modified-occurrence, validation, skip-dir, missing-sibling, `main()` output, runner-flag and repository tests.

**The check.** `python3.13 scripts/checks/check_cfg_boundaries.py` → `conditional-compilation boundary guard: 1196 files, 264 listed occurrences (gwz-core 262, gwz-cli 2, gwz-py 0); nothing new`, exit 0. With `--skip-repo gwz-cli --skip-repo gwz-py` → two `SKIPPED GATE: …` lines, `1034 files, 262 listed occurrences (gwz-core 262); nothing new`, exit 0. `--list`: 251 keys; by item kind: 118 `mod`, 107 `use`, 11 `fn`, 6 `type`, 4 `const`, 2 `struct`, 1 `static`, 2 macro keys (`delegate!` count 14 in `src/git/gitbackend.rs`, `reverse_entry_preflight_fixture!` 1) — 15 macro items, matching the disclosure. The 11 `fn` entries are real required trait methods (`src/git/gitbackend/contract.rs:26-47`, `src/filesystem.rs:342,388`), not `end()` misfires.

**Allowlist versus HEAD blobs (not the working tree).** Scanning `git show HEAD:<path>` for every tracked `.rs` in the three repos with the module's `analyze()`: gwz-core 1029 files / 262 occurrences, gwz-cli 145 / 2, gwz-py 17 / 0; `entries not found at HEAD: {}`, `found at HEAD but not listed: {}`, `count mismatches: {}`; 251 keys, sum 264 both sides. The allowlist's claim "complete inventory … taken at gwz-core bd538656, gwz-cli ebbea902 and gwz-py 0b535dc5" holds exactly. The concurrent CS1.4/CS1.5 files (`src/session_host/*`, `src/lib.rs`) add no occurrence (working-tree run also green).

**Plan's estimate.** CS1.7 says "about 90 … almost all `cfg(test)` imports"; the inventory is 264 because the implementer's "unbraced declaration" also takes `mod name;` (118), required trait `fn`s, `type`/`const`/`static`, unit/tuple structs and macro items. `use` alone is 107. The broader reading follows the rule's "other unbraced declarations"; the discrepancy is the estimate's, not the code's.

**Coverage.** gwz-core roots `.`: `src/`, `crates/*/src`, `tests/*` (including the `tests/transport_*` sub-crates), `build.rs`, `build_support/`, `examples/`, `protocol/*/rust/vectors.rs`. gwz-cli roots `.`: `src/`, `tests/`, `examples/`, `build_support/`, `build.rs`. gwz-py root `native`: `gwz-py/Cargo.toml` has `[lib] path = "native/src/lib.rs"`, no `build.rs`, no `.rs` outside `native/` except `.venv`. Skipped as documented: hidden directories (`.github/bootstrap-crate/src/lib.rs` in gwz-core and gwz-cli — neither contains a `cfg` today; `protocol/.regen-venv`) and the five `target/` directories, each beside a `Cargo.toml`. No symlinks outside `.regen-venv`. No non-ASCII identifiers, no `<{`/`= {` item headers, no leading-`::` macro items in any tree (grep).

**Wiring.** `run_tests.py:122-126,149` runs the check before the transport check with `check=True`; without siblings the checker prints `MISSING gwz-cli: …` and exits 1 (test `test_a_missing_sibling_fails_closed_and_the_skip_flag_says_so`, `test_main_prints_skipped_gate_and_fails_on_new_occurrences`). `--skip-cfg-siblings` maps to `--skip-repo gwz-cli --skip-repo gwz-py` (`Runner.test_skip_flag_skips_only_the_siblings`); `--skip-cfg` is rejected (`allow_abbrev=False`). Every CI invocation of `run_tests.py` carries the flag: `release.yml:92,145`, `platform-matrix.yml:51`, `windows-matrix.yml:45`; no other workflow runs `run_tests.py`. `release.py:131-135` passes it and `test_release_tests_skip_the_cfg_siblings_the_worktree_lacks` pins it. The boundary job runs the check with both skips (`checked-artifact-boundary.yml:82`) and names `test_check_cfg_boundaries.py` in its unittest list (`:83`). Python: `3.x` on the matrix/release runners, `3.11` on the boundary job; the module needs 3.10+ as the precedent does. The two recorded HEAD failures are unchanged: `tests/publish_workflow.rs::local_release_runs_checked_artifact_boundary_before_rust_tests` compares string positions (`run_tests.py` at release.py:133 before `[sys.executable, CHECKED_ARTIFACT_BOUNDARY]` at :589; at HEAD 129 < 582, the same relation); `release_workflow_runs_full_rust_verification`'s `contains("python scripts/run_tests.py")` still holds.

**Snippets.** Thirty-three constructed Rust snippets run through `analyze()` (§2 lists those that held). Four families did not (§1).

**Statement class.** A classification of every conditional attribute whose target `unbraced_item()` returns `None` for, over the three trees: 156 braced blocks/control flow (allowed), **138 expression statements in fn bodies, 7 in control blocks, 2 in plain blocks, 24 `let` statements, 2 macro statements** (173 `;`-terminated statements), 31 fields/variants, 8 match arms, 4 parameters, 11 tail expressions. Fifteen `cfg_if!` invocations sit in statement position inside fn bodies; three of them declare a `let` inside an arm and use the binding after the macro (`gwz-py/native/src/dispatch/mod.rs:422` and `:482`, `dispatch/merge.rs:94`).

**Budget.** `check_cfg_boundaries.py` 404 lines, 361 non-blank, about 319 non-blank outside the 42-line module docstring; `run_tests.py` +23, `release.py` +11 (with docstring). The implementer's 427 counts blank lines. The lexer (`:55-107`) is a copy of the precedent's with one divergence (`r#` identifiers).

## 1. Findings (severity-ordered)

### P2-1 — Statement-level conditional compilation is excluded from the scan on a premise the codebase itself refutes

- **Root cause.** `Analysis.unbraced_item()` (`check_cfg_boundaries.py:233-256`) recognises only items; a conditional attribute on a `let`, an expression statement or a statement macro returns `None` and `occurrences()` drops it. The docstring (`:20-23`) and the test `test_statements_fields_and_variants_are_out_of_scope` (`:110-125`) pin this, justified as: "the rule's remedies hold items … a `let` braced into a block would end its binding there."
- **Location.** `scripts/checks/check_cfg_boundaries.py:20-23, 233-256`; `scripts/checks/test_check_cfg_boundaries.py:110-125`.
- **Violated invariant.** AGENTS.md: "Conditional compilation must have an explicit enclosing boundary. … Do not place `#[cfg(...)]` or a conditional `cfg_attr` directly on individual imports or other unbraced declarations. … A condition must never silently transfer to the next declaration." The Rust Reference classes `let` as a *declaration statement*; and the rule's own remedy, `cfg_if!`, works in statement position and keeps call-site `let` bindings — this tree relies on that: `gwz-py/native/src/dispatch/mod.rs:422-447` declares `let response_meta` in both arms of a `cfg_if!` inside `fn` and uses it after the macro (also `:482` `let session`, `merge.rs:94`). The stated premise is false, so the exclusion is unsupported by the controlling documents.
- **Reproduction.**
  ```
  fn f() {
      #[cfg(not(unix))]
      let mode = 0;
      #[cfg(unix)]
      apply(mode);
      #[cfg(unix)]
      assert_eq!(mode, 0);
  }
  => []
  ```
  In the tree: `src/checked_artifact/capability/pre_catalog/provider/admission_mutation.rs:183-185` has `#[cfg(test)] let faults = install_faults(&bytes);` immediately followed by `let source = ObservedFileV1 { … };`. Delete the `let faults` line and leave its attribute, as cfa14b8 did with an import, and `source` becomes test-only: `cargo test` passes, `cargo build` fails. The check reports nothing before or after. Platform arms are in the class too: `crates/refcopy/src/ordinary.rs:734` `#[cfg(not(unix))] let _ = metadata;`, `src/checked_artifact/platform.rs:60` `#[cfg(windows)] super::fault::fault(…)`, `src/checked_artifact/tests/recovery_protocol.rs:50,254` `#[cfg(unix)] assert_eq!(…)`.
- **Impact.** 173 existing placements of exactly the rule's mechanism (24 `let`, 147 expression statements, 2 statement macros) are neither flagged nor inventoried, and new ones pass every gate. In disabled platform arms this is the blindness the rule exists to remove. The codebase's own idiom for the braced form exists 156 times (`#[cfg(unix)] { … }`), so enforcement is not alien to it.
- **Required correction.** Flag a conditional attribute on any `;`-terminated statement inside a block: `let` (key `let NAME` or `let _`), expression statement (key: first path segment, e.g. `stmt apply` / `stmt crate::checked_artifact::fault_v1::hit`), statement macro (`assert_eq!`). Keep fields, variants, match arms and parameters out: they are not declarations and no remedy holds them. Inventory the 173 existing statements in the allowlist as debt; the list then only shrinks from that baseline. Replace docstring `:20-23` with the true scope statement and drop the false justification. If the lane owner instead rules that "declaration" means "item", the ruling must land in AGENTS.md and the plan's standing rule (line 96) so that code and documents agree; that path is outside the step and outside my pre-commit.
- **Regression test.** The snippet above yields three occurrences (`let mode`, `stmt apply`, `assert_eq!`); the same three lines inside `cfg_if::cfg_if! { if #[cfg(unix)] { … } }` inside a fn body yield none; `struct S { #[cfg(unix)] a: u8 }`, `enum E { #[cfg(unix)] A }`, `match x { #[cfg(unix)] 0 => {} _ => {} }` and `fn f(#[cfg(unix)] p: u8)` still yield none.

### P2-2 — A `{ … }` in an unbraced item's header makes the scanner judge the item braced

- **Root cause.** `end()` (`:204-214`) returns at the first `{` outside `()`/`[]`, and `in_item_list()` (`:216-231`) stops its backward walk at any `}`; both treat every brace as a body or block boundary, but `{ expr }` is legal in generic-argument position (const-generic arguments and defaults).
- **Location.** `scripts/checks/check_cfg_boundaries.py:204-214, 224-226`.
- **Violated invariant.** The docstring's own definition: an unbraced declaration is "an item that ends in `;` instead of a braced body" — these end in `;`.
- **Reproduction** (each `=> []`; the control without the brace flags all three):
  ```
  #[cfg(unix)] struct D<const N: usize = { 2 }>;
  trait T { #[cfg(unix)] fn f() -> Foo<{ N }>; }
  #[cfg(unix)] struct W<T>(T) where T: Trait<{ N }>;
  impl Foo<{ N }> for Bar { #[cfg(test)] delegate!(x => y); }
  ```
- **Impact.** A `#[cfg]` on a unit or tuple struct, required trait method or macro item whose header (or enclosing `impl` header) carries a const-generic block expression passes the gate. None exists in the trees today (grep for `<\s*\{` and `=\s*\{…\}\s*>` finds only format strings), so exposure is to future code, but the syntax is ordinary in const-generic Rust and the contract rates a false negative P2.
- **Required correction.** In `end()`, when `t == '{'` and the previous token is `<`, `=` or `,`, step over the matched group as for `(`/`[`; in `in_item_list()`'s backward walk, when `text(h) == '}'` and the token before its matching `{` is `<`, `=` or `,`, jump over the group instead of stopping.
- **Regression test.** The four snippets flag `struct D`, `fn f`, `struct W`, `delegate!`; `#[cfg(unix)] const X: u8 = { 3 };` still keys `const X`; `#[cfg(unix)] fn g() -> Foo<N> { … }` still passes.

### P3-1 — No CI job runs the check over gwz-cli or gwz-py; the sibling entries are never validated outside a developer's workspace

- **Root cause.** Every gwz-core CI invocation passes `--skip-cfg-siblings` or `--skip-repo gwz-cli --skip-repo gwz-py`; gwz-cli's and gwz-py's own workflows run no gwz-core check (`gwz-py` runs its own `run_tests.py`; `gwz-cli/.github/workflows/*` none). `check()` skips STALE for a skipped repo (`:361-363`).
- **Location.** `.github/workflows/checked-artifact-boundary.yml:80-82`; `release.yml:92,145`; `platform-matrix.yml:51`; `windows-matrix.yml:45`.
- **Violated invariant.** CS1.7: "The check covers gwz-core with its `crates/`, gwz-cli and gwz-py's native crate"; standing rule "CS1.7's check runs in every step's gate." The precedent for the same shape (transport check in no CI, B11: Safety P2-9 / Consistency P3-31) was closed by CS1.8 pinning a commit and checking it out in the boundary job.
- **Reproduction.** In the boundary job the run prints `SKIPPED GATE … gwz-cli … gwz-py`; `Repository.test_trees_add_no_unbraced_conditional_declaration` skips the same two. A stale or wrong `gwz-cli`/`gwz-py` entry, or a new bare cfg import in either repo, is reported by no job.
- **Impact.** Sibling coverage holds only on a developer machine, where it fails gwz-core's `run_tests.py` for a change made in another lane; an allowlist entry for a sibling can be added or left stale without any CI catching it.
- **Required correction.** Either check gwz-cli and gwz-py out beside gwz-core in the boundary job at commits the allowlist records machine-readably (it already names them in prose) and run the check without skips there, as CS1.8 does for gwz-transport; or record in the plan's CS1.7 exit that sibling coverage is local-only until the multi-repo checkout (R2-D §11.3 item 7), so the gap is a decision and not an omission.
- **Regression test.** For the first option, a test in `test_check_cfg_boundaries.py` asserting the allowlist's recorded sibling commits are 40-hex and a workflow-text assertion that the boundary job runs the check without `--skip-repo`; for the second, the plan note.

### P3-2 — A macro item whose path starts with `::` is not recognised

- **Root cause.** `unbraced_item()` (`:245-248`) requires the first token to be an identifier; a leading `::` yields `None`.
- **Location.** `check_cfg_boundaries.py:245-248`.
- **Violated invariant.** The docstring includes "a macro invocation item such as `m!(...);`" without restriction on the path form.
- **Reproduction.** `#[cfg(test)] ::probe::fixture!(x);` `=> []`; `#[cfg(test)] probe::fixture!(x);` `=> [('#[cfg(test)]', 'probe::fixture!')]`.
- **Impact.** Bounded: the form is absent from the trees and unidiomatic; a bare cfg on such an item would pass.
- **Required correction.** Skip a leading `::` before the `id (:: id)*` loop and include it in the key.
- **Regression test.** The snippet yields `::probe::fixture!`.

### P3-3 — A non-ASCII identifier is not lexed as an identifier, so a named item carrying one passes

- **Root cause.** `_LEX` `id` is `(?:r\#)?[A-Za-z_]\w*` (`:63`); a name whose first character is non-ASCII lexes as `punct` + `id`, and `unbraced_item()` requires `kind(name) == 'id'` (`:242-243`).
- **Location.** `check_cfg_boundaries.py:63, 242-243`.
- **Violated invariant.** Non-ASCII identifiers are stable Rust; the docstring's definition does not exclude them.
- **Reproduction.** `#[cfg(unix)] mod über;\n#[cfg(unix)] const ünit: u8 = 0;` `=> []`.
- **Impact.** Bounded: no such identifier exists in the trees (grep); `use` items are unaffected (their key renders the same text).
- **Required correction.** `(?:r\#)?[^\W\d]\w*`. Note the precedent's lexer has the same limitation; fixing only here widens the divergence recorded in §3.
- **Regression test.** The snippet yields `mod über`, `const ünit`.

## 2. Invariant analysis (attacks that held, with evidence)

- **Lexer.** Nested block comments containing a cfg and an import (`/* a /* b */ #[cfg(unix)] use a::B; */`), line and doc comments (`//`, `///`, `//!`, `/** */`), plain strings with escaped quotes and backslashes (`"\\\"#[cfg(unix)] use a::B;"`), raw strings (`r#"…"#`, `r"…"`), byte strings, chars `'\''`, `b'"'`, `'#'`, `b'['`, lifetimes `'a`, `'_`, labels, CRLF line endings, `#![cfg]` inner attributes at file and block level, `r#type`/`r#match`/`r#mod2`/`r#try` raw identifiers — all lexed correctly; the real import after each decoy was flagged and nothing inside comments or literals was.
- **Attribute attachment.** `#[cfg]` separated from its item by a line comment, a doc comment, a block comment, `#[allow]`, `#[doc = "x"]` or `#[path]`; multi-line predicates; `#[cfg(unix)] #[cfg(target_os = "linux")]` stacking rendered into one key; `#[cfg( windows )]` and `#[cfg(windows)]` render identically; rustfmt trailing commas and wrapping do not change a key (`test_keys_survive_line_drift_and_reformatting` plus my own run).
- **Item classification.** `pub`, `pub(crate)`, `pub(in crate::x)`, `pub(super)`; qualifiers `unsafe`, `async`, `const`, `default`, `safe`, `extern "C"`, `extern` without ABI; `use` groups and globs, `extern crate … as …`, `pub extern crate`; `mod name;` with `#[path]` and `#[cfg_attr(unix, path = "x.rs")]`; `type`/`const`/`static`/`static mut`, `const _`; unit, tuple and empty-tuple structs, tuple struct with a `where` clause; required versus provided trait methods including `-> impl Iterator<Item = u8> + '_;`; foreign `fn` and `static` in `extern "C" { }` and `unsafe extern "C" { }`; items inside fn bodies; `macro_rules!` with `( … );` versus `{ … }`; `#[cfg]` on `fn`, `impl`, `mod { }`, `struct { }`, `enum`, `union`, `trait`, `extern { }`, `cfg_if! { }`, `thread_local! { }` all pass as braced; a bare cfg inside a `cfg_if!` arm or a platform module is flagged while the plain import inside the arm passes (the plan's two required outcomes).
- **`cfg_attr` folding.** `all()` true, `any()` false, `not(any())` true, nested `cfg_attr`, an empty applied list, `cfg_attr(unix, cfg_attr(all(), allow(x)))` conditional — as documented. Contradictions such as `all(unix, windows)` and the constant `#[cfg(all())]`/`#[cfg(any())]` are over-flagged; harmless, nobody writes them.
- **Ratchet.** NEW, COUNT and STALE each fail (tests, and `test_a_modified_occurrence_is_new` shows an edited predicate or item as NEW + STALE); a moved file is STALE + NEW; an occurrence moving within a file keeps its key and count (acceptable: the inventory is unchanged); a count above or below the tree fails; an entry with no occurrence is STALE, so the list cannot grow without a matching occurrence in the same change; invalid entries, duplicates, unknown repos and non-positive counts are rejected (`test_entries_are_validated`). The `--list` mode filters ratchet errors only for listing.
- **Coverage.** All 1196 `.rs` files under the declared roots are reached (verified by an independent `git ls-files` enumeration at HEAD giving the same occurrence set); skips are exactly hidden directories and `target/` beside a `Cargo.toml` (`test_build_output_and_hidden_directories_are_skipped` shows `src/target/mod.rs` is still scanned).
- **Wiring.** Fail-closed without siblings; SKIPPED GATE printed per skipped repo by the checker (the transport precedent prints it in `run_tests.py` — equivalent outcome); `--skip-repo nowhere` rejected; every CI and release path that lacks siblings carries the flag; the boundary job runs both the check and its tests; the release helper's flag is pinned by a test; nothing else now fails in CI or release beyond the two recorded HEAD failures, which are unchanged.
- **Scope and budget.** Additions beyond the plan's file list (`release.py`, the four workflows, `test_release_boundary.py`, `--list`) are required by the fail-closed design and mirror CS1.8; nothing extraneous. Non-blank code is at the < 350 aspiration; the 427 figure counts blank lines. The decomposition (lexer, attribute walk, item classifier, ratchet, CLI) is the precedent's and is right for a lexical check.

## 3. Risks and next action

- **Escalation.** The two P2s escalate the step to Safety by the brief's rule. I request it on P2-1's merits independently: the excluded class contains disabled-arm placements (`#[cfg(not(unix))] let`, `#[cfg(windows)] super::fault::fault(…)`), which is the blindness that broke v1.0.5.
- **Inventory growth.** Correcting P2-1 adds about 173 debt entries. That is what §1.3 provides for ("CS1.7 inventories that code; it does not migrate it"); the planning estimate of "about 90" was already superseded by 264.
- **Lexer duplication.** `_LEX` is copied from `check_process_globals.py` with one divergence (`r#` identifiers); P3-3's fix adds a second. A shared lexer module would stop the two checks disagreeing about the same file; out of this step's scope, worth a line in the plan.
- **Hidden crates.** `.github/bootstrap-crate/src/lib.rs` in gwz-core and gwz-cli is real Rust under a hidden directory and is not scanned; it has no `cfg` today.
- **Sibling coupling.** Until P3-1 is settled, a dirty gwz-cli or gwz-py lane fails gwz-core's `run_tests.py` and the `Repository` unittest on a developer machine. That is the plan's intent, but the CS1.7 exit should say so.
- **Next action.** A revision resolving P2-1 and P2-2 (P3-2 and P3-3 are one-line lexical fixes worth taking in the same pass; P3-1 needs an owner decision between checkout-and-pin and a recorded gap). Re-review the revision's diff on this axis against the regression tests named above, alongside the Safety review.
