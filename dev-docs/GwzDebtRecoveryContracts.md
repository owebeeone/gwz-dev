# Debt recovery contract and recovery audit

2026-09-07. This records the current policy owner, accepted exceptions and
remaining verification. It is not a claim that every matrix cell has executed
on every platform. The wire catalog remains unchanged at slots 0–71; no error
slot is recycled by this audit.

## Selection ownership

`gwz-core/src/workspace_ops/target_selection.rs` owns normalization, action
policy, frozen-open-merge selection and literal lifecycle selector grammar.
The action match is exhaustive over the current 30 `ActionKind` variants.
Both drivers forward selections; `forall` calls `resolve_forall_targets`.
The source tripwire and its negative fixtures reject direct handler rescans.
It is a source check, not a Rust data-flow proof.

Normal set semantics: expand includes (or the verb default), deduplicate,
then subtract excludes. An absent selection and an empty/default envelope
have the same meaning. `@all` means all supported targets. A surviving literal
`@root` on a member-only capability is an explicit refusal; an `@all` expansion
may omit unsupported root capability. Execution ordering is separately owned:
commit and push must still perform members before root publication.

| Action | Suboperations / form | Default and root policy | Verification / exception |
|---|---|---|---|
| CreateWorkspace | create | Workspace structural operation | Nonempty target selection refused before effects; empty envelopes accepted. |
| InitFromSources | initialize, update | Workspace structural operation | Source operands define members; nonempty target selection refused before effects. |
| AddExistingRepo | adopt existing repository | Workspace structural operation | Explicit source/path owns the new member; unused target selection refused. |
| CreateRepo | create member | Workspace structural operation | Explicit source/path owns the new member; unused target selection refused. |
| CloneRepoMember | clone member URL | Workspace structural operation | Explicit source/path owns the new member; unused target selection refused. |
| DetachRepoMember | detach | Exactly one literal member id or path | No sets or exclusions; grammar is centralized. |
| AttachRepoMember | attach | Exactly one literal member id | No sets, paths or exclusions; grammar is centralized. |
| CloneWorkspace | URL clone | Root clone, then selected supported members | Explicit selection validated against remote manifest before allocation; materialize policy owns member expansion. Dry-run remains refused. |
| CloneLocalWorkspace | verbatim | Whole workspace; nonempty selection refused | Clean, bare and copy-source modes remain unsupported; local dry-run refused before effects. |
| LocalFamily | list, dispose, disband | Whole family operation; nonempty selection refused | Empty envelopes accepted; list is read-only; other dry-runs remain unsupported. |
| RemoteIdentity | get, set, unset | Members by default; root supported through common selection | Local Git configuration only; full local preflight; dry-run reports proposed settings; no network or transport options. |
| Materialize | lock, head, named branch, tag | Active supported members | `@all` omits root; explicit surviving `@root` refuses. Existing tagged default subset is preserved. |
| Snapshot | capture named snapshot | Members | Root unsupported by this action policy; member-only snapshot artifacts retain their existing owner. |
| Capture | update recorded member states | Members | Root is not a selected repository; lock publication remains an effect of capture. |
| PullSnapshot | materialize snapshot | Members | Same supported-member policy; explicit root refuses. |
| ListSnapshots | list snapshot records | Workspace records | Does not select repositories; nonempty selection refused before effects. |
| RepoSync | reconcile repository configuration | Members | Supported-member policy; root not silently reconfigured. |
| Status | inspect | Root and members | Shared resolver; read-only. |
| Diff | working/index/tree/range | Root and members | Existing path routing and range semantics remain separate from target expansion. |
| Log | history/range/output | Root and members | Existing history planner and continuation contracts remain; direct policy owner is shared. |
| Ls | list targets | Members; root supported | Root uses the existing `MemberEntry` and `TargetKind::Root`. |
| Forall | resolve, execute command | Members; root supported | Core resolves; drivers execute. Rust/Python dry-runs do not execute commands. |
| Stage | ordinary add | Root and members | Path ownership further constrains the selected scope. |
| Stage | add during an open merge | Frozen merge participants | Uses the record, not a moving manifest. Exclusion-only and unknown-token cases are tested. |
| Commit | commit work and lock | Root and members | Member effects precede the root lock commit; explicit selection remains bounded. |
| Push | selected refs, root-only, partial | Root and members | Member failures block root; committed-lock dependency proof also applies to omitted members. |
| PullHead | head materialization / integration | Root and members | Both head preflight paths consume the shared policy; network identity preflight precedes effects. |
| Merge | ordinary start, family start | Root and members | Default/all and explicit member-only cases tested; default lane merge/disposal passes. |
| Merge | status, continue, abort, preserve, GC | Existing operation record | Recovery participant vector is frozen; no fresh target expansion is authorized. |
| Branch | list, create, create-and-switch, delete | Members; root supported | Root-only lifecycle tested. Combined switch refuses changed selected member identities in the destination root manifest. |
| Branch | legacy merge suboperation | Refused | Existing deprecated-operation contract remains; use the merge action. |
| Tag | local create/list/delete | Members; root supported | Root annotated tag create/list/delete tested. |
| Tag | remote list/fetch/push/delete | Members; root supported | Root push/deletion tested; tagged-commit dependency proof is shared with push. Additional remote fetch/negative combinations remain. |
| Stash | push | Members; root supported | Root participates in the coordinated bundle and native stash lifecycle. |
| Stash | list | Members plus discoverable root preservation bundles when unfiltered | Deliberate discovery exception: recovery stashes must remain visible. Explicit scopes constrain discovery. |
| Stash | apply/pop/drop | Eligible participants of the chosen bundle when unfiltered | Explicit selection narrows the bundle; empty envelopes behave as unfiltered. Root push/list/pop and existing coordinated recovery tests pass. |

