# GWZ transport release — handoff, 2026-10-02

## Current update, 2026-10-03

The five-file split in §6.1 is completed sequentially in main, as requested.
See [CurrentProgramCheckpoint.md](CurrentProgramCheckpoint.md) for the layout,
preservation proof and validation. TR2.4/TR2.7 are merged and tr2-4-7 has
been disposed without --keep. Earlier lane/status paragraphs below are the
historical handoff snapshot.


## Integration update — ready lanes merged

TR2.1, TR2.5 step 1, TR2.8 and TR2.4/TR2.7 are now merged into main through GWZ. The latest
entry in [CurrentProgramCheckpoint.md](CurrentProgramCheckpoint.md) records
the source tuple, fixture conflict resolution, validation and remaining work.
TR2.1 and TR2.5a were detached with `--keep`, preserving their entire trees.
TR2.8 has also been detached with `--keep`. TR2.4/TR2.7 remains registered,
but its committed work is fully merged. The two decisions in §3.3 were
resolved in favour of enclosing modules and profile-2 negotiation for mixed
offers. The preceding integrated tuple was pushed; TR2.4/TR2.7's merge and
this closing record have not been pushed or tagged. The handoff itself was
committed before these merges.

The moved workspace's old catalog blocked the first merge. On the operator's
explicit approval it was preserved outside the workspace, with checksums;
GWZ initialized a fresh catalog and the merge succeeded. All unrelated drafts
were restored and verified. The source snapshot below is historical wherever
it says these lanes are unmerged, TR2.8 is stopped, or this file is untracked.

## Takeover update — TR2.8 implementation completed

The operator asked to finish the stopped TR2.8 lane. Its implementation is now
committed in `/Volumes/projects/limbo/gwz-dev-tr2-8`: root
`497a9fec2671e1011655e4c27a979ae8dde10c69`, core
`da8994d1ef29f95498657f1fbe0104af0a9946f5`. The latter is a docs-only closing
commit over the source reviewed at `c51b1b5ff75a772f42847ce22fa4f35a61cf4497`.

Both Code and State reviewers reported GO after one bounded remediation round.
Two distinct P2 findings (malformed RSA fallback and native CR/LF parsing) and
one P3 fixture-cleanup finding were closed. Their complete reports and the
validation matrix are in the lane's
[`GwzTransportSshKeyTypes-Implementation.md`](../../gwz-dev-tr2-8/gwz-core/dev-docs/GwzTransportSshKeyTypes-Implementation.md).

The corrected transport-only SSH suite passed 153 tests; the full suite with
both candidate switches passed without failures. The original ordinary and
full transport-only suites also passed. Source guards, formatting and the
per-commit boundary checks pass. Strict Clippy remains red on inherited
diagnostics outside this lane's changed files; it is recorded debt, not a
waiver. Gate logs are in `logs/tr2-8-final/` in the external handoff directory.

This accepts implementation only. Hardware-key manual execution still needs
the operator's go; Windows agent primitives and release-platform qualification
remain in their planned batch. Nothing from this lane has been merged, pushed
or tagged, and it has not been disposed of. The stopped/partial TR2.8 entries
in the snapshot below are historical; this update supersedes them.

---

This is a snapshot taken at about 20:45 local time on 2026-10-02, for the agent taking over the transport release from the session that wrote it.

- **Live state** is the top entry of [`CurrentProgramCheckpoint.md`](CurrentProgramCheckpoint.md). It wins wherever it disagrees with this file.
- **The plan:** [`GwzTransportReleasePlan.md`](../gwz-core/dev-docs/GwzTransportReleasePlan.md), with [amendment 1](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md) and [amendment 2](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2.md). Amendment 2 is at revision 6; its §3.13 says what can start, and §3.19 and §3.20 hold the 2026-10-02 decisions.
- **The release split** (OD13): 1.1.0 is the transport in process in the CLI and gwz-py, on macOS ARM64, Linux x86-64 and Windows x86-64, with Windows parity. 1.2.0 is the session host, reuse and the server.

**This file is uncommitted.** It is an untracked file in the root repository, so it blocks `gwz merge` (`DirtyMember`) until it is committed or parked like the drafts (§4).

## 1. Background work still owned by the old session

Four subagents were running when this was written:

| Lane or file | Work |
|---|---|
| `gwz-dev-tr2-1` | TR2.1's last fixes |
| `gwz-dev-tr2-8` | TR2.8 |
| `gwz-dev-tr2-4-7` | TR2.4, then TR2.7 |
| The session scratchpad | The connection-statistics design draft (TR2.24); finished, see §5.4 |

**Update:** the TR2.1, TR2.4/TR2.7 and statistics agents have since finished (§3.1, §3.3, §5.4). TR2.8's agent was stopped when the session closed. At that point the `tr2-8` lane had 1 gwz-core commit(s) past `bb67a82`, and these uncommitted gwz-core paths: ` M dev-docs/GwzTransportSshKeyTypes.md; M src/git/endpoint/agent_auth.rs; M src/git/endpoint/agent_client.rs; A src/git/endpoint/agent_keys.rs; M src/git/endpoint/mod.rs; M src/git/endpoint/ssh_fixture.rs; M src/git/endpoint/ssh_key_container.rs; M src/git/endpoint/ssh_password_fixture.rs;`. Review that partial work before restarting TR2.8 from §3.3's brief. They report only to the old session and may stop when it ends. Before you take a lane over:
- check that nothing is still writing to it, for example with `find <lane> -newer <file> -mmin -10 -not -path '*/target/*'`;
- or ask the operator whether the old session is stopped.

