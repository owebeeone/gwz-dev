# GWZ URL scheme: baseline of the reader-without-SSH experience

Date: 2026-09-11. Status: **baseline measurement only, no feature code.**
Intended home: `dev-docs/` of the `urlscheme` lane (`gwz local clone urlscheme`).
The lane could not be created yet (§10), so the measurements below were taken
against the root workspace at commit `a0f6067` used as a **read-only** clone
source. Its manifest is the one the lane would carry.

Product intent being served: manifests keep recording members as
`git@github.com:owebeeone/<repo>.git`; a reader without SSH keys must still get
every public member from `gwz clone`; a contributor with SSH keys must see no
change; no manifest edits; no per-user git-config instructions as the primary
path.

## 1. Environment

- `gwz 1.0.10` from `/Users/owebeeone/.cargo/bin/gwz`; `git 2.52.0`; macOS
  (Darwin 25.6.0). No `target/` build was used.
- gwz-core links `git2 0.21.0` over `libgit2-sys 0.18.8+1.9.7` with the
  `https` and `ssh` features (`gwz-core/Cargo.toml:50`), `libssh2-sys 0.3.2`.
  Member fetches never spawn the `git` binary (§5).
- Root workspace `/Users/owebeeone/limbo/gwz-dev` at `a0f6067`, identical to
  `origin/main` on GitHub. `gwz.conf/gwz.yml` lists eight members, every one
  `git@github.com:owebeeone/<repo>.git` (scp form); `gwz-core-evidence` carries
  `private: true`.
- SSH was disabled with `env -u SSH_AUTH_SOCK GIT_SSH_COMMAND=/bin/false`.
  `GIT_SSH_COMMAND` is inert for gwz because libgit2 never runs `ssh`; unsetting
  `SSH_AUTH_SOCK` is the effective part, since the ssh-agent is gwz's only SSH
  credential source (`gwz-core/src/git/gitbackend/transport_support.rs:195`).
- Two facts about this machine shape every measurement (details in §4, §9):
  the ssh-agent authenticates to GitHub as **`gripd`**, not `owebeeone`
  (`ssh -T git@github.com`), and `~/.gitconfig` plus the Homebrew system
  gitconfig configure credential helpers (`gh auth git-credential`,
  `osxkeychain`) that hold an **https credential for `owebeeone`**.

## 2. Reader without SSH: what happens today

From a fresh scratch directory:

```sh
env -u SSH_AUTH_SOCK GIT_SSH_COMMAND=/bin/false gwz clone /Users/owebeeone/limbo/gwz-dev ws
```

Outcome, 2.2 s, exit status 1, one line on stderr, nothing on stdout:

```text
gwz: RemoteRejected: SSH key authentication failed (no usable identity in the ssh-agent); run `ssh-add` or check your SSH setup
```

- The root clone succeeds: `ws/` holds `gwz.conf/`, `dev-docs/`, `AGENTS.md`
  and the rest of the root tree.
- **Zero member directories exist afterwards.** All seven public member clones
  were attempted and rolled back: when any member fails, the apply loop removes
  every fresh clone it created and returns the first error
  (`gwz-core/src/workspace_ops/handle_materialize.rs:451`, the `first_error` block
  after the `par_map_per_host` call, comment "F2/Q6 reject-partial").
- The failure is a **whole-operation abort, not per-member**. The message names
  no member and prescribes `ssh-add`, which a reader without keys cannot do.
- `--partial` changes nothing: the materialize apply loop never consults the
  partial policy (`grep partial handle_materialize.rs` finds no use).
- The same happens on the documented path through GitHub:
  `env -u SSH_AUTH_SOCK gwz clone https://github.com/owebeeone/gwz-dev.git ws`
  clones the root over https in 2.9 s and then fails identically on the
  members. This is what the README and `AGENTS_GWZ.md` tell a reader to run.

Machine output for the same run:

