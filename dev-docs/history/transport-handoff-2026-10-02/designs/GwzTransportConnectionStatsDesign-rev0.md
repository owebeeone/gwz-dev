# GWZ transport connection statistics — design (`--verbose` diagnostics in machine output)

Date: 2026-10-02. Status: **DRAFT, 2026-10-02, not reviewed; review: dual Consistency and Safety plus Surface (it freezes machine output)**. It authorizes no implementation, commit, tag, push or publish.
- **The request** (the operator, 2026-10-02): "consider adding some connections stats on the jsonl responses (maybe with --verbose or --json_verbose) so that we can diagnose connection issues more easily."
- **Two inputs folded in:** the response-level list of quiet skips, which the [TR1.6 design](GwzTransportCredentialHelpersDesign.md)'s verdict hands to this design (its Safety-3 R-H), and a rule for reading `meta.transport` rows once TR2.1 merges a member's attempts onto one row (§4.3).
- **Code lines** are at gwz-core `ee06d16` (its child `fb7a992` changes only dev-docs), gwz-cli `236f753`, gwz-py `43a07a2`, gwz-transport `6910ba66`, and git2-rs as gwz-core pins it. Lines marked `@f5f0` are TR2.1's, at its lane's gwz-core `f5f0a1ff`, read from git; the lane has since merged main (`7107223d`). TR1.6's are its draft, revision 3. The five large files are split after TR2.1 merges (`fb7a992`), so line numbers will move; the cited functions will not.
- **Prefixes:** `EP/` is `gwz-core/src/git/endpoint/`, `TH/` is `gwz-core/src/transport_host/`, `GB/` is `gwz-core/src/git/gitbackend/`, `WO/` is `gwz-core/src/workspace_ops/`, `XP/` is `gwz-transport/src/`, `CLI/` is `gwz-cli/`, `PY/` is `gwz-py/`. "Plan" is `GwzTransportReleasePlan.md` and "A2" its [amendment 2](GwzTransportReleasePlanAmendment-2.md), cited by line. "TR1.5" is the accepted [transport setting design](GwzTransportOffSwitchDesign.md), "the Python design" is `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md`, and "the crate map" is `dev-docs/GwzCoreSessionCrateMap.md`.

**Names.** "Transport diagnostics" is what `--verbose` asks for. In JSON it is `meta.transport_diagnostics` and a `stats` object on each `meta.transport` row (§4). A *setup* is one attempt to establish a physical connection. A *stream* is one Git exchange over a connection: an `Open` and what answers it.

## 1. Decision

- **D1. `--verbose` asks; nothing else changes.** With `--verbose`, a command whose request is in transport scope (`gwz-core/src/transport_scope.rs:1-15`) asks core for transport diagnostics. Its `--json` and `--jsonl` response then carries `meta.transport_diagnostics` and each row's `stats`, and human mode prints host, connection and quiet-skip lines (§5). Without `--verbose`, every payload is byte-identical to today's. There is no new flag (§3.1).
- **D2. Core builds one record, and the drivers only render it**, as the placement design requires of rows (`GwzRemoteTransportPlacementDesign.md:349-356`). Core always records, at a few clock reads per open, stream and setup phase, and attaches the record only when the request's policy asks: `OperationPolicy.transport_diagnostics`, a candidate field beside TR2.1's `max_retries` (`protocol/candidate/candidate.taut.py:91-99 @f5f0`).
- **D3. The final response carries it**, in `--jsonl` as in `--json`. There is no new event kind (§3.2, OQ3).
- **D4. Rows stay one per remote call.** Per-call figures go on that call's row. Per-connection and per-host figures, and quiet skips, go in `meta.transport_diagnostics`. `stream_id` and `connection_id` join them (§4.3).
- **D5. No wire change at the endpoint boundary.** The endpoint hands its figures to the driver through a ledger the runtime owns, in process, as TR2.18's handoff carries URL extras the other way (`EP/ssh_handoff.rs:1-16`). gwz-transport's schema does not change, and its `Failure` still carries no attempt number (`EP/setup_retry.rs:111-131 @f5f0`). The only schema change is four optional fields in gwz-core's candidate protocol, with the messages they carry, under rule (a) (A2 line 346), which S7.1 (1.1.0) names for the ordinary protocol (A2 line 278).
- **D6. Safe and bounded** (§8): codes, causes, counts, milliseconds and bytes; the pool key's host, port and SSH user; never a URL, a credential, helper output or server text; capped lists with exact drop counts.
- **D7. 1.1.0, Phase 2,** as TR2.24 in three steps (§9, OQ1).

