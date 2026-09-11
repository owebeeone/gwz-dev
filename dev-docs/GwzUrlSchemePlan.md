# GWZ URL scheme plan: explicit ssh/https selection for clone and materialize

Date: 2026-09-11. Status: **plan, awaiting owner confirmation before any code.**
Companion: `GwzUrlSchemeBaseline-2026-09-11.md` (measurements and code path).
Work location: the root workspace `/Users/owebeeone/limbo/gwz-dev` directly,
per owner instruction of 2026-09-11 (no lane). Installed `gwz 1.0.10` for every
workspace operation; never a `target/` build.

Owner decisions folded in (2026-09-11):

- **No automatic switching.** The manifest URL is used as written unless the
  user asks for a scheme. The `auto` mode, its failure classifier, the retry
  and the host-key question are out of scope.
- The unknown-host-key prompt is tracked separately in
  `gwz-cli/dev-docs/GwzGaps.md` ("SSH transport: unknown host key").
- Manifests keep `git@github.com:owebeeone/<repo>.git` everywhere. No manifest
  edits, no per-user git-config instructions as the primary path.

## 1. Goal and non-goals

Goal. A reader without SSH keys runs one of

```sh
gwz clone --url-scheme https https://github.com/owebeeone/gwz-dev.git
GWZ_URL_SCHEME=https gwz clone https://github.com/owebeeone/gwz-dev.git
```

and receives every public member over https, with the private member skipped
quietly exactly as today. A contributor with SSH keys runs the same commands as
today and gets the same ssh remotes, byte for byte. Both see, in JSON and in
human output, which URL was actually used for each member and why.

Non-goals: automatic fallback; host-key trust; changing any remote already
configured in an existing checkout; rewriting manifests; version bumps; release
tooling; `gwz-py` protocol beyond the additive bindings; Windows-specific work
(the derivation is pure string handling and the state file uses the existing
durable-write helpers).

## 2. Behaviour contract

### 2.1 Option and precedence

- `gwz clone --url-scheme <manifest|ssh|https> <url> [dir]` and
  `gwz materialize --url-scheme <manifest|ssh|https> [--lock|...]`.
  Sub-command options, not global ones, because no other verb clones.
- `manifest` means "as written" and is the built-in default. It exists as an
  explicit value so a persisted preference can be overridden for one run and
  cleared (§2.4).
- Environment: `GWZ_URL_SCHEME=<manifest|ssh|https>`, case-insensitive, read by
  the CLI and by gwz-py, never by core.
- Precedence, highest first: flag, environment variable, the workspace's
  recorded preference `.gwz/url-scheme.yml`, then `manifest`.
- Core receives the explicit choice in the request (`TransportOptions.url_scheme`,
  §2.7) and resolves the workspace file itself, so a remote gwz-core honours the
  same rule and gwz-py needs no extra logic.

### 2.2 Derivation rules

Known hosts: `github.com`, `gitlab.com`, `bitbucket.org` (host compared
case-insensitively, emitted lowercase). Everything else, including local paths,
`file://`, `ssh.github.com`, self-hosted GitLab and any host with a port other
than the scheme default, is **unknown** and passes through verbatim with
`host_known: false`.

| input form (known host) | `--url-scheme https` | `--url-scheme ssh` |
|---|---|---|
| `git@HOST:owner/repo.git` | `https://HOST/owner/repo.git` | unchanged |
| `git@HOST:owner/repo` | `https://HOST/owner/repo` | unchanged |
| `ssh://git@HOST/owner/repo.git` | `https://HOST/owner/repo.git` | `git@HOST:owner/repo.git` |
| `ssh://git@HOST:22/owner/repo.git` | `https://HOST/owner/repo.git` | `git@HOST:owner/repo.git` |
| `https://HOST/owner/repo.git` | unchanged | `git@HOST:owner/repo.git` |
| `https://HOST/owner/repo` | unchanged | `git@HOST:owner/repo` |
| `https://user@HOST/owner/repo.git` | unchanged | `git@HOST:owner/repo.git` |
| `https://HOST:443/owner/repo.git` | unchanged | `git@HOST:owner/repo.git` |
| `git@HOST:group/sub/repo.git` (gitlab) | `https://HOST/group/sub/repo.git` | unchanged |
| `ssh://git@HOST:2222/owner/repo.git` | **refusal** | unchanged |
| `http://HOST/owner/repo.git` | **refusal** | **refusal** |
| `git@HOST:` or `https://HOST/` (empty path) | **refusal** | **refusal** |

