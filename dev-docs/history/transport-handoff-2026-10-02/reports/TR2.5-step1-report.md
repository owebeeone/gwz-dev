TR2.5 step 1 is done: gwz-core's `transport_setting` module is committed in the lane, every gate you listed passes, and nothing was merged, pushed or tagged.

## Commits (lane `/Users/owebeeone/limbo/gwz-dev-tr2-5a`)
| | gwz-core | root |
|---|---|---|
| 1. Docs only, landed first: the paired `GWZDesign.md` / `GWZRequirements.md` paragraphs | `7392b3be00c95d5a285c2a0106ac45eb35c097af` | `706d279ea58dbe75b4bb6c51d6c7f7d58da3bee6` |
| 2. The module, its tests, the `lib.rs` wiring and one inventory line | `ec1eeee3b2a9f1c94e71cdf038c99ca046eb27c5` | `05f6a4387b2b99f31f3ae51e6aa5011d46454179` |

Both were made with `gwz commit`, files staged by path, and carry no AI trailer. The untracked `gwz-core/dev-docs/GwzRemoteTransportBugReport.md` was not staged.

## API (`gwz_core::transport_setting`)
```rust
pub fn resolve(flag: Option<Transport>, environment: &EnvironmentSnapshot) -> Result<Setting, Refusal>;
pub fn ignored_values(operation: transport_scope::Operation, start: &Path, meta: &RequestMeta) -> Vec<IgnoredValue>;
pub fn path_text(path: &Path) -> String;                    // E1 path escaping, for the --verbose line
pub enum Transport { Gwz, Native }                           // name()
pub enum Source { Flag, Environment, GlobalConfiguration(Location), Default }   // name() = the JSON words
pub struct Location { pub file: PathBuf, pub included: bool }   // where_text(), remove_text()
pub struct Setting { pub transport: Transport, pub source: Source, pub read: Vec<PathBuf>, pub skipped: Vec<PathBuf> }
pub enum Refusal { Environment { value: String }, EnvironmentNotUtf8,
                   Configuration { location: Location, value: Option<String> },
                   Unparsable { file: PathBuf, cause: String } }  // message(Driver)
pub enum Driver { Cli, Python }
pub enum Scope { Root, Member(String) }                      // name(), member_id(), text()
pub struct IgnoredValue { pub scope: Scope, pub location: Location, pub value: Option<String> }  // entry_text()
```
`message(Driver)` gives §10's refusals in gwz-cli's words or gwz-py's. The module renders only the refusals and the shared pieces (`<where>`, `<remove>`, `<entry>`, `<scope>`); the notes, the `--verbose` line and the JSON are left for steps 2 and 3.

## Tests: 44 rows in `src/transport_setting/tests/{precedence,files,text,scan}.rs`
- **Precedence and values (11):** each form alone; each pair both ways; a lower form neither read nor checked, malformed included; the variable's case, trimming, empty-as-unset and non-UTF-8 refusal; the key's case; other values, empty value and no value refused; last value in a file wins; and the no-form default.
- **Files (12):** the XDG file then `.gitconfig`, which wins; empty and set `XDG_CONFIG_HOME`; empty, relative or unset `HOME`; `GIT_CONFIG_GLOBAL` and command scope not read; the snapshot's `HOME` read, not the process's; `include.path` followed and reported as included; a `~/` include resolved against libgit2's process-wide home; `includeIf` not applied; an unreadable file skipped; a FIFO or directory skipped without blocking; unparsable files refused with libgit2's file and line, an included file's included.
- **Text (9):** every refusal word for word for both drivers; value quoting; `<where>`/`<remove>`; no command for a path with a control character; `<entry>`/`<scope>` and the JSON words. The printed commands are run through `sh` and git:
  - the unset command removes both lines;
  - the locating command prints one line from the workspace and from `~`, even with a matching `includeIf` (control: without `-C /` git prints two);
  - the path `it's $(touch marker)` runs nothing else;
  - a newline in a path gives one escaped line.
