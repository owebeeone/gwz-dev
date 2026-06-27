# GWZ diff plan independent review 55-5

## Verdict

Review48-4 was substantially incorporated. The two-phase request/response
design is now coherent, and the prior rename/copy, root filtering, stale-entry,
`file_id`, `--quiet`, and batch partial-result concerns are mostly addressed.

Remaining risk is not the architecture; it is request/response schema
correctness before D0. I found two blocking protocol gaps and two smaller
contradictions.

## Findings

### [P1] Tier 0 comparison modes cannot be requested

`gwz diff --cached`, `--staged`, and `--merge-base` are Tier 0 requirements
(`GwzDiffPlan.md:1159`-`1165`) and comparison rows are specified
(`GwzDiffPlan.md:640`-`652`), but `DiffRequest` carries only `meta`,
`workspace_cwd`, raw `operands`, `explicit_pathspecs`, and `options`
(`GwzDiffPlan.md:458`-`467`). `DiffOptions` has rendering/filter options, but
no `cached`/`staged`/`merge_base` request fields (`GwzDiffPlan.md:434`-`456`).

Because the client is supposed to parse options while leaving only ambiguous
positional operands for core (`GwzDiffPlan.md:256`-`263`,
`GwzDiffPlan.md:470`-`473`), these flags cannot safely be smuggled through
`operands`. Add explicit request fields, or a structured unresolved comparison
mode, before freezing the protocol.

### [P1] Resolved per-repo targets are not scoped or reused by Phase 2

`DiffManifestResponse.targets` is described as per-repo operand classification
(`GwzDiffPlan.md:522`-`523`), but `DiffParsedTarget` contains only `comparison`
and `pathspecs` (`GwzDiffPlan.md:428`-`432`). It has no `DiffRepoScope`, so
multi-repo target classification is ambiguous unless clients infer list order.

Phase 2 also sends raw `operands` again (`GwzDiffPlan.md:525`-`541`,
`GwzDiffPlan.md:583`-`586`) instead of the resolved target for that scope. That
can reclassify differently if refs, paths, or ambiguity change between Phase 1
and Phase 2. Add `scope` to `DiffParsedTarget`, include resolved object IDs
where possible, and let `DiffFileRequest` reference or carry the scoped resolved
target.

### [P2] Batch per-file failures conflict with required `DiffFileResponse` payload fields

The plan says `diff.files` may contain per-file successes and failures because
each `DiffFileResponse` has its own `ResponseEnvelope` (`GwzDiffPlan.md:601`-
`606`). But `DiffFileResponse` requires `scope`, `format`, `encoding`, and
`data` (`GwzDiffPlan.md:543`-`554`). A failed item may not have any patch bytes
or even a resolved scope.

Make the payload fields optional on failure, or introduce a separate
`DiffFileResult`/error wrapper for batch entries.

### [P2] Copy detection is both deferred and described as Phase 1 behavior

The plan says Phase 1 runs rename/copy similarity (`GwzDiffPlan.md:569`-`570`)
and includes `find_copies`/`DiffStatus.copied` in the protocol
(`GwzDiffPlan.md:398`-`405`, `GwzDiffPlan.md:434`-`442`). Later it says copy
detection is Tier 1 and must fail clearly until proven (`GwzDiffPlan.md:1181`-
`1188`, `GwzDiffPlan.md:1291`-`1293`).

That is probably the intended policy, but the earlier Phase 1 wording still
advertises copy manifests in v0. State explicitly that `find_copies` is
rejected in v0 unless the Phase 2 copy strategy is proven.

## Residual Risks And Test Gaps

Add protocol corpus cases for `--cached`, `--staged`, `--merge-base`, and
`A...B` to prove the request schema carries the comparison mode.

Add a multi-repo manifest test where two members resolve the same operand
differently, and assert `targets` are scope-addressable.

Add a `diff.files` partial failure corpus/example where one file succeeds and
one fails without dummy patch bytes.

Overall risk is moderate until the request schema is tightened; after that, the
design is ready to implement.
