# GWZ SSPI bounded design acceptance

2026-10-03. **GO — design/API mechanism only.** Accepted revision 2 at this
reviewed tuple:

| Repository | Commit |
|---|---|
| root | `f2029e4b1739c0214138675dfb16abdb44f6a0d7` |
| gwz-core | `d78a664e3c5a325c6f12be409eb7645c1c1b51d0` |
| gwz-core-evidence | `1beb1d204c824701ddbd033c7f89df9a3561f5e5` |

Independent GPT-6.1 Sol verdicts on this exact tuple:
[Consistency GO](GwzSspiDesign-ReviewConsistency-1.md),
[Safety GO](GwzSspiDesign-ReviewSafety-1.md),
[API Surface GO](GwzSspiDesign-ReviewSurface-1.md).
Raw reports are filed verbatim. One merged remediation round closed the two
blocking contract issues and four nonblocking wording/planning issues. The
Consistency/Safety blind convergence was Digest's missing first challenge carrier.
The initial Safety caller-only pass is preserved with its scope correction and
reviewer withdrawals; it did not substitute for the full Safety gate. No third
architectural root cause was found.

Accepted: [Windows-specific `gwz-sspi` design](GwzSspiDesign.md),
[implementation plan](GwzSspiPlan.md), [caller API](GwzSspiCallerGuide-DRAFT.md),
and their narrowly enumerated Windows-draft/crate-map amendments. CLI self-exec
and Python bundled worker compose the same independent library. Creation-time
Job attachment, private taut IPC, explicit identity/secret ownership, immutable
parent deadline, publication revocation and retained quarantined capacity are
required. Normal disposal and forced exit have different guarantees. Negotiate
permits native Kerberos or NTLM; no Kerberos-only restriction is introduced.

Not accepted: product implementation, full Windows parity, provider/Digest/EPA,
interactive SSO, TLS binding lifetime, Pageant/external cancellation, proxy/trust,
package installation, guard removal, platform/release activation. These keep their
existing gates. No push/tag/publication or product code change occurred.

Next: establish the `gwz-sspi` member once its remote is supplied, then implement
standalone contract/secret codecs with fake tests and the explicit dual secret-
boundary stop before native supervision work. Builds remain external, public CI
fixtures independent of private evidence. A later implementation instruction is
separate from this completed design-review task.

Verification: package links and diff whitespace checks; existing merge/local-clone
fast document guards; private archive verification including indexed bytes; exact
bytes/modes for all 549 imported raw files. These do not substitute for executable
implementation tests. Existing macOS source qualification remains unchanged.