## 2. What a user needs, and what the code observes

*Free* means recorded today and not shown; *cheap*, measured or nearly so and then dropped; *new*, needing new marks.

| Need | Figures (§4) | What the code has today | Cost |
|---|---|---|---|
| Opened vs reused | stream `connection_id`, `reused`; host `setups`, `streams_reused`; connection `streams` | Every `Opened` carries its connection's ID and `reused` (`EP/ssh_worker.rs:801-805`; `EP/https_worker/prepare.rs:113-120`). The row stores them, overwriting them for each stream (`GB/transport_observations.rs:98-104`), and gwz-cli renders none of them (`CLI/src/response_meta_json.rs:37-42`), so TR8.1 counts connections with a `connect()` interposer (A2 lines 47, 263). SSH counts a connection's exchanges (`EP/ssh_setup.rs:156, 285`) | free |
| Setup vs transfer | stream `open_ms`, `duration_ms`; connection `setup_ms`; row `duration_ms` | The driver's open (`TH/session/driver.rs:93-237`) and a stream's retirement (`:586-588`) keep no time. HTTPS measures `allocation_elapsed` and `connect_elapsed` only to spend budgets (`EP/https_pool.rs:110-187`; `prepare.rs:109-112`); SSH measures neither (`EP/ssh_setup.rs:107-194`). No response has a per-member time (`protocol/gwz.taut.py:1130-1142, 1731-1743`) | cheap |
| Where a setup spends its time | connection `dns_ms`, `connect_ms`, `handshake_ms`, `auth_ms`, `phase` | SSH's setup is one supervised closure: `known_hosts` and name resolution, TCP, key exchange (`EP/ssh_network.rs:66-78`), then authentication (`EP/ssh_local.rs:80-123`), on an injectable clock (`EP/agent_job.rs:56-57, 81-86`). HTTPS resolves in its setup job (`EP/https_connection.rs:205-211`), then TCP (`:364`), a proxy's CONNECT and TLS (`:382-453`) and HTTP's handshake (`:457`) | new: four marks per scheme |
| Waits | connection `queue_ms`; stream `helper_ms` | The endpoint's queue subtracts its wait from the open's allocation and keeps nothing (`EP/placement_endpoint/admission.rs:157-169, 341-346`); the SSH pool checkout (`EP/ssh_worker.rs:688-719`); HTTPS's request slot, helper slot and lookup (`prepare.rs:34-36, 59-75`). Which cap held an allocation is decided and not kept (`XP/pool/allocation.rs:53-58`) | cheap; the holding cap absent |
| Drops, refusals, stalls | connection `outcome`, `cause`; host `failures` | The wire carries a code and a setup cause (`gwz-transport/protocol/transport.taut.py:20-26, 41-43`). A `MaxStartups` drop is `Io` with no cause (A2 line 545; `EP/ssh_setup.rs:394-419`) | cheap: the endpoint keeps the I/O kind, for these figures only |
| Retries | connection `attempt`; host `retry` | TR2.1's machine stamps each flight's attempt (`EP/setup_retry/machine.rs:55-61 @f5f0`) and keeps only the failure that closes the key (`:11-17`). A retried failure keeps its facts and loses its code and cause (`EP/placement_endpoint/completion.rs:205-206 @f5f0`) | cheap, in that machine |
| Bytes | stream `sent_bytes`, `received_bytes`; row `received_bytes` | The driver's end of each stream keeps its Git payload bytes, readable after close (`XP/stream/machine.rs:5-19, 424-439`), and no production code reads them. libgit2's indexer counts a fetch's pack bytes (`git2-rs/src/remote.rs:384`) | free |
| Which transport | row `stats.transport` | `configure` picks the route for each call (`GB/transport_binding.rs:132-137, 163-165`) | free |
| Why a private member is missing | `quiet_skips` | Clone's skip removes the directory and the member's row (`WO/handle_materialize/apply.rs:68-84`; `GB/transport_observations.rs:41-51`; `CLI/docs/commands/clone.md:65-70`). TR1.6 names the reasons and prints them under `--verbose` (its §11, "Skip line") | cheap |