- `--json`: `meta.aggregate_status: "Failed"`, `members: []`, and
  `errors: [{"code": "RemoteRejected", "member_id": null, "member_path": null,
  "message": "SSH key authentication failed (no usable identity in the ssh-agent); ..."}]`.
  `meta.transport[]` has one row per attempted repository: the root row shows
  `credential_method: "unknown", credential_offered: false, authenticated: null`
  (local path, no credentials asked); each of the seven public members shows
  `credential_method: "agent", credential_offered: true, authenticated: false`.
  The private member has no row (§6).
- `--jsonl`: `OperationStarted`; `MemberStarted` and `MemberFinished` for
  `workspace_root`; `MemberStarted` for each of the seven public members and
  never a `MemberFinished`; `OperationFinished`; then the same response object.
  No event carries the per-member failure.
- `--verbose` (human): the same error line, plus one stderr line per public
  member such as `<ws>/gwz-cli origin: credential=agent source=ambient
  offered=true authenticated=no`.
- `MachineOutput.md` states that per-member failures retain `member_id` and
  `member_path`; the clone path does not honour that today (`member_id: null`).

After the failure:

- `gwz status` inside `ws` prints "Members not materialized (run
  `gwz materialize --lock` to complete the clone)" followed by all eight
  members, and "live state was not observed" for each.
- `gwz materialize --lock` (with or without `--json`, `--jsonl`, `--partial`)
  fails with the identical `RemoteRejected` error; `meta.action` is
  `Materialize` and the seven member transport rows are the same.
- `gwz.conf/gwz.lock.yml` still says `materialized: true` for all eight
  members: it is the committed lock copied from the root, never rewritten on
  failure.

## 3. Contributor with SSH: control run (must stay unchanged)

```sh
gwz --json --verbose clone /Users/owebeeone/limbo/gwz-dev ws
```

Exit 0 in 4.9 s, 68 MB on disk. `aggregate_status: "Ok"`; seven members with
`status: "Ok"` and `lock_match: "Matches"`; every member's `origin` recorded
exactly as the manifest wrote it (`git@github.com:owebeeone/<repo>.git`);
transport rows `credential_method: "agent", credential_offered: true,
authenticated: true`.

`gwz-core-evidence` is absent from `members[]`, from `transport[]` and from
disk. The agent identity (`gripd`) has no access to it, GitHub answers
`ERROR: Repository not found.`, and the private-member rule (§6) skips it
quietly. A later `gwz status` lists `gwz-core-evidence` under "Members not
materialized". So this run is also the control for an **outsider who has SSH
keys**: public members arrive over ssh, the private member is silently skipped
during clone and visible afterwards in status.

## 4. What GitHub answers, by transport and caller

Measured through gwz's own clone funnel (`gwz repo clone <url>` in a scratch
workspace, which uses the same `clone_repo_named` path as member
materialization) and cross-checked with plain `git ls-remote`.

| repository | https, anonymous | https, keychain credential (`owebeeone`) | ssh as `gripd` | ssh, no usable key |
|---|---|---|---|---|
| public `gwz-core` | Ok | Ok | Ok | `RemoteRejected: SSH key authentication failed (no usable identity in the ssh-agent); run ...` |
| private `gwz-core-evidence` | `RemoteRejected: GWZ could not acquire credentials for the requested remote` (GitHub 401) | **Ok, cloned** | `RemoteRejected: ERROR: Repository not found.` | as public |
| missing `definitely-not-a-repo-xyz123` | same as private (401) | `RemoteRejected: unexpected http status code: 404` | `RemoteRejected: ERROR: Repository not found.` | as public |

Plain git agrees: anonymous https gives `could not read Username for
'https://github.com'` for both private and missing; ssh as `gripd` gives
`ERROR: Repository not found.` for both.

Consequences:

- For a caller without access, GitHub does **not** distinguish "private" from
  "does not exist" on either transport. The only observable split is "my
  credentials or transport failed" versus "the server answered and refused this
  repository". gwz's transport rows already record that split
  (`credential_offered`, `authenticated`; `ERROR: Repository not found.` arrives
  with `credential_offered: true, authenticated: null`, while a missing agent
  arrives with `authenticated: false`).
