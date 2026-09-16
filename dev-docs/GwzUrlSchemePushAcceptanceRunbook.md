# Push plan step 4.2: acceptance run against GitHub (runbook)

**Status:** draft of 2026-09-16, reviewed by the integrator. It waits for the
owner's decisions (below). The workspace owner runs every step.

**Plan:** `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzUrlSchemePushPlan.md`, step
4.2. That step depends on 0.2, 2.2, 3.6, 3.8 and 4.1.

**Produces:**
- the evidence directory `~/limbo/accept-<date>/evidence/`;
- the text of `dev-docs/GwzUrlSchemePushAcceptance-<date>.md`, from the template in §4.

> **Read §6 (safety summary) before starting.**
> - **Cases 6 and 7 publish to GitHub.** Every other step either reads from GitHub
>   or stays on this Mac.
> - **Every measured push is gated.** It runs only after its dry-run gate has
>   written `gate.pass`.
> - **The two publishing commands also ask you to type `PUBLISH`.**

## Before you start: your decisions

- **Binary deviation (§0).** Confirm that this run uses the installed
  `~/.cargo/bin/gwz` 1.0.13, the published release, rather than a scratch
  `--root` install.
- **Agent socket.** Before §1.1, set `OWB_SOCK` to the SSH agent socket that
  holds your publishing key: `export OWB_SOCK=<socket path>`.
- **Case 5.** Choose the member that gets an SSH `pushurl`. The default is
  `taut-shape-py`, set as `CASE5_MEMBER` in `session.env`.
- **Case 6.** Choose the member, branch, change and two commit messages to
  publish. The runbook runs this case in the https clone, which also gives the
  first real pack upload over https.
- **Case 7.** Decide whether to run it, and with which tag (annotated or
  lightweight).
- **Instrumentation.** Decide whether evidence from the injected `connect()`
  logger is acceptable, or whether you also want uninstrumented runs (§2.3).

## Conventions

- **One terminal, in order.** Run the blocks in one terminal, in the order given.
  - The blocks work in zsh and in bash.
  - They contain no `#` comments, so pasting into zsh without `interactivecomments`
    is safe.
- **After a closed terminal.** Run `. ~/limbo/accept-<date>/session.env`, then `cd`
  into the workspace the current case uses.
- **The binary.** `$GA` is `/Users/owebeeone/.cargo/bin/gwz`, the installed gwz
  1.0.13 (P1 pins it). Every measured command runs `$GA`; no step runs a
  `target/` build. Where the text below says `gwz-alpha` (the pre-release build
  the runbook was drafted against, since removed), it means the process `$GA`
  starts.
- **Recording.** Every gwz command in the cases runs under `sw` (sockwatch, §2). It
  keeps the JSON output, the sockets, a `connect()` log and the wall-clock time.
- **Checking.** `chk` (gwzcheck, §2) reads that evidence back and prints `[PASS]` or
  `[FAIL]` for each check, then an overall verdict.
- **Evidence is never overwritten.** `sw` refuses an output prefix that already
  exists. To repeat a step, change its `-1` suffix to `-2`, in the `sw` line and in
  the `chk` lines that read it.
- **`N` is the number of members.**
  - Case 1 measures it.
  - On 2026-09-16 it was 8: gwz-cli, gwz-core, taut, gwz-py, taut-shape,
    taut-shape-rs, taut-shape-py and the private gwz-core-evidence.
  - Every expected count below is written in terms of `N`.
- **Time.** About an hour, most of it reading check output. Measured gwz commands
  take seconds each.

## 0. Deviation, recorded up front

Step 4.2 says the binary is "installed to a scratch `--root`". This run uses
the installed `~/.cargo/bin/gwz` instead: gwz 1.0.13, released on 2026-09-16
from the code the plan landed. The draft of this runbook was written against a
pre-release local build (`gwz-alpha`), which was removed once 1.0.13 was
installed. The owner confirms this deviation before starting.

What the step protects still holds:
- the binary under test is the published release, not a `target/` build;
- no `target/` build runs against gwz-dev;
- every workspace used here is a scratch clone under `~/limbo/accept-<date>/`.

P1 (§1.2) ties the binary to the `v1.0.13` tags. The results note records this
deviation.

## 1. Preconditions

### 1.1 Session and evidence layout (local)

This block creates the run directory, one evidence subdirectory per case and
`session.env`, then loads the session.

Set `OWB_SOCK` first, to the SSH agent socket that holds your publishing key:
`export OWB_SOCK=<socket path>`. The block writes that value into `session.env`
and checks that it is a socket.

<!-- local-block: setup -->
```sh
export ACC_DATE="$(date +%Y-%m-%d)"
export ACC="$HOME/limbo/accept-$ACC_DATE"
mkdir -p "$ACC/tools" "$ACC/evidence/00-preconditions" "$ACC/evidence/selftest" "$ACC/evidence/case1" "$ACC/evidence/case2" "$ACC/evidence/case3" "$ACC/evidence/case4" "$ACC/evidence/case5" "$ACC/evidence/case6" "$ACC/evidence/case7"
cat > "$ACC/session.env" <<GWZ_ACCEPT_SESSION
export ACC_DATE="$ACC_DATE"
export ACC="$ACC"
export EV="$ACC/evidence"
export TOOLS="$ACC/tools"
export GA=/Users/owebeeone/.cargo/bin/gwz
export OWB_SOCK="$OWB_SOCK"
export WS_HTTPS="$ACC/ws-https"
export WS_SSH="$ACC/ws-ssh"
export CASE5_MEMBER=taut-shape-py
unset GWZ_URL_SCHEME
if [ -f "\$EV/N.txt" ]; then export N="\$(cat "\$EV/N.txt")"; fi
sw() { python3.13 "\$TOOLS/sockwatch.py" --interpose "\$TOOLS/connlog.dylib" "\$@"; }
chk() { python3.13 "\$TOOLS/gwzcheck.py" "\$@"; }
GWZ_ACCEPT_SESSION
. "$ACC/session.env"
printf 'ACC=%s\nEV=%s\nGA=%s\nWS_HTTPS=%s\nWS_SSH=%s\nCASE5_MEMBER=%s\n' "$ACC" "$EV" "$GA" "$WS_HTTPS" "$WS_SSH" "$CASE5_MEMBER"
ls "$EV"
if [ -S "$OWB_SOCK" ]; then echo "OWB_SOCK is an agent socket"; else echo "STOP: set OWB_SOCK to your agent socket, then rerun this block"; fi
```

**Expected:**
- the variables print;
- `ls` lists `00-preconditions`, `case1` to `case7` and `selftest`;
- the last line says `OWB_SOCK is an agent socket`.

**Harmless noise.** `/Users/owebeeone/.git` is an empty, read-only file, so a bare
`git` run in `$ACC` (not a repository) says `fatal: invalid gitfile format`.
- No step runs git there; every git command names its repository with `-C`.
- While drafting, a copy of that layout in scratch did not affect `$GA clone`,
  dry runs or `--check-remotes`.

`CASE5_MEMBER` is the member case 5 configures. Change it in `session.env` before
case 5 if you prefer another member.

### 1.2 P1: binary identity (local)

<!-- local-block: p1 -->
```sh
"$GA" --version | tee "$EV/00-preconditions/binary.txt"
"$GA" --build-info | tee -a "$EV/00-preconditions/binary.txt"
shasum -a 256 "$GA" | tee -a "$EV/00-preconditions/binary.txt"
ls -l "$GA" | tee -a "$EV/00-preconditions/binary.txt"
codesign -dv "$GA" 2>&1 | tee -a "$EV/00-preconditions/binary.txt"
git -C /Users/owebeeone/limbo/gwz-dev/gwz-cli rev-parse v1.0.13 | tee -a "$EV/00-preconditions/binary.txt"
git -C /Users/owebeeone/limbo/gwz-dev/gwz-core rev-parse v1.0.13 | tee -a "$EV/00-preconditions/binary.txt"
```

Expected values, as observed on 2026-09-16 after gwz 1.0.13 was installed:

| check | expected |
|---|---|
| `$GA --version` | `gwz 1.0.13` |
| `$GA --build-info`, `cli:` line | `revision=0be04bfa09af2ea591f19a636a5fbf3dcfbb0629 dirty=false` |
| `$GA --build-info`, `core 1.0.13:` line | `revision=unavailable dirty=unknown source-sha256=23419038f240d3536d05f9f86b2a8bcd6410bef1b296741bdb0ba459045326f5` (gwz-core from crates.io) |
| sha256 of `$GA` | `57edf42e371920aec04f1bc10d4160344876deb061257c19e8c642f4e2a96905` |
| `codesign` flags | `0x20002(adhoc,linker-signed)`: no `runtime` flag, so `--interpose` can load |
| gwz-cli tag `v1.0.13` | `0be04bfa09af2ea591f19a636a5fbf3dcfbb0629`, the `cli:` revision above |
| gwz-core tag `v1.0.13` | `83626d247686ac0a989f789345615531f7bf2d1f` (`chore(release): gwz-core 1.0.13`) |

`$GA --build-info` names the gwz-cli commit it was built from and a digest of the
packaged gwz-core sources. The evidence is this chain:
- the `cli:` revision is the commit gwz-cli's `v1.0.13` tag names;
- the `core` line is 1.0.13, the version gwz-core's `v1.0.13` tag released to
  crates.io;
- the hash matches the table.

**Stop** if any value differs.

### 1.3 P2: gwz-dev is pushed (contacts GitHub, read-only)

This compares every local head in gwz-dev with GitHub over https. It uses the `gh`
credential helper for the private member, and it changes nothing in gwz-dev.

<!-- local-block: p2-list -->
```sh
{ printf '. %s\n' "$(git -C /Users/owebeeone/limbo/gwz-dev config --get remote.origin.url)"; awk '/^  path: /{p=$2} /^    url: /{print p, $2}' /Users/owebeeone/limbo/gwz-dev/gwz.conf/gwz.yml; } > "$EV/00-preconditions/gwz-dev-repos.txt"
cat "$EV/00-preconditions/gwz-dev-repos.txt"
```

**Expected:** 9 lines, the root `.` and the 8 members, each with its
`git@github.com:owebeeone/<repo>.git` URL.

```sh
while read -r p url; do b="$(git -C "/Users/owebeeone/limbo/gwz-dev/$p" symbolic-ref --short HEAD)"; l="$(git -C "/Users/owebeeone/limbo/gwz-dev/$p" rev-parse HEAD)"; r="$(git ls-remote "$(printf '%s' "$url" | sed -E 's#^git@github\.com:#https://github.com/#')" "refs/heads/$b" </dev/null | cut -f1)"; if [ -n "$r" ] && [ "$l" = "$r" ]; then v=same; else v=DIFFERENT; fi; printf '%-18s %-8s local %s github %s %s\n' "$p" "$b" "$l" "${r:-none}" "$v"; done < "$EV/00-preconditions/gwz-dev-repos.txt" | tee "$EV/00-preconditions/gwz-dev-pushed.txt"
```

**Expected:** 9 lines, each ending `same`.
- **If any line says `DIFFERENT`, stop.** Publish gwz-dev with your usual gwz 1.0.12
  flow (outside this runbook), then rerun this block.
- **When all are `same`,** the published root lock names the binary's three member
  commits (P1).

**Owner declaration, for the note.** Nobody else pushes to these nine repositories
between now and the verification after case 6. The expected counts assume it.

### 1.4 P3: accounts, configuration and tools

**Local checks:**

<!-- local-block: p3-local -->
```sh
command -v python3.13 jq lsof nettop pgrep gh cc codesign shasum ssh ssh-add | tee "$EV/00-preconditions/tools.txt"
{ python3.13 --version; jq --version; gh --version | head -1; ssh -V 2>&1; lsof -v 2>&1 | sed -n '2p'; sw_vers; } | tee -a "$EV/00-preconditions/tools.txt"
git config --show-origin --get-regexp '^credential' | tee "$EV/00-preconditions/credential-helpers.txt"
git config --show-origin --get-regexp '^(url\.|http\.|core\.sshcommand|fetch\.prune)' | tee "$EV/00-preconditions/git-config-risky.txt"
env | grep -i -E '^(https?_proxy|all_proxy|no_proxy|git_ssh|git_ssh_command|gwz_url_scheme)=' | tee "$EV/00-preconditions/env-risky.txt"
SSH_AUTH_SOCK="$OWB_SOCK" ssh-add -l | tee "$EV/00-preconditions/owebeeone-agent-keys.txt"
```

**Expected:**
- all tools found;
- `credential-helpers.txt` shows `credential.https://github.com.helper` as an empty
  reset followed by `!/opt/homebrew/bin/gh auth git-credential`, plus the Homebrew
  system `credential.helper osxkeychain`;
- `git-config-risky.txt` and `env-risky.txt` are empty;
- `ssh-add -l` lists the owebeeone keys.

**Stop if a `url.*.insteadOf` or `pushInsteadOf` rule, a proxy or `GWZ_URL_SCHEME`
shows up.** libgit2 applies `insteadOf` rules, so an https clone could silently use
SSH, and a proxy changes which ports are seen.

