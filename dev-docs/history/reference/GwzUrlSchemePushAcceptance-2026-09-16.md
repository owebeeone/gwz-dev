# GWZ push: acceptance evidence against GitHub

Date: 2026-09-16. Plan: `GwzUrlSchemePushPlan.md`, step 4.2. Runbook:
`GwzUrlSchemePushAcceptanceRunbook.md` (draft of 2026-09-16). Evidence:
`~/limbo/accept-2026-09-16/evidence/` on the Mac and
`~/limbo/accept-2026-09-16-pi/evidence/` on the Pi (both kept locally).

Status: **accepted for cases 1 to 5 on macOS and case 4 on Linux/aarch64, at
gwz 1.0.13 (gwz-cli `v1.0.13` = `0be04bfa`, gwz-core `v1.0.13` = `83626d24`),
against gwz-dev root `276153e5`.** Cases 6 and 7, the publishing cases, were
skipped by the owner's decision on 2026-09-16 and remain open (§6).

## 1. Binary, workspace and method

- **Binary.** `~/.cargo/bin/gwz`, `gwz 1.0.13`, sha256
  `57edf42e371920aec04f1bc10d4160344876deb061257c19e8c642f4e2a96905` on the Mac.
  - `--build-info`: CLI revision `0be04bfa09af2ea591f19a636a5fbf3dcfbb0629`
    (gwz-cli tag `v1.0.13`), core 1.0.13 from crates.io, source-sha256
    `23419038f240d3536d05f9f86b2a8bcd6410bef1b296741bdb0ba459045326f5` (gwz-core
    tag `v1.0.13` = `83626d247686ac0a989f789345615531f7bf2d1f`).
  - `codesign` flags `0x20002(adhoc,linker-signed)`: no hardened runtime, so the
    `connect()` interposer loads.
  - On the Pi the same release, aarch64 build: sha256
    `02616a97b2c35a45a7d0b1cb62b0fbf9a9153243c49b7245dd196075bf51d7b3`, ELF pie
    linked against OpenSSL 3; `--build-info` identical to the Mac's, including
    the core source digest.
  - **Deviation from step 4.2:** the installed release binary was used, not an
    install into a scratch `--root`. No `target/` build ran against gwz-dev.
- **Published workspace.** gwz-dev root `276153e55a3c923ee567e2791af3886af1a2fc0c`,
  with N = 8 members. Every local head in gwz-dev equalled GitHub before the run
  (P2: all nine repositories `same`).
- **Clones.**
  - https: `~/limbo/accept-2026-09-16/ws-https` (cases 1 to 3 and 5), root
    `276153e5`;
  - SSH: `~/limbo/accept-2026-09-16/ws-ssh` (case 4), root `276153e5`;
  - SSH on the Pi: `~/limbo/accept-2026-09-16-pi/ws-ssh` (case 4), root
    `276153e5`.
- **Method.**
  - Each command ran under sockwatch, which polls `lsof -g <pgid> -iTCP` back to
    back and keeps JSON, exit code and wall time. Median poll gap 30 to 31 ms on
    the Mac (maximum 107 ms), 5 to 7 ms on the Pi.
  - A `connect()` log injected into gwz was the event-based count
    (`DYLD_INSERT_LIBRARIES` on the Mac, `LD_PRELOAD` on the Pi); the lsof count
    was checked against it. In every measured run on both hosts lsof and
    `connect()` agreed exactly.
  - Self-test on loopback, Mac: 200 ms connections 10 lsof / 10 `connect()`; 5 ms
    connections 0 / 10. Pi: 10 / 10 and 6 / 10; the Pi's faster `lsof` catches
    more of the short ones. lsof never exceeded `connect()`.
  - Attribution comes from `meta.transport` rows.
  - https cases ran with `SSH_AUTH_SOCK` unset. SSH cases ran with the
    publishing agent, whose `ssh -T` greeting is `Hi owebeeone!`. The default
    agent greets as a different account and was not used by any case.
  - The Pi has no key of its own; its SSH cases used the same publishing agent,
    forwarded over the session.

## 2. Before (gwz 1.0.12, 2026-09-14: plan §2.2 and step 0.2)

| measurement | result |
|---|---|
| one SSH session (ls-remote) | 2.4–2.6 s |
| one HTTPS session | 0.5–0.6 s (TCP 12 ms, TLS 21 ms) |
| `gwz push` of gwz-dev, 8 members, SSH everywhere | 26 connections (3N+2), 44 s |
| `gwz --json clone --url-scheme https` (step 0.2) | 9 port-443 connections, no SSH, 5.7 s; all 8 members, the private one through a credential helper |
| default dry run in that clone | no connections; every row planned `refs/heads/main:refs/heads/main`, level with `origin/main`, clean |
| `gwz --json push` in that clone, nothing to publish | exit 0 after 42.4 s, every row and the aggregate `ok`; 18 port-443 + 16 port-22 = 34 connections (2N+2 https + 2N SSH); 9 `push` rows via `helper`, 16 SSH reads via `agent`, the private member's read via `helper`, 8 anonymous https reads |
| not established by step 0.2 | which helper answered; a real pack upload over https |

