# GWZ push: prove root dependencies through the member's own destination, and contact only what changed

Date: 2026-09-14. Status: **plan; decisions D1–D9 taken on 2026-09-14; no code yet.**
Step 0.2 ran on 2026-09-14.
Parent: `GwzUrlSchemePlan.md` (clone and materialize) and its acceptance note
`GwzUrlSchemeAcceptance-2026-09-12.md`.
Base: gwz-core `c9c7a98` / root `aa97cc1` ("Skip the second availability read
for members an ordinary push just published"). On 2026-09-14 that commit is
local and not yet pushed.
Work location: the root workspace `/Users/owebeeone/limbo/gwz-dev` directly.
Use the installed `gwz 1.0.12` for every workspace operation, and never run a
`target/` build against the workspace.

Owner direction (2026-09-14):

- **https end to end, the second way.** gwz treats URLs that differ only by
  scheme as the same repository and reads through each member's configured
  remote. Manifests keep `git@github.com:owebeeone/<repo>.git`. There is no
  scheme flag on push and no remote rewriting. (Phases 1 and 2.)
- **Contact only what changed.** A push looks only at repositories that changed
  relative to their origin. An option, `--check-remotes` (D7), keeps today's
  behaviour of checking every remote. (Phase 3.)
- **Check once.** A repository already checked in an operation is not contacted
  again when nothing changed. (Phase 3, in both modes.)

## 1. Goal and non-goals

**Goals:**

1. A workspace cloned with `gwz clone --url-scheme https` publishes with
   `gwz push` over https only, with zero SSH connections (Phase 2).
2. By default, a push contacts only repositories whose branch differs from what
   their origin last reported. A push with nothing to publish makes no
   connections (Phase 3).
3. With the option, a push checks every selected repository and every root
   dependency, and contacts each destination once (Phase 3).

Phase 2 changes nothing in a contributor's SSH workspace: it keeps reading its
committed URLs.

**Non-goals:**

- https URLs in manifests;
- a `--url-scheme` option on push;
- a command that rewrites existing remotes (the parent plan's §1 non-goal
  stands);
- reusing one connection for the read and the push of the same repository;
- read deduplication and change detection for tag publication;
- protocol changes other than the one additive push option (step 3.2).

## 2. Facts this plan rests on

### 2.1 Code

All paths are in gwz-core at `c9c7a98`.

- **Where member pushes go.** Each member push goes to the member's configured
  destination. `prepare` captures the push remote's `pushurl`, or its `url`
  when there is no `pushurl` (`src/git/gitbackend/push_plan.rs:12-17`).
- **Every captured push connects.** Each member with a captured plan is pushed
  (`push_member.rs:306-311`), even when the remote already holds its commit.
  `perform_push` skips the connection only when a plan has no refspecs
  (`transport.rs:485-490`).
- **Where root publication reads.** Root publication proves every lock member
  at the **committed URL**: the fetch remote that the committed root manifest
  records (`src/workspace_ops/publication.rs:289-297`). It reads that URL in
  three places:
  - identity validation, `validate_dependency_identity` (143-153);
  - the pre-transfer read, `preflight_dependencies_with_reads` (192-216);
  - the post-push proof, `checked_root_request` (68-108).
- **Both shortcuts compare URLs exactly.**
  - The pre-transfer dedup keys on `(identity repo, remote, URL)`
    (`ReadPreflight`, 11-35). Member reads are recorded at
    `push_member.rs:233-237`.
  - The published shortcut requires `plan.url == dependency.url`
    (`dependency_was_published`, 116-133).
- **Reads run one at a time.** The pre-transfer loop (`push_member.rs:222-257`)
  and the post-push proof read each destination in turn. Pushes already run
  through `par_map_per_host` (`src/operation/par_map_per_host.rs:39`) under
  `--jobs` and `--max-per-host`.
- **Tracking refs follow gwz pushes.** `push_prepared` pushes through the named
  remote when its configured URL matches the captured one, which "preserves
  normal tracking updates" (`transport.rs:459-472`). The gwz-dev reflogs agree:
  `refs/remotes/origin/main` records `update by push` at 14:21:38 on 2026-09-14
  for gwz-core and gwz-cli, and 22 s later for the root.
- **No last-known-state query.** The backend contract has no query for a
  destination's remote-tracking ref or for ahead/behind state. `is_ancestor`
  exists (`publication.rs:92`).
- **The scheme-only predicate already exists.**
  `scheme_only_difference(manifest_url, observed_url)`
  (`src/git/url_scheme.rs:161-177`) is true exactly when deriving the manifest
  URL into the observed URL's scheme on a known host yields the observed URL.
  The known hosts are github.com, gitlab.com and bitbucket.org.
  - `gwz repo sync` already uses this rule to keep a manifest URL
    (`handle_create_repo.rs:779`).
  - An https clone's member remotes satisfy it by construction, because they
    are `derive(manifest, https)`.
- **Scheme precedence.** `resolve_url_scheme` (`url_scheme_state.rs:72-91`)
  resolves the effective scheme from the request first, then
  `.gwz/url-scheme.yml`, then `manifest`.
- **Tag publication shares the proof.** Tag publication
  (`handle_tag.rs:125-130`, `151`, `163-175`, `287-298`) calls the same
  `root_dependencies`, `preflight_dependencies` and `checked_root_request`. It
  has no dedup and passes an empty published map.
- **Credentials.**
  - For SSH, gwz offers the agent once.
  - A username and password come from configured git credential helpers
    (`transport_support.rs:195-235`). The production policy is
    `AllowConfigured` (`backend.rs:44`).
  - A per-remote SSH identity named for a non-SSH URL is refused
    (`transport_support/identity.rs:200-204`).
- **Push options and request types.** `gwz push` already has `--force` (a forced
  push), `--partial`, `--sync <mode>` (workspace sync policy), `--jobs` and
  `--max-per-host`. Request types are generated from `protocol/gwz.taut.py` by
  `protocol/regen.py`.
- **The test seam cannot see URLs.** The g01 tracking backend:
  - has no remotes (`tests/g01/tracking_backend.rs:421-423`);
  - makes up `ssh://<name>.invalid/repo.git` push URLs (453-467);
  - ignores the URL passed to `ls_remote_url` (469-477).

### 2.2 Measurements

Measured on this Mac against github.com on 2026-09-14:

| what | result |
|---|---|
| one SSH session (ls-remote) | 2.4–2.6 s |
| one HTTPS session | 0.5–0.6 s (TCP 12 ms, TLS 21 ms) |
| `gwz push` 1.0.12 of gwz-dev (8 members, SSH everywhere) | 26 connections, 44 s |
| `gwz push` 1.0.12 from a fresh https clone, nothing to publish (step 0.2) | 34 connections (18 https + 16 SSH), 42.4 s |

### 2.3 Session counts

These come from reading the code; step 0.2 confirmed the 34, and step 4.2
measures the rest.

**By URL scheme (Phase 2).** A whole-workspace push of N members plus the root:

| workspace | 1.0.12 | `c9c7a98` | after Phase 2 |
|---|---|---|---|
| SSH remotes, SSH manifest (gwz-dev) | 3N+2 SSH (26) | 2N+2 SSH (18) | 2N+2 SSH (18), unchanged |
| `--url-scheme https` clone, SSH manifest | 2N+2 https + 2N SSH (34) | the same (34): dedup and shortcut both miss | 2N+2 https (18), 0 SSH |

A root-only push uses 2N+2 sessions in every column. In an https clone before
Phase 2, 2N of them are SSH.

**By what changed (Phase 3).** Any workspace after Phase 2, with N = 8:

| push | `c9c7a98` | check once (3.3) | default (3.5) | with the option (3.5) |
|---|---|---|---|---|
| whole workspace, nothing to publish | 2N+2 (18) | N+1 (9) | 0 | N+1 (9) |
| whole workspace, one member and the root changed | 2N+2 (18) | N+3 (11) | 4 | N+3 (11) |
| root only, root changed | 2N+2 (18) | N+2 (10) | 2 | N+2 (10) |

The default column assumes each unchanged member's lock commit is at or behind
its last-known ref (§3.5). Each dependency that is not adds one read.

## 3. Behaviour contract

### 3.1 Same repository (Phase 2)

A candidate URL is the **same repository** as the committed URL in exactly two
cases:

- `candidate == committed`;
- `scheme_only_difference(committed, candidate)` is true.

Everything else counts as a different repository and keeps today's behaviour:

- an unknown host;
- a different `.git` suffix;
- an uppercase host, a user or an explicit default port in the candidate;
- an `ssh://` candidate that is not the committed URL itself;
- any nonstandard port.

The rule can only cost an extra read. It never skips a proof.

### 3.2 The read URL (Phase 2)

Each root-lock dependency gets one **read URL**. `root_dependencies` chooses it
once. The same URL is then used for:

- identity validation;
- the pre-transfer read;
- the dedup key;
- the published shortcut;
- the post-push proof.

"That remote" below means the member repository's remote with the name that the
committed fetch remote records. The first rule that applies wins:

1. **Materialized member, push destination.** If that remote's push destination
   (`pushurl`, or else `url`) is the same repository, use the push destination.
2. **Materialized member, fetch URL.** Otherwise, if that remote's fetch `url`
   is the same repository, use the fetch URL.
3. **Member not materialized.** Use `derive(committed, effective scheme)`. The
   scheme is resolved the way materialize resolves it: the request, then
   `.gwz/url-scheme.yml`, then `manifest`. If the derivation is refused, use the
   committed URL.
4. **Otherwise.** Use the committed URL, as today.

Rule 1 checks the push destination before the fetch URL for a reason. A member
that this operation publishes is then read, deduplicated, pushed and proven at
one URL through one authentication path, so the existing exact comparisons still
hold (`plan.url == read_url`).

Some remotes have a push URL that is deliberately unusable, such as `DISABLE`
or a fork. That URL fails the same-repository test, so the rules fall through to
the next one.

### 3.3 What the operator sees (Phase 2)

- **Proof refusal.** When the read URL differs from the committed URL, the
  refusal names it. For example: "... available at its committed fetch remote
  origin (read through https://github.com/o/r.git)".
- **SSH identity with an https read URL.** A per-remote
  `--remote-identity NAME=PATH` for a remote whose read URL is https is refused
  before any transfer, with today's message. That member's own push is already
  refused the same way (D4).
- Nothing else changes in JSON or human output.

### 3.4 What Phase 2 leaves alone

- Push never writes the manifest, the lock or member remotes.
- In an SSH workspace the read URLs equal the committed URLs, so Phase 2 leaves
  its reads byte-identical to today's. Step 1.2 pins this until Phase 3 changes
  the counts on purpose.
- A proof that cannot be made still refuses root publication.

### 3.5 Contacting a repository (Phase 3)

Two definitions:

- **Kept advertisement:** the refs one read returned during this operation, for
  one destination (identity repo, remote and read URL).
- **Last-known ref:** the push destination mapped through the named remote's
  fetch refspecs (`refs/heads/main` becomes `refs/remotes/origin/main`). gwz
  uses it only when the remote's push URL is absent or is the same repository
  as its fetch URL. It does not use the branch's configured upstream, which not
  every checkout has.

**Rule 1: check once (both modes).** gwz reads a destination at most once per
operation and answers later questions from the kept advertisement:

- A selected repository whose advertisement already shows every destination ref
  at its source object is `Noop` ("already on origin") and is not pushed.
- The root dependency preflight answers from the kept advertisement.
- The post-push proof answers from the kept advertisement for every destination
  this operation did not push to (D9). For a destination it did push to, the
  accepted push is the proof: it covers the pushed commit and every ancestor of
  it (D8). Anything else is read, as today.

**Rule 2: contact only what changed (default).** Before any read, gwz classifies
each selected repository against its last-known ref:

| local branch compared with the last-known ref | ordinary push | forced push |
|---|---|---|
| equal | `Noop`, not contacted | `Noop`, not contacted |
| behind | `Noop`, not contacted (D6) | contacted |
| ahead | contacted | contacted |
| diverged | contacted; the remote rejects it, as today | contacted |
| no last-known ref | contacted | contacted |

- A root that is not contacted publishes nothing, so no dependency is proven.
- When the root is published, a dependency whose lock commit is at or behind a
  last-known ref of the remote chosen by §3.2 rules 1–2 is proven by that ref,
  without a read. Dependencies under §3.2 rules 3–4 are read.
- Contacted repositories go through rule 1.

**Rule 3: always check (the option).** The option (D7) skips the classification.
Every selected repository and every root dependency is read once, and rule 1
applies.

`--dry-run` shows the classification and still makes no connections.

### 3.6 What the operator sees (Phase 3)

- Every `Noop` row carries its reason: "already on origin" (read in this
  operation), "up to date with origin/main as of the last fetch or push", or
  "behind origin/main as of the last fetch or push".
- Human output adds one summary line when repositories were not contacted, for
  example "7 repositories unchanged since the last fetch or push, not contacted;
  --check-remotes to verify".
- The aggregate status is `Noop` when nothing was pushed, as today
  (`push_member.rs:764`).

### 3.7 The default's accepted risk

A last-known ref records what the remote held at the last fetch or push.

- **Remote moved ahead** (someone else pushed): skipping is still right. There
  is nothing to publish, and the lock commit is still on the remote.
- **Remote rewound or deleted** since the last fetch or push: the default does
  not notice. A root published on that basis can name a commit its member
  remote no longer advertises. The option, or any fetch, closes the gap. The
  owner accepted this on 2026-09-14 (D5).
- **Last-known ref left behind** (for example after a push through an anonymous
  remote, `transport.rs:471`): costs one contact, never a skipped push.

## 4. Decisions for the owner

All nine were decided on 2026-09-14.

- **D1 (decided 2026-09-14): what counts as the same repository** is only the
  rule in §3.1. Rejected for now: a canonical URL form that also folds case,
  `.git`, users and default ports, which needs a soundness argument for each
  host.
- **D2 (decided 2026-09-14): push destination before fetch URL** (§3.2). It
  matters only for a remote with a separate push URL; no gwz-dev repository had
  one on 2026-09-14.
  - Rejected: check the fetch URL first. For a remote that fetches over https
    and pushes over SSH, a pushed member would get an extra read before the
    push and another after it, because its push URL and read URL differ.
  - Accepted cost: a member this operation does not push is still read through
    its push URL, so a root-only push with `--check-remotes` reads it over SSH
    even though an https fetch URL exists.
- **D3 (decided 2026-09-14): unmaterialized dependencies follow the effective
  scheme.** A malformed `.gwz/url-scheme.yml` refuses push, as it already
  refuses materialize. Rejected: always use the committed URL, which keeps one
  SSH session for each unmaterialized member.
- **D4 (decided 2026-09-14): an SSH identity override with an https read URL is
  refused.** Rejected: silently read the committed URL for that remote.
- **D5 (decided 2026-09-14): contact only what changed by default,** with an
  option that keeps today's check of every remote.
- **D6 (decided 2026-09-14): a branch behind its last-known ref is `Noop`
  without contact** on an ordinary push, with its reason shown. Rejected:
  contact the remote and let it reject the push, as today.
- **D7 (decided 2026-09-14): the option is `--check-remotes`,** carried as an
  additive `remote_check` field on `PushRequest` (`changed`, the default, or
  `always`). Rejected: the working name `--force-check`, which reads as a kind
  of `--force` (a forced push on `gwz push`).
- **D8 (decided 2026-09-14): an accepted push of commit Y proves every ancestor
  of Y.** A remote accepts a ref update only with the full history of the new
  object. This relaxes the intentionally exact comparison in
  `dependency_was_published` (`publication.rs:110-133`). Rejected: keep the
  exact comparison and read.
- **D9 (decided 2026-09-14): the post-push proof reuses this operation's
  pre-transfer advertisement** for destinations it did not push to. This
  replaces the deliberate second read described at `publication.rs:187-191`.
  Rejected: keep the second read, whose only extra coverage is a rewind by
  someone else during the operation.

## 5. Phases and steps

### Conventions

- Foundational steps come first. Steps in the same phase are independent unless
  a step says otherwise.
- Budgets are aspirational: under 500 LOC per step, tests included.
- One commit per step, member-scoped through gwz:
  `gwz add <paths> --target <member>`, then
  `gwz commit --target <member> -m ...`. Files the root owns (`dev-docs/`)
  commit with `--target @root`.
- Before each commit, the suites of the touched members run green (gwz-core:
  `python3 scripts/run_tests.py`).
- Every control-flow body is braced, and there is no `#[cfg]` on a bare import.
- No AI attribution trailer.

### Phase 0: preconditions (no product code)

**Step 0.1: adoption and baseline.** Docs only.

- D1–D9 were decided on 2026-09-14.
- Record the root and member heads, and confirm that `c9c7a98` is present.
- Run the gwz-core suite once to get the reference count.
- File this plan (root commit).

**Step 0.2: HTTPS publication spike.** The owner runs it or authorizes it. It
uses the installed 1.0.12, runs in parallel with Phase 1, and changes no remote.

1. In a scratch directory outside gwz-dev, run
   `gwz clone --url-scheme https https://github.com/owebeeone/gwz-dev.git <dir>`.
2. Export the owebeeone agent socket, because the 1.0.12 proof reads still use
   SSH and the private member needs an account with access.
3. Run `gwz --json push` while polling the gwz process's sockets
   (`lsof -a -p <pid> -iTCP`) for ports 22 and 443, as on 2026-09-14.
4. The push has nothing to publish, so the remotes do not change. If gwz reports
   anything to publish, or preflight rejects a member (for example, a detached
   HEAD), stop and record it. The owner then picks a real publication for the
   spike instead.

Record:

- whether each member push authenticates over https through the helper:
  `meta.transport` rows show `credential_method: helper` and
  `authenticated: true`;
- the session count by port. Expected: 18 https + 16 SSH;
- wall-clock time.

If https push does not authenticate, stop at the end of Phase 1 for an owner
decision: without it, Phase 2 has no user-visible value. The evidence goes into
the "before" section of the step-4.2 note.

**Result (2026-09-14, gwz 1.0.12): https push authenticates.**

- **Clone.** `gwz --json clone --url-scheme https` took 5.7 s over 9 port-443
  connections and no SSH. All 8 members materialized; the private member
  authenticated through a credential helper.
- **Dry run.** No connections. Every member and the root planned
  `refs/heads/main:refs/heads/main` at a commit level with `origin/main`
  (`+0 -0`, clean), so the push had nothing to publish.
- **Push.** Exit 0 after 42.4 s; every member and the root `ok`.
  - Connections: 18 on port 443 and 16 on port 22, first seen in the order
    443×9, 22×8, 443×8, 22×8, 443×1. That is the member and root reads, the SSH
    dependency reads, the member pushes, the SSH proof reads, then the root
    push: exactly §2.3's 2N+2 https + 2N SSH.
  - Transport rows: all 9 `push` rows `helper`, `authenticated: true`; 16 SSH
    reads via `agent`; the private member's read via `helper`; 8 anonymous
    https reads.
- **Not established.** Which helper answered (gwz does not report it; `gh`, whose
  account is `owebeeone`, is configured for github.com ahead of the global
  `osxkeychain`), and a real pack upload over https, since nothing needed
  sending.

### Phase 1: foundation (parallel-friendly, no behaviour change)

**Step 1.1: same-repository rule and read-URL selection.** Pure functions with
no I/O.

- `same_repository(committed, candidate)` goes beside `scheme_only_difference`
  in `src/git/url_scheme.rs`.
- `select_read_url` goes in a new `src/workspace_ops/publication_url.rs`.
  - Inputs: the committed URL, plus either the member's `GitRemote` (for a
    materialized member) or the effective scheme (for a member that is not
    materialized).
  - Output: the URL and which rule of §3.2 chose it.

Unit tests are table-driven and cover:

- each rule of §3.2;
- each "different repository" case of §3.1;
- a fork push remote and a `DISABLE` push URL;
- an https fetch URL with an SSH push URL;
- an unmaterialized member under `ssh`, `https` and `manifest`;
- a refused derivation, for example `ssh://git@github.com:2222/...` under
  `https`.

Depends on nothing. Leaf owner. Budget about 200 LOC. Commit: gwz-core.

**Step 1.2: an observable seam and a characterization of today.** Extend
`tests/g01/tracking_backend.rs`, or the backend the g08 publication tests build:

- remotes configurable per path (`name`, `url`, `push_url`);
- `prepare_push` returns the configured destination when there is one, and
  today's made-up URL otherwise;
- recorded lists of `ls_remote_url` calls as `(path, url, remote, identity
  repo)`, and of `push_prepared` calls with their URLs.

Pin today's read and push sequence with three tests:

- **(a)** A whole push of two members plus the root, with configured URLs equal
  to the committed URLs. Expect N+1 reads and N+1 pushes: the dependency
  preflight hits the dedup and the proof hits the shortcut.
- **(b)** A root-only push. Expect 2N+1 reads and one push.
- **(c)** Member remotes that are the https form of SSH committed URLs. Expect
  today's SSH dependency and proof reads. This test is the gap, and step 2.1
  flips it.

Depends on nothing. Leaf owner. Budget about 300 LOC. Commit: gwz-core.

### Phase 2: https end to end

**Step 2.1: read through the selected URL.**

Implementation:

- `PublicationDependency` gains `read_url` and the rule that chose it.
- `root_dependencies` fills `read_url` with step 1.1's function:
  - for a materialized member, it calls `backend.remotes(member path)`, which
    reads local config and makes no network call;
  - it resolves the effective scheme lazily, only when a dependency is not
    materialized.
- `validate_dependency_identity`, `preflight_dependencies_with_reads` and
  `checked_root_request` read `read_url`.
- `dependency_was_published` compares `plan.url` with `read_url`.
- The proof refusal names the read URL when it differs from the committed URL.
- The push dry run (`push_member.rs:533-541`) and tag publication inherit the
  change through `root_dependencies`.

Tests on the step-1.2 seam:

- 1.2(c) flips: zero SSH reads, a dedup hit, a shortcut hit, and N+1 reads in
  total.
- 1.2(a) and (b) are unchanged, byte for byte.
- A fork push remote reads the committed URL and gets no shortcut.
- A remote that fetches over https and pushes over SSH reads the SSH push
  destination.
- An https remote on an unknown host reads the committed URL.
- An unmaterialized dependency with `.gwz/url-scheme.yml` set to `https` reads
  the derived https URL with no identity repo.
- A malformed `.gwz/url-scheme.yml` gets the existing refusal.
- A per-remote SSH identity with an https read URL gets the existing identity
  refusal before any transfer.
- `a_completed_member_push_counts_as_publication_with_or_without_force` is
  updated for the new field.

Depends on 1.1 and 1.2. Integration owner. Budget about 350 LOC. Commit:
gwz-core.

**Step 2.2: tag publication parity.** Tests only, unless they find a gap.

- In a workspace whose member remotes are https and whose committed URLs are
  SSH, pushing a root tag reads its dependencies only through https.
- The tag path's validation, preflight and proof sites are
  `handle_tag.rs:125-130`, `163-175` and `287-298`.
- An all-SSH tag push is unchanged.

Depends on 2.1. Leaf owner. Budget about 150 LOC. Commit: gwz-core.

### Phase 3: contact only what changed

Milestone: a push with nothing to publish makes no connections, and with the
option each destination is read once.

**Step 3.1: last-known-state classification.** A pure function in a new
`src/workspace_ops/push_state.rs`.

- Inputs: the source object, the last-known ref's object if there is one,
  whether the push is forced, and the ancestry of the two objects.
- Output: equal, behind, ahead, diverged or unknown, with the contact decision
  of §3.5 rule 2.
- Unit tests cover every cell of the §3.5 table.

Depends on nothing, so it can start alongside Phase 1. Leaf owner. Budget about
150 LOC. Commit: gwz-core.

**Step 3.2: protocol field for the option.** Add `remote_check` (`changed` or
`always`; absent means `changed`) to `PushRequest` in `protocol/gwz.taut.py`,
per D7.

- Regenerate with `protocol/regen.py`: generated code, corpus and message
  catalog.
- Every `PushRequest { .. }` literal gains `remote_check: None`; there are three
  in `handle_tag.rs` alone. The compiler finds them.
- Regenerate the gwz-py bindings and move the drift pin in
  `gwz-py/scripts/check_protocol_drift.py` with a dated note, as the parent
  plan's step 1.2 did.

Depends on nothing, so it can start alongside Phase 1. Integration owner, since
it changes the contract. Budget about 150 hand-written LOC plus generated
output. Commits: gwz-core, then gwz-py.

**Step 3.3: check once.** `ReadPreflight` keeps each advertisement it reads,
keyed as today by identity repo, remote and read URL. Then, in `push_member.rs`
and `publication.rs`:

- After the pre-transfer reads, a selected repository whose kept advertisement
  shows every destination ref at its source object is `Noop` ("already on
  origin") and is not pushed.
- `preflight_dependencies_with_reads` answers from a kept advertisement instead
  of only skipping the read.
- `checked_root_request` proves a dependency from its kept advertisement when
  this operation did not push to that destination (D9). For a destination it
  pushed to, `dependency_was_published` accepts a lock commit that is the pushed
  source or one of its ancestors (D8). Anything else is read, as today.

Tests on the step-1.2 seam:

- the check-once counts of §2.3's second table, with N from the fixture; 1.2(a)
  and (b) move to these counts;
- a destination pushed in this operation is never proven from its pre-push
  advertisement;
- an advertisement without the destination ref still pushes;
- a member whose head is ahead of its lock commit is proven by its push, with
  no read.

Depends on 1.2 and 2.1, which edit the same functions. Integration owner.
Budget about 350 LOC. Commit: gwz-core.

**Step 3.4: concurrent reads.** Run the pre-transfer reads
(`push_member.rs:222-257`) and any remaining proof reads through
`par_map_per_host`, under the same `--jobs` and `--max-per-host` policy as
pushes.

- Plan the set of destinations first, so a destination shared by a member and a
  dependency is still read once.
- Keep the report order, and keep the rule that every read failure is reported
  and no transfer starts.

Tests: the report order is unchanged; failure aggregation is unchanged; the
per-host limit holds (the tracking backend counts concurrent calls).

Depends on 3.3. Integration owner. Budget about 250 LOC. Commit: gwz-core.

**Step 3.5: contact only what changed.**

- **Backend.** A contract query for a destination's last-known ref. It answers
  `None` by default, so fakes and every unknown case contact the remote. The
  Git2 implementation maps the destination through the named remote's fetch
  refspecs and answers only under §3.5's push-URL condition.
- **Push.** With `remote_check` absent or `changed`, classify each selected
  repository (step 3.1) before any read, and apply §3.5 rule 2. With `always`,
  skip the classification (rule 3).
- **Output.** The `Noop` reasons of §3.6, in JSON and in `--dry-run`.

Tests:

- nothing changed: no transport calls;
- one member and the root changed: reads and pushes for those two only;
- a lock commit that no last-known ref contains is read, and refused when the
  remote lacks it;
- `always` gives the check-once counts;
- no last-known ref, a renamed remote, and a push URL that is a different
  repository all contact the remote;
- a forced push of a branch behind its last-known ref contacts the remote;
- a last-known ref left behind contacts the remote, then finds nothing to push.

Depends on 3.1, 3.2 and 3.4. Integration owner. Budget about 400 LOC. Commit:
gwz-core.

**Step 3.6: Rust CLI option.** `gwz push --check-remotes` (name per D7), mapped
to `remote_check: always`.

- Help text, then regenerate the CLI reference with
  `python scripts/generate_cli_reference.py --write` and confirm with
  `--check`.
- Human output: the summary line of §3.6, and each `Noop` reason under
  `--verbose`.
- Tests in `gwz-cli/tests`: parsing, JSON passthrough, the summary line.

Depends on 3.2, and on 3.5 for the end-to-end assertions. Leaf owner. Budget
about 200 LOC. Commit: gwz-cli.

**Step 3.7: gwz-py option.** The same option on gwz-py's push parser
(`src/gwz/cli_shared.py` or `cli_mutation.py`), the request field, and rendering
of the `Noop` reasons. Tests run through `run_tests.py`.

Depends on 3.2, and on 3.5 for the end-to-end assertions. Parallel with 3.6.
Leaf owner. Budget about 150 LOC. Commit: gwz-py.

### Phase 4: finish

**Step 4.1: documentation.** In gwz-cli's docs where publication is described
(Concepts, Troubleshooting, QuickStart "Clone An Existing Workspace"), cover:

- an https clone publishes over https end to end;
- the rule for the URL the root proof reads, in one sentence;
- how to switch an existing SSH workspace: either
  `git -C <path> remote set-url origin https://github.com/<owner>/<repo>.git`
  for the root and each member, or a fresh clone with `--url-scheme https`.
  `gwz repo sync` keeps the manifest URL (parent plan §2.8);
- the identity refusal of §3.3;
- a push contacts only repositories that changed since the last fetch or push,
  and what each `Noop` reason means;
- `--check-remotes`, and when to use it: when someone else may have rewound a
  remote, or before a release.

Also:

- If gwz-core `dev-docs/GWZDesign.md` names the committed URL as what the proof
  reads, or describes the second read, update that paragraph.
- Regenerate the CLI reference only if help text changes beyond step 3.6.

Depends on 2.1 and 3.6. Leaf owner. Budget about 180 LOC. Commits: gwz-cli, and
gwz-core if `GWZDesign.md` changes.

**Step 4.2: acceptance against GitHub.** The owner runs it with a binary built
from the committed tree and installed to a scratch `--root`. It never replaces
`~/.cargo/bin/gwz` and is never a `target/` build against gwz-dev.

Capture JSON, poll sockets and record wall-clock time for every case. Each push
has nothing to publish unless the owner names a real publication.

1. A fresh `--url-scheme https` clone, whole push: no sockets.
2. The same clone with `--check-remotes`: N+1 port-443 sessions (one read per
   repository), no port-22 sessions, and no pushes.
3. The same clone, root-only push with `--check-remotes`: port-443 sessions
   only.
4. gwz-dev itself (SSH), whole push with `--check-remotes`: N+1 port-22
   sessions and no pushes.
5. A scratch member that fetches over https and pushes over SSH, with
   `--check-remotes`: that member is read over SSH only.
6. One member and the root changed, default, as a real publication the owner
   names: 4 sessions.
7. Optional: a tag push, only for a tag the owner wants published.

Write the results to `dev-docs/GwzUrlSchemePushAcceptance-<date>.md` (root
commit). Include:

- the "before" numbers from step 0.2;
- wall-clock times;
- the account each transport used;
- what remains open.

Depends on 0.2, 2.2, 3.5, 3.6, 3.7 and 4.1.

### Dependency summary

```text
0.1 -> {1.1, 1.2} -> 2.1 -> {2.2, 3.3}
3.3 -> 3.4 -> 3.5
{3.1, 3.2} -> 3.5            (3.1 and 3.2 can start alongside Phase 1)
3.2 -> {3.6, 3.7}            (end-to-end assertions after 3.5)
{2.2, 3.5, 3.6, 3.7} -> 4.1 -> 4.2
0.2 (ran 2026-09-14) -> 4.2
```

## 6. Test matrix (what "done" means)

| behaviour | test | step |
|---|---|---|
| same-repository rule and every read-URL rule, including each "different" case | unit | 1.1 |
| today's read and push sequence over SSH, whole and root-only push | core, tracking backend | 1.2 |
| https member remotes with SSH committed URLs: no SSH reads, dedup and shortcut hit | core | 2.1 |
| fork, split fetch/push and unknown-host remotes resolve as §3.2 specifies | core | 2.1 |
| unmaterialized dependency follows `.gwz/url-scheme.yml`; malformed file refuses | core | 2.1 |
| SSH identity override with an https read URL refuses before any transfer | core | 2.1 |
| SSH workspace reads unchanged by Phase 2 | core (1.2 tests unchanged) | 2.1 |
| root tag push reads through the read URL | core | 2.2 |
| last-known-state classification, every cell of §3.5 | unit | 3.1 |
| `remote_check` round-trips; py drift pin | generated corpus, `check_protocol_drift.py` | 3.2 |
| check once: §2.3 counts; no proof from a pre-push advertisement; an ahead member proven by its push | core, tracking backend | 3.3 |
| concurrent reads: report order, failure aggregation, per-host limit | core | 3.4 |
| default: no transport calls when nothing changed; only changed repositories contacted; unknown cases contact | core | 3.5 |
| option parsing, JSON passthrough, human summary | gwz-cli and gwz-py tests | 3.6, 3.7 |
| real GitHub: nothing to publish makes no connections; the option reads each repository once over its own scheme | manual, acceptance note | 4.2 |

## 7. Risks and open points

- **HTTPS push through libgit2 authenticates (step 0.2, 2026-09-14).**
  - This Mac resolves github.com credentials through
    `credential.https://github.com.helper` (an empty reset, then
    `gh auth git-credential`) and a global `osxkeychain` helper. All 9 pushes
    of the spike authenticated through one of them.
  - Still open: which helper answered; a machine with only `osxkeychain`, or
    with no helper; and a real pack upload over https, which the first real
    publication from an https workspace will cover.
- **The account depends on the transport.**
  - SSH with the default agent is `gripd`, which cannot push. SSH with the
    explicit socket is `owebeeone`.
  - https through the `gh` helper is `owebeeone`.
  - The acceptance note names the account for each case.
- **Private members over https.** An https read of a private member needs helper
  credentials where the SSH read used keys.
  - An https clone could only have materialized that member with those
    credentials, so the exposed case is a remote someone switched by hand.
  - The refusal names the read URL. The remedy is a credential helper, or
    reverting that remote.
- **Stale last-known refs.** The default trusts them. §3.7 lists what that
  misses, and D5 records the owner's choice.
- **The test seam cannot see URLs today.** Step 1.2 exists so that the changes
  in 2.1 and 3.3 are measured against a pinned sequence, not against memory.
- **The base is unpushed.** If `c9c7a98` is reworked, 2.1's shortcut comparison
  follows the rework.
- **Open: connection reuse.** A changed repository still costs one read and one
  push. Reusing the read's connection for the push is a separate item.
- **Open: sockets, not transport rows.** `TransportObservation` carries no URL
  or scheme, so acceptance counts sockets. Adding a field is a protocol change
  for a later plan.

## 8. Reporting at each stop

- Each step's commit message names the step.
- The step-4.2 note is the acceptance evidence: session counts by port before
  and after, wall-clock times, accounts, and what remains open.
- The final report lists short hashes per member.