**GitHub checks (read-only):**

```sh
gh auth status --hostname github.com 2>&1 | tee "$EV/00-preconditions/gh-auth-status.txt"
ssh -F /dev/null -o IdentityAgent="$OWB_SOCK" -o IdentityFile=none -o BatchMode=yes -T git@github.com 2>&1 | tee "$EV/00-preconditions/ssh-account-owebeeone-agent.txt"
ssh -F /dev/null -o IdentityFile=none -o BatchMode=yes -T git@github.com 2>&1 | tee "$EV/00-preconditions/ssh-account-default-agent.txt"
```

**Expected:**
- `gh`: logged in to github.com as `owebeeone`. The token is shown masked.
- The owebeeone agent: `Hi owebeeone! You've successfully authenticated, ...`. `ssh`
  exits 1, which is normal.
- The default agent (optional, for contrast): the greeting names the default
  agent's account, which is not the account under test.

The SSH probe uses no config file and no key files, only the agent. libssh2 inside
gwz does the same, since it reads no `~/.ssh/config` and offers only the agent. So
the greeting names the account gwz's SSH reads use.

## 2. Socket measurement method (no sudo)

### 2.1 Choice and why

**Primary: poll the process group with lsof, as step 0.2 did.**
- Step 0.2 polled `lsof -a -p <pid> -iTCP` for ports 22 and 443. Using the same
  kind of observation keeps the before and after numbers comparable.
- `sockwatch.py` starts the measured command in a new session, so the command leads
  its own process group.
- It then runs `lsof +c 0 -nP -a -g <pgid> -iTCP -F pcfnT` back to back until the
  command exits.
- It keeps each distinct connection, keyed by (local endpoint, remote endpoint),
  with its process name, first and last sighting and TCP states.
- A `ps` census every 200 ms records the processes in the tree. Any descendant that
  leaves the group is polled by pid.
- It also writes the JSON output, the wall-clock time from spawn to exit, the exit
  code and a per-port session count.

**Why the process group is the right scope:**
- **SSH runs in-process.** gwz-core uses `git2` with the `ssh` and `https` features.
  The vendored libgit2-sys 0.18.8+1.9.7 defines `GIT_SSH_LIBSSH2` and
  `GIT_SECURE_TRANSPORT`, and the binary contains `SSH-2.0-libssh2_1.11.1_DEV`.
  gwz never starts `ssh` or `git`.
- **Confirmed on loopback.** With `https://127.0.0.1:<port>` and
  `ssh://git@127.0.0.1:<port>` remotes, both sockets belonged to the `gwz-alpha` pid,
  and no child process existed.
- **The only children are credential helpers.** git2's `CredentialHelper` starts
  them as `sh -c '<helper> get'`. On this Mac that means `gh auth git-credential`,
  since git2 tries `credential.https://github.com.helper` before any global helper.
  The process group includes them.

**Cross-check: an event-based `connect()` log.**
- `connlog.dylib` is loaded with `DYLD_INSERT_LIBRARIES`, which `sw` sets.
- It logs every `connect()` call gwz-alpha makes: address, port and result. Polling
  cannot miss those.
- It also logs every process gwz-alpha starts (the credential helpers, with their
  argv) and every Unix-socket connect (the SSH agent socket).
- This is how the note can name which helper answered and which agent was used. Step
  0.2 left the first open, and gwz's `public_key_fingerprint` is always null (gwz-core
  `transport_observations.rs:75`), so gwz cannot answer the second.
- The log never records environment variables or socket data.

**Considered and not chosen:**
- **`nettop -L` (logging mode).** It is event-based and needs no sudo. On loopback it
  reported 15 of 15 flows, including 20 ms ones.
  - But it observes the whole system unless filtered by process name.
  - Filtering by pid, started just after the process, caught 0 of 6 flows.
  - Its CSV format is undocumented.
  - Optional use is in §2.3.
- **`dtrace`, `dtruss` and `fs_usage`.** They need root.

### 2.2 What was verified locally while drafting (no network)

Loopback connections of fixed lifetime, 20 per row, 50 ms apart. lsof polled back
to back, with a median gap of 29 ms and a maximum of 65 to 80 ms.

| connection lifetime | lsof polling saw | `connect()` log saw |
|---|---|---|
| 500 ms | 20 | 20 |
| 200 ms | 20 | 20 |
| 100 ms | 20 | 20 |
| 50 ms | 19 | 20 |
| 20 ms | 5 | 20 |
| 5 ms | 0 | 20 |
| 1 ms | 0 | 20 |

- **Gaps grow under load.** In the §2.5 self-test runs during drafting, the median
  gap ranged from 30 to 61 ms and the maximum reached 206 ms. All 200 ms connections
  were still seen.
- **Children are covered.** 9 concurrent 500 ms connections from child processes
  started through `/bin/sh`: lsof saw 9. No `connect()` log here, because `/bin/sh`
  drops `DYLD_INSERT_LIBRARIES`.
- **gwz-alpha's own sockets (loopback https and ssh remotes):**
  - held 400 ms: lsof 1, `connect()` 1, owned by `gwz-alpha`, no spawns;
  - held 20 ms: lsof 0, `connect()` 1.
- **The interposer changes nothing.** On successful local-transport pushes (default,
  and `--check-remotes`), it left exit codes and JSON unchanged.
- **Real GitHub sessions are long.** §2.2 of the plan measured SSH sessions at
  2.4 to 2.6 s and HTTPS sessions at 0.5 to 0.6 s. That is 17 to 90 times the poll
  gap.

### 2.3 Limits and mitigations

**Limits:**
- **Short connections.** Polling misses connections shorter than the poll gap:
  about 50 ms on an idle Mac (table above), up to about 200 ms when it is busy.
  - A connection refused or reset at once never shows in lsof.
  - The `connect()` log records it, with its `errno`, under `failed connect`.
- **Interposer scope.** The `connect()` log covers only gwz-alpha itself.
  - macOS drops `DYLD_INSERT_LIBRARIES` for SIP-protected programs such as
    `/bin/sh`, and for everything they start.
  - So a helper's own network use (`gh`) would show only in lsof, as
    `[WARN] TCP sessions from helper processes`. `chk` does not count those as gwz
    sessions.
  - Certificate revocation checks run in the system `trustd`, outside the tree, and
    are not counted. They were not counted in step 0.2 either.
- **The interposer is an injected library.** It adds a logging library to the
  measured process. If you want an uninstrumented run as well, run
  `python3.13 "$TOOLS/sockwatch.py"` directly, without `--interpose`, using a new
  suffix, and compare the lsof counts.
- **Sessions, not requests.** A kept-alive HTTPS connection counts once however many
  requests it carries. The plan counts sessions.
- **Attribution.** Sockets do not name a repository. Attribution comes from the JSON
  `meta.transport` rows (repository path, operation, credential method). This is open
  item "sockets, not transport rows" in plan §7.
- **Address fallback.** If an address fails and libgit2 falls back to the next one,
  the fallback is a second `connect()`. It appears under `failed connect` and in
  `by address`.
- **Port reuse.** Two sequential connections reusing one ephemeral port within one
  run would be counted once. macOS picks ephemeral ports at random, so this is
  unlikely.

**Mitigations built into the steps:**
- **Two observers per run.** Every run uses both lsof and `connect()`. `chk` takes
  the `connect()` count as the session count, and fails if lsof saw more than
  `connect()`.
  - That would mean a connection outside gwz-alpha's own calls.
  - When lsof saw fewer, `chk` prints `[WARN] ... polling missed`.
- **Cross-check with JSON.** Read and push counts from JSON transport rows
  (`--expect-reads`, `--expect-pushes`) are checked against the session counts.
- **Repeat when unsure.** If counts disagree, or a result is surprising, repeat the
  case with the next suffix. Cases 1 to 5 publish nothing, so repeats are safe. Never
  repeat the publication in case 6 or 7. Their evidence is the `connect()` log plus
  the transport rows.
- **Optional third observer.** Start nettop in a second terminal before a measured
  command, and stop it with Ctrl-C afterwards:

  ```sh
  nettop -L 0 -m tcp -n -x -s 1 -p gwz > "$EV/caseN/nettop-1.csv"
  ```

  Then count distinct `<->` lines ending `:443` or `:22`. Replace `caseN` with the
  case directory. The name filter was tested on loopback with `-p Python`, not with
  `gwz`.

### 2.4 Install the tools (local)

Paste each block as a whole. Each writes one file.

**`sockwatch.py`**, the wrapper:

