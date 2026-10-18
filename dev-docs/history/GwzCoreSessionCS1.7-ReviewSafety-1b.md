# CS1.7 final touch-ups — Safety-axis confirmation

**Object:** the ten files at `scratchpad/cs17pg2-object.sha256`, all OK; HEADs unchanged (gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5`). Changed from the frozen first post-GO state (`cs17-pg1/`, which matches `cs17pg-object.sha256`, 10 OK): the checker, its tests, and the allowlist's line 2 only (lines 1 and 3–end byte-identical). `cs17pg2.diff` 108 lines, SHA-256 `1fd3f611…`. Read-only; nothing written.

**Runs on the final tree:** `test_check_cfg_boundaries.py` + `test_release_boundary.py` → `Ran 58 tests OK`; the check → `1199 files, 437 listed occurrences (gwz-core 435, gwz-cli 2, gwz-py 0); nothing new`, exit 0; sibling skip `1037 files, 435`; `--shrink-from` self exit 0; `--list` byte-identical across two runs; my HEAD-blob scan with the final checker (1191 tracked files) → 432 keys, 437 occurrences, EXACT MATCH with the allowlist, so the classification changes moved no inventory.

## Items

1. **`impl`/`extern` blocks in a fn body — CONFIRMED.** `target()` returns `None` for `impl` and `extern` before the statement branch (after `extern crate` and the `fn` normalisation). Repros: `fn f() { #[cfg(unix)] impl Trait for X {} go(); }`, `unsafe impl Send for X {}`, `extern "C" { fn getpid() -> i32; }`, `unsafe extern "C" { safe fn getpid() -> i32; }` → all `[]` (were `impl Trait for X{}go()` etc.). Nothing lost: a bare cfg inside such a block is still flagged (`extern "C" { #[cfg(unix)] fn getpid() -> i32; }` → `fn getpid`; `impl X { #[cfg(unix)] const ID…; #[cfg(test)] delegate!(…); }` → `const ID`, `delegate!(…)`); `#[cfg(unix)] extern crate alloc;` in a fn body → `extern crate alloc`; `extern "C" fn cb() {}` → `[]`; module-level `impl`/`extern` unchanged; the same `impl` inside a statement-position `cfg_if!` arm → `[]`. Test `test_impl_and_extern_blocks_in_a_body_are_braced_items`.

2. **`async` dropped from `BRACED_STATEMENTS` — CONFIRMED.** `BRACED_STATEMENTS = {'{', 'if', 'match', 'loop', 'while', 'for', 'unsafe'}`. Repros: `#[cfg(unix)] async { go().await }.await;` → `async{go().await}.await`; the `async move` form → `async move{go().await}.await`; the never-awaited `async { go() };` → `async{go()}` (an expression statement that needs its `;`, consistent with the grammar); the deletion replay lands on `next()`. Controls: `async fn g() {}` in a fn body → `[]`; an `async` block as a tail → `[]`; `spawn(async move { … });` keys the whole call; `unsafe { async { … }.await };` → `[]` (braced by `unsafe`); `async { unsafe { go() } }.await;` → keyed. Test `test_an_async_block_statement_is_not_braced`.

3. **Summing trade-off recorded — CONFIRMED.** The `rule` text no longer contains "fails on an added entry or a raised count"; it now says the mode "fails on an added occurrence (a count, summed over files, above the base's) or a narrowed scope (a repository dropped or moved, or a base root no longer covered)" and that it "passes a change that removes a listed occurrence in one file and adds an identical one elsewhere in the same repository, because a movement-only split cannot be told apart from remove-and-recreate; review reads an allowlist path change as a move claim to verify". The docstring carries the same paragraph. Test `test_the_docstring_and_the_rule_state_what_shrinking_means` pins both texts and the removal of the old phrase.

## Regression sweep

The braced forms (block, `if`, `match`, `for`, `while`, `loop`, `unsafe`, a labeled loop, statement-position `cfg_if!`, `probe! { x };`, `const { … };`, a tail) still give `[]`; the P3-8 keys (`state = State{mode}`, `return S{a:1}`, `let s`) and the P3-9 case (`let x` after `#![allow(unused)]`) are unchanged; the match-arm shapes and the two real arm sites yield no key.

## New defects

None. I looked for an unbraced form beginning with `impl` or `extern` that the early return could swallow (`extern crate` is handled first; `extern "C" fn` and `unsafe extern "C" fn` normalise to `fn`, so a bodiless one in a trait is still `fn f`) and for a braced-by-grammar statement beginning with `async` (there is none: an `async` block is an expression without a block), and found neither.
