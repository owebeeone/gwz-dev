# GWZ invocation path contract

**Status:** implementation audit, 2026-09-10

This document records the path base for every path-bearing GWZ request field.
It is the authority for drivers and remote bridges that construct `RequestMeta`.
The core rule is simple: a serialized request supplies its caller filesystem
context; the receiver never substitutes its own process working directory.

## Shared base and compatibility

`RequestMeta.invocation.caller_cwd` is an execution-filesystem path. Serialized
drivers capture it once before parsing, dispatch, or asynchronous handoff. It
must be absolute; core lexically normalizes it and rejects a relative value.
`WorkspaceRef.root`, when supplied in a serialized request, must likewise be an
absolute normalized execution-filesystem path. With no root, core discovers the
workspace from `caller_cwd`.

Direct in-process compatibility APIs may omit `invocation`, but must supply an
explicit absolute `start`. That `start` is the only legacy base. A legacy
relative workspace root or operand is qualified at that adapter boundary; no
core path resolver reads ambient cwd. A serialized request with an invocation
context rejects a relative workspace root instead of applying this legacy rule.

An executor on another host or in a container must only serialize a
`caller_cwd` that exists in *its* accessible filesystem namespace. A desktop
client path does not become meaningful merely by sending it to a remote daemon.
The bridge must map it to an accessible execution path before serialization, or
reject the request. The same applies to explicit roots, local repository
operands, local clone sources and identity files.

## Field audit

| Field or namespace | Meaning and required base |
| --- | --- |
| `RequestMeta.invocation.caller_cwd` | Absolute caller directory in the execution filesystem. It is the base for caller-relative filesystem operands. |
| `RequestMeta.workspace.root` | Absolute serialized workspace root. Omitted means discover from `caller_cwd`; legacy direct calls may qualify a relative root against explicit `start`. |
| `CreateWorkspaceRequest.workspace_root` and `InitFromSourcesRequest.workspace_root` | Filesystem destination. A serialized relative value resolves against `caller_cwd`; an absent invocation requires an absolute legacy value for create and uses explicit `start` for init. |
| `AddExistingRepoRequest.repository_path` | Local repository operand. Absolute values stay absolute; relative values resolve against the invocation base. A supplied `member_path` is separately a logical workspace path and must name that repository under the resolved root. |
| `CloneWorkspaceRequest.target` | Local destination. Absolute values stay absolute; relative values resolve against the invocation base. The legacy clone wrapper accepts only an absolute target because it has no serialized context. |
| `CloneWorkspaceRequest.url`, `CloneRepoMemberRequest.source.url`, and every `InitFromSourcesRequest.sources[*].url` | A local Git source path resolves against the invocation base before validation, probing, or cloning. Scheme URLs, including `file://`, and scp-style Git sources retain their exact wire spelling. |
| `TransportOptions.default_identity` and `remote_identities[*].private_key_path` | Invocation-scoped SSH key files resolve against the explicit invocation base before transport setup. |
| `RemoteIdentityRequest.private_key_path` | The persisted local Git identity file resolves against the invocation base when setting an identity. Get and unset do not resolve a new file. |
| `StageRequest.cwd` | Physical operand base for stage pathspecs. Serialized requests carry it as an absolute path; legacy direct calls qualify it against explicit `start`. |
| Diff and log `workspace_cwd` | Optional *logical* workspace-relative cwd used for reporting and compatibility. It is not an execution filesystem base. When caller cwd lies outside the workspace, drivers serialize `None`. |
| Diff/log pathspecs | Resolve against the physical invocation caller directory, then undergo ordinary workspace/member routing. An outside caller may use an absolute path inside the workspace; a relative path outside it is rejected as an escape. |
| Selection `paths`, command `member_path` values, manifest member paths, `SourceUrl.path`, and emitted member paths | Logical workspace-relative namespaces. They are validated as member/selection paths and never resolved against caller cwd. |
| Snapshot names/IDs, branch names, tags, remote names, source IDs, and merge IDs | Identifiers, not filesystem paths. They do not accept caller-relative path semantics. |

## Returned path namespaces

`LsResponse.members[*].abspath` is an absolute execution-filesystem path formed
from the resolved workspace root and the logical member path. `MemberEntry.path`,
member response paths, planned action paths, and event member paths remain
logical workspace-relative values. Transport observation `repository_path` and
reported configured identity paths describe the execution filesystem; callers
must not reinterpret them as portable client paths.

Core-owned artifact names such as `gwz.conf`, lock files, snapshots, merge
records, and bootstrap files are derived under the resolved workspace root.
They are not request operands and therefore never use the executor's ambient
working directory.

## Evidence

Passed focused tests:

- `workspace_ops::tests::g02::serialized_invocation_context_overrides_an_unrelated_executor_start`
- `workspace_ops::tests::g02::serialized_context_refuses_relative_caller_and_root`
- `workspace_ops::tests::g02::outside_caller_allows_absolute_workspace_operands_but_rejects_relative_ones`
- `workspace_ops::tests::g02::local_git_sources_bind_to_the_serialized_caller_but_remotes_do_not`
- `tests::g09::serialized_context_keeps_an_outside_caller_distinct_from_workspace_root`

The broad `cargo test invocation --lib` filter was not used as release evidence:
it also selected two unrelated tests, both of which failed because their fixture
workspace repository was missing:

- `workspace_ops::merge::v1_lifecycle::service::tests::preparation_failure_halts_and_returns_without_same_invocation_retry`
- `workspace_ops::merge::v1_lifecycle::service::tests::failed_owned_action_is_not_executed_twice_in_one_invocation`