- **Scan (12):** root and member `config` and `config.worktree` each reported once; untargeted member never read; FIFO (including at `.git/config`), symlink and >1 MiB skipped, exactly 1 MiB read; gitfile and `commondir` followed; an unlocatable git directory gives no note; an included value reported as such; clone, member clone and init check nothing; a global value is never an ignored value; each operation's default targets; whatever the operation would refuse gets no note; a shared file noted once; a member path with a newline.

**Evidence:**
- **Fail before:** the 43 rows written first, run against stubs of every function (`resolve` included), gave 39 failing and 4 passing. The 4 that passed are rows a do-nothing stub satisfies: the no-form default, clone checks nothing, unlocatable git directory, and refused selection.
- **Pass after:** all 44 pass. One row, `GIT_CONFIG_GLOBAL` and command scope not read, was added after the stub run.
- **Mutation runs on the negative rows:**
  - Four deliberate defects together failed 5 rows, three of them by hanging until the tests' 20-second guard: scanning a clone like a fetch, dropping the scan's regular-file/1 MiB check, scanning every target when the selection is rejected, and handing a FIFO global file to libgit2.
  - Ignoring the selection failed the untargeted and refused rows.

## Where I extended or narrowed the design's text (please confirm)
1. **Gating is `all(unix, gwz_transport_candidate)`,** not bare `gwz_transport_candidate`. §7 says the Windows sites open at S4.5, and libgit2's Windows file templates and Windows quoting belong to TR1.8. The inventory gains one site: 19 in total.
2. **The snapshot is core's existing `session_host::EnvironmentSnapshot`.** gwz-py's native entry already builds one; gwz-cli would build one from `vars_os` at its edge.
3. **A global file that isn't a regular file** after following links (a directory, FIFO or device) is treated as unreadable: skipped and reported, never opened. libgit2 itself would block on a FIFO.
4. **Cases the design doesn't cover:** an unreadable file that a global file *includes*, or a `~/` include when libgit2 has no home, makes libgit2 fail the whole file. That surfaces as the "could not parse" refusal, with libgit2's message (e.g. `failed open - '<path>' is locked`). git also dies on both. D4's skip covers only the global files themselves; skipping these too would need a design line.
5. **No printed command** for a path that isn't UTF-8 or isn't absolute. E1 names only control characters. Non-UTF-8 bytes in paths and values display as U+FFFD.
6. **How the scan finds targets:**
   - It uses `resolve_action_targets` with each handler's action.
   - Pull snapshot uses materialize's targets, because its handler runs materialize.
   - A materialize to a tag later narrows to the tagged members, which needs a backend, so the scan reads the whole selection.
   - A symlinked `.git` gets no note.
   - A file shared by two targets gets one note, under the first target's scope.
7. **Residual race:** a file could be swapped between the `symlink_metadata` guard and libgit2's open. The design specifies that guard as stated.
8. **Lane-gate base:** the brief names `ee06d16`, but the lane's gwz-core HEAD at creation was `fb7a992`. I ran the gate from both.
9. **Size:** 729 production lines (473 excluding comments and blanks) against §9's estimate of about 230; tests are 1,736 lines. Every file is under 500 lines.

## Gates (all pass)
- **Candidate, transport switch** (`run_tests.py` via the prepared manifest): 2,729 passed, 7 ignored. That is 18 + 129 + 1 in the fake and crosscheck runs, 2,512 in the library's main run, and 69 integration tests.
- **Candidate, both switches:** `cargo check --tests` is clean. Clippy finds nothing in the new files under either switch set; the 50 lib warnings it reports are existing candidate code.
- **Ordinary build:** `run_tests.py` passes 2,320, the same count as before. The boundary job's clippy with `-D warnings` is clean.
- **Source checks:** `cargo fmt --check`, checked-artifact boundary, switch inventory, process-globals (nothing new), cfg-boundary, filesystem and target-selection checks all pass.
- **Lane gate:** ok at every commit from both bases.

## Cleanup and disk
I deleted `candidate-target`, the lane's `gwz-core/target` (68G, mostly APFS-shared) and the prepared manifest in the scratchpad; re-prepare it to rerun the candidate leg. The root `target/` (202M) predates this work and I left it. Free disk is 28Gi. The logs (stub run, mutants, both suites, clippy) are in `/private/tmp/claude-501/-Users-owebeeone-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/tr2-5a/`.