- Without a `known_hosts` entry for github.com, every ssh clone fails as
  `GitCommandFailed: invalid or unknown remote ssh hostkey` before any
  credential is offered. That is classified as a non-access failure today, so a
  private member is then **not** skipped and the whole clone fails on it.
- On this machine, an https attempt is **not anonymous**: libgit2 runs the
  configured credential helper and gets `owebeeone`'s token, so private repos
  clone over https here. Only an emptied `HOME` removes that (§12); neither
  `GIT_CONFIG_NOSYSTEM=1` nor `GIT_CONFIG_SYSTEM`/`GIT_CONFIG_GLOBAL` overrides
  are honoured by gwz's libgit2 for this purpose.

## 5. Code path for `gwz clone` and `gwz materialize`

Member fetches go through **libgit2 (`git2` crate)**, never the git binary.
The only shell-out to `git` in the transport layer is a local-import fallback
for one libgit2 defect (`transport.rs`, `fetch_anonymous_with_git`), unrelated
to remote clones.

1. CLI. Global options are declared in `gwz-cli/src/globalargs/parser.rs`
   (`--identity` at 48-54, `--remote-identity` at 56-58, `--partial` at
   138-144, `--verbose` at 219-225, `--ssh-timeout` at 227-235). They become a
   `gwz_core::RequestMeta` in `gwz-cli/src/clirequest/invocation.rs`
   (`request_meta`, `policy`): identities go to `meta.transport`
   (`TransportOptions { default_identity, remote_identities }`), concurrency
   and partial/force go to `meta.policy` (`OperationPolicy`). The backend is
   `Git2Backend::new()` (`gwz-cli/src/globalargs/dispatch.rs:5`), which sets
   `CredentialHelperPolicy::AllowConfigured`
   (`gwz-core/src/git/gitbackend/backend.rs:40-48`). The ssh timeout is
   process-wide (`gwz-cli/src/lib.rs:163-166`). No environment variable is read
   anywhere in gwz-cli or gwz-core for transport behaviour (only pagers and
   test modes).
2. Clone request. `handle_clone_workspace_request`
   (`gwz-core/src/workspace_ops/handle_materialize.rs:544`) scopes the backend
   with `with_transport` (identity selection frozen per operation), refuses
   `--dry-run`, refuses cloning over an existing workspace, and when a
   `--target` selection or per-remote identity is given first reads the remote
   manifest via `read_remote_file` (a bare probe clone of the root URL).
3. Root clone. `clone_workspace_with_emitter` (`:632`) clones the root with
   `backend.clone_repo_with_progress(&request.url, ...)`, emitting the root as a
   member-like lifecycle (`workspace_root`), then verifies `gwz.conf/gwz.yml`.
4. Planning. It builds a `MaterializeRequest` for the lock target and calls
   `prepare_materialize_execution` (`:238`), which resolves the selection and
   runs `materialize_preflight`
   (`gwz-core/src/workspace_ops/materialize_preflight.rs`). **This is where
   the URL is chosen**: for a member path that is not already a repository,
   `clone_url = Some(first_remote_url(member))`, the URL of the member's first
   `fetch: true` remote in the manifest, verbatim (`first_remote_url`, end of
   that file). For a member that already is a repository, `clone_url = None`
   and no URL is consulted at all.
5. Apply. `apply_materialize_plans` (`handle_materialize.rs:294`, options
   `skip_private_access: true` for clone and for the lock target, `:220`)
   records the fresh clone paths, then runs the plans through
   `par_map_per_host`, bounding concurrency per host with `git_host(clone_url)`
   (`gwz-core/src/git/git_host.rs`). Per plan: `member_started` (suppressed for
   a quiet private clone), `backend.clone_repo_named(url, member_root,
   remote.name, progress)`, then checkout of the branch head or lock commit,
   `member_finished`, and a `MemberResponse` built by `materialized_response`
   (`:1158`) from the observed state. Failures: a `RemoteRejected` on a
   **private** member's fresh clone deletes the directory, forgets its
   transport row and yields nothing (`:354` onwards); any other error is
   returned as-is and becomes `first_error`, which rolls back every fresh clone
   and fails the operation without member attribution. The lock is rewritten
   only for snapshot and tag targets (`rewrite_lock`), never for `--lock`/clone.
