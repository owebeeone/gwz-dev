# GwzRemoteTransport HTTPS H1 — Code Review

**Date:** 2026-09-22  
**Axis:** Code  
**Verdict:** **NO-GO**

## Review object

The tuple was verified unchanged at the start and end of review:

| Repository | Commit | Tree |
|---|---|---|
| root | `ead57a6353daa145d7372231c22e9d058f4cf085` | `570dba2e07e15bee4a36f0640fb6dbdaf699d6ab` |
| gwz-core | `280f970a66047942b006cfecfae51db60fa0550b` | `42cb148dd9ff023f5610635c94b144ffbcbcecbb` |
| gwz-transport | `aa40936d0805e8cb60f8027615abe20d4f2045e4` | `e2c13ebdad077f447cb142c3143fdd3ed1d1a5a2` |
| taut | `bcf98b64d465fc54841121b6d1a2d46940f81a3c` | `583da1f46c3eb56cab1908997e41d5b35e8a37ff` |
| gwz-cli | `7db07bbdefd2897c07fd0f9e550bf032bd8b1314` | `cdcfb401e8ba5ae67e015e175d4caceb742d52b9` |
| gwz-py | `d07d55dacb1725d9306be9c04d157ac29a78e000` | `6d6ab339a70bdd72907c35b6e6b309c772e66358` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |
| gwz-core-evidence | `a4088b50f6322fb6c8985f3559fb45486d29a873` | `bff890871db16b7080128270efd8ee13d21ef0e6` |

I inspected the committed H1 report and accepted HTTPS/transport contracts, the `https_*` implementation and tests, shared reservation code, transport codec/mux changes, and the archived `2026-09-22-h1` evidence. Evidence source hashes match the reviewed blobs. The archived logs record 45 endpoint tests, the full transport suite, and the default core check passing. I also ran the permitted existing binary filter: **37 passed, 0 failed**.

The native clone, multi-round fetch, and large-push tests exercise the git2 client against fixture upload-pack/receive-pack server processes. Redirect construction, authentication transition restrictions, POST effect timing, final connection attribution, Gh-only reused credential facts, pool disposal, bounded bodies, and default-manifest isolation otherwise hold under the inspected paths.

## Findings

### P2-1 — A per-remote owner retires the entire operation’s redirect routes

**Root cause:** whole-operation route retirement is attached to `LocalRpc::drop`.

**Locations:** `gwz-core/src/git/endpoint/https_local.rs:151-155`, `https_worker.rs:101-105`, `https_policy.rs:112-114`; contract `GwzRemoteTransportHttpsDesign.md:196-212`.

`LocalRpc::drop` cancels its current exchange and calls `Client::finish_operation`. That removes every route whose key has the same operation ID. The accepted contract instead keeps each write-once route until all dependent streams for the entire operation retire and expressly says per-stream cancellation or close cannot erase it.

Concrete interleaving: two remote adapters share a client and operation ID; remote A installs the first final base; remote B remains live; dropping or cancelling A drops its `LocalRpc` and removes the operation routes. B can then admit the original key afresh and install a conflicting redirect rather than receiving `Protocol`, allowing the losing remote to reach its POST.

Move route retirement to an explicit whole-operation owner, or use an operation lifetime/reference count whose last dependent releases the routes. `LocalRpc` drop should retire only its own RPC work. Add a regression with two distinct `LocalRpc` owners sharing an operation: pin A, drop/cancel A, then prove B cannot replace the route or POST; only explicit final operation retirement may clear it.

### P2-2 — The H1 budget model conflates allocation, interaction, connect, and network time

**Root cause:** one allocation-derived absolute deadline caps unrelated domains, while active-I/O configuration is absent from the endpoint input.

**Locations:** `https_worker.rs:107-176`, `178-257`, `275-303`; `gwz-transport/src/pool/mod.rs:15-39`; contracts `GwzRemoteTransportDesign.md:658-668,705-735` and `GwzRemoteTransportHttpsDesign.md:270-280`.

