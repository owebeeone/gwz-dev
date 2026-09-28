# Crate map post-GO confirmation — architecture (Code axis)

Object: `dev-docs/GwzCoreSessionCrateMap.md`, 220 lines, SHA-256 `584b8431aa1ce6206ab2043e7eb5fbe5bf67bc3fae731e326f9aeec8c9c38f13`; diff `cratemap-r1-to-postgo.diff`, 72 lines, SHA-256 `823e641734c927c4b639cd6c2de4fb3cc008bc0395a148a62bfcfd276df0173d`. Both verified at start and end.

**Scope of the diff.** It changes only the status line, §1's test-support bullet, §4's contract, policy, instance, ssh and https rows, the prepare.py bullet, CS7.1's three commits, §7's reuse-design entry and §9's round-2 table. Nothing else.

| Correction | Status | Evidence |
|---|---|---|
| N1 | CONFIRMED | `placement_endpoint.rs` is the SSH engine: session.rs holds `engine: Option<PlacementEndpoint>` built from `ssh_local::connect_with_authority` (:139, :407), and the file wraps `ssh_worker::{BridgeContext, Endpoint, EndpointAttachment}` (:4-10). The HTTPS bridge imports only `EndpointError` and `Outbound` from it (https_endpoint.rs:7; every other use is an `EndpointError::` variant). Both are free of SSH coupling: a six-variant enum (:35-42) and `Outbound { request: String, envelope: Envelope }` (:73-76), so the contract crate can hold them. |
| N2 | CONFIRMED | §4: the transport workspace is linked into the candidate root as `crates/` is, so `../../crates/ids` from `<candidate>/transport/x/` and the root's `crates/ids` name one path. |
| N3 | CONFIRMED | §7 adds the reuse design's §14 step for the host-given pool ID beside T1–T5. |
| N4 | CONFIRMED | §1's `test-support` feature, enabled only by core's dev dependencies, matches the `contract-tests` precedent (repo-inspect and local-testrepo enable it as dev edges); §4 splits CS7.1 into movement with unit tests, the test-support repointing, then the splits. |

**Two notes, no verdict change.**
- The bridge's one other cross-crate call is `super::session::unique()` (https_endpoint.rs:178), the `SERIAL` counter in the instance crate. §4's rule that the counters take IDs from the host context's `IdSource` removes that edge; the extraction should hand the HTTPS engine its source at construction rather than keep the call.
- CS7.20 also changes `https_pool.rs`; the gwz-https-endpoint row lists its parts of CS7.13 and CS7.22 but not CS7.20.

GO stands.
