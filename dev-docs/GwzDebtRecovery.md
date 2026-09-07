# GWZ debt recovery plan

Linux follow-up (2026-09-07): the Raspberry Pi's native ARM64 build and ext4
untouched disposal / root+member commit / strict family merge / ordinary disposal /
detach cycle passed. It used the same clean source digests as Magenta. See
[Pi acceptance](GwzPiAcceptance.md). These focused platform runs do not replace
the remaining release gates or same-platform merge-base census review.

Magenta follow-up (2026-09-07): native Windows explicit-file authentication,
publication to isolated branches, fresh checkout with all member locks verified,
and local merge/disposal passed. This closes the Windows publication evidence
gap; the Dabeest credential limitation remains machine-specific. Fresh checkout
still exposes derived-marker status churn. See [Magenta acceptance](GwzMagentaAcceptance.md).

Latest acceptance update (2026-09-07): the source was committed and pushed, then
built natively on Dabeest/Windows. An initial signed-credential-bitmask compile
failure was fixed and published (root `8f40897`, core `4d35577`). Untouched
clone/disposal and root+member work → one default family merge → ordinary
disposal passed without force. Separate detach/retain also passed. Windows
network push remains blocked by credential acquisition; detached marker status
and stale command prose need follow-up. See [the detailed acceptance record](GwzDabeestAcceptance.md).
Earlier no-publication statements below describe the prior checkpoint.

Date: 2026-09-06. Status: **local implementation verified; release acceptance and explicit deferrals remain**.

The operator approved working through this plan on 2026-09-06, including its
recommended product decisions. Implementation owner: the current Codex task.
Completion is recorded only with executable evidence; release and remote CI
results are not implied by local implementation.

Resumed after quota reset on 2026-09-07. See [the exact checkpoint](GwzDebtRecoveryCheckpoint.md) before resuming: file-identity wiring, configuration, typed transport reporting and startup timeout support are implemented; captured publication and local verification are finished. Remote acceptance and explicitly deferred work remain open.

### Implementation ledger (2026-09-07, local macOS evidence)

- DR-0: both reported lifecycle failures reproduced with real repositories;
  selected-member rejection also reproduced advancing the root remote.
  [The contract audit](GwzDebtRecoveryContracts.md) now lists all 30 actions,
  suboperation exceptions and the existing 0–71 error catalog; unresolved
  matrix cells remain explicit.
- DR-2: canonical marker admission and byte-preserving manifest publication
  implemented. Uncommitted marker work is preserved when verified, otherwise
  refused before allocation; suppressed status cannot authorize overwriting.
  All **13 integrity tests pass**, including late marker edits and new root
  history at the removal boundary. The preceding local-clone suite passed 63
  tests; the completed broad run and corrected focused rechecks are accounted for below.
- DR-3: centralized action policy, root-inclusive ordinary/family merge,
  root branch/tag/stash, materialize capability handling and direct `forall`
  resolution are implemented. Focused planner, merge lifecycle, branch, stash,
  listing and materialize suites pass. Rust CLI library **174 tests** and local
  workflow **9 tests** pass; Python local workflow **14 tests** pass. Python
  `forall` dry-run no longer executes commands. The additive protocol method
  passes measured compatibility and packaged-IR drift checks. The policy
  audit and documentation are recorded in the contract/coverage manifest; platform acceptance remains pending.
  Mixed root/member branch switches now refuse a changed member identity before
  branch creation (12 branch tests pass). Partial family merges explain the
  remaining root integration/preservation action (10 family-merge tests pass).
  Tag publication freezes all tag objects before transfers (5 tag tests pass).
- DR-4: failed/rejected selected-member pushes block root publication;
  committed-lock dependency proof also covers root-only pushes and root tags.
  Real remote-ref regressions pass. A native server-hook test reproduced a
  falsely successful named push; the callback fix now reports RemoteRejected.
  Read-authentication preflight and source/destination capture now precede member transfers. Fifteen publication scenarios have green evidence, including root retry/fresh clone and inactive dependency checks; platform acceptance remains open.
- DR-1/DR-6: Cargo's supported cleanup removed the separate workspace debug
  outputs, restoring about 27 GiB free. Cargo and Bazel now report matching package versions and CLI/core source
  digests, revision, dirty state and build system; both artifacts built and
  their identities were compared. Workspace/candidate workflows and generated
  inventory checks are drafted, with local negative tests; remote CI, required
  check configuration and platform acceptance remain pending.