<!-- local-block: tool-sockwatch -->
```sh
cat > "$TOOLS/sockwatch.py" <<'GWZ_ACCEPT_SOCKWATCH_PY'
#!/usr/bin/env python3.13
"""sockwatch: run one command and record the TCP connections its process tree makes.

usage:
  python3.13 sockwatch.py [--unset-env NAME] [--set-env NAME=VALUE]
                          [--interpose DYLIB] OUT_PREFIX -- COMMAND [ARG ...]

The command starts in a new session, so it leads its own process group, and gwz
and every helper it starts (sh, gh, git-credential-*) stay in that group. Until
the command exits, sockwatch runs `lsof +c 0 -nP -a -g <pgid> -iTCP -F pcfnT`
back to back and keeps every distinct connection, keyed by (local endpoint,
remote endpoint). Every 200 ms a `ps` census records the processes in the tree
and adds any descendant that left the group to a second lsof selection.

Files written next to OUT_PREFIX:
  OUT_PREFIX.stdout        the command's stdout, unmodified (gwz --json output)
  OUT_PREFIX.stderr        the command's stderr
  OUT_PREFIX.sockets.json  distinct TCP connections, in first-seen order
  OUT_PREFIX.procs.json    processes seen in the tree
  OUT_PREFIX.summary.json  argv, cwd, environment choices, start and end time,
                           wall seconds, exit code, poll statistics, sessions
                           by remote port and, with --interpose, connect() counts
  OUT_PREFIX.connect.log   with --interpose only: one line per connect() call
                           and process spawn inside the measured program

sockwatch refuses to overwrite an existing OUT_PREFIX.summary.json, prints one
summary line on stderr, and exits with the command's exit code.

Limits: polling sees a connection only if it is open during a poll. One lsof
call takes about 30 ms on this Mac, so a connection that opens and closes
between two polls is missed. --interpose logs every connect() call instead, but
only in programs that load the library: macOS drops DYLD_INSERT_LIBRARIES for
SIP-protected programs such as /bin/sh, /usr/bin/env and /usr/bin/time, and for
everything those programs start. Give the measured program directly as COMMAND,
and set or unset environment variables with the options above, not with env.
"""

import argparse
import datetime
import json
import os
import re
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path

LSOF = "/usr/sbin/lsof"
PS = "/bin/ps"
CENSUS_INTERVAL_S = 0.2
EINPROGRESS = 36
LOG_FIELD = re.compile(r"(\w+)=(\S*)")


def local_now():
    return datetime.datetime.now().astimezone().isoformat(timespec="milliseconds")


def split_endpoint(text):
    host, _, port = text.rpartition(":")
    return host.strip("[]"), (int(port) if port.isdigit() else None)


def lsof_connections(selection):
    """TCP connections (not listeners) for one lsof selection, e.g. ["-g", "123"]."""
    result = subprocess.run(
        [LSOF, "+c", "0", "-nP", "-a", *selection, "-iTCP", "-F", "pcfnT"],
        capture_output=True,
        text=True,
    )
    records = []
    pid = None
    command = None
    current = None
    for line in result.stdout.splitlines():
        if not line:
            continue
        tag, value = line[0], line[1:]
        if tag == "p":
            pid, command, current = int(value), None, None
        elif tag == "c":
            command = value
        elif tag == "f":
            current = {"pid": pid, "command": command, "name": None, "state": None}
            records.append(current)
        elif tag == "n" and current is not None:
            current["name"] = value
        elif tag == "T" and current is not None and value.startswith("ST="):
            current["state"] = value[3:]
    return [record for record in records if record["name"] and "->" in record["name"]]


def process_census(root_pid, pgid):
    """Processes in the command's group or below its pid, and descendants that left the group."""
    result = subprocess.run([PS, "-axo", "pid=,ppid=,pgid=,comm="], capture_output=True, text=True)
    table = {}
    for line in result.stdout.splitlines():
        parts = line.split(None, 3)
        if len(parts) == 4 and parts[0].isdigit():
            table[int(parts[0])] = (int(parts[1]), int(parts[2]), parts[3])
    tree = {root_pid} if root_pid in table else set()
    grew = True
    while grew:
        grew = False
        for pid, (ppid, _group, _comm) in table.items():
            if ppid in tree and pid not in tree:
                tree.add(pid)
                grew = True
    selected = tree | {pid for pid, (_ppid, group, _comm) in table.items() if group == pgid}
    escaped = {pid for pid in tree if table[pid][1] != pgid}
    return {pid: table[pid] for pid in selected}, escaped


def read_connect_log(path):
    summary = {
        "connect_calls": 0,
        "tcp_attempts_by_port": {},
        "tcp_attempts_by_address": {},
        "tcp_failures": [],
        "unix_connects": {},
        "spawns": [],
    }
    if not path.exists():
        summary["missing"] = True
        return summary
    for line in path.read_text(errors="replace").splitlines():
        kind, _, rest = line.partition(" ")
        if kind == "connect":
            head, _, address = rest.partition(" addr=")
            fields = dict(LOG_FIELD.findall(head))
            summary["connect_calls"] += 1
            family = fields.get("family")
            if family in ("inet", "inet6"):
                result = int(fields.get("result", "0"))
                error = int(fields.get("errno", "0"))
                if result == 0 or error == EINPROGRESS:
                    port = fields.get("port", "?")
                    summary["tcp_attempts_by_port"][port] = summary["tcp_attempts_by_port"].get(port, 0) + 1
                    where = f"{address}:{port}"
                    summary["tcp_attempts_by_address"][where] = summary["tcp_attempts_by_address"].get(where, 0) + 1
                else:
                    summary["tcp_failures"].append(line)
            elif family == "unix":
                summary["unix_connects"][address] = summary["unix_connects"].get(address, 0) + 1
        elif kind == "spawn":
            head, _, argv = rest.partition(" argv=")
            fields = dict(LOG_FIELD.findall(head))
            summary["spawns"].append(
                {key: fields.get(key) for key in ("t", "pid", "fn", "child", "result")} | {"argv": argv}
            )
    return summary


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--unset-env", action="append", default=[], metavar="NAME")
    parser.add_argument("--set-env", action="append", default=[], metavar="NAME=VALUE")
    parser.add_argument("--interpose", metavar="DYLIB")
    parser.add_argument("out_prefix")
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        parser.error("give the command after --")

    prefix = Path(args.out_prefix)
    summary_path = Path(f"{prefix}.summary.json")
    if summary_path.exists():
        sys.exit(f"sockwatch: {summary_path} exists; choose a new OUT_PREFIX (evidence is never overwritten)")
    prefix.parent.mkdir(parents=True, exist_ok=True)

    env = dict(os.environ)
    for name in args.unset_env:
        env.pop(name, None)
    for item in args.set_env:
        name, _, value = item.partition("=")
        env[name] = value
    connect_log = None
    if args.interpose:
        connect_log = Path(f"{prefix}.connect.log").resolve()
        env["DYLD_INSERT_LIBRARIES"] = str(Path(args.interpose).resolve())
        env["CONNLOG_FILE"] = str(connect_log)

    started = local_now()
    exit_info = {}
    with open(f"{prefix}.stdout", "wb") as out, open(f"{prefix}.stderr", "wb") as err:
        t0 = time.monotonic()
        proc = subprocess.Popen(command, stdout=out, stderr=err, env=env, start_new_session=True)

    def wait_for_exit():
        code = proc.wait()
        exit_info["seconds"] = time.monotonic() - t0
        exit_info["ended"] = local_now()
        exit_info["code"] = code

    waiter = threading.Thread(target=wait_for_exit, daemon=True)
    waiter.start()

    pgid = proc.pid
    connections = {}
    processes = {}
    poll_times = []
    escaped = set()
    escaped_seen = set()
    last_census = None
    interrupted = False
    while True:
        try:
            finished = not waiter.is_alive()
            elapsed = time.monotonic() - t0
            if last_census is None or finished or elapsed - last_census >= CENSUS_INTERVAL_S:
                seen, escaped = process_census(proc.pid, pgid)
                escaped_seen |= escaped
                last_census = elapsed
                for pid, (ppid, group, comm) in seen.items():
                    processes.setdefault(
                        pid,
                        {"pid": pid, "ppid": ppid, "pgid": group, "command": comm, "first_seen_s": round(elapsed, 3)},
                    )
            poll_times.append(time.monotonic() - t0)
            records = lsof_connections(["-g", str(pgid)])
            if escaped:
                records += lsof_connections(["-p", ",".join(str(pid) for pid in sorted(escaped))])
            observed = round(time.monotonic() - t0, 3)
            for record in records:
                local, _, remote = record["name"].partition("->")
                entry = connections.get((local, remote))
                if entry is None:
                    host, port = split_endpoint(remote)
                    entry = {
                        "local": local,
                        "remote": remote,
                        "remote_host": host,
                        "remote_port": port,
                        "pid": record["pid"],
                        "command": record["command"],
                        "first_seen_s": observed,
                        "last_seen_s": observed,
                        "polls_seen": 0,
                        "states": [],
                    }
                    connections[(local, remote)] = entry
                entry["last_seen_s"] = observed
                entry["polls_seen"] += 1
                if record["state"] and record["state"] not in entry["states"]:
                    entry["states"].append(record["state"])
            if finished:
                break
        except KeyboardInterrupt:
            interrupted = True
            try:
                os.killpg(pgid, signal.SIGINT)
            except ProcessLookupError:
                pass
    waiter.join()

    ordered = sorted(connections.values(), key=lambda entry: entry["first_seen_s"])
    by_port = {}
    by_command_port = {}
    runs = []
    for entry in ordered:
        port = str(entry["remote_port"])
        by_port[port] = by_port.get(port, 0) + 1
        key = f'{entry["command"]}:{port}'
        by_command_port[key] = by_command_port.get(key, 0) + 1
        if runs and runs[-1][0] == port:
            runs[-1][1] += 1
        else:
            runs.append([port, 1])
    gaps = sorted((later - earlier) * 1000 for earlier, later in zip(poll_times, poll_times[1:]))

    def gap_at(fraction):
        if not gaps:
            return None
        return round(gaps[min(len(gaps) - 1, int(fraction * len(gaps)))], 1)

    try:
        json.loads(Path(f"{prefix}.stdout").read_bytes())
        stdout_is_json = True
    except ValueError:
        stdout_is_json = False

    code = exit_info.get("code", 1)
    summary = {
        "tool": "sockwatch 1",
        "argv": command,
        "cwd": os.getcwd(),
        "environment": {
            "SSH_AUTH_SOCK": env.get("SSH_AUTH_SOCK", "(unset)"),
            "GWZ_URL_SCHEME": env.get("GWZ_URL_SCHEME", "(unset)"),
            "DYLD_INSERT_LIBRARIES": env.get("DYLD_INSERT_LIBRARIES", "(unset)"),
        },
        "started": started,
        "ended": exit_info.get("ended"),
        "wall_seconds": round(exit_info.get("seconds", 0.0), 3),
        "exit_code": code,
        "interrupted": interrupted,
        "polls": len(poll_times),
        "poll_gap_ms": {"median": gap_at(0.5), "p95": gap_at(0.95), "max": round(gaps[-1], 1) if gaps else None},
        "sessions_total": len(ordered),
        "sessions_by_remote_port": by_port,
        "sessions_by_command_and_port": by_command_port,
        "first_seen_order": ", ".join(f"{port}x{count}" for port, count in runs),
        "processes": sorted({process["command"] for process in processes.values()}),
        "escaped_process_group": sorted(escaped_seen),
        "stdout_is_json": stdout_is_json,
    }
    if connect_log is not None:
        summary["connect_log"] = read_connect_log(connect_log)

    Path(f"{prefix}.sockets.json").write_text(json.dumps(ordered, indent=2) + "\n")
    Path(f"{prefix}.procs.json").write_text(
        json.dumps(sorted(processes.values(), key=lambda process: process["first_seen_s"]), indent=2) + "\n"
    )
    summary_path.write_text(json.dumps(summary, indent=2) + "\n")

    line = (
        f"sockwatch: exit={code} wall={summary['wall_seconds']}s sessions={json.dumps(by_port)}"
        f" order=[{summary['first_seen_order']}] polls={len(poll_times)}"
        f" gap_ms(median/max)={summary['poll_gap_ms']['median']}/{summary['poll_gap_ms']['max']}"
    )
    if connect_log is not None:
        line += f" connect()={json.dumps(summary['connect_log']['tcp_attempts_by_port'])}"
    print(line, file=sys.stderr)
    sys.exit(code if code >= 0 else 128 - code)


if __name__ == "__main__":
    main()
GWZ_ACCEPT_SOCKWATCH_PY
```

**`connlog.c`**, the `connect()` and spawn logger:

<!-- local-block: tool-connlog -->
```sh
cat > "$TOOLS/connlog.c" <<'GWZ_ACCEPT_CONNLOG_C'
/* connlog.c: log connect() calls and process spawns inside the program that loads it.
 *
 * Build: cc -dynamiclib -O2 -Wall -Wextra -o connlog.dylib connlog.c
 * Use:   CONNLOG_FILE=/abs/run.connect.log DYLD_INSERT_LIBRARIES=/abs/connlog.dylib PROGRAM ...
 *        (sockwatch.py --interpose DYLIB sets both variables.)
 *
 * macOS ignores DYLD_INSERT_LIBRARIES for SIP-protected programs (/bin/sh,
 * /usr/bin/env, /usr/bin/time), for hardened-runtime binaries, and for anything
 * those programs start, so run the measured program directly.
 *
 * The log holds addresses, ports, results and spawned argv only. It never
 * records environment variables or any data sent on a socket.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

static void connlog_append(char *line, size_t capacity, int written)
{
    const char *path = getenv("CONNLOG_FILE");
    if (path == NULL || written <= 0) {
        return;
    }
    size_t length = (size_t)written;
    if (length >= capacity) {
        length = capacity - 1;
        line[length - 1] = '\n';
    }
    int out = open(path, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0600);
    if (out < 0) {
        return;
    }
    (void)write(out, line, length);
    close(out);
}

static int connlog_connect(int fd, const struct sockaddr *address, socklen_t address_len)
{
    int result = connect(fd, address, address_len);
    int saved_errno = errno;
    char text[256] = "?";
    const char *family = "other";
    int port = -1;
    if (address != NULL && address->sa_family == AF_INET) {
        const struct sockaddr_in *in4 = (const struct sockaddr_in *)address;
        inet_ntop(AF_INET, &in4->sin_addr, text, sizeof text);
        port = ntohs(in4->sin_port);
        family = "inet";
    } else if (address != NULL && address->sa_family == AF_INET6) {
        const struct sockaddr_in6 *in6 = (const struct sockaddr_in6 *)address;
        inet_ntop(AF_INET6, &in6->sin6_addr, text, sizeof text);
        port = ntohs(in6->sin6_port);
        family = "inet6";
    } else if (address != NULL && address->sa_family == AF_UNIX) {
        const struct sockaddr_un *local = (const struct sockaddr_un *)address;
        snprintf(text, sizeof text, "%.*s", (int)strnlen(local->sun_path, sizeof local->sun_path),
                 local->sun_path);
        family = "unix";
    }
    struct timeval now;
    gettimeofday(&now, NULL);
    char line[512];
    int written = snprintf(line, sizeof line,
                           "connect t=%ld.%06d pid=%d fd=%d family=%s port=%d result=%d errno=%d addr=%s\n",
                           (long)now.tv_sec, (int)now.tv_usec, (int)getpid(), fd, family, port, result,
                           result == 0 ? 0 : saved_errno, text);
    connlog_append(line, sizeof line, written);
    errno = saved_errno;
    return result;
}

static void connlog_spawn(const char *function, const char *path, char *const argv[], int result,
                          const pid_t *child)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    char line[1024];
    int child_pid = -1;
    if (result == 0 && child != NULL) {
        child_pid = (int)*child;
    }
    int written = snprintf(line, sizeof line, "spawn t=%ld.%06d pid=%d fn=%s child=%d result=%d path=%s argv=",
                           (long)now.tv_sec, (int)now.tv_usec, (int)getpid(), function, child_pid, result,
                           path != NULL ? path : "?");
    if (written < 0) {
        return;
    }
    size_t used = (size_t)written;
    for (int index = 0; argv != NULL && argv[index] != NULL && used + 2 < sizeof line; index++) {
        int added = snprintf(line + used, sizeof line - used, "%s%s", index == 0 ? "" : " ", argv[index]);
        if (added < 0) {
            break;
        }
        used += (size_t)added;
    }
    if (used + 2 > sizeof line) {
        used = sizeof line - 2;
    }
    line[used] = '\n';
    line[used + 1] = '\0';
    connlog_append(line, sizeof line, (int)used + 1);
}

static int connlog_posix_spawn(pid_t *child, const char *path, const posix_spawn_file_actions_t *actions,
                               const posix_spawnattr_t *attributes, char *const argv[], char *const envp[])
{
    int result = posix_spawn(child, path, actions, attributes, argv, envp);
    connlog_spawn("posix_spawn", path, argv, result, child);
    return result;
}

static int connlog_posix_spawnp(pid_t *child, const char *file, const posix_spawn_file_actions_t *actions,
                                const posix_spawnattr_t *attributes, char *const argv[], char *const envp[])
{
    int result = posix_spawnp(child, file, actions, attributes, argv, envp);
    connlog_spawn("posix_spawnp", file, argv, result, child);
    return result;
}

static int connlog_execve(const char *path, char *const argv[], char *const envp[])
{
    connlog_spawn("execve", path, argv, 0, NULL);
    return execve(path, argv, envp);
}

typedef struct {
    const void *replacement;
    const void *replacee;
} connlog_interpose_t;

__attribute__((used)) static const connlog_interpose_t connlog_interpose_table[]
    __attribute__((section("__DATA,__interpose"))) = {
        {(const void *)&connlog_connect, (const void *)&connect},
        {(const void *)&connlog_posix_spawn, (const void *)&posix_spawn},
        {(const void *)&connlog_posix_spawnp, (const void *)&posix_spawnp},
        {(const void *)&connlog_execve, (const void *)&execve},
};
GWZ_ACCEPT_CONNLOG_C
```

