# GwzCoreServerDesign (TR1.3, revision 1) — SURFACE-AXIS REVIEW

**Review object:** `dev-docs/GwzCoreServerDesign.md`, the uncommitted working-tree revision 1 of the gwz server design, SHA-256 `15d5410fa4bd17735299bd8b37025f25fcc3730b996c8c35a22e7b977d6e5739`, 1221 lines; status: proposed, first review (design-stage review of its text), TR1.3 of the transport release; reviewed 2026-09-28. On this axis only its §17 "User-facing surface" (lines 1067–1151) was read, plus the tables §17 contains.

**Baseline:** root `4bf52e002e9444c6d0460042fca0a486132bc55d`, gwz-core `bd53865690ab4c179babfac15e051b69423a5cff`, gwz-cli `ebbea9025632ba8181df7ddb0bb57ac7b09f862e`, gwz-py `0b535dc5815748fdd01d31bad6b2b9738f3f13b2`, gwz-transport `a7a36aec0ec6d31e38647b61567166d612f5d2c5`, verified at start and end (object SHA-256 and line count unchanged). The nine frozen controlling copies in `scratchpad/tr13-controls/` and `srv-r0-head.md` (`8c38616d…`) matched their listed SHA-256 values; their checksums were verified only — on this axis none of them was read. The comparison surface is the captured help of released gwz 1.0.17 and gwz-py in `scratchpad/tr13-surface/` (the live `gwz --help` is byte-identical to the capture), plus the permitted live `gwz help hook|repo|server|local …|auth …` runs.

**Date:** 2026-09-28.

**Axis:** SURFACE — the interface as the person using it meets it, judged from §17 alone against the existing command families. Independent, adversarial, read-only; the other axes run in parallel and nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: NO-GO** — 0 P0, 0 P1, 2 P2, 13 P3 (one P3 confined to the SSH form and labelled "[SSH form]"). Both P2s have bounded, text-fixable remedies: **I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified** (the P3s do not block, but P3-1, P3-3, P3-4 and P3-7 are the cheapest to fold into the same revision).

## 0. Evidence base (what you read and ran)

Read, in full: the reviewer prompt; §17 of the object (lines 1067–1151: the synopsis block 1074–1078 and OD12 line 1081, the options table 1087–1095, the environment/Python/address-grammar/files paragraphs 1097–1103, the `server` output table 1109–1117 and its JSON paragraph 1119, the error table 1125–1131 with the "existing codes" line 1133, the `--verbose` templates 1136–1138, and the lifecycle-pair table 1144–1150); the object's heading list only (`grep -n '^#'`), to locate §17 without reading any other section; `gwz-help.txt`, `gwz-help-local.txt`, `gwz-help-auth.txt`, `gwz-help-forall.txt`, the first 40 lines of `gwz-help-fetch.txt`, `gwz-help-hook-claude-code.txt` (it is the error `unknown help topic 'hook claude-code'`) and `gwz-py-help.txt`.

Ran: `shasum -a 256` on the object, the nine frozen controls and `srv-r0-head.md`; `wc -l`; `git -C … rev-parse HEAD` for the five repositories (start and end); `gwz --help` (diffed against the capture: identical); `gwz help hook` (subcommand `claude-code`), `gwz help repo` (subcommands `add clone create detach attach sync`), `gwz help server` (`unknown help topic`, so no name collision exists today), and `gwz help hook claude-code`, `local dispose`, `local clone`, `auth identity` (all `unknown help topic`: the released `help` verb does not resolve two-word topics — a pre-existing gwz behaviour, noted in §3, not charged to the object). Nothing else: no code, no plan, no other design section, no build, no test, no write.

Deferrals honoured: OD2's outcome (unset `GWZ_SERVER` means in-process; the server is opt-in) is not contested — only the shape of the opt-in; the stdio mode shipping; the reuse design's decisions; OD12's outcome (the SSH form's design is reviewed, its inclusion is not).

## 1. Findings (one subsection per finding, severity-ordered)

### P2-1 — `--ssh-timeout` has two defaults on the surface (server 9, released client 3), and the design's own rule makes a mismatch a refusal

