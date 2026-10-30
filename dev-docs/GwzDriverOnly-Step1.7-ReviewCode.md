# Step 1.7 driver-only construction — CODE-AXIS REVIEW
**Review object:** plan step 1.7, gwz-core `b0ac8de7..edfc623e`. It covers `src/transport_host/driver_only.rs`, `driver_only_tests.rs`, the `mod.rs` declarations, the named route refusals in `src/git/gitbackend/transport_support/identity.rs` and `transport.rs`, and `scripts/checks/check_driver_only_boundary.py` with its test. **Baseline:** gwz-core `b0ac8de7`. gwz-transport `96938616` and root `34251a9c` are unchanged and were used for reference only. **Date:** 2026-10-30
**Axis:** Code (architecture, interfaces, call graphs, compatibility). Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2, 2 P3. I pre-commit to GO on a revision that resolves P2-1 as specified.

## 0. Evidence base

- **Tuple checked at start and at end; it did not move.**
  - Lane root HEAD `34251a9c`, gwz-core HEAD `edfc623e` with a clean tree, gwz-transport `96938616`.
  - The lane root has two staged files, `gwz.conf/gwz.lock.yml` and `conf-integrity.yml`. They record gwz-core at `edfc623e`. I did not judge them.
- **Read in full:** the diff of all 7 files; `transport.rs`; `transport_binding.rs`; `transport_support.rs` (`remote_callbacks`, `credential_callback`, `remote_credential`); `identity.rs::for_remote`/`resolve_remote`; `cli_placement.rs`; `transport_host/mod.rs` (`TransportRuntime` and `open_request`); `composition.rs`; `cancellable.rs` entries; `transport_setting.rs`; `backend.rs` (`new`, `with_host_context`); `gitbackend.rs::with_transport`; the fork's `RemoteCallbacks::smart_transport`; libgit2 `remote.c` `apply_insteadof` call sites (`:239–255` create, `:509–533` lookup).
- **Read, relevant sections:** split design §3.4–§3.8, §4.3–§4.6, line 82; plan §4 steps 1.7 and 2.3, and 2.5's R7 text; contract §4 and the 2.2c §15 row (`snapshot()` refusal); placement design §2 (`gwz-core/dev-docs/history/reference/GwzRemoteTransportPlacementDesign.md:89–98`); the brief; the drafter report; the drafter's red, green and mutation logs (red-2 shows 5 failures, green-final shows 193 passing, M1–M6 killed, M7 survives).
- **Runs**, all with `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p1-7-review-code` and `TMPDIR=/Volumes/gwztmp`:
  - **Lane, candidate shape:** `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --lib --locked -- driver_only route_tests` gave 17 passed, 0 failed (`lane-candidate.log`).
  - **Python:** `python3.13 -B -m unittest scripts/checks/test_check_driver_only_boundary.py` gave 13 OK. `check_driver_only_boundary.py` printed ok and `check_spawn_gate_minting.py` printed ok.
  - **Reviewer probe, outside the lane:** a copy at `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/probe/gwz-core`, with sibling paths pointed at the lane's members. It adds `probe_insteadof_rewrite_bypasses_the_route_refusal` to `driver_only_tests.rs` and `probe_leak` to `driver_only.rs`. Same command with the filter `probe_ --nocapture`: it compiled and ran (log `probe-candidate.log`).
  - **Check counterexamples:** two modified copies of `driver_only.rs` under `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/chk/{v1,v2}`, run through `check_driver_only_boundary.py --root`. Both printed ok.
- **Not run:**
  - The ordinary-shape tests. The drafter ran only a type check (`check-ordinary.log`). The only ordinary-shape behaviour change is the reorder in `read_remote_file`, read in §2.
  - Windows and the WSL matrix, which are out of scope.

## Problem, requirement provenance, and scope

- **Requested outcome** (plan §4 step 1.7; brief deliverables 1–3; split §4.3 and §4.6):
  - a core-side builder with no endpoint, `Handoff`, agent, key or native gate;
  - every request is the `cli` placement;
  - `file`, local paths, the clone family, `http://` and `git://` are refused by name before any effect;
  - the native route is refused however it is selected, with zero agent or key access.