**`gwzcheck.py`**, the gates and checks:

<!-- local-block: tool-gwzcheck -->
```sh
cat > "$TOOLS/gwzcheck.py" <<'GWZ_ACCEPT_GWZCHECK_PY'
#!/usr/bin/env python3.13
"""gwzcheck: gates and checks for the push acceptance run.

Reads gwz --json output, sockwatch summaries and local git state. It never
contacts a remote.

  gwzcheck.py gate DRYRUN_JSON [--planned PATH ...]
      Every row must be status Noop, planned.action Noop, with the message
      "up to date with origin/<branch> as of the last fetch or push", where
      <branch> is the row's destination branch. "behind ..." fails. Rows named
      with --planned (case 6) must instead be status Planned, action Push.

  gwzcheck.py precheck WORKSPACE DRYRUN_JSON --scheme https|ssh|any
                       [--ssh-push PATH] [--ahead PATH ...]
      For every row's repository: on a branch; refs/remotes/origin/<branch>
      equals HEAD (--ahead: HEAD is a fast-forward ahead of it); remotes are
      exactly origin; the fetch refspec is +refs/heads/*:refs/remotes/origin/*;
      the URL has the scheme; no pushurl (--ssh-push: the SSH form of the URL);
      no staged or modified tracked files. With --ahead . and a member, the
      root's committed lock names the member's HEAD.

  gwzcheck.py result RUN_PREFIX [--expect-exit N] [--expect-aggregate S]
                     [--expect-rows N] [--expect-443 N] [--expect-22 N]
                     [--expect-pushes N] [--expect-ok PATH ...]
                     [--expect-reason up-to-date|already] [--expect-no-transport]
                     [--ssh-only PATH]
      Prints the run (rows, transport rows, sessions, connect() log, helper
      spawns, agent sockets) and checks the expectations. With a connect() log,
      the connect() count is the session count, and lsof must not see more.

  gwzcheck.py row CASE RUN_PREFIX
      Prints one Markdown table row for the results note.

Each subcommand exits 0 only when every check passes.
"""

import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path

UP_TO_DATE = re.compile(r"^up to date with (?P<remote>[^/]+)/(?P<branch>.+) as of the last fetch or push$")
BEHIND = re.compile(r"^behind .+ as of the last fetch or push$")
STANDARD_FETCH = "+refs/heads/*:refs/remotes/origin/*"


class Report:
    def __init__(self):
        self.failures = 0

    def check(self, ok, text):
        print(f"  [{'PASS' if ok else 'FAIL'}] {text}")
        if not ok:
            self.failures += 1
        return ok

    def finish(self, title):
        print(f"{title}: {'PASS' if self.failures == 0 else f'FAIL ({self.failures} failed)'}")
        return 0 if self.failures == 0 else 1


def load_json(path):
    with open(path, "rb") as handle:
        return json.load(handle)


def destination_branch(row):
    to_ref = (row.get("planned") or {}).get("to_ref") or ""
    destination = to_ref.split(":", 1)[-1]
    return destination[len("refs/heads/"):] if destination.startswith("refs/heads/") else None


def git(repo, *arguments):
    result = subprocess.run(["git", "-C", str(repo), *arguments], capture_output=True, text=True)
    return result.returncode, result.stdout.strip()


def ssh_form(url):
    match = re.match(r"^https://github\.com/([^/]+)/([^/]+\.git)$", url)
    return f"git@github.com:{match.group(1)}/{match.group(2)}" if match else None


def command_gate(args):
    document = load_json(args.dryrun)
    report = Report()
    meta = document.get("meta") or {}
    rows = document.get("members") or []
    paths = [row.get("member_path") for row in rows]
    report.check(meta.get("action") == "Push", f"meta.action is Push ({meta.get('action')})")
    report.check(not document.get("errors"), f"no top-level errors ({len(document.get('errors') or [])})")
    report.check("." in paths, "the root row is present")
    report.check(len(paths) == len(set(paths)), "row paths are distinct")
    for wanted in args.planned:
        report.check(wanted in paths, f"{wanted} is a row")
    for row in rows:
        path = row.get("member_path")
        planned = row.get("planned") or {}
        status, action, message = row.get("status"), planned.get("action"), planned.get("message") or ""
        label = f"{path}: {status} {action} {message!r}"
        if row.get("error"):
            report.check(False, f"{label} error={row['error']}")
            continue
        if path in args.planned:
            report.check(
                status == "Planned" and action == "Push" and message.startswith("push to "),
                f"{label} (named for publication: must be Planned, Push)",
            )
            continue
        match = UP_TO_DATE.match(message)
        branch = destination_branch(row)
        report.check(
            status == "Noop"
            and action == "Noop"
            and match is not None
            and match.group("remote") == "origin"
            and branch is not None
            and match.group("branch") == branch
            and not BEHIND.match(message),
            f"{label} (must be Noop, up to date with origin/{branch})",
        )
    print(f"  rows={len(rows)} (N={len([p for p in paths if p != '.'])} members + root)")
    return report.finish("gate")


def command_precheck(args):
    workspace = Path(args.workspace).expanduser().resolve()
    document = load_json(args.dryrun)
    report = Report()
    heads = {}
    for row in document.get("members") or []:
        path = row.get("member_path")
        repo = workspace / path
        print(f"{path}:")
        code, branch = git(repo, "symbolic-ref", "--short", "-q", "HEAD")
        if not report.check(code == 0 and bool(branch), f"on a branch ({branch or 'detached HEAD'})"):
            continue
        _, head = git(repo, "rev-parse", "HEAD")
        heads[path] = head
        code, tracking = git(repo, "rev-parse", "-q", "--verify", f"refs/remotes/origin/{branch}")
        if not report.check(code == 0, f"refs/remotes/origin/{branch} exists"):
            continue
        if path in args.ahead:
            ancestor, _ = git(repo, "merge-base", "--is-ancestor", tracking, head)
            _, count = git(repo, "rev-list", "--count", f"{tracking}..{head}")
            report.check(
                ancestor == 0 and head != tracking,
                f"HEAD {head[:12]} is a fast-forward {count} commit(s) ahead of origin/{branch} {tracking[:12]}",
            )
        else:
            report.check(head == tracking, f"HEAD {head[:12]} equals origin/{branch} {tracking[:12]}")
        _, remotes = git(repo, "remote")
        report.check(remotes.split() == ["origin"], f"remotes are exactly origin ({' '.join(remotes.split())})")
        _, fetch = git(repo, "config", "--get-all", "remote.origin.fetch")
        report.check(fetch.splitlines() == [STANDARD_FETCH], f"fetch refspec {' '.join(fetch.splitlines())}")
        _, url = git(repo, "config", "--get", "remote.origin.url")
        if args.scheme == "https":
            report.check(re.match(r"^https://github\.com/owebeeone/[^/]+\.git$", url) is not None, f"url {url}")
        elif args.scheme == "ssh":
            report.check(re.match(r"^git@github\.com:owebeeone/[^/]+\.git$", url) is not None, f"url {url}")
        else:
            print(f"  [INFO] url {url}")
        _, pushurl = git(repo, "config", "--get-all", "remote.origin.pushurl")
        if path in args.ssh_push:
            wanted = ssh_form(url)
            report.check(wanted is not None and pushurl == wanted, f"pushurl {pushurl or '(none)'} is the SSH form {wanted}")
        else:
            report.check(pushurl == "", f"no pushurl ({pushurl or 'none'})")
        _, dirty = git(repo, "status", "--porcelain", "--untracked-files=no")
        report.check(dirty == "", "no staged or modified tracked files")
    scheme_file = workspace / ".gwz" / "url-scheme.yml"
    if args.scheme == "https":
        text = scheme_file.read_text() if scheme_file.exists() else ""
        report.check("scheme: https" in text, f".gwz/url-scheme.yml records https ({'present' if text else 'missing'})")
    elif args.scheme == "ssh":
        report.check(not scheme_file.exists(), ".gwz/url-scheme.yml is absent")
    if "." in args.ahead:
        _, lock = git(workspace, "show", "HEAD:gwz.conf/gwz.lock.yml")
        for path in args.ahead:
            if path != "." and path in heads:
                report.check(heads[path] in lock, f"root HEAD's lock names {path} HEAD {heads[path][:12]}")
    return report.finish("precheck")


def relative(path_text, base):
    return os.path.relpath(os.path.normpath(path_text), os.path.normpath(base))


def load_run(prefix):
    document = load_json(f"{prefix}.stdout")
    summary = load_json(f"{prefix}.summary.json")
    meta = document.get("meta") or {}
    transport = meta.get("transport") or []
    base = summary.get("cwd", ".")
    gwz_name = Path(summary["argv"][0]).name
    lsof_gwz, lsof_other = {}, {}
    for key, count in summary.get("sessions_by_command_and_port", {}).items():
        command, _, port = key.rpartition(":")
        target = lsof_gwz if command == gwz_name else lsof_other
        target[port] = target.get(port, 0) + count
    connect_log = summary.get("connect_log")
    connect = connect_log.get("tcp_attempts_by_port") if connect_log else None
    per_path = {}
    for item in transport:
        per_path.setdefault(relative(item.get("repository_path", ""), base), []).append(item)
    return document, summary, meta, transport, lsof_gwz, lsof_other, connect, per_path


def sessions_text(port, lsof_gwz, connect):
    observed = lsof_gwz.get(port, 0)
    if connect is None:
        return f"{observed}"
    return f"{connect.get(port, 0)} (lsof {observed})"


def credential_label(item):
    method = item.get("credential_method")
    if method == "unknown" and item.get("authenticated") is None:
        return "anonymous"
    return f"{method}{'' if item.get('authenticated') else ' (not authenticated)'}"


def accounts_text(transport):
    counts = {}
    for item in transport:
        key = f"{'push' if item.get('operation') == 'push' else 'read'} {credential_label(item)}"
        counts[key] = counts.get(key, 0) + 1
    return "; ".join(f"{key} x{count}" for key, count in sorted(counts.items())) or "no transport"


def command_result(args):
    document, summary, meta, transport, lsof_gwz, lsof_other, connect, per_path = load_run(args.run_prefix)
    rows = document.get("members") or []
    report = Report()
    print(f"run: {' '.join(summary['argv'])}")
    print(f"cwd: {summary.get('cwd')}   SSH_AUTH_SOCK: {summary['environment']['SSH_AUTH_SOCK']}")
    print(
        f"started {summary['started']}   wall {summary['wall_seconds']} s   exit {summary['exit_code']}"
        f"   aggregate {meta.get('aggregate_status')}"
    )
    for row in rows:
        planned, error = row.get("planned") or {}, row.get("error") or {}
        print(
            f"  row {row.get('member_path')}: {row.get('status')} {planned.get('message') or ''}"
            f" {error.get('code') or ''} {error.get('message') or ''}".rstrip()
        )
    for error in document.get("errors") or []:
        print(f"  error {error.get('member_path')}: {error.get('code')} {error.get('message')}")
    for path in sorted(per_path):
        print(f"  transport {path}: {', '.join(item.get('operation') + ' ' + credential_label(item) for item in per_path[path])}")
    print(f"  accounts: {accounts_text(transport)}")
    print(f"  sessions by port, {summary['argv'][0]} (lsof): {json.dumps(lsof_gwz)}   order: {summary.get('first_seen_order')}")
    if connect is not None:
        log = summary["connect_log"]
        print(f"  connect() attempts by port: {json.dumps(connect)}   by address: {json.dumps(log.get('tcp_attempts_by_address', {}))}")
        for address, count in log.get("unix_connects", {}).items():
            print(f"  unix socket connect: {address} x{count}")
        for spawn in log.get("spawns", []):
            print(f"  spawned: {spawn.get('argv')}")
        for line in log.get("tcp_failures", []):
            print(f"  failed connect: {line}")
    if lsof_other:
        print(f"  [WARN] TCP sessions from helper processes (not gwz): {json.dumps(lsof_other)}")
    print(f"  processes: {' '.join(summary.get('processes', []))}")

    report.check(summary["exit_code"] == args.expect_exit, f"exit code {summary['exit_code']} == {args.expect_exit}")
    report.check(bool(summary.get("stdout_is_json")), "stdout is one JSON document")
    report.check(not document.get("errors"), f"no top-level errors ({len(document.get('errors') or [])})")
    if args.expect_aggregate:
        report.check(
            meta.get("aggregate_status") == args.expect_aggregate,
            f"aggregate {meta.get('aggregate_status')} == {args.expect_aggregate}",
        )
    if args.expect_rows is not None:
        report.check(len(rows) == args.expect_rows, f"{len(rows)} rows == {args.expect_rows}")

    def port_check(port, expected):
        observed = lsof_gwz.get(port, 0)
        if connect is None:
            report.check(observed == expected, f"port {port}: {observed} sessions (lsof) == {expected}")
            return
        attempted = connect.get(port, 0)
        report.check(observed <= attempted, f"port {port}: lsof saw {observed}, not more than connect() {attempted}")
        if observed < attempted:
            print(f"  [WARN] port {port}: polling missed {attempted - observed} short connection(s); using connect()")
        report.check(attempted == expected, f"port {port}: {attempted} sessions (connect()) == {expected}")

    if args.expect_443 is not None:
        port_check("443", args.expect_443)
    if args.expect_22 is not None:
        port_check("22", args.expect_22)
    other_lsof = sum(count for port, count in lsof_gwz.items() if port not in ("22", "443"))
    other_connect = sum(count for port, count in (connect or {}).items() if port not in ("22", "443"))
    report.check(
        other_lsof == 0 and other_connect == 0,
        f"no gwz sessions on other ports (lsof {other_lsof}, connect() {other_connect})",
    )
    pushes = sum(1 for item in transport if item.get("operation") == "push")
    if args.expect_pushes is not None:
        report.check(pushes == args.expect_pushes, f"{pushes} push transport rows == {args.expect_pushes}")
    reads = sum(1 for item in transport if item.get("operation") == "read_advertisement")
    if args.expect_reads is not None:
        report.check(reads == args.expect_reads, f"{reads} read_advertisement transport rows == {args.expect_reads}")
    ok_rows = sorted(row.get("member_path") for row in rows if row.get("status") == "Ok")
    expected_ok = sorted(path for path in args.expect_ok if path != "none")
    if args.expect_ok:
        report.check(ok_rows == expected_ok, f"rows with status Ok {ok_rows} == {expected_ok}")
    if args.expect_no_transport:
        report.check(not transport, f"no meta.transport rows ({len(transport)})")
    if args.expect_reason:
        for row in rows:
            if row.get("member_path") in expected_ok:
                continue
            message = (row.get("planned") or {}).get("message") or ""
            if args.expect_reason == "up-to-date":
                wanted = f"up to date with origin/{destination_branch(row)} as of the last fetch or push"
            else:
                wanted = "already on origin"
            report.check(
                row.get("status") == "Noop" and message == wanted,
                f"{row.get('member_path')}: {row.get('status')} {message!r} == Noop {wanted!r}",
            )
    for path in args.ssh_only:
        items = per_path.get(path, [])
        shape = [(item.get("operation"), item.get("credential_method"), item.get("authenticated")) for item in items]
        report.check(shape == [("read_advertisement", "agent", True)], f"{path}: one read through the agent, authenticated {shape}")
        others = sorted(
            {other for other, entries in per_path.items() for item in entries if item.get("credential_method") == "agent"}
            - {path}
        )
        report.check(not others, f"no other repository used the agent {others}")
    return report.finish("result")


def command_row(args):
    document, summary, meta, transport, lsof_gwz, _lsof_other, connect, _per_path = load_run(args.run_prefix)
    pushes = sum(1 for item in transport if item.get("operation") == "push")
    reads = sum(1 for item in transport if item.get("operation") == "read_advertisement")
    print(
        f"| {args.case} | {sessions_text('443', lsof_gwz, connect)} | {sessions_text('22', lsof_gwz, connect)}"
        f" | {pushes} | {summary['wall_seconds']:.1f} s | {accounts_text(transport)} |  | exit {summary['exit_code']},"
        f" {meta.get('aggregate_status')}, {reads} reads, started {summary['started']} |"
    )
    return 0


class Tee:
    def __init__(self, stream, path):
        self.stream = stream
        self.file = open(path, "a", encoding="utf-8")

    def write(self, text):
        self.stream.write(text)
        self.file.write(text)
        return len(text)

    def flush(self):
        self.stream.flush()
        self.file.flush()


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--log", metavar="FILE", help="also append everything printed to FILE")
    sub = parser.add_subparsers(dest="command", required=True)
    gate = sub.add_parser("gate")
    gate.add_argument("dryrun")
    gate.add_argument("--planned", action="append", default=[])
    precheck = sub.add_parser("precheck")
    precheck.add_argument("workspace")
    precheck.add_argument("dryrun")
    precheck.add_argument("--scheme", choices=["https", "ssh", "any"], required=True)
    precheck.add_argument("--ssh-push", action="append", default=[])
    precheck.add_argument("--ahead", action="append", default=[])
    result = sub.add_parser("result")
    result.add_argument("run_prefix")
    result.add_argument("--expect-exit", type=int, default=0)
    result.add_argument("--expect-aggregate")
    result.add_argument("--expect-rows", type=int)
    result.add_argument("--expect-443", type=int)
    result.add_argument("--expect-22", type=int)
    result.add_argument("--expect-pushes", type=int)
    result.add_argument("--expect-reads", type=int)
    result.add_argument("--expect-ok", action="append", default=[])
    result.add_argument("--expect-reason", choices=["up-to-date", "already"])
    result.add_argument("--expect-no-transport", action="store_true")
    result.add_argument("--ssh-only", action="append", default=[])
    row = sub.add_parser("row")
    row.add_argument("case")
    row.add_argument("run_prefix")
    args = parser.parse_args()
    if args.log:
        sys.stdout = Tee(sys.stdout, args.log)
    handlers = {"gate": command_gate, "precheck": command_precheck, "result": command_result, "row": command_row}
    return handlers[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
GWZ_ACCEPT_GWZCHECK_PY
```

