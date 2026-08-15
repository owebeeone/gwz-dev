# Review: I2 re-freeze delta (compatibility-contract supersession banner + docs-gate pinning)

**Reviewer axis:** consistency / citation-fidelity (single-axis, read-only)
**Review object:** uncommitted working-tree delta, exactly three files, reviewed at their current uncommitted state on **2026-08-15**:

| File | Blob transition | Shape |
|---|---|---|
| `dev-docs/GwzM5-8I2CompatibilityContract.md` | 262e969 → 01c3a58 | +8/−0, single hunk after Status line (banner-only, confirmed via `git diff --numstat`) |
| `gwz-core/scripts/checks/merge_docs_manifest.json` | 4393cb7 → 0f95f88 | +15/−0, one new source `i2_compatibility_contract` |
| `gwz-core/scripts/checks/test_check_merge_docs.py` | 0e8eda0 → 4d4ddbe | +1/−1, `source_count` 10→11 only |

Surrounding working tree contains unrelated in-flight packages (windows-matrix workflow, `checked_artifact` test edits, untracked Windows-matrix/escape/panic docs). None of them is a docs-gate source; review confined to the three files plus read-only verification sources as directed.

## (a) Banner records accepted reality — VERIFIED

- **Amendment status/date:** `GwzM5-8R4bInterfaceAmendment-1.md` line 3 `Date: 2026-08-10`, lines 5-7 Status **accepted** after both re-reviews GO, no open P0-P3. Banner's "Amended 2026-08-10" matches the amendment's own date; sibling banners (record/journal, "Amended 2026-08-11") use the same convention — date of the effecting amendment, not of the edit.
- **§4 text:** §4 opens "The normal-build boundary is split by capability" (line 372) — banner phrase is near-verbatim. §4.1 (375-383): neutral archive projection normal-build. §4.2 (385-411): `decode_archived_v0` normal-build, `decode_archived_for_r3_tests` `#[cfg(test)]`, "P3 ordinary archived status consumes only `decode_archived_v0`". §4.3 (413-472): `record_wire/location.rs` sole normal-build acquisition owner, `acquire_canonical_merge_locations`, arbitration table, optimistic reread. §8 item 5 (765-766) is exactly the banner's normal-build acquisition/accept-v0/reject-v1 gate, and §8 closes satisfied (779-780).
- **Code sites (normal-build, not cfg(test)):**
  - `gwz-core/src/workspace_ops/merge/gc.rs:162` `handle_gc` (ungated) → `acquire_canonical_merge_locations` at :183, `decode_archived_v0` at **:194**, archived projection attached at :195-199. `mod gc;` ungated (`merge/mod.rs:7`); dispatched from the production `MergeOp::Gc` arm at `runtime/dispatch.rs:191` (the only `cfg(test)` in dispatch.rs, line 73, gates a separate test seam).
  - `gwz-core/src/workspace_ops/merge/status/snapshot.rs:116` `acquire_status_v0` → `acquire_canonical_merge_locations` at :117, `decode_archived_v0` at **:127**; zero `cfg(test)` in the file; `mod snapshot;` ungated (`status/mod.rs:5`) and exports the production `handle_status` (`status/mod.rs:25`). The optimistic reread (snapshot.rs:93-113) implements §4.3's lineage recheck.
  - The split is real in code: `#[cfg(test)] mod v1_lifecycle;` at `merge/mod.rs:23-24`, while `gc`/`status`/`record_wire` are ungated.
- **"consumed by ordinary status and GC":** status is verbatim §4.2; GC is grounded compositely — §7.1's exact **production** manifest lists `gwz-core/src/workspace_ops/merge/gc.rs` (amendment line 619) and `store/gc.rs` (620), §6 requires GC preflight/restart/final-exhaustion "from the unchanged archive", and §4.3's sole-owner rule forces normal-build GC through canonical acquisition — and it is code-true at the cited lines.
- **"migration remain test-gated until A1":** true in the amendment (preamble lines 9-12: does not enable production migration) and in code — `prepare_upgrade` is re-exported only under `#[cfg(test)]` (`record_wire/mod.rs:23-27`, `merge/mod.rs:44-48`) and its sole caller is in the `#[cfg(test)] mod tests` tree (`workspace_ops/mod.rs:27-28`), matching contract §9's compile/call-graph boundary.

## (b) No overreach; §-references accurate — VERIFIED

The banner adds no normative content beyond restating the accepted §4 split. Authority stays with documents: "boundary-authoritative only as amended by that accepted interface" mirrors the 06689bb sibling phrasing ("wire-authoritative only as amended by those accepted interfaces") and does not make code authoritative. The single internal reference — "§9 below describes the superseded test-only form" — is accurate: §9 ("Disabled implementation and activation", line 275) is precisely the section that froze decoders/projection/migration behind test-only entry points, and it sits below the banner. Citing only Amendment-1 is correct: Amendment-2 (`GwzM5-8R4bInterfaceAmendment-2.md`) is additive, root-preservation-only (compound `NormalizeRoot`/`RestoreRoot` phases), with no normal-build/archived-boundary content — no omitted amending authority.