- **Authority for the behaviour beyond the builder:** placement design §2 says "Local filesystem/family routes, http:// and git:// are local-only", and "Explicit cli requires … supported routes before any operation mutation, key/helper access or Git-host network effect. Failure never falls back."
  - This authorises the refusals in `identity.rs` and `transport.rs` for every `cli` context, not only driver-only hosts. That includes the in-process `TransportRuntime::install_cli`, so the clone-family refusal there is in scope.
- **Shared-backend changes:** every new refusal is gated on `backend.ssh.is_cli_context()`. Local and native placements change in only two ways:
  - **Unchanged refusal set.** `scp_like` is extracted with the same expression. Under `cli`, `local_only_route` refuses exactly what the old `!ssh_scheme && !scp && !https_scheme` test refused; only the words change.
  - **Reorder in `read_remote_file`:** the route and identity check now runs before the scratch tempdir is made. For local placement this only decides which error wins when both would fail.
  - No retained reader matches the old message ("native transport fallback is disabled") outside archived evidence trees in gwz-core-evidence.
- **File ownership:** the edits are within the brief's owned set.
- **Deferrals respected:** the §4.5 request-type table, served `transport_capabilities`, and HostHandle routing are not touched. The builder's shape does not force any of them.

## 1. Findings

### P2-1 — The route refusal checks the caller's URL, but libgit2 connects to the URL after `url.<base>.insteadOf` rewriting. Under `cli`, `ls_remote_url` serves `file://` natively and opens core-side `http://` connections.

- **Location:**
  - `src/git/gitbackend/transport.rs:180–184` (`ls_remote_url`): `repo.remote_anonymous(url)`, then `identity::for_remote(…, url)` on the raw `url`.
  - `transport.rs:203–213` (`advertised_refs`): reads `remote_handle.url()`, which is the rewritten URL, and passes it to `remote_callbacks` and then `transport_binding::configure`.
  - For a URL that is neither SSH nor HTTPS, `configure` (`transport_binding.rs:279–290`) installs nothing, so libgit2's native transport runs.
- **Same root cause on other entries:**
  - `clone_repo_named` (`:45`: check on raw `url`; `repo.remote(remote, url)` in the `remote_create` callback applies insteadOf, libgit2 `remote.c:253–255`).
  - `read_remote_file` (`:685`).
  - `push_prepared`'s anonymous remote (`:418–420` against the check at `:443`).
  - These three pass `configure` the raw URL, so libgit2 uses the custom transport chosen for the raw scheme. They fail closed by accident, but still after making the target or temp directory, or after sending an `Open` with the rewritten destination. The check is not judging the route that runs.
  - By contrast, `fetch`, `tag_fetch`, `ls_remote` and `push` check `remote_handle.url()` or `pushurl()` after lookup, which is post-rewrite, and are correct.
- **Violated invariant:**
  - plan 1.7 ("local-only routes … are refused before any effect");
  - split §4.6 "Routes";
  - split §3.5's last line: a destination rewritten by the core's insteadOf must still only be an `Open` the client admits;
  - placement design §2 ("Failure never falls back"; no "Git-host network effect" or "key/helper access");
  - plan 2.5 R7 ("the core's `connect()` count is 0").
  - The drafter's proposed §15 row states the opposite ("refuses … before any effect").
- **Reproduction** (reviewer probe, candidate shape, on a `DriverOnly` request harness, lane code unchanged):
  - **Setup:** a receiver repository whose config has `url.<file:///…/source>.insteadOf = https://example.invalid/r.git` and `url.http://127.0.0.1:<port>/h.insteadOf = https://example.invalid/h`.
  - **Control:** `backend.fetch(receiver, "origin")`, where origin is `https://example.invalid/r.git`, returns `Err(UnsupportedOperation, "the cli placement does not serve the file:// route …")`.
  - **`file://` rewrite:** `backend.ls_remote_url(receiver, "https://example.invalid/r.git", "origin", None)` returns `Ok([HEAD, refs/heads/main → febe06e2…])`. The source repository was read through libgit2's native local transport under `cli`.
  - **`http://` rewrite:** `backend.ls_remote_url(receiver, "https://example.invalid/h.git", …)` returns `Err(GitCommandFailed, "unexpected EOF")` after 1 core-side TCP connection to the http listener.
