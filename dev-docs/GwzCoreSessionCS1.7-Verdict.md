# Core session plan CS1.7 (the conditional-compilation boundary check) — first review verdict

Date: 2026-09-28. Status: **NO-GO on the ten files at the SHA-256 values in the lane owner's `cs17-object.sha256` (diff SHA-256 `71f65bc0…`, 1173 lines): [Consistency](GwzCoreSessionCS1.7-ReviewConsistency.md) and [Safety](GwzCoreSessionCS1.7-ReviewSafety.md) each reported NO-GO, and each committed in advance to GO on a revision that resolves its blocking findings as specified.** This verdict accepts nothing.

The object was step CS1.7 of the [core session plan](GwzCoreSessionPlan.md): a lexical check that the root `AGENTS.md` rule on explicit conditional-compilation boundaries holds, with its allowlist of existing occurrences and its wiring into `run_tests.py`, `release.py` and four workflows. It is uncommitted in gwz-core's working tree.

The plan records a single-axis review, Consistency first.
- The Consistency reviewer found two P2s. By the plan's rule, and at the reviewer's request, that escalated the step to the Safety axis.
- The Safety reviewer ran on the same object, peer-blind: the Consistency report was held outside `dev-docs` until the Safety review had finished.
- Both reviewers verified the ten files and the gwz-core, gwz-cli and gwz-py HEADs at the start and at the end. Both ran the step's tests and the check, and exercised the check's functions on constructed snippets.
- Neither wrote a file.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | NO-GO | 0 | 0 | 2 | 3 |
| Safety | NO-GO | 0 | 0 | 1 | 6 |

## Blocking findings

| ID | Axis | Finding |
| --- | --- | --- |
| C-P2-1, S-P2-1 | both | Conditional attributes on `let` statements, expression statements and statement macros are outside the scan. The docstring excludes them on the premise that the rule's remedies hold only items. The codebase refutes that: `cfg_if!` works in statement position, and gwz-py uses it there with `let` bindings that outlive the macro. The cfa14b8 hazard passes with no signal at every such site: deleting a conditional `let` and leaving its attribute makes the next statement conditional. About 173 to 184 sites exist, disabled platform arms among them (for example, `#[cfg(not(unix))] let executable = false;` in the file-system handles). |
| C-P2-2 | Consistency | A `{ … }` in an item's header (a const-generic argument or default) makes the scanner judge the item braced, so a `#[cfg]` on such a unit struct, required trait method or macro item passes. There is no exposure in the trees today. |

Neither reviewer classified a finding as architectural. The lexer, the attribute walk, the item classifier, the ratchet mechanics, the fail-closed paths and the wiring all held under attack. This was the first round; the two-round cap has one round left.

## Blind convergence

Reviewing blind, the two axes landed on the same findings three times:
1. **Statements** (C-P2-1 and S-P2-1). Both cite the same Rust Reference definition: a `let` statement is a declaration statement. Both give the same correction: extend the scan and inventory the sites as debt. Both pre-committed to GO on it.
2. **Non-ASCII identifiers** (C-P3-3 and S-P3-5): the same one-character-class fix.
3. **No CI covers gwz-cli or gwz-py** (C-P3-1 and S-P3-6). Both name the transport check's precedent, which pins its sibling in the boundary job.

The Safety reviewer also found C-P2-2's header braces as a residual without exposure in its §2, and filed no finding for it.

## Confirmed by the reviews

- **Allowlist.** It matches the HEAD blobs of all three trees exactly: 251 keys and 264 occurrences, with none missing, none extra and no count mismatch.
- **Fail-closed paths.** A missing root, a root without Rust, and a wrong, malformed or missing allowlist each exit 1. A missing sibling fails closed unless skipped, and every skip prints SKIPPED GATE.
- **Lexer.** It held against raw, byte and C strings, char literals and lifetimes, nested and doc comments, raw identifiers, a BOM, CRLF, shebangs and `macro_rules!` bodies.
- **Item positions.** Every item-position replay of cfa14b8 is caught, inside disabled `cfg_if!` arms included.
- **Wiring.** Every CI and release path that runs `run_tests.py` passes the flag it needs, and the boundary job runs the check and its tests. Nothing else newly fails beyond the two failures recorded at HEAD.
- **Cost.** The check runs in about 2 seconds over about 1200 files, deterministically.

## Carried, not this step's

- **The lexer is duplicated.** The check's lexer is a copy of `check_process_globals.py`'s, with one divergence (`r#` identifiers), and C-P3-3's fix adds a second. A shared lexer would stop the two checks from disagreeing about one file (Consistency §3).
- **The process-globals allowlist has the same gap as S-P3-3:** a new entry can land with its occurrence. S-P3-3's mode could be shared by both checks.
- **`release.py --no-test`** skips `run_test_suite`, and with it this check and the other lexical checks. The tag can be pushed unchecked, though `release.yml` then fails on the tagged tree. This predates the step (Safety §3).

## Next action

The [remediation plan](GwzCoreSessionCS1.7-RemPlan.md) gives every finding one disposition. The implementer applies it in one patch. The same two reviewers then re-verdict the revision with their context intact.
