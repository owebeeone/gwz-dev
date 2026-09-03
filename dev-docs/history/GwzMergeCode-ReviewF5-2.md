# GWZ Merge M0 Remediation — Independent Re-Review (F5-2)

Date: 2026-07-18. Reviewer: Claude (Fable 5), read-only re-review.

Scope: the remediation of `dev-docs/GwzMergeCode-ReviewF5.md` per
`dev-docs/GwzMergeCodeRemPlan-1.md`, reviewed independently against the
implementation, tests, machine output, and documentation.

Ranges reviewed (docs-freeze commit + remediation commit per repo):

| Repo | Range | Commits |
| --- | --- | --- |
| gwz-core | `c315edc..7321362` | `bb2215c` docs freeze, `7321362` fix |
| gwz-cli | `c1d6c24..7a90a9a` | `b586663` docs freeze, `7a90a9a` fix |
| gwz-py | `e482c21..7cde9c0` | `7cde9c0` fix |
| root | `7f667f2..dd48d70` | `bc679ea` docs freeze, `dd48d70` fix |

Every file in each range was read. The two untracked HN drafts were ignored
and remain untouched.

---

## 1. Findings

No P0, P1, or P2 findings. Two P3 findings.

### [P3-1] Structured member fields do not survive either CLI's top-level error envelope

- **Where:**
  [clirequest.rs:336](gwz-cli/src/clirequest.rs:336) — `CliError::from_model`
  keeps only `message` and `code`, dropping the new
  `ModelError.member_id`/`member_path`;
  [append_branch_summary.rs:938](gwz-cli/src/append_branch_summary.rs:938) —
  `render_error_json` hardcodes `member_id`/`member_path`/`detail` to null;
  [shims.rs:16](gwz-py/native/src/shims.rs:16) — the Python bridge flattens
  the error to `err.to_string()`;
  [cli_render.py:87](gwz-py/src/gwz/cli_render.py:87) — Python's
  `render_error` JSON mode likewise emits `member_id: null`.
- **Violated criterion:** remediation exit criterion 1 — "every per-member
  preflight failure identifies the member in **both human text and structured
  machine error fields**." The structured half is implemented and tested at
  the core protocol-conversion layer
  ([convert.rs:174](gwz-core/src/protocol/convert.rs:174), and asserted
  end-to-end for participant-row errors by the real-Git halt test), but a
  whole-operation preflight rejection reaches both CLIs as a top-level error,
  and on that path the structured fields are discarded.
- **Failure scenario:** `gwz merge feature/x --json` in a workspace where the
  second member lacks the source ref prints
  `{"errors":[{"code":"GitCommandFailed","message":"member 'mem_a' at 'a':
  revspec 'feature/x' not found…","member_id":null,"member_path":null,…}]}`.
  A JSON consumer must regex the member out of the message.
- **Impact:** the P2-1 diagnosability regression itself is fixed — every
  surface (human, JSON, JSONL, Python, Rust) now names the member in a stable
  `member '<id>' at '<path>': ` message prefix, and the typed code is
  preserved — so this is a completeness gap, not a recurrence. It also
  creates an asymmetry: participant errors inside a `MergeResponse` carry
  structured member fields; whole-operation errors do not, even when core
  knows them.
- **Recommended correction:** carry optional `member_id`/`member_path` on
  `CliError`, populate them in `from_model`, and emit them in
  `render_error_json`; on the Python side, either encode the fields into the
  bridge error in a structured form or accept the message-prefix contract and
  document it in `MachineOutput.md`'s error section.
- **Regression test:** Rust CLI test asserting
  `errors[0]["member_id"] == "mem_a"` for a missing-source second member under
  `--json`; a Python equivalent through `cli.main`.

### [P3-2] The canonical parity fixture does not pin the participant error sub-shape

- **Where:**
  [merge_response.json](gwz-core/protocol/fixtures/cli_parity/merge_response.json)
  — every `repos[].error` is `null` and the envelope `errors` array is empty;
  the error object serializers remain two independent implementations
  ([git_status_json.rs:65](gwz-cli/src/git_status_json.rs:65) `error_json` and
  [cli_render.py:509](gwz-py/src/gwz/cli_render.py:509) `_merge_error_json`).
  Both currently emit the same five keys and both omit `GwzError.target_kind`,
  which the protocol type carries and which core now sets for member-context
  errors.