- DR-5: the pinned native libraries were inspected; broad agent authentication
  cannot enforce an exact-key promise. See [the capability report](GwzDebtRecoveryAuthCapability.md).
  Ten controlled production-backend SSH cases passed with temporary keys and an isolated
  agent, covering both agent orders, file/remote/configured selection, wrong/missing/encrypted
  keys and host-key mismatch. Both drivers have identity configuration, structured
  reporting and startup timeouts. Exact encrypted-agent selection remains a separately
  scoped library extension; platform acceptance remains open.

Current continuation evidence: Rust CLI library **177 passed**, lifecycle workflows
**10 passed**, build identity **1 passed**. Python broad verification found 784
passes and 13 identical local-family JSON compatibility failures; the correction
passed all 195 affected tests, and the rebuilt native extension passed 208 affected
checks. Core protocol **37 passed**; current all-targets Clippy, compatibility,
document, source-boundary and pin gates pass. Captured push preserves ordinary
tracking updates; root rejection/retry/fresh clone and inactive fetch-only
lock-dependency checks pass. Root branch/tag/stash dry-runs preserve state.
The broad core run completed with 1,863 passes, two stale guard failures and one
intentionally ignored helper; both guards pass after correction. The boundary
checker negative/compiler suite passed **all 75 tests**. See the checkpoint for
precise broad-run and focused-rerun accounting. No known failure remains; no new
commit or publication was made.

CI now freezes the test executable selected from Cargo's artifact JSON, generates
the platform census and reconciles full execution. The evidence gate refuses an
unexecuted or incomplete census. Nine census controls and fifteen workspace
helper tests pass. The current local census has **1,867 identities**, 23 added and
none removed relative to the earlier local snapshot. That comparison is not a
merge-base or Linux acceptance result; manual pins remain during the transition.

Earlier checkpoint evidence (historical, before the continuation above):

The generated inventory runner reconciled 1,118 executed remainder tests and
one ignored helper against a frozen binary. Its eight negative/control tests
pass, including rejection of mismatched or unknown baseline profiles. This is
local evidence alongside existing count pins, not Linux baseline acceptance.

A fresh broad core run passed **1842 tests, 0 failed, 1 intentionally ignored**
in 952.96 seconds. Subsequent publication source-capture and selection-owner
refactoring passed focused suites. The real Rust CLI `add` / `commit` / default
merge / ordinary dispose acceptance sequence passes without workarounds. The
Python broad run passed 738 tests; its 42 fixture/environment failures all passed
on recheck with the exact Rust binary supplied. The complete compatibility
battery passes. Final validation after the remaining changes is still required.
The operator authorized a local checkpoint commit without rerunning tests.
This is unfinished work, not release acceptance; nothing has been published.
The checkpoint includes the existing remote-auth proposal edits without altering them.

The immediate objective is to make the ordinary lane lifecycle work: create a
lane, optionally make and commit changes, merge it back, and dispose of it
without a force waiver or an unexplained second merge. The structural objective
is to give selection, generated state, publication and verification explicit
owners, so the next command cannot accidentally reintroduce these failures.

The operator adopted the product decisions below. Before implementation,
incorporate the adopted decisions into
`gwz-core/dev-docs/GWZDesign.md`, `GWZRequirements.md`, the local-clone design and
the relevant protocol contracts. Preserve the existing ruling against Git-CLI
transport fallback. Additional local-clone modes remain outside this plan.
The previously deferred cohesion cleanup remains a separately scheduled item.

## 1. Inputs and corrected baseline

Inputs are [structural findings](GwzStructuralFindings.md),
[target-selection design](GwzTargetSelectionDesign.md),
[remote-auth proposal](GwzRemoteAuthProposal.md), and the operator's two lab
reports: an untouched lane refuses disposal because of a regenerated integrity
marker, and a member-only merge does not preserve the lane's root commits.

Source inspection for this plan used gwz-dev `e5300b1`, gwz-core `bea84c1` and
gwz-cli `73d80dc`. The remote-auth document had an existing working-tree edit;
its on-disk contents were read. This is a source review, not a fresh execution of
the lab scenarios. DR-0 must reproduce them against the implementation tuple.

