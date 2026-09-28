# GWZ core server design, revision 2 (TR1.3) — acceptance verdict

Date: 2026-09-28. Status: **accepted as a design at SHA-256 `9fc802617c0478a7568baea6e1a9623bb46bed9a0a9375eb83ed9f9647384e85` after [Consistency-1](GwzCoreServerDesign-ReviewConsistency-1.md), [Safety-1](GwzCoreServerDesign-ReviewSafety-1.md) and [Surface-1](GwzCoreServerDesign-ReviewSurface-1.md) reported GO; this accepts the design text only**.
- It authorizes no implementation, activation or release. TR1.4b places the design's steps.
- On 2026-09-28 the operator decided OD12: the SSH remote form (§16) ships in this release. The operator also signed off the corrections the design carries to accepted plan text: the release plan's Phase 10 check, and the amendment's §3.4 probe and `/net` sentences and §3.6 macOS rows.
- TR1.3 closes with this verdict and the "On GO" list applied (below).

The same three reviewers re-verdicted revision 2 with their round-1 context intact.
- Each verified the object, the five HEADs and the nine frozen controlling copies at the start and the end.
- Each confirmed that the supplied diff is exactly the change from revision 1, re-ran its own round-1 counterexamples against revision 2, and analysed every changed range.
- None read another's round-2 report; the reports were filed only after all three had finished.
- All ran only inspection commands and wrote nothing.

| Axis | Verdict | Round-1 findings | New |
| --- | --- | --- | --- |
| Consistency | GO | All 11 closed (C-P3-1 to C-P3-11) | P3-1 to P3-3 |
| Safety | GO | All 17 closed (P1-1, P2-1 to P2-3, P3-1 to P3-13; P3-3 disposed as the plan chose) | P3-14 to P3-16 |
| Surface | GO | All 15 closed (P2-1, P2-2, P3-1 to P3-13) | P3-14 to P3-17 |

Every reviewer judged each of the remediation plan's three stated choices, and the drafter's disclosed departures, against its round-1 attacks:
- the absolute Linux sandbox rule;
- the server-first `SessionHello` frame with its bounds;
- the `server start|stop|status|list|stdio` subcommands.

No reviewer classified anything as architectural, so the two-round cap is not reached. Each cleared its new findings to land without a further round.

## Corrections applied after the GO

The drafter applied the new findings and the cheap residuals as one patch, each as its reviewer specified (the drafter's record lists every departure from a correction's letter):
1. **Consistency.**
   - P3-1: only the session path refuses a differing core, so `server list` and `server stop` reach older servers.
   - P3-2: an empty `SSH_AUTH_SOCK` or `SSLKEYLOGFILE` is a value, not a relative path.
   - P3-3: on Windows, `server start` recovers from a stale recorded `auto` pipe, as an `auto` client does.
   - Residuals R1 to R3: the reuse design's status sentence uses the §7.2 pattern; a server double tests the sandbox row; a non-pipe agent path gets its own message.
2. **Safety.**
   - P3-14: the lock record is quoted "from its lock file", and read and walked under the file and address rules.
   - P3-15: every client-side connect is non-blocking under the same bounds.
   - P3-16: the Windows pipe name comes from the OS cryptographic generator, is redrawn on a collision, and is recorded only once the pipe exists.
   - §14 states that without `/proc` the Linux check fails closed.
3. **Surface.**
   - P3-14: the program name is the invoking CLI's.
   - P3-15 [SSH form]: the commands refused under the form are listed, with a usage error and its message.
   - P3-16: `permission_denied`'s rules for `SSH_AUTH_SOCK` are enumerated, with a next step.
   - P3-17: the session-cap message carries the hint only for the CLI.
   - The residuals taken: `--idle-exit` on the `already running` line; "after closing its sessions"; `XDG_RUNTIME_DIR` or `TMPDIR` in the directory refusal; and help tests that use `gwz server start --help`.

**A narrowing the lane owner decided.** While applying Safety P3-15, the drafter found that on macOS a connection to a full accept queue is refused exactly as a stale socket's is. So at an explicit address, `server start` could have removed a stuck program's socket. Holding the lock, the host now removes an existing socket only when all three hold:
- it is a socket the user owns;
- the connection is refused;
- the lock record beside it names exactly that address.

Otherwise the start refuses and deletes nothing. gwz's own crash recovery is unchanged, since a gwz socket never outlives its record.

**Confirmation.** All three reviewers confirmed the post-GO patch (`700e2e39…`), each for its own items. Safety confirmed that the narrowing keeps its P2-3 closed and adds no stuck state or disclosure. The lane owner then applied their last notes:
- Consistency: the host writes its record only after the stale-socket check passes, so a refused start leaves no record naming the address;
- Consistency: on macOS, `server list` shows a refused address as not answering instead of dropping it;
- Consistency: the unreachable "without `/proc`" residual is struck;
- Surface: the held-lock message gains a next step.

The design then carries the accepted status and hashes `61d8dfa741ddc1df23f73d2c6b9bdf363099d235754be6e68a65e6a1fefcbbfa` (1634 lines).

## "On GO" applied

Under AgentProcessRules §7.2, each with a changelog entry:
- **The contract's status** is amended for §1, §3, §4.1, §4.2, §5.1, §5.6, §5.7, §5.8, §9, §10, §11, §12, §13, §15 and §16. §1 is included because OD12 is yes.
- **The release plan's status:**
  - the Phase 10 check's subcommand spelling;
  - OD12 decided yes, with the amendment's §3.11 "If yes" texts applying.
- **The amendment's status and changelog:**
  - OD12: yes, and the "If yes" edits listed;
  - the four corrected §3.4 and §3.6 clauses.
- **The reuse design's status:** §11's "Two partitions compose" note.
- **The proposals' G1 row:** it takes OD9's wording.
- **gwz-core's GWZDesign and GWZRequirements:**
  - paired "Core server" paragraphs;
  - the git2 crate named as the credential helpers' spawner.

## Recorded

- **For TR1.4b:**
  - Linux CI rows that run a server need a runner outside a seccomp-profiled container (Consistency R4).
  - The session plan calls gwz-py's console script `gwz` where it is `gwz-py`.
  - The plan's Phase 7 gate names only gwz-core's checker, where the design's release gate also covers gwz-py's allowlist.
- **For S7.2's migration notes:**
  - no server inside a container that applies a seccomp profile;
  - the risk of forwarding an agent through the SSH remote form;
  - `--ssh-timeout`'s default changing from 3 to 9 seconds (the retry plan's change).
- **Program:**
  - `gwz help COMMAND SUBCOMMAND` does not resolve in released gwz.
  - The macOS release build may link Homebrew's OpenSSL dynamically; a separate task was offered to check it.

## Next action

TR1.4b, the session plan's second revision, places the steps of the reuse and server designs and revises the steps that depend on the runtime model.