Current audit: non-local structural requests now either refuse unused selection or
consume it explicitly. Root branch, tag and stash lifecycle tests also check dry-run
non-mutation. Partial family merge diagnostics name the remaining root integration
requirement without relaxing disposal. The shared resolver owns intentional empty
results; platform/driver execution is reported separately rather than inferred from
this policy table. Ordinary publication, root failure/retry/fresh clone and inactive
fetch-only dependency cases have local executable coverage. There is no general
remote-push cancellation API in the current protocol; termination cannot promise
rollback of already accepted member refs. Root publication still requires its
dependency barrier, and retry rechecks current availability.

## Error and recovery catalog

| Existing wire codes | Recovery class | Caller action |
|---|---|---|
| ok (0) | Success | No recovery. |
| invalid_request (1), tag_invalid (21), deprecated_operation (37) | Request/selection | Correct the command or select its supported replacement; retrying unchanged is unhelpful. |
| workspace_not_found (2), workspace_already_exists (3), nested_workspace (4), manifest_not_found (5), manifest_invalid (6), schema_unsupported (7), lock_not_found (19) | Workspace admission | Locate or repair the correct workspace through supported GWZ operations; do not bypass integrity checks. |
| member_not_found (8), member_inactive (9), path_escape (10), path_collision (11), path_reserved (12) | Target/layout admission | Correct the member or path before effects. |
| unsupported_source_kind (13), unsupported_operation (14) | Capability unavailable | Choose a supported operation/source; do not blindly retry or force. |
| dirty_member (15), diverged_member (16) | Work/history protection | Preserve work or integrate histories before retrying. |
| missing_remote (17), snapshot_not_found (18), tag_not_found (20), operation_not_found (25), stash_not_found (33) | Missing named input | Inspect the correct namespace and correct the name. |
| remote_rejected (22) | Remote ref rejection | Fix permissions, server policy or history; already successful peer pushes remain real effects. |
| git_command_failed (23), external_tool_missing (24) | Native/required-tool failure | Inspect the reported operation; install the actually required tool or resolve the native failure. This currently conflates authentication failures and needs refinement in DR-5. |
| attribution_denied (26), permission_denied (27) | Attribution / local access | Correct actor authority or filesystem permissions. Neither proves a remote account identity. |
| io_error (28), internal_error (29) | I/O / invariant failure | Inspect exact effects and evidence; do not convert these into a loss waiver. InternalError is not a capability or selector fallback. |
| branch_detached_head (30), branch_unborn_head (31), branch_mixed (32) | Branch state | Supply an appropriate branch/ref or initialize the repository. |
| stash_incomplete (34), stash_conflict (35) | Coordinated stash recovery | Inspect the bundle, restore remaining participants or resolve conflicts; retain recovery evidence. |
| source_identity_mismatch (36) | Wrong source pairing | Restore compatible member identity/path or perform an intentional structural transition. Also applies to the new mixed root/member branch guard. |
| merge_validation_failed (38), merge_id_mismatch (39), merge_drift (40), open_operation (41), merge_recovery_required (42), merge_phase_unsupported (43) | Merge lifecycle | Inspect the named operation and use its supported continue/abort/recovery route. |
| root_merge_not_yet_supported (44), merge_record_unreadable (45), unsupported_record_version (46), unsupported_legacy_mode (47) | Legacy/record compatibility | Preserve the record and use the supported reader/version. Slot 44 is not reusable just because current root merge is supported. |
| archived_record_unreadable (48), unexpected_acceptance_evidence (49), acceptance_input_drift (50), candidate_integrity_mismatch (51), ambiguous_evidence_commit (52), recorded_evidence_drift (53), publication_prefix_mismatch (54), published_candidate_mismatch (55), preservation_evidence_mismatch (56), rollback_evidence_mismatch (57), unexpected_publication_evidence (58), terminal_evidence_mismatch (59), recovery_evidence_mismatch (60), terminal_rollback_mismatch (61) | Frozen acceptance/evidence reason catalog | Preserve evidence and inspect the specific predicate. The compatibility battery freezes all fourteen reason contracts; no renumbering or catch-all replacement. |
| unknown_local (62) | Family namespace/state | Inspect local family membership/readiness. Never fall back to a Git remote with the same token. |
| unsupported_source_layout (63) | Unsafe clone source layout | Refuse before reservation; use a supported self-contained source. |
| copy_failed (64) | Partial copy | Inspect the retained creating row and destination. Source remains intact. |
| source_drift (65) | Source changed during clone | Quiesce the source and inspect retained partial destination before retrying. |
| destination_incomplete (66) | Install completion failed | Inspect retained destination/creating row; no ready state is claimed. |
| pairing_mismatch (67) | Family import pairing | Correct source/member identity before imports. |
| import_incomplete (68) | Partial family import | Inspect reported retained refs; retry uses a fresh import id. |
| unwaived_hazard (69) | Known protected work/history | Preserve it, keep the lane, or explicitly waive the named loss. |
| unknown_evidence (70) | Preservation cannot be proved | Obtain reliable evidence; no force name waives unknown evidence. |
| disposal_incomplete (71) | Disposal lifecycle/effects | Inspect the remaining directory and disposing row. A late marker-proof change can reach this class before any file removal; the message explicitly says nothing was removed. |