Several details in the inputs need correcting before they become requirements:

| Topic | Current source evidence | Consequence for this plan |
|---|---|---|
| Clone integrity | `src/local_clone/adapters/install.rs::publish_manifest` rewrites the manifest through the typed writer, regenerating the marker over the copied lock. Disposal observes ordinary Git work and filters structural paths, without a marker-specific classification. | Fix the generated-state contract across installation and disposal; do not simply hide the marker by filename. |
| Merge and `@all` | Both `src/local_clone/family_merge.rs::selected_keys` and `src/workspace_ops/merge/plan.rs::build_merge_plan` require a literal `@root` after resolution. | Changing the resolver default alone is insufficient. Remove both secondary selection rules. The existing `@all @root` example reflects a real current workaround, despite being redundant under the intended model. |
| `ls` and `forall` | `handle_ls.rs` already returns `id: @root`, `path: .`, and `TargetKind::Root`. Rust `forall.rs` consumes these entries and sets `GWZ_TARGET_KIND`. | Preserve and test that representation. There is still an architectural dependency on `ls`; do not plan a nonexistent missing-root schema fix. Verify Python separately. |
| Push ordering | `push_member.rs` waits for member results, then calls `push_root` even when a member response is Failed or Rejected. Its earlier preflight checks local policy and shape. | Root-last ordering alone does not ensure safe publication. Add a success/dependency barrier and failure tests. Network preflight is an extension of existing preflight. |
| Gate coverage | The aggregate runner has six named batteries, including byte-equivalence and unknown-field. Core release CI selects fault, byte-equivalence, unknown-field and privacy; boundary CI selects privacy and call-graph. Compatibility and Bazel pin-drift still lack the workspace checkout they need. | Inventory actual jobs and commands. Fill the missing coverage without claiming all existing CI is absent or paying for unnecessary duplicate runs. |

Paths in the evidence column are relative to `gwz-core/`, except the explicitly
named Rust driver file in `gwz-cli/src/`.

## 2. Product decisions recommended for adoption

| Decision | Proposed contract | Compatibility treatment |
|---|---|---|
| Merge scope | Ordinary and family merge default to root plus active members. Explicit member selection remains member-only. | A deliberate behavior change: release-note it, show the resolved participants before effects, and document `--target @all --no-target @root` for the previous default. Do not silently give family merge a different default. |
| Root operations | Branch, tag and stash support the root when selected. Their default remains members. Cover all suboperations, including tag list/delete/push. | Additive capability; default tag/branch/stash scope stays stable. Root stash must participate in the existing coordinated stash lifecycle. |
| `@all` | Includes every supported target. For root-capable verbs that is root plus members. For materialize it is members, since the root is already present. | Preserve selector provenance until policy is applied: an explicit surviving `@root` on materialize receives a specific refusal. Explain this one capability-dependent rule in help. |
| Whole-workspace verbs | Local clone/list/dispose/disband reject nonempty selection with a reason before effects. | Previously ignored flags become explicit errors in both drivers and direct core requests. Empty/default wire envelopes remain valid. |
| Root reporting | Reuse the existing root entry and `TargetKind`; `forall` obtains targets directly from the common core selection service. | Preserve existing response fields and default output. Avoid another competing target schema. |
| Generated integrity marker | The marker is derived data, but a disposal exemption requires fresh proof that it contains only the canonical derived value and masks no configuration or staged work. | Keep the tracked format and hand-edit detection. No blanket `gwz.conf/` exclusion, automatic commit, or weaker history check. |
| Publication | A selected root is published only after the member dependencies of its committed lock are satisfied. | Member failures leave the root remote unchanged. Root-only and partial selections need an explicit dependency check, not an assumption that omitted members are published. |
| SSH identity | Explicit identity means only that identity may authenticate. Unspecified identity retains current behavior. | Additive request/configuration support; no Git shell-out and no fallback to unrelated agent keys after an explicit identity fails. |

These decisions close the open merge-default and root-tag choices rather than
leaving implementation owners to choose separately. Their adoption was a design checkpoint; the operator subsequently authorized
implementation. Release remains subject to the evidence and publication gates.

## 3. Delivery sequence and ownership