Rules behind the table: the `.git` suffix is preserved as written in both
directions; the ssh user is always emitted as `git`; a user or a default port in
the source form is dropped; a leading `/` in an scp path and a trailing `/` are
trimmed; the scp form is the ssh output because it is the form every existing
manifest uses. A URL already in the requested form is reported as
`derived: false`. A refusal is typed (`UrlSchemeUnavailable`, §2.5) and names
the member, the URL and the remedy; nothing is fetched before it.

`host_known`, `derive` and a `scheme_only_difference(a, b)` predicate (true
when `derive(a, scheme_of(b)) == b`) live in one pure module,
`gwz-core/src/git/url_scheme.rs`, beside `git_host.rs`.

### 2.3 Where the scheme applies

- The **root URL** given to `gwz clone`, before the root is cloned, so a reader
  who copies an ssh clone line from a README and adds the flag is not stopped
  by the root.
- Every **member clone URL** chosen by `materialize_preflight`
  (`first_remote_url`, `gwz-core/src/workspace_ops/materialize_preflight.rs`),
  for `gwz clone` and for every `gwz materialize` target that clones a missing
  member. The manifest is read, never written.
- **Never** to a member that is already a repository: materialize issues no
  fetch for those, and `pull`/`push` keep using the remote recorded in the
  member's git config. Existing checkouts are untouched.
- The effective URL is what the new clone's `origin` records, because later
  `pull`/`push` read it from git config.
- Contradictory options refuse early: `--remote-identity NAME=PATH` names an SSH
  identity for a remote whose effective URL is https under `--url-scheme https`.
  Today's message ("a per-remote SSH identity override names a non-SSH
  destination") is kept; the refusal happens in preflight before any network.

### 2.4 Workspace memory: `.gwz/url-scheme.yml`

- After a successful `clone` or `materialize` whose scheme came from the
  request (`ssh` or `https`), core writes `<workspace>/.gwz/url-scheme.yml`:

  ```yaml
  schema: gwz.url-scheme/v1
  scheme: https
  recorded_by: clone        # or materialize
  recorded_at_ms: 1789093300078
  ```

- `materialize` with no request scheme reads it and applies it. `clone` cannot
  read it (no workspace exists yet) and only writes it.
- An explicit `--url-scheme manifest` (flag or env) removes the file.
- It is workspace-local runtime state like `.gwz/local-family.yml`: never under
  `gwz.conf/`, never in the manifest, copied into verbatim local clones (it is
  not in the local-clone exclusion list, and should not be: same user, same
  machine). Written through the existing atomic staged write
  (`artifact::write_atomic_in`, the path the manifest and lock already use),
  read strictly (a malformed file is a typed refusal naming the file, not a
  silent default).

### 2.5 Failures and messages

- `UrlSchemeUnavailable` (new `ErrorCode` / `GwzErrorCode` value): a requested
  scheme cannot be derived for a known-host URL (§2.2). Message names member id
  and path, the URL, the reason, and the remedy: "use `--url-scheme manifest`
  for this run, or record a remote in the wanted form with `gwz repo sync`".
- Member attribution: a member clone failure inside `apply_materialize_plans`
  now carries `member_id` and `member_path` on the error, which
  `MachineOutput.md` already promises and the baseline shows is missing.
