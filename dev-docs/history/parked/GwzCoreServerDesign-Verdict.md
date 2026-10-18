# GWZ core server design, revision 1 (TR1.3) — first review verdict

Date: 2026-09-28. Status: **NO-GO at SHA-256 `15d5410fa4bd17735299bd8b37025f25fcc3730b996c8c35a22e7b977d6e5739`: [Safety](GwzCoreServerDesign-ReviewSafety.md) and [Surface](GwzCoreServerDesign-ReviewSurface.md) reported NO-GO, and [Consistency](GwzCoreServerDesign-ReviewConsistency.md) reported GO. Both NO-GO reviewers committed in advance to GO on a revision that resolves their blocking findings as specified.** This verdict accepts nothing.

The object was revision 1 of the server design, step TR1.3 of the transport release plan: the draft committed at root `17a5d5d` (SHA-256 `8c38616d…`, 422 lines), revised in the working tree to 1221 lines. It was read against root `4bf52e00`, gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5` and gwz-transport `a7a36aec`, and against frozen copies of nine controlling documents whose accepted edits are not yet committed.
- Three fresh reviewers ran in parallel. None read another's report; the reports were filed only after all three had finished.
- Each verified the object, the five HEADs and the frozen copies at the start and the end.
- The Surface reviewer read only the design's user-facing section (§17) and the captured help of gwz 1.0.17 and gwz-py.
- All ran only inspection commands and wrote nothing.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | GO | 0 | 0 | 0 | 11 |
| Safety | NO-GO | 0 | 1 | 3 | 13 |
| Surface | NO-GO | 0 | 0 | 2 | 13 |

## Blocking findings

| ID | Axis | Finding |
| --- | --- | --- |
| S-P1-1 | Safety | The lock and log files the host creates in the per-user directory, which the design assumes a same-user sandboxed process can write, have no no-follow, file-type, owner or append rule. A planted symbolic link makes the unsandboxed server write to any file of the user, and a planted FIFO wedges the start. |
| S-P2-1 | Safety | The relative Linux sandbox rule (namespaces and seccomp filter count) cannot tell two sibling sandboxes of one user apart. A sandboxed caller's snapshot can reach, and a sandboxed server can serve, another sandbox, which three sentences of the design deny. |
| S-P2-2 | Safety | The client waits for `SessionOpened` with no bound. A wedged server hangs every client, `server --stop` and `server --status` included, with no message naming the process. |
| S-P2-3 | Safety | The stale-socket rule removes a live socket of another program at an explicit address, since a free `.lock` shows only that no gwz server holds it. |
| Su-P2-1 | Surface | `--ssh-timeout` has two defaults on the surface: 9 seconds for the server and 3 in the released client help. Under the design's own timeout rule, a user with default settings is refused on the first native-route operation through a server. |
| Su-P2-2 | Surface | The `server` command's actions are mutually exclusive flags (`--start`, `--stop`, `--status`, `--stdio`), where gwz's families of verbs are subcommands (`local clone|list|dispose|disband`, `repo add|…`). The shape cannot change after release without breaking scripts and service units. |

No reviewer classified any finding as architectural: the trust boundary, the must-match model, the wire and the reuse model held under all three attacks. This was the first round; the two-round cap has one round left.

No blocking finding is confined to §16, the SSH remote form. Its two findings, S-P3-13 and Su-P3-13, are P3.

## Blind convergence

Reviewing blind, the axes landed on the same clauses:
1. **`server --stdio` and an inherited `GWZ_SERVER`.** Consistency P3-5 and Safety P3-12: the stdio child inherits `GWZ_SERVER=stdio`, or a remote profile's value, and nothing says whether `server --stdio` parses it.
2. **A library session's timeout under `auto`.** Consistency P3-3 and Safety P3-11: a Python client that configures a timeout after opening is refused under `auto`, which three sections say cannot happen. Surface P2-1 reaches the same timeout row from the default's side.
3. **The Linux sandbox rule.** Consistency P3-1 and Safety P2-1 attack the same clause from opposite sides: the rule does not give the refusal the plan's TR1.3 "Defaults" bullet asks for, and it cannot tell sibling sandboxes apart.

## Confirmed by the reviews

- Consistency checked every quotation of another document verbatim, and spot-checked the enumerated native-path reads and the address rules against the vendored sources and the helper reports; all held. It found a verification row for every Phase 7 exit row.
- Safety found the enumerated environment list complete against libgit2 1.9.7, libssh2 1.11.1, openssl-probe and native-tls, and the walk-then-connect race bounded to a mount or automounter contact, never a disclosure.
- The design's corrections to amendment §3.4's macOS probe and §3.6's `/net` rows were found to quote the amendment exactly.

## Next action

The [remediation plan](GwzCoreServerDesign-RemPlan.md) gives every finding one disposition. The drafter applies it as revision 2, in one patch. The same three reviewers then re-verdict revision 2 with their context intact, each over its own findings and over every changed range.
