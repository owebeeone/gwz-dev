# Phase 5 rows A, B and guard (steps 5.2, 5.3, 5.5): code-axis review

**Review object:** the Phase 5 rows A, B and guard as landed in lane `p5-gitmap`. That is gwz-git `7444b1f..12521b4302` (`0b9d2be`, `a5a4a74`, `e3bc2a4`, `12521b4`) and gwz-core `642211b9..f0bff70f80` (`0b378bd4`, `63fbd5b1`, `849691c7`, `f0bff70f`). The brief says five gwz-core commits; `git log 642211b9..HEAD` lists four. Step 5.2 is two packages, which are the commits `0b378bd4` and `63fbd5b1`.
**Baseline:** step 5.1 and 5.1b (gwz-core `642211b9`, gwz-git `7444b1f`), used as reference only.
**Date:** 2026-10-25
**Axis:** Code (architecture, interfaces, call graphs, compatibility reality)
**Verdict:** **NO-GO.** P0 0, P1 0, P2 2, P3 4.

## 0. Evidence base

**Tuple, identical at start and end**
- gwz-core `f0bff70f8035bf6aede24452dfd8374178acde15`, clean.
- gwz-git `12521b4302a2e68fd1350da2aa6b633808ad7e88`, clean.
- gwz-core-evidence `efba6961a9d8cc73865fd92a82e8e9ff695fe4b4`. One untracked run directory was already there before this review (`campaigns/transport-qualification/runs/2026-09-22-alpha-setup-timeout/`).
- gwz-transport `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252`.
- git2-rs `d13951f7e0bfb6e0efcee1207ac5b140adefa455`.
- Lane root `f0e7c0ecd2`. It already had staged changes to `gwz.conf` and untracked `dev-docs/*` before this review.
- I changed nothing in the lane. All probes ran from the session scratchpad.

**Pin per gwz-core commit** (`git show <sha>:.github/gwz-git.commit`):

| gwz-core commit | Pins gwz-git | gwz-git commit holds the code core calls |
|---|---|---|
| `0b378bd4` | `0b9d2be` | `Error::message`, `is_repository` |
| `63fbd5b1` | `a5a4a74` | `native_commit` |
| `849691c7` | `e3bc2a4` | refs and reads |
| `f0bff70f` | `12521b4` | `index_snapshot` |

Each pin names the gwz-git commit holding the code its core commit calls. The `.github/git2-rs.commit` files of gwz-git and gwz-core both name `d13951f7`.

**Read**
- Every diff in the range, both members.
- The old core modules at `642211b9`: `native_commit/**`, `repository.rs`, `refs.rs`, `comparison.rs`, `transport.rs`, `repository_support.rs`, the status-character helpers and `verify_checkout_state.rs`.
- The gwz-git surface: `lib.rs`, `error.rs`, `repository.rs`, `refs.rs`, `objects.rs`, `graph.rs`, `status.rs`, `remotes.rs`, `index.rs`, `native_commit.rs`, `native_commit/{cli,verify,notes,environment,route,system_config}.rs` and `tests/compat.rs`.
- gwz-core: `reads.rs`, `commit_tag.rs`, `backend.rs` and `spawn_gate.rs`.
- The checks and their wiring: `check_filesystem_boundary.py`, `check_gwz_git_adoption.py`, `gwz_git_boundary.json`, `gwz_git_adoption.json`, `run_tests.py`, `windows_lane_check.py`, `checkout-siblings.sh`, every gwz-core workflow, gwz-git's `ci.yml` and `tests/transport_backend/prepare.py`.
- Documents: plan §4 Phase 5, the Git plan §3–§6, guide A1–A3, map §2.1 and §8.
- Evidence: the p5-2, p5-3 and p5-5 READMEs and inventories.

**Moved-versus-changed, by direct `diff` of `642211b9:src/git/gitbackend/native_commit/**` against `gwz-git/src/native_commit/**`**
- Byte-identical: `route.rs`, `config_view.rs`, `hooks.rs`, `ident.rs`, `message.rs`, `notes.rs`, `staging.rs`, `system_config.rs`, `system_config/tests.rs`.
- `exec.rs`: only the error type changed.
- `environment.rs`: only `process()` was removed, plus a doc edit.
- The Git CLI fallbacks in `cli.rs` match the old `commit_cli`, `tag_create_cli` and `tag_delete_cli` in commands, `current_dir`/`-C`, flags and messages.