- Two message hints, text only, no behaviour change, only when the effective
  URL is ssh-form on a known host and the scheme is not `https`:
  - the credential failure `SSH key authentication failed (no usable identity
    in the ssh-agent); run ssh-add or check your SSH setup` gains
    `; for public repositories, retry with --url-scheme https or set
    GWZ_URL_SCHEME=https`;
  - the host-key failure `invalid or unknown remote ssh hostkey` gains
    `; run ssh -T git@<host> once to record the host key, or retry with
    --url-scheme https`.
  Both are appended in `apply_materialize_plans` (member) and in
  `clone_workspace_with_emitter` (root), where member, URL and scheme are
  known, by matching the two fixed strings. Transport code is not changed.
- Everything else keeps today's semantics: reject-partial rollback of fresh
  clones on the first error, `--partial` still without effect on clone.

### 2.6 Private members

Unchanged code path: `skip_private_access` and the `RemoteRejected` quiet skip
apply to the clone attempt with the effective URL. Under `https` an outsider's
private member answers 401 (`GWZ could not acquire credentials for the requested
remote`), which is already the quiet-skip class; under `ssh` or `manifest` it
answers `ERROR: Repository not found.` as today. The transport row is forgotten
as today. Existing tests in `tests/g00/private_members.rs` are extended, not
rewritten.

### 2.7 Machine output and protocol (additive)

Schema edits in `gwz-core/protocol/gwz.taut.py`, then regeneration:

```text
UrlScheme       = Enum(manifest=0, ssh=1, https=2)
UrlSchemeSource = Enum(default=0, request=1, workspace=2)
MemberUrlResolution = Msg(
    manifest_url=F(1, STR),
    effective_url=F(2, STR),
    scheme=F(3, Ref.UrlScheme),
    source=F(4, Ref.UrlSchemeSource),
    derived=F(5, BOOL),
    host_known=F(6, BOOL))
MemberResponse.url_resolution = F(12, Ref.MemberUrlResolution, optional=True)
TransportOptions.url_scheme   = F(3, Ref.UrlScheme, optional=True)
GwzErrorCode.url_scheme_unavailable = <next slot>
```

- `url_resolution` is set on every member response produced by clone and by
  materialize for a member that was cloned in this operation (`planned.action ==
  Clone`), including the `manifest` case (`derived: false`, `source: default`),
  so consumers can rely on its presence for cloned members. Members that were
  only checked out carry `null`.
- The root's effective URL is reported in `meta.message` when derivation changed
  it and in human output; the machine caller already knows the URL and scheme it
  asked for.
- `source` distinguishes only what core can know: `request`, `workspace`
  (`.gwz/url-scheme.yml`) or `default`. The CLI adds "flag" versus "env" in
  human output.