Assign one accountable owner to each package before implementation. Shared
selection policy, error allocation and taut schema changes have a single core
contract owner; Rust and Python drivers consume those contracts. Existing
[library boundaries](GwzLocalCloneLibraryBoundaries.md) remain in force.

| Package | Scope and owner | Dependency and exit |
|---|---|---|
| DR-0 — baseline and contracts | Core contract owner: reproduce failures, freeze decisions, audit error taxonomy and compatibility. | First. Exact source tuple, expected behaviors and bounded interfaces recorded. |
| DR-1 — trustworthy verification | Workspace/build owner: workspace CI, required checks and build identity. | Starts after DR-0; required before calling later packages release-ready. Missing siblings and stale binaries can no longer masquerade as evidence. |
| DR-2 — disposable untouched lanes | Installation/disposal owners: narrow generated-marker treatment and real lifecycle fixtures. | DR-0 contracts; can land independently of selection. Untouched valid clones dispose without a waiver. |
| DR-3 — unified targets and complete merge | Core selection/merge owner, then drivers: common policy/resolver and root lifecycle support. | DR-0; normal lane lifecycle acceptance also depends on DR-2. No second root merge required. |
| DR-4 — safe root publication | Core push owner: member-result barrier and lock dependency checks. | DR-0; use the common selection contract as DR-3 lands. Partial failure never publishes a new dangling lock. |
| DR-5 — explicit credentials | Transport owner, then configuration/protocol/drivers: capability spike, identity resolution and honest reporting. | Contract audit in DR-0; share DR-4's operation plan and preflight. Do not hold the lane fixes for this larger feature. |
| DR-6 — maintenance costs | Build/test owners: replace manual count churn, retire dead artifacts, and schedule deferred cohesion work. | Coverage work depends on DR-1. Cleanup is bounded separately from behavior changes. |

The first recovery release requires DR-1 through DR-4 and the relevant DR-0
contracts and error review. DR-5 can ship separately after its capability tests.
DR-6 must have owners and explicit remaining items; deferred file splitting is
not a reason to delay a tested lane fix. Do not expand this program into clean
or bare clones, arbitrary source refs for clone, or family pull/push support.

## 4. DR-0 — establish behavior and compatibility before changing it

Create small disposable workspace fixtures through supported GWZ operations.
Pin the executable path and version information. Capture both reported failures
before implementing their fixes, including the actual root index/worktree diff,
protected roots and witness coverage. Test a clean source with a stale committed
marker as well as a source whose marker already agrees with its configuration.
Do not silently normalize a fixture before measuring the bug.

Inventory every public action and subaction in both drivers and direct core
dispatch. Record default targets, root behavior, exclusions, empty selection,
dry-run, and whether refusal occurs before mutation. Include pull, remote tag
operations, branch checkout and coordinated stash restore; a command-name table
alone does not cover their different effects.

Review the local-clone error codes against the complete catalog before allocating
selection, publication or auth codes. Group failures by recovery: invalid request
or selection, authentication, authorization, lifecycle state, dirty/unpreserved
work, unknown evidence, and partial effects. Keep existing numeric/string wire
codes stable unless an explicit compatibility decision establishes they were
never released. Add structured classification only through taut and regenerate
both bindings. New codes require a distinct recovery action, not a new call site.

Exit evidence: failing reproductions, a command-policy matrix, proposed schema
diffs, and an error/recovery table. Counts and platform results are measured on
the actual platform, never copied from an earlier report.

## 5. DR-1 — CI that tests the workspace and identifies the binary

Add a workspace workflow under `gwz-dev/.github/workflows/`. Materialize the
exact member revisions recorded by the workspace lock, with the required sibling
layout. For member PR validation, accept an explicit candidate revision override
and record the entire resulting tuple. Never combine the candidate with floating
sibling `main` branches. A release check must match the tuple being released.

Run the complete compatibility battery, including both document-consistency
checkers and their negative fixtures, and `check_bazel_pin_drift.py` with its
negative fixtures. Retain the existing single-repository gates. Add the new
selection and lifecycle checks to an explicit coverage manifest mapping invariant
to command, repository topology, platform and required CI job.