**`probe.py`**, the loopback probe for the self-test:

<!-- local-block: tool-probe -->
```sh
cat > "$TOOLS/probe.py" <<'GWZ_ACCEPT_PROBE_PY'
#!/usr/bin/env python3.13
"""Loopback-only probes for testing sockwatch. Never connects anywhere but 127.0.0.1.

  probe.py listen --hold-ms H --seconds S   print a port, accept connections on
                                            127.0.0.1, close each after H ms
  probe.py client --port P --count N --hold-ms H --gap-ms G
                                            open N connections to 127.0.0.1:P, one
                                            at a time, each held H ms, G ms apart
"""
import argparse
import socket
import sys
import threading
import time


def listen(args):
    server = socket.socket()
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("127.0.0.1", args.port))
    server.listen(128)
    print(server.getsockname()[1], flush=True)
    server.settimeout(0.1)
    deadline = time.monotonic() + args.seconds
    while time.monotonic() < deadline:
        try:
            connection, _ = server.accept()
        except socket.timeout:
            continue
        threading.Timer(args.hold_ms / 1000.0, connection.close).start()


def client(args):
    for _ in range(args.count):
        connection = socket.create_connection(("127.0.0.1", args.port))
        time.sleep(args.hold_ms / 1000.0)
        connection.close()
        time.sleep(args.gap_ms / 1000.0)


def main():
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="mode", required=True)
    server = sub.add_parser("listen")
    server.add_argument("--port", type=int, default=0)
    server.add_argument("--hold-ms", type=float, default=1000)
    server.add_argument("--seconds", type=float, default=60)
    user = sub.add_parser("client")
    user.add_argument("--port", type=int, required=True)
    user.add_argument("--count", type=int, default=10)
    user.add_argument("--hold-ms", type=float, default=100)
    user.add_argument("--gap-ms", type=float, default=100)
    args = parser.parse_args()
    if args.mode == "listen":
        listen(args)
    else:
        client(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
GWZ_ACCEPT_PROBE_PY
```

**Build and record the hashes:**

<!-- local-block: tool-build -->
```sh
cc -dynamiclib -O2 -Wall -Wextra -o "$TOOLS/connlog.dylib" "$TOOLS/connlog.c"
shasum -a 256 "$TOOLS/sockwatch.py" "$TOOLS/connlog.c" "$TOOLS/connlog.dylib" "$TOOLS/gwzcheck.py" "$TOOLS/probe.py" | tee "$EV/00-preconditions/tools.sha256"
python3.13 "$TOOLS/gwzcheck.py" --help | head -3
```

**Expected:** the build prints no warnings, then five hashes and the gwzcheck usage
line.

### 2.5 Self-test on loopback (local, about a minute)

Two runs against a local listener, whose connections close after 5 s:
- 10 connections held for 200 ms;
- 10 connections held for 5 ms.

<!-- local-block: selftest -->
```sh
python3.13 "$TOOLS/probe.py" listen --hold-ms 5000 --seconds 45 > "$EV/selftest/port.txt" &
sleep 1
export PROBE_PORT="$(head -1 "$EV/selftest/port.txt")"
sw "$EV/selftest/hold200-1" -- python3.13 "$TOOLS/probe.py" client --port "$PROBE_PORT" --count 10 --hold-ms 200 --gap-ms 50
sw "$EV/selftest/hold5-1" -- python3.13 "$TOOLS/probe.py" client --port "$PROBE_PORT" --count 10 --hold-ms 5 --gap-ms 50
jq -c '{sessions_by_remote_port, connect: .connect_log.tcp_attempts_by_port, poll_gap_ms}' "$EV/selftest/hold200-1.summary.json" "$EV/selftest/hold5-1.summary.json"
```

**Expected:**
- the 200 ms run shows 10 sessions in both lsof and `connect()`;
- the 5 ms run shows 0 to 2 in lsof and 10 in `connect()`;
- the median poll gap is 30 to 60 ms; it grows when the Mac is busy.

**If `connect()` is missing or 0, stop.** The interposer did not load. Check the
`codesign` flags in P1 and that `python3.13` is Homebrew's.

## 3. Cases

### 3.0 The gate every measured push uses

Plan step 4.2 requires a default `gwz --json push --dry-run` before each
socket-polled push. A `--check-remotes` dry run skips classification, so it cannot
confirm anything. Each case therefore runs this sequence:

1. **`sw ... dryrun-1 -- "$GA" --json push --dry-run`.**
2. **`chk result` on the dry run.** Exit 0, no sessions on port 443 or 22 by either
   observer, no transport rows, `N+1` rows.
3. **`chk gate`.**
   - Every row is `Noop` with `planned.action` `Noop`.
   - Every message is exactly `up to date with origin/<branch> as of the last fetch or
     push`, where `<branch>` is the row's destination branch.
   - A `behind origin/<branch> ...` row fails.
   - In case 6 only, the named member and the root must instead be `Planned`/`Push`
     with `push to origin`.
4. **`chk precheck` on the workspace.** It checks the conditions that make the
   expected counts valid. For every repository in the dry run:
   - it is on a branch;
   - `refs/remotes/origin/<branch>` equals `HEAD`, or in case 6 is a fast-forward
     behind it;
   - its remotes are exactly `origin`;
   - the fetch refspec is exactly `+refs/heads/*:refs/remotes/origin/*`;
   - the URL scheme is the expected one;
   - there is no `pushurl`, except the case-5 member;
   - there are no staged or modified tracked files;
   - for https, `.gwz/url-scheme.yml` records `https`, and for SSH it is absent.

   Why the refspec matters: plan §3.5 uses a last-known ref only with that forced
   refspec. Without it every repository would be contacted, and case 1 would open
   sockets.
5. **`gate.pass`.** Only when all three checks pass does the chain write
   `evidence/caseN/gate.pass`. The measured push line runs only if that file exists.
   All check output is appended to `evidence/caseN/checks.txt`.

Commands that change the repositories are never chained to a gate. They are separate
lines, so a failure stops you in plain sight.

### Case 1: fresh https clone, whole push, no sockets

**Purpose:** plan goal 2. A default push with nothing to publish makes no
connections. Before (step 0.2): 34 connections, 42.4 s.

**Setup: https clone (contacts GitHub, reads only).** No SSH agent is given, so any
SSH attempt would show on port 22 and fail.

```sh
cd "$ACC"
sw --unset-env SSH_AUTH_SOCK "$EV/case1/clone-1" -- "$GA" --json clone --url-scheme https https://github.com/owebeeone/gwz-dev.git "$WS_HTTPS"
chk --log "$EV/case1/checks.txt" result "$EV/case1/clone-1" --expect-22 0
jq -c '.members[] | {member_path, status, scheme: .url_resolution.scheme, source: .url_resolution.source, derived: .url_resolution.derived}' "$EV/case1/clone-1.stdout"
cat "$WS_HTTPS/.gwz/url-scheme.yml"
git -C "$WS_HTTPS" rev-parse HEAD | tee "$EV/case1/root-commit.txt"
```

**Expected:**
- exit 0;
- every member `Ok` with `https`, `request`, `derived: true`;
- `url-scheme.yml` says `scheme: https`;
- no port-22 sessions;
- the port-443 count prints; step 0.2's clone made 9 connections in 5.7 s.

**Gate, which also measures `N`:**