6. Transport. `clone_repo_named` (`gwz-core/src/git/gitbackend/transport.rs`)
   builds a `git2::build::RepoBuilder`, names the remote, disables checkout
   filters, resolves an identity with `identity::for_remote` (scp and `ssh://`
   URLs only; https URLs never get an SSH identity, `identity.rs`
   `resolve_remote`), begins a `TransportAttempt` row, and clones. Errors pass
   through `clone_error(url, error)` (`transport.rs:576`), which maps to
   `ErrorCode::RemoteRejected` only when `git_host(url)` is `Some` **and** the
   failure is an access refusal: `git2::ErrorCode::Auth` (not proxy), HTTP
   401/403/404 messages, or the ssh stderr forms `ERROR: Repository not
   found.` / `ERROR: Permission to ... denied to ...`. Everything else becomes
   `GitCommandFailed` via `git_error`.
7. Credentials. `remote_callbacks` (`transport_support.rs`) installs one
   credentials callback. Without an explicit identity, `remote_credential`
   (`:195`) offers `Cred::ssh_key_from_agent` once for SSH and fails the second
   time with the message quoted in §2 (`git2::ErrorCode::Auth`,
   `ErrorClass::Callback`, hence `RemoteRejected`); for https it offers the
   configured git credential helper when `AllowConfigured`, then
   `Cred::default()`, else `GWZ could not acquire credentials for the requested
   remote`. With `--identity`/`--remote-identity`, `explicit_credential` offers
   exactly that key file and never falls back. Each attempt records
   `credential_method` (`agent`, `file`, `helper`, `unknown`),
   `credential_offered` and `authenticated` in `TransportObservations`
   (`transport_observations.rs`), which surface as `meta.transport[]`.
8. `git_host.rs` (28 lines) extracts a lowercase host from scp-form
   (`[user@]host:path`), scheme URLs (`https://`, `ssh://`, ...), and returns
   `None` for local paths and Windows drive letters. It is used only for
   per-host concurrency and for the "is this a remote host" test in
   `clone_error`. It does not parse the path or port; both derivation
   directions for the feature are new code.

A prior proposal, `dev-docs/GwzRemoteAuthProposal.md` (2026-09-06), records an
owner ruling that a **git-CLI transport fallback is a non-starter**; the SSH
identity work it proposed has since landed (`--identity`, `--remote-identity`,
`gwz auth identity`, `remote.<name>.gwzSshIdentity` in a member's local git
config as the "LocalConfiguration" source). That local-config precedent is the
closest existing per-user, never-in-`gwz.conf` setting.

## 6. The private member today