- **Impact:**
  - A served core (`gwz server`, which 2.3 builds on this builder), or any in-process `cli` host, reads a local repository the client never admitted. It also makes core-side Git-host connections over plain HTTP.
  - **By code reading,** a 401 on that native route reaches `credential_callback` → `remote_credential`. With `CredentialHelperPolicy::AllowConfigured`, the default from `Git2Backend::new()`, that is a credential-helper spawn on the core on Unix. On Windows, a Negotiate challenge gets `Cred::default()`, which is the core user's logon session. This is the "key/helper access" placement §2 forbids.
  - **Production reach:** `publication.rs:61/251/324/739`, `handle_tag.rs:169` (tag push and delete, push class), and `push_plan.rs:33/80` (the push preflight).
  - In the push preflight the URL is already rewritten once, and the anonymous remote rewrites it again, so a two-rule chain is enough.
  - The trigger is ordinary user git configuration, either the core's global file or a member's `.git/config`, which split §3.5 assigns to the core.
- **Required correction:**
  - Apply the route check to the URL libgit2 will connect to, before any effect, and pass that same URL to `configure`:
    - in `ls_remote_url` and `push_prepared`, check the anonymous remote's `url()` (fetch) or `pushurl().unwrap_or(url())` (push) after creating it, which has no effect;
    - in `clone_repo_named` and `read_remote_file`, compute the effective URL before the target or temp directory exists. One option is to apply the configuration's insteadOf rules first. Another is to check in the `remote_create` callback and rely on libgit2 removing a target it created; state which.
  - The finding is about the check's input, not its classifier. `local_only_route` itself is correct.
  - The mechanism predates 1.7, but 1.7 is the step that claims this route refusal is closed, and its §15 row records that claim.
- **Closure test:**
  - A `DriverOnly` row per URL-taking entry (`ls_remote_url`, `push_prepared` through an anonymous remote, `clone_repo`, `read_remote_file`) with a repository-level, and for clone a global-snapshot, insteadOf rule mapping an `https://` URL to `file://` and to a loopback `http://` listener.
  - Each must give the named refusal, zero listener connections, and no target, temp directory or ref.
  - The reviewer probe above is a ready template.

### P3-1 — `check_driver_only_boundary.py` and its unit test run in no CI job and in no `run_tests.py` gate

- **Location:**
  - `scripts/run_tests.py:488–501` (`source_checks` lists every check explicitly; the new one is absent);
  - `.github/workflows/checked-artifact-boundary.yml:110–137` (the explicit check list and unittest list; absent);
  - `scripts/checks/run_boundary_job.py` replays only the workflow's own lines.
- **Violated invariant:** the plan's type-level red row has to be a gate. The workflow comment says "a module nobody adds here is a module CI never runs". The proposed §15 row says the script "holds that over `driver_only.rs`".
- **Reproduction:** grep for `check_driver_only` in `run_tests.py` and `.github/workflows/` finds nothing. Only the operator's `remote-gates.zsh` (lines 344–345, a `check_*.py` glob outside the repository) runs the check. Nothing runs its unit test.
- **Impact:** a later edit that names `EndpointSettings` or `Handoff` in `driver_only.rs` merges with green CI.
- **Correction:** add the check to `run_tests.py` `source_checks` and to the workflow step. Add `test_check_driver_only_boundary.py` to the workflow's unittest list, beside `check_carrier_peer_rule.py`.
  - The drafter attributes `run_tests.py` to the gate-fixes lane. If that ownership holds, the lane owner must sequence it, but the row is not closed until it is wired.
- **Closure test:** `run_boundary_job.py` shows the check and its test running. A seeded `use super::Handoff;` fails both `run_tests.py`'s source checks and the workflow step.

### P3-2 — The boundary check's deny-list omits the entries that build a full local endpoint from inputs the module already holds

- **Location:** `scripts/checks/check_driver_only_boundary.py` `FORBIDDEN_TYPES`, `FORBIDDEN_MODULES` and `FORBIDDEN_CALLS`. The `Session` calls are matched by spelling (`Session\s*::`).
- **Violated invariant:** brief deliverable 1: no way to construct a local endpoint, agent or key configuration from the builder's module. The check is the only enforcement of the type-level row (drafter decision b).
- **Reproduction:**
  - **(v1)** Appending this to `driver_only.rs` compiles in the probe build and passes the check:

    ```rust
    fn probe_leak(environment: &EnvironmentSnapshot, token: &CancellationToken, meta: RequestMeta) {
        let _ = super::with_cancellable_local_transport(meta, "fetch".into(), environment, token, |_backend| ());
    }
    ```

    `with_cancellable_local_transport` (`cancellable.rs:40`) builds an SSH and HTTPS `TransportRuntime` from the snapshot, with agent and helpers (`cancellable.rs:222–235`).
  - **(v2)** `use super::session::Session as Host; … Host::endpoint(…)` also passes the check (alias). I tested it only against the check and did not compile it.
  - `build()` takes the full `EnvironmentSnapshot`, which includes `SSH_AUTH_SOCK` and the credential variables. That is exactly the input those entries consume.