```sh
cd "$WS_HTTPS"
rm -f "$EV/case1/gate.pass"
sw --unset-env SSH_AUTH_SOCK "$EV/case1/dryrun-1" -- "$GA" --json push --dry-run
jq '[.members[] | select(.member_path != ".")] | length' "$EV/case1/dryrun-1.stdout" | tee "$EV/N.txt"
export N="$(cat "$EV/N.txt")"
if [ "$N" = 8 ]; then echo "N=8 as on 2026-09-16"; else echo "NOTE: N=$N, not 8; the counts below use N, record it"; fi
chk --log "$EV/case1/checks.txt" result "$EV/case1/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case1/checks.txt" gate "$EV/case1/dryrun-1.stdout" && chk --log "$EV/case1/checks.txt" precheck "$WS_HTTPS" "$EV/case1/dryrun-1.stdout" --scheme https && touch "$EV/case1/gate.pass"
ls "$EV/case1/gate.pass"
```

**Measured push:**

```sh
if [ -f "$EV/case1/gate.pass" ]; then sw --unset-env SSH_AUTH_SOCK "$EV/case1/push-1" -- "$GA" --json push; else echo "case 1: the gate has not passed; push not run"; fi
chk --log "$EV/case1/checks.txt" result "$EV/case1/push-1" --expect-443 0 --expect-22 0 --expect-pushes 0 --expect-reads 0 --expect-ok none --expect-no-transport --expect-aggregate Noop --expect-rows "$((N+1))" --expect-reason up-to-date
chk row 1 "$EV/case1/push-1" | tee -a "$EV/results-rows.md"
```

**Pass (plan):** no sockets. That means port 443: 0 and port 22: 0, from both
observers.

**Supporting:**
- exit 0;
- aggregate `Noop`;
- every row `Noop`, `up to date with origin/main as of the last fetch or push`;
- no `meta.transport`.

### Case 2: same clone, `--check-remotes`

**Purpose:** plan goal 3. With the option, each repository is read once and nothing
is pushed. Before: 18 port-443 and 16 port-22 sessions for the same workspace.

**Gate:**

```sh
cd "$WS_HTTPS"
rm -f "$EV/case2/gate.pass"
sw --unset-env SSH_AUTH_SOCK "$EV/case2/dryrun-1" -- "$GA" --json push --dry-run
chk --log "$EV/case2/checks.txt" result "$EV/case2/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case2/checks.txt" gate "$EV/case2/dryrun-1.stdout" && chk --log "$EV/case2/checks.txt" precheck "$WS_HTTPS" "$EV/case2/dryrun-1.stdout" --scheme https && touch "$EV/case2/gate.pass"
```

**Measured push (contacts GitHub, reads only):**

```sh
if [ -f "$EV/case2/gate.pass" ]; then sw --unset-env SSH_AUTH_SOCK "$EV/case2/push-1" -- "$GA" --json push --check-remotes; else echo "case 2: the gate has not passed; push not run"; fi
chk --log "$EV/case2/checks.txt" result "$EV/case2/push-1" --expect-443 "$((N+1))" --expect-22 0 --expect-pushes 0 --expect-reads "$((N+1))" --expect-ok none --expect-aggregate Noop --expect-rows "$((N+1))" --expect-reason already
chk row 2 "$EV/case2/push-1" | tee -a "$EV/results-rows.md"
```

**Pass (plan):** N+1 port-443 sessions (9), one read per repository, no port-22
sessions and no pushes.

**Supporting:**
- `N+1` `read_advertisement` rows and no `push` rows;
- every row `Noop`, `already on origin`;
- aggregate `Noop`.

**Accounts:**
- 8 reads anonymous (public repositories);
- 1 read through the helper, for private gwz-core-evidence. The `spawned:` lines name
  the helper that ran.

**Why this can still publish.** `--check-remotes` would push a repository whose
remote lacks its commit. After a fresh clone that cannot happen unless someone
rewinds a remote meanwhile (P2 declaration).

### Case 3: same clone, root-only push with `--check-remotes`

**Purpose:** proving an unchanged root reads its dependencies over https only.
Before: a root-only push used 2N+2 sessions, 2N of them SSH in an https clone (§2.3).

**Gate:**

```sh
cd "$WS_HTTPS"
rm -f "$EV/case3/gate.pass"
sw --unset-env SSH_AUTH_SOCK "$EV/case3/dryrun-1" -- "$GA" --json push --dry-run
chk --log "$EV/case3/checks.txt" result "$EV/case3/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case3/checks.txt" gate "$EV/case3/dryrun-1.stdout" && chk --log "$EV/case3/checks.txt" precheck "$WS_HTTPS" "$EV/case3/dryrun-1.stdout" --scheme https && touch "$EV/case3/gate.pass"
```

**Measured push (contacts GitHub, reads only):**

```sh
if [ -f "$EV/case3/gate.pass" ]; then sw --unset-env SSH_AUTH_SOCK "$EV/case3/push-1" -- "$GA" --json push --check-remotes --target @root; else echo "case 3: the gate has not passed; push not run"; fi
chk --log "$EV/case3/checks.txt" result "$EV/case3/push-1" --expect-22 0 --expect-pushes 0 --expect-ok none --expect-aggregate Noop --expect-rows 1 --expect-reason already
chk --log "$EV/case3/checks.txt" result "$EV/case3/push-1" --expect-443 "$((N+1))" --expect-reads "$((N+1))"
chk row 3 "$EV/case3/push-1" | tee -a "$EV/results-rows.md"
```

**Pass (plan):** port-443 sessions only, meaning port 22 is 0 (first `chk` line).

**Derived, not a plan criterion (second `chk` line):** N+1 port-443 sessions (9). That
is one root read plus one read per dependency (rule 3).
- The local rehearsal at N=2 produced exactly 3 reads.
- A different count with port 22 at 0 still passes the plan, but record the
  difference.

**Supporting:** one row, the root, `Noop`, `already on origin`.

### Case 4: fresh SSH clone of the published workspace, `--check-remotes`

**Purpose:** the SSH workspace reads each repository once over SSH. Before:
1.0.12's push of gwz-dev made 26 SSH connections in 44 s; `c9c7a98` made 18
(§2.2 and §2.3).

**Setup: SSH clone (contacts GitHub over SSH as owebeeone, reads only).**

```sh
cd "$ACC"
sw --set-env SSH_AUTH_SOCK="$OWB_SOCK" "$EV/case4/clone-1" -- "$GA" --json clone git@github.com:owebeeone/gwz-dev.git "$WS_SSH"
chk --log "$EV/case4/checks.txt" result "$EV/case4/clone-1" --expect-443 0
printf '%s\n' "$WS_SSH" | tee "$EV/case4/clone-path.txt"
git -C "$WS_SSH" rev-parse HEAD | tee "$EV/case4/root-commit.txt"
diff "$EV/case1/root-commit.txt" "$EV/case4/root-commit.txt" && echo "same root commit as the https clone"
```

**Expected:**
- exit 0;
- no port-443 sessions;
- the clone path and root commit recorded, as the plan requires;
- the same root commit as case 1.

**Gate:**

```sh
cd "$WS_SSH"
rm -f "$EV/case4/gate.pass"
sw --set-env SSH_AUTH_SOCK="$OWB_SOCK" "$EV/case4/dryrun-1" -- "$GA" --json push --dry-run
chk --log "$EV/case4/checks.txt" result "$EV/case4/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case4/checks.txt" gate "$EV/case4/dryrun-1.stdout" && chk --log "$EV/case4/checks.txt" precheck "$WS_SSH" "$EV/case4/dryrun-1.stdout" --scheme ssh && touch "$EV/case4/gate.pass"
```

**Measured push (contacts GitHub over SSH, reads only):**

```sh
if [ -f "$EV/case4/gate.pass" ]; then sw --set-env SSH_AUTH_SOCK="$OWB_SOCK" "$EV/case4/push-1" -- "$GA" --json push --check-remotes; else echo "case 4: the gate has not passed; push not run"; fi
chk --log "$EV/case4/checks.txt" result "$EV/case4/push-1" --expect-22 "$((N+1))" --expect-443 0 --expect-pushes 0 --expect-reads "$((N+1))" --expect-ok none --expect-aggregate Noop --expect-rows "$((N+1))" --expect-reason already
chk row 4 "$EV/case4/push-1" | tee -a "$EV/results-rows.md"
```

**Pass (plan):** N+1 port-22 sessions (9) and no pushes.

**Supporting:**
- no port-443 sessions;
- every transport row `read_advertisement agent`, authenticated;
- the `unix socket connect` lines name `$OWB_SOCK`, the owebeeone agent.

### Case 5: a member that fetches over https and pushes over SSH, `--check-remotes`

**Purpose:** plan §3.2 rule 1 and D2. The member's read URL is its SSH push
destination. The member read and the root's dependency read share that one read, so
the member is read over SSH only.

**Setup: a `pushurl` on one member of the https clone (local configuration only).**

```sh
git -C "$WS_HTTPS/$CASE5_MEMBER" config --get remote.origin.url | sed -E 's#^https://github\.com/#git@github.com:#' | tee "$EV/case5/pushurl.txt"
git -C "$WS_HTTPS/$CASE5_MEMBER" config remote.origin.pushurl "$(cat "$EV/case5/pushurl.txt")"
git -C "$WS_HTTPS/$CASE5_MEMBER" config --get-regexp '^remote\.origin\.' | tee "$EV/case5/remote-config.txt"
```

**Expected:** `url` is `https://github.com/owebeeone/taut-shape-py.git`, `pushurl` is
`git@github.com:owebeeone/taut-shape-py.git`, and `fetch` is the standard refspec.
These are the same repository by plan §3.1, a scheme-only difference on a known
host. So the last-known ref still applies, and the member must still dry-run as up to
date.

**Fetch, as the plan requires (contacts GitHub over https, reads only):**

```sh
cd "$WS_HTTPS"
sw --unset-env SSH_AUTH_SOCK "$EV/case5/fetch-1" -- "$GA" --json --sync fetch-only pull --head
chk --log "$EV/case5/checks.txt" result "$EV/case5/fetch-1" --expect-22 0
```

This fetches the root and every member through their fetch URLs, and moves no
branch. It was verified locally: a fetch-only pull updates the root's tracking ref
too, though it lists only member rows. With no agent, a fetch that tried the SSH push
URL would show on port 22 and fail.

**Gate, with `pushurl` allowed on that member only:**

```sh
rm -f "$EV/case5/gate.pass"
sw --unset-env SSH_AUTH_SOCK "$EV/case5/dryrun-1" -- "$GA" --json push --dry-run
chk --log "$EV/case5/checks.txt" result "$EV/case5/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case5/checks.txt" gate "$EV/case5/dryrun-1.stdout" && chk --log "$EV/case5/checks.txt" precheck "$WS_HTTPS" "$EV/case5/dryrun-1.stdout" --scheme https --ssh-push "$CASE5_MEMBER" && touch "$EV/case5/gate.pass"
```

**If only the case-5 member fails the gate** (contacted rather than up to date): the
Git2 last-known-ref push-URL condition did not treat the SSH push URL as the same
repository. That is a finding. Record it, and stop case 5.

**Measured push (contacts GitHub: that member over SSH as owebeeone, the rest over
https; reads only):**

```sh
if [ -f "$EV/case5/gate.pass" ]; then sw --set-env SSH_AUTH_SOCK="$OWB_SOCK" "$EV/case5/push-1" -- "$GA" --json push --check-remotes; else echo "case 5: the gate has not passed; push not run"; fi
chk --log "$EV/case5/checks.txt" result "$EV/case5/push-1" --expect-22 1 --expect-443 "$N" --expect-pushes 0 --expect-reads "$((N+1))" --expect-ok none --expect-aggregate Noop --expect-rows "$((N+1))" --expect-reason already --ssh-only "$CASE5_MEMBER"
chk row 5 "$EV/case5/push-1" | tee -a "$EV/results-rows.md"
```

**Pass (plan):** that member is read over SSH only. Concretely, all of these hold:
- **Sockets.** Exactly 1 port-22 session, and N port-443 sessions (8): the root and
  the other N-1 members.
  - The member has no port-443 session.
  - If the dependency proof also read the member's https URL, port 443 would show
    N+1, and this fails.
- **Transport rows.** The member has exactly one row, `read_advertisement agent`,
  authenticated (`--ssh-only`), and no other repository used the agent.
- **Total reads.** `N+1` reads and no pushes.
- **Agent socket.** The `unix socket connect` line names `$OWB_SOCK`.
- **Row.** The member's row is `Noop`, `already on origin`.

**Undo the setup (local configuration only):**

```sh
git -C "$WS_HTTPS/$CASE5_MEMBER" config --unset remote.origin.pushurl
git -C "$WS_HTTPS/$CASE5_MEMBER" config --get-regexp '^remote\.origin\.' | tee "$EV/case5/remote-config-after.txt"
```

**Expected:** only `url` and `fetch` remain.

### Case 6: one member and the root changed, default push (PUBLISHES TO GITHUB)

**Purpose:** plan goal 2 when something changed. Only the changed member and the
root are contacted, and every other root dependency is read once (D10). This case
also makes the first real pack upload over https, open since step 0.2.

**What you name first.** Choose a member, a branch, and a real change you want
published, with its two commit messages. Edit this block, then paste it:

```sh
export PUB_MEMBER=REPLACE-ME
export PUB_BRANCH=main
export PUB_MEMBER_MSG='REPLACE-ME'
export PUB_ROOT_MSG='REPLACE-ME'
```