**Absent, and not invented:** SSH wire bytes (a stream counts payload; HTTPS counts both directions together per connection, `EP/https_progress.rs:12-42`); which cap held an allocation; why the pool closed a connection (`XP/pool/mod.rs:275-285`, ignored at `EP/ssh_pool.rs:292-299`); and every figure of libgit2's own connections (§7).

## 3. Where it appears

### 3.1 The flag

- **(a) `--verbose` with `--json` or `--jsonl`** (recommended). No new option. `--verbose` already means transport diagnostics (`CLI/src/globalargs/parser.rs:219-225`); the plan already freezes "the `--verbose` transport-row fields" in S7.5 (plan line 441; A2 line 298), and wants connection counts in `--verbose` rows at Phase 6's exit (plan line 366). The cost: `--verbose` changes machine output for the first time. Today rows are in JSON with or without it (`CLI/src/response_meta_json.rs:10-12`), as its long help says; §5 changes that help, and the retry plan's "`--verbose` and `--json` keep their current gates" (`GwzRemoteTransportRetryPlan.md:283`) gains this exception (§9).
- **(b) A new flag,** `--json-verbose` or `--stats`: one more global option to freeze, with its own rules for `--jsonl` and human mode, and `--json_verbose`'s spelling is not gwz's kebab case. It buys nothing that (a) lacks.
- **(c) Always on in JSON:** it breaks the byte identity that TR1.5's D7 and DR-1 keep, puts clock values into every network payload and fixture, and grows every response.

### 3.2 `--jsonl`: events or the final response

- **The final response only** (recommended). The record is a key of the final `kind: "response"` object, which is the field to read, as for `crash_recovery` (`CLI/docs/MachineOutput.md:559-563`) and TR1.5's `transport_setting` (its §10).
- **Live events would need** a sink the transport can reach, and it has none: the emitter borrows its sink (`gwz-core/src/operation/eventemitter.rs:21-33`), and the transport's callbacks are `'static` (`GB/transport_binding.rs:57-62`). They would also need a structured field on `OperationEvent`, which has none (`protocol/gwz.taut.py:1703-1729`) and whose shape a cross-driver fixture pins (`CLI/docs/MachineOutput.md:587-590`), and a bound on event volume at 500 members. gwz-py's `--jsonl` streams events only for merge (`PY/src/gwz/cli_merge.py:156-159`). OQ3 keeps a live form for 1.2.0.

## 4. Schema

### 4.1 Shape

Abbreviated; every key shown is always present inside its object.

```json
"meta": {
  "transport": [{ "repository_path": "/w/app", "remote": "origin", "operation": "fetch",
    "stats": { "transport": "gwz", "duration_ms": 2310, "received_bytes": 18234,
      "streams": [{ "stream_id": 7, "connection_id": "ssh-worker-1-2", "service": "upload_pack_exchange",
        "reused": true, "open_ms": 840, "helper_ms": null, "duration_ms": 1420,
        "sent_bytes": 912, "received_bytes": 19873, "outcome": "closed" }],
      "streams_dropped": 0 } }],
  "transport_diagnostics": {
    "connections_observed": true,
    "hosts": [{ "scheme": "ssh", "host": "github.com", "port": 22, "user": "git",
      "setups": 5, "setups_failed": 2, "streams": 32, "streams_reused": 27,
      "failures": [{ "cause": "eof", "count": 2 }],
      "retry": { "state": "healthy", "max_attempts": 4, "waits": 1, "wait_ms": 1104 } }],
    "hosts_dropped": 0,
    "connections": [{ "connection_id": "ssh-worker-1-1", "scheme": "ssh", "host": "github.com", "port": 22, "user": "git",
      "for_stream": 3, "attempt": 1, "outcome": "io", "cause": "eof", "phase": "handshake",
      "queue_ms": 0, "setup_ms": 47, "dns_ms": 4, "connect_ms": 31, "handshake_ms": 12, "auth_ms": null, "streams": 0 }],
    "connections_dropped": 0,
    "quiet_skips": [{ "member_id": "mem_secret", "member_path": "secret", "reason": "no_usable_credential" }],
    "quiet_skips_dropped": 0 } }