Make the workspace check required for workspace landing and release promotion.
Ensure member changes trigger validation of a candidate workspace tuple before
promotion; a workflow that runs only after an occasional root commit leaves the
same gap. Missing sibling revisions, skipped required commands and zero executed
tests must fail the required job. Keep unsupported platform evidence explicitly
pending rather than reporting a green aggregate.

Use macOS and Linux for the applicable lifecycle and existing fault evidence;
record the supported Windows checks separately. Upload the resolved tuple,
toolchains, commands and results. A local run remains useful evidence, but its
report says it ran locally and names any coverage omitted. Validate the workflow
with a deliberate doc mismatch and a deliberately drifted Bazel pin before
declaring F1/F7 closed.

Build identity (F2) is part of this package. Cargo and Bazel must report the same
package version and a useful development identity: CLI revision, core revision
or source fingerprint, dirty state and build system. A CLI hash alone cannot
identify a changed path dependency. Preserve release SemVer while exposing
provenance in long version output or an additive machine-readable equivalent.
Source archives must report unavailable provenance honestly. Add a smoke check
for each artifact path and test that stale and current builds are distinguishable.

After locating consumers, retire the obsolete `gwz-cli/target` build output and
update developer commands to the current output path. Limit cleanup to identified
generated artifacts; do not delete repositories or unrelated work. Prevent Bazel
compile inputs from admitting `target/`, cache trees or recursive output symlinks.

## 6. DR-2 — generated state must not trap an untouched lane

Keep authoritative configuration, derived integrity data and family bookkeeping
distinct. `gwz.yml` and `gwz.lock.yml` changes are workspace work. The marker may
be regenerated, but arbitrary bytes at its path are not automatically disposable.
Family metadata continues through its existing owner and is not a substitute for
fresh work/history observations.

Implement in this order:

1. Make verbatim installation preserve valid copied configuration bytes where
   possible. Avoid gratuitous serialization changes. Use sanctioned artifact
   helpers and keep final manifest publication before the ready transition.
   Inspect source/index state before any rewrite: a stale committed marker is
   different from copied uncommitted marker work. Preserve the latter or refuse
   the installation; never erase it and then classify the replacement as clean.
2. Extract a read-only canonical marker renderer/validator from the integrity
   owner. The writer and disposal admission check must share that definition.
   Keep the current tracked marker location and schema in this recovery release.
3. Admit an unavoidable marker-only refresh as derived maintenance only when the
   manifest and lock are present, readable and unchanged against index and HEAD;
   the marker is a regular file with exactly the supported canonical bytes for
   those files; and there is no staged marker change, conflict, unsupported
   payload or unobserved state. Missing/legacy baseline markers need explicit
   fixtures and rules; they are not an implicit broad exemption.
4. Translate that fresh admission into narrow evidence at the core adapter. The
   pure work classifier must not parse YAML or read the filesystem. Never add
   the marker to an unconditional path filter. Disposal must not rewrite or
   stage evidence to make its own precondition pass.
5. Revalidate the admitted observations at the existing disposal boundary under
   the family lifecycle rules. Any change or inability to establish the facts
   restores dirty/unknown refusal. Do not claim stronger concurrency guarantees
   than the existing disposal design provides.

If a verbatim install would change manifest or lock bytes, fix the unnecessary
rewrite or report real generated configuration work; do not extend the exemption
to those files. The same classification should explain marker maintenance in
status/disposal diagnostics without falsely saying raw Git status is empty.

Support already-created lanes when the same fresh proof succeeds; do not require
recreation, a hidden root commit, or `--force dirty`. A root commit made merely to
hide the symptom creates another history-preservation obligation. Retain the
existing conservative history test for root, members, other refs and reflogs.

Acceptance includes immediate clone/dispose, stale and current markers, source
work copied into a lane, staged marker changes, manifest/lock edits, malformed or
future marker schemas, symlinks, unreadable files and observation drift. Positive
tests must use the real install and disposal adapters together, not only a fake
clean observation.

## 7. DR-3 — one target decision from request to effect

Declare policy exhaustively by action and necessary subaction in core. A new
action must fail compilation or an exhaustive registry test until its policy is
declared; no catch-all default. Use the selection design's vocabulary, with
explicit capability/refusal reasons, and distinguish whole-workspace actions.