**Guard (local):**

```sh
for v in "$PUB_MEMBER" "$PUB_BRANCH" "$PUB_MEMBER_MSG" "$PUB_ROOT_MSG"; do case "$v" in *REPLACE-ME*|'') echo "STOP: set PUB_MEMBER, PUB_BRANCH, PUB_MEMBER_MSG and PUB_ROOT_MSG";; esac; done
if [ -d "$WS_HTTPS/$PUB_MEMBER/.git" ] && [ "$PUB_MEMBER" != "." ]; then echo "member $PUB_MEMBER found"; else echo "STOP: $WS_HTTPS/$PUB_MEMBER is not a member repository"; fi
if [ "$(git -C "$WS_HTTPS/$PUB_MEMBER" symbolic-ref --short HEAD)" = "$PUB_BRANCH" ]; then echo "on $PUB_BRANCH"; else echo "STOP: $PUB_MEMBER is not on $PUB_BRANCH"; fi
git -C "$WS_HTTPS/$CASE5_MEMBER" config --get remote.origin.pushurl && echo "STOP: undo case 5 first" || echo "no case-5 pushurl left"
```

**Expected:** no `STOP` lines.

**Fetch and baseline gate, before the change (contacts GitHub over https, reads
only):**

```sh
cd "$WS_HTTPS"
sw --unset-env SSH_AUTH_SOCK "$EV/case6/fetch-1" -- "$GA" --json --sync fetch-only pull --head
chk --log "$EV/case6/checks.txt" result "$EV/case6/fetch-1" --expect-22 0
sw --unset-env SSH_AUTH_SOCK "$EV/case6/dryrun-0" -- "$GA" --json push --dry-run
chk --log "$EV/case6/checks.txt" result "$EV/case6/dryrun-0" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case6/checks.txt" gate "$EV/case6/dryrun-0.stdout" && chk --log "$EV/case6/checks.txt" precheck "$WS_HTTPS" "$EV/case6/dryrun-0.stdout" --scheme https && echo "baseline clean"
```

**Expected:** `baseline clean`.
- This proves that after the fetch, nothing but your change will be planned.
- A `behind` row means GitHub moved. Stop, and find out why.

**Make the change (local).** Edit the files under `$WS_HTTPS/$PUB_MEMBER` now. Then
stage and commit through gwz-alpha, member first, then the root lock:

```sh
cd "$WS_HTTPS"
git -C "$WS_HTTPS/$PUB_MEMBER" status --short
"$GA" add -A --target "$PUB_MEMBER"
"$GA" commit --target "$PUB_MEMBER" -m "$PUB_MEMBER_MSG"
"$GA" commit --target @root -m "$PUB_ROOT_MSG"
git -C "$WS_HTTPS/$PUB_MEMBER" log --stat "origin/$PUB_BRANCH..HEAD" | tee "$EV/case6/to-publish-member.txt"
git -C "$WS_HTTPS" log --stat "origin/$(git -C "$WS_HTTPS" symbolic-ref --short HEAD)..HEAD" | tee "$EV/case6/to-publish-root.txt"
```

**Expected:**
- one member commit;
- one root commit changing `gwz.conf/gwz.lock.yml`, plus gwz's snapshot and marker
  files.

Verified locally: `commit --target <member>` alone leaves the lock staged in the root,
and `commit --target @root` records it. The `git status` line shows what `add -A`
will stage. Review it before the `add` line.

**Gate after the change.** Only the named member and the root may be planned.

```sh
rm -f "$EV/case6/gate.pass"
sw --unset-env SSH_AUTH_SOCK "$EV/case6/dryrun-1" -- "$GA" --json push --dry-run
chk --log "$EV/case6/checks.txt" result "$EV/case6/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" && chk --log "$EV/case6/checks.txt" gate "$EV/case6/dryrun-1.stdout" --planned "$PUB_MEMBER" --planned . && chk --log "$EV/case6/checks.txt" precheck "$WS_HTTPS" "$EV/case6/dryrun-1.stdout" --scheme https --ahead "$PUB_MEMBER" --ahead . && touch "$EV/case6/gate.pass"
```

The precheck also confirms two things:
- the root's committed lock names the member's new `HEAD`, so the root proof needs
  that commit, and the member's accepted push proves it (D8);
- the member and the root are fast-forwards of their tracking refs.

**Confirm and publish. Paste this line by itself;** its prompt would swallow a
following pasted line.

```sh
if [ -f "$EV/case6/gate.pass" ]; then python3.13 -c 'import sys; sys.exit(0 if input("This pushes the member and root commits listed above to GitHub. Type PUBLISH: ").strip() == "PUBLISH" else 1)' && sw --unset-env SSH_AUTH_SOCK "$EV/case6/push-1" -- "$GA" --json push; else echo "case 6: the gate has not passed; push not run"; fi
```

**Result:**

```sh
chk --log "$EV/case6/checks.txt" result "$EV/case6/push-1" --expect-443 "$((N+3))" --expect-22 0 --expect-pushes 2 --expect-reads "$((N+1))" --expect-ok "$PUB_MEMBER" --expect-ok . --expect-aggregate Ok --expect-rows "$((N+1))" --expect-reason up-to-date
chk row 6 "$EV/case6/push-1" | tee -a "$EV/results-rows.md"
```

**Pass (plan):** N+3 sessions (11): the member's read and push, the root's read and
push, and one read for each of the other N-1 dependencies.

**Supporting:**
- all 11 sessions on port 443, none on port 22;
- 2 `push` rows through the helper;
- `N+1` reads;
- the member and root rows `Ok`, and every other row `Noop`,
  `up to date with origin/main ...`.

**Verify on GitHub (contacts GitHub, read-only):**

```sh
for p in "$PUB_MEMBER" .; do u="$(git -C "$WS_HTTPS/$p" config --get remote.origin.url)"; b="$(git -C "$WS_HTTPS/$p" symbolic-ref --short HEAD)"; l="$(git -C "$WS_HTTPS/$p" rev-parse HEAD)"; r="$(git ls-remote "$u" "refs/heads/$b" </dev/null | cut -f1)"; if [ -n "$r" ] && [ "$l" = "$r" ]; then v=same; else v=DIFFERENT; fi; printf '%-18s %-8s local %s github %s %s\n' "$p" "$b" "$l" "${r:-none}" "$v"; done | tee "$EV/case6/github-heads.txt"
for p in "$PUB_MEMBER" .; do repo="$(basename "$(git -C "$WS_HTTPS/$p" config --get remote.origin.url)" .git)"; echo "== $repo"; gh api "repos/owebeeone/$repo/activity?per_page=3" --jq '.[] | [.timestamp, .activity_type, .actor.login, .ref, .after] | @tsv'; done | tee "$EV/case6/github-activity.txt"
```

**Expected:**
- both lines `same`;
- the newest activity for each repository is a `push` to `refs/heads/<branch>` by
  `owebeeone`, whose `after` equals the local head. This is the account the https
  pushes used.

**If the activity endpoint errors,** use the events feed. It can lag by minutes:

```sh
for p in "$PUB_MEMBER" .; do repo="$(basename "$(git -C "$WS_HTTPS/$p" config --get remote.origin.url)" .git)"; echo "== $repo"; gh api "repos/owebeeone/$repo/events?per_page=10" --jq '.[] | select(.type == "PushEvent") | [.created_at, .actor.login, .payload.ref, .payload.head] | @tsv'; done | tee "$EV/case6/github-events.txt"
```

**After publication (local):** the tracking refs now match, so a default dry run is
clean again.

```sh
sw --unset-env SSH_AUTH_SOCK "$EV/case6/dryrun-2" -- "$GA" --json push --dry-run
chk --log "$EV/case6/checks.txt" gate "$EV/case6/dryrun-2.stdout"
```

**Follow-up outside this runbook.** gwz-dev is now behind GitHub for `$PUB_MEMBER`
and the root. Bring it up to date with your usual gwz 1.0.12 flow before committing
the results note there.

### Case 7 (optional): a tag push (PUBLISHES A TAG TO GITHUB)

**Purpose:** record what root tag publication costs after this plan.
- Plan step 2.2: its reads use the https read URLs.
- Step 3.3: tag keeps no read dedup, which is a stated non-goal.
- Step 2.2 as landed: a root tag push needs `--target @root` or `--all`. This runbook
  uses `--target @root` only.

Run this case **only for a tag you want published.**

**What you name:**

```sh
export PUB_TAG=REPLACE-ME
export PUB_TAG_MSG='REPLACE-ME'
```

**Guard, fetch and gate.** `git ls-remote` contacts GitHub, read-only.

```sh
case "$PUB_TAG$PUB_TAG_MSG" in *REPLACE-ME*|'') echo "STOP: set PUB_TAG and PUB_TAG_MSG";; *) echo "tag $PUB_TAG";; esac
cd "$WS_HTTPS"
sw --unset-env SSH_AUTH_SOCK "$EV/case7/fetch-1" -- "$GA" --json --sync fetch-only pull --head
git ls-remote --tags "$(git -C "$WS_HTTPS" config --get remote.origin.url)" </dev/null | awk -v t="refs/tags/$PUB_TAG" '$2 == t || $2 == t "^{}"' | tee "$EV/case7/github-tag-before.txt"
rm -f "$EV/case7/gate.pass"
sw --unset-env SSH_AUTH_SOCK "$EV/case7/dryrun-1" -- "$GA" --json push --dry-run
chk --log "$EV/case7/checks.txt" result "$EV/case7/dryrun-1" --expect-443 0 --expect-22 0 --expect-no-transport --expect-rows "$((N+1))" --expect-reason up-to-date && chk --log "$EV/case7/checks.txt" gate "$EV/case7/dryrun-1.stdout" && chk --log "$EV/case7/checks.txt" precheck "$WS_HTTPS" "$EV/case7/dryrun-1.stdout" --scheme https && [ ! -s "$EV/case7/github-tag-before.txt" ] && [ -z "$(git -C "$WS_HTTPS" rev-parse -q --verify "refs/tags/$PUB_TAG")" ] && touch "$EV/case7/gate.pass"
```

**Expected:**
- `github-tag-before.txt` is empty, since the tag does not exist on GitHub;
- the checks pass;
- `gate.pass` is written.

**Create the tag on the root only (local):**

```sh
"$GA" tag "$PUB_TAG" -m "$PUB_TAG_MSG" --target @root
git -C "$WS_HTTPS" rev-parse "$PUB_TAG" "$PUB_TAG^{commit}" HEAD | tee "$EV/case7/local-tag.txt"
for p in $(jq -r '.members[] | select(.member_path != ".") | .member_path' "$EV/case7/dryrun-1.stdout"); do if git -C "$WS_HTTPS/$p" rev-parse -q --verify "refs/tags/$PUB_TAG" >/dev/null; then echo "UNEXPECTED: $p has $PUB_TAG"; fi; done; echo "member tag check done"
```

**Expected:**
- the tag object, then its commit, which equals `HEAD`;
- no `UNEXPECTED` lines.

For a lightweight tag instead, drop `-m "$PUB_TAG_MSG"`.

**Confirm and publish. Paste this line by itself.** Never run `tag --push` without
the tag name, because that pushes every gwz tag.

```sh
if [ -f "$EV/case7/gate.pass" ]; then python3.13 -c 'import sys; sys.exit(0 if input("This pushes the root tag named above to GitHub. Type PUBLISH: ").strip() == "PUBLISH" else 1)' && sw --unset-env SSH_AUTH_SOCK "$EV/case7/tag-push-1" -- "$GA" --json tag --push "$PUB_TAG" --target @root; else echo "case 7: the gate has not passed; tag push not run"; fi
```

**Result and verification.** `git ls-remote` and `gh api` contact GitHub, read-only.

```sh
chk --log "$EV/case7/checks.txt" result "$EV/case7/tag-push-1" --expect-22 0 --expect-pushes 1 --expect-aggregate Ok --expect-rows 0
chk --log "$EV/case7/checks.txt" result "$EV/case7/tag-push-1" --expect-443 "$((2*N+2))" --expect-reads "$((2*N+1))"
chk row 7 "$EV/case7/tag-push-1" | tee -a "$EV/results-rows.md"
git ls-remote --tags "$(git -C "$WS_HTTPS" config --get remote.origin.url)" </dev/null | awk -v t="refs/tags/$PUB_TAG" '$2 == t || $2 == t "^{}"' | tee "$EV/case7/github-tag-after.txt"
gh api "repos/owebeeone/gwz-dev/git/ref/tags/$PUB_TAG" --jq '.object' | tee -a "$EV/case7/github-tag-after.txt"
```

**Pass:** the plan states no count for case 7. Require:
- exit 0 and aggregate `Ok`;
- one `push` row and no port-22 sessions;
- GitHub's `refs/tags/<tag>` equals the local tag object, and its peeled `^{}` line
  equals `HEAD`.

**Derived from `handle_tag.rs`, recorded rather than required (second `chk` line):**
2N+2 port-443 sessions (18). That is:
- one root read;
- N dependency reads in the preflight;
- N dependency reads in the proof, which gets an empty `ReadPreflight`;
- the tag push.