```

### 4.2 Fields and presence

- **Units.** Durations are whole milliseconds, rounded down, each the difference of two `Instant`s, the process's monotonic clock; tests inject it where the code already does (`EP/agent_job.rs:56-57`). There are no timestamps. Stream bytes are Git payload (pkt-lines and pack, not SSH or TLS framing). A row's `received_bytes` is libgit2's indexer count for a fetch or clone, the same on both routes, and null for a push or an advertisement read.
- **Row `stats`:** `transport`, `gwz` or `native`; `duration_ms`, the remote call's time, libgit2's own work included; `received_bytes`; `streams`, each stream the call opened, in open order, at most 8 (`streams_dropped`). A stream has `stream_id`; `connection_id` and `reused`, null when its open failed; `service`, gwz-transport's `GitService` in snake case; `open_ms`, from the driver's open to `Opened` or `OpenFailed`, so every wait, retry and setup it met; `helper_ms`, HTTPS's credential lookup, null when none ran; `duration_ms`, from `Opened` to its terminal, null while open; `sent_bytes` and `received_bytes`; and `outcome`: `closed`, `open`, or gwz-transport's error code in snake case (`timeout`, `io`, …).
- **`hosts`,** one per pool key (`XP/pool/mod.rs:84-107`), at most 64 (`hosts_dropped`): `scheme`; `host`, lowercased as the key holds it; `port`; `user`, SSH's, null for HTTPS; `setups` started, `setups_failed`, `streams`, `streams_reused`; `failures`, a count per cause; and `retry`, TR2.1's state for the key when the response is built (`cold`, `healthy`, `waiting`, `degraded` or `closed`, `EP/setup_retry/machine.rs:42-53 @f5f0`), its `max_attempts`, and the backoff `waits` and their total `wait_ms`. Host counts stay exact when `connections` is cut.
- **`connections`,** one per setup, in start order, at most 256 (`connections_dropped`): `connection_id`, in the receipt's form, also for a setup that failed, which the pool names while it opens (`XP/pool/asynchronous.rs:216-223`); the key's fields; `for_stream`, the stream whose open started it; `attempt`, TR2.1's N; `outcome`, `ok` or a code; `cause`, a wire setup cause (`stall`, `aggregate`, `interaction`, `allocation`, `connection_refused`, `not_found`, `address_not_available`) or, for `io` alone, `reset` or `eof` from the I/O kind the endpoint saw, else null; `phase` reached: `queue`, `dns`, `connect`, `handshake`, `auth` or `ready`; `queue_ms`, from the open's arrival at the endpoint to the setup's start; `setup_ms` and its parts (`dns_ms` includes reading `known_hosts`; HTTPS's `handshake_ms` covers a proxy's CONNECT, TLS and HTTP's handshake; `auth_ms` is SSH's, null for HTTPS, whose credentials belong to a request); and `streams`, the exchanges it served.
- **`quiet_skips`,** one per member clone skipped quietly, at most 1024 (`quiet_skips_dropped`): `member_id`, `member_path` and `reason`, TR1.6's classes in snake case: `no_usable_credential`, `challenge_unanswerable`, `credential_helpers_off`, `access_refused`, `credential_after_discovery` (TR1.6 §11).
- **Presence.** `meta.transport_diagnostics` is present only when the request asked (in gwz-cli, `--verbose`) and the response is that of a command in transport scope, a `--dry-run` included. It is also on an error's response meta that carries rows (`WO/publication.rs:739-780`). Otherwise it is absent, and absence means "not asked". When present, its lists are arrays, empty when empty. Row `stats` is present on every row of such a response and absent otherwise. A top-level error whose `meta` is null carries neither.
- **`connections_observed`** is true only when some row of the response went through an endpoint whose figures this process collected. It is false on the native route (§7), for an endpoint in another process (OQ4), and before step b (§9). `hosts` and `connections` are then empty, meaning "not observed", not "none".
- **Per operation** means the sums over `hosts`. No separate total is stored, so none can disagree with them.

### 4.3 Rows and attempts: the reading rule

- A row is one remote call by one repository: `begin` makes one per call (`GB/transport.rs:46, 85, 125, 162, 183`; `GB/transport_observations.rs:53-92`). TR2.1 keeps that. A retried attempt adds no row, and its facts merge onto the member's one row, a later attempt's value winning and an offer staying offered (`EP/setup_retry.rs:133-154 @f5f0`; commit `0fb61c5e`).
- **The rule for consumers:** read one row per call. A member with several calls, such as a push's advertisement read and its push, has a row for each, in call order. A row's credential facts summarise all its attempts, and its four receipt fields, where rendered, are its last stream's (`GB/transport_observations.rs:98-104`). `MachineOutput.md`'s "one per remote authentication attempt" (line 203) then reads "one per remote call by one repository; retries and HTTPS's several streams add no row".
- **Attempts are never rows.** Each setup is one `connections` entry with its attempt, outcome and cause, and its `for_stream` names a stream in a row's `stats.streams`. A member that a closed key finishes carries the key's failure and its `attempt M of M` suffix though it set nothing up (`EP/placement_endpoint/admission.rs:182-191 @f5f0`). Its stream has an `open_ms` and an outcome, and no connection names it: the figures tell that case apart, which the suffix cannot.
- Diagnostics add no row and change none of a row's eight documented fields. A quietly skipped member's row and streams leave with it (`GB/transport_observations.rs:41-51`); its setups stay in `connections`, whose `for_stream` then names no row.

### 4.4 Versioning

Additive only. Every new protocol field is optional and `MISSING_OK`, since gwz-py's strict decoder refuses a missing known key otherwise; it drops unknown keys (`PY/src/gwz/protocol/codec.py:228-250`, over taut-proto 0.10.0). JSON consumers tolerate additive keys, as `MachineOutput.md` asks (line 449). The enumerated strings (`outcome`, `cause`, `phase`, `state`, `reason`, `service`) may gain values in a later release, and a consumer treats an unknown value as other. No field changes its meaning.

## 5. Human `--verbose`

The lines follow TR1.5's `transport:` line and the rows (`CLI/src/globalargs/render_exit.rs:13-29`), and an error's rows (`CLI/src/clirequest/common.rs:175-187`), in gwz-cli and in gwz-py, whose row lines mirror gwz-cli's (`PY/src/gwz/cli_render_parts/machine.py:255-270`). They use the rows' `key=value` form and JSON's names, and omit null values.
- **Each row line** that today's filter prints (`CLI/src/response_meta_json.rs:45-73`) ends with ` transport=<gwz|native> duration_ms=<n> streams=<n> reused_streams=<n> received_bytes=<n>`.
- **One line per host:** `transport host ssh git@github.com:22: setups=5 failed=2 failures=eof:2 streams=32 reused_streams=27 retry=healthy waits=1 wait_ms=1104`. An HTTPS key has no `user@`.
- **One line per connection,** at most 64, then `transport connections: <n> more in --json`:
  - `transport connection ssh-worker-1-2 ssh git@github.com:22: outcome=ok setup_ms=1840 queue_ms=0 dns_ms=4 connect_ms=30 handshake_ms=400 auth_ms=1406 attempt=2/4 streams=27`
  - `transport connection ssh-worker-1-1 ssh git@github.com:22: outcome=io cause=eof phase=handshake setup_ms=47 queue_ms=0 dns_ms=4 connect_ms=31 handshake_ms=12 attempt=1/4 streams=0`
- **Quiet skips:** TR1.6's skip line, unchanged.
- **Native:** rows end ` transport=native duration_ms=<n> …`, followed by `transport connections: not observed on libgit2's native transport`.
- **`--verbose`'s help** reads `Show transport diagnostics: authentication, connections and timings`. Its long help keeps TR1.5's sentence; its second sentence reads `Authentication rows are omitted from normal human output and remain available in --json and --jsonl output.`; and it gains: ` With --json or --jsonl, --verbose also adds each row's timings and bytes (stats) and meta.transport_diagnostics: each host's and connection's setups, failures and retries, and the private members clone skipped quietly. Without --verbose, --json and --jsonl output is unchanged.`