## 2. The tuple

### 2.1 Main: `/Volumes/projects/limbo/gwz-dev`

The workspace moved to an external APFS disk; `~/limbo` is a symlink to `/Volumes/projects/limbo`. Old `~/limbo/...` paths resolve, so use the `/Volumes` paths.

| Repo | HEAD | Branch |
|---|---|---|
| root | `7e85b6f84e7a` | main |
| gwz-core | `bb67a8264a71a5141d3345a5d1367f4228aeb5db` | main |
| gwz-cli | `236f7530e135` | main |
| gwz-py | `43a07a22687d` | main |
| gwz-transport | `6910ba669ccc` | main |
| taut | `a7cab03ea487` | main |
| taut-shape | `5779c9676d29` | main |
| taut-shape-rs | `502671593f45` | main |
| taut-shape-py | `f86f0e9fa793` | main |
| gwz-core-evidence | `90740fe07b12` | main |
| gwz-git | `a9d7ee09cce6` | main |
| libgit2 | `b172e3d187a4` | `codex/local-fetch-noncommit-1.9.7` |
| git2-rs | `d13951f7e0bf` | `codex/per-remote-transport` |

**Dirty state.** No tracked file is modified. These files are untracked, and none of them is this session's to commit:
- root `dev-docs/GwzRemoteTransportSshN2b-PromptCode.md`, `-PromptCode-1.md`, `-PromptState.md` and `-PromptState-1.md`, and `dev-docs/GwzWorkspaceRouteMappingDesign.md`: other lanes' drafts;
- `gwz-core/dev-docs/GwzRemoteTransportBugReport.md`: a working draft. It names a private repository path and the machine, so **never commit it to public gwz-core as is**;
- 27 run files under `gwz-core-evidence/campaigns/transport-qualification/runs/2026-09-22-alpha-setup-timeout/`. **Never touch gwz-core-evidence.**
- this file.

**Pushed.** The last push was on 2026-10-02 (checkpoint, "Pushed on the operator's go"): root `a9479d8`, gwz-core `9d4dd92f`, gwz-cli `0218cc7`, gwz-py `766de53` and gwz-transport `6910ba6`.
- Main has unpushed commits since; check each repository with `git rev-list --count origin/main..main`.
- A push needs the operator's explicit go each time.

**Last commits on main** (the newest records):
- root `7e85b6f` and gwz-core `bb67a82`: the operator's answers on TR1.6, the `--ssh-timeout` help answer, and the lane-disposal answer;
- root `acc48a7` and gwz-core `e51d44b`: TR1.6 accepted.

### 2.2 Lanes

No lane from this round has been merged or disposed. The five earlier lanes (`split-pe`, `ci-py`, `tr2-19`, `tr2-20` and `tr2-18`) were disposed on the operator's go, after each lane's heads were proven in main (checkpoint, line 272).

The operator approved, on 2026-10-02, disposing of `tr2-1` and `tr2-5a` once each merge is verified. That approval does not cover `tr2-8` or `tr2-4-7`.

| Lane | Path | State at handoff |
|---|---|---|
| `tr2-1` | `/Volumes/projects/limbo/gwz-dev-tr2-1` | TR2.1, retry Phase 3 with OD18. All five follow-ups are committed and every gate is green at the final tree; ready to merge (§3.1) |
| `tr2-5a` | `/Volumes/projects/limbo/gwz-dev-tr2-5a` | TR2.5 step 1, done and gated. No agent. Merge after TR2.1 (§3.2) |
| `tr2-8` | `/Volumes/projects/limbo/gwz-dev-tr2-8` | TR2.8, just started; no commits at handoff (§3.3) |
| `tr2-4-7` | `/Volumes/projects/limbo/gwz-dev-tr2-4-7` | TR2.4 and TR2.7 done and gated, after this file was first written. Two TR2.4 decisions await the operator (§3.3) |

## 3. Lane details

### 3.1 `tr2-1` (TR2.1)

**Update: all five follow-ups are committed, after this file was first written.** The tables below show the earlier state.

| Fix | gwz-core | gwz-cli | root |
|---|---|---|---|
| 1. The help | — | `5f2c159` | `3c9858e` |
| 2. Refuse `--max-retries` on the Cli placement (P3-1) | `6ac41f6` | — | `73615b4` |
| 3. Refuse values above `u32::MAX` (P3-2) | `2758db3` | `0164e66` | `8c511b2` |
| 4. Refuse `--max-retries` on merge (P3-3) | `828093f` | — | `e6df053` |
| 5. The backstop counts each attempt's 5 s cleanup | `7bb2fb0` | — | `85c9778` |

