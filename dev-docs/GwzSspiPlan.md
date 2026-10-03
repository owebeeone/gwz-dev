# GWZ SSPI implementation plan

2026-10-03. **ACCEPTED bounded plan**, with [design](GwzSspiDesign.md) revision 2
and exact GO tuple in [acceptance](GwzSspiAcceptance.md).
Scope: Windows native authentication process boundary. Full Windows activation
and provider parity retain their separate gates. No implementation starts before
Consistency/Safety and caller-guide Surface GO on the exact settled tuple.

1. **Repository and contract.** Once a remote is supplied, use GWZ to add the
   `gwz-sspi` member. Author the private IPC schema with taut, export its schema and
   generated Rust, and implement the narrow caller API. The message checkpoint
   now accepts the required raw TokenLimit caller input after its own revised
   Surface GO (GwzSspiMessagesAcceptance.md). Schema/IR-only acceptance cannot close
   the later codec/API secret-boundary review. No dependency on core,
   Git, transport implementation, CLI or Python. Standalone fake OS/IPC/deadline
   tests first, including secrets in codecs and malformed bounded framing. Stop
   for dual Code/State review of the codec/API secret boundary before step 2.
2. **Supervision kernel.** Implement bounded admission, creation-time Job/HANDLE
   attachment, private pipes and dedicated charged I/O/launch ownership. Test every
   state/effect race with deterministic schedules and seeded random sequences;
   retain failing seed/input/trace. Cover dropped futures, late native completion,
   shutdown/quarantine and saturated admission. Review Code/State on this boundary.
3. **Native SSPI and bootstrap.** Serial context/token handling, checked identity,
   native buffer/credential disposal and default/explicit identity. Native public
   Windows fixtures reproduce the proved Job, parent loss and pipe cases, secret
   storage normal cleanup and owned-handle reaping. Rejection means no degraded
   fallback. Digest remains unavailable for product use until parity is proved.
   Stop for dual Code/State review of native secret ownership/disposal before step 4.
4. **Hosts and core composition.** CLI self-exec and Python bundled executable
   use the same worker entry. Core supplies identity/CBT/deadline/route lifecycle;
   no new CLI/core protocol or network carrier. Installation/mismatch/no-worker,
   cancellation, deadline/provenance, route retirement and post-effect retry tests
   cover both hosts. No new helper timeout allowance. Review secret adapters on
   both axes, then installed caller Surface check.
5. **Qualification.** Settle library/core/CLI/Python tuple and obtain aggregate
   Code/State GO, preserving exact raw native evidence privately and public CI
   fixtures publicly. Resolve all Windows parity §11 rows and compatibility
   decisions before any Windows endpoint activation. Design mechanism GO does not
   qualify Kerberos/Digest/EPA/trust/proxy/Pageant or authorize guard removal.

Steps 1–3 form a cohesive library sequence with the two explicit secret-boundary
stops and kernel review above; step 4 is a composition chunk. Do not
split implementation into per-function reviews. Independent remaining Windows
investigations may run alongside after scoped authorization, without changing the
frozen library contract. Builds/runtime caches stay outside repositories; prompts,
reports and current decisions stay in dev-docs. No push/tag/release is implied.