## 6. gwz-py

- **The same record.** gwz-py's network operations run core's handlers in the per-operation runtime (`PY/native/src/route/transport.rs:96-121`), so core attaches the same fields. The submit path's `OperationResult` gains the same field, which `finish` copies as it copies `transport` (`PY/native/src/operations.rs:214-233`), and the error path's `response_meta` passes it on (`PY/src/gwz/errors.py:60-75`).
- **The types** gain the fields at S7.1 (1.1.0)'s regeneration (A2 line 278). Until then Python drops the extension's extra keys (`PY/src/gwz/protocol/codec.py:239-250`).
- **Asking.** 1.1.0 gives `max_retries` no Python form (Python design §2.8, line 131). OQ2 recommends one keyword, `transport_diagnostics`, on `Client.meta()` and so on every operation, since it is the only way a Python caller can learn why a private member is missing. gwz-py's CLI passes it under `--verbose` and prints §5's lines, and its `--verbose` help (`PY/src/gwz/cli_shared.py:334-340`) gains §5's JSON sentence. Its JSON dumps the generated types (`PY/src/gwz/cli_render.py:73-79`), so after S7.1 its rows carry `stats: null` and its meta `transport_diagnostics: null` when not asked, as they will carry the four receipt fields.
- **The per-operation view.** Each operation owns its runtime and pool, and nothing is reused across operations (Python design §2.4). Its record therefore covers its own connections: `reused` means reused within that operation, eight overlapping operations give eight independent records, and `ClientHost` aggregates only cleanup (§2.7), not diagnostics. From 1.2.0 the session host shares one pool: a stream's `connection_id` may then name a connection an earlier operation set up, whose `connections` entry is in that operation's record. The ledger stays keyed by request.

