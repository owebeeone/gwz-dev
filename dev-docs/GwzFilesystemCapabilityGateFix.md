# Filesystem capability admission fix

Date: 2026-09-10. Status: Windows fix implemented and validated; Linux admission residual recorded.
See [results and audit](GwzFilesystemCapabilityGateFix-Results.md).

## Decision and defect

Filesystem support must depend on capability flags and operation probes, never
filesystem names. Names are diagnostic information only. This decision supersedes
the historical “Windows keeps `require_ntfs`” exception in
`GwzM5-8DR1-WarnOrRefuse-Charter.md` and the corresponding source comment.

In `gwz-core/src/filesystem/native/facts/windows.rs`, `facts()` calls
`require_ntfs()`, which rejects every name except `NTFS` before querying the
actual identity. The defect is established by inspection; reproducing the old
failure is not a prerequisite to fixing it.

Dabeest's E: drive already reports `FILE_SUPPORTS_OPEN_BY_FILE_ID`, returns a
nonzero 128-bit file ID and a local volume GUID, and answers the directory case
mode query. The name gate incorrectly excludes it. These probes support the
replacement; they do not alone establish the complete recovery contract.

The current consequence depends on the operation: ordinary `--no-ff` admission
warns and proceeds without crash recovery; `--filesystem-strict` refuses.
Preserve that distinction when reporting missing capabilities.

## Implementation

1. **Replace the Windows gate.** In
   `gwz-core/src/filesystem/native/facts/windows.rs`, query
   `GetVolumeInformationByHandleW` flags and check
   `FILE_SUPPORTS_OPEN_BY_FILE_ID`. Keep the existing `FileIdInfo` query,
   nonzero 128-bit identity, local volume GUID, and required case/handle checks.
   A flag alone must not bypass failed operation probes. Preserve the distinction
   between unsupported capability and an operational I/O error.
2. **Separate reporting from admission.** Keep filesystem-name retrieval only
   for diagnostics; a missing or undecodable name must not reject otherwise
   supported operations. Replace NTFS-specific failure/remedy wording in that
   file and `gwz-core/src/checked_artifact/capability.rs` with the actual missing
   capability. Do not add a ReFS allowlist or substitute filesystem magic numbers
   for names.
3. **Audit the remaining decisions.** Inspect
   `gwz-core/src/filesystem/native/facts/`,
   `gwz-core/src/checked_artifact/capability/`, and
   `gwz-core/crates/refcopy/src/native.rs`. Follow name-derived values through
   their consumers, including Linux `classify_volume()` remote/volatile flags.
   Replace any resulting support, refusal, or fallback decision with an actual
   capability check or probe. Retain display-only labels. Record exact remaining
   sites if this audit reveals work beyond the Windows fix.
4. **Preserve stored-record compatibility.** Existing serialized identifiers
   such as `WindowsNtfsFileId128V1` and `WindowsNtfs` are historical format names,
   not admission rules. Keep their wire representation in this fix; document
   their capability meaning in `checked_artifact/capability.rs` and
   `checked_artifact/capability/durable_identity.rs`. No generated protocol churn
   solely to rename these labels.
5. **Update the authority and user guidance.** Reconcile the affected sections
   of `gwz-core/dev-docs/GWZDesign.md`, `gwz-core/dev-docs/GWZRequirements.md`,
   and the old charter. Update `gwz-cli/docs/MergeRecovery.md`,
   `gwz-cli/docs/Troubleshooting.md`, and `gwz-cli/docs/LocalClones.md` where
   needed. Explain capabilities separately: successful block cloning does not
   prove recovery support. CLI and Python inherit the same core decision.

## Focused validation and completion

- Add small admission tests: identical capabilities give identical decisions
  regardless of name, including an unavailable name; a missing required flag,
  zero identity, or failed required probe still produces the appropriate error
  or warning. Use test doubles for this decision matrix.
- Build on Dabeest and run targeted native checks on C: and E: in disposable
  fixtures. Verify identity across reopen/rename, distinguish a replacement
  object, and exercise strict `--no-ff` merge plus interrupted-merge continue
  and abort. Check final repository state. Record actual results for each drive;
  process-restart checks must not be described as power-loss testing.
- Run the relevant capability/recovery tests and platform compilation checks.
  No compiler-mutation suite or full release is required for this fix.
- Finish when the Windows replacement passes these checks, the admission audit
  records its findings, and no active instruction still requires a name gate.
  Report any remaining admission violations explicitly rather than claiming
  the whole-codebase policy is complete.

The separate CRLF `MergeDrift` defect and ReFS partial-block allocation behavior
are outside this fix. No change to block-clone arithmetic is proposed here.
