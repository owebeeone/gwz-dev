# p1c-https (1.3c/C2(b) per-exchange HTTPS auth) — remediation plan, round 1

Object: gwz-core `dabe7dc6` (lane p1c-https). Reviews:
- `GwzP1cHttps-ReviewCode.md`: NO-GO on P2-1, pre-committed to GO on a revision that resolves it;
- `GwzP1cHttps-ReviewState.md`: GO, with P3-1.

**Blind convergence:** both axes independently raised the missing standing row for the refused POST (Code P3-1 and State P3-1).

One patch, as a new commit (gwz has no amend). The Windows receipt re-runs on the corrected tree.

| ID | Disposition | Closure test |
|---|---|---|
| Code P2-1: POST pre-authentication keys on a sticky "ever helper-authenticated" connection record, so an anonymous POST to another repository on that connection runs the helper and fails when it declines | Fix. The connection's scope record keeps the repository path, or a digest of it, that the helper-token exchange authenticated. A POST is pre-authenticated only when its own path equals that path; otherwise it is sent anonymously as before `dabe7dc6`. Nothing secret is stored. | The reviewer's probe as a standing row in `connection_host/https_auth_tests.rs`. A `/private` advertisement is authenticated, a `/public` advertisement and POST follow on the same connection, and the helper declines `public`. The `/public` POST succeeds, reaches the server once, and carries no Authorization (compared by digest). It is red on `dabe7dc6`. `a_push_is_authenticated_on_the_connection_its_advertisement_was` stays green. |
| Code P3-1 = State P3-1: no standing row pins the deferred failure mode (a POST on a connection its advertisement did not authenticate, answered 401) | Fix: a row. | A push POST forced onto an anonymous connection: `PermissionDenied`, `Disposition::Discarded`, exactly one POST recorded with no credential, the POST's connection closed, the host `Open`. Red under a mutation that answers a POST's 401. |
| State P3-1 (doc part): the module doc and commit claim "an authenticated push can go through the host" without qualification | Fix. Qualify `https_adapter.rs:19-21`: a POST is pre-authenticated only on a connection a helper token authenticated for its own binding and the same repository path; a POST the pool places elsewhere is refused (a Phase 3 prerequisite). The lane owner puts the same text in the §15 1.3c/C2(b) row and names the deferral in the plan as a Phase 3.1 prerequisite. | Reviewer re-read. |
| State §4: stale comment in `changed_helper_gives_each_operation_its_own_credential` ("the instance's helper … is the first operation's") | Fix: correct the comment. | Re-read. |
| Code §3 clerical: the §15 1.3c row still cites the `reuse_tests.rs:141/:149` obligation; state the scope AND as implemented | Lane owner, at merge (root dev-docs). | — |
| Code §4: native caller per instance (a hypothesis for Phase 3); helper spawn cost | Recorded for Phase 3 design (findings). | — |
| State §4: no `credential approve/reject` | Recorded as a limitation (findings). | — |