## 7. The native transport

- With `--transport native`, and for `file://`, `git://` and `http://` remotes under either setting (`GB/transport_binding.rs:132-137, 163-165`; TR1.5 §6), libgit2 serves the call. It gives gwz the credential callback, which fills the row's facts (`GB/transport_support.rs:166-207`); transfer progress, which gwz installs for clone only (`:80-97`); and the indexer's counts after a fetch (`git2-rs/src/remote.rs:384`). It reports no connection, reuse, setup time, phase or retry, and it has neither pooling nor setup retry (TR1.5 §5).
- So a native row has `stats` with `transport: "native"`, `duration_ms`, `received_bytes` for a fetch or clone (null for a push or an advertisement read), and no streams. A response whose rows are all native has `connections_observed: false`. A `gwz`-selected fetch with an `http://` member mixes the two, and is `connections_observed: true`.
- Quiet skips are listed on either transport. On the native one the reason is `access_refused` for a refusal clone recognises, and `no_usable_credential` for libgit2's authentication class, which is all libgit2 tells.

## 8. Safety and size

- **Shown:** codes, causes, phases, counts, milliseconds and bytes; the pool key's scheme, host, port and SSH user; the opaque connection and stream IDs the receipt already holds; rows' `repository_path` and `remote`, as today; a skipped member's ID and path, which the manifest holds.
- **Never shown:** a URL, a path on the server, a query, a password or token (TR2.18's password never leaves its handoff, `EP/ssh_handoff.rs:1-16`, and an HTTPS key has no user, `XP/pool/mod.rs:100-107`), a helper's output, arguments or exit text, a key path or key material, any fingerprint beyond the row's own, server text (banners, stderr, HTTP reason phrases and bodies), environment values, or a proxy (an HTTPS key is the origin's, `EP/https_worker/prepare.rs:100`, and the proxy is chosen inside the setup, `EP/https_connection.rs:205-211`).
- **The host rule.** Today no JSON field is a host: hosts reach JSON only inside `url_resolution`'s URLs, on clone and materialize (`CLI/docs/MachineOutput.md:164-199`), and inside libgit2's error text. With `--verbose`, every network command's JSON names its pool keys. `MachineOutput.md` says so: a shared `--verbose` payload shares remote hosts and local repository paths.
- **Bounds.** At most 8 streams per row, 64 hosts, 256 connections and 1024 quiet skips, each with an exact drop count; host totals count everything. At 500 members on one host that is about 500 × 250 bytes of row figures, plus at most 256 × 300 bytes of connections: under 200 KB. The ledger holds at most its caps per request and drops a request's entries when it finishes or is dropped (`TH/request.rs:314-332`), so a 1.2.0 host context keeps no history.
- **No state outside an owner.** The runtime owns the ledger and passes it explicitly; IDs are the pool's and the mux's. No counter, global or thread-local is added (gwz-core `AGENTS.md`).

## 9. Placement, flow and cost