- `ManifestMember.private` (`gwz-core/src/artifact/mod.rs:83-96`, doc comment:
  "Access refusals for fresh clones may be quietly skipped; not remote
  visibility") is set with `gwz repo sync --private|--public` and serialized
  only when true.
- During `gwz clone` and `gwz materialize --lock` (`skip_private_access`), a
  private member whose **fresh** clone fails with `RemoteRejected` is removed
  from disk, dropped from `members[]`, `transport[]` and events
  (`forget_private_clone`, `transport_observations.rs`), and the operation
  continues. Snapshot and tag targets do not skip. A pre-existing non-repo
  directory at the member path is never deleted (a test pins this).
- Outcome for an outsider, measured in §3: clone succeeds, `gwz status` then
  lists the member as not materialized. The lock keeps `materialized: true`.
- Any failure class other than `RemoteRejected` on the private member (for
  example unknown host key, network error, disk error) fails the whole clone,
  by design ("public access and non-access failures must remain errors",
  `gwz-core/src/workspace_ops/tests/g00/private_members.rs`).
- Requirement for the feature: this member must keep failing quietly for
  outsiders exactly like this, and the message an outsider sees for it must
  not become an https 401/404 that hides the ssh answer.

## 7. Existing checkouts, pull/push, and `repo sync`

- `gwz materialize` issues **no fetch** for a member that is already a
  repository; it only checks out from local refs. Existing remotes are never
  touched by clone or materialize. That already satisfies "no change to any
  remote already configured".
- `pull`, `push`, tag fetches and `ls-remote` open the member repository and use
  the remote URL **as configured in that member's git config**
  (`transport.rs`, `fetch`, `perform_push`), not the manifest. A member cloned
  through an alternate scheme would therefore keep working over that scheme.
- Hazard: `gwz repo sync` copies the observed git remote URL into the manifest
  (`sync_member_remotes`, `gwz-core/src/workspace_ops/handle_create_repo.rs:788`, applied at `:748`,
  observed URL wins over the manifest URL). A member that had been cloned over
  the alternate scheme would flip the manifest URL to that scheme on the next
  `repo sync`. This is the concrete reason the optional "report, do not
  rewrite" step in the plan exists.

## 8. Machine output and the protocol

- The response envelope (`kind: "response"`, `meta`, `members[]`, `errors[]`)
  and `MemberResponse` fields are generated from the taut schema
  `gwz-core/protocol/gwz.taut.py` (`MemberResponse` at 1607-1624,
  `GwzError` at 1119, `TransportObservation` and `TransportOptions` at
  1061-1072, `ResponseMeta.transport` at 1108) into
  `gwz-core/src/protocol/generated.rs` by `python protocol/regen.py`
  (`gwz-core/docs/Regeneration.md`). gwz-py regenerates its own bindings from
  the same schema (`gwz-py/scripts/regen_protocol.py`) and pins a projection
  hash in `gwz-py/scripts/check_protocol_drift.py` that must be moved
  deliberately with each additive change. New per-member fields (manifest URL,
  effective URL, scheme, fallback) therefore mean: schema slots, regeneration,
  a drift-pin move, and a `MachineOutput.md` update.
- Today `MemberResponse.state.remotes[]` reports the manifest remotes
  (`protocol_state`), not the effective clone URL; `TransportObservation` has
  no URL or scheme field, only `repository_path`, `remote`, `operation`,
  `credential_method`, `selection_source`, `credential_offered`,
  `authenticated`, `public_key_fingerprint`.
- `MachineOutput.md` documents no `transport` array at all.

## 9. Test harness available

- `gwz-core/src/workspace_ops/tests/g00/private_members.rs`: `RefusingServer`,
  a local TCP fixture answering any HTTP request with 401/403/404/500, used
  with `Git2Backend::new()` (real libgit2) and local `RemoteFixture` repos (`tests/g00.rs:782`) to
  drive `handle_clone_workspace` end to end. This is the model for
  integration tests of refusal classes without touching GitHub.
- `GWZ_TEST_GIT=fake|real` (`gwz-core/src/test_backend.rs`) selects the fake
  or native backend for the shared fixtures; the private-member tests bypass
  it and always use libgit2.
- Nothing in the tree exercises an scp-form URL against a fixture host: the
  scp form always names a real host, and `git_host` treats `127.0.0.1:port`
  strings as host:path. Tests for the derivation and the auto fallback will
  need an injectable known-host table or a fake backend seam, to be designed
  in the plan.
- `transport.rs` has unit tests for `clone_error` classification (the table of
  `(code, class, message, denied)` cases) that any classification change must
  extend.

## 10. Lane blocker

`gwz local clone urlscheme ../gwz-dev-urlscheme`
was refused four times over the session:

```text
gwz: InvalidRequest: local clone `urlscheme` refused before reservation: source workspace /Users/owebeeone/limbo/gwz-dev is not ready for a local family because managed workspace configuration is uncommitted (gwz.conf/markers/conf-integrity.yml); commit the initial GWZ configuration and its integrity marker first. ...
```

The root has a **staged, uncommitted** `gwz.conf/markers/conf-integrity.yml`
(its recorded digest of `gwz.lock.yml` moves from `10c1c8...` to `61f520...`;
the marker's mtime is 2026-09-10 17:06, three minutes after the last root
commit `a0f6067`). This is not the "file vanished mid-copy" failure the retry
rule covers, and committing it would modify the root, which another campaign
owns. Nothing was committed. Every measurement above used the root only as a
read-only clone source. Existing family lanes (`fixhelp`, `fixoutcomes`,
`fixpaths`, `fixpython`, `fsbench`, `skill`) are all `ready` in
`.gwz/local-family.yml` and were not touched.

Postscript, 2026-09-11 evening: the marker was committed by the owner in root
`a98c124` (with gwz-core `a6469cc`, gwz-cli `859dd36`) and the root is clean at
`7a0d527`. The owner then directed the work into the root workspace directly;
no lane is used. Nothing in §§2-9 changes: the clone and transport files cited
are identical between gwz-core `ca0f20c` and `a6469cc`.

## 11. Surprises and risks

1. The https path is not anonymous on this machine (§4). Any manual check of
   an https fallback here must neutralize the credential helpers, or a private
   repo will "work" for the wrong reason.
2. The ssh-agent identity is `gripd`. A contributor test that expects the
   private member to arrive over ssh will not pass on this machine; the
   `owebeeone` identity is reachable only through https helpers here.
3. `--partial` is accepted by `clone` and `materialize` and does nothing.
4. The clone error is not attributed to a member, contrary to `MachineOutput.md`.
5. `url.<base>.insteadOf` rewriting supplied through `GIT_CONFIG_GLOBAL` is not
   applied by gwz's libgit2 clone (tested: members still went to ssh). A
   rewrite in the real `~/.gitconfig` was not tested, deliberately, since it
   would edit the user's global config; either way it is not the primary path
   the owner wants.
6. Missing `known_hosts` produces a different, non-access failure class
   (`invalid or unknown remote ssh hostkey`) that today also defeats the
   private-member skip. A reader on a fresh machine hits this before the
   credential failure. The plan must say whether auto treats it as a reason to
   try https (recommendation to be argued there; distinguishing "unknown" from
   "changed" host keys needs a certificate callback gwz does not have).
7. `/Users/owebeeone/.git` is an invalid gitfile; plain `git` commands run from
   any directory under `$HOME` that is not inside another repository fail with
   `fatal: invalid gitfile format`. It does not affect gwz (libgit2 opens
   repositories by explicit path) but it broke several probe commands until
   they were run from inside a repository.

## 12. Reader environment recipe (for later verification on this machine)

A faithful "reader without SSH and without stored https credentials" is a
temporary `HOME` that contains only `.ssh/known_hosts` (copied), no
`.gitconfig`, and no agent:

```sh
RH=$(mktemp -d) && mkdir -p "$RH/.ssh" && cp ~/.ssh/known_hosts "$RH/.ssh/" && chmod 700 "$RH/.ssh"
env -u SSH_AUTH_SOCK HOME="$RH" XDG_CONFIG_HOME="$RH" gwz --json clone <root-url-or-path> ws
```

Verified: this reproduces the §2 failure exactly (same message, same transport
rows). Dropping `known_hosts` instead yields the host-key failure of §4, which
is a different bug class and must not be mistaken for the auth failure.

## 13. Implications carried into the plan (observations, not decisions)

- The retry decision in `auto` can be taken from data gwz already has: the
  `TransportAttempt` state (`credential_offered`, `authenticated == Some(false)`)
  plus the `clone_error` classes. "Credentials for this scheme were unavailable
  or rejected" is the only class where the alternate scheme can plausibly
  succeed; "server answered and refused the repository" (`Repository not
  found.`, permission denied, 403/404 after an accepted credential) must not
  trigger a retry, or the private member's ssh answer would be replaced by an
  https 401.
- The effective URL must be what the new clone's `origin` records, because
  later `pull`/`push` read it from git config, and the report must show both
  the manifest URL and the effective URL.
- `repo sync` needs the scheme-aware "report, do not rewrite" behaviour, or the
  first sync after a fallback clone rewrites the manifest.
- The private-member quiet skip must be evaluated on the **final** outcome
  after any retry, so the behaviour in §3 and §6 is preserved byte for byte.
- Machine output changes are schema changes (§8), which sizes the reporting
  step and puts gwz-py's drift pin in scope.