The local rehearsal at N=2 produced 5 reads and 1 push.

## 4. Results template

Copy the block below into `dev-docs/GwzUrlSchemePushAcceptance-<date>.md`. Then:
1. fill the rows from `$EV/results-rows.md`, the `checks.txt` files and the GitHub
   verification files;
2. commit it as a root commit in gwz-dev with gwz 1.0.12, after the case 6 follow-up.

The "before" numbers are filled in already.

~~~markdown
# GWZ push: acceptance evidence against GitHub

Date: <date>. Plan: `GwzUrlSchemePushPlan.md`, step 4.2. Runbook:
`GwzUrlSchemePushAcceptanceRunbook.md` (draft of 2026-09-16). Evidence:
`~/limbo/accept-<date>/evidence/` (kept locally).

## 1. Binary, workspace and method

- **Binary.** `~/.cargo/bin/gwz`, `gwz 1.0.13`, sha256
  `57edf42e371920aec04f1bc10d4160344876deb061257c19e8c642f4e2a96905`.
  - `--build-info`: CLI revision `0be04bfa09…` (gwz-cli tag `v1.0.13`), core
    1.0.13 from crates.io (gwz-core tag `v1.0.13` = `83626d24…`).
  - **Deviation from step 4.2:** the installed release binary was used, not an
    install into a scratch `--root`. No `target/` build ran against gwz-dev.
- **Published workspace.** gwz-dev root `<case 1 root commit>`, with N = `<N>`
  members. Every local head in gwz-dev equalled GitHub before the run (P2).
- **Clones.**
  - https: `<ACC>/ws-https` (cases 1–3 and 5–7), root `<commit>`;
  - SSH: `<ACC>/ws-ssh` (case 4), root `<commit>`.
- **Method.**
  - Each command ran under sockwatch, which polls `lsof -g <pgid> -iTCP` back to back
    (median gap `<ms>` ms) and keeps JSON, exit code and wall time.
  - A `connect()` log inside gwz-alpha was the event-based count; the lsof count was
    checked against it.
  - Attribution comes from `meta.transport` rows.
  - https cases ran with `SSH_AUTH_SOCK` unset. SSH cases ran with the owebeeone
    agent socket.

## 2. Before (gwz 1.0.12, 2026-09-14: plan §2.2 and step 0.2)

| measurement | result |
|---|---|
| one SSH session (ls-remote) | 2.4–2.6 s |
| one HTTPS session | 0.5–0.6 s (TCP 12 ms, TLS 21 ms) |
| `gwz push` of gwz-dev, 8 members, SSH everywhere | 26 connections (3N+2), 44 s |
| `gwz --json clone --url-scheme https` (step 0.2) | 9 port-443 connections, no SSH, 5.7 s; all 8 members, the private one through a credential helper |
| default dry run in that clone | no connections; every row planned `refs/heads/main:refs/heads/main`, level with `origin/main`, clean |
| `gwz --json push` in that clone, nothing to publish | exit 0 after 42.4 s, every row and the aggregate `ok`; 18 port-443 + 16 port-22 = 34 connections (2N+2 https + 2N SSH), first seen 443×9, 22×8, 443×8, 22×8, 443×1; transport rows: 9 `push` via `helper`, authenticated; 16 SSH reads via `agent`; the private member's read via `helper`; 8 anonymous https reads |
| not established by step 0.2 | which helper answered; a real pack upload over https |

Expected after Phase 3 (plan §2.3, N = 8):

| push | default | `--check-remotes` |
|---|---|---|
| whole workspace, nothing to publish | 0 | N+1 (9) |
| whole workspace, one member and the root changed | N+3 (11) | N+3 (11) |
| root only, root unchanged | 0 | N+1 (9): root read + N dependency reads |

## 3. After (this run)

| case | command | port-443 sessions | port-22 sessions | pushes | wall time | account used | pass/fail | notes |
|---|---|---|---|---|---|---|---|---|
| 1 | https clone, `push` | | | | | | | plan: no sockets |
| 2 | https clone, `push --check-remotes` | | | | | | | plan: N+1 on 443, 0 on 22, no pushes |
| 3 | https clone, `push --check-remotes --target @root` | | | | | | | plan: 443 only; derived N+1 |
| 4 | SSH clone, `push --check-remotes` | | | | | | | plan: N+1 on 22, no pushes |
| 5 | https clone, `<member>` pushurl SSH, `push --check-remotes` | | | | | | | plan: that member read over SSH only (1 on 22, N on 443) |
| 6 | https clone, `<member>` + root changed, `push` | | | | | | | plan: N+3; published `<member> <sha>`, root `<sha>` |
| 7 | https clone, `tag --push <tag> --target @root` | | | | | | | optional; derived 2N+2; tag `<tag>` → `<sha>` |

Clones and fetches (supporting):

| step | port-443 | port-22 | wall time | notes |
|---|---|---|---|---|
| case 1 `clone --url-scheme https` | | | | before: 9 on 443, 5.7 s |
| case 4 `clone` (SSH) | | | | |
| case 5 fetch-only pull | | | | |
| case 6 fetch-only pull | | | | |

## 4. Accounts

| transport | evidence | account |
|---|---|---|
| https reads of public repositories | transport rows `unknown`, `authenticated: null` | anonymous |
| https read of gwz-core-evidence (private) | transport row `helper`; spawned `<helper argv>` | `<gh account>` |
| https pushes (case 6, case 7) | transport rows `helper`; GitHub activity actor | `<login>` |
| SSH reads (cases 4, 5) | transport rows `agent`; `connect()` to `<agent socket>`; `ssh -T` greeting | `<login>` |
| default launchd agent | not used by any case; `ssh -T` greeting | `<login>` |

## 5. Findings and deviations

- Binary location (see §1).
- `<anything that failed, was repeated, or differed from a derived count>`

## 6. What remains open

- **Credential helpers elsewhere.** Which helper answered on this Mac is `<settled by
  the spawn log / still open>`. A machine with only `osxkeychain`, or with no helper,
  is still untested (plan §7).
- **Connection reuse.** A changed repository costs a read and a push (plan §7).
- **Sockets, not transport rows.** `TransportObservation` has no URL or scheme, and
  `public_key_fingerprint` is always null. So SSH accounts come from the agent socket
  and `ssh -T`, not from gwz output (plan §7).
- **Tag publication.** It reads every dependency twice (2N+2); dedup there is a
  non-goal of this plan.
- **Accepted risks not exercised here.** The §3.7 risks (remote rewound, branch
  deleted, tracking ref from another repository, a published root that has since
  become unsound) are covered by native tests (step 3.6), not by this run.
- **Not exercised against GitHub.** Human output (the summary line and `--verbose`
  reasons), gwz-py's CLI, and a root-only push with an SSH `pushurl` (D2's accepted
  cost).
- `<new items from §5>`
~~~

## 5. Cleanup

**Check that nothing unpublished is left in either scratch clone (local).** This
lists modified tracked files and commits ahead of the tracking ref for every
repository:

```sh
for w in "$WS_HTTPS" "$WS_SSH"; do echo "== $w"; for p in $(jq -r '.members[].member_path' "$EV/case1/dryrun-1.stdout"); do git -C "$w/$p" status --short --untracked-files=no; b="$(git -C "$w/$p" symbolic-ref --short HEAD)"; git -C "$w/$p" log --oneline "origin/$b..HEAD"; git -C "$w/$p" config --get remote.origin.pushurl; done; done
```

**Expected:** only the two `==` header lines.
- Anything else is work not on GitHub, or a leftover `pushurl`.
- Resolve it before deleting.

**Delete the two scratch clones; keep the evidence, tools and `session.env`:**

```sh
du -sh "$WS_HTTPS" "$WS_SSH" "$EV"
rm -rf "$WS_HTTPS" "$WS_SSH"
ls "$ACC"
```

**Expected:** `ls` shows `evidence`, `session.env` and `tools`. Nothing on GitHub,
in gwz-dev or in `~/.cargo/bin` is touched by cleanup.

## 6. Safety summary

**Contacts GitHub, read-only:**
- P2 `git ls-remote` over https;
- P3 `gh auth status` and the two `ssh -T` probes;
- case 1 clone, and case 4 clone;
- the measured pushes in cases 2, 3, 4 and 5, which read only because the gate
  proved nothing is ahead;
- the fetch-only pulls in cases 5, 6 and 7;
- the GitHub verification lines in cases 6 and 7.

`--check-remotes` would push a repository whose remote lacks its commit, so cases 2
to 5 rely on the P2 declaration that nobody rewinds a remote during the run.

**Stays local:**
- setup, P1, the P3 local block, tools, self-test;
- every `--dry-run` (each gate checks that it opened no sockets), every `chk` and
  `precheck`;
- the case 5 `pushurl` edit and its undo;
- the case 6 edits and commits, the case 7 tag creation;
- cleanup.

**Publishes to GitHub:**
- **Case 6:** pushes `$PUB_MEMBER`'s `$PUB_BRANCH` and the gwz-dev root `main`.
- **Case 7 (optional):** pushes one root tag.

Each runs only with its `gate.pass` present, after you type `PUBLISH`.

**Never run:**
- `$GA` against `/Users/owebeeone/limbo/gwz-dev`. P1 and P2 only read gwz-dev
  with `git`.
- Any `target/` build against gwz-dev, or `cargo install` without `--root <scratch>`.
  Nothing may write `~/.cargo/bin/gwz`.
- `$GA tag --push` without a tag name (it pushes every gwz tag), or with
  `--all` (it pushes members too).
- `--force`, a `+` refspec, a hand-run `git push`, or any remote delete.
- A measured push without its `gate.pass`.
- A case 6 or 7 line while a `REPLACE-ME` value is still set.
- A `PUBLISH` line pasted together with other lines.
- The SSH cases (4, and 5's measured push) without
  `--set-env SSH_AUTH_SOCK="$OWB_SOCK"`. The default launchd agent holds a different
  account, which cannot push and is not the account under test.
- `rm -rf` of anything but `$WS_HTTPS` and `$WS_SSH`.

## Appendix A: what the draft checked locally (no network)

Everything below ran on this Mac on 2026-09-16, under the session scratchpad, with a
temporary `HOME`. Nothing contacted a remote.

**Binary and code:**
- The facts in P1.
- `git2 = 0.21` with the `https` and `ssh` features. libgit2-sys `build.rs` defines
  `GIT_SSH_LIBSSH2` and, on macOS, `GIT_SECURE_TRANSPORT`.
- Credential order in git2-rs `CredentialHelper::config`: the exact URL key, then
  `credential.<scheme>://<host>.helper`, then `credential.helper`.
- `public_key_fingerprint` is only ever set to null.
- `handle_tag_in` reads the root, then preflights the dependencies, then proves them
  with an empty `ReadPreflight`.

**Toy workspace.** A root and two members (N=2) with local bare remotes, cloned with
`gwz-alpha clone`:
- **Refspec.** The clone writes `+refs/heads/*:refs/remotes/origin/*` with a single
  `origin`.
- **Unchanged workspace.** The dry run has no connections: every row is `Noop`,
  `up to date with origin/main as of the last fetch or push`, aggregate `Noop`. A
  default push gives the same, with `meta.transport: null`.
- **`--check-remotes`.** 3 `read_advertisement` rows, rows `already on origin`, no
  pushes.
- **`--check-remotes --target @root`.** One row and 3 reads.
- **Member and root changed.**
  - The dry run has the two changed rows `Planned`/`Push`, `push to origin`, and
    aggregate `Ok`.
  - The push makes 3 reads and 2 pushes; its rows are `Ok` with `planned: null`, and
    the aggregate is `Ok`.
  - Afterwards the dry run is clean.
- **`tag --push <name> --target @root`.** 5 reads, 1 push, `members: []`, aggregate
  `Ok`. The tag is on the root remote only.
- **`--sync fetch-only pull --head`.** Moves tracking refs, including the root's, and
  no branches. A member whose remote moved then dry-runs as `behind origin/main ...`,
  and `chk gate` fails it.
- **`commit --target <member>`.** Leaves the root lock staged, and
  `commit --target @root` commits it.
- **Remote head ahead of the lock.** A fresh clone checks the member out at the
  remote head and dry-runs up to date.
- **Invalid ancestor `.git`.** With an ancestor `.git` file like `~/.git`, clone, dry
  run and `--check-remotes` behave the same.

**Tools:**
- the §2.2 table for sockwatch, `connect()` and nettop;
- every `chk gate`, `precheck` and `result` pass and fail path, against the toy
  workspace's JSON and synthetic agent rows;
- every `sh` block in this file, parsed with `zsh -n` and `bash -n`;
- the local blocks, extracted from this file and run under zsh and under bash with
  `$HOME/limbo` pointed at scratch:
  - setup, P1, the P2 repository list, tool install, build and self-test;
  - then case 1's gate block and measured push, against a toy clone at `$WS_HTTPS`
    with `--scheme any`.

  Results: the written tools matched the tested files; the self-test saw 10 of 10
  connections at 200 ms in both observers, and 10 of 10 at 5 ms in `connect()` only;
  both case-1 checks passed and `gate.pass` was written.