- **Endpoint to ledger.** A new module, `EP/diagnostics.rs`: record types and a bounded ledger keyed by request ID (`EP/placement_endpoint.rs:38`), pure data, with no I/O and no protocol type. `TransportRuntime` creates it and passes it with the handoff (`TH/mod.rs:157-163`; `TH/session.rs:343-364`; `EP/ssh_local.rs:24-31`). Its writers: the placement endpoint's queue and attempts; TR2.1's machine (flights, waits, state); the SSH worker's checkout (`EP/ssh_worker.rs:688-719`) and setup, whose phase marks travel in the setup's `Opening` beside its progress (`EP/ssh_pool.rs:22-32`); and the HTTPS pool, connection and helper lookup (`EP/https_pool.rs:110-187`; `EP/https_connection.rs:360-470`; `prepare.rs:59-75`).
- **Why a module.** In 1.1.0 it is a gwz-core module, as the operator's answer to TR1.5's OQ2 placed that design's code (`fb7a992`): no candidate crate enters 1.1.0's ordinary build (A2 §3.16), and a crate would be a fourteenth crates.io name. At CS7.1's extraction (1.2.0) its types move to `gwz-endpoint-contract`, which the endpoint crates and the instance share (crate map §4).
- **Driver to row.** The driver's stream entry gains its open and `Opened` instants. `Opened`, `OpenFailed` and the terminal (`TH/session/driver.rs:397-442`) pass `open_ms`, `duration_ms` and the peer's byte counts to the row through the callbacks `configure` installs (`GB/transport_binding.rs:146-156, 174-184`). The terminal is reported before the stream's client sees it (`driver.rs:436-440`), so a row's figures are final before libgit2's call returns. `TransportAttempt` keeps the route, the call's duration and its streams, and the skip branch records into `TransportObservations` beside `forget_private_clone`.
- **Core to response.** `attach_transport` and `attach_transport_error` (`WO/publication.rs:739-780`) build the fields from the rows and, through the host context, from the request's ledger entries, when the policy asks.
- **Schema.** Composed in `protocol/candidate/candidate.taut.py` as TR2.1's field is: `OperationPolicy.transport_diagnostics` (tag 10, bool), `TransportObservation.stats` (13), `ResponseMeta.transport_diagnostics` (10) and `OperationResult.transport_diagnostics` (11), all `MISSING_OK`, with §4.1's objects as new messages and its enumerations as enums. Each site is gated by `gwz_transport_candidate` (rule (a)) and listed in its repository's inventory file. S7.1 (1.1.0) names the four fields, and S7.5 (1.1.0)'s Surface review takes them as "the `--verbose` transport-row fields". gwz-cli sets the field where `policy()` fills `--jobs` and `--max-per-host` (`CLI/src/clirequest/invocation.rs:108-130`).
- **Steps,** as TR2.24, after TR2.1, the split of the five large files (`fb7a992`), TR2.5 (for the native rows) and TR2.22 (whose classification gives `quiet_skips` its reasons):
  - **a.** gwz-core: the schema, row and stream figures, native figures, quiet skips and attaching. Until b, `helper_ms` is null and `connections_observed` false. About 340 lines.
  - **b.** gwz-core: the ledger and its writers, with the phase marks. About 430 lines. It can move to 1.2.0 alone, with no schema change.
  - **c.** gwz-cli's rendering, help and `MachineOutput.md`, and gwz-py's (OQ2). About 310 lines, beside b.

  About 1,080 production lines in all. TR2.6 reviews the code with the rest of Phase 2.
- **On GO:** the plan's changelog gains TR2.24 and its edges; the retry plan's §5 sentence on gates (line 283) gains "except as `GwzTransportConnectionStatsDesign.md` §3.1 adds"; the Python design's §2.7 "1.1.0 adds no protocol field" (line 122) gains "but the candidate fields S7.1 (1.1.0) names"; `clone.md`'s private-member paragraph gains the `--verbose` list; and gwz-core's `GWZDesign.md` and `GWZRequirements.md` gain paired paragraphs before step a, as gwz-core's `AGENTS.md` requires.

## 10. Tests