Expected after Phase 3 (plan §2.3, N = 8):

| push | default | `--check-remotes` |
|---|---|---|
| whole workspace, nothing to publish | 0 | N+1 (9) |
| whole workspace, one member and the root changed | N+3 (11) | N+3 (11) |
| root only, root unchanged | 0 | N+1 (9): root read + N dependency reads |

## 3. After (this run)

macOS, gwz 1.0.13:

| case | command | port-443 sessions | port-22 sessions | pushes | wall time | account used | pass/fail | notes |
|---|---|---|---|---|---|---|---|---|
| 1 | https clone, `push` | 0 (lsof 0) | 0 (lsof 0) | 0 | 0.037 s | no transport | PASS | plan: no sockets. Before: 34 connections, 42.4 s |
| 2 | https clone, `push --check-remotes` | 9 (lsof 9) | 0 (lsof 0) | 0 | 0.78 s | read anonymous ×8; read helper ×1 | PASS | plan: N+1 on 443, 0 on 22, no pushes. Before: 18 on 443 + 16 on 22 |
| 3 | https clone, `push --check-remotes --target @root` | 9 (lsof 9) | 0 (lsof 0) | 0 | 0.74 s | read anonymous ×8; read helper ×1 | PASS | plan: 443 only; derived N+1 met exactly, one row (the root). Before: 2N+2, 2N of them SSH |
| 4 | SSH clone, `push --check-remotes` | 0 (lsof 0) | 9 (lsof 9) | 0 | 4.3 s | read agent ×9 | PASS | plan: N+1 on 22, no pushes. Before: 26 SSH connections, 44 s |
| 5 | https clone, `taut-shape-py` pushurl SSH, `push --check-remotes` | 8 (lsof 8) | 1 (lsof 1) | 0 | 2.2 s | read agent ×1; read anonymous ×7; read helper ×1 | PASS | plan: that member read over SSH only (1 on 22, N on 443); `--ssh-only` confirmed one authenticated agent read and no other repository on the agent |
| 6 | https clone, member + root changed, `push` | | | | | | not run | skipped by decision (§6) |
| 7 | https clone, `tag --push <tag> --target @root` | | | | | | not run | optional; skipped by decision (§6) |

Every dry run in cases 1 to 5 made no connection on either observer and had no
transport row; every gate passed and wrote its `gate.pass`. Every measured push
exited 0 with aggregate `Noop`, every row `Noop`, and reasons `up to date with
origin/main as of the last fetch or push` (dry runs) or `already on origin`
(`--check-remotes`).

Linux/aarch64 (Pi), gwz 1.0.13:

| case | command | port-443 | port-22 | pushes | wall time | account used | pass/fail | notes |
|---|---|---|---|---|---|---|---|---|
| 4 | SSH clone, `push --check-remotes` | 0 (lsof 0) | 9 (lsof 9) | 0 | 4.32 s | read agent ×9 | PASS | identical to the Mac: 9 reads, 0 pushes, all rows `Noop` |

Clones and fetches (supporting):

| step | port-443 | port-22 | wall time | notes |
|---|---|---|---|---|
| case 1 `clone --url-scheme https` (Mac) | 9 | 0 | 5.2 s | before: 9 on 443, 5.7 s. Every member `Ok`, `https`, `request`, `derived: true`; `.gwz/url-scheme.yml` records `scheme: https` |
| case 4 `clone` (SSH, Mac) | 0 | 9 | 10.4 s | nine `clone agent` transport rows; nine `connect()` to the agent socket |
| case 4 `clone` (SSH, Pi) | 0 | 9 | 11.6 s | same root commit; nine `clone agent` rows |
| case 5 fetch-only pull (Mac) | 17 | 0 | 2.3 s | root fetch + 8 member read+fetch pairs; the pushurl member still fetched over https |
| case 1 `clone --url-scheme https` (Pi, no credential) | 9 | 0 | 3.5 s | **7 members, not 8** (§5) |

## 4. Accounts

| transport | evidence | account |
|---|---|---|
| https reads of public repositories | transport rows `anonymous` (`credential_method` unknown, `authenticated: null`) | anonymous |
| https read of gwz-core-evidence (private) | transport row `helper`; spawned `sh -c /opt/homebrew/bin/gh auth git-credential get` | `gh auth git-credential`, owebeeone |
| https pushes (cases 6 and 7) | not exercised | — |
| SSH reads (cases 4 and 5) | transport rows `agent`, authenticated; `connect()` to the publishing agent socket; `ssh -T` greeting `Hi owebeeone!` | owebeeone |
| default launchd agent (Mac) | not used by any case; `ssh -T` greeting names a different account | not under test |
| Pi | no key and no helper of its own; SSH through the forwarded publishing agent | owebeeone |