- **Impact:** the guard enforces spelling, not the invariant, so the type-level row can regress unnoticed even once P3-1 is wired.
- **Correction:**
  - Forbid the local-transport entry points: `with_cancellable_local_transport*`, `with_local_transport*`, `TransportRuntime`, and `cancellable` or `local_command` paths.
  - Forbid `as` aliases of `Session`, or switch to an allow-list of the names the module may import.
  - Optionally narrow `build`'s input to what `transport_setting::resolve` needs (the resolved `Setting`, or HOME, XDG_CONFIG_HOME and GWZ_TRANSPORT), so the builder's type never receives agent configuration.
- **Closure test:** unit rows for v1 and v2 that fail the check.

## 2. Invariant analysis

**Attacks that failed:**

- **Reaching endpoint configuration through the public API.**
  - `DriverOnlyBuilder` has private fields and derives only `Clone`, `Copy` and `Debug`.
  - `DriverOnly(Arc<Mutex<State>>)` has a private field and no `Deref`, `From` or generics.
  - `TransportPort` is opaque (`next_message`, `deliver`). `TransportRequest` carries a backend with a `cli` host context; `with_transport` keeps `ssh` (`gitbackend.rs:228–238`).
  - No path reaches `EndpointSettings`, `Handoff`, `ProcessSafety` or agent settings, apart from the module-internal leak in P3-2.
  - The module is candidate-only through `lib.rs:47–51`.
- **Native refusal.**
  - **Selection forms:** the flag, `GWZ_TRANSPORT` (case and padding), and `gwz.transport` in the XDG file and `~/.gitconfig` including includes are all resolved by `transport_setting::resolve` before `Session::driver`, with the form named.
  - **Precedence:** a malformed form below a decisive one is not read (tested). The default is never native, so the `Source::Default` arm is unreachable but required by exhaustiveness. A repository-level `gwz.transport` never selects (off-switch design §3).
  - **Agent and key access:** none on this path. `resolve` reads only the snapshot and global config files, and the refusal text carries no canary (tested).
  - **Per-request native route:** none exists inside `DriverOnly`. Its backends always carry a `cli` context. The native fallthrough in `configure`, including Windows `serves_http_natively` and therefore SSPI, is reachable only through P2-1's rewritten URL.
  - **HTTPS on Windows** goes through `HostHttpsRoute` to the client.
- **Route classifier against libgit2's transport choice** (`local_only_route`, `scp_like`):
  - `file:/x` without `//` is SCP-like for both gwz and libgit2 (host `file`) and goes to the custom SSH transport.
  - `C:\…` and `C:/…` are a local path (one-letter host). UNC paths are a local path.
  - Uppercase schemes are covered.
  - A scheme with a space or more than 16 characters is named `unrecognised`.
  - The refusal text repeats no part of the URL (tested).
  - The only gap is that the classifier is fed the pre-rewrite URL (P2-1).
- **Call graph of URL-taking and clone-family entries.**
  - I enumerated these from the `gitbackend.rs` delegates (`:279–286`, `:380–386`) and the libgit2 call sites. `remote_anonymous`, `RepoBuilder` and `connect_auth` appear only in `transport.rs`, and `gwz-git` and the crates do no transport.
  - Every entry reaches the check before connecting. `fetch_anonymous` and `push_anonymous` refuse before `admitted_local_peer`.
  - Before the check: `open_repo`, `find_remote`, `ensure_clone_target_is_empty` and the in-memory `remote_anonymous` are reads only.
  - The identity cache in `for_remote` inserts only after `resolve_remote` has passed the check.