Normalize legacy `all`, `member_ids` and `paths` into canonical includes/excludes
at the protocol boundary. Preserve their current combination semantics in
characterization tests. Retain provenance for explicit root requests until after
exclusions and capability policy have been applied. The core policy service is
the sole interpreter for CLI, Python and direct API callers.

Return a typed resolved target set and have orchestration consume it. Remove
legacy members-only resolution paths, literal-`@root` scans and filtering of
already-resolved targets. Migrate every caller, including the family import
planner, ordinary merge planner, tag/branch/stash/materialize and `forall`.
Resolve `forall` through a core selection API that preserves existing path and
materialization behavior; do not add a second driver resolver. The public merge
boundary may revalidate, but must not choose a different participant set from
the family importer.

Add a boundary gate against selector expansion and reads of raw selection fields
outside the normalizer/policy service. Driver request construction is allowed;
driver interpretation is not. Keep explicit migration exceptions temporary and
remove them before closing this package. Exercise the gate with a negative
fixture that reintroduces a handler-level literal-`@root` check.

Pin include/exclude ordering, deduplication, deterministic presentation order,
unknown tokens, inactive/ambiguous members and empty selections. Preserve
intentional empty-selection no-ops with an explicit zero-target result. Refusals
name the command, why the request is unsupported and a valid alternative, and
occur before imports, lock-file creation or other mutation. Keep execution order
separate from presentation order: a resolver returning root first must not make
commit or push execute root first.

For merge, adopting All requires the existing root-aware engine, not a wrapper
that imports root history and then performs only member merges. Capture the
complete source participant vector before effects. Apply the existing root
merge, lock reconciliation, conflict, continue and abort protocols. Verify that
the resulting lock names the actual accepted member results, and that source
root history survives in the receiver through the intended merge/fast-forward
semantics. Retained import refs remain protected as currently designed; they are
not a substitute for integrating the requested root changes.

The primary acceptance sequence uses a clean valid workspace, creates A, makes a
member change in A, uses `gwz add`/`gwz commit`, returns to the family root, runs
`gwz merge --remote A`, then `gwz local dispose A`. It succeeds without a second
root selector, manual Git operations or waivers. Also test root-only work,
divergence, conflicts and abort. Additional unmerged branches, stashes, ignored
data or new work must still prevent ordinary disposal.

An intentionally member-only merge remains valid and must not silently expand
scope. When it leaves root history unpreserved, report that fact and identify
the remaining root integration/preservation action. Do not relax disposal to
make a partial merge look complete.

Correct the tag help and comments, redundant future `@all @root` examples, both
drivers' command references and the local-clone guide in the behavior package.
Preserve historical reports as evidence and mark superseded claims explicitly.
A temporary documentation fix before this package lands must describe current,
not proposed, behavior.

## 8. DR-4 — root publication is a dependency barrier

Retain the existing complete local push preflight. Freeze the root commit to be
published, its committed lock, selected member refs and destinations. Perform
member transfers, collect actual outcomes, and only then consider the root.

The immediate correction is to withhold root publication when a required member
fails or is rejected, including push-time failures after preflight succeeds.
Report successful member effects and a root result such as blocked by member
failure, with retry guidance. Never roll back remote commits to simulate an
atomic multi-repository transaction.

Close the broader dependency gap in the same contract: a successful push of a
member HEAD does not alone prove that an arbitrary OID in the root lock is
available to a cloner. For each lock entry, establish that the referenced graph
is available at the intended source, through successful publication of a ref
containing it or a bounded remote reachability check. Cover excluded, inactive,
read-only and intentionally skipped members, different remotes/refspecs, and
root-only selection. Classify non-Git source kinds by their actual availability
contract. If the dependency cannot be established, withhold the root with a
specific reason; do not silently push an unselected member.

Test both ordinary remote push and any already-supported composed push path;
future family push must reuse the contract when implemented. Cover single and
parallel jobs, root failure after successful members, cancellation, and retry.
Root tag publication from DR-3 uses the same dependency rule for the tagged root
commit, rather than assuming the branch HEAD's lock is the relevant one.
A fresh clone from test remotes is the end-to-end success check. A member failure
must leave the root remote ref unchanged even when its own push would succeed.