- **Violated criterion:** the R3 intent — "so future M1 data cannot be
  silently dropped" — is met for drift, preservation, counts, and publication
  step, but not for the error object, which is exactly the sub-shape M1's
  halted/failed responses will exercise most.
- **Failure scenario:** a future edit adds `target_kind` (or renames a key)
  in `error_json`; all Rust tests pass, all Python tests pass, the fixture
  compares equal on both sides (it contains no error object), and the drivers
  drift apart on error payloads undetected — the drift class P3-5 was
  remediated to prevent.
- **Impact:** latent cross-driver divergence risk on error rows; also the
  omission of `target_kind` means the "complete current response shape"
  principle stops one level above the error object.
- **Recommended correction:** add one error-bearing repository row (and
  optionally one envelope error) to the canonical fixture, and decide
  deliberately whether both drivers should emit `target_kind` in merge error
  objects (if yes, add it to both simultaneously).
- **Regression test:** the extended fixture itself, compared by the existing
  Rust and Python fixture tests.

---

## 2. Disposition of original findings

| Original finding | Plan disposition | Verified outcome |
| --- | --- | --- |
| **P2-1** memberless preflight errors | Fix | **Fixed** at the contract layer. All six fallible probes in `preflight_member` wrap failures with `with_member` ([plan.rs:176–236](gwz-core/src/workspace_ops/merge/plan.rs:176)); the typed backend `ErrorCode` is preserved; `ModelError → GwzError` maps `member_id`, `member_path`, and `target_kind: Member` ([convert.rs:174](gwz-core/src/protocol/convert.rs:174)); tests cover second-member missing source, second-member foreign integration state, and a six-point failure matrix. No double-prefixing (`with_member` has exactly two production call sites, one per error path); `member_error` was rebased onto the same builder so structured and text context cannot diverge; workspace-level errors correctly carry no member context. Residual: the structured fields are dropped at both CLIs' top-level error envelopes — new finding P3-1. |
| **P3-2** lock advancement after halt | Keep and specify | **Intentionally retained, documented, and tested.** Design §4 now states the halt rule explicitly; `merge.md` and `Releases.md` repeat it; the real-Git halt test asserts the lock advances for the verified earlier outcome only. |
| **P3-3** M0 commit-message divergence | Specify phase boundary | **Intentionally retained and documented.** Design §10, `merge.md`, and `Releases.md` all state M0 keeps `Merge <source> into <target-branch>` and that quoting plus `GWZ-Merge-ID`/`GWZ-Operation-ID` trailers begin with the durable M1 record. |
| **P3-4** no real-repository halt test | Fix | **Fixed.** `real_git_drift_halts_and_advances_only_verified_earlier_lock_outcomes` ([start.rs:607](gwz-core/src/workspace_ops/merge/start.rs:607)) plans three real repos normally, injects a real branch move on the second member between frozen planning and execution inspection via a `#[cfg(test)]`-only `ExecutionBackend` wrapper delegating to the real `Git2Backend`, and asserts stop order (`["app","lib"]` inspected, tool untouched), `FastForwarded`/`Failed`/`Unattempted` rows, `Halted`/`Failed` aggregate, `live_commit` unknown for failed/unattempted, structured `MergeDrift` error with `member_id`, real Git end-state, and lock advancement for the first member only. The injection seam is test-only; production mutation checks are unchanged. (Note: it drives `execute_plan` + `advance_m0_lock` — the same composition `handle_start` wires — rather than `handle_start` itself, which cannot accept the wrapper; acceptable.) |
| **P3-5** one-sided parity fixture | Fix | **Fixed.** One canonical fixture at `gwz-core/protocol/fixtures/cli_parity/merge_response.json`; the gwz-py copy is deleted; Rust ([g02.rs:76–100](gwz-cli/src/tests/g02.rs:76)) and Python (`test_cli_merge.py`) construct the same synthetic response and compare semantic JSON against that one file, covering `response_json`, `--json`, and `--jsonl`. The sibling-checkout path is exactly the constraint the existing `path = "../gwz-core"` Cargo dependencies already impose, so no new layout requirement; a missing sibling fails loudly. Documented in `MachineOutput.md`. |
| **P3-6** omitted future protocol fields | Fix now | **Fixed.** Both serializers emit `continued`/`aborted`/`rolled_back` counts, per-repo `drift` (full eight-field shape), `operation_drift`, `preservation` (null vs array preserved), and `publication_step`, consuming real response values — no hardcoded empties. The fixture carries non-empty drift, preservation, publication step, and non-zero reserved counts, so silently dropping M1 data now fails tests. Design §16, `MachineOutput.md`, and `Releases.md` document the complete-shape and additive-keys contract. Gap: error sub-shape unpinned — finding P3-2. |
| **P3-7** unrelated-history behavior | Keep and specify | **Intentionally retained, pinned, and documented.** Three real-repository tests: the checked primitive ([g12.rs:180](gwz-core/src/git/tests/g12.rs:180)), first-class `handle_merge` ([g23.rs:244](gwz-core/src/workspace_ops/tests/g23.rs:244)), and `pull --sync merge` ([g01.rs:476](gwz-core/src/workspace_ops/tests/g01.rs:476)) — each asserting target ref, HEAD, index bytes, worktree bytes, status, native merge state, `MERGE_HEAD` absence, and (where applicable) lock bytes unchanged. `classify_merge` is the single classification authority for analysis and checked execution; no allow-unrelated flag or bypass exists. Documented in `merge.md`, `Releases.md`, and the design M0 phase list. |
| **P3-8** late identity rejection | Fix | **Fixed.** `validate_git_identity_field` ([model/mod.rs:496](gwz-core/src/model/mod.rs:496)) rejects angle brackets, NUL/control characters, and values that trim to empty under libgit2's exact edge-crud set (verified against libgit2's `is_crud` semantics and empirically — see §4). Validation runs in `OperationRequest::context` before workspace locking, planning, or mutation; proven by the new `operation_runtime` test (author and committer independently) and by the mixed FF/true-merge g23 test, which asserts the manifest-earlier fast-forward member's HEAD, the divergent member, and the lock are all unchanged after rejection. Empirically there are no false negatives vs git2 0.21.0 / libgit2 1.9.4; the validation is deliberately stricter on embedded newline/CR — which libgit2 *accepts* but which would inject unescaped bytes into commit headers — so the extra strictness closes a real corruption vector (see §4 for the acceptance table). |
| **P3-9** global-option diagnostics | Fix diagnostics/docs | **Fixed.** Five field-specific rejections name both the protocol field and the CLI option (`policy.sync (--sync)` etc., [validate.rs:65–75](gwz-core/src/workspace_ops/merge/validate.rs:65)) with the `MergeValidationFailed` code preserved and a table test asserting both names appear. `merge.md` explains why generated global options appear yet are rejected; generated `CLI.md` untouched by hand (reference check clean). |
| **P3-10** module-wide dead-code allow | Narrow | **Fixed.** The module-wide `#![allow(dead_code)]` is gone. Remaining allowances: per-item allows with phase-removal comments on the M1 seams in `mod.rs` and `validate.rs`, and file-scoped allows on the wholly-frozen `model.rs`/`response.rs` seam files, each with a phase comment. Live M0 code (`plan.rs`, `start.rs`, `validate.rs` bodies) has no blanket exemption; clippy passes with `-D warnings`. |