## (c) Manifest pinning and pattern — VERIFIED (with one nuance, P3-2)

- The checker whitespace-normalizes literal matches (`check_merge_docs.py:68-69, 96-100`), so the wrapped banner matches the single-line manifest values; both pinned phrases ("normal-build code", "boundary-authoritative") occur **only** in the banner (doc lines 9, 12).
- **Demonstrated, not assumed** (via the checker's own `source_overrides` API against scratchpad copies; no repo files touched): banner removed → findings `[i2_compat_a1_production_archive_split, i2_compat_amended_boundary_authority]`; `root_publication_prefix` reintroduced into the doc → finding `[retired_root_prefix_field]`. The retired-spelling protection comes from the manifest's 7 `global_forbidden` assertions (snake-case `normalize_root`/`restore_root`/`root_publication_prefix` + four no-ff regexes), which apply to this doc **only because this delta makes it a source** — confirmed by the assertion delta 138→147 = +2 required +7 global.
- **Pattern vs ca520e4:** ca520e4 ("Add I2 retired-spelling doc tripwires") added per-source `forbidden` tripwires to the two pre-existing I2 wire-doc sources (journal: `NormalizeRoot`, `RestoreRoot`, `root_publication_prefix`; record: `adds four top-level fields`, `root_publication_prefix`). The new source follows the siblings' **required** pattern exactly — leading banner-authority sentence pin plus current-truth sentence pin, literal match, `i2_*` id naming, placed with the I2 group after `i2_record_contract`.

## (d) Gates — GREEN (direct exit codes, unpiped)

- `check_merge_docs.py` → `merge document consistency: ok (11 sources, 147 assertions)`, `exit=0`.
- `test_check_merge_docs.py` → 3 tests OK, `exit=0` (includes the fixture-override test proving the real gate fires on tampered content).
- Baseline against the HEAD manifest → `ok (10 sources, 138 assertions)`, `exit=0` — confirms the 10→11 claim and that all +9 assertions belong to the new source.

## (e) Other executor verdicts — CONFIRMED by spot-check

- **Protocol contract not stale:** top allocation row `| 61 | terminal_rollback_mismatch / TerminalRollbackMismatch |` (`GwzM5-8I2ProtocolContract.md:31`) matches `gwz-core/src/protocol/generated.rs` — enum variant :1403, `wire()` `Self::TerminalRollbackMismatch => 61` :1468, `from_wire` :1532 — plus the model-side enum `src/model/mod.rs:75` and projection `src/protocol/convert.rs:72`.
- **Record/Journal re-frozen at outer 06689bb (2026-08-11):** that commit adds both "Amended 2026-08-11 by Amendment-1 and Amendment-2 … wire-authoritative only as amended" banners and the content re-freeze (four→five top-level fields; `root_publication_prefix` → `root_publication_handoff`).
- **Tripwires at ca520e4 fire:** by checker inspection (forbidden match → finding → exit 1), by grep both docs are currently clean of all tripwire spellings, and my override demonstration actually fired `retired_root_prefix_field`. No files edited.

## Findings

- **[P3-1]** `dev-docs/GwzM5-8I2CompatibilityContract.md:7-13` — the banner attributes its whole sentence to "Amendment-1 §4", but two elements are amendment-wide rather than §4-literal: "migration remain test-gated until A1" (from the amendment preamble lines 9-12 and the contract's own §5/§9) and the GC half of "consumed by ordinary status and GC" (grounded in §6 + §7.1's production manifest + §4.3's sole-owner rule, not a single §4 sentence). Both statements are true, accepted, and code-verified; direction is conservative. Informational; no change requested.
- **[P3-2]** `gwz-core/scripts/checks/merge_docs_manifest.json:205-218` — new source carries `forbidden: []`, unlike the ca520e4-tripwired siblings; retired-spelling protection for this doc rides on `global_forbidden` (snake-case spellings and no-ff regexes; demonstrated firing), while the PascalCase journal spellings are not checked against this doc. Justified: §9's superseded test-only text is intentionally retained, so a per-source tripwire on the old boundary phrasing would fire on §9 itself. No action needed.

## Verdict

**GO** for landing this delta on the docs train. All three files match their described shape exactly; the banner faithfully records the accepted Amendment-1 §4 supersession without exceeding it; the gate pins the banner with assertions demonstrated to fail on removal and on retired-spelling reintroduction; both named gates pass with direct exit code 0 (11 sources, 147 assertions); and the executor's collateral I2 verdicts (protocol contract fresh, record/journal re-frozen at 06689bb, ca520e4 tripwires live) all check out.

*(Reviewed at uncommitted working-tree state: contract 262e969→01c3a58, manifest 4393cb7→0f95f88, test 0e8eda0→4d4ddbe; 2026-08-15.)*
