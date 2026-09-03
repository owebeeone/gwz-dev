# R4b Linux durable-identity execution-evidence re-review

Date: 2026-08-12

Reviewed independently on the settled workspace at:

- root commit `fed4c6009c7c1386809eb35ef01eff6a289176ef`;
- `gwz-core` commit `11d0817657939cdc995a3696b6606b7f3e7e6614`;
- `GwzM5-8R4bLinuxCapabilitySpike.md` and the prior
  `GwzM5-8R4bLinuxIdentityEvidence-Review.md`;
- every file under `gwz-core/scripts/linux_identity_probe/` and
  `gwz-core/.github/workflows/linux-identity-probe.yml`;
- GitHub Actions run
  [`31537368340`](https://github.com/owebeeone/gwz-core/actions/runs/31537368340),
  attempt 1, including job metadata and logs; and
- freshly downloaded raw and aggregate artifacts from that run, compared with
  the retained `GwzM5-8R4bLinuxIdentityEvidence.json`.

The workspace and core checkout were clean at review start. The reviewed
tooling and workflow are byte-identical to the stated core commit. This review
did not modify production or test code.

## Verdict

**GO: accept R0-L and begin R1.**

The prior P1-1 closed-schema/binding/integrity defect and P2-1 typed-negative-
evidence defect are closed. I found no new P0, P1, or P2 defect in the R0-L
execution gate or retained evidence.

One non-blocking P3 strict-JSON-type hardening opportunity is recorded below.
It does not allow a false required fact to pass, and the retained artifact uses
the correct JSON types throughout.

## Prior finding closure

### P1-1: closed schema, workflow binding, and aggregate integrity -- closed

The aggregate validator now requires exactly two rows with the exact
`linux-aarch64` and `linux-x86_64` architecture set. Each row has an exact
top-level key set and exact nested key sets for the durable tuple,
substitution, query contract, remount result, and diagnostics. The exact
15-row negative table is also required. Missing keys, unknown keys, false
proof values, malformed tuple values, invalid diagnostics, and altered typed
negative results are rejected.

The rows are bound to the aggregate job rather than merely to one another:

- the workflow supplies `GITHUB_SHA` and `GITHUB_RUN_ID` as the aggregate
  validator's expected commit and run;
- each native row must equal those expected values;
- the aggregate checkout independently recomputes the probe-source digest;
- the validator independently recomputes the provider-table digest; and
- each declared architecture must have its exact reviewed
  `platform.machine()` value (`aarch64` or `x86_64`). The producer checks this
  before probing and the aggregate checks it again.

I replayed the reviewed aggregate CLI over the freshly downloaded raw rows.
It produced byte-for-byte the retained artifact with SHA-256
`5ebddb1141576816609c2fe1e8e65b163d75ba9a3e4001852e0a6ed7baff313c`.

I also independently mutation-tested the downloaded rows. Eighty-three
deletions and corruptions covering every row field, every nested field,
unknown nested fields, all former false-proof paths, tuple validity, typed
negative outcomes, architecture/machine identity, and commit/run/source/table
bindings were rejected. In particular, the exact former escape route -- a
missing tuple plus false substitution, query, and mount-authority facts -- no
longer aggregates.

Changing only a remount mount ID, with the matching diagnostic updated, still
aggregates. That is the intended result: the mount ID is diagnostic and
non-authoritative, while UUID, handle type/bytes, and path modes form the
durable comparison.

### P2-1: complete typed query/error negatives -- closed

The expected table now contains the exact 15 named outcomes. The producer
derives them by executing the reviewed provider validators and error mapper,
then requires exact table equality before returning evidence. This includes:

- a nonzero malformed 15-byte UUID result of `UnsupportedOperation`;
- missing `AT_EMPTY_PATH`, `AT_SYMLINK_FOLLOW`, `AT_HANDLE_FID`, and pathname
  fallback rejection through `validate_handle_query`;
- injected `EACCES` mapping to `IoError` through the same
  `handle_query_error` path used by the real retained-handle call; and
- injected `EOPNOTSUPP` mapping to `UnsupportedOperation` through that same
  path.

The native probe separately executes the raw missing-`AT_EMPTY_PATH` syscall
and requires `ENOENT`. Real tmpfs and overlay mounts are observed through
`fstatfs` and rejected by the ext4-only provider. These are not literal success
booleans: the query-contract object is derived from the executed rows and must
equal the closed expected contract.

Both downloaded architecture rows contain all 15 exact outcomes, including
`permission_denial: IoError`,
`unsupported_empty_path: UnsupportedOperation`, and
`malformed_uuid_length: UnsupportedOperation`. Their query-contract fields
agree with those executed outcomes.

## Execution and provenance confirmed

| Gate item | Independent result |
| --- | --- |
| GitHub run | **Confirmed.** Run `31537368340`, attempt 1, push to `main`, concluded success at exact head `11d0817657939cdc995a3696b6606b7f3e7e6614`. |
| Required jobs | **Confirmed.** Contract, native AArch64, native x86-64, and aggregate jobs all succeeded. The aggregate depended on both native jobs. |
| Exact native architectures | **Confirmed.** GitHub job labels were `ubuntu-24.04-arm` and `ubuntu-24.04`; the Python toolcache paths were arm64 and x64; the emitted and aggregate-validated native machines were `aarch64` and `x86_64`. |
| Exact checkouts | **Confirmed.** All four job logs show checkout of `11d0817657939cdc995a3696b6606b7f3e7e6614`. |
| Fail-not-skip native execution | **Confirmed.** Tool installation, ext4 creation, loop mount, fixture/casefold setup, descriptor query/substitution, checked unmount/remount, real tmpfs/overlay mounts, and output generation are on the successful path. Required subprocesses use `check=True`; artifact upload follows probe success. |
| ext4 durable tuple | **Confirmed.** Both rows record external UUID `718c918e3cc343c9ae9e27f5cecc8a17`, positive type-1 eight-byte handles, and complete sensitive/casefold modes. The probe requires those facts to compare across unmount/remount. |
| Retained descriptor substitution | **Confirmed.** The real native path retains object A's descriptor, renames A away, replaces its pathname with B, re-queries A through empty-path `name_to_handle_at`, and requires A unchanged and B different. Both rows contain the validated proof. |
| Casefold evidence | **Confirmed.** The image is created with ext4 casefold support, `chattr +F` must succeed, and `FS_IOC_GETFLAGS` must report the expected sensitive and `AsciiCaseFold` modes before and after remount. |
| Mount-ID authority | **Confirmed.** Mount ID is absent from the normalized tuple and comparison fields. Both real runs happened to reuse it (`263` and `59` respectively), but the contract test and my aggregate mutation both accept a changed diagnostic mount ID. |
| Contract tests | **Confirmed.** The CI contract job passed all 10 tests. The same 10 tests passed locally from the settled core checkout. |
| Probe-source digest | **Confirmed independently.** Recomputed from the five declared `.py`/`.md` sources as `fbf2de6426dc728af8db8c6444eeeff7c1bbc4bde61b0ee4d576a43492a2311d`, matching both rows and the aggregate. |
| Provider-table digest | **Confirmed independently.** Recomputed as `5eb3679744e641b4f718cb86b7ea92f71238ed4a69802d1f058d7496d8017aaa`, matching both rows and the aggregate. |

## Artifact integrity

Fresh GitHub downloads produced:

| Artifact | GitHub ID | Downloaded payload SHA-256 |
| --- | ---: | --- |
| `linux-identity-linux-x86_64` | `9119295613` | `ed7dbc486e4f73fa4569d0f365ebbea840b9866aa1838f740e6569bac6d274bc` |
| `linux-identity-linux-aarch64` | `9119297526` | `ac180214f589a628f571a3afecba2c0ee59b4afaa35d34b5527a0caca38298f7` |
| `linux-durable-identity-evidence` | `9119301458` | `5ebddb1141576816609c2fe1e8e65b163d75ba9a3e4001852e0a6ed7baff313c` |

The downloaded zip SHA-256 values were respectively
`3674b3e0a57b0637e72b3137abb92f6acdfc4278cd051e02932d08c71e0590fb`,
`5b4bf6ffd7e8256564a0828143b9aaa83104ce467e131fb122659849aae8ca96`,
and `01e3d39d1a11f39a0e875174b53e423c2a38b1ca3d4494426bc5d819ed833e47`.
Each matches the corresponding upload digest in the job log. The downloaded
aggregate payload and retained canonical file are byte-identical.

## Non-blocking hardening note

### [P3-1] Python equality admits numeric aliases for a few JSON scalar types

`schema_version != 1` accepts JSON `true` because Python considers
`True == 1`. Equality against dictionaries containing expected `true` values
similarly accepts numeric `1` or `1.0` for the two substitution and two query
booleans. I reproduced those five type-alias acceptances.

This does not reopen P1-1's false-proof path: `false` and zero are rejected,
all required keys and facts are still enforced, the trusted producer emits
real JSON booleans and an integer schema version, and the retained artifact
passes an independent strict-type check. It is therefore not an R0-L blocker.

For stricter representation closure, use exact-type checks (`type(value) is
int` for the schema version and `value is True` for proof booleans) and add the
five numeric-alias mutations to the contract test.

## Exit decision

Run `31537368340` and retained artifact SHA-256
`5ebddb1141576816609c2fe1e8e65b163d75ba9a3e4001852e0a6ed7baff313c`
satisfy the R0-L execution-evidence gate. Mark R0-L complete and proceed to R1.