Extend network preflight only after identity resolution is reliable. Check each
distinct effective destination/identity combination; one remote's success does
not establish access to every repository. Bound timeouts and aggregate related
failures. Reachability/read authentication is not proof of write permission, and
preflight cannot eliminate a later rejection or network failure. The publication
barrier remains authoritative. No guarantee covers later external deletion or
force-rewrite of a remote that was valid at publication.

## 9. DR-5 — explicit SSH identity with an honest capability boundary

Keep libgit2 transport. Begin with a bounded spike against the pinned git2 and
libgit2/libssh2 versions, including the actual multi-identity agent use case.
The current agent credential API takes a username, not a key selector; the
explicit file-key constructor is a separate API. Consequently, pinning an
encrypted key through an agent is additional capability work, not established
by adding one `Cred::ssh_key` call. See the
[libgit2 agent credential signature](https://libgit2.org/docs/reference/main/credential/git_credential_ssh_key_from_agent.html)
and [git2 credential constructors](https://docs.rs/git2/0.21.0/git2/struct.Cred.html).

Choose one typed credential-selection owner and carry it through every applicable
remote operation: clone/fetch/pull/push and remote tag operations. Review
`OperationAttribution.credential_ref` before adding a field. Prefer a dedicated
transport option if attribution is merely descriptive; never reinterpret a
released audit label as authority to select a private key. All public additions
are taut-defined and generated for Rust and Python.

Use the proposed precedence: per-remote invocation override, invocation default,
configured remote identity, then existing agent behavior. Freeze grammar for
repeated overrides, conflicts, unknown remote names and paths containing `=`.
Remote names such as `origin` recur across members: document that a name override
applies to each selected repository with that remote name. Resolve root remotes
through the same contract, including where their configuration is stored.

Persist machine-specific key paths in local configuration, with an optional
logical identity reference in shared remote configuration if needed. Specify
home/relative path resolution and a sanctioned GWZ command for setting it;
instructions to hand-edit `gwz.conf` are not an acceptable configuration UI.
No key material or passphrase belongs in manifests, locks, requests, logs or
operation records. Preserve the existing HTTPS helper policy and host-key checks.
Anonymous local family transfers must not acquire network credentials.

An explicit identity must fail closed when unavailable, rejected or unsupported.
For an encrypted key, support only a demonstrated exact-key agent mechanism;
otherwise return an explicit unsupported/unavailable result. Do not ask the
operator to weaken key protection or silently offer every agent key. If the
spike cannot provide exact agent selection through the permitted library path,
record that limitation and size the library work separately. File-key support
alone does not close the original multi-identity incident.

Report credential method, selection source and public-key fingerprint when
actually known, with per-repository results in human and JSON output. Distinguish
the selected credential from the identity proven to have authenticated. Agent
mode may legitimately report an unknown key; do not invent an account name.
Authentication identity and Git commit author/committer attribution are separate
facts, so a successful push cannot diagnose commit attribution.

Acceptance uses controlled SSH fixtures with two agent identities in both orders,
explicit file and encrypted-agent cases, wrong/missing keys, unsupported cases,
timeouts and repeated callbacks. The unselected key must never authenticate an
explicit-identity request. Verify precedence, root handling, driver parity,
secret redaction and that auth failures leave DR-4's root barrier intact.

## 10. DR-6 — reduce recurring maintenance without losing protection

**Test census (F4).** Replace hand-maintained pass-count equalities with a generated
inventory of test identities per package/platform and explicit partition rules.
Check that partitions are disjoint, cover the intended inventory and actually
execute; account for ignored tests separately. Compare the candidate inventory
with the merge-base inventory to report removals and renames requiring a reviewed
reason. A runtime count alone misses a deleted test; a floor lets an addition hide
a deletion. Preserve named critical fault scenarios and their executed evidence.
Generate inventory on macOS and Linux rather than predicting Linux from Darwin.
Run the replacement alongside existing pins until both platforms agree and
negative fixtures catch deletion, omission, duplicate coverage and zero-test
runs. Then retire the manual equalities and their repeated bookkeeping prose.

**Error taxonomy (F5).** Land the DR-0 catalog review before the next release tag
and before the new refusal codes are frozen. Test representative failures through
both drivers and JSON. A class should help a driver choose the right recovery
presentation without losing exact codes or partial-effect detail. Avoid renumbering
the catalog as cosmetic cleanup.

**Build debris and pins (F6/F7).** Audit razel references and whether any current
consumer still requires it. If retired, remove its generated artifacts and stale
entry points in a dedicated cleanup package, retaining defensive traversal
exclusions until clean-tree Cargo/Bazel checks pass. If retained, assign a supported
owner and smoke job. Keep hand-pinned Bazel dependencies under the existing drift
gate; test dependency set, versions and features. DR-1 supplies its missing CI
home. Do not reopen the settled Option A workspace split to fix pin maintenance.

**Cohesion (F3; previously deferred).** Add L1-23's convention to the boundaries
document and implementation brief template. Review files around 1,000 lines for
responsibility boundaries; this is an alarm, not a quota. Prioritize
`local-disposal/src/lib.rs` and large test suites. Prefer sibling `tests.rs` or
focused test modules for substantial suites, allowing small inline tests when
cohesive. Record the `rust-split` workflow required by the applicable mechanical
move policy; establish tool availability before scheduling such moves. Keep
movement-only commits separate from disposal behavior changes and preserve test
identities or record their renames. Schedule actual splitting after the existing
deferral is lifted; do not claim it completed with a documentation edit.

The standing prevention rule is small: a new invariant must name its owner and
automated check, or explicitly record why it is reviewed manually. Generate or
check command policy/help from the same declarations where feasible. Avoid a
new prose checklist that every implementation brief must copy by hand.

## 11. Acceptance and release evidence

These are required test cases, not executed results. Add the behavioral tests
before their fixes, then run the smallest relevant library/adaptor suites and
the real driver scenarios. Keep the existing broad required gates for landing;
do not repeatedly rerun them for documentation-only iteration.

| Acceptance case | Required observation | Package |
|---|---|---|
| Clean source → clone A → dispose A | Successful deletion and correct family row transition, no forced waiver or hidden commit. | DR-2 |
| Marker-only regeneration; then real configuration/staged-marker changes | Canonical derived refresh admitted; real work and unknown evidence still refuse. | DR-2 |
| Member work in A → GWZ commit → default merge A → dispose A | Member and root history preserved, coherent resulting lock, ordinary disposal succeeds. | DR-2/3 |
| Root-only work; diverged root/member histories; conflict/continue/abort | Root participates and existing recovery semantics remain valid; open work blocks disposal. | DR-3 |
| Explicit partial merge and additional refs/stashes/ignored data | No silent scope expansion or loss; remaining hazards are named. | DR-2/3 |
| Every action × default/`@all`/`@root`/exclude/empty/legacy spelling | One declared policy; no ignored flags or handler-level target reinterpretation. | DR-3 |
| Root tag create/list/delete/push; root branch and stash lifecycle | Actual root effects, truthful help and both-driver parity. | DR-3 |
| `gwz --target @root ls` and `gwz --target @root forall ...`, including dry-run | Existing root representation preserved; correct directory, environment and no dry-run spawn. | DR-3 |
| Member push rejection after successful preflight | Earlier member effects reported; root remote unchanged; retry converges. | DR-4 |
| Root-only/partial push with absent or unknown lock dependency | Root withheld with precise diagnosis; no implicit member publication. | DR-4 |
| Successful publication followed by fresh clone | Root lock resolves to available member commits at configured sources. | DR-4 |
| Two SSH identities and explicit selection | Selected identity only, regardless of agent order; truthful supported/unsupported results. | DR-5 |
| Missing sibling, doc drift, Bazel drift, missing policy row or test partition | Required CI fails, with the exact candidate tuple identified. | DR-1/3/6 |
| Cargo/Bazel/current/stale artifacts | Correct version and distinguishable source provenance. | DR-1 |

For each package record the changed repository tuple, commands, actual platform
results, relevant negative cases and any known limitations. Update generated
bindings, help, core design, user guides, document gates and release notes with
the behavior change. Independent review should use that settled tuple, with
particular attention to deletion admission, merge preservation, publication
failure and credential fallback.

The lane-recovery release is complete only when both operator-reported sequences
pass through the shipped driver and real adapters, negative disposal cases still
refuse, selection is consistent, failed member publication cannot advance the
root, and the required workspace checks run in CI. Keep DR-5 and deferred DR-6
items explicitly open until their own evidence exists; a corrected test sheet or
a green unit suite alone does not close the product failures.