**Ran (read-only)**
- `check_gwz_git_adoption.py`: 37 capabilities, 32 consumed, 5 exceptions, pass.
- `check_filesystem_boundary.py`: 5 zero-tolerance files, 6 exception files with 82 uses, pass. Wall time 10.3 s.
- `python3.13 -m unittest test_check_gwz_git_adoption.py test_check_filesystem_boundary.py`: 31 OK.
- Scratchpad probes:
  - the adoption check's attribution of each capability to its caller files;
  - the token scanner on cfg_if test blocks, alias, glob and helper samples;
  - the `windows_lane_check.triggered()` result per commit;
  - prepare.py's `anchor_dependency_paths` on gwz-core's manifest with and without a gwz-git sibling.
- No cargo run. The 1-minute load was 42–74 throughout, so I took test results from the lane's logs.

## Problem, requirement provenance, and scope

**Authority:**
- D-G: Phase 5 rows 5.1–5.3 and the Git plan.
- D-K: F2 superseded, so native commit and tag move to gwz-git.
- Plan §4, steps 5.2, 5.3 and 5.5.
- R9: no capability is complete without its first production consumer.

**Components changed:** gwz-git, gwz-core and gwz-core-evidence only. The gwz-cli and gwz-py lockfiles are out of this lane, as the brief says. Component authorization holds.

**The lane owner's additions:**
- **Moving `repository_paths` and `repository_index` (in 5.5).** Map §2.2 lists both among G2's callers, and plan 5.3 is row G2, so the work traces to an authorized row. The new public `index_snapshot`, `IndexSnapshot` and `IndexEntry` have a real production consumer: `checked_artifact/.../provider/index.rs:26,87-91` uses every field, including `flags_extended`, `ctime` and `stat`. Accepted.
- **The "CLI reasons stay in core" reading.** Core keeps the decision whether anyone is told: `with_route_notes`, `RouteNotes` re-exported, `Services::with_notes`. `CliReason` text moved with the route rule that produces it. Accepted. It is behavior-neutral (`notes.rs` is byte-identical) and no gwz-core caller ever inspected the reason.

No unauthorized expansion found. The four P3s below are defects inside authorized deliverables.

## 1. Findings

### P2-1: The Transport candidate workflow cannot build gwz-core after `0b378bd4`, because gwz-git is never checked out

- **Root cause:** `0b378bd4` added the unconditional dependency `gwz-git = { path = "../gwz-git" }` (`gwz-core/Cargo.toml:85`, `[dependencies]`). The same commit taught `checkout-siblings.sh` to fetch gwz-git. But `.github/workflows/transport-candidate.yml` does not use `checkout-siblings.sh`. Both of its jobs check out their siblings by hand:
  - `candidate`: lines 64–90, which check out gwz-transport, gwz-py, gwz-sspi and taut;
  - `candidate-windows`: lines 251–281, the same set.
  Neither checks out gwz-git, and the range never touches the workflow (`git diff --stat` lists only `checked-artifact-boundary.yml`).
- **Invariant violated:** every job that runs cargo on gwz-core has every path dependency beside it. The rule is stated at `checkout-siblings.sh:1-9`, and that script's own pin comment names this job.
- **Reproduction:** I ran prepare.py's own `anchor_dependency_paths` (`tests/transport_backend/prepare.py:42-47`) on the tuple's `Cargo.toml`, with a core directory whose siblings are git2-rs, gwz-transport and gwz-sspi:
  - without gwz-git, it exits with `path dependency has no manifest: …/gwz-git`;
  - with a gwz-git sibling added, it reports `anchored OK`.
  So the step "Prepare the candidate build" fails in both jobs (lines 153 and 355). Even without that step, `cargo check` (the Windows "Library check" steps) and `run_tests.py` would fail: `check_git_process_globals()` raises "no gwz-git checkout found", and the module docstring says there is no skip flag.
- **Impact:** the candidate job and `candidate-windows` are red on every push from `0b378bd4` on. AGENTS.md says "A pushed lane is covered by the `candidate-windows` job". That is the transport release's Windows behavior gate and the candidate-suite gate, so the release's main CI signal is lost while the lane looks green locally. The local transport legs passed only because the lane workspace has gwz-git beside gwz-core.
- **Required correction:** add a gwz-git checkout at the `.github/gwz-git.commit` pin to both jobs, either by calling `checkout-siblings.sh` or with an explicit `actions/checkout` step. Add gwz-git to the "pinned commits are on main" loop (line 110 and its Windows twin).
- **Closure test:** a source check, which could be added to `test_run_boundary_job.py` or a new unit test. For every workflow job that runs `cargo`, `prepare.py` or `run_tests.py`, it asserts that the job either calls `checkout-siblings.sh` or checks out every `path = "../X"` sibling named in `Cargo.toml`. It must fail on the tuple and pass after the fix. Then a green `Transport candidate` run on the pushed lane.

