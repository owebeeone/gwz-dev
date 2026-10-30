# Step 1.7 driver-only construction — remediation round 3 (non-architectural, Windows)

Object: gwz-core `25941ce9` (GO on both axes: `GwzDriverOnly-Step1.7-ReviewCode-3.md`, `-ReviewState-3.md`). The
Windows receipt `p1-7-20261030-c` (WSL `p1-7-remote-20261030-c` OVERALL PASS) failed two new rows, beside the three
named Python-codec host gaps:
`driver_only_tests::rewrites::a_conditional_include_rewrite_to_a_refused_route_is_refused_by_name_for_the_clone_and_the_read`
(`clone file://`: left `GitCommandFailed`, right `UnsupportedOperation`, routes_and_binding.rs:62) and
`…_to_a_served_route_is_refused_only_when_it_is_another_kind`.

| Item | Disposition | Closure |
|---|---|---|
| includeIf rows fail on native Windows | Root-cause on dabeest first: does the `includeIf "gitdir:"` pattern match the new repository's gitdir on Windows (drive letter, case, separators, `\\?\`, trailing slash), i.e. is the rewrite never applied (fixture defect; the clone then fails at the fixture's https host, fail-closed), or is the rewrite applied and the product's created-remote observation missing it (product defect)? Fix at the cause. If the fixture: write the pattern as git expects on Windows and assert inside the row that the rewrite actually took effect (so a non-matching pattern fails loudly instead of passing elsewhere vacuously). If the product: report before fixing. | Red: the receipt rows (and a dabeest focused run). Green: focused run on dabeest; the next receipt. Both reviewers re-verify. |
