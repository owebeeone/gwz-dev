# Step 2.2 unpark the cli placement — STATE-AXIS RE-VERDICT, round 2 (gwz-core 58a03c4d, gwz-py 55ff56c, evidence 4578557c)

**Review object / Baseline / Date / Axis / Verdict:** gwz-core `df31c221..58a03c4d`, gwz-py `bde35e92..55ff56c`, evidence `4578557c` (the round-1 remediation of run `2026-10-25-p2-2`) / round-1 State review on `df31c221`, `54a74ecb`, `bde35e92` / 2026-10-26 / State / **GO**: P0 0, P1 0, P2 0, P3 0 open, 1 new observation.

## 0. Evidence base

- **Tuple:** the same at start and end. gwz-core `58a03c4dd7`, gwz-py `55ff56c45c`, gwz-cli `54a74ecb4e` (unchanged), gwz-core-evidence `4578557ccd`, gwz-transport `22b53dce5d`. All clean, except one unrelated untracked run directory in the evidence repo that was already there before this round.
- **Digest:** I rebuilt the evidence digest from the commit range `df31c221..58a03c4d` (base `df31c221`, then the diff; no files were added). It gives `241e13c0…1018`. That matches the start and end of remote run `b` and the at-booking and after-run digests of Windows receipts `b` and `c`. So the committed tree is the tree all three gates ran on.
- **Runs** (load about 5):
  - `cargo test --test protocol`: 46 passed, including `the_owner_pin_survives_a_crlf_checkout_and_refuses_a_changed_export` and `generated_protocol_is_current`.
  - `cargo test --lib cli_placement_refusal_tests`: 3 passed.
  - gwz-py `test_client.py -k cli_placement`: 10 passed.

## 1. Prior-finding closure

| ID | Remedy on this tree | Counterexample re-run | Status |
|---|---|---|---|
| P2-1: the owner pin hashed raw bytes, so a CRLF checkout fails | `gwz.taut.py` `_with_owner` and `tests/transport_consumer/protocol/regen.py` hash and parse `read_bytes().replace(b"\r\n", b"\n")`. | The new row writes a CRLF copy and loads it, which succeeds. It changes one tag and the copy is refused in both LF and CRLF form, with "is not the pinned one". The row passes on the Mac, on remote `b` (2387 passed) and on Windows `c` (3978 passed). Windows `c` had taut-proto 0.10.0 in a venv, and `generated_protocol_is_current` and both `log_addition_*` rows ran and passed. The three remaining gaps are the embedded-Python rows, which fail with "codec input is missing", and the receipt now classifies gaps by failure text (`is_host_gap`, `MISSING_TOOLING`). | **Closed** (see the observation in §2) |
| P3-1: gwz-py Client sent `cli` without checking capabilities | `_require_transport_capability` now queries capabilities when `placement == cli`. It requires `cli` in `placements` and a common `message_versions`, and raises `UnsupportedOperation` otherwise. This covers both the call path and the submit path. | Tests with an old two-key answer, and with an answer lacking `cli` or the version, raise before any call is made. The recorded bridge calls are `["transport_capabilities"]` only. | **Closed** |
| P3-2: the trait's default `with_transport` ignored `cli` | `methods_transport.rs`: the default refuses `placement == Some(Cli)` with `UnsupportedOperation` before its identity check. | `the_trait_default_refuses_an_explicit_cli_placement` on `FakeGitRepository` passes. | **Closed** |
| P3-3: remote matrix `r3` was not bound to the final tree | Remote run `b` records digests at start and end. Both are `241e13c0…`, the same digest as Windows `c`. | Recomputed from the commit, as in §0. | **Closed** |

## 2. New findings

None at P0–P3.

**Observation (no severity):** the Windows receipts record `owner_export_line_endings: lf` for runs `a`, `b` and `c`. dabeest's staging does not reproduce the hosted CRLF checkout. So the CRLF case on Windows is proven only by the test's synthetic CRLF copy, which is a platform-independent mechanism test that also ran on Windows. It is not proven by a real `core.autocrlf=true` checkout.

The fix normalizes regardless of where the CRLF comes from, so I do not hold acceptance on this. After the push, read the hosted `windows-matrix` run and gwz-py's `package-smoke` Windows rows. They are the first real CRLF checkouts this code will see.

## 3. Risks and next action

- The State axis is GO for step 2.2.
- After the push, read gh CI for gwz-core `windows-matrix` and `release` `verify-windows`, and for gwz-py `package-smoke` on windows-2022. This confirms the observation above on real CRLF checkouts.
- The round-1 observations that were not findings are unchanged: the gwz-py `--json` output shape, the Python constructor keyword arguments, and the `gwz_core::ErrorCode` name exposure, which is for the Surface review. `transport_message` and a stray `endpoint_path_base` are now refused in every build (C-P3-2), and that removes the round-1 observation about the two build shapes validating differently.
