# Gwz merge findings — draft

## 1. Process findings (extract, 2026-08-23 session)

What the record has actually taught, written as findings rather than a grade:

1. Review structure beats reviewer intelligence. The clearest result in the data: five of seven interior first-rounds were NO-GO with real blockers, and several of the biggest catches (the Windows compile P0, the provenance gap) came from Opus single-axis reviewers, not Fable. Peer-blind role separation between instances recovered defects that generation-time carefulness missed — same model, different context, different blind spots. The tier mattered less than the blindness, the graded-findings format, and the adversarial mandate.

2. My failure modes are state-shaped, not reasoning-shaped. Every burn — the checker sweep, the swept lane edits, the misrouted messages, the repeated ===/cwd slips — was a state-discipline failure across context boundaries, not a thinking failure. And notably: documenting a gotcha in my own notes did not prevent me repeating it; only structural mechanisms did (pristine overlays, exact-ref pushes, live-status stashes). Lesson: govern AI engineers with mechanism, not instruction.

3. Incidents → rituals is the real learning loop. Six burns produced six structural rituals, and the system got measurably safer while getting faster — seven landings today at zero damage versus the early CI-breaking sweep. The checkpoint's incident-record discipline (record, root-cause, mint mechanism) is doing more work than any individual artifact.

4. Freeze contracts, not work breakdowns. The frozen interface plus dated activation-records absorbed a dozen amendments without drift — extremely robust. The step decomposition leaked twice (3.1b didn't exist; E17 had no budgeted home). Vocabulary and invariants freeze well; task boundaries don't.

5. Estimation error has a specific, priceable cause. The 2× miss wasn't implementation difficulty — it was unpriced discovery chains: every fault family's first execution on every platform minted a diagnose-prove-fix-review sequence. Next time: price discovery per (novel surface × platform), not as free CI.

6. Diagnosis confidence needs a causal-proof gate. Three ARM attributions were confidently wrong before being cheaply overturned (parity rig, single-variable chmod, reading the dependency source). The discipline that saved it: nothing lands on a plausible mechanism, only on a single-variable flip or red-green. And corrections get recorded, not overwritten — which is why the third correction could cite the second.

7. Agent continuity compounds. The same implementer across 3.1→3.1b→3.2→3.3 absorbed the activation lesson, invented the single-crossing probe unprompted, and converged from 2× over budget to within it. Briefs that explicitly carry the previous review's lessons outperform fresh spawns — and transcript-resume survived two kill events at zero loss.

8. The human's leverage concentrated at the economics layer. Your interventions that shaped outcomes were quota windows, "burn it," conservation, and the thin-A1 scope law — zero lines of code review. The experiment suggests the human role in AI-autonomous engineering migrates to constraint design and resource governance, and that this frame was load-bearing, not decorative.

9. Speculative completeness is a new incident class. An AI can notice a real shape (“this is a DSL”) and, when asked to spell the missing facet, emit a coherent host/runtime (GwzAi: intent, capabilities, fences, receipts). Prose coherence is not a need. The merge program already taught the coordinator lesson; implementing the specimen would rehearse it. Default: park the facet as noise until a need is *observed* — a caller that dies and retries without a human, or an agent job that has no legal sentence and already invents git. Mint a job-freeze (who calls, which sentence is illegal today, what evidence would unpark) before any object model. `GwzAiReqDd.md` is the parked specimen.

Still open, and worth watching as the tail runs: whether deferred-escalation batching is sound — the Phase 5 dual is the designed test of whether six second-axis reviews can safely age to one gate, and it will say something either way; whether the assurance density purchased here pays off in production life (only A1 activation and time answer that); and whether any of this transfers off a well-specified systems domain onto messier work, which this experiment can't answer at all.

## 2. Merge-architecture findings

The section above is about how the program ran. This one answers the actual question — what was learned about building a *merge* this way.

**What "good merge" meant, operationally.** Four criteria, each now enforced rather than aspired to. *Handle-bound publication*: authority to publish, replace, or remove flows only from held, verified handles — one sealed no-replace primitive with a census-checked caller inventory (13), the retained rename-source handle ruled to BE the seam's identity proof (the os-32 fix was barred from reordering around it on principle), and "a path string or a successful `exists()` is not parent authority" made a property of the coordinator's types, not a convention. *Recovery from every open state*: a frozen 165-key fault vocabulary, every activated family's boundaries interrupted, restarted, and converged on both target variants — with the corollary that where post-crash causes are indistinguishable from durable evidence, the correct terminal is a typed refusal, and a recovery arm would be unsound (the E16 ruling). *Platform identity as a first-class contract*: per-platform primitive rows with win cells, carriage assignments, and stated negative space — writer-class-conditional durability and the barrier's caller-declared class are recorded arms in a contract table, not `#[cfg]` folklore; the table even says what a witness does *not* prove on Windows. *Rendering never deciding merge truth*: merge evidence is blob-exact while checkouts stay filter-aware; CRLF materialization is presentation, and every fixture pins that distinction at creation time.

**Which architectural bets paid.** The frozen interface plus dated activation-records absorbed roughly fourteen amendments from three concurrent lanes with zero drift — supersession clauses preserved history instead of rewriting it, which is why the third attribution correction could cite the second. Track P spikes before the freeze: the riskiest publication seam ran green natively on Windows before the freeze that sealed it was accepted. And causal-proof diagnosis as a landing gate — single-variable flips, a parity rig, reading dependency sources — meant nothing shipped on a plausible mechanism, three times over.

**Which bets leaked.** Work-breakdown IDs: the step list omitted a whole step (3.1b), left E17 without a budgeted home, and orphaned two keys no step owns — contracts froze well, task boundaries didn't. First-execution cost per platform went unpriced, which is the entire 2× estimate miss. And module-level `allow(dead_code)` made every inner allow inert, so "the allow fell away" was a fake reachability argument that let dead helpers and stale forward-references survive review — narrowing those blankets is now a settle item.

**The recipe, stated plainly.** Freeze the seams first; put a peer-blind dual on the freeze itself; implement against the freeze, never against intent; review every interior step single-axis with graded findings; auto-escalate on P0–P2, folding the deferred second axis into one settled dual; and when anything burns, mint a mechanism, not a memo. That loop — not model capability alone — is how this program got AI-built merge quality; section 1 only implies it.

## 3. Standing reminder (2026-08-30)

Do not design **non-transactional APIs where it matters** — crash, identity, publication, multi-step mutation. The object store can be fine while the live API (check-then-rename, porcelain files as truth, recover-by-heuristic) is the whole tax. A later “real git database” with a legacy impl and a transactional impl is a thought exercise, not this program’s next build. Finish merge on the current plan. Standing rule: `.cursor/rules/transactional-apis.mdc`.

## 4. Standing reminder (2026-09-01) — speculative facets default to noise

When a model proposes a complete-looking architecture for a hole nobody is calling, **park it**. Do not implement it to discover whether it was needed. Unpark only when the need is observed in use (retrying unattended caller, or a missing verb agents already escape to git for). Until then the document is a specimen, not a train. Current specimen: `GwzAiReqDd.md`.

---

**Status.** Keep this document as the process extract from that conversation. Do not treat it as the merge-architecture learnings record until the rituals are named and the product findings sit beside them with their evidence citations.