- **Location:** §17 options row `--ssh-timeout SECS`, global (line 1094): applies to `server --start`, `server --stdio`, default `9`, "the server's libgit2 network timeout, which a client's native-path operations must match (§5)"; error row `server_environment_mismatch` (79) (line 1127): "for the timeout: `this operation takes libgit2's native path, which uses the server's --ssh-timeout of <n> seconds, not yours`", exit 1. Released help (`gwz-help-local.txt`, `-auth.txt`, `-forall.txt`, identical text): `--ssh-timeout <secs> … Defaults to 3.` gwz-py help: `--ssh-timeout secs` with no default stated.
- **Root cause:** the row assigns the existing global option a server-side default that differs from the option's released default, and says nothing about the client's default, while §17 elsewhere makes "differs from yours" a code-79 refusal.
- **Violated invariant:** the review brief's "the defaults agree wherever they appear"; and the surface's own promise that the default configuration works ("`--server auto` finds a matching server").
- **Reproduction (from the surface text):** `gwz server --start` (server timeout 9). `gwz --server auto push` on a remote whose key the transport cannot sign with (§17's own listed cause) → the operation takes the native path → the client's timeout (3, the released default, nothing typed by the user) ≠ 9 → `gwz: this operation takes libgit2's native path, which uses the server's --ssh-timeout of 9 seconds, not yours; run with --no-server to run in-process`, exit 1. Every default user with an ECDSA-only or gh-helper remote hits it on the first push through a server. If instead the design silently changes the global default 3 → 9 for every command, then a released behaviour changes with no note anywhere on the surface.
- **Impact:** a default-path refusal (or an undocumented default change to an existing option) that a user can only work around with `--no-server` or by learning to pass `--ssh-timeout 9` on every command. After release, either correction changes a default: a compatibility break, hence P2.
- **Required correction:** state one default for `--ssh-timeout` that is the same in both roles and say so in the row (for example "9, in every role; changed from 3 in this release" or "3, unchanged; the server inherits the client's value per session and no timeout mismatch exists"). State the accepted range for the server (the released option accepts `0` = no timeout; is `0` legal for `--start`?) and gwz-py's default (its help states none). If the client's default changes, add the compatibility note to §17.
- **Closure/regression test:** a CLI test that starts a server with no options, runs a native-path operation from a client with no options, and asserts no `server_environment_mismatch`; a help-text test asserting the same default string in `gwz help <any command>`, `gwz help server` and `gwz-py --help`.

### P2-2 — `server`'s actions are flags (`--start|--stop|--status|--stdio`, "exactly one is required"), against the family convention that a verb family is subcommands

- **Location:** §17 synopsis lines 1075–1078; options row line 1089 ("none: exactly one is required"). Comparison: `gwz --help` lists `Members: repo add|create|clone|detach|attach|sync`, `Lanes: local clone|list|dispose|disband hook claude-code`; `gwz help local` ("Commands: clone list dispose disband", each a subcommand with a one-line summary); `gwz help repo` and `gwz help hook` likewise; gwz-py's `local` is an argparse subparser family.
- **Root cause:** the create/inspect/retire triad (`start`/`status`/`stop`, plus the `stdio` mode) — exactly `local clone|list|dispose`'s shape — was expressed as mutually exclusive flags on a leaf. The only flag precedent the family offers is `hook claude-code --write/--remove`, a leaf whose primary action (serving hooks) runs with no flag; `server` has no primary action, which is the case the family answers with subcommands.
- **Violated invariant:** placement consistency with the existing families; a user who knows `gwz local list` looks for `gwz server status` and finds `unexpected argument`.
- **Concrete consequences the flag form creates and §17 leaves open:** (a) bare `gwz server` is a usage error instead of the family's verb listing; (b) the top-level help cannot list `server start|stop|status|stdio` in its `a|b|c` style, so the verbs are only discoverable in `gwz help server`; (c) the action × option matrix is unstated — `--foreground`, `--max-sessions`, `--idle-exit`, `--log` "apply to `server --start`", but whether `gwz server --stop --idle-exit 5m` or `gwz server --stdio --server auto` is refused, ignored or accepted is not said (subcommands would make this structural); (d) in gwz-py, "exactly one is required" becomes an argparse mutually-exclusive required group whose error text differs from clap's, so the two CLIs' refusals diverge at the first mistake; (e) the SSH remote form (§17 line 1138) executes the stdio mode on the remote host, so its spelling becomes a cross-host, cross-version contract the moment it ships — it must be settled now.
- **Impact:** ships forever; converting `--start` to `start` after release breaks every script and service unit that used the flags: P2 by the axis rule.
- **Required correction:** `gwz server start [--foreground] [--max-sessions N] [--idle-exit DURATION] [--log PATH] [--server auto|ADDRESS]`, `gwz server stop [--server …]`, `gwz server status [--server …]`, `gwz server stdio`, with the one-line summary of each, and the top-level help line `Server: server start|stop|status|stdio` (see P3-3); the same in gwz-py. If the operator instead keeps the flag form, §17 must record the deviation and specify (a)–(d) explicitly; I would re-verdict on that text.
- **Closure/regression test:** `gwz server` with no verb prints the verb list and exits as `gwz local` does; `gwz --help` lists the verbs; `gwz-py server` matches; a test that each action-specific option is refused under the other actions.

### P3-1 — `--log PATH` has no stated default and no stated interaction with `--foreground`

- **Location:** §17 options row `--log PATH` (line 1093): default column reads "§6's table"; row `--foreground` (line 1090): "log to standard error"; §17 "Files" (line 1103): `server-<key>.log` in "the per-user directory (§4, §11)".
- **Root cause:** the one option whose default a user needs most (where do I look when it breaks) is the one whose default cell refers out of the surface, and the two logging rows do not say which wins when combined.
- **Violated invariant:** "every option … states its default" (the brief; an option without a default is a finding on every axis); §17's own claim that it freezes "every name, default and message".
- **Reproduction:** a service manager runs `gwz server --start --foreground --log /var/log/gwz/server.log`; the operator cannot tell from §17 whether the log goes to the file, to standard error, both, or whether the combination is refused. An auto-started server's log location is likewise only inferable from the Files paragraph, whose directory is elsewhere.
- **Impact:** the first diagnostic step (open the log) has no documented path.
- **Required correction:** put the default in the cell: the literal path pattern for each platform (for example `<per-user dir>/server-<key>.log`, with the per-user directory stated per P3-2), the auto-started server's default, and one sentence on `--foreground` + `--log` (file wins / stderr wins / refused).
- **Closure test:** `gwz help server` shows the default path pattern; a test for the `--foreground --log` combination asserting the documented behaviour.

### P3-2 — §17 refers a user elsewhere for facts a user needs: the address grammar, the per-user directory, `<key>`, and the log-path checks

- **Location:** line 1087 ("an address form (§3)"), 1093 ("checked as §3 and §11 say"), 1101 ("The address grammar is §3's table"), 1103 ("In the per-user directory (§4, §11)").
- **Root cause:** the surface section describes the grammar and locations partially and delegates the rest to design sections a user never reads.
- **Violated invariant:** the brief's rule that the surface section is self-contained ("If that section refers you elsewhere for something a user would need, that is itself a finding").
- **What a user cannot answer from §17:** (1) must a socket path be absolute (the stated rule — no empty, `.` or `..` component — admits `sock/gwz`); (2) is a socket path accepted on Windows at all (Files says the socket exists "on Linux and macOS"); (3) where the per-user directory is on each platform, so that `server-<key>`, `.lock` and `.log` can be found; (4) what `<key>` is derived from, so that two `server-…` entries can be told apart and so that a user understands why `--server auto finds a matching server` (lines 1126–1127) after a version or environment change; (5) which checks a `--log` path must pass. The 81 message will name "a walk component and its type or trigger" for a refused path, wording that only §3/§11 explain.
- **Impact:** a user with `no gwz server answers at <address>` or a refused explicit path cannot inspect the socket, lock or log without reading the design.
- **Required correction:** state in §17 the full address grammar (absolute required or not; Windows socket paths), the per-user directory per platform, one sentence on what `<key>` encodes, and the log-path rules, or reproduce §3's table in §17.
- **Closure test:** `gwz help server` prints the per-user directory and the grammar; a help-text test that the grammar in the help equals the grammar the parser enforces (absolute-path and Windows cases).

### P3-3 — The help text a user meets first is not frozen: no one-line summary, no top-level group, no per-action descriptions, and `server --stdio` on a terminal is unspecified

- **Location:** §17 has no `gwz --help` line and no `gwz help server` text; synopsis lines 1074–1078; row 1089.
- **Root cause:** §17 freezes names, defaults and messages but not the descriptive text of the new command family, unlike every existing family (`gwz --help` groups: Inspect/Change/Workspace/Members/Lanes/Other; `gwz help local` opens with a paragraph and one line per verb; gwz-py has a `help=` one-liner per command).
- **Violated invariant:** "names and one-line summaries read cold" are part of the surface a review is asked to freeze.
- **Reproduction:** a first-day user runs `gwz --help`: under which heading is `server`, and what does its line say? Then `gwz help server`: what does `--stdio` do? Read cold, `gwz server --stdio` invites an interactive try; nothing in §17 says whether it refuses a terminal with a message or waits silently for a wire handshake on standard input. (`gwz help hook` shows the family's precedent: it states that the command reads standard input and that a by-hand run is a probe.)
- **Impact:** the summaries and group placement ship unreviewed; a by-hand `server --stdio` may hang with no output.
- **Required correction:** add to §17 the top-level help line and group, the `gwz help server` opening paragraph, one line per action, and the terminal behaviour of `--stdio` (refuse with a message, or state that it serves standard input and a Ctrl-C ends it); the same strings for gwz-py.
- **Closure test:** a help-snapshot test for `gwz --help` and `gwz help server`; a test that `gwz server --stdio` with a TTY on standard input behaves as documented.

### P3-4 — `server --start` on an already-running server exits 0 and silently discards the options it was given

- **Location:** §17 output row "`--start`, already running" (line 1110): `gwz server: already running at <address> (pid <pid>)`, exit 0; lifecycle row 1144 ("`server --start`, idempotent"); options rows 1091–1093.
- **Root cause:** idempotence was defined on the address only; the options of the second invocation have no stated effect and the message does not say they were ignored.
- **Violated invariant:** each message "says what happened".
- **Reproduction:** `gwz --server auto status` auto-starts a server with `--idle-exit 10m`. The user then runs `gwz server --start --idle-exit off --log /srv/gwz.log`, sees `already running at … (pid N)`, exit 0, and believes the server is now persistent and logging to `/srv/gwz.log`. Ten idle minutes later it is gone, and the log never existed.
- **Impact:** a silent no-op on a command the user believes configured something; the same trap for `--max-sessions`.
- **Required correction:** either refuse when any option differs from the running server's ("already running at <address> (pid <pid>) with different options; run gwz server --stop first", exit 1) or keep exit 0 and state in the message that the running server's options were kept, and add the running options (`idle-exit`, `max-sessions`, `log`) to `--status` and its JSON record so a user can see them.
- **Closure test:** start with options A, start again with options B, assert the documented refusal or the message text and that `--status` reports A.

### P3-5 — A server at another key (older version, different environment) cannot be found, listed or stopped; a manually started one lives forever

- **Location:** §17 lines 1103 (`server-<key>`), 1126–1127 ("`--server auto` finds a matching server" after a version or environment mismatch), 1092 (`--idle-exit` default `off` for `server --start`), 1116–1117 (`--status` looks at one address), 1144–1145 (lifecycle pairs end only with `server --stop` at the resolved address).
- **Root cause:** `auto` resolves to a per-key address, keys change with version and environment, and the only stop and status actions address one key; there is no enumeration.
- **Violated invariant:** the lifecycle-pair rule (what a user can start, a user can find and stop); "anything a user can create but not remove".
- **Reproduction:** `gwz server --start` (idle-exit off). Upgrade gwz. `gwz server --status` → `not running at <new address>`, exit 1; `gwz server --stop` → `not running at <new address>`, exit 0. The old server keeps running at `server-<old key>` with its socket, lock and log, until reboot, and only `ps`/Task Manager finds it — the CLI never mentions it. The same happens per environment key when a user changes `SSH_AUTH_SOCK` or a must-match variable between shells.
- **Impact:** leaked processes and files, one per upgrade or environment, invisible to the tool that created them.
- **Required correction:** a `server list` (or `status --all`) that enumerates this user's servers in the per-user directory with pid, version, key and idle-exit, and a `stop --all`; or, minimally, make `--stop`/`--status` report the other `server-*` entries they found. State what an old-version server does to `auto` (it is never chosen; is it ever stopped?).
- **Closure test:** two servers at two keys; the documented list/stop-all action finds and stops both.

### P3-6 — `--stop`'s wait bound is unstated, there is no forced stop, and the family's `--force` global has no stated effect on `server`

- **Location:** §17 output row "`--stop`, still running at the bound" (line 1115): `still stopping at <address> (pid <pid>) after <n> seconds`, exit 1; JSON state `stopping` (line 1119); the global `--force` ("Allow destructive behavior when required. GWZ refuses destructive changes unless this is explicit", released help) is not mentioned anywhere in §17.
- **Root cause:** the drain-then-give-up behaviour is described by its failure message only: no number, no next step, no explicit path.
- **Violated invariant:** every number a user meets has a stated default; every message says what to do next; existing globals have a defined effect on a new command.
- **Reproduction:** a long fetch runs through the server; `gwz server --stop` prints `still stopping … after <n> seconds`, exit 1. Is the server now refusing new sessions? Will a second `--stop` wait again or close the session? Does `gwz --force server --stop` kill it, or is `--force` ignored? §17 does not say; the user's only known next step is `kill <pid>`.
- **Impact:** a bounded but undocumented recovery; scripts cannot rely on `--stop` finishing.
- **Required correction:** state the bound (and whether an option sets it), state that a stopping server refuses new sessions, define `--force` on `server --stop` (close open sessions and exit) or explicitly say it is refused, and put the next step in the message ("re-run to keep waiting, or --force to close <k> open sessions").
- **Closure test:** stop with an open session; assert the documented bound, message and the `--force` behaviour.

### P3-7 — Recovery hints are static and are wrong for two invocations: `--no-server` is appended to `server`-command errors where `--no-server` is refused, and "run gwz server --stop" omits the address

- **Location:** §17 line 1121 ("The CLI appends '; run with --no-server to run in-process' where the table says '+ hint'"), rows 77, 79, 80, 82 (lines 1125, 1127, 1128, 1130); row 1088 (`--no-server` "every command except `server`"); the 79 "file read once" message "run gwz server --stop" (line 1127) with `--server`'s default `GWZ_SERVER` (line 1087).
- **Root cause:** the hint text is composed per code, not per invocation.
- **Violated invariant:** each message says what to do next, and the next step must be executable.
- **Reproduction:** (i) `gwz server --start` fails to bind → `gwz: could not start a gwz server at <address>: <cause>; run with --no-server to run in-process`; the user runs `gwz server --start --no-server` and is refused. (ii) `gwz --server /run/u/me/gwz.sock push` → `<file> changed after the server at /run/u/me/gwz.sock started; run gwz server --stop`; the user runs `gwz server --stop` with `GWZ_SERVER` unset → it resolves `auto`, prints `not running at <auto address>`, exit 0; the explicit server is untouched and the next push refuses again.
- **Impact:** the two most-followed recovery instructions on the surface fail as written.
- **Required correction:** omit the hint for the `server` command (or give it its own: "check the log at <path>"); write the stop instruction with the address the client used (`run gwz server --stop --server <address>`), which the message already knows.
- **Closure test:** error-text tests for `server --start` failures asserting no `--no-server` hint, and for the file-read-once message asserting the address appears in the instruction.

### P3-8 — The `server_address_refused` (81) row is not a message template: its wording is wrong for log paths, its `<reason>` list omits the refusals §17 itself defines, and it has no next step

- **Location:** §17 line 1129: "the address, or a directory or log path the host would open, is refused … `server address from <source> refused: <reason>: <address>`", `<reason>` "names the rule: the grammar's, a walk component and its type or trigger, a name `GetFullPathNameW` would change, or a `PIPE` device redefined"; refusals defined elsewhere in §17 that map to 81: `stdio` given to `server --start/--stop/--status` (line 1087), `ssh://` from `GWZ_SERVER` (line 1097), `ssh://` on `server` (line 1081), `stdio`/`ssh://` to `SocketCoreBridge` (line 1099).
- **Root cause:** the row was written for the explicit-socket-path case and reused for every other refusal.
- **Violated invariant:** each error code lists "its message template" (the brief); a message says what happened.
- **Reproduction:** a user with `GWZ_SERVER=stdio` in the shell profile (the "set it and forget it" walkthrough) runs `gwz server --status` and gets `gwz: server address from GWZ_SERVER refused: <reason>: stdio` — none of the four listed reasons applies, and nothing says "pass --server auto". A refused `--log` path prints `server address from --log refused: …` although no address was typed. "A walk component and its type or trigger" is not a sentence a user can act on.
- **Impact:** the address refusal — the first error a mis-set environment variable produces — cannot be understood or recovered from the message.
- **Required correction:** one template per refusal class, with the concrete `<reason>` strings (for example `stdio is not a server-command address; use auto or a socket path`, `the SSH remote form is not accepted from GWZ_SERVER; pass it with --server`, `log path refused: <rule>`), and a next step for each.
- **Closure test:** error-text tests for each of the five refusal classes above.

### P3-9 — The 79 "at open" template does not compose with three of its four `<name>` values

- **Location:** §17 line 1127: `the server at <address> runs with a different <name>, which it reads process-wide`, where `<name>` is "a variable, `the file-creation mask`, `the OpenSSL library` or `the Windows logon session`".
- **Root cause:** the enumerated values carry their own article.
- **Violated invariant:** messages read cold; §17 freezes "every … message".
- **Reproduction:** composed as written: `runs with a different the OpenSSL library, which it reads process-wide`; `runs with a different the file-creation mask, …`.
- **Impact:** frozen, ungrammatical text that also hides what to do beyond the trailing `--server auto` clause.
- **Required correction:** per-name phrasing (`runs with a different value of SSL_CERT_FILE`, `runs with a different file-creation mask`, `was started against a different OpenSSL library`, `runs in a different Windows logon session`), keeping the no-values rule.
- **Closure test:** a message-composition test over all four `<name>` classes.

### P3-10 — Existing codes are reused for new situations without their message on the surface: `transport_session_full` for the `--max-sessions` cap, `external_tool_missing` for `ssh`

- **Location:** §17 line 1091 ("the next is refused with `transport_session_full`") and line 1133 ("Existing codes these paths use: … `transport_session_full`, beyond `--max-sessions`; and `external_tool_missing`, when `ssh` is not on `PATH` (§16)").
- **Root cause:** the code is named but its message and next step for the new situation are not, and the reused name describes a different thing (a transport session, not a server session).
- **Violated invariant:** each error the surface introduces lists its message template and says what to do next.
- **Reproduction:** the 65th concurrent client of a default server is refused with whatever text `transport_session_full` prints today; nothing on the surface says it will mention the server, `--max-sessions`, or that waiting or a second server is the remedy. The `ssh`-missing case ([SSH form]) likewise has no stated text.
- **Impact:** the one capacity error a busy user will meet is unreadable in context.
- **Required correction:** give both situations their template in the error table (for example `the server at <address> has <N> open sessions, its --max-sessions; wait, or run with --no-server`), or introduce `server_session_full`.
- **Closure test:** error-text test at the cap.

### P3-11 — There is no machine-readable confirmation that a server served the command; the `--verbose` line diverges from the existing `--verbose` contract

- **Location:** §17 lines 1135–1138: "One line on standard error … in every output mode, so machine output stays identical to the in-process run". Released `--verbose` (help, all families): diagnostics "are omitted from normal human output and remain available in --json and --jsonl output".
- **Root cause:** the server line adopts the opposite rule from the existing `--verbose` diagnostics under the same flag.
- **Violated invariant:** parity with the existing surface; the walkthrough step "confirm it was used" must be answerable by a script as well as a person.
- **Reproduction:** a CI job runs `gwz --json --verbose --server auto status` and wants to assert that the server was used. The JSON record is by design identical to the in-process run; the only evidence is an unstructured standard-error line. Meanwhile, the same flag puts the authentication diagnostics inside the JSON.
- **Impact:** no scriptable check of the release's headline feature; two behaviours for one flag.
- **Required correction:** under `--verbose` only (as today's diagnostics), add a `server` object (`address`, `pid`, `protocol`, `mode`) to `--json`/`--jsonl` output, or state explicitly in §17 that this flag's server line is the one verbose diagnostic that never enters machine output, and why.
- **Closure test:** a JSON-output test with `--verbose --server auto` asserting the field (or its documented absence).

### P3-12 — Which executable an auto-start, `server --start`, `--server stdio` and `SocketCoreBridge("auto")` launch is unstated, so a missing one is undiagnosable — especially for gwz-py

- **Location:** §17 line 1069 ("gwz-py's forms are the same"), 1099 ("`auto` may start a server"), 1148 ("The stdio child of `--server stdio`"), 77's `could not start a gwz server at <address>: <cause>` (line 1125), 1133 (`external_tool_missing` only for `ssh`).
- **Root cause:** the surface names the process ("a server", "the stdio child") but never the program or how it is found.
- **Violated invariant:** the surface says what a command runs; the first-day user of gwz-py cannot tell whether `gwz-py server --start` needs `gwz` installed.
- **Reproduction:** on a machine with only the gwz-py wheel, `gwz-py --server auto status` — does it start a bundled core server, look for `gwz` on `PATH`, or fail? If it fails, 77's `<cause>` list (only an OpenSSL example) gives no "no server program" cause.
- **Impact:** an unexplained `could not start` for a whole user population.
- **Required correction:** one sentence per form: the running binary for gwz; the program gwz-py and `SocketCoreBridge` launch and how it is located; the 77 cause text when it is missing (or `external_tool_missing` for it, with its message).
- **Closure test:** a gwz-py test with the server program absent asserting the documented code and message.

### P3-13 [SSH form] — `ssh://[user@]host[:port]/path` is not readable cold: what `/path` is, how `--root` composes with it, that the remote needs gwz, and the verbose line's unresolved phrases

- **Location:** §17 line 1081 (the form), line 1138 (`gwz: server ssh://<destination>/<path>: remote pid <pid>, …; the remote host's environment; off switch <on or off>, from this client`), 77's ssh causes (line 1125), 1133 (`external_tool_missing` for local `ssh` only).
- **Root cause:** the form's one operand is undefined on the surface and the verbose template contains descriptive prose where values belong.
- **Violated invariant:** names read cold; every message is a template.
- **Reproduction:** `gwz --server ssh://build@ci/srv/ws status`: is `/srv/ws` the remote workspace root, the remote socket, or the remote program? Does `--root` then name a local or a remote directory? If gwz is absent on `ci`, the user sees `ssh exited with status 127 before the session opened` with no hint that the remote lacks gwz. The verbose line would print the literal words "the remote host's environment" and "off switch on", neither of which a user can parse.
- **Impact:** confined to the SSH form; the form cannot be used from its documentation.
- **Required correction:** define `/path` (and whether it may be relative to the remote home), state `--root`'s meaning under the form, add the remote-gwz requirement with its 77 cause text, and rewrite the verbose template with placeholders (`environment: remote`, `transport off switch: <on|off> (from this client)`).
- **Closure test:** help-text and verbose-line tests for the form.

## 2. Invariant analysis (attacks that held, with evidence)

- **Placement of the client forms held.** `--server`/`--no-server` as globals match the existing global style (`--target`/`--no-target`, `--remote`, `--ssh-timeout`); `--no-server` "refused with `--server`" is a clean conflict; `GWZ_SERVER` as the only environment knob, with "empty or unset means in-process" and `SocketCoreBridge` never reading it, is stated once and consistently (lines 1087, 1097, 1099, 1146). No user-facing name collides with an existing command (`gwz help server`: unknown topic today).
- **The names read acceptably cold**, `server`, `auto`, `--foreground`, `--idle-exit off|10m`, `--max-sessions`; `--server stdio` (client spawns a child) versus `server --stdio` (be the child) are distinct spellings on distinct sides and I could not construct a misreading with a consequence beyond P3-3's terminal case. `gwz server --status` versus `gwz status` is a possible slip but each help text disambiguates.
- **Defaults agree where they appear**, with the one exception filed as P2-1: `--idle-exit` (`off`; `10m` for auto) agrees between row 1092 and lifecycle row 1145; `--max-sessions` 64 appears once; the 30-second `ssh` handshake bound appears identically in the 77 "when" and message columns; `--server`'s default (`GWZ_SERVER`, `auto` for `server`) is stated once.
- **Lifecycle pairs are complete for the same key.** Start/stop are both idempotent with matching outputs (rows 1109–1110, 1113–1114); an auto-started server has a stated end (10-minute idle exit or `--stop`); `--foreground` ends on the listed signals and exits 0 on a clean stop, 1 when the lock is held (right for a service manager); every client form has a per-command or environment off switch; `SocketCoreBridge` ends with `Client.close()`. The gap is across keys (P3-5) and at the stop bound (P3-6).
- **No message leaks a value the user did not type.** 79's `<name>` rule ("never a value") is stated and its routing messages obey it; 80, 82 and 83 name the address the user supplied; the one printed foreign value is the server's `--ssh-timeout` `<n>`, which is not sensitive. 83 honestly states "the command's effect is unknown, and it was not retried".
- **Output/exit semantics are coherent:** `--stop` on nothing exits 0 (idempotent), `--status` on nothing exits 1 (a query), and the JSON `state` vocabulary (`running`, `stopped`, `not_running`, `stopping`) covers every human row.
- **First-day walkthrough, from §17 alone.** Start: `gwz server --start` → `running at <address> (pid)` — found. Run one command through it: I had to *guess* that `gwz status` would not use it and that `--server auto` or `GWZ_SERVER=auto` is required (it is stated in row 1087, but the start message does not say it — a suggestion, §3). Confirm: `--verbose` stderr line — found for a person, none for a script (P3-11). Stop: `gwz server --stop` — found. Profile `GWZ_SERVER=auto`, forget it: works; with `GWZ_SERVER=stdio` the `server` command refuses with an 81 whose reasons do not cover it (P3-8). Sandboxed tool: 80 with the `--no-server` hint — the next command was found (see §3 for the tool-run caveat). Environment mismatch: the at-open message says `--server auto finds a matching server` and the file-read-once message says `run gwz server --stop` — found, but the latter is wrong for an explicit address (P3-7), and the composed at-open text is ungrammatical (P3-9). Stdio local form once: `gwz --server stdio status` — found; the by-hand `gwz server --stdio` — could not find what it does on a terminal (P3-3).

## 3. Risks and next action

- **gwz-py option placement.** gwz-py is argparse: every global (`--json`, `--verbose`, `--ssh-timeout`) sits before the subcommand in its usage line, whereas the object's synopsis (line 1075) places `--server` after `server --start`. "gwz-py's forms are the same" is therefore only true if the `server` subparser re-declares `--server`, `--json`, `--verbose` and `--ssh-timeout`; §17 should say which spelling gwz-py accepts. Not filed, as it is a pre-existing difference between the two CLIs.
- **Tool-run invocations inherit `GWZ_SERVER`.** With `GWZ_SERVER=auto` in a profile, every gwz command a sandboxed tool runs (including `hook claude-code`, if the tool sandboxes it) fails closed with 80; the per-command remedy `--no-server` exists, but only if the tool's configuration can carry it. Whether the `hook` family should ignore `GWZ_SERVER`, or the sandbox case should degrade to in-process, is a §4 decision, not a surface defect; flagging it so the operator decides knowingly.
- **Stale sockets and files.** §17 says the lock stays after exit and says nothing about the socket file after a crash or about log growth on a never-idle manual server; the recovery for `no gwz server answers at <address>` when a stale socket file exists is undocumented (fold into P3-2/P3-5's correction).
- **The start message could carry the opt-in.** `gwz server: running at <address> (pid <pid>)` would serve the first-day user better as `… ; use it with --server auto or GWZ_SERVER=auto` — OD2's outcome is deferred, its discoverability is not; a suggestion only.
- **`gwz help COMMAND SUBCOMMAND` does not resolve today** (`unknown help topic` for `local clone`, `hook claude-code`, `auth identity`); whichever shape P2-2 lands on, `gwz help server start` will inherit that limitation unless the release fixes the help verb — pre-existing, noted for the lane owner.
- **`--jsonl` and `--dry-run` on `server`** have no stated effect (the object lists `--json` only); a one-line statement each avoids two more "the CLI's usual" gaps.
- **Next action:** revise §17 for P2-1 (one shared `--ssh-timeout` default, stated for both roles and both CLIs, with the compatibility note if it changes) and P2-2 (subcommand form, or a recorded deviation with the open behaviours specified), folding in the P3 corrections that are pure text (P3-1, P3-3, P3-4, P3-7, P3-8, P3-9, P3-10, P3-13); then re-verdict on the revision's SHA-256. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified above.
