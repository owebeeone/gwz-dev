I've revised the design to revision 1 and edited only the new copy. The old copy is untouched (still `b9817f17…`), no git command changed any state, and the repos show the same untracked files as before.

**File:** `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/designs/GwzTransportConnectionStatsDesign.md`
- SHA-256 `67175c3ac2050fb3a703d2d15c84672f454b973cfbd579f556ede538ba1f56e1`
- 193 lines. Status is still DRAFT, not reviewed, now marked revision 1.

**Changes, by line in the new file**

1. **TR1.6 cited as accepted** (lines 5-7, 36, 89, 129, 171, 183).
   - Line 5 now says the accepted design hands the quiet-skip list to this one. Line 7 is new: accepted at `760f7ad4…`, filed as revision 4 at `e51d44b`, cited by its lines at `bb67a82`, with the operator's addition of the attempt number.
   - The `quiet_skips` classes (line 89) are now revision 4's names word for word, with underscores for spaces: `no_usable_credential`, `challenge_gwz_cannot_answer`, `credential_helpers_off`, `access_refused`, `credential_asked_after_discovery`.
   - That line also says M1, which now covers a `git` that cannot start, is never skipped. Neither is an M6 challenge on macOS or Linux that offers `Negotiate` without `Basic`, which now fails loudly.
   - Native skips (line 129) are classed by TR1.6's "1.0.17's case" column. The quiet-skip test (line 171) now uses TR1.6's T8(c) and T17(b).
2. **The wire, with TR2.22** (lines 18, 33, 65-66, 86, 98-99, 133-135, 144, 146, 168, 182).
   - D5 now says this design changes no gwz-transport schema. The boundary's one wire change is TR2.22's detail on `Failure`; this design reads it and adds nothing to it.
   - A failed stream gains three fields: its wire setup `cause`, plus `attempt` and `helper_cause`, both read from TR2.22's detail. M6's scheme tokens stay only in the member's error message.
   - New "Nothing is counted twice" rule (line 99):
     - Host counts come from the endpoint's setup records only; a member's failure adds nothing.
     - Every attempt number comes from TR2.1's machines at the endpoint. The driver derives none, so a stream's number and the connections' numbers cannot disagree.
   - OQ4 (line 182): with the endpoint in another process, the detail still gives each failed stream its `cause`, `attempt` and `helper_cause`. Setup records, host counts, machine states, phases and waits do not cross.
   - There is a new test row for this (line 168).
3. **Step edges confirmed** (lines 147-152):
   - TR2.1 comes before TR2.24a (field tag order, and the retry machines).
   - The split of the five large files comes before TR2.24a and b.
   - TR2.2, then TR2.22, then TR2.24a, as the plan's line 608 orders it. TR2.22's wire change has its own per-step review; TR2.24 makes no wire change.
   - TR2.5 comes before TR2.24a's native rows.
   - b and c follow a and can run in parallel, then the TR2.6 review, S7.1 and S7.5.
4. **Changelog revision 1** (lines 189-193).

**One change you didn't ask for.** Since `f5f0a1ff`, the TR2.1 lane's commit `1afea66` keeps one SSH retry machine per pool key *and* the identity each open names (members on the agent share one). HTTPS stays one machine per pool key. My revision 0 assumed one machine per key, so I updated it:
- Each host's `retry` is now a list, one entry per machine, labelled `agent` or `key` and never with a key path.
- Each connection names its `machine`.
- This touched lines 33, 73, 76, 87-88, 110 and 184, and added a test row (line 167).

**Judgement calls worth a reviewer's eye**
- I put the quiet-skip class names in snake case, word for word from revision 4, to match the other JSON values. If "exactly" means the literal strings with spaces, line 89 is the one to change.
- TR1.6 says no public field carries a setup cause today. Under `--verbose`, connection and stream entries now do (noted at line 133).