- Regeneration ritual: `python protocol/regen.py` and `python
  docs/generate_message_catalog.py` in gwz-core; `python scripts/regen_protocol.py`
  in gwz-py and a deliberate move of the drift pin in
  `gwz-py/scripts/check_protocol_drift.py` with a dated note in the file's
  existing style ("removing exactly these additions reproduces prior
  projection ...").

### 2.8 `gwz repo sync` (optional, last)

Today `sync_member_remotes` copies the observed git remote URL into the
manifest, so a member cloned under `https` flips its manifest URL on the next
sync. With the feature: when the observed URL and the manifest URL differ only
by scheme on a known host (`scheme_only_difference`), sync reports the
difference (member status `Noop` plus a `lock_difference_reasons`-style note in
`state`/message, and a human line "remote origin is https, manifest records
ssh; not rewritten") and leaves the manifest alone. `--force` (the existing
destructive-allow policy) rewrites as today. Any other difference syncs as today.

### 2.9 Out of scope, recorded

- `auto` fallback (owner decision). If wanted later it is one more step on top
  of this plan: a retry inside `apply_materialize_plans` using the same
  derivation and report fields plus a `fallback` field.
- Unknown-host-key prompt (`GwzGaps.md`).
- Reporting which account a credential helper supplied (noted in the baseline
  §11; a helper username could be surfaced in `TransportObservation` later).
- `gwz init <url>` and `gwz repo clone <url>`: the user types the URL and can
  type either form.

## 3. Phases and steps

Foundational first; steps in the same phase are independent unless stated.
Budgets are aspirational (< 500 LOC each, tests included). One commit per step,
member-scoped through gwz (`gwz add <paths> --target <member>` then
`gwz commit --target <member> -m ...`); root-owned files (`dev-docs/`) commit
with `--target @root`. Touched members' suites run green before each commit.
Braced control-flow bodies everywhere; no `#[cfg]` on bare imports; no AI
attribution trailer.

### Phase 0: preconditions (no product code)

**Step 0.1: baselines and filing.** Root verified clean at `7a0d527` on
2026-09-11 (members: gwz-core `a6469cc`, gwz-cli `a5abba2`, gwz-py `34e4e0e`);
the integrity-marker refusal met during the baseline is gone, and the
`GwzGaps.md` host-key entry is already committed as gwz-cli `a5abba2`. Run and
time the gwz-core and gwz-cli suites once as the reference (`cargo test -p
gwz-core`, `cargo test -p gwz-cli`; note wall-clock), and `python
protocol/regen.py --check` in gwz-core plus `python
scripts/check_protocol_drift.py` in gwz-py, all expected green. File this plan
and the baseline under `gwz-dev/dev-docs/` (root commit). Budget: docs only.

### Phase 1: foundation (parallel-friendly, no behaviour change)

**Step 1.1: derivation module.** `gwz-core/src/git/url_scheme.rs`, exported
from `git/mod.rs`: `UrlScheme`, `KnownHost`, `ParsedRemote` (scp, `ssh://`,
`https://`, other), `host_known`, `derive`, `scheme_only_difference`, and the
refusal type. Pure functions, no I/O. Unit tests for every row of the §2.2
table, both directions, all three hosts, with and without `.git`, default and
nonstandard ports, `user@`, uppercase host, nested GitLab groups, unknown-host
and local-path passthrough, Windows drive letters, empty and garbage input, and
the predicate. Depends on nothing. Budget ~300 LOC. Suitable for a leaf owner.

**Step 1.2: protocol and error code.** Schema edits of §2.7, `ErrorCode::
UrlSchemeUnavailable` in `model/mod.rs` and its mapping in
`protocol/convert.rs`, regeneration in gwz-core (generated.rs, cbor.rs, corpus,
MessageCatalog.md), `url_resolution: None` at every `MemberResponse { .. }`
literal: 53 sites in 15 files today (14 files in gwz-core across
workspace_ops, status, operation and stash projection, plus one gwz-cli
rendering test), one line each and compiler-enforced, gwz-py bindings
regenerated and the drift pin moved with a dated note. Depends on nothing (1.1
not needed to compile). Contract change: integration owner. Budget ~260
hand-written LOC plus generated output. Commits: gwz-core, then gwz-py.

### Phase 2: behaviour

**Step 2.1: core plumbing, state file, report, hints.** Read
`TransportOptions.url_scheme`; resolve request, then `.gwz/url-scheme.yml`,
then `manifest`; derive the root URL in `handle_clone_workspace_request` before
the root clone; thread the scheme into `prepare_materialize_execution` and
`materialize_preflight` so `MaterializePlan` carries a `UrlResolution` and
`clone_url` is the effective URL; refuse contradictions with
`--remote-identity`; attach `url_resolution` in `materialized_response`; attach
member id and path to member clone errors; append the two hints of §2.5; write
or remove `.gwz/url-scheme.yml` after success. Tests with the existing fixtures:
(a) https-form fixture manifest (unknown host, passthrough) materializes under
`https` and reports `derived: false, host_known: false`; (b) known-host
derivation reaches the clone seam: the fake backend cannot clone
(`gitbackend/fake_repository.rs`, `clone_repo` at line 489 is `unsupported`)
and every clone test uses real libgit2, so the assertion sits one level up:
call `materialize_preflight` directly with a manifest member at
`git@github.com:o/r.git` and the `https` scheme and assert `plan.clone_url ==
"https://github.com/o/r.git"` with the matching `UrlResolution`; the root URL
goes through a small `resolve_root_url(url, scheme)` function unit-tested the
same way; the network proof is step 3.2; (c) private member under `https`
with a 401 `RefusingServer` stays quiet and absent, under `manifest` unchanged;
(d) precedence: request beats workspace file, explicit `manifest` removes the
file, malformed file refuses; (e) no scheme anywhere: `clone_url` equals the
manifest URL and `url_resolution.scheme == manifest`, asserted on the existing
clone test so the contributor path is pinned; (f) refusals of §2.2 are
`UrlSchemeUnavailable` and fetch nothing (fake backend records no clone); (g)
error attribution and hint text. Depends on 1.1 and 1.2. Coupled step:
integration owner. Budget ~450 LOC. Commit: gwz-core.

**Step 2.2: Rust CLI.** `--url-scheme` on `CloneArgs`
(`gwz-cli/src/clirequest/workspace.rs`) and `MaterializeArgs`
(`clirequest/snapshot_materialize.rs`) as a clap value enum with
`GWZ_URL_SCHEME` fallback read explicitly in request assembly (gwz-cli builds
clap with `derive` and `wrap_help` only, no `env` feature; flag wins), mapped into `TransportOptions.url_scheme` in
`clirequest/invocation.rs::request_meta` (the `transport` option is built
whenever any transport field is present). Human output: one summary line when
the scheme is not `manifest` ("url scheme: https (from --url-scheme)"), per
member `manifest -> effective` lines under `--verbose`, and the root line for
clone. Long help and examples in `clone_long.rs`, `clone_after.rs`,
`materialize_long.rs`. Regenerate `gwz-cli/docs/commands/*.md` with `python
scripts/generate_cli_reference.py --write` and confirm `--check`. Tests in
`gwz-cli/tests` in the existing style: parsing, env precedence, `manifest`
override, JSON passthrough of `url_resolution`, human summary line. Depends on
1.2 (types) for compilation and on 2.1 for the end-to-end assertions. Budget
~250 LOC. Commit: gwz-cli.

**Step 2.3: gwz-py parity.** `--url-scheme` on the clone and materialize
parsers in `gwz-py/src/gwz/cli_shared.py` / `cli_mutation.py` with the same env
fallback, `TransportOptions(url_scheme=...)` in the request meta, rendering of
`url_resolution` in the human output, tests via `run_tests.py`. Depends on 1.2;
end-to-end assertions on 2.1. Parallel with 2.2. Leaf owner. Budget ~200 LOC.
Commit: gwz-py.

### Phase 3: finish

**Step 3.1: documentation.** `gwz-cli/docs/QuickStart.md` "Clone An Existing
Workspace": manifests may record ssh or https; contributors with SSH need
nothing; without SSH pass `--url-scheme https` or set `GWZ_URL_SCHEME=https`;
the workspace remembers it for `materialize`. `Troubleshooting.md` "SSH Or
Credential Failure": the same remedy. `MachineOutput.md`: the `url_resolution`
fields and the `transport` array it never documented. `Concepts.md`: one line
beside `--ssh-timeout`. Depends on 2.2. Leaf owner. Budget ~120 LOC. Commit:
gwz-cli.

**Step 3.2: verification against GitHub and acceptance note.** Manual, with the
installed binary rebuilt from the committed tree by the owner's normal process
(this plan bumps nothing and tags nothing; if a local install is needed for the
check, say so explicitly rather than running a `target/` build against the
workspace). Cases, each captured as JSON: reader environment from the baseline
§12 recipe with `--url-scheme https` on the published root (seven public
members arrive, remotes https, private member absent and quiet, then `gwz
status` lists it as not materialized); the same via `GWZ_URL_SCHEME`; a second
`materialize --lock` in that workspace with no flag honours
`.gwz/url-scheme.yml`; contributor environment with the bare command reproduces
the baseline §3 JSON except for the added `url_resolution` fields; `--url-scheme
ssh` on an https-form root URL; a refusal case. Results go to
`gwz-dev/dev-docs/GwzUrlSchemeAcceptance-<date>.md` (root commit). Depends on
2.2 and 2.3.

**Step 3.3 (optional): scheme-aware `repo sync`.** §2.8 in
`handle_create_repo.rs::repo_sync_plan_member` using `scheme_only_difference`,
`--force` to rewrite, CLI help text, tests for report-not-rewrite, force,
unknown-host and non-scheme differences. Depends on 1.1 only; commit after 2.1
to keep history linear. Leaf owner. Budget ~250 LOC. Commits: gwz-core, gwz-cli.

### Dependency summary

```text
0.1 -> {1.1, 1.2} -> 2.1 -> {2.2, 2.3} -> 3.1, 3.2
1.1 -> 3.3 (commit after 2.1)
```

1.1 and 1.2 run in parallel; 2.2 and 2.3 run in parallel; 3.3 can be built any
time after 1.1.

## 4. Test matrix (what "done" means)

| behaviour | test | step |
|---|---|---|
| every derivation row, both directions, three hosts | unit, `url_scheme.rs` | 1.1 |
| unknown host, local path, garbage pass through or refuse as specified | unit | 1.1 |
| schema round-trips, corpus golden, py drift pin | generated corpus, `check_protocol_drift.py` | 1.2 |
| known-host ssh manifest under `https` asks the backend for https | core integration, fake backend | 2.1 |
| https fixture manifest under `https` and `ssh` (unknown host) works, `host_known: false` | core integration, real libgit2 | 2.1 |
| private member quiet under `https` (401 fixture) and under `manifest` | core integration | 2.1 |
| no scheme: effective == manifest, contributor path pinned | core integration | 2.1 |
| precedence request > workspace file > default; `manifest` clears file; malformed file refuses | core integration | 2.1 |
| refusals fetch nothing; errors carry member id and path; hints present | core integration | 2.1 |
| flag, env, precedence, JSON passthrough, human summary | gwz-cli tests | 2.2 |
| gwz-py flag, env, rendering | gwz-py tests | 2.3 |
| real GitHub: reader gets all public members over https, private quiet; contributor unchanged | manual, acceptance note | 3.2 |
| repo sync reports scheme-only drift, rewrites only with `--force` | core + cli tests | 3.3 |

## 5. Risks and open points

- **Struct-literal fan-out.** Adding `url_resolution` to `MemberResponse`
  touches 53 literal construction sites in 15 files. Mechanical and
  compiler-enforced, but it is the bulk of step 1.2 and the reason that step
  is its own commit. Placing the field on `ResolvedMemberState` instead would
  touch one builder, at the cost of putting an operation fact into a state
  record; rejected for that reason, reopen if the fan-out proves worse.
- **No clone in the fake backend.** Plumbing is tested at the preflight seam
  and with real libgit2 against local fixtures; the known-host end-to-end case
  is manual (3.2). If a fixture-host mapping is wanted later, the known-host
  table becomes a parameter of `derive` with the production table as default.
- **`.gwz/url-scheme.yml` and lanes.** It is copied into verbatim local clones by
  design; if the owner prefers lanes to start clean of it, it joins the
  exclusion list in `local_clone/adapters/exclusions.rs` (one line).
- **Hint strings match fixed messages.** The two hints key on gwz's own
  credential message and libgit2's host-key message; a unit test pins both
  strings so a wording change fails loudly.
- **Manual verification identity.** On the owner's Mac ssh authenticates as
  `gripd` and https helpers as `owebeeone` (baseline §11); the acceptance note
  states which environment each case ran in.

## 6. Reporting at each stop

Each step's commit message names the step. The step-3.2 note is the acceptance
evidence: what a reader without SSH experiences after the change, what a
contributor with SSH experiences (unchanged), what was measured, what remains
open. Short hashes and members are listed in the final report.