- **Placement.**
  - `require_cli` refuses `None`, an unset placement and `Some(Local)` before any session work.
  - Rewriting instead of refusing would break `RequestContext::validate`'s meta equality (`request.rs:96`), and split line 82 has the client send `placement = cli`. Drafter decision (e) is correct.
  - `request_with_token` shares `open`.
  - The ordering `validate_meta`, `require_cli`, `refuse_budget_for_cli`, `begin`, `ready` matches `TransportRuntime::open_request`. Shutdown and Drop mirror `RuntimeState`.
- **Drafter decision (a), no HostHandle.**
  - Split §3.4 builds `remote::handle(binding)` per session from the lane binding, so a build-time handle has nothing to bind to.
  - The 2.2c §15 row already records `snapshot()` as a refusal on the carrier-backed handle, and plan 2.2c lists it as a red row. Recording rather than implementing it is within the brief's stated alternative.
  - One host serves one client endpoint (a single `TransportPort`), so 2.3 builds one `DriverOnly` per carrier session. That is cheap, and the setting is re-resolved per session.
- **Drafter decision (c), no agent recording double.**
  - Acceptable for the builder path, which reads no agent by construction.
  - The double would not have caught P2-1. Only a route-level probe like the one above does, and that is what the closure test asks for.
- **Drafter decision (d), explicit timeouts.**
  - Accepted. The values feed only `Session::driver`, as `install_cli` does.
  - The builder reads no global. Each request's `Git2Backend::new()` still calls `ensure_server_timeout()` and `process_environment()` (§4).
- **Surviving mutation M7** (the tempdir before the route check): this ordering is established by reading the code, and holds at `:684–686`.
- **Unit and fake integrity:** the harness uses an empty snapshot (no developer HOME), a real in-process `CliEndpoint` over a carrier link, and a loopback listener for zero connections. The rows exercise production code.

## 3. Risks and next action

- **Next action:** remediate P2-1. Move the check onto the effective URL for all four entries and add the insteadOf rows. That closes NO-GO to GO under the pre-commit.
- **P3s:** wire P3-1, the CI and `run_tests.py` entries, coordinated with the gate-fixes lane if it owns `run_tests.py`. Widen the deny-list or switch to an allow-list (P3-2).
- **Correct the proposed §15 row** before recording it. Its "before any effect" sentence is true only after P2-1 is fixed. Its "holds" sentence is true only after P3-1 is fixed.
- **Residual risk after remediation:** a destination rewritten to another SSH or HTTPS host is still sent as an `Open`. That is the design (split §3.5), with client admission (2.3) as the control.
- **Pending gates:** the Windows receipt (`p1-7-20261030-a`) and the remote gate are still queued. The native-refusal rows and P2-1's new rows need the Windows run.

## 4. Out-of-scope observations (follow-up proposals; not findings)

1. **Process globals in the request path.**
   - **Observation:** `Git2Backend::new()` (`backend.rs:58–70`), which every request uses including driver-only ones, calls `ensure_server_timeout()`. That sets libgit2's process-wide server timeout and freezes `TIMEOUT_STATE`. It also calls `process_environment()`, which reads `std::env::vars_os()`.
   - **Impact:** in a long-lived `gwz server`, the first request freezes the timeout state, and the backend's Git environment is the server's process environment rather than a snapshot.
   - **Status:** pre-existing and identical in `TransportRuntime`. A limitation, not a confirmed defect. It does not block 1.7. Proposal: phase 3 composition passes both explicitly.
2. **The drafter's outside findings 2–5.**
   - **Status:** confirmed by reading.
   - **Details:**
     - The `configure` fallthrough is now reachable through P2-1.
     - `transport_capabilities` for a served core belongs to 2.3.
     - `warn_once` with `notes = None` goes to stderr, but its only caller (`transport_binding.rs:284`) is unreachable under `cli` once P2-1 is fixed.
     - `TIMEOUT_STATE` is covered by item 1.
3. **`clone_repo_named` raw-URL mismatch outside `cli`.**
   - **Observation:** it hands `configure` the raw URL while libgit2 connects to the rewritten one. With the common rule `url."git@github.com:".insteadOf https://github.com/`, a local-placement clone of an `https://` URL installs the HTTPS custom transport for an SCP destination.
   - **Status:** pre-existing. A hypothesis for ordinary gwz, not tested here. It shares P2-1's root cause, and P2-1's fix would address it.