### P2-2: The moved G1/G2 tests no longer run on Linux or macOS in any CI, and the Linux `Host::Unix` route is unexercised by every gate

- **Root cause:** before the move, gwz-core's `platform-matrix.yml` ran the full library suite on `macos-14` and `ubuntu-24.04-arm` (lines 20–23, 64). That suite included the native commit and tag tests (133 lib tests, among them the Unix-only hook and mock-signer characterization) and the G2 read tests (`g01`, `g05`, `g14`). After the move they live in gwz-git. gwz-git's only test workflow, `gwz-git/.github/workflows/ci.yml`, has one job, `windows` (lines 19–20). No gwz-core workflow or `run_tests.py` runs gwz-git's `cargo test`: `run_tests.py` runs only gwz-git's process-global and adoption checks. The workflow's own comment (`ci.yml:11-12`) promises "step 5.2 adds the ones its gates need", and 5.2 added none. The range never touches gwz-git's `.github/`.
- **Invariant violated:** a mechanical move must not reduce the platforms its tests run on. This is the Git plan §5 G2 exit ("implementation and adapter tests pass"), on a release scoped as "CLI transport on 3 platforms". The moved production code has a Linux-only branch: `system_config.rs:38-47` `Host::current()` returns `Host::Unix` on Linux and selects Linux's system-config conventions for the route decision. The lane's evidence ran gwz-git tests only on macOS (local) and Windows (dabeest). Nothing has run them on Linux since the move.
- **Impact:**
  - A Linux regression in the commit/tag route rule (system gitconfig, hooks executable bit, `includeIf`) or in the reads would reach release undetected.
  - The 12 Unix-only tests now run only on a developer's Mac, in no CI.
  - This is the repository's documented failure mode: the "CI after push" record of three days of red Linux CI that local gates missed.
- **Required correction:** run gwz-git's `cargo test --locked` on Linux and macOS in CI. Either add `ubuntu-24.04-arm` and `macos-14` legs to gwz-git's `ci.yml`, or have gwz-core's `platform-matrix` run `cargo test --manifest-path ../gwz-git/Cargo.toml` against the checked-out pin. Update the stale `ci.yml:11-12` comment either way.
- **Closure test:** green CI logs at the gwz-git pin on Linux and macOS showing the 133 lib tests (121 on Windows), `refs_reads` (18) and `index_snapshot`. A source assertion that some workflow runs gwz-git's tests on a non-Windows runner.

### P3-1: The 5.5 guard does not see native git2 use reached through core helpers; `comparison.rs` sits in the zero list while holding native handles