All ten dispositions in `GwzMergeCodeRemPlan-1.md` §2 are confirmed as
executed. The §3 contract decisions are recorded in `GwzMergeDesign.md`
(halt rule §4, preflight context and identity validation §7, message §10,
machine-output completeness §16, unrelated histories §19-M0) and mirrored in
`merge.md`, `MachineOutput.md`, and `Releases.md` — area G is fully
consistent across all four documents.

## 3. Verification commands run and results

All from `/Users/owebeeone/limbo/gwz-dev` (macOS arm64, Darwin 25.5.0), against
the exact remediation heads (workspace lock records `7321362`/`7a90a9a`/
`7cde9c0`, matching the requested commits):

| Command | Result |
| --- | --- |
| `cargo fmt --all -- --check` | pass (exit 0) |
| `cargo test -p gwz-core -p gwz` | pass — **567 passed, 0 failed** across 15 suites (exit 0, captured directly). The known rename-test temp-directory collision did **not** occur in this run; no rerun with `--test-threads=1` was needed. |
| `cargo clippy --workspace --all-targets -- -D warnings` | pass (exit 0) |
| `gwz-core: protocol/regen.py --check` | pass — committed protocol artifacts current |
| `gwz-cli: generate_cli_reference.py --check` | pass (exit 0) |
| `uvx … mkdocs build --strict` | pass (exit 0; the red banner in the output is mkdocs-material's 2.0 upgrade notice, not an error) |
| `gwz-py: maturin develop` | pass — wheel built and installed |
| `gwz-py: run_tests.py` | pass — protocol regen check OK, **261 pytest tests passed** |
| `bazel build //gwz-core/... //gwz-cli/...` | pass — 3 targets built successfully (build-only; the repo has no Bazel test targets, per the plan's own note) |
| `git diff --check` | clean (exit 0) |

Additional independent verification performed:

- **libgit2 acceptance probe** (scratchpad-only, no repo changes): a throwaway
  crate against the workspace's exact `git2 0.21.0` / `libgit2-sys
  0.18.5+1.9.4` exercised `Signature::now` on 13 boundary values. Results in
  §4.
- Full diff read of every changed file in all four ranges; call-site greps for
  `with_member` (no double-application paths), dead-code allowances (eight,
  all seam-scoped with phase comments), and unrelated-history bypasses
  (none).

## 4. Identity validation vs libgit2 (empirical)

`git2::Signature::now` acceptance on git2 0.21.0 / libgit2 1.9.4 versus
`GitObjectIdentity::validate`:

| Value | libgit2 | GWZ validate | Note |
| --- | --- | --- | --- |
| plain name/email | accept | accept | agree |
| angle bracket in name or email | reject | reject | agree |
| embedded NUL | reject | reject | agree |
| crud-only name (`,;:"\'`) | reject (empty after trim) | reject | agree — edge-trim set matches libgit2's `is_crud` exactly |
| edge crud (`  'Alice'  `) | accept (trims) | accept | agree |
| comma/space mid-value | accept | accept | agree |
| embedded newline / CR | **accept** | reject | GWZ stricter — libgit2 would write the raw byte into the commit header; rejecting prevents malformed objects |
| embedded tab / ESC / C1 control | accept | reject | GWZ stricter — benign over-restriction for pathological identities |

Conclusion: **no false negatives** (nothing libgit2 rejects is accepted, so a
mid-batch identity halt from the libgit2-rejection set is impossible), and the
false-positive class is limited to embedded control characters, where
rejection is either safety-critical (newline/CR) or harmless (tab, ESC, C1).
The design's phrase "validated for libgit2 representability" slightly
understates this — the check is representability **plus** header safety —
which is worth a one-line clarification at the next design edit, but is not a
defect.

## 5. Residual risks and missing coverage

- **CLI top-level error envelopes** carry member context in message text only
  (finding P3-1). Participant-level errors are fully structured end-to-end.
- **Error sub-shape not fixture-pinned**, and `target_kind` is consistently
  omitted from both drivers' merge error JSON (finding P3-2).
- **Halt test composition:** the real-Git halt test replicates
  `handle_start`'s execute-then-advance wiring rather than calling
  `handle_start` (the test-only injection seam is the internal
  `ExecutionBackend` trait). The wiring itself is covered by the other g23
  end-to-end tests plus the fake-backend halt test; the risk of the two
  drifting is low but nonzero.
- **Canonical fixture location** requires the sibling `gwz-core` checkout —
  identical to the constraint the Cargo `path` dependencies already impose on
  every build; violation fails loudly. No standalone-clone layout regresses
  silently.
- **Platform coverage:** this run verified macOS arm64 only; Linux/Windows
  behavior (ref transactions, path handling) remains unverified here.
- **Bazel:** build targets only; no Bazel test targets exist, so `bazel build`
  success is not a test gate (correctly acknowledged in the plan).
- Inherent M0-scope items from the original review remain as documented
  behavior, now with markedly better documentation: advisory dry-run,
  failure-window worktree residue, `open: true` without a durable record, and
  partial lock advancement (now release-noted with its M1 transition).

## 6. Verdict

**Remediation complete with non-blocking follow-up.**

All ten original findings are fixed or intentionally-retained-and-specified
exactly as the remediation plan committed, every §7 exit criterion is met in
substance, and the full verification gate passes cleanly (567 Rust + 261
Python tests, clippy, fmt, regen, reference, mkdocs strict, bazel build, git
diff --check). The two P3 findings above — structured member fields on the
CLIs' top-level error envelopes, and an error-bearing row in the canonical
parity fixture — are small, well-bounded, and suitable for a follow-up or the
M1 wave; neither blocks M0 nor undermines the remediation's substance.
