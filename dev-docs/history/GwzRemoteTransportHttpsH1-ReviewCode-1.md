# GwzRemoteTransport HTTPS H1 Correction 1 — Code Re-verdict

**Date:** 2026-09-22  
**Axis:** Code  
**Verdict:** **GO**

## Review object

The tuple was verified unchanged at the start and end of review:

| Repository | Commit | Tree |
|---|---|---|
| root | `7944b963b96f557768a332e64efaa018346573a1` | `82d69dc592ae020dc3e9a5b6943c2ae254c6ad47` |
| gwz-core | `afd3b8f58ca4e5bea788f1a4d40317d4e2a74a6e` | `d71255e0bae12dbf9b8366fd3a60994c0e3744b8` |
| gwz-transport | `aa40936d0805e8cb60f8027615abe20d4f2045e4` | `e2c13ebdad077f447cb142c3143fdd3ed1d1a5a2` |
| taut | `bcf98b64d465fc54841121b6d1a2d46940f81a3c` | `583da1f46c3eb56cab1908997e41d5b35e8a37ff` |
| gwz-cli | `7db07bbdefd2897c07fd0f9e550bf032bd8b1314` | `cdcfb401e8ba5ae67e015e175d4caceb742d52b9` |
| gwz-py | `d07d55dacb1725d9306be9c04d157ac29a78e000` | `6d6ab339a70bdd72907c35b6e6b309c772e66358` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |
| gwz-core-evidence | `3f5a4cce6bc108960f81a69880c2fda91be3895b` | `f62add1a43e6b5fa7dfb3b3da40ee2d818c511b6` |

I reviewed the original Code report, merged remediation plan, corrected core range `280f970a..afd3b8f5`, controlling H1/HTTPS documents, and the `2026-09-22-h1-rem1` evidence. Manifest hashes for the changed endpoint sources match the committed blobs.

The archived correction gate records 64 endpoint tests and the default core check passing. I independently ran the permitted existing corrected binary filter: **56 passed, 0 failed**. No build was performed.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 | Explicit whole-operation sealing and dependent lifetime accounting; per-remote drop only cancels its own work. | Retraced the original A/B/C interleaving. `LocalRpc` and each admitted `Prepared` hold operation dependencies; `LocalRpc::drop` no longer retires routes. The causal test pins route A, drops remote A, rejects remote C’s conflicting route, lets surviving B POST through route A, then proves explicit sealing rejects new work until dependencies drain. | **Closed** |
| P2-2 | Separate allocation, helper, connect, active-I/O, and cleanup budgets; configured I/O capture including zero and Open shortening. | `Budget` now maintains independent domains. Admission and pool wait alone consume allocation time; helper lookup consumes interaction time; connection setup and network headers consume their own remaining allowances. Tests prove a delayed helper survives a 1 ms uncontended allocation budget, real capacity contention expires independently, zero/custom I/O behavior works, Open can shorten but not disable a configured timeout, and redirect/auth transitions do not refill the network allowance. | **Closed** |
| P2-3 | Classify Hyper parse/protocol errors separately from transport loss. | `classify_hyper_error` is used by both discovery and POST send paths. Verified-TLS raw malformed status/header fixtures produce `Protocol`; an empty/disconnected peer produces `Io`; actual worker outcomes cross the mux as one `OpenFailed` without creating a stream. | **Closed** |
| P3-1 | Replace the free conditional test attribute with an explicit boundary. | `shared_reservation.rs` now encloses its test module in `cfg_if::cfg_if!`. The archived token-aware check inspected disabled branches in 21 files and reported zero violations. | **Closed** |
| P3-2 | Cancel only after proving the helper started; verify reap and admission recovery. | The new barrier starts eight hanging helpers before owner cancellation, verifies active count reaches eight, joins cancellation, reaps retained children, and proves admission recovery. A second abort-after-start test proves the helper permit remains reserved until owner reap. The original pre-cancel case remains separately named and scoped. | **Closed** |

## Changed-range analysis

The correction adds 1,962 lines and removes 129 across fourteen core files; transport and protocol sources are unchanged.

`https_operation` supplies the missing lifecycle boundary. Lock ordering remains consistent: operation state is acquired before route state, and dependency drop retires routes only after explicit sealing and the last dependent. An already admitted preparation may complete after sealing; later preparation acquisition is rejected. H2 still owns attachment of `finish_operation` to the production operation owner, as explicitly deferred.

`https_worker` and `https_pool` now carry independent remaining budgets. Pool checkout reports allocation wait separately from connector elapsed time, including reused connections. Endpoint construction captures active-I/O configuration, including zero-disabled behavior, while admitted Open values only shorten the captured settings. Redirect and anonymous-to-Gh attempts reuse the same budget object.

`https_auth` introduces endpoint-scoped active and retained-child ownership. Cancellation after spawn kills and joins when possible; task abortion moves the killed child and its global helper permit into the owner’s pending set until actual reap. Endpoint shutdown closes preparation/helper admission before cancellation and reports active preparations, retained helpers, and pool work. The changed future-abort path does not prematurely release capacity.

`https_opening` now drives real Open admission and routes actual `Opened`/`OpenFailed` receipts through an in-process mux pair before stream exposure. Automatic authentication uses a distinct second Open under the remaining budget. Successful stream messages continue through the same admitted mux session and stream identity. This remains candidate-local composition and does not claim the deferred H2 host integration.

The changed range also preserves cumulative `credential_offered` across redirects while leaving status and authentication tied to the current response. I found no new architectural root or other P0–P3 defect in these interactions.

## Verdict and pre-commit conditions

All five original Code findings are closed on the corrected immutable tuple. **GO** for the HTTPS H1 candidate.

There are no additional Code-axis pre-commit conditions. This verdict does not certify H2 host integration, shared SSH/HTTPS authority injection, platform or selected-source qualification, production construction or activation, physical transport, release, or performance.