- **Fix 4, for the operator:** the refusal uses `MergeValidationFailed`, which is merge's code for every policy field it refuses, `--jobs` and `--max-per-host` included. The State review had asked for `InvalidRequest`. Merge as is unless the operator prefers `InvalidRequest`.
- **Fix 5, the backstop:** the risk was real. A failed setup is disposed before it is reported, and a timed-out setup waits up to 5 s for its thread. Each attempt's backstop now includes the 5 s, so the default goes from 159 s to 164 s per attempt.
- **The switch inventory** now lists 20 sites, since fix 4 adds two.
- **The help strings,** before and after, for the retry plan's §8 erratum. The full text is in `reports/TR2.1-final-report.md` in the handoff folder, and in `git -C gwz-cli show 5f2c159`. The changes:
  - "setup" becomes "SSH setup";
  - "SSH and HTTPS use this same stall clock and the same 30 second setup budget." becomes "HTTPS setup has no stall clock, only the 30 second setup budget that SSH setup also has.";
  - the no-progress bound adds "an HTTPS setup that makes no progress after 4 times 30 seconds plus the same waits, about 128 seconds";
  - `--max-retries` says "--ssh-timeout sets only the stall of an SSH setup attempt and of a body read."
- **`docs/CLI.md`:** its 43 changed hunks are all this flag's long help.
- **Gates on the final tree:**
  - gwz-core candidate: 2,755 passed on each switch set. The transport leg's first run missed one HTTPS throughput timing bound at load average 69–118, and the rerun passed.
  - gwz-core ordinary: 2,326 passed.
  - gwz-cli: 253 + 92 ordinary and 254 + 92 candidate.
  - gwz-py candidate: 1,000 passed.
  - consumer: 27 passed, and compat 11 of 11.
  - the checkers pass.
  - clippy is as before.
  - rustfmt: gwz-core is clean. gwz-cli fails only on `src/tests/g02/partial_errors.rs`, unchanged from main's `236f753`; that is a pre-existing issue for main.
  - the lane gate from `ab48966f`: 22 of 23 commits are ok, and the red one is main's `341a669`, the accepted deviation.
- **Left alone, for the record:**
  - on an HTTPS GET, the same allowance also times the wait for response headers;
  - the ordinary build's help still mentions retries and `--max-retries`, which belongs to TR2.5.
- **Builds:** about 3.5 GB, kept in the lane: `candidate-target`, `candidate-target-both`, `candidate-target-py`, `target` and `gwz-core/target`. gwz-cli's candidate tests need a `gwz-core` link beside their prepared manifest; the agent put one in its scratchpad.

**Heads at handoff.**

| Repo | HEAD | Dirty |
|---|---|---|
| root | `3c9858e9581b` | clean |
| gwz-core | `7e28261890ac` | `src/transport_host/retry_tests.rs`, +30 lines, uncommitted |
| gwz-cli | `5f2c159137cb` | clean |
| gwz-py | `b2369f1d0bf7` | clean |

**The CLI edits you asked about** (`parser.rs`, `retry.rs` and `g09.rs`) are now committed, as gwz-cli `5f2c159` with root `3c9858e`.
- They fix the help of `--ssh-timeout` and `--max-retries`, the operator's answer "Fix the help text": the stall clock is SSH's, and HTTPS setup has only the 30-second aggregate budget.
- The same commit regenerated `gwz-cli/docs/CLI.md` (+658/−608). That file was already stale before TR2.1. Check that its diff is regeneration only.
- Amendment 2's next revision must record the change as an erratum to the retry plan's §8, with the exact before and after strings (`git -C gwz-cli show 5f2c159`).

**The one uncommitted edit** is gwz-core's `retry_tests.rs`. It is the agent's test-first work on the next follow-up, most likely State P3-1 or P3-2 below. Finish it, with its fix and a commit, or discard it.

**The lane's commits.**
- **gwz-core:**
  - `d14c05f8`, S3.2;
  - `9a235acb`, S3.4's wire field `OperationPolicy.max_retries`, tag 9, `MISSING_OK`, candidate protocol only;
  - `766b2953`, S3.3;
  - `0fb61c5e`;
  - `f5f0a1ff`, S3.5;
  - `7107223d`, the merge of main;
  - `eea6bb4`;
  - `1afea66`, SSH retry machines keyed by pool key and the Open's identity. HTTPS stays keyed by pool key.
  - `7e28261`, the candidate overlay test made exact, with `OperationPolicy` compat rows.