On macOS ARM64 and Linux x86-64, against the disposable fixtures (`EP/ssh_fixture.rs`, `EP/https_fixture.rs`), with no live account; on dabeest after S4.5.
- **Byte identity:** fetch, push, clone and materialize, on both routes: `--json` and `--jsonl` without `--verbose` equal their output before the change byte for byte; `gwz --verbose --json status` carries neither key; a top-level error with null `meta` carries none.
- **Unit, gwz-core:** the ledger's caps and exact drop counts, with caps lowered through its constructor; phase marks under `ManualClock` (`EP/agent_job.rs:555-576`); `ConnectionReset` gives `reset` and `UnexpectedEof` gives `eof`, while `ConnectionAborted` stays `cancelled` with no cause (A2 line 545); a payload without the new fields decodes in Rust and in Python; nothing is attached unless the policy asks.
- **Reuse:** 8 members at `--max-per-host 1` give one connection entry with `streams` 8, seven `reused` streams, each stream's bytes equal to what the fixture served, and `open_ms` rising with queue position.
- **Drops:** a fixture that accepts and then closes its first N connections before its banner, as `MaxStartups` does: the host's `failures` count `eof` (or `reset`) N times, those entries are in `handshake` with TR2.1's attempt numbers, the key ends `healthy`, and every member is Ok. With every connection dropped: at most the per-host limit plus three setups (OD18, A2 line 541), one row per member, and the members a closed key finished named by no connection entry.
- **Stalls and refusals:** the silent SSH fixture gives `timeout` with `stall` on attempts 1 to 4, `max_attempts` 4, three `waits` totalling about 7 s plus jitter; a closed port gives `unavailable` with `connection_refused`, retried.
- **HTTPS:** an anonymous fetch's row has an advertisement stream and an exchange stream on one connection, the second `reused`. With a recording credential helper, `helper_ms` is set, and a 401-then-helper row lists each of its streams. TR1.6's hanging helper gives a `helper_ms` near the interaction deadline.
- **Quiet skips:** `gwz --verbose --json clone` of a workspace whose `private: true` member is refused gives no member row and one `quiet_skips` entry with its reason; without `--verbose` the output is byte-identical; human `--verbose` prints TR1.6's line once.
- **Native:** `--transport native --verbose --json fetch` gives native `stats` with `duration_ms` and `received_bytes`, and `connections_observed: false`; an `http://` member under `gwz` gives one native row in a response with `connections_observed: true`.
- **Privacy:** an `ssh://` URL with a password (TR2.18's fixture), an `https://` URL with a token as its user, a helper whose output and stderr hold a marker, and a server whose banner and HTTP reason phrase hold markers: no marker in any JSON, JSONL or human output.
- **Size:** 600 members with lowered caps: the drop counts and host totals are exact and the output stays within §8's bound.
- **gwz-py** (`run_tests.py --candidate`): with OQ2's keyword, `fetch` and `submit` results carry the record; gwz-py's CLI `--verbose` human lines equal gwz-cli's for one fixture run.

## 11. Open questions for the operator

- **OQ1. Which release.** Recommended: 1.1.0, Phase 2, as TR2.24 a to c. 1.1.0 is when users first run the transport and meet its new failure modes, among them OD18's first wave against `MaxStartups` and the retry machine (A2 lines 545-546), and TR8.1 (1.1.0) can then read connection counts from gwz itself, checked once against its `connect()` record. Step b can move to 1.2.0 alone. Alternative: all of it in 1.2.0, with the session host and Phase 6's exit row (plan line 366).
- **OQ2. gwz-py's request in 1.1.0.** Recommended: one keyword, `transport_diagnostics` on `Client.meta()`, default `None` and additive, so A2 line 84's "public API unchanged" holds but for it. Alternative: none until 1.2.0, as for `max_retries`; Python callers then cannot see quiet skips in 1.1.0, and gwz-py's CLI prints no diagnostics.
- **OQ3. Live JSONL events.** Recommended: none in 1.1.0. In 1.2.0, when the session host owns the event stream, one `diagnostic` event per failed setup attempt (code, cause, attempt, host), the final record staying the field to read. Alternative: a 1.1.0 event, which needs a sink in the host context and an `OperationEvent` field.
- **OQ4. An endpoint in another process** (1.2.0's `cli` placement) cannot share the ledger. Recommended: `connections_observed: false` there at first, and an optional setup record on gwz-transport's `Opened` and `Failure`, decided when that placement ships. Alternative: that owner-schema field now, which changes gwz-transport's schema for no 1.1.0 caller.
- **OQ5. Quiet skips without `--verbose`.** Recommended: no, so that `clone.md`'s quiet contract and TR1.6's "JSON and gwz-py are unchanged" stand. Alternative: `quiet_skips` present whenever it is non-empty, as `transport_setting` is present whenever it is not the default.
- **OQ6. Names and the SSH user.** Recommended: `meta.transport_diagnostics`, row `stats` and `quiet_skips`, with the SSH user shown, since the caps and TR2.1's machine work per key and `url_resolution` already shows the user. Alternatives: `meta.connections`; hosts merged by host and port, without the user.

## Changelog

- 2026-10-02: revision 0, drafted on the operator's request of 2026-10-02, with TR1.6's quiet-skip list and TR2.1's row rule folded in.
