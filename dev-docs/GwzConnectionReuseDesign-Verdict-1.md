# GWZ connection reuse design — revision 1 verdict

Date: 2026-09-27. Status: **accepted as a design at SHA-256 `e8ee63f83c377866e72ff7851aabf0644cc3c5e340b82f687cf866488505624b` after [Consistency-1](GwzConnectionReuseDesign-ReviewConsistency-1.md) and [Safety-1](GwzConnectionReuseDesign-ReviewSafety-1.md) reported GO; this accepts the design text only**.
- It authorizes no implementation, activation or release.
- §16.1's fourteen decisions are the operator's. So are §13's two release-plan clauses: TR1.2 question 3 and Phase 6's swapped-agent row.
- TR1.2 closes when §13's "On GO" list is applied, the contract's status edit included.

The same two reviewers re-verdicted revision 1 with their context intact.
- Each verified the object and the five HEADs at the start and the end.
- Each confirmed that the supplied diff is exactly the change from the reviewed object.
- Neither read the other's round-2 report.
- Both ran only inspection commands and `diff`, and wrote nothing.

| Axis | Verdict | Round-1 findings | New |
| --- | --- | --- | --- |
| Consistency | GO | All 8 closed (P2-1, P3-1 to P3-7) | P3-8 |
| Safety | GO | All 5 closed (P2-1, P3-1 to P3-4) | P3-5 |

Both reviewers confirmed that every hunk of revision 1 maps to a disposition or a residual note in the [remediation plan](GwzConnectionReuseDesign-RemPlan.md), and that each of the plan's three stated choices is applied as stated. Neither classified anything as architectural, so the two-round cap is not reached. Both cleared their new finding to land without a further round.

## Corrections applied after the GO

Each was applied as its reviewer specified. The design then carries the accepted status and hashes `4f0fae585affb71b707c47573fb25aa1886a79287a9d142cb22fa35300dd8950`.

1. **Safety P3-5: one clock origin.** §7's suspend-aware clock now moves the instance's single origin as a whole: the SSH worker's `now`, the admission deadline conversion, the setup helpers' `Control` deadlines and the HTTPS pool's epoch. A3's single-origin rule therefore holds on the new clock.
   - A request in flight across a sleep expires on wake by its own deadlines.
   - Where no suspend-inclusive clock exists, the origin stays monotonic and the wall-clock check discards idle entries.
   - §13 amends A3:12-14 to match, and the "On GO" list adds A3. O5's amendment says both kinds of clock read one origin. §16.2 notes that sleep is not interaction. §15 item 11 gains the setup-across-sleep case.
2. **Consistency P3-8: a lease under proof.** §4 states the order. On a cancelled token or a passed deadline, the host releases the lease as reusable before it cancels the binding's pool work, so the connection returns to idle. When the binding is abandoned, the `cancel_session` backstop closes the lease with `Cancelled`. The outcome table and §15 item 6 say the same.
3. **Safety residuals:**
   - the bound's wait holds no registry lock (§7, C3);
   - binding session IDs never repeat within a host context's life, because equality skips the check (T3);
   - one detacher: a finishing operation marks its binding, and only the instance pump detaches it (§2).
4. **Consistency residuals:**
   - T4 names the `Revoked` disposition that `release` gains;
   - gwz-py's drop at interpreter exit defers to the session plan's CS4.5, which owns the exit path;
   - §13 records A3:24-25's reading, a cleanup error closing admission without an immediate stop, as a note in the A3 slice;
   - a binding keeps its link thread, and its driver session is stepped as today.

## Recorded

- **Not taken:** a separate note on decision 10's alternative, which §12 already discloses.
- **Carried to TR1.3:** the `auto` key for routed operations with the off switch off, and the server-design edits §13 lists.
- **Carried to TR1.4b:** the session plan's runtime sentences §13 lists, and CS4.5's exit path.

## Application

§13's "On GO" list is applied under AgentProcessRules §7.2 once the operator decides §16.1 and signs off the two release-plan clauses:
- **The contract's status:** "Amended … by `GwzConnectionReuseDesign.md` …" for the sections §13 lists. The paired GWZDesign paragraphs change with it.
- **The release plan's two clauses,** with the operator's sign-off.
- **The accepted gwz-core documents:** the placement design, HTTPS design, retry plan, transport requirements, transport design, SSH production setup, SSH agent design and its A3 slice. Each gets a status sentence and a changelog entry.
- **The guides** change with steps C4 and T2. **The server design and the session plan** change through TR1.3 and TR1.4b.

## Next action

The operator decides §16.1 and signs off the two plan clauses. The "On GO" list then lands and TR1.2 closes. TR1.3, the server design revision, can start on this design.