No new code is needed for an unsupported root selector or a changed branch
source pairing: existing InvalidRequest and SourceIdentityMismatch have the
right recovery actions. Two distinctions still warrant an additive schema
proposal before release: local publication-dependency blocking versus a server's
ref rejection, and credential-selection/authentication failure versus generic
Git failure. Identity availability, authentication, server authorization and
commit attribution must not be collapsed into one claim about an account.

Any new classification belongs in taut, the model conversion and both generated
bindings together. Preserve all existing numeric/string slots and prove the
additive projection before advancing the compatibility pins. Human and JSON
fixtures must demonstrate the distinct recovery action, including already
completed member effects when root publication is withheld.

## Required coverage manifest

The workspace job is `.github/workflows/debt-recovery.yml` / `workspace`, on
ubuntu-24.04 and macos-14. It remains unactivated until a reviewed workspace
revision is published and required checks are configured. Commands below are
relative to the indicated repository. Generated census execution covers the
entire core library; focused commands identify the invariant's owner without
asserting that a filtered test alone constitutes release acceptance.

| Invariant / owner | Executable check | Topology | Platform / required job |
|---|---|---|---|
| Derived-marker admission / local-clone installation and disposal | Core `cargo test --lib local_clone::tests::dispose::integrity` | Real root/member repositories, untouched and edited lanes, late drift | Linux/macOS workspace job; current macOS evidence |
| Root-inclusive lifecycle / shared selection and family merge | CLI `cargo test --test local_family_workflows` and Python local-family tests | Member change, GWZ commit, default family merge, ordinary disposal | Linux/macOS workspace job; current macOS evidence |
| Root branch, tag and stash / shared resolver plus verb owner | Core `workspace_ops::tests::g21`, `g17`, `g20` within full census | Root-only and mixed repositories; dry-run and recovery | Linux/macOS workspace job; current macOS evidence |
| Publication barrier / publication.rs and native push_plan.rs | Core `cargo test --lib workspace_ops::tests::g08` and native prepared-push regression | Root/member bare remotes, rejection, inactive/omitted dependencies, callback drift, retry and fresh clone | Linux/macOS workspace job; current macOS evidence |
| Explicit file identity / native transport owner | Python native read/mutation suites, core identity tests; `ssh_identity_probe.py --product` controlled fixture | Isolated keys/agent/loopback server; real production read transport | Unit/driver checks in workspace job; controlled SSH fixture locally measured, remote acceptance pending |
| Protocol compatibility / taut owner | Core protocol integration test, additive projection, Python packaged-IR drift; aggregate `compatibility` battery | Generated bindings and both document-consistency gates | Workspace job plus existing core release gates |
| Protected write authority / checked-artifact owner | `check_checked_artifact_boundaries.py` and its compiler/negative suite | Mutated throwaway source trees, compiler authority checks | Existing checked-artifact-boundary job; local suite recorded in checkpoint |
| Exact build tuple / workspace CI helper | `test_workspace_tuple.py`, `test_workspace_evidence.py`, build identity tests and Cargo/Bazel provenance comparison | Exact locked siblings with optional candidate override | Workspace job; required-check activation pending |
| Census completeness / test_inventory.py | Cargo artifact selection, frozen binary listing/run, reconciled identities and negative controls | Separate platform/package/profile inventories | Workspace job alongside old count pins; merge-base baselines and cutover require both platforms |

Windows remains a separate existing core `release.yml` / `windows-matrix.yml`
contract, including its CRLF/creation-time-filter sentinels. No Windows result is
inferred from macOS, and the new workspace job does not claim Windows coverage.
Exact encrypted-agent support and deferred mechanical splits have their explicit
separate owners and acceptance criteria in the capability report and checkpoint.
