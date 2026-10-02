# TR2.5 Python transport setting — SURFACE-AXIS REVIEW

**Review object:** TR2.5 Python implementation `b2369f1d0bf7..5bf260d040964a3dd9ec606b58a625bc74ac4afc`, workspace `/Volumes/projects/limbo/gwz-dev-tr2-5-py`. Settled review candidate; reviewed 2026-10-03.

**Baseline:** Root `b256a791f1dd05c04caedd8391ef146ff4d3d8fa`; gwz-py `5bf260d040964a3dd9ec606b58a625bc74ac4afc`; gwz-core `2e64e88a28c332ed422cc390adc76738dc701bb1`. README read with `git show 5bf260d040964a3dd9ec606b58a625bc74ac4afc:README.md`; public API inspected through signatures and docstrings only.

**Date:** 2026-10-03

**Axis:** Surface: public names, placement, defaults, availability, diagnostics, and setting/override/removal discoverability. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, or P2 findings; two bounded P3 documentation findings remain.

---

## 0. Evidence base

Read the canonical `py-review/Surface.txt` and workspace `AGENTS_GWZ.md`. No implementation, package contents, controlling implementation document, design, plan, diff, gate log, or peer report was read.

Verified `git rev-parse HEAD` and `git status --porcelain=v1` in root, gwz-py, and gwz-core at both review boundaries. All three HEADs matched the specified tuple and remained unchanged. gwz-py was clean; root and core retained the same out-of-scope untracked files, whose contents were not read.

Read the entire committed README, including:

- Install and Python API sections.
- Python CLI and repository lifecycle documentation.
- Development and Platform And Status sections.
- “Choosing the network transport,” lines 180–217.

Verified both SHA-256 hashes against `py-review/artifacts.json`, at start and end:

| Artifact | SHA-256 |
|---|---|
| `extension-final/gwz/_gwz_core.abi3.so` | `66bb05fd89a2409bfe9b25b3164f6d149eb43863c111f6b1f59c5d5bec1397ed` |
| `extension-transport/gwz/_gwz_core.abi3.so` | `77d1704a066fd8b5d2d4fd876c698ac52da8782df473b1bccaf88424fded52f5` |

Ran the permitted root, fetch, push, and pull help commands, then lane-owner-authorized clone and materialize help. Each exited successfully. Commands used:

```sh
PYTHONPATH=src:../taut/src \
GWZ_PY_NATIVE_MODULE=/Volumes/projects/limbo/gwz-tr25-py-candidate-61-20261003/extension-final/gwz/_gwz_core.abi3.so \
.venv/bin/python -m gwz.cli [command] --help
```

Ordinary root help, with `GWZ_PY_NATIVE_MODULE` omitted, also exited successfully and presented the same reviewed transport-related text.

Public Client introspection produced:

```text
(root: 'str | Path | None' = None, bridge: 'CoreBridge | None' = None, *,
 max_connections_per_host: 'int | None' = None) -> 'None'
```

Its class docstring states that `None` leaves the limit unset, resolves to 32 on gwz or 8 on native in the transport candidate, honours explicit positive values, and permits per-call override. `Client.__init__.__doc__` was `None`.

Additional authorized signature/docstring introspection showed:

```text
configure_transport_timeout (self, seconds: 'int') -> 'TransportRuntimeResponse'
close (self) -> 'TransportCleanup | None'
fetch (self, **meta: 'Any') -> 'FetchResponse'
fetch_stream (self, **meta: 'Any') -> 'AsyncIterator[OperationEvent]'
pull_head (self, **meta: 'Any') -> 'PullHeadResponse'
pull_head_stream (self, **meta: 'Any') -> 'AsyncIterator[OperationEvent]'
pull_snapshot (self, snapshot_id: 'str', **meta: 'Any') -> 'PullSnapshotResponse'
pull_snapshot_stream (self, snapshot_id: 'str', **meta: 'Any') -> 'AsyncIterator[OperationEvent]'
push (self, *, remote: 'str | None' = None, refspec: 'str | None' = None,
      remote_check: 'RemoteCheck | None' = None, **meta: 'Any') -> 'PushResponse'
push_stream (self, *, remote: 'str | None' = None, refspec: 'str | None' = None,
             remote_check: 'RemoteCheck | None' = None,
             **meta: 'Any') -> 'AsyncIterator[OperationEvent]'
```

