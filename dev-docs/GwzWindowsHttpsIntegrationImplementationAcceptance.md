# Windows HTTPS WH1 — limited implementation acceptance

Date: 2026-10-04.

**Status:** **accepted after [Code-3](GwzWindowsHttpsIntegrationImplementation-ReviewCode-3.md) and [State-3](GwzWindowsHttpsIntegrationImplementation-ReviewState-3.md) reported GO.**
- **Reviewed round-3 tuple:** root `c8ebae9a5dee0e876a96484607af3b92ae0298d3`, core `21f9e15ed4360c031f6d2c803224b173f82b3111`, transport `cd007b6868905543caa212155ad8ec99b3a4052c`, evidence `1add0746a1ffc50cd4f6cbe479c630b9cc2babeb`.
- **Unchanged members:** CLI `6ab16d461acb6daf9fca8281eca6384971cc0c44`, Python `5df15766298fbbd97da1d6ecec74c6cc9dd69fda`, sspi `582ec001bd2972076ea65a7db87d81d988c6f2e7`.
- **Native refresh:** the exact-source native refresh at the same sources passed. It is archived at evidence `566db869313cb072beeefe12670f1cab77d07d99` and root `738820d7f09d81f9be0d57577652001a05b6ee20`.
- **Scope:** this accepts the limited WH1 implementation only.

See the [merged round-3 verdict](GwzWindowsHttpsIntegrationImplementation-Verdict-3.md), and the [round-2](GwzWindowsHttpsIntegrationImplementation-Verdict-2.md) and [round-1](GwzWindowsHttpsIntegrationImplementation-Verdict-1.md) verdicts.

**What is accepted.**
- The Windows HTTPS qualification route, under `all(windows, gwz_transport_candidate, gwz_windows_https_qualification)`.
- **HTTPS support:** Anonymous and WindowsDefault, with captured WinHTTP DIRECT.
- **Native SSPI:** in a contained process, with the final-origin channel binding.
- **Native publication arbitration:** decided inside the mux lock through `gwz-transport` `Owner::send_if`, the one shared interface the operator approved.
- **Stale admitted input on HTTPS-only endpoint sessions:** treated as no-work.
- **On this route, these refuse:** SSH, Gh and configured-helper policies, and usable helper lookup.
- **The rest of the build is unchanged:** ordinary Windows and candidate-only Windows keep their earlier route.

**What is not accepted, and stays NO-GO:**
- ordinary Windows activation;
- WH2 (configured helpers on Windows);
- WH3 (integrated deadline, cancellation and identity adversity, installed paths with spaces or Unicode, worker provenance, pool reuse and concurrency, effects and retry adversity);
- provider parity (Kerberos/domain, Digest, proxy and native 407, Windows SSH/Pageant, differing identity, stalled provider);
- TR1.8's design;
- the platform, performance, selected-source, package and aggregate release gates;
- release.

**Disclosed and not waived.**
- strict core Clippy RED45;
- the generator owner-IR pin mismatch;
- six pre-existing candidate-leg failures, identical on the base:
  - two SSPI request-validator tests that need an external `CARGO_TARGET_DIR`;
  - `corpus_byte_parity` for `AddExistingRepoRequest`;
  - three gwz-py embedding tests whose candidate codec lacks `windows_configured`.

**Obligations.**
- **Push order:** gwz-transport `cd007b68` reaches transport `main` before gwz-core.
- **CI pin:** gwz-core's `.github/gwz-transport.commit` (`a24e70a`) moves with the owner-IR pins. The IR mismatch blocks that today.
- **gwz-sspi** has no remote.
- **Budget:** cumulative WH1 is 55 of 55 source, test and build files plus three inventories, and 2,438 of 2,600 gross added lines. Any further WH1 change needs a new disposition.
- **Root `19beeeb`** carries a placeholder commit message. Its content, the core lock move, is correct, and `c8ebae9` explains it.

**What this landing adds.** It adds these documents and the checkpoint entry after root `738820d7`. No push, tag, publication, installation or activation took place.
