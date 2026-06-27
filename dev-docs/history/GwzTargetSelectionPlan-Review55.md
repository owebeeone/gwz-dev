# GwzTargetSelectionPlan Review55

## Findings

- **P0: W0 is not independently mergeable after the Taut changes.** W0 adds
  `Selection.targets`, `Selection.exclude_targets`, and
  `MemberEntry.target_kind` in the generated protocol
  ([plan:644](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:644)),
  but Rust/Python generated structs and dataclasses require those fields at
  construction sites. Current literals exist in
  [gwz-cli/src/clirequest.rs:299](/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs:299),
  [gwz-core/src/workspace_ops/handle_ls.rs:43](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_ls.rs:43),
  [gwz-cli/src/forall.rs:240](/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/forall.rs:240),
  and Python constructors at
  [gwz-py/src/gwz/client.py:132](/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py:132).
  Move the mechanical constructor/default-field sweep into W0, or change
  generation/builders so newly optional fields are source-compatible. Add
  `cargo check` for `gwz-core`/`gwz-cli` and Python import/compile checks to
  the W0 gate, not only protocol regen tests.

- **P1: Unsupported-root rejection timing is ambiguous and can break the
  canonical subtractive selector.** The algebra says final selection is
  includes minus excludes
  ([plan:102](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:102)),
  but the unsupported-root rule says reject if an explicit selector "includes"
  an unsupported target
  ([plan:204](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:204)).
  If implementers check raw includes, `materialize --all --no-target @root`
  would reject even though `--all --no-target @root` is the documented
  all-except-root spelling
  ([plan:543](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:543)).
  Specify that unsupported-target policy applies after include-minus-exclude
  resolution, while `--all` alone still rejects for root-unsupported commands.
  Add tests for both cases.

- **P1: Several commands in the default matrix are not assigned implementation
  ownership.** The matrix requires explicit `@root` rejection for `capture`,
  `snapshot`, `materialize`, `tag`, `branch`, and `stash`
  ([plan:195](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:195)),
  but W2c only audits status/pull/commit/stage
  ([plan:844](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:844)).
  Current branch and stash handlers call the old locked member resolver directly
  ([handle_branch.rs:24](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs:24),
  [handle_stash.rs:53](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs:53)).
  Add an explicit member-only command migration slice covering
  branch/stash/tag/capture/snapshot/materialize with acceptance tests for
  default, `--all`, `--all --no-target @root`, and `--target @root`.

- **P1: `forall` positional selector semantics are underspecified for
  combinations with global selectors.** The plan correctly notes current
  Rust/Python `forall` filters the default `ls` result
  ([forall.rs:52](/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/forall.rs:52),
  [cli_local.py:53](/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_local.py:53)),
  but only says positionals become selector tokens
  ([plan:410](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:410)).
  Define whether positionals replace global includes, append to them, or are
  just another include list subject to global exclusions. Add parity tests for
  mixed forms such as `gwz forall --target mem_a mem_b -- ...`,
  `gwz forall --all --no-target @root mem_a -- ...`, and unknown positional
  `@name`.

- **P2: Taut target identity is only added to `MemberEntry`, leaving
  push/events/errors sentinel-based.** The plan adds `MemberEntry.target_kind`
  ([plan:244](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:244))
  but root push responses and events use `MemberResponse`, `OperationEvent`,
  and `GwzError`, whose current fields are still member-shaped
  ([generated.rs:1870](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs:1870),
  [generated.rs:1934](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs:1934)).
  Either ratify `member_id="@root"`/`member_path="."` as the full v0 protocol
  contract for non-list responses, or add optional `target_kind` consistently
  to response/event/error surfaces in W0. Do not leave this as an open decision
  past schema freeze
  ([plan:958](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzTargetSelectionPlan.md:958)).

- **P2: Acceptance tests miss some compatibility and parity edges.** Add
  explicit gates for legacy `all=true` normalization to `@all`, legacy
  `member_ids=["@root"]` alias handling or rejection, Rust/Python
  request-construction equality for `targets`/`exclude_targets`, global `--all`
  versus command-local `add -A/--all` and `commit -a/--all`, machine-output
  root entries, and Python `all_members` API behavior after its semantics
  change from "members" to "targets".