The fetch docstring describes contacting selected remotes without integration or a workspace artifact. The other listed method docstrings were `None`. An initial introspection request for `Client.pull` stopped with `AttributeError`; subsequent public-name discovery identified the actual `pull_head` and `pull_snapshot` methods. No operations, builds, or tests were executed.

## 1. Findings

### [P3-1] CLI concurrency options omit their transport-dependent defaults

**Location:** Root, fetch, push, pull, clone, and materialize `--help`, entries for `--jobs` and `--max-per-host`.

**Violated invariant:** Every reviewed option must state its default. Transport selection changes the effective concurrency defaults, making this information relevant to using the new setting.

**Reproduction:** Read any reviewed help output. The entries say only:

```text
--jobs …          Global ceiling on concurrent member operations
--max-per-host …  Max concurrent connections to any one host
```

Neither states the omitted-option behaviour. README lines 203–207 instead establish jobs/host defaults of 100/32 for gwz and 50/8 for native, and distinguish omission from an explicit value.

**Impact:** A help-only user cannot determine the concurrency policy or understand that explicitly supplying 32 differs from leaving the native host limit unset. The README supplies the missing information, so this is bounded documentation debt.

**Required correction:** State the effective defaults and omission semantics in the shared help text, including that explicit positive values override those defaults.

**Closure/regression test:** Inspect root and all reviewed network-command help outputs and confirm that both effective default pairs appear consistently.

### [P3-2] CLI help does not expose the transport-setting lifecycle

**Location:** Root and reviewed network-command `--help`; the sole transport-selection hint is inside `--ssh-timeout`.

**Violated invariant:** The first-day help-only walkthrough must reveal how to select the setting, use an override, and remove it, including candidate availability and the default.

**Reproduction:** Start at root help and follow fetch, push, pull, clone, or materialize help. The timeout entry mentions `GWZ_TRANSPORT=native`, but no reviewed help explains:

- The `gwz`/`native` choices and default.
- The global `gwz.transport` setting and environment precedence.
- How to remove either setting or override.
- That the selector belongs to the 1.1.0 transport candidate.

**Impact:** A CLI user can infer an environment-variable spelling but must guess the remainder of the setting’s lifecycle or find the README independently. README lines 182–191 document the lifecycle correctly, so no compatibility change is needed.

**Required correction:** Add a concise transport-selection section or epilog to root and network-command help, with choices, default, candidate availability, precedence, and removal commands.

**Closure/regression test:** Repeat the help-only walkthrough and derive a temporary native invocation, persistent native selection, explicit gwz override, and removal of both forms without consulting another document.

## 2. Invariant analysis

The documented setting lifecycle otherwise held. README lines 182–191 specify selection at each operation, environment override, persistent global configuration, explicit removal, and applicability to an existing Client. No new transport constructor argument is advertised, and the constructor signature confirms that interface shape.

The host-limit documentation is consistent between README and Client’s public class docstring. `None` means unset; explicit positive values, including 32, remain explicit; per-call values override the constructor.

The diagnostic distinction is explicit: ignored repository/worktree values produce logger `gwz` WARNING records once per file per Client; native selection produces a `UserWarning`. The README explains Python warning-filter control, including warnings-as-errors, and CLI stderr notes. These were assessed as public contracts, not verified runtime outcomes.

Timeout shape is consistent across README and help: default 9 seconds, zero disables it, one process clock, and transport changes do not reset it. The README identifies the API configuration method and its before-first-operation constraint.

Candidate and platform availability are described in the README. Release, Linux, Windows, live-route, performance, and distributable-wheel outcomes were not evaluated.

The command names and summaries examined did not reveal an additional placement or naming defect attributable to this object. Fetch explicitly promises no integration; pull identifies an explicit target; push identifies member refs. Clone and materialize retain their established command-family placement.

## 3. Risks and next action

This review establishes the public surface only. It does not establish resolver execution, warning timing, warnings-as-errors side effects, cancellation, cleanup timing, or transport behaviour.

The next action is to add the missing defaults and transport lifecycle information to CLI help, then inspect the resulting help text. Neither finding requires a compatibility break or blocks this Surface verdict.