- **gwz-cli:** `44e053a` (`--max-retries` and the final failure's display suffix), `f00c9ed` (merge) and `5f2c159`.
- **gwz-py:** `e5ba3d3`, `d98d005` and `b2369f1` (merge).
- **root:** ends with `d1b60bd`, `1578cc9` and `3c9858e`.

**How main was merged into the lane.**
- The merge was targeted: `@root`, gwz-core, gwz-cli and gwz-py. That kept gwz-core-evidence out.
- The six drafts were parked and restored, and all six hashes matched.
- **Conflicts and their resolution:**
  - **`https_worker/prepare.rs`:** `prepare_attempt` stays the transport host's entry. It calls a private `run_attempt` that holds TR2.20's body unchanged. No `allow_transition` and no `prepare_budget_inner` remain.
  - **`https_worker.rs`, three hunks:**
    - the test-only block's doc comment merges both wordings;
    - the test-only `prepare_budget_for_transition`, `prepare_attempt` without its connect report, is kept, so that TR2.20's 44 call sites compile;
    - `prepare` and `prepare_budget` stay removed.
- **Two silent semantic breaks** came from TR2.18's interfaces:
  - `placement_endpoint/retry_tests.rs`: `start_reported` takes an `Opening`;
  - `ssh_tests/retry.rs`: `connect_with_authority` takes a sixth argument, the handoff.

  `merge --continue` refuses changes outside the conflict paths (L8), so the fixes are their own commit, `eea6bb4`. **`7107223d` alone does not compile its candidate tests.**

**Gates at `7e28261`** (the agent's report; logs in §5.4):
- gwz-core candidate: 2,751 passed, 7 ignored, on each switch set;
- gwz-core ordinary: 2,326 passed;
- ordinary clippy `-D warnings`: clean. Candidate clippy: 85 warnings, none on a line the lane adds;
- gwz-cli: 253 + 92 ordinary, 254 + 92 candidate;
- gwz-py `--candidate`: 1,000 passed;
- consumer: Python 27 passed, regenerator check passes, Rust compat 11 of 11;
- the boundary, switch-inventory (18 sites), process-globals and cfg-boundary checks, and rustfmt, all pass;
- the lane gate from `ab48966f`: green except `341a669`, main's TR2.18 erratum, an accepted deviation.

**Reviews of `9a235acb`** (wire format, so dual): Code GO and State GO. The reports are uncommitted; copies are in §5.4. File them verbatim as `gwz-core/dev-docs/GwzTransportRetryWireField-ReviewCode.md` and `-ReviewState.md` with the merge record.
- **Code's P3** (the exact overlay test) is closed by `7e28261`.
- **State's P3s are being fixed.**

**Follow-ups sent to the agent, with their status:**

| # | Follow-up | Status |
|---|---|---|
| 1 | The help text | Done |
| 2 | State P3-1: `open_request` refuses a Cli-placed request whose `policy.max_retries` is `Some(_)` (`unsupported`, naming the placement); closure test: a Cli pair with `Some(0)` refused at `request()` | Not committed |
| 3 | State P3-2: refuse values above `u32::MAX` in `validate_meta` and in gwz-cli's parser | Not committed |
| 4 | State P3-3: a candidate `cfg_if` arm in merge's `validate_common_meta` refusing `policy.max_retries (--max-retries)`, with its switch-inventory line | Not committed |
| 5 | State's risk 1: whether an attempt can delay its failure past `open_backstop_ms`, which omits §5's 5-second cleanup allowance; if it can, add the allowance per attempt, test-first | Not committed |

Then rerun every gate and the lane gate before merging.

**Not changed, recorded as open:**
- `parse_non_negative_i64`'s message;
- gwz-cli has no candidate CI leg;
- the backstop excludes the allocation wait;
- 1.2.0 plumbing: the Cli placement carries no `max_retries`, `concurrency` or `max_connections_per_host` to an endpoint in another process.

### 3.2 `tr2-5a` (TR2.5 step 1, the transport setting module)

**Commits:** gwz-core `7392b3be` (the paired `GWZDesign.md` and `GWZRequirements.md` paragraphs) and `ec1eeee3` (the module and its 44 tests); root `706d279e` and `05f6a438`.

**The API:** `gwz_core::transport_setting::{resolve, ignored_values, path_text}`, with `Transport`, `Source`, `Setting`, `Refusal`, `Driver`, `Scope` and `IgnoredValue`. It renders §10's refusals of [TR1.5](../gwz-core/dev-docs/GwzTransportOffSwitchDesign.md) word for word.

**Gates:** candidate 2,729 passed; ordinary 2,320 passed; every source check passes; the lane gate is green from `ee06d16` and `fb7a992`; the switch inventory lists 19 sites.

**The agent's interpretations.** Accept them, or raise them in TR2.6:
1. The gate is `all(unix, gwz_transport_candidate)`: Windows opens at S4.5.
2. The snapshot is `session_host::EnvironmentSnapshot`.
3. A global file that is not a regular file is treated as unreadable: skipped and reported.
4. An unreadable file that a global file includes, or a `~/` include with no home, gives the "could not parse" refusal, as git also fails. D4's skip covers only the global files themselves.
5. No command is printed for a non-UTF-8 or relative path.
6. Scan targets come from `resolve_action_targets`. Pull's snapshot uses materialize's targets; a tag-narrowed materialize reads the whole selection; a symlinked `.git` gets no note.
7. A residual race remains between `symlink_metadata` and libgit2's open.
8. The lane gate was run from both bases.
9. The module is 729 production lines, against §9's estimate of about 230.

### 3.3 `tr2-8` and `tr2-4-7`

**Update: `tr2-4-7` finished after this file was first written.** Nothing is merged, pushed or published.
- **TR2.4:** gwz-transport `35475977`, root `54b8d281`.
  - `src/sequenced.rs` opens with `#![cfg(feature = "unstable-sequenced")]`.
  - `binding.rs` has two braced `mod profile { }` blocks, one under the feature and one under its negation.
  - Without the feature, a Bind that offers only `[3]` is refused `UnsupportedVersion`, with `Effect::None`.
  - CI runs the suite again with the feature.
  - Test: `profile_three_binds_only_with_the_unstable_sequenced_feature`.
- **Two TR2.4 decisions for the operator, before merging:**
  1. **No `cfg_if`.** gwz-transport has no dependencies by charter (crate map §8), so the agent used enclosing modules instead, as `session-host` does. Switching to `cfg_if` costs about 10 lines, the dependency, and a change to the consumer proof's lock.
  2. **Mixed offers.** An offer of `[1, 2, 3]` binds at 2 rather than being refused, which is how the agent read the sequenced design's §1. Refusing every offer that names 3 is a small change.
- **Correction to the brief.** gwz-transport 0.1.0 has never been published. crates.io lists only `0.0.0-bootstrap.1`, the manifest says `publish = false`, and the consumer proof's `=0.1.0` resolves to a local archive. So no version bump is needed, and the first real publish carries the feature gate.
- **TR2.7:** gwz-core `49d6bc4f`, root `882feb0f`.
  - New `src/git/endpoint/ca_bundle.rs` parses every `CERTIFICATE` block, in file order, the same way on every platform.
  - A block that is malformed, a block that is not base64, and a file with no certificate are all refused as `InvalidRequest`, before any endpoint exists. The 1 MiB bound stays.
  - The roots add to the platform's.
  - Tests are in `src/transport_host/ca_bundle_tests.rs`. Both of the plan's rows failed before the change on macOS and pass after.
  - The Linux-only roots test is inside `cfg_if!`. It did not run here; it runs in CI's Linux candidate job.
- **TR2.7 points to confirm:**
  - two new messages: "endpoint CA file has a malformed certificate block" and "endpoint CA file has no certificate";
  - `TRUSTED CERTIFICATE` blocks do not count;
  - the step is 368 lines, about 75 of them production code, against "under 200";
  - `gwz-core/dev-docs/GwzRemoteTransportAlpha.md:17`, which says "single additional PEM root", needs an erratum.
- **Gates.**
  - gwz-transport: 149 passed without the feature and 167 with it.
  - gwz-core: 2,326 ordinary and 2,694 candidate passed.
  - `cargo check --tests` is clean for both switches.
  - The consumer proof passed 31.
  - The checkers pass, and the switch inventory still lists 18 sites.
  - The lane gate from `bb67a82` is ok.
  - Logs: `logs/tr2-4-7/` in the handoff folder (§5.4).
- **The lane root holds untracked build directories:** `candidate-target`, `candidate-target-check` and `transport-package`.
- **A privacy slip, reported by the agent.** A read-only crates.io query sent the operator's email address in its User-Agent header.

Both were cloned from main at the tuple in §2.1.

**TR2.8,** keys and signatures as 1.0.17 uses them. The controlling text is amendment 2 §3.19, lines 477–497.
- First, a dev-doc listing every agent key type and signature that libssh2 1.11.1 used in 1.0.17, per platform. WinCNG lacks Ed25519, and lacks ECDSA without `LIBSSH2_ECDSA_WINCNG`.
- Then the transport signs with all of them: Ed25519, RSA with `rsa-sha2-*`, ECDSA P-256, P-384 and P-521, `sk-` security keys, certificates, and DSA if libssh2 offered it.
- `ssh-rsa` (SHA-1) is used in exactly libssh2's three cases, as the operator confirmed.
- A key of a type the transport cannot use is skipped, and a later key is tried. The route check goes.
- Tests run against the fixture and a disposable `sshd`: a software security-key authenticator, a user-CA certificate, and one row for each SHA-1 case.
- **Review:** dual Code and State, since it handles signing. **The manual hardware-key row needs the operator's go.**

**TR2.4**, in gwz-transport: `sequenced` moves behind the non-default feature `unstable-sequenced`, in a `cfg_if` boundary. A version-3 Bind is refused without the feature.
- gwz-transport is published at 0.1.0, and the consumer proof pins `=0.1.0`. The agent will assess whether gating the module needs a version bump. Don't bump or publish without the operator.

**TR2.7**, in gwz-core: every certificate in `GIT_SSL_CAINFO` or `SSL_CERT_FILE` becomes a root.
- A malformed block, or a file with no certificate, is refused before any connection.
- The file's roots are added to the platform's.
- The test that pins this on OpenSSL is Linux-only, inside a `cfg_if`.

**Instructions for both lanes:**
- leave the five split-pending files alone (§6.1);
- keep builds in the lane;
- run the lane gate from `bb67a8264a71a5141d3345a5d1367f4228aeb5db`.

## 4. Merging a lane

1. **Check main** with `~/.cargo/bin/gwz status`. Any untracked file in a selected repository blocks the merge (`DirtyMember`). By the operator's rule, park the drafts:
   - move them, never delete them, to a dated folder outside the workspace, recording their SHA-256s first;
   - restore them after the merge, or after `--abort`, and verify the hashes;
   - **never touch gwz-core-evidence**;
   - **a dirty tracked file is not a draft:** ask the operator first.

   The drafts are the six files in §2.1, plus this handoff if it is still uncommitted.
2. **Run a targeted merge,** so that gwz-core-evidence stays out:
   `~/.cargo/bin/gwz --root /Volumes/projects/limbo/gwz-dev --target @root --target gwz-core --target gwz-cli --target gwz-py merge --remote tr2-1`.
   Add `--target gwz-transport` for `tr2-4-7`. Merge lanes one at a time, `tr2-1` first, then `tr2-5a`.
3. **Known gwz defects** (`dev-docs/GwzLaneIssues.md`, L7–L9):
   - **L7:** a merge that exposes an ignored directory goes to `recovery-required`. Move the directory aside, then run `gwz merge --continue`.
   - **L8:** `--continue` refuses changes outside the conflict paths. Put such fixes in a follow-up commit.
   - **L9:** `gwz add` needs `--target` during a merge.
4. **Verify both sides with patch-ids.** For each repository, `git diff <base> <side> | git patch-id --stable` must equal `git diff <other side> <merge> | git patch-id --stable`.
5. **Run the gates on merged main** (§5). For the full suite, `cargo metadata` must resolve the copied lock before `--locked`.
6. **Record the merge in the checkpoint,** and file the lane's reviews. Commit with `gwz add <paths>` and `gwz commit --target ...`. The operator's "go = commit" (2026-10-02) has covered record commits and lane merges.
7. **Dispose of the lane** from main with `gwz local dispose <lane>`, but only after the merge is verified.
   - A verbatim lane may refuse with `UnwaivedHazard`.
   - Before you waive anything, prove nothing in the lane is unique. For each repository, the commits from `rev-list --all`, `reflog --all` and `stash list` in the lane, minus those in main, must come out empty.
   - Only then run `--force dirty,unpreserved-history`.

## 5. Reproduction

### 5.1 Tools
- **gwz:** `~/.cargo/bin/gwz`, version 1.0.17, the installed release. Never the workspace's own build.
- **Python:** `python3.13` for every script; gwz-core's runner needs 3.11 or newer.
- **Node:** pnpm, never npm.
- **Raw git** is for read-only inspection. Commit and push through gwz.

### 5.2 Commands
Run these from the workspace or lane root unless noted. Set `export CARGO_INCREMENTAL=0 CARGO_PROFILE_DEV_DEBUG=0 CARGO_PROFILE_TEST_DEBUG=0` first.

- **gwz-core ordinary:** `cd gwz-core && python3.13 scripts/run_tests.py`.
- **gwz-core candidate.**
  - `prepare.py` refuses a destination inside the workspace, so put `<dir>` outside it.
  1. `python3.13 tests/transport_backend/prepare.py <dir>`
  2. `cargo metadata --format-version 1 --manifest-path <dir>/Cargo.toml >/dev/null`
  3. `RUSTFLAGS="--cfg gwz_transport_candidate" python3.13 scripts/run_tests.py --no-fail-fast --manifest-path=<dir>/Cargo.toml --target-dir=<lane>/candidate-target`
  4. The second leg, `RUSTFLAGS="--cfg gwz_transport_candidate --cfg gwz_session_candidate"`, at least as `cargo check --tests`.
- **gwz-core checks** (`gwz-core/scripts/checks/`):
  - `check_checked_artifact_boundaries.py`;
  - `check_candidate_switches.py --repo gwz-core`, run from the root;
  - `check_process_globals.py`;
  - `check_cfg_boundaries.py`;
  - `check_filesystem_boundary.py`;
  - `check_target_selection_boundary.py`;
  - `cargo fmt --check`;
  - clippy, as CI runs it.
- **The per-commit lane gate**, which CI's "Checked-artifact boundary" job runs: `cd gwz-core && PYTHON=python3.13 bash scripts/checks/check_lane_commits.sh <base> HEAD`. Run it before every gwz-core push too, with `origin/main` as the base.
- **gwz-cli:** its tests in both builds, the second with `RUSTFLAGS="--cfg gwz_transport_candidate"`, plus its switch-inventory and process-globals checks.
- **gwz-py:** `python3.13 run_tests.py --candidate`.
- **The consumer proof:** `gwz-core/tests/transport_consumer`, with its Python tests, the regenerator's `--check` and the archive proof.

### 5.3 Build directories
- Each lane builds in its own `target` and `candidate-target` directories, and the external disk has about 864 GiB free.
- A lane clone copies main's build outputs with copy-on-write. To warm a lane's candidate build, `cp -cR` an idle candidate target, never one that a build is writing.
- The old scratch builds lived under `/private/tmp/...`, on the internal disk. Don't build there.

### 5.4 Logs and in-flight documents
These are copied to `/Volumes/projects/limbo/gwz-handoff-2026-10-02/`, because the session scratchpads under `/private/tmp` are temporary:
- `logs/tr2-1/`: TR2.1's gate logs, 34 files. Among them are `final-cand.log`, `merged-cand.log`, `merged-both.log`, `cli-cand-final.log` and `cli-ordinary-final.log`, and the commit-message drafts;
- `logs/tr2-5a/`: TR2.5 step 1's logs, 10 files: the stub run, the mutants, both suites and clippy;
- `reviews/`: the two wire-field reviews, verbatim and uncommitted;
- `reports/`: the implementers' final reports, verbatim. They are TR2.1, TR2.4 and TR2.7, TR2.5 step 1, and TR2.24's revision 1. They give the full detail behind §3;
- `logs/tr2-1-final/` and `logs/tr2-4-7/`: the later gate logs;
- `designs/`: the connection-statistics design. `GwzTransportConnectionStatsDesign-rev0.md` (`b9817f17…`) is revision 0. `-rev1.md` (`67175c3a…`, 193 lines) is revision 1, finished after this file was first written: TR1.6 cited as accepted, TR2.22's wire detail read rather than duplicated, and one retry entry per machine, since TR2.1's `1afea66` keys SSH machines by identity too. It is not yet reviewed. Its open judgement calls: the quiet-skip classes are snake case, and under `--verbose` setup causes appear in public fields for the first time.

## 6. Remaining work

### 6.1 Phase 2 (1.1.0), in order
1. **TR2.1:** the follow-ups are done (§3.1). Merge with the drafts parked, verify by patch-id, run merged main's gates, file the two reviews, record, then dispose of the lane.
2. **TR2.5 step 1:** merge `tr2-5a`, then dispose of it.
3. **The five-file split.** The operator chose "Split all five". It is movement only, in one lane, after TR2.1 merges:
   - `checked_artifact/entry.rs` (1,080 lines);
   - `gitbackend/fake_repository.rs` (1,079);
   - `gitbackend/contract.rs` (1,067);
   - `transport_host/session/driver.rs` (813 in TR2.1);
   - `https_worker_tests.rs` (959).

   Prove each moved item byte-identical, as the earlier split did with `rust-split`'s `explode` and an item checker. Approve any `#[path]` edge in the same commit.
4. **The rest of TR2.5,** after TR2.1. TR1.5 was accepted, and the operator answered all its questions as recommended; D2, D3 and E3 are kept.
   - **Step 2, gwz-cli:**
     - `--transport <gwz|native>`, `GWZ_TRANSPORT` and the `gwz.transport` setting, through `transport_setting::resolve`;
     - the ignored-value notes and the `--verbose` line;
     - JSON `meta.transport_setting`;
     - with native, 1.0.17's defaults: 50 jobs, 8 per host, and a 3 s `--ssh-timeout`.
   - **Step 3, gwz-py:**
     - the same resolution;
     - the note as a `logging` record, never a warning a filter can turn into a refusal;
     - `Client(max_connections_per_host=None)`;
     - one 9 s clock.
   - **The retry row** comes after TR2.1.
   - Every site is gated by `gwz_transport_candidate` and listed in its repository's inventory. The design is [TR1.5](../gwz-core/dev-docs/GwzTransportOffSwitchDesign.md), §7–§10.
5. **TR2.2, then TR2.22: credential helpers.**
   - **TR2.2** reproduces defect 1 first, on the HTTPS fixture with a fake `gh`, named by absolute path and through `PATH`. Its regression tests stay. Under TR1.6's C7, either TR2.2 puts its fix outside the spawn, or TR2.22 keeps only the fixes that TR1.6's spawn does not replace.
   - **TR2.22** implements [TR1.6](../gwz-core/dev-docs/GwzTransportCredentialHelpersDesign.md) (accepted) with the operator's answers: OQ1 (b), OQ2 (a), OQ3 (b), OQ4 (a), OQ5 (a), giving `credential_helper_timeout` = 75, OQ6 (a) and OQ7 (1).
   - **The pending wire change, OQ7 (1) plus the operator's addition.** gwz-transport's wire `Failure` (`src/protocol.rs:603-608`) gains an optional detail field.
     - **It carries:**
       - one cause from M8's fixed set;
       - M6's scheme tokens, at most four, each an HTTP token cut at 32 characters;
       - TR2.1's attempt number, so that `attempt N of M` comes from the endpoint instead of the driver's own count, `spent_budget`.
     - **It never carries** helper output, stderr or a URL.
     - **Review:** it takes a per-step dual Code and State review, as a wire change.
     - **Naming and release:** S7.1 (1.1.0) names it. It ships in gwz-transport's first real release; 0.1.0 has never been published (§3.3). The operator decides the release.
     - **Who does it:** TR2.22 owns it. TR2.1's display then reads the wire attempt.
6. **TR2.23,** new and not yet in the plan. A server that offers only SSH `password` runs the configured credential helpers, as 1.0.17 does, through TR1.6's lookup. TR2.18 found the gap. Amendment 2's next revision adds the step.
7. **TR2.8** is in progress. **TR2.4 and TR2.7** are done in `tr2-4-7`. Merge them once the operator answers TR2.4's two decisions (§3.3).
8. **TR2.24, connection statistics,** for the operator's request on 2026-10-02.
   - **The draft recommends:**
     - `--verbose` with `--json` or `--jsonl` adds `meta.transport_diagnostics` (hosts, connections, retries, quiet skips) and a `stats` object on each `meta.transport` row;
     - with no new flag, output without `--verbose` stays byte-identical;
     - the record goes on the final response only;
     - the endpoint's figures go into a ledger the runtime owns, with no gwz-transport schema change;
     - it lands in 1.1.0 Phase 2, after TR2.22.
   - **Next:**
     1. revision 1 is done (§5.4);
     2. run Consistency, Safety and Surface reviews, since it freezes machine output;
     3. take its OQ1–OQ6 to the operator.
9. **TR2.6: the Phase 2 implementation review,** dual Code and State, on the settled tree after all of the above. Its scope is amendment 2 line 243 and §3.20.

### 6.2 Windows (1.1.0, OD15)
None of it has started, and it needs dabeest, the operator's Windows machine.
- **S4.1,** then **TR1.8**, the design. It covers, in the transport:
  - Pageant's window protocol, with a visible Pageant first, as on 1.0.17;
  - the WinHTTP machine proxy, including the 407 case;
  - SSPI for Negotiate, NTLM and Digest;
  - libgit2's home order: `HOME`, then `HOMEDRIVE`+`HOMEPATH`, then `USERPROFILE`.
- **OD16 is lifted:** the logon session's default credentials go to any host, as on 1.0.17, and the notes state the hazard.
- **Precedence:**
  - a configured helper answers first when the challenge offers NTLM, Basic or Digest;
  - a Negotiate-only challenge takes the logon session, and no helper is asked.
- **Then:** S4.2–S4.4, TR4.8–TR4.10 after TR1.8's GO (TR4.10 has its own dual review), TR4.6 (Windows CI on push), TR4.7 (the Windows review) and TR8.4 (parity rows).
- **The Windows candidate build is blocked until S4.4, TR4.8 and TR4.9 write the Windows arm.** `endpoint_environment.rs` has a deliberate `compile_error!` arm, and TR2.5 step 1 is unix-only until S4.5.

### 6.3 Other 1.1.0 work
- S2.1–S2.3.
- TR3.3, the crates.io names, then TR3.2.
- S3.1–S3.3.
- **S7.1** names the new wire and protocol fields: `OperationPolicy.max_retries`, the `Failure` detail, and TR2.24's fields.
- **S7.3** gains a consumer-build row with `GWZ_TRANSPORT=native` (TR1.5's OQ4).
- **S7.5** is the Surface review of the `--verbose` rows and TR2.3's `errors`.
- **TR8.1:** performance qualification against 1.0.17.

### 6.4 Documents owed
- **Amendment 2, revision 7.** Skim-reviewed, as revisions 3–6 were. It records:
  - TR1.6's amended texts (§9, C3), with status lines on each of those documents, and C2 (a "validated discovery redirect" is the whole redirect sequence) and C7;
  - TR2.23;
  - TR2.22's wire detail and attempt number;
  - the retry plan's §8 erratum for the help text (§3.1);
  - TR1.5's S7.3 row;
  - TR2.24, once it is accepted, with the retry plan's §5 line 283 exception for `--verbose`;
  - the 1.2.0 Cli-placement plumbing gap (§3.1).
- **The session plan's next amendment:**
  - TR2.19's stale text: CS2.5, D10 and the session design's §5.4;
  - the CS3.10, CS8.3, C1, CS8.28 and CS4.5 re-checks;
  - the 1.2.0 native-default fill under CS3.10, CS4.7 and CS6.4;
  - the edge CS6.6 ── CS6.4, also in the Phase 6 sketch;
  - CS3.4 naming `-c core.askPass=` (TR1.6 C1);
  - the Cli placement carrying `max_retries` and the other per-request limits.
- **The server design's next revision:**
  - TR1.5's four OQ3 items;
  - TR2.18's URL password across processes;
  - the choice for the `auto` key.
- **The reuse design** (before 1.2.0's Phase 6):
  - presence-key revalidation;
  - TR1.6's C4 (pooling under a credential's scope);
  - C9 (slot waits charged to the allocation deadline, per OQ6 (a)).

## 7. Rules the operator has set

- **Git.** Do exactly the operation asked.
  - Never commit, push or tag without the operator's go. The 2026-10-02 "go = commit" has covered record commits and lane merges. A push needs a fresh go.
  - **Never create, move or delete a tag.**
  - **No AI attribution** in a commit or PR. `~/.claude/settings.json` sets `attribution` empty.
  - **Use gwz:** `gwz add <paths>` (never `-A`), `gwz commit --target ...`, and `gwz push --target ...` with `--dry-run` first.
  - **If gwz fails,** retry once, then report.
  - **After a push,** read every CI run it starts once. Never poll.
- **Pushing.** Use the operator's owebeeone SSH agent identity, as the operator's private notes describe. Verify with `ssh -T git@github.com`.
- **Never touch** git2-rs, gwz-git or gwz-core-evidence.
- **Public documents** must not name the default SSH agent's account or any agent socket path.
- **Code:**
  - brace every control-flow body;
  - put cfg only inside `cfg_if` or enclosing modules, never on a single import;
  - no globals, and thread-locals count;
  - remove dead code now;
  - test first;
  - nothing that is production code lives under `tests/`;
  - new files stay under 500 lines;
  - surface structural debt to the operator.
- **Native routes are not a way to cover a setup.** Build each behaviour into the transport and state the cost. Only the off switch, TR2.11's callers without a host context, and `git://`, `http://` and `file://` remotes stay native. Parity with 1.0.17 wins when asked.
- **Reviews.**
  - **Per phase:** one Consistency and Safety review per phase, plus Surface when a phase freezes a user-facing surface.
  - **Per step:** a dual review only for a wire format, secret handling and the release gate.
  - **Skim review:** one replaces the dual loop when the operator says so.
  - **Filing:** file reports verbatim.
  - **Remediation:** at most two rounds.
- **Models:** Fable reviews; Opus implements, so pin `model: opus` on implementers.
- **Measuring progress:** from git, never from plan budgets.
