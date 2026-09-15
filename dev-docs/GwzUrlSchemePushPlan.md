# GWZ push: prove root dependencies through the member's own destination, and contact only what changed

Date: 2026-09-14. Status: **accepted at gwz-dev root `0136094` (gwz-core
`c9c7a98`, gwz-cli `2c4c0ea`, gwz-py `334f53f`, other members unchanged) after
`GwzUrlSchemePushPlan-ReviewConsistency-3.md` and
`GwzUrlSchemePushPlan-ReviewSafety-3.md` reported GO; this accepts the plan
text only. The round-3 P3 findings and residual wording were folded in
afterwards without re-review, as both reports allow (§9).**
Implementation: steps 1.1, 1.2, 3.1 and 3.2 landed on 2026-09-15, merged from
parallel lanes (gwz-core `3f5ff1e`, gwz-cli `9fa3189`, gwz-py `2921d09`, root
`a9ffadb`). §7 and step 3.2 were then corrected from what step 3.2 found.
Steps 2.1, 3.7 and 3.8 landed the same day from a second round of lanes
(gwz-core `5bf8f1a`, gwz-cli `31b14a3`, gwz-py `b97ff00`, root `c5d970c`).
Steps 2.2 and 3.3 followed from a third round (gwz-core `34cf67b`, root
`2bac332`), and step 3.4 from a fourth (gwz-core `d7b2d41`, root `fc69673`).
What the lanes found is recorded as "As landed" notes under steps 2.1, 2.2,
3.3, 3.4, 3.7 and 3.8, and as the row-status rule in step 3.5.
Step 0.2 ran on 2026-09-14.
Parent: `GwzUrlSchemePlan.md` (clone and materialize) and its acceptance note
`GwzUrlSchemeAcceptance-2026-09-12.md`.
Base: gwz-core `c9c7a98` / root `aa97cc1` ("Skip the second availability read
for members an ordinary push just published"). On 2026-09-14 that commit is
local and not yet pushed.
Reviews:
- round 1 at root `b64f691`, both NO-GO:
  `GwzUrlSchemePushPlan-ReviewConsistency.md` and
  `GwzUrlSchemePushPlan-ReviewSafety.md`; remediation
  `GwzUrlSchemePushPlan-RemPlan.md`;
- round 2 at root `f3a395b`: Consistency GO
  (`GwzUrlSchemePushPlan-ReviewConsistency-2.md`), Safety NO-GO
  (`GwzUrlSchemePushPlan-ReviewSafety-2.md`); remediation
  `GwzUrlSchemePushPlan-RemPlan-2.md`;
- round 3 at root `0136094`: Consistency GO
  (`GwzUrlSchemePushPlan-ReviewConsistency-3.md`) and Safety GO
  (`GwzUrlSchemePushPlan-ReviewSafety-3.md`).
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
- **Publishing the root always proves its dependencies** (D10, from the round-1
  review). When a push publishes the root, this operation proves every member
  commit the root's lock names; `origin/*` alone never does. (Phase 3.)

## 1. Goal and non-goals

**Goals:**

1. A workspace cloned with `gwz clone --url-scheme https` publishes with
   `gwz push` over https only, with zero SSH connections (Phase 2).
2. By default, a push contacts only repositories whose branch differs from what
   their origin last reported, plus a read of each root-lock dependency when
   it contacts the root (D10). A push with nothing to publish makes no
   connections (Phase 3).
3. With the option, a push reads every selected repository and every root
   dependency once, re-reading only where §3.5 rule 1 requires it, and proves
   the root's lock even when the root has nothing to push (Phase 3).

Under Phase 2, a contributor's SSH workspace whose member remotes equal their
committed URLs keeps reading exactly those URLs.

**Non-goals:**

- https URLs in manifests;
- a `--url-scheme` option on push;
- a command that rewrites existing remotes (the parent plan's §1 non-goal
  stands);
- pruning remote-tracking refs during fetch or pull;
- reusing one connection for the read and the push of the same repository;
- read deduplication and change detection for tag publication;
- protocol changes other than the one additive push option (step 3.2).

## 2. Facts this plan rests on

### 2.1 Code

Paths are in gwz-core at `c9c7a98` unless another repository is named.

- **Where member pushes go.** Each member push goes to the member's configured
  destination. `prepare` captures the push remote's `pushurl`, or its `url`
  when there is no `pushurl` (`src/git/gitbackend/push_plan.rs:12-17`), and
  resolves each refspec source to an object id (`push_plan.rs:111-114`).
- **Every captured push connects.** Each member with a captured plan is pushed
  (`push_member.rs:306-311`), even when the remote already holds its commit.
  `perform_push` skips the connection only when a plan has no refspecs
  (`transport.rs:485-490`).
- **Push reports no `Noop` today.** A successful push sets `Ok`
  (`push_member.rs:784-803`), so the all-`Noop` aggregate rule
  (`push_member.rs:764`) is unreachable for push; step 0.2 recorded every row
  `ok`. `PlannedChange { action: Noop, message }` exists
  (`handle_repo_lifecycle.rs:338`). A `Noop` aggregate exits 0 (gwz-cli
  `docs/MachineOutput.md:616-626`).
- **Where root publication reads.** Root publication proves every lock member
  at the **committed URL**: the fetch remote that the committed root manifest
  records (`src/workspace_ops/publication.rs:289-297`). It reads that URL in
  three places:
  - identity validation, `validate_dependency_identity` (143-153);
  - the pre-transfer read, `preflight_dependencies_with_reads` (192-216), which
    checks read access only;
  - the post-push proof, `checked_root_request` (68-108), which accepts a
    commit that is advertised or an ancestor of an advertised ref (86-99).
- **Both shortcuts compare URLs exactly.**
  - The pre-transfer dedup keys on `(identity repo, remote, URL)`
    (`ReadPreflight`, 11-35). Member reads are recorded at
    `push_member.rs:233-237`; unmaterialized dependencies are read without an
    identity repo and not recorded (`publication.rs:199-203`).
  - The published shortcut requires `plan.url == dependency.url`
    (`dependency_was_published`, 116-133).
- **Reads run one at a time.** The pre-transfer loop (`push_member.rs:222-257`)
  and the post-push proof read each destination in turn. Pushes already run
  through `par_map_per_host` (`src/operation/par_map_per_host.rs:39`) under
  `--jobs` and `--max-per-host`.
- **Tracking refs follow gwz pushes, and ignore the URL.** `push_prepared`
  pushes through the named remote when its configured URL matches the captured
  one, which "preserves normal tracking updates" (`transport.rs:459-472`).
  libgit2 1.9.7 (the vendored `libgit2-sys-0.18.8+1.9.7` that gwz-core's
  `Cargo.lock` pins) maps each accepted destination through the named remote's
  fetch refspecs and writes it with reflog "update by push", without looking at
  the push URL (`push.c:164-212`). The gwz-dev reflogs show `update by push` at
  14:21:38 on 2026-09-14 for gwz-core and gwz-cli, and 22 s later for the root.
- **Fetch never prunes.** gwz's fetch passes no prune option
  (`transport.rs:69-104`, `transport_support.rs:66-90`), and libgit2 prunes only
  when asked or when `remote.<name>.prune` or `fetch.prune` is set
  (`remote.c:582-586`, `1420-1430`). A tracking ref for a branch deleted on the
  remote survives `gwz pull` and a plain `git fetch`.
- **No last-known-state query.** The backend contract has no query for a
  destination's remote-tracking ref or for ahead/behind state. `is_ancestor`
  exists (`publication.rs:92`) and passes errors through
  (`src/git/gitbackend/refs.rs:285-296`).
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
  `.gwz/url-scheme.yml`, then `manifest`. The unreadable-file refusal names
  `--url-scheme manifest` as its remedy (`url_scheme_state.rs:109-117`).
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
    (`transport_support/identity.rs:200-204`); an invocation-wide default does
    not affect https (`identity.rs:206-207`).
- **Push options and request types.**
  - `gwz push` has `--partial`, `--sync <mode>` (workspace sync policy),
    `--jobs`, `--max-per-host` and the global `--force`.
  - `--force` sets `OperationPolicy.destructive = Allow` (gwz-cli
    `src/clirequest/invocation.rs:106`), which no push or publication code
    reads.
  - The Rust CLI builds `PushRequest` with `refspec: None`
    (`invocation.rs:255`); gwz-py's push command passes only `remote`
    (`cli_mutation.py:212-216`).
  - A forced push exists only as a `+`-prefixed `PushRequest.refspec` from an
    API caller, such as gwz-py `client.push(refspec=...)`; force is decided by
    that prefix (`publication.rs:44`, `push_plan.rs:18`).
  - Request types are generated from `protocol/gwz.taut.py` by
    `protocol/regen.py`; `docs/MessageCatalog.md` by
    `docs/generate_message_catalog.py`; the gwz-py bindings by gwz-py's
    `scripts/regen_protocol.py`.
- **The design promises proof at destinations.** `dev-docs/GWZDesign.md:96-98`:
  push "waits for successful member transfers and proof that all member objects
  named by the committed root lock are available at their destinations,
  including partial and root-only pushes." gwz-cli `docs/commands/push.md:75-83`
  and the module comment `publication.rs:1-3` say the same.
- **The test seam is thin.** The g01 tracking backend:
  - has no remotes (`tests/g01/tracking_backend.rs:421-423`);
  - makes up `ssh://<name>.invalid/repo.git` push URLs and returns refspecs
    unresolved (453-467);
  - ignores the URL passed to `ls_remote_url` (469-477);
  - serves no committed files (the contract default of `read_file_at_commit`
    is unsupported, `contract.rs:312-319`), so root publication cannot run on
    it;
  - reports every path as a repository (198-200), has one fixed head
    (413-419), and answers every ancestry question true (524-526);
  - refuses transport options through the contract default `with_transport`
    (`contract.rs:224-238`).

  The g08 publication tests use the real `Git2Backend` with local bare
  remotes, which records no calls.

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
| whole workspace, one member and the root changed | 2N+2 (18) | N+3 (11) | N+3 (11) | N+3 (11) |
| root only, root changed | 2N+2 (18) | N+2 (10) | N+2 (10) | N+2 (10) |

- The default saves connections when the root is not contacted. When it is,
  D10 reads every root-lock dependency once, in the same parallel round as the
  other reads (step 3.4).
- The counts assume the CLI's `refs/heads/<branch>:refs/heads/<branch>`
  refspec. The matching (`:`) and shorthand refspec forms make `prepare_push`
  read the remote before classification (`push_plan.rs:31-43`, `76-87`).

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
   scheme is resolved as core resolves it for materialize: a scheme in the
   request, then `.gwz/url-scheme.yml`, then `manifest`. Push clients send no
   scheme; `GWZ_URL_SCHEME` is read client-side for clone and materialize only.
   If the derivation is refused, use the committed URL.
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
- **SSH identity with an https read URL (D4).** A per-remote
  `--remote-identity NAME=PATH` for a remote whose read URL is https is refused
  before any transfer, with today's message.
  - For a member this operation pushes, that member's own push is already
    refused the same way.
  - A root-only push is newly refused: at `c9c7a98` it read the committed SSH
    URL and succeeded.
  - The refusal names the remedies that work: `--identity PATH` (an
    invocation-wide SSH default does not affect https reads), or no override
    for that remote.
- **Malformed `.gwz/url-scheme.yml` (D3).** A push with an unmaterialized
  dependency refuses. The refusal names the remedy that fits a push: delete or
  repair `.gwz/url-scheme.yml`. It does not suggest a bare `gwz materialize`,
  which materializes the lock (clones missing members, and moves or detaches
  checkouts). Deleting the file drops the workspace's scheme preference, so
  unmaterialized dependencies are read, and later cloned, at their manifest
  URLs; repairing it keeps the preference.
- Nothing else changes in JSON or human output.

### 3.4 What Phase 2 leaves alone

- Push never writes the manifest, the lock or member remotes.
- In an SSH workspace whose member remotes equal their committed URLs, the read
  URLs equal the committed URLs, so Phase 2 leaves its reads byte-identical to
  today's. Step 1.2 pins this until Phase 3 changes the counts on purpose.
- A proof that cannot be made still refuses root publication.

### 3.5 Contacting a repository (Phase 3)

Definitions:

- **Kept advertisement:** the refs one read returned during this operation, for
  one destination: identity repo (none for an unmaterialized dependency),
  remote name and read URL.
- **Same repository, between destinations:** two destination URLs reach the
  same repository when they are equal, or when `scheme_only_difference` holds
  in either direction. This relation calls unequal spellings of one repository
  different (an SSH host alias, a missing `.git` suffix, owner case). It is used
  only where "different" fails toward a contact or a read, which here means the
  last-known ref's push-URL condition below. It never limits invalidation.
- **Last-known ref:** the destination `refs/heads/<branch>` mapped through the
  named remote's fetch refspecs. gwz uses it only when all of these hold;
  otherwise there is no last-known ref:
  - the result lies under `refs/remotes/<remote>/`, and the remote's fetch
    refspecs map `refs/heads/*` exactly into that namespace with a forced
    refspec (`+`), as `git clone` and `git remote add` write it;
  - no other configured remote's fetch refspec writes into that namespace;
  - the remote's push URL is absent or is the same repository as its fetch URL.

  gwz does not use the branch's configured upstream, which not every checkout
  has.
- **Forced:** a destination whose captured refspec starts with `+`
  (`publication.rs:44`). `--force` sets `OperationPolicy.destructive`; it does
  not make a push forced and does not affect classification.

**Rule 1: check once (both modes).** gwz reads a destination at most once per
operation and answers later questions from the kept advertisement:

- A selected repository whose advertisement already shows every destination ref
  at its source object is `Noop` ("already on origin") and is not pushed.
- The root dependency preflight answers from the kept advertisement.
- The post-push proof uses a kept advertisement only when it proves
  availability (the commit is advertised, or is an ancestor of an advertised
  ref), and only for a destination this operation did not push to (D9).
  Otherwise it reads.
- For a destination this operation pushed to, the accepted push proves the
  pushed commit and every ancestor of it (D8).
- **A forced or deleting transfer invalidates evidence for the whole
  operation.** If this operation makes any forced or deleting transfer, no kept
  advertisement and no D8 proof counts as evidence for any dependency. When this
  operation proves the root, every root-lock dependency is read after all member
  transfers and before the root transfer. The rule covers the whole operation because URLs cannot tell which
  destinations share a repository.

**Rule 2: contact only what changed (default).** Before any read, gwz classifies
each selected repository against its last-known ref. Here "contacted" means read
for its own push decision, and pushed if rule 1 finds anything to send. A
repository that is not contacted can still be read once as a root-lock
dependency (D10); that read never changes its classification.

| local branch compared with the last-known ref | ordinary push | forced push |
|---|---|---|
| equal | `Noop`, not contacted | contacted |
| behind | `Noop`, not contacted (D6) | contacted |
| ahead | contacted | contacted |
| diverged | contacted; the remote rejects it, as today | contacted |
| no last-known ref, or an ancestry error | contacted | contacted |

- Equality compares object ids. An ancestry error (missing objects, shallow
  history) counts as unknown.
- A member's classification is final for its row and its push decision. If the
  D10 read of an uncontacted member shows its commit missing, the member stays
  `Noop` and the root is refused (§3.6).
- A root that is not contacted publishes nothing and is not proven.
- A contacted root is proven, whether rule 1 publishes it or finds it already on
  origin. Every root-lock dependency is proven by this operation (D10): by a
  read before any transfer, reused per rule 1, or by an accepted push (D8).
  Last-known refs never prove a dependency.
- Contacted repositories go through rule 1.

**Rule 3: always check (the option).**

- The option skips the classification. Every selected repository and every root
  dependency is read once, re-reading only where rule 1 requires it.
- A selected root that rule 1 makes `Noop` still runs the dependency proof after
  member transfers, from the same evidence a published root uses. A failure
  reports the root `Rejected` with today's message.

`--dry-run` shows the classification and still makes no connections.

### 3.6 What the operator sees (Phase 3)

- **Reasons (D11).** Every `Noop` row carries
  `planned: PlannedChange { action: Noop, message }` with its reason:
  - "already on origin" (read in this operation);
  - "up to date with origin/<branch> as of the last fetch or push";
  - "behind origin/<branch> as of the last fetch or push".
- **Summary line.** Human output adds one line when repositories were not
  contacted, for example "7 repositories unchanged since the last fetch or push
  were not checked for changes; --check-remotes to verify".
- **Missing dependency.** A root refused because a dependency is missing names
  the member, the commit and, when it differs from the committed URL, the read
  URL (§3.3). It suggests publishing the member (pushing a branch that contains
  the commit), or `gwz push --check-remotes`, which re-checks the members and
  pushes those whose remote lacks their branch's commit.
- **Machine-output compatibility change.**
  - At `c9c7a98`, a push of an unchanged workspace reports every row and the
    aggregate `ok` (step 0.2).
  - After Phase 3, a repository with nothing to push reports a `noop` row, and
    the aggregate is `noop` when nothing was pushed and nothing was refused. A
    `noop` aggregate exits 0.
  - JSON tells a checked `noop` from an assumed one only by the message text. A
    script that needs a result checked against the remote runs
    `--check-remotes`.
  - Step 4.1 records this in `MachineOutput.md`.

### 3.7 The default's accepted risks (D5, narrowed by D10)

A last-known ref records what gwz last saw, and gwz fetch and pull never prune.
Under the default, these refs decide only whether a member is contacted (checked
for changes and pushed). Every contacted root is proven by this operation (D10).

- **Remote moved ahead** (someone else pushed): for an ordinary push, skipping
  is still right. There is nothing to publish, and the member's commit is still
  on the remote.
- **Remote rewound** after its tracking ref was written: until the next fetch,
  the default treats the member as up to date and does not push it. A fetch
  updates the tracking ref of a branch that still exists, which ends this.
- **Branch deleted** on the remote after its tracking ref was written:
  - The default treats the member as up to date and does not push it.
  - A fetch does not repair this, because gwz does not prune. It lasts until a
    pruning fetch (`git fetch --prune`, or `fetch.prune`), a push outside the
    default classification that recreates the branch (for example `git push`),
    or `--check-remotes`. Until then, a script that runs only default
    `gwz push` is refused whenever it publishes a root that needs the commit.
- **A tracking ref written from a different repository** (a push URL since
  removed, a remote since re-pointed or renamed): the default compares the
  member with whatever the ref last recorded. A fetch through the remote
  refreshes branches its current repository has, a pruning fetch removes those
  it lacks, and `--check-remotes` reads the destination directly. The §3.5
  conditions exclude what the current configuration shows, not the history of
  the ref.
- In each of these cases, if a root being published needs that member's commit,
  the dependency read finds it missing and refuses the root (§3.6).
- **A published root that has since become unsound** (a member rewound after
  the root was published): the default does not detect it, because an
  unchanged root is not contacted. `--check-remotes` detects it (rule 3).
  `gwz tag --push` detects it only when it pushes a tag on that root commit.
- **Last-known ref left behind** (for example after a push through an anonymous
  remote, `transport.rs:471`): costs one contact, never a skipped push. For the
  root, that contact includes its dependency reads.

## 4. Decisions for the owner

All eleven were decided on 2026-09-14. D10 and D11 came from the round-1 review.

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
  scheme.** A malformed `.gwz/url-scheme.yml` refuses a push that has an
  unmaterialized dependency, as it already refuses materialize. Rejected: always
  use the committed URL, which keeps one SSH session for each unmaterialized
  member.
- **D4 (decided 2026-09-14): an SSH identity override with an https read URL is
  refused.** Rejected: silently read the committed URL for that remote.
- **D5 (decided 2026-09-14): contact only what changed by default,** with an
  option that keeps today's check of every remote. Narrowed by D10.
- **D6 (decided 2026-09-14): a branch behind its last-known ref is `Noop`
  without contact** on an ordinary push, with its reason shown. Rejected:
  contact the remote and let it reject the push, as today.
- **D7 (decided 2026-09-14): the option is `--check-remotes`,** carried as an
  additive `remote_check` field on `PushRequest` (`changed`, the default, or
  `always`). Rejected: the working name `--force-check`, which reads as a kind
  of the global `--force` ("allow destructive behaviour").
- **D8 (decided 2026-09-14): an accepted push of commit Y proves every ancestor
  of Y.** A remote accepts a ref update only with the full history of the new
  object. This relaxes the intentionally exact comparison in
  `dependency_was_published` (`publication.rs:110-133`). Rule 1's
  operation-wide invalidation applies. Rejected: keep the exact comparison and
  read.
- **D9 (decided 2026-09-14): the post-push proof reuses this operation's
  pre-transfer advertisement** when it proves availability, for destinations
  this operation did not push to. This replaces the deliberate second read
  described at `publication.rs:187-191`.
  - That second read covered a rewind during the operation, either by someone
    else or by a forced or deleting transfer in the same operation.
  - Rule 1's operation-wide invalidation turns the second kind back into a
    read.
  - Rejected: keep the second read.
- **D10 (decided 2026-09-14, from review finding Safety P1-1): publishing the
  root always proves its dependencies in this operation.** The proof is a read
  before any transfer (reused per D9) or an accepted push (D8); last-known refs
  never prove a root dependency.
  - Rejected: making tracking refs prune-aware and recording which repository
    wrote them.
  - Rejected: accepting the deleted-branch and re-pointed-remote cases as a
    risk for root publication. Their consequence for member pushes stays under
    D5 (§3.7).
- **D11 (decided 2026-09-14, from review finding Safety P2-4): a `Noop` row's
  reason is carried in `planned.message`.** Rejected: a typed field saying
  whether the result was checked or assumed, which would be a second protocol
  change.

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

- D1–D11 were decided on 2026-09-14.
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

Depends on 0.1. Leaf owner. Budget about 200 LOC. Commit: gwz-core.

**Step 1.2: an observable seam and a characterization of today.** Extend
`tests/g01/tracking_backend.rs` so it can express every case the later steps
test on it:

- remotes configurable per path (`name`, `url`, `push_url`, fetch refspecs);
- committed files per root commit (`read_file_at_commit`: manifest and lock),
  so root publication runs;
- `prepare_push` resolving refspec sources to object ids, as `Git2Backend`
  does (`push_plan.rs:111-114`), with the configured destination URL;
- per-path materialization (`is_repository`), heads, an ancestry table
  (`is_ancestor`, including errors), and advertisement stores for
  `ls_remote_url` that a recorded push can move, with several URLs able to map
  to one store, so that one repository can be reached through different
  spellings;
- a recorded non-forced push refused, as libgit2 refuses it before transfer,
  when the store's ref is not an ancestor of the pushed object;
- recorded lists of `ls_remote_url` calls as `(path, url, remote, identity
  repo)`, and of `push_prepared` calls with their URLs and refspecs;
- an overlap counter for `ls_remote_url`, like the existing fetch and push
  trackers.

Tests that need identity options (D4) run on the native `Git2Backend`, as g08
does, because the contract default `with_transport` refuses transport options.
Those refusals happen before any network.

Pin today's read and push sequence with three tests:

- **(a)** A whole push of two members plus the root, with configured URLs equal
  to the committed URLs. Expect N+1 reads and N+1 pushes: the dependency
  preflight hits the dedup and the proof hits the shortcut.
- **(b)** A root-only push. Expect 2N+1 reads and one push.
- **(c)** Member remotes that are the https form of SSH committed URLs. Expect
  today's SSH dependency and proof reads. This test is the gap, and step 2.1
  flips it.

Depends on 0.1. Leaf owner. Budget about 450 LOC. Commit: gwz-core.

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
- On the push path, the malformed-file refusal names deleting or repairing the
  file (§3.3).
- The identity refusal of §3.3 names `--identity`.
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
- A malformed `.gwz/url-scheme.yml` refuses a push with an unmaterialized
  dependency, naming deletion or repair of the file and not a bare
  `gwz materialize`.
- A proof refusal names the read URL.
- `a_completed_member_push_counts_as_publication_with_or_without_force` is
  updated for the new field.

Tests on the native backend: a per-remote SSH identity with an https read URL
refuses before any transfer, for a whole push and for a root-only push, naming
`--identity`.

Depends on 1.1 and 1.2. Integration owner. Budget about 400 LOC. Commit:
gwz-core.

As landed (gwz-core `5bf8f1a`, about 100 lines of code and 338 of tests):

- The `--identity` remedy is in `transport_support/identity.rs`, so clone,
  fetch and tag now show it as well as push. Step 4.1 can describe it once for
  every command.
- The three read sites take the identity repository from the read rule instead
  of calling `is_repository` again.
- `read_workspace_url_scheme` now takes the remedy text. Push asks for the file
  to be deleted or repaired, and materialize keeps its own wording.
- Step 2.1 corrected the first lines of the `publication.rs` module comment;
  step 3.3 still owns its rewrite.

**Step 2.2: tag publication parity.** Tests only, unless they find a gap.

- In a workspace whose member remotes are https and whose committed URLs are
  SSH, pushing a root tag reads its dependencies only through https.
- The tag path's validation, preflight and proof sites are
  `handle_tag.rs:125-130`, `163-175` and `287-298`.
- An all-SSH tag push is unchanged.

Depends on 2.1. Leaf owner. Budget about 150 LOC. Commit: gwz-core.

As landed (gwz-core `19232d7`, 16 lines of code and 139 of tests):

- **No gap.** Tag publication already validates, reads and proves each
  dependency through the URL step 2.1 selects. A mutation check switched each
  of the four sites back to the committed URL in turn: URL selection,
  validation, preflight read and proof read. Each time the https test failed
  and the all-SSH test still passed.
- **Test seam.** `handle_tag` accepts only the sealed `MergeAuthorityBackend`,
  which the recording test backend cannot implement. The body moved into a
  crate-internal `handle_tag_in` that takes any `GitBackend`, as
  `handle_push_with_events_in` does for push. The public entry point and the
  seal are unchanged.
- **Tests.** They are in `src/workspace_ops/tests/g26/tag_publication.rs` and
  pin which URLs are read, not how many reads happen. The test backend now
  models lightweight tags and records the URL passed to each identity check.
- **Selection.** A root tag push needs `--target @root` or `--all`, because
  tag selects only members by default. Step 4.2's tag case needs the same.

### Phase 3: contact only what changed

Milestone: a push with nothing to publish makes no connections; a push that
contacts the root proves every dependency in the operation; with the option,
each destination is read once (re-read only where rule 1 requires it) and an
unchanged root is still proven.

**Step 3.1: last-known-state classification.** A pure function in a new
`src/workspace_ops/push_state.rs`.

- Inputs: the source object, the last-known ref's object if there is one,
  whether the captured refspec starts with `+`, and the ancestry answer (true,
  false or error).
- Output: equal, behind, ahead, diverged or unknown (no ref, or an ancestry
  error), with the contact decision of §3.5 rule 2.
- Equality compares object ids, never ancestry.
- Unit tests cover every cell of the §3.5 table, including an ancestry error.

Depends on 0.1, so it can start alongside Phase 1. Leaf owner. Budget about
150 LOC. Commit: gwz-core.

**Step 3.2: protocol field for the option.** Add `remote_check` (`changed` or
`always`; absent means `changed`) to `PushRequest` in `protocol/gwz.taut.py`,
per D7.

- Regenerate: `protocol/regen.py` (generated code and corpus),
  `docs/generate_message_catalog.py` (`docs/MessageCatalog.md`), and gwz-py's
  `scripts/regen_protocol.py`. Move all three fingerprint pins of the protocol
  projection, each with a dated note, as the parent plan's step 1.2 did:
  - gwz-core `protocol/check_log_additive.py`, which `regen.py` checks before
    writing;
  - gwz-py `scripts/check_protocol_drift.py`;
  - gwz-py `src/tests/test_log_protocol.py`.
- Fan-out: every `PushRequest { .. }` literal gains `remote_check: None`:
  - gwz-core `src/workspace_ops/handle_tag.rs` (3 literals) and
    `src/workspace_ops/tests/g08.rs` (11 literals; 13 grep matches of
    `PushRequest {`, two of which are return types);
  - gwz-cli `src/clirequest/invocation.rs:255`.

  Check gwz-py's two `PushRequest(...)` calls in `src/gwz/client.py`
  (803, 813).
- Gate: after the step's commits, `cargo build --locked --bin gwz` from the
  gwz-dev root succeeds, and gwz-py's `run_tests.py` passes without
  `GWZ_RUST_BIN`.

Depends on 0.1, so it can start alongside Phase 1. Integration owner, since
it changes the contract. Budget about 160 hand-written LOC plus generated
output. Commits: gwz-core, gwz-cli, then gwz-py.

**Step 3.3: check once.** Implement §3.5 rule 1 in `publication.rs` and
`push_member.rs`:

- `ReadPreflight` keeps each advertisement it reads, keyed by identity repo
  (none for an unmaterialized dependency), remote and read URL.
- After the pre-transfer reads, a selected repository whose kept advertisement
  shows every destination ref at its source object is `Noop` ("already on
  origin") and is not pushed.
- `preflight_dependencies_with_reads` answers from a kept advertisement instead
  of only skipping the read.
- `checked_root_request` proves a dependency from a kept advertisement that
  proves availability, for a destination this operation did not push to (D9).
  For a destination it pushed to, `dependency_was_published` accepts a lock
  commit that is the pushed source or one of its ancestors (D8). Anything else
  is read.
- Any forced or deleting transfer in this operation invalidates every kept
  advertisement and every D8 proof; when the operation proves the root, every
  root-lock dependency is then read after all member transfers and before the
  root transfer.
- Update the module comment at `publication.rs:1-3`.

Tests on the step-1.2 seam:

- the check-once counts of §2.3's second table, with N from the fixture; 1.2(a)
  and (b) move to these counts;
- a destination pushed in this operation is proven by D8 or by a read, never by
  its pre-push advertisement;
- an advertisement without the destination ref still pushes;
- a member whose head is ahead of its lock commit is proven by its push, with
  no read;
- invalidation, each case ending with the root refused and no root
  `push_prepared` recorded:
  - two members that reach one repository through the same URL, with a forced
    refspec and `always`;
  - a push remote that reaches the dependency's repository through an SSH host
    alias, with a forced refspec, under `always` and under the default;
  - two members whose URLs differ only by a `.git` suffix, with one forced
    push;
  - a non-forced push through one spelling and a forced push through another,
    run in both transfer orders, where the non-forced push's D8 proof must not
    count;
  - a forced push that rewinds a member's remote;
- the non-forced variant of the host-alias case reuses the kept advertisement
  with no extra read;
- under `always`, one unmaterialized dependency is read exactly once.

Depends on 1.2 and 2.1, which edit the same functions. Integration owner.
Budget about 450 LOC. Commit: gwz-core.

As landed (gwz-core `f715672`; code +213/−83, tests +503/−97):

- **Brought forward from 3.5.** A root already on origin is still proven, as at
  `5bf8f1a`. Otherwise a published root with a rewound dependency would report
  `noop` instead of being refused
  (`a_root_already_on_origin_is_refused_when_a_dependency_is_missing`). This
  costs no reads when every dependency is present.
- **`noop` rows start here.** A repository already on origin reports `noop`
  with "already on origin", so the §3.6 machine-output change begins with 3.3,
  not 3.5. Neither CLI's summary line counts that reason.
- **Forcing rules.**
  - The root's own forced or deleting refspec also voids kept evidence.
  - A forced refspec whose destination is already at its source is `Noop` and
    voids nothing.
  - Deletions and empty refspec lists are never `Noop`.
- **Invalidation case (iv)** cannot happen through the product, because
  `PushRequest.refspec` applies to every target. The test mixes forced and
  ordinary transfers through `TrackingBackend::force_pushes` instead. The
  host-alias cases use a `pushurl` on `origin`.
- **Tag path.** `checked_root_request` now takes the operation's
  `ReadPreflight`. Tag passes an empty one, so a tag's proof still reads every
  dependency. Two unmaterialized dependencies with the same remote and read URL
  are now read once in the tag preflight.
- **Before 3.5.** The tests' "default" variants run the same path as `always`.
  Case (v) guards only the D8/D9 boundary until 3.5 adds last-known refs.
- **Integration.** The merge after 2.2 conflicted only in the test backend,
  where both steps had added members. The conflict was resolved by keeping
  both.

**Step 3.4: concurrent reads.** Run the pre-transfer reads
(`push_member.rs:222-257`) and any remaining proof reads through
`par_map_per_host`, under the same `--jobs` and `--max-per-host` policy as
pushes.

- Plan the set of destinations first, so a destination shared by a member and a
  dependency is still read once.
- Keep the report order, and keep the rule that every read failure is reported
  and no transfer starts.

Tests: the report order is unchanged; failure aggregation is unchanged; the
per-host limit holds (the step-1.2 overlap counter).

Depends on 3.3. Integration owner. Budget about 250 LOC. Commit: gwz-core.

As landed (gwz-core `0495405`; code +298/−87, tests +328/−18):

- **Push only.** `GitBackend` has no `Sync` supertrait, and the tag path accepts
  any `GitBackend`. So the concurrent reads live in new push-only functions
  bounded by `Sync`: `ReadPreflight::read_before_transfers` and
  `checked_root_request_concurrently`.
  - Tag still reads one at a time and stops at its first failure.
  - `preflight_dependencies_with_reads` was folded into tag's
    `preflight_dependencies`.
  - No public signature changed.
- **Planned rounds.** A push plans its reads, deduplicated by the
  `ReadPreflight` key, and runs them all. `jobs` and `per_host` are resolved
  once and shared with the transfers.
  - Failures are reported as before: each target's first failure in the
    target's own order, and the proof's first failure in lock order.
  - A round with a failure still makes every planned read; the sequential
    code stopped at the first failure.
  - A failed destination shared by a member and a dependency is read once and
    reported on both rows.
- **Push proof dedup.** Two unmaterialized dependencies with the same remote and
  read URL are read once in push's proof, as step 3.3 already does in tag's
  preflight.
- **Output.**
  - Reads emit no events, so event order is unchanged.
  - Git2's `meta.transport` rows for overlapping reads now appear in arrival
    order, as they already did for pushes.
  - Concurrent https reads can start several credential helpers at once.
- **Tests.**
  - Four new tests in `src/workspace_ops/tests/g26/concurrent_reads.rs` cover
    report order, failure aggregation, the per-host and `--jobs` limits, and
    concurrent proof reads after member transfers. Each passed 30 repeated
    runs.
  - The test backend gained a read-failure injector and a separate counter for
    reads after a push.
  - Two existing g26 tests asserted call order across two hosts, which
    concurrent reads make vary. They now compare calls per host
    (`calls_by_host`), still pinning every URL, count and within-host order.
- **Integration.** Merged after gwz-core `45c72c94`, which adds only
  `dev-docs/GwzRemoteTransportRequirements.md`. The merge changed neither side.

**Step 3.5: contact only what changed.**

- **Backend.** A contract query for a destination's last-known ref. It answers
  `None` by default, so fakes and every unknown case contact the remote. The
  Git2 implementation applies every §3.5 condition. The tracking backend gains
  an override.
- **Push.**
  - With `remote_check` absent or `changed`, classify each selected repository
    (step 3.1) before any read, and apply §3.5 rule 2.
  - A contacted root proves every root-lock dependency in the operation (D10),
    whether rule 1 publishes it or finds it already on origin.
  - With `always`, skip the classification, and prove a `Noop` root after member
    transfers (rule 3).
- **Output.** The `Noop` reasons of §3.6 in `planned.message`, in JSON and in
  `--dry-run`, and the missing-dependency refusal of §3.6.
- **Row status.** A row that carries a §3.6 `Noop` reason has status `Noop` and
  `planned.action: Noop`, in a dry run as well. Pull's dry run already does
  this for its no-op rows (`PullHeadPlan::planned_response`,
  `pull_head_plan.rs:82-97`). The two CLIs pick summary rows differently: the
  Rust CLI by status `Noop` (`gwz-cli/src/pushargs.rs`), gwz-py by
  `planned.action` (`gwz-py/src/gwz/cli_render_parts/push.py`). Any other status
  for these rows makes the CLIs disagree.

Tests on the step-1.2 seam:

- nothing changed: no transport calls; rows and aggregate `noop`, each row with
  its reason;
- one member and the root changed: those two are read and pushed, and every
  other dependency is read once (N+3 in total); the unchanged rows keep the
  reason "up to date with origin/main as of the last fetch or push", and the
  summary line counts them as not checked for changes;
- `always` gives the check-once counts;
- `--dry-run` reports the classification with no transport calls, unchanged
  rows with status `Noop`;
- `policy.destructive = Allow` with `refspec: None`, on a branch behind its
  last-known ref: `Noop` ("behind …"), no transport calls;
- the same state with a `+` refspec: contacted;
- a `+` refspec with an equal last-known ref and an advertisement at a
  descendant: contacted and pushed;
- a published root whose dependency was rewound, under `always`: root
  `Rejected`, nothing pushed; the root-only variant gives the same result; the
  sound variant gives root `Noop` with no extra reads;
- no last-known ref, a push URL that is a different repository, and an ancestry
  error all contact the remote;
- a contacted root that rule 1 finds already on origin is proven, and refused
  when a dependency is missing;
- a last-known ref left behind contacts the remote, then finds nothing to push.

Depends on 3.1, 3.2 and 3.4. Integration owner. Budget about 450 LOC. Commit:
gwz-core.

**Step 3.6: native tests for last-known refs and the root proof.** On the
native `Git2Backend` with local bare remotes, as g08 does:

- **A deleted remote branch.** A member branch pushed through gwz (which writes
  the tracking ref) is deleted in the bare remote from another clone, and a
  non-pruning fetch follows. The root lock names the member commit, and a
  default push runs. Expected: the member is `Noop`, the dependency read refuses
  the root with the §3.6 message, and the bare root ref is unchanged.
- **A fork push URL, later removed.** Two bare remotes stand in for upstream and
  fork. Push through the fork push URL, unset it, record the commit in the root
  lock, and push. Expected: the dependency read of upstream refuses the root.
- **Excluded layouts.** A second remote whose fetch refspec writes into
  `refs/remotes/origin/*`, and a `+refs/heads/*:refs/heads/*` fetch refspec,
  each give no last-known ref, so the member is contacted.
- **A different-repository push URL.** A remote whose `pushurl` names another
  repository, with an equal tracking ref, is contacted.
- **A renamed remote.** The original `origin` is removed first, because
  `git remote rename` refuses an existing name. A fork remote with fetched
  tracking refs is then renamed with `git remote rename fork origin`; the
  member branch equals the moved ref; the root lock names a commit only the
  fork holds. Expected: the member is `Noop`, the dependency read of the
  committed URL refuses the root, and the bare root ref is unchanged.
- **A non-forced fetch refspec.** A remote whose fetch refspec is
  `refs/heads/*:refs/remotes/origin/*` (no `+`) gives no last-known ref, so the
  member is contacted.

Depends on 3.5. Integration owner. Budget about 250 LOC. Commit: gwz-core.

**Step 3.7: Rust CLI option.** `gwz push --check-remotes` (D7), mapped to
`remote_check: always`.

- Help text, including the push help source behind `docs/commands/push.md`
  "Publication and authentication":
  - dependencies are proven by this operation's reads or accepted pushes;
  - by default, unchanged repositories are not checked for changes or pushed,
    though a push that contacts the root still reads each dependency;
  - what `--check-remotes` checks.
- Regenerate the CLI reference with
  `python scripts/generate_cli_reference.py --write`, and confirm with
  `--check`.
- Human output: the summary line of §3.6, and each `Noop` reason under
  `--verbose`.
- Tests in `gwz-cli/tests`:
  - parsing, including that `push --force` still yields `refspec: None`;
  - JSON goldens for an unchanged workspace under `changed` and under `always`,
    and for one member and the root changed under `changed` (rows, aggregate
    and reasons);
  - the summary line.

Depends on 3.2, and on 3.5 for the end-to-end assertions. Leaf owner. Budget
about 250 LOC. Commit: gwz-cli.

As landed (gwz-cli `31b14a3`, about 420 lines, 274 of them tests):

- Parsing, help and summary-line tests are in
  `src/tests/g02/push_check_remotes.rs`, not `tests/`, because the parser and
  renderers are crate-private.
- `docs/commands/push.md` has no generator, so it was edited directly, along
  with `src/push_long.rs`.
- The JSON goldens wait for 3.5. They belong in `tests/local_workflows.rs`,
  against local bare remotes.

**Step 3.8: gwz-py option.** Add the same option to gwz-py's push parser
(`src/gwz/cli_shared.py` or `cli_mutation.py`) and the request field. Render
both kinds of `Noop` reason and the §3.6 summary line. Tests run through
`run_tests.py`.

Depends on 3.2, and on 3.5 for the end-to-end assertions. Parallel with 3.7.
Leaf owner. Budget about 180 LOC. Commit: gwz-py.

As landed (gwz-py `b97ff00`, about 285 lines, 205 of them tests):

- The two CLIs share the text, not the layout. gwz-py prints the summary line
  after the status line and lists `<path>: <reason>` under `--verbose`. The
  Rust CLI prints the summary last and adds the reason to each row under
  `--verbose`.
- No test compares the two CLIs: `test_cli_parity.py` runs gwz-py's parser
  only. After 3.5, the owed end-to-end tests should also check that both CLIs
  print the same summary text and the same JSON rows for one workspace.

### Phase 4: finish

**Step 4.1: documentation.** In gwz-cli's docs where publication is described
(Concepts, Troubleshooting, QuickStart "Clone An Existing Workspace"), cover:

- an https clone publishes over https end to end;
- the rule for the URL the root proof reads, in one sentence;
- how to switch an existing SSH workspace:
  - either `git -C <path> remote set-url origin https://github.com/<owner>/<repo>.git`
    for the root and each member, or a fresh clone with `--url-scheme https`;
  - a workspace switched by hand has no `.gwz/url-scheme.yml`, so unmaterialized
    members keep reading their committed URLs until one is recorded;
  - `gwz repo sync` keeps the manifest URL (parent plan §2.8);
- the identity refusal of §3.3, and `--identity` as its remedy;
- a push contacts only repositories that changed since the last fetch or push,
  what each `Noop` reason means, and that a push that contacts the root still
  reads every dependency;
- `--check-remotes`, and when to use it: when someone else may have rewound a
  remote, before a release, or to check an unchanged root;
- in Troubleshooting, that gwz does not prune: a branch deleted on the remote
  keeps its member up to date until `git fetch --prune`, `fetch.prune`, a push
  that recreates the branch outside the default classification (for example
  `git push`), or `--check-remotes`.

Also, unconditionally:

- Update `dev-docs/GWZDesign.md:96-98` in gwz-core: dependencies are proven by
  this operation's reads or accepted pushes; by default, unchanged repositories
  are not checked for changes or pushed, though a push that contacts the root
  still reads each dependency; `--check-remotes` proves an unchanged root.
- In gwz-cli `docs/MachineOutput.md`, document push `noop` rows and aggregate,
  their reasons, and the compatibility change of §3.6.
- Grep gwz-core and gwz-cli docs for "available at their destinations" and
  similar statements, and qualify any that remain.

Depends on 2.1, 3.5 and 3.7. Leaf owner. Budget about 220 LOC. Commits: gwz-cli,
then gwz-core.

**Step 4.2: acceptance against GitHub.** The owner runs it with a binary built
from the committed tree and installed to a scratch `--root`. It never replaces
`~/.cargo/bin/gwz` and is never a `target/` build against gwz-dev.

Capture JSON, poll sockets and record wall-clock time for every case. Before
each socket-polled push, run a default `gwz --json push --dry-run` and record
its output; a `--check-remotes` dry run skips classification and cannot confirm
anything. Continue only if every row is `Noop` "up to date with
origin/<branch>" (not "behind"). For case 6, only the repositories the owner
named may be planned. Cases 5 and 6 do not start from a fresh clone, so fetch
before their dry run.

1. A fresh `--url-scheme https` clone, whole push: no sockets.
2. The same clone with `--check-remotes`: N+1 port-443 sessions (one read per
   repository), no port-22 sessions, and no pushes.
3. The same clone, root-only push with `--check-remotes`: port-443 sessions
   only.
4. A fresh SSH clone of the published workspace
   (`gwz clone git@github.com:owebeeone/gwz-dev.git <dir>`), whole push with
   `--check-remotes`: N+1 port-22 sessions and no pushes. Record the clone's
   path and root commit.
5. A scratch member that fetches over https and pushes over SSH, with
   `--check-remotes`: that member is read over SSH only.
6. One member and the root changed, default, as a real publication the owner
   names: N+3 sessions (the member's read and push, the root's read and push,
   and one read for each other dependency).
7. Optional: a tag push, only for a tag the owner wants published.

Write the results to `dev-docs/GwzUrlSchemePushAcceptance-<date>.md` (root
commit). Include:

- the "before" numbers from step 0.2;
- wall-clock times;
- the account each transport used;
- what remains open.

Depends on 0.2, 2.2, 3.6, 3.8 and 4.1.

### Dependency summary

```text
0.1 -> {1.1, 1.2, 3.1, 3.2}
{1.1, 1.2} -> 2.1 -> {2.2, 3.3}
3.3 -> 3.4 -> 3.5 -> 3.6
{3.1, 3.2} -> 3.5            (3.1 and 3.2 can start alongside Phase 1)
3.2 -> {3.7, 3.8}            (end-to-end assertions after 3.5)
{2.1, 3.5, 3.7} -> 4.1
{0.2, 2.2, 3.6, 3.8, 4.1} -> 4.2
```

Step 0.2 ran on 2026-09-14.

## 6. Test matrix (what "done" means)

| behaviour | test | step |
|---|---|---|
| same-repository rule and every read-URL rule, including each "different" case | unit | 1.1 |
| today's read and push sequence over SSH, whole and root-only push | core, tracking backend | 1.2 |
| https member remotes with SSH committed URLs: no SSH reads, dedup and shortcut hit | core | 2.1 |
| fork, split fetch/push and unknown-host remotes resolve as §3.2 specifies | core | 2.1 |
| unmaterialized dependency follows `.gwz/url-scheme.yml`; malformed file refuses with push remedies | core | 2.1 |
| proof refusal names the read URL | core | 2.1 |
| SSH identity override with an https read URL refuses before any transfer, whole and root-only, naming `--identity` | native | 2.1 |
| SSH workspace reads unchanged by Phase 2 | core (1.2 tests unchanged) | 2.1 |
| root tag push reads through the read URL | core | 2.2 |
| classification, every cell of §3.5 including an ancestry error | unit | 3.1 |
| `remote_check` round-trips; the catalog lists it; py drift pin; gwz-cli builds | generated corpus, catalog, `check_protocol_drift.py`, `cargo build` | 3.2 |
| check once: counts; pushed destinations never proven from pre-push advertisements; ahead member proven by its push; operation-wide forced-transfer invalidation across URL spellings; unmaterialized dependency read once | core, tracking backend | 3.3 |
| concurrent reads: report order, failure aggregation, per-host limit | core | 3.4 |
| default: no transport calls when nothing changed; `noop` rows, reasons and summary line; a contacted root reads every dependency and is proven; `--dry-run`; `--force` and `+` refspec cases; `always` proves a `Noop` root | core, tracking backend | 3.5 |
| deleted remote branch, removed fork push URL and renamed remote refuse the root; excluded refspec layouts (including a non-forced fetch refspec) and a different-repository push URL are contacted | native | 3.6 |
| option parsing; `--force` keeps `refspec: None`; JSON goldens under `changed` and `always`; summary line | gwz-cli tests | 3.7 |
| option, both `noop` reasons, summary line | gwz-py tests | 3.8 |
| GWZDesign, push help, MachineOutput and Troubleshooting state the proof sources, `noop` compatibility and pruning | review of the step-4.1 diff | 4.1 |
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
  - https reads of a private, unmaterialized member, under rule 3 or under D10
    when the root is contacted, can start an interactive credential helper
    during a push. This fails closed.
- **Stale last-known refs.** The default trusts them only to decide whether a
  member is checked for changes and pushed (§3.7); every contacted root is
  proven in the operation (D10).
- **Mixed versions.** Both directions are safe:
  - A core that predates `remote_check` reads only the fields it knows, so it
    ignores the new field and contacts everything, as today.
  - A new core refuses an old client's request at decode. The generated
    decoder requires every tag, so a request without tag 4 fails with
    `MissingKey(4)`; the same rule already applies to `url_scheme`. Current
    Rust and Python encoders always send the tag, with null for `None`.

  Corrected on 2026-09-15: step 3.2's implementation found the earlier claim,
  that such a request gets `changed`, to be wrong.
- **The test seam.** Step 1.2 specifies everything later steps test on it,
  including one repository reached through several URLs, so the changes in 2.1,
  3.3 and 3.5 are measured against a pinned sequence. The
  Git2 last-known-ref mapping is tested natively in step 3.6.
- **The base is unpushed.** If `c9c7a98` is reworked, 2.1's shortcut comparison
  follows the rework.
- **Pre-existing, not this plan.** `GWZDesign.md:1601` lists a push `--refspec`
  flag that gwz-cli does not have.
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

## 9. Review record

Dual peer-blind review on the Consistency and Safety axes, every report filed
verbatim in `dev-docs/`:

| round | revision | Consistency | Safety | remediation |
|---|---|---|---|---|
| 1 | `b64f691` | NO-GO: 5 P2, 5 P3 | NO-GO: 1 P1, 4 P2, 4 P3 | `GwzUrlSchemePushPlan-RemPlan.md`; owner decisions D10 and D11 |
| 2 | `f3a395b` | GO: 3 P3 | NO-GO: 1 P2, 1 P3 | `GwzUrlSchemePushPlan-RemPlan-2.md` |
| 3 | `0136094` | GO: 1 P3 | GO: 1 P3 | folded in after acceptance, without re-review |

- **Blind convergence (round 1).** Both axes found that `--force` is not a
  forced push (Consistency P2-1, Safety P3-1). Both also found the design's
  proof promise and the `noop` compatibility change missing from the plan
  (Consistency P2-5; Safety P3-4 and P2-4).
- **No architectural root cause** was found in any round. Every defect was found
  at plan review; none escaped into code, because no code exists yet.
- **Folded in after acceptance:**
  - Safety round-3 P3-6: a last-known ref needs a forced fetch refspec.
  - Consistency round-3 P3-9: the seam refuses non-forced pushes that are not
    fast-forwards, and the mixed-spelling invalidation test runs in both
    transfer orders.
  - The residual wording of both round-3 reports.
