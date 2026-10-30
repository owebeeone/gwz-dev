# Step 1.7 driver-only construction — remediation plan, round 1

Object: gwz-core `edfc623e` (lane p1-7). Reviews: `GwzDriverOnly-Step1.7-ReviewCode.md` (NO-GO: P2-1, P3-1, P3-2) and
`GwzDriverOnly-Step1.7-ReviewState.md` (NO-GO: P1-1, P2-1, P3-1). Both reviewers pre-committed to GO on a revision
resolving their blocking IDs as specified. **Blind convergence:** Code P2-1 and State P2-1 are the same root cause
(the route check judges the caller's URL, libgit2 binds the insteadOf-rewritten one), found independently.

One merged patch on top of `edfc623e`; the lane commit is amended (unpushed).

| ID | Disposition | Closure test |
|---|---|---|
| State P1-1 — check and binding classify SCP-like URLs differently (`\` before the first `:` → native libssh2 from the core) | Fix both halves. (i) One classifier: the cli route check is built on the same predicates `transport_binding::configure` binds on (`Destination::parse` + `is_https_remote`); a URL the binding would not bind to the endpoint is refused by name. (ii) Defence in depth: in a cli host context `configure` never returns leaving the native route in place (the `Ok(None)` branch and the Windows `serves_http_natively` branch install a refusing transport or surface an error). | Route-table row: `a\b:c`, `CORP\user@host:r.git`, `[u\x@127.0.0.1:1]:r`, and on Windows `.\x:y`, `\x:y` refused under cli. Harness row like the State probe B (loopback listener recording accepts) for `ls_remote`, `fetch` and `push`: named refusal, zero accepts. A unit or source check that `configure` cannot leave `callbacks.transport` unset in a cli context. The rows run in the Windows receipt. |
| Code P2-1 = State P2-1 — insteadOf rewrite after the check | Fix. Every URL-taking entry checks the URL libgit2 will bind, before any effect: `ls_remote_url` and `push_prepared` check the anonymous remote's `url()` / `pushurl().unwrap_or(url())` after `remote_anonymous`; `clone_repo_named` and `read_remote_file` compute the effective (rewritten) URL before the target/temp directory exists (state the mechanism chosen), and pass that URL to `configure`. | Harness rows per entry (`ls_remote_url`, `push_prepared` via an anonymous remote, `clone_repo`, `read_remote_file`, push prepare with `pushInsteadOf`) with a repo-level (and for clone a global-snapshot) insteadOf rule mapping an `https://` URL to `file://`, to a loopback `http://` listener and to `git://`: named refusal, zero listener accepts, no target, temp dir or ref. |
| State P3-1 — `read_remote_file` tempdir ordering unprotected (M7 survived) | Fix. The row points the scratch root at an empty directory and asserts it is still empty after each refused route. | M7 re-run is killed. |
| Code P3-1 — boundary check wired into no gate | Fix. Add `check_driver_only_boundary.py` to `run_tests.py` `source_checks` and to the boundary workflow's check list, and `test_check_driver_only_boundary.py` to its unittest list. The gate-fixes lane edits the same lists; the lane owner resolves the textual merge. | `run_boundary_job.py`'s workflow replay shows both; a seeded `use super::Handoff;` fails the check. |
| Code P3-2 — check enforces spelling not the invariant | Fix. Switch the scan to an allow-list of the names `driver_only.rs` may import/call (or extend the deny-list to `with_cancellable_local_transport*`, `with_local_transport*`, `TransportRuntime`, `cancellable`, `local_command`, and refuse `as` aliases). Optionally narrow `build`'s input to what `transport_setting::resolve` needs; state whether done. | Unit rows for the reviewer's v1 (`with_cancellable_local_transport`) and v2 (`Session as Host`) fail the check. |

The proposed contract §15 row is corrected after this patch: "before any effect" and "holds" become true only with
P1-1, P2-1 and P3-1 closed. Out-of-scope observations (IPv6 SCP spellings refused as local; transport-helper syntax
sent as SSH; process globals in `Git2Backend::new`; the local-placement `DOMAIN\user` classifier split) go to the
findings file, not this patch, except where P1-1(i) closes them as a side effect.