- **Root cause:** `git2_uses()` (`check_filesystem_boundary.py:280-298`) counts the identifier `git2`. Native handles reach a zero-tolerance file without that token:
  - through `use super::*;` (glob-importing the parent's private `use` items);
  - through core helpers that return `git2::Repository`: `open_repo` re-exported at `gitbackend.rs:80`, `repo_head`, `find_remote`.
  `src/git/gitbackend/comparison.rs` is listed in `zero` (`gwz_git_boundary.json:7`), yet opens `git2::Repository` at lines 10 and 19 via `open_repo` (the G8 diff stays in core). Probe: the sample `use super::*; fn f(p:&Path){ let repo = open_repo(p)?; repo.merge_base(a,b); }` yields `git2_uses == []`.
- **Invariant violated:** Git plan §6, "guard the chosen core migration area against new direct binding imports **or duplicate single-repository implementations**". The probe tests in `gwz_git_tests.rs` catch only exact old `fn` names in old files.
- **Impact:** a second live `merge_base`, `status` or `head` written in `reads.rs` or `comparison.rs` over `open_repo` passes the guard. The zero list claims more insulation than it checks.
- **Correction:** also count calls to core's native-handle producers (`open_repo`, `repo_head`, `find_remote`, `git_error`) in zero files, or record those producers in the config as banned tokens for the zero list. Then either move `comparison.rs` to `exceptions`, with a count and the G8 row, or drop it from `zero`.
- **Closure test:** a unit test in `test_check_filesystem_boundary.py` where an `open_repo(path)?.merge_base(..)` sample fails in a zero file. The checker run on the tuple then reports `comparison.rs` as an exception, not zero.

### P3-2: The adoption check credits gwz-git capabilities to calls of the same name on git2 or `GitBackend`

- **Root cause:** `first_caller()` (`check_gwz_git_adoption.py:246-272`) treats a file as a caller of a receiver method when the file names `gwz_git` and the owner identifier, and contains `.name(`. The owner `Repository` also matches `git2::Repository`, and `.head(`, `.status(` and `StatusOptions::new` also match git2 and trait calls. Probe output at the tuple:
  - `Repository::head` matches `reads.rs`, `repository.rs:35,88` (`backend.head(path)`, a `GitBackend` call) and `verify_checkout_state.rs:76` (git2 `.head()`).
  - `Repository::status` and `StatusOptions::new` match `verify_checkout_state.rs:86,94` (`git2::StatusOptions::new()`, `entry.status()`).
  - `Error::message` is credited to `verify_checkout_state.rs`, which calls `error.message()` on both a `gwz_git::Error` and a `git2::Error`.
  - Module-level test gating is also invisible: a file reached only through `#[cfg(test)] mod x;` and not named `*_tests.rs` counts as production (`is_test_source`, `check_filesystem_boundary.py:273-277`).
- **Invariant violated:** R9 and Git plan §5 G2 ("a test-only consumer does not count"), plus the 5.5 ratchet's own claim that the list "only shrinks".
- **Impact:** if `reads.rs` stopped calling gwz-git's `head` or `status`, the check would still report them consumed and the ratchet would not object. Combined with P3-1, both 5.5 checks pass while a duplicate implementation lives in core.
- **Correction:** match a receiver call only on a binding the file obtained from `gwz_git`. The cheap form: require that the file lists the owner in a `use gwz_git::…` leaf or writes it as the `gwz_git::Owner` path, and exclude owners that also appear qualified by `git2::`. Alternatively, report every matching file and require at least one whose match is not shadowed by a `git2` path.
- **Closure test:** a unit test where a core file with only `git2::Repository`, `repo.head()` and a `use gwz_git::Error` import does not count as a caller of `Repository::head`.

### P3-3: `Repository::open_from_env` has no consumer, even inside gwz-git, and its ratchet entry and guide A1 say native commit uses it

- **Root cause:** native commit opens through its own copy of the same open: `native_commit/route.rs:82-89` calls `git2::Repository::open_ext(path, NO_SEARCH | FROM_ENV, &[])` directly, duplicating `Repository::open_with` (`repository.rs:28-39`). `grep` finds no caller of `open_from_env` outside `tests/open_resolve.rs`. Yet:
  - `gwz_git_adoption.json`, the reason of the `open_from_env` entry (line 21), says "Native commit and tag open with it inside gwz-git";
  - guide A1 (`GwzGitLibraryApi.md`) says "It is the open native commit and tag use";
  - `lib.rs:24` calls it "the one open that lets the native layer read Git's variables".
- **Invariant violated:** an R9 exception must be true (map §6 ratchet: each entry says why it is unconsumed), and there must be no second live implementation (Git plan §5 G2).
- **Impact:** the exception's exit ("the G3 decision") rests on a false premise. A future fix to the environment-honouring open (for example, which variables are honoured) will not reach native commit. A reviewer reading the ratchet is told a consumer exists when none does.
- **Correction:** make `route::open` delegate. Either `Repository::open_from_env(path).map(|r| r.native).map_err(|_| CliReason::CannotOpen)`, or give `Repository` a `pub(crate)` accessor. Then correct the entry's reason, guide A1 and `lib.rs:24`, or state truthfully that no caller exists.
- **Closure test:** `grep -rn 'RepositoryOpenFlags::FROM_ENV' gwz-git/src` finds only `repository.rs`. The existing `native_commit` route tests stay green on macOS and Windows.

### P3-4: `RoutePolicy::CLI_ONLY` and `AUTO` became public gwz-git API with only test consumers, outside the adoption check's view

- **Root cause:** in core, the `RoutePolicy` constants were `pub(crate)` and `cfg(test)` (old `native_commit.rs`, test `cfg_if`). In gwz-git, `native_commit.rs:53-69` makes `RoutePolicy` public with `pub const AUTO` and `pub const CLI_ONLY`. The type is reachable through the public field `Services::policy` and `Services::with_policy`.
  - Core production never sets a policy other than the default. `CLI_ONLY` is used only by `spawn_gate_tests.rs:24-28` and gwz-git's tests.
  - `public_item()` (`check_gwz_git_adoption.py`) skips constants by design, so this test-only knob is invisible to the R9 check.
- **Invariant violated:** R9 / Git plan §5 G2, "a test-only consumer does not count". Guide §3 calls for narrow capabilities.
- **Impact:** once gwz-git is published (its `publish = false` is to be lifted at Phase 8), "force the Git CLI" becomes a supported knob. Removing it would be semver-breaking, and a production caller could start forcing the CLI route without any gate noticing.
- **Correction:** either record `RoutePolicy::CLI_ONLY` (and `AUTO`, or the `with_policy` path) as a dated exception, with an exit, by making the checker cover public constants; or put the forcing knob behind a non-default `test-support` feature that gwz-core enables only for dev-dependencies.
- **Closure test:** the adoption check lists public associated constants, and either fails on `RoutePolicy::CLI_ONLY` or shows it as a dated exception.

## 2. Invariant analysis (attacks that failed, with evidence)

**No git2 type crosses the public surface.** No `pub` item outside `pub(crate)`/`pub(super)` names `git2`. `Repository.native` is `pub(crate)`. `CliReason`, `route::open` and `HeadState`/`TagPlan` stay private. Records are owned and `#[non_exhaustive]`: `Head`, `Remote`, `Status`, `FileStatus`, `StatusOptions`, `AheadBehind`, `IndexEntry`, `IndexSnapshot`, `CommitOutcome`, `TagOutcome`, `Services`. `NativeDiagnostic` (pub fields, not `non_exhaustive`) is G0/5.1b, so it is baseline.

**Error translation preserves codes and texts.**
- `model_error` (`verify_checkout_state.rs`) maps `InvalidPath` to `InvalidRequest` (the old `read_file_at_commit` code) and everything else to `GitCommandFailed`. Old `git_error` gave `GitCommandFailed`, except the credential-helper classes, which no gwz-git call reaches; the old `native_commit` used only `git_error` and `GitCommandFailed`.
- The text is `Error::message()`, which equals the native `error.message()` that `git_error` used, or the identical plain strings moved in `cli.rs`, `verify.rs` and `exec.rs`.
- The open of a non-repository gives the same code and the native message (`from_open`, NotFound → `NotARepository`). Core's `is_repository` matches the old `git2::Repository::open`, which is libgit2 `open_ext(NO_SEARCH)`.

**The recorded behavior changes hurt no caller.** I checked every core call site of `is_ancestor`, `ahead_behind`, `merge_base`, `changed_paths_between`, `read_file_at_commit` and `commit_exists`:
- `publication.rs:463,528` and `observe.rs:183-215` propagate or accumulate errors without branching on their code or text.
- Strict full-hex parsing (`parse_hex` accepts either case, like `Oid::from_str`) only refuses zero-padded short text that never named an object. `commit_exists` still answers `false` for non-ids.
- Open-before-parse changes only which error is reported when both are wrong.
- `StateRead` instead of `IoError` affects only an unborn `HEAD` file that cannot be read immediately after libgit2 read it.
- The index id round-trip is `ObjectId::from_git2(entry.id).as_bytes()`, the same bytes as `Oid::as_bytes`.

**Status-character mapping is the same.** It is byte-equivalent to the deleted `index_status_char.rs` and `worktree_status_char.rs`, now `char` projected to `String`.

**The CLI fallback moved intact.** `cli.rs` has the same three `Command::new("git")` sites as the old `repository.rs`/`refs.rs`, with the same `current_dir` and `-C`, flags and messages. They run through `ProcessGate`, which `SpawnGate` implements (`spawn_gate.rs:140-145`) with the old `SpawnGate::output` call. gwz-git's allowlist holds exactly those 3 (`process_globals_allowlist.json`, `debt`). The spawn-gate tests still drive all three through the core gate (`spawn_gate_tests.rs`, forced CLI).

**gwz-git reads no process environment.** In non-test code, the only `std::env` use is `split_paths` over the captured `PATH` (`system_config.rs:100`). The capture is core's `process_environment()` (`backend.rs`), and `GitEnvironment::process()` was deleted. The self-verify reads open `open_exact`, exactly as the old `backend.head` and `tag_list` reads did through `open_repo`. That is no regression for `FROM_ENV`-opened commits.

**The old implementations are gone (R9).**
- `native_commit/**` and both characterization files are deleted from core.
- `commit_cli`, `tag_*_cli` and `verify_tag_*` are deleted.
- The eleven G2 functions, `git_file_status`, `rename_delta`, `unborn_head` and both status-character files are deleted.
- The `repo_head` body is now `head_at(repo.path())`, so there is no second HEAD reader.
- No new `#[allow(dead_code)]` in production; the two in the diff are gwz-git `tests/compat.rs`.
- Every remaining capability has a real production caller (`commit_tag.rs`, `reads.rs`, `repository.rs`, `backend.rs`, `spawn_gate.rs`). The exceptions are the five listed; P3-2 and P3-3 qualify their honesty.

**Pin discipline holds.** Each commit's pin holds the code it calls (table in §0). `run_tests.py` refuses a checkout that is not at the pin unless `--allow-git-pin-mismatch` is given. The boundary workflow runs `checkout-siblings.sh` (lines 56/64) before the adoption check (line 113).

**The guard's mechanics are sound.**
- Counts may fall and never rise; both directions fail (`git2_boundary_failures`).
- A zero path that also holds an exception is refused.
- Test items are blanked, including `cfg_if! { if #[cfg(test)] { … } }` (probed).
- Alias imports count one token, by documented design.
- The cheap prefilter keeps the quadratic `tokens()` slicing off unrelated core files. `tokens()` is pre-existing and not in the diff.
- `windows_lane_check.triggered()` fired on all four commits (`.github/gwz-git.commit` and `gitbackend.rs`). `reads.rs` alone does not trigger, but it has no platform branch, and any gwz-git change moves the pin, which is a trigger path. That is an observation, not a defect.

**Windows parity.**
- The `cfg(unix)` gating of the `commit_tag_characterization` module drops no test: at `642211b9` its whole test body was already in a `#[cfg(unix)] mod unix`.
- The identity characterization has its own `on_both_routes!` and stays ungated.
- Production Windows branches (`route.rs:98-106`, `staging.rs:47`, `system_config.rs:224`, `hooks.rs:25`) moved byte-identical.
- gwz-git's Windows CI runs 121 lib tests, including the 3 Windows-only ones.

**Build graph.** There is one libgit2. gwz-git uses `gwz-git2` at `path ../git2-rs`, `=0.21.0`; features unify with core's. Publishing gwz-core with a path-only gwz-git (`publish = false`) is recorded for Phase 8 (map, `GwzV110RemainingPlan.md:37`), so it is not a finding.

## 3. Risks and next action

**Risks outside this object (observations, not findings)**
- **Lossy paths become published API.** `changed_paths` returns lossy UTF-8 strings and `Status.files` omits non-UTF-8 paths, both moved verbatim from core. Once gwz-git publishes, both are hard to change, and `pull_head_*` preflights compare one against the other. This is a pre-existing limitation: decide it at the Phase 6 API review.
- **`merge_base` returns `None` for a missing commit.** It conflates a missing commit with unrelated histories (both are libgit2 `ENOTFOUND`). Pre-existing, but now in the public docs.
- **Case-insensitive environment names on Unix.** `GitEnvironment::from_vars` upper-cases names on every platform, so on Unix lowercase names count as `GIT_*`. This is the conservative direction (more CLI routes). Pre-existing.
- **Nothing enforces the git2-rs pin equality.** The equality of gwz-git's and gwz-core's `.github/git2-rs.commit` is stated, not checked.
- **A ten-second boundary check.** `check_filesystem_boundary.py` now takes about 10 s, because of the pre-existing quadratic tokenizer.

**Next action (bounded remedy; I commit to GO once it lands and nothing new appears)**
1. P2-1: add the gwz-git sibling checkout to both `transport-candidate.yml` jobs, plus the workflow-coverage source check.
2. P2-2: add Linux and macOS test legs for gwz-git, or run gwz-git's tests from `platform-matrix`, and show green logs.
3. P3-1 to P3-4 may close in the same package or be recorded with dates. Each correction is local:
   - one checker rule each for P3-1 and P3-2;
   - a one-line delegation and doc fixes for P3-3;
   - a feature gate or a recorded exception for P3-4.

None needs an architectural change.
