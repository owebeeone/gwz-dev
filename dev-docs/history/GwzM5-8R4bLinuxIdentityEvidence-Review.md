# R4b Linux durable-identity execution-evidence review

Date: 2026-08-12

Reviewed independently:

- `GwzM5-8R4bLinuxCapabilitySpike.md` at SHA-256
  `b809ebff99e1298cedf26e1a3f86d259ce58e47f0960ed5509fd4b3701d63e4b`;
- retained `GwzM5-8R4bLinuxIdentityEvidence-Run31536272593.json` at SHA-256
  `46357d09608205eac0b0868181d0bc9c6d643c2e5dbd0d7c8fedd51648184b57`;
- every source under `gwz-core/scripts/linux_identity_probe/` and
  `.github/workflows/linux-identity-probe.yml` at core commit
  `808e64a21a36d5e0bbe5b4400cd5975eef5cff97`;
- GitHub Actions run
  [`31536272593`](https://github.com/owebeeone/gwz-core/actions/runs/31536272593),
  attempt 1, including its job metadata and logs; and
- freshly downloaded aggregate and per-architecture artifacts from that run.

The local core checkout is exactly the reviewed commit, and every reviewed
tool/workflow file is byte-identical to that commit. This was a read-only
evidence review; I did not modify production or test code.

## Verdict

**NO-GO for accepting R0-L as complete.** There is no P0 finding. One P1
aggregate-gate integrity defect and one P2 missing-negative-evidence defect
remain.

The executed positive evidence is credible. The run used distinct hosted
`ubuntu-24.04` and `ubuntu-24.04-arm` jobs, both checked out the exact core
commit, created a fixed-UUID ext4 image, mounted it, established sensitive and
casefold directories, obtained an empty-path retained-descriptor handle,
unmounted and remounted the image, and reacquired the durable tuple. The real
overlay and tmpfs rows also executed on both architectures. All four jobs and
their required steps completed successfully.

The downloaded aggregate is byte-identical to the retained JSON. Replaying the
reviewed aggregator over the freshly downloaded raw architecture files produces
the same bytes and digest. The source and provider-table digests independently
recompute to the values in both rows.

Those facts do not close the gate because the aggregate validator can publish a
successful artifact after deleting the durable tuple and falsifying the query,
substitution, and mount-authority evidence. In addition, three negative forms
explicitly required by the spike were not executed or recorded.

## Findings

### [P1-1] The aggregate job can pass with the durable evidence missing or false

**Where:** `scripts/linux_identity_probe/provider.py`,
`aggregate_evidence`; `aggregate.py`; workflow aggregate job; contract tests.

`aggregate_evidence` currently checks only:

- set equality and row count for the two declared architecture strings;
- equality of five shared fields across the rows;
- schema version and the provider-table digest;
- equality of the eight support-table negative rows; and
- `remount.identity_equal == true`.

It does **not** require or validate:

- the normalized durable tuple, including UUID, handle type/length/bytes, and
  sensitive/casefold facts;
- `substitution.retained_handle_unchanged` or
  `replacement_handle_different`;
- the missing-`AT_EMPTY_PATH`, forbidden-flags, or pathname-fallback contract;
- `remount.mount_id_is_non_authoritative`;
- a nonempty kernel/native-machine fact;
- that a declared architecture matches `platform.machine()` on the native
  runner;
- that `probe_source_sha256` equals a digest recomputed by the aggregate
  checkout; or
- that the shared core commit and workflow-run strings equal the aggregate
  job's actual `GITHUB_SHA` and `GITHUB_RUN_ID`.

**Demonstrated false-positive path.** I loaded the two downloaded raw rows,
removed `tuple` from both, set both substitution booleans false, changed the
query contract to success/false/false, and set
`mount_id_is_non_authoritative` false. The unmodified reviewed
`aggregate_evidence` accepted the rows and returned a publishable two-
architecture aggregate containing those false facts.

The native producer in this exact run did perform the positive checks, so this
finding does not claim that the downloaded values are fabricated. It shows
that workflow success—the declared executable gate—does not enforce the
evidence it claims to gate. A regression or malformed producer row can obtain a
green aggregate and canonical artifact.

**Required correction.** Make aggregation validate the complete closed
evidence schema before preserving a row:

1. validate every normalized tuple field and exact query/substitution/remount
   result;
2. record `platform.machine()` in each native row and require the frozen mapping
   to `linux-x86_64` or `linux-aarch64`;
3. recompute the probe-source and provider-table digests in the aggregate
   checkout;
4. pass and compare the expected `GITHUB_SHA` and `GITHUB_RUN_ID` rather than
   accepting any equal pair of strings;
5. reject missing and unknown fields where the canonical schema promises exact
   evidence; and
6. add mutation tests proving that deletion or falsification of every required
   field fails aggregation.

Rerun both native jobs and retain the newly aggregated artifact after this
validator is installed.

### [P2-1] The artifact omits required query/error negative rows

**Where:** `GwzM5-8R4bLinuxCapabilitySpike.md`, final paragraph of “Executed
evidence”; `run_probe.py`; `provider.py`; `test_probe_contract.py`; aggregate
JSON.

The spike requires negative coverage for:

- missing `AT_EMPTY_PATH`;
- permission denial;
- unsupported empty-path lookup;
- `AT_SYMLINK_FOLLOW`;
- `AT_HANDLE_FID`;
- non-16-byte UUID length; and
- pathname fallback.

The run executes missing `AT_EMPTY_PATH` as a real syscall and observes
`ENOENT`. The portable contract executes the pre-syscall rejections for a
nonempty path, `AT_SYMLINK_FOLLOW`, and `AT_HANDLE_FID`. The native evidence
then writes the corresponding forbidden-flag and pathname booleans as literal
`true` values.

There is no executed or recorded permission-denial row, no injected/native
unsupported-empty-path outcome, and no explicit malformed nonzero UUID length.
The existing `no_uuid` row uses length zero and empty bytes; it proves absence,
not the separately named malformed-length case. `retained_handle` also exposes
raw `OSError` for syscall failure, so the probe has no tested typed mapping for
permission denial versus unsupported empty-path capability.

**Required correction.** Add an injected syscall-result seam, while retaining
the real ext4 positive call, and pin the intended typed outcomes for permission
denial and unsupported empty-path lookup. Add at least one explicit 15- or
17-byte UUID-length row. Compute the forbidden flag/path evidence by executing
the validator used by the provider rather than assigning booleans. Include the
new rows in the exact expected table, aggregate schema validation, and contract
mutation tests, then rerun both architectures.

If permission denial is intended to remain `IoError` under the spike's typed
failure rules, encode that exact result; do not force every negative outcome to
`UnsupportedOperation` merely to fit the existing table.

## Evidence confirmed

| Gate item | Independent result |
| --- | --- |
| Core/run binding | **Confirmed for this run.** Run head and both native jobs are commit `808e64a…`; workflow run is `31536272593`. P1-1 concerns automated enforcement. |
| Aggregate artifact | **Confirmed.** GitHub artifact ID `9118878875`; downloaded zip SHA-256 `647b8da828828a4ebe683fe9315c753203431cd09a2c6a46565e4cb45083c60c`; contained JSON and retained local JSON both have SHA-256 `46357d09…`. |
| Raw architecture artifacts | **Confirmed.** Artifact IDs `9118872931` (x86-64) and `9118873522` (AArch64); extracted JSON SHA-256 values are respectively `af2eca74000c06ed85a4a139b70aab193c80fc931a2d25473c4d2e48f0b0fa6d` and `70f9616fd044ca485e813b2fb6760339c04572a3d3cb2a65fed4f223ad1f4b8f`. |
| Exact architecture set | **Confirmed for this run from GitHub job metadata/logs.** `ubuntu-24.04` used x64 Python and `ubuntu-24.04-arm` used arm64 Python. Artifact-native binding needs P1-1 hardening. |
| Fail-not-skip execution | **Confirmed.** Contract success is a prerequisite; native commands run under `bash -e`, all mount/tool calls use `check=True`, upload follows probe success, and aggregate depends on both native jobs. |
| ext4 create/remount | **Confirmed.** Both rows use the fixed UUID on real ext4, successfully unmount/remount, and compare UUID, handle type/bytes, and path modes. |
| Retained-fd query | **Confirmed.** `name_to_handle_at(fd, "", …, AT_EMPTY_PATH)` is used on the retained no-follow descriptor; path replacement leaves A's handle unchanged and B's handle differs. |
| Casefold facts | **Confirmed.** Image is formatted with ext4 casefold support; `chattr +F` establishes the casefold parent; `FS_IOC_GETFLAGS` yields sensitive/casefold modes before and after remount. |
| Support-table negatives | **Confirmed for the existing eight rows.** Overlay and tmpfs are real mounts on both architectures; network, UUID, handle, provider, and mode failures are injected through the provider contract. P2-1 lists omitted required rows. |
| Source digest | **Confirmed.** Recomputed `04a5cc39cf6437783a350fdff19375cafe045c346ebabddd045397c01b40a9f7`; all source files match the reviewed core commit. |
| Provider-table digest | **Confirmed.** Recomputed `5eb3679744e641b4f718cb86b7ea92f71238ed4a69802d1f058d7496d8017aaa`. |
| Two-row aggregation replay | **Confirmed.** Fresh replay produces byte-identical canonical JSON with SHA-256 `46357d09…`. |
| Mount ID authority | **Confirmed in producer logic.** Mount ID is omitted by tuple normalization and remount equality, and the contract changes it without failure. Both real runs happened to reuse their mount IDs. P1-1 requires the aggregate to validate the non-authoritative declaration. |

## Exit decision

Do not mark R0-L complete and do not use run `31536272593` as the final R1
execution gate. The positive native observations may be retained as useful
evidence, but the aggregate validator must reject missing/false mandatory
fields and the omitted query/error negative rows must execute. After correcting
the tooling, rerun the exact two release architectures, retain the new canonical
artifact and digest, and repeat this focused evidence review.