## 5. Findings and deviations

- **Binary location** (§1): the installed release rather than a scratch
  `--root` install.
- **Plan goal 2 holds in both directions.** Nothing to publish: zero
  connections and 0.037 s, against 34 connections and 42.4 s before.
  `--check-remotes`: exactly N+1 reads, run concurrently, 0.78 s over https and
  4.3 s over SSH, against 34 and 26 connections before. The root-only case met
  the derived N+1 exactly.
- **Which helper answered** is settled: `gh auth git-credential`, named by the
  spawn log. Step 0.2's open item closes.
- **Linux agrees with macOS.** Case 4 on aarch64 Debian 13 produced the same
  counts to the session and the same time to a tenth of a second. The observer
  port needed only `LD_PRELOAD` with `dlsym(RTLD_NEXT)` in `connlog.c`, the
  Linux `EINPROGRESS` value and `lsof` path in `sockwatch.py`, and a small `jq`
  stand-in; `gwzcheck.py` and `probe.py` ran unmodified.
- **A quiet clone drops an inaccessible private member with no trace in the
  clone response.** On the Pi, with no https credential, case 1's clone exited
  0 with aggregate `Ok`, `errors: []`, empty stderr, seven `Ok` member rows and
  no mention of `gwz-core-evidence` in the JSON or on disk, although a ninth
  https session did contact it. This reproduces on the Mac with an empty HOME.
  It is the designed path (`handle_materialize/apply.rs`, `quiet_clone`: the
  directory is removed, the transport observation forgotten, no row returned;
  `GwzUrlSchemePlan.md` §"skip_private_access" names it as unchanged), so a
  workspace with private members clones for someone who cannot see them
  without disclosing them. Two consequences for operators:
  - `gwz ls` in such a clone lists the member with `materialized: true` and a
    path that does not exist; `gwz status` reports `live state was not
    observed` for it.
  - This runbook measures N from case 1's clone rows, which under-counts on a
    host without the credential. The Pi run measured N from case 4's dry run
    instead (8) and stopped rather than measure a seven-member workspace.
- **Windows was not run.** From a non-interactive session the credential store
  is unreachable and no SSH agent runs, so no case can clone the workspace.
  The Windows evidence for this code is the full suite on windows-2022 (the
  dispatched Windows matrix of 2026-09-16, green).
- **Procedural.** In case 3 one `chk result` line was piped through `grep` to
  trim a duplicated banner; same command, same `--log`, all seven checks PASS.
  No tool fix, no retry and no repeat suffix was needed on either host.

## 6. What remains open

- **Cases 6 and 7.** The publishing cases (N+3 sessions for one changed member
  plus the root, and a root tag push) were skipped on 2026-09-16 by decision.
  The https scratch clone is in place with its gates passed, so they can run
  from the runbook when there is a change worth publishing.
- **Cases 1 to 3 and 5 on Linux.** They need an https credential on the Pi that
  can read the private member (`gh auth login` + `gh auth setup-git`, or an
  equivalent helper), present before case 1's clone.
- **Credential helpers elsewhere.** A machine with only `osxkeychain`, or with
  no helper, is still untested (plan §7); the Pi with no helper is the
  quiet-clone case above.
- **Connection reuse.** A changed repository costs a read and a push (plan §7).
- **Sockets, not transport rows.** `TransportObservation` has no URL or scheme,
  and `public_key_fingerprint` is always null. So SSH accounts come from the
  agent socket and `ssh -T`, not from gwz output (plan §7).
- **Tag publication.** It reads every dependency twice (2N+2); dedup there is a
  non-goal of this plan.
- **Accepted risks not exercised here.** The §3.7 risks (remote rewound, branch
  deleted, tracking ref from another repository, a published root that has since
  become unsound) are covered by native tests (step 3.6), not by this run.
- **Not exercised against GitHub.** Human output (the summary line and
  `--verbose` reasons), gwz-py's CLI, and a root-only push with an SSH `pushurl`
  (D2's accepted cost).
- **The quiet clone's reporting.** Whether `gwz ls` should say `materialized:
  true` for a member the clone silently omitted, and whether the clone response
  should carry a row for it, is a design question this run only records.

## 7. Cleanup

Done on 2026-09-17 per runbook §5. Before removal every repository in the four
scratch clones (Mac `ws-https` and `ws-ssh`, Pi `ws-https` and `ws-ssh`) was
checked: clean, nothing unpushed, no stash, no `pushurl` left from case 5. The
clones were removed; `evidence/`, `tools/`, `session.env` and the drafts remain
on each host (580 KB on the Mac, 348 KB on the Pi) as the record cited above.
