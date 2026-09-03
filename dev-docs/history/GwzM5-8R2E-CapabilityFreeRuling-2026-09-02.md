# Operator ruling, 2026-09-02 — the capability-free list stands; the remaining E4 rows carve out

Recorded verbatim by the implementation lane from the operator's message
(2026-09-02), in answer to the lane's decision packet (options A–D) after
`GwzM5-8R2E-E4.4-CharterPrep.md` §7 verified that every remaining §10 row
writes from an operation on E0.2 §5.2's frozen capability-free list. This
file is the ruling's record; the consolidated amendment quotes it.

> D), with FAT32 out of product and out of the lab.
> Closed. (A) — Ground 2 stands. Ordinary merge / commit / create / abort / GC stay capability-free. Ext4-only was for the checked feature, not "gwz dies on Fedora." Do not amend the list to put those operations on the catalog probe.
> Now. One dual-tier amendment, not four more conversion deliveries:
>
> 1. Record the tension: R2-D "production writes go through the boundary" vs R2-E §5.2 capability-free list. E4.3 was the first symptom.
> 2. Capability-free list stands. Rows `:275–:279` whose writers are on that list are carved out — raw durable writers stay, documented and pinned (generalize the E4.3-B / O13 inventory shape). Convert only arms already on `acquire_activated`.
> 3. Re-scope O1, the R2-D milestone, and E4.7: checked-feature writes go through the boundary; capability-free arms are a dated exception, not unfinished work. E4.7 does not retire those writers.
> 4. Mint or explicitly defer the tier-2 archive sub-surface (do not invent it in E4.4).
> 5. Route (C) (non-identity / degraded boundary), reader-side record reconciliation, and O14 (convert `authorize_write` vs amend freeze) to one phase-end design round. Not four builders hitting the wall at delivery. Do not start (C) inside E4.
>
> FAT32. Not a supported filesystem. No FAT32 volume drive required. Do not spend a row or a dispatch on it.
> Abort sentence. Settle from the tree: post-publication v1 abort already reaches `write_checked` → `observation.rs` identity. If that path is real, the E4.1 "`--abort` needs no such filesystem" line is over-claim. Fix the sentence (scope it to the activated-lease / capability-free abort you actually shipped, or date the residual). A Linux non-ext4 drive is optional and only if you already have one.
> Launch now (standalone, either way):
>
> * GC: `gc.rs` `decode_production_v0` on archive bytes — completed `--no-ff` since 0.11.0 is un-GC-able. Read-side, no probe. Fix it.
> * E4.3-B continues (record-root carve-out + tripwire). Unaffected.
>
> Do not start E4.4–E4.6 as originally chartered. After the amendment GO: pins package + any remaining activated-lease forward arms only. Park the E4.4 conversion candidate if it assumes archive rides the boundary from abort/GC.
> Quote this ruling in the amendment and in the E4.3-B / GC briefs. Terminal NO-GO on a different scoping comes back to me; do not pick (A) or start (C) to unblock a step.

Lane reading, for the record: (A) is CLOSED; (D) is adopted with the list
standing as written; the consolidated amendment is the next object (dual
tier); the GC decode fix and E4.3-B run standalone now; no FAT32 anywhere;
no E4.4 conversion candidate exists (only the charter-prep report), so
nothing is parked beyond that report; E4.4–E4.6 as chartered do not start.
