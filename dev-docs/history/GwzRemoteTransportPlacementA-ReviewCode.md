# PlacementA aggregate — CODE-AXIS REVIEW

**Review object:** Phase 4 PlacementA aggregate at root `9cc9747c9466e877579bf95779dece092864b97e`, controlling DRAFT `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md` at `c0649c3d9a4804cee6eb180911da4863266c3045`, 2026-09-22.  
**Baseline:** `gwz-core` `c0649c3d9a4804cee6eb180911da4863266c3045`; `gwz-transport` `f6e9ee21fe5e39e3fc61d8484a9410062728131c`; `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `39a93c945b872d3a63fab08047165cc89853cc9f`. Sources were read from committed objects with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Code — architecture, interfaces, call graphs, compatibility, and error paths. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

I reviewed the complete pinned ranges, concentrating on:

- `gwz-transport/src/mux/{mod.rs,routing.rs,asynchronous.rs}`, `binding.rs`, codec/admission/schema changes, and the four permitted focused tests.
- Taut’s `missing_ok` model, validation, compatibility classification, Rust/Python generation, runtime CBOR helpers, and unsupported-target rejection.
- The owner profile-v2 schema, retained-v1 fixtures, terminal-facts validation, and candidate Rust/Python admission guard.
- `GwzRemoteTransportPlacementDesign.md` §§3–4 and 8, PlacementA, `TransportPlacement.md`, and the transport README.

The permitted external-target command passed: 34 tests across `mux`, `mux_async`, `placement_v2`, and `async_stream`; zero failures.

All nine pinned HEADs and commit trees matched at review start and end. Concurrent untracked current-round report outputs appeared under root; they were excluded and not read. Member trees remained clean.

## 1. Findings

### [P2-1] `finish` can delete an admitted terminal message before the port transfers it

`gwz-transport/src/mux/mod.rs:346-364` enqueues an endpoint terminal reply and immediately calls `transition`; `routing.rs:221-236` then removes the route. If the request owner calls `finish` before `Port::next_message` extracts that reply, `finish` calls `cancel` (`mod.rs:478-484`), whose `Queue::discard` removes `Failed`, `OpenFailed`, `IdentityChecked`, `IdentityCheckFailed`, and `Closed` (`mod.rs:116-127`). With the route already absent, `finish` returns success and unregisters the request.

Concrete sequence: bind; issue `CheckIdentity`; endpoint admits the action; endpoint calls `send` with `IdentityCheckFailed`; immediately call endpoint `finish`; it returns `Ok`, and `next_message` yields no typed failure. The initiator remains without its terminal result until unrelated timeout/closure.

This violates PlacementDesign lines 193-199 and the README lines 372-373: registration must survive terminal cleanup, and `finish` must block while owned delivery remains. It also defeats the claimed typed-terminal-before-EOF behavior.

Keep a terminal-pending route or equivalent owned-delivery state until the port transfers the terminal. `finish` must return `WouldBlock` and must not discard that terminal. Add regressions for immediate `finish` after worker terminal and timeout terminal: the terminal remains transferable, the peer observes its exact type/facts, and only subsequent cleanup permits successful finish.

### [P2-2] The mux collapses typed binding rejection into generic protocol closure

`binding::EndpointConfig::accept` returns an effect-free `Failure` for unsupported capability/limit negotiation (`binding.rs:145-181`). The endpoint mux discards that value by mapping every rejection to `Error::Protocol` (`mux/routing.rs:120-125`); `receive` then disconnects (`routing.rs:4-16`) without emitting `BindRejected`. Conversely, if a host supplies `BindRejected`, the initiator branch simply disconnects and returns `Ok` (`routing.rs:148-150`). `Owner::ready` can consequently expose only `Closed` (`asynchronous.rs:93-99`), not the typed reason.

A concrete SSH-only counterexample uses otherwise default endpoint limits with `metadata_bytes=128`. `Mux::endpoint` accepts this no-greater configuration, but `EndpointConfig::accept` rejects the negotiated limits as `UnsupportedOperation`; the peer receives no `BindRejected` and cannot distinguish unsupported limits from malformed protocol or carrier loss.

This contradicts `GwzRemoteTransportDesign.md:101-110`, which requires bounded typed `BindRejected` for unsupported version/capability/limit, and prevents the later public mapping of unsupported placement to `UnsupportedOperation`.

The endpoint mux must queue a validated `BindRejected` carrying the exact effect-free failure before retiring bootstrap. The initiator must preserve and expose that failure through a bootstrap result/action rather than erase it during disconnect. Add an incompatible-limit regression asserting exact `UnsupportedOperation`, no installed binding/routes/effects, and distinguish malformed Bind, which should still close as protocol failure.

## 2. Invariant analysis

The attacks on additive compatibility otherwise held: `missing_ok` is restricted to optional fields; prior optional slots remain strict; malformed present values fail; unsupported generators reject the option. Retained v1 readers and new v1 projections passed. Profile-v2 tags, single-authority `Closed.facts`, session/request/operation correlation, allocation-before-clone accounting, control reserve, waiter bounds, cancellation isolation, and candidate receiver-generation admission matched the committed claims. No Batch B facade, physical carrier, platform qualification, activation, or HTTPS implementation was inferred.

## 3. Risks and next action

The focused suite does not cover either ordering/error-disposition counterexample. Apply one bounded mux correction with the two regressions above, preserve the exact schema and deferrals, then request a focused changed-range Code re-verdict.