`Budget.until` is initialized from `allocation_timeout_ms`. Helper lookup uses `min(helper_deadline, until)`, pool checkout receives the same remaining `until`, and GET header receipt uses `min(network_deadline, until)`. Thus a short allocation timeout expires helper, connection, and header work even when no capacity wait occurred. H1 also hard-codes the network allowance to three seconds. `pool::Config` and `Endpoint::new` carry no configured active-I/O timeout, so the required zero-disabled or nondefault network timeout cannot be captured. `Prepared` can only publish the hard-coded remaining value.

For example, with `allocation_timeout_ms = 1` and a valid helper taking more than 1 ms, authentication returns `Timeout` without having waited for capacity. A configured zero network timeout is likewise impossible: H1 always installs a positive stream timeout.

Represent and spend allocation wait, connect/auth network, helper interaction, active I/O, and cleanup separately. Start the allocation clock only while waiting for admission/capacity; carry each remaining domain through redirects and the authentication transition. Add endpoint configuration for active-I/O timeout with the specified zero semantics and Open shortening rule. Add causal tests for delayed helper work with no allocation contention, independent allocation contention expiry, zero/nondefault I/O timeouts, and redirect/auth consumption of remaining budgets without replenishment.

### P2-3 — Malformed HTTP responses are reported as network I/O failures

**Root cause:** Hyper request failures are flattened without inspecting their protocol/parse classification.

**Locations:** `https_worker.rs:297-301` and `566-570`; contract `GwzRemoteTransportHttpsDesign.md:298-317`.

Both discovery and POST map every `SendRequest::send_request` error to `ErrorCode::Io`. A verified TLS peer that returns a malformed HTTP status line or malformed header makes Hyper fail while parsing the response, but H1 reports `Io`. The accepted failure table assigns malformed headers/framing to `Protocol` and reserves `Io` for network loss. The existing truncated-body test reaches a later body-frame path and therefore does not exercise this header failure.

Introduce one classifier for Hyper errors that distinguishes HTTP parse/protocol failure from underlying transport loss, preserving conservative POST effect facts. Add real TLS raw fixtures for a malformed status/header and a disconnect before valid headers, through `Client` and the endpoint translation seam, proving `Protocol` and `Io` respectively.

### P3-1 — New conditional test code violates the workspace’s explicit-boundary rule

**Location:** `gwz-core/src/git/endpoint/shared_reservation.rs:196`.

The new direct `#[cfg(test)] mod tests` declaration violates the workspace rule requiring `cfg_if::cfg_if!` or an enclosing conditional module. This recreates the attribute-reassociation hazard the rule is intended to prevent. Wrap the test module in an explicit conditional boundary and include the repository’s syntax-aware conditional-compilation check.

### P3-2 — The helper cancellation test never starts the helper it claims to reap

**Locations:** `https_auth.rs:409-421`; claim `GwzRemoteTransportHttpsH1.md:32-36`.

`cancellation_kills_and_reaps_hanging_helper` cancels its token before calling `lookup`. The lookup can return at its initial cancellation check without spawning `sleep 5`, so the test passes even if post-spawn cancellation, child kill, reaping, or permit retention regresses.

Have the fixture signal that the child has started, cancel afterward, then assert bounded completion and no lingering child, pending-cleanup record, or retained admission permit. Keep the pre-cancel case separately if desired.

## Verdict and pre-commitment

The candidate is **NO-GO** because P2-1 permits loss of an operation’s write-once route, P2-2 violates the accepted timeout-domain contract, and P2-3 breaks the typed failure classification.

I pre-commit to **GO** on a revision that resolves **P2-1, P2-2, and P2-3** with the specified causal regressions, provided changed-range review finds no new blocking interaction. P3-1 and P3-2 are bounded and should be closed in the same correction.
