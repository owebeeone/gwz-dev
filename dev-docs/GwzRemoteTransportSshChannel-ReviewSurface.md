# SSH Channel — Surface Review

**Date:** 2026-09-20  
**Axis:** Surface  
**Object:** Phase 3c preactivation SSH-channel ownership and cleanup qualification.

**Verdict: GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Exact tuple

| Repository | Commit |
|---|---|
| root | `823ffdf50686bae2a222f80d673e7ad122780684` |
| gwz-core | `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

All commits matched at review start and end. Member working trees remained clean. Root contained only the three authorized current-round reviewer prompts; peer contents were not read.

## Evidence base

Read [the public fixture README](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_ssh/README.md) in full. Read `AGENTS_GWZ.md` and the supplied Surface prompt for process instructions. No implementation, test source, internal design, plan, checkpoint, or peer report was read.

Executed the authorized native gate:

```sh
cargo +1.95.0 test \
  --manifest-path gwz-core/tests/transport_ssh/Cargo.toml \
  --offline --locked --test channel -- --nocapture
```

**Result:** Two passed; zero failed or ignored:

- `upload_pack_advertisement_and_abort_keep_cleanup_explicit`
- `receive_pack_reuses_one_authenticated_connection_and_quotes_repository`

No edits, commits, production activation, or remote-service actions were performed.

## Findings

None. The public contract and exercised walkthrough revealed no concrete surface defect.

## First-day walkthrough and invariant analysis

**Setup and entry point:** The README names Rust, Git, `ssh-keygen`, and the required `/usr/sbin/sshd` location, specifies the workspace-root command, and explains that a missing server fails qualification. This is a test fixture with no separate product CLI or command hierarchy.

**Fixture use and cleanup:** Documentation identifies loopback communication, temporary keys, known-hosts data, repository, and server processes. It states that fixture teardown reaps servers and removes temporary files, while build outputs remain at a named deletable location. The test run passed without skips. Process/file cleanup was not independently inspected beyond the permitted public output.

**Ownership and construction:** `SshChannel::new` takes sole ownership of an authenticated, nonblocking Session. Trust verification must precede authentication, and the caller must retain no Session/Channel clone that can drive the connection. Service selection and caller-owned path resolution are explicit.

**Limits and defaults:** Repository paths must be nonempty UTF-8, at most 16 KiB, without NUL bytes. The primitive adds no timeout defaults, queue, background thread, or transport messages. Cancellation, readiness, and deadlines belong to the host.

**Readiness and errors:** `poll_open`, `WouldBlock`, and the snapshot nature of `block_directions` are explained. Read/write and stderr processing share one worker, use bounded buffers, and require stderr draining despite stdout backpressure. Pending write buffers must remain valid until accepted. EOF, readiness waits, and native errors are distinct; error-source preservation and lack of automatic replay are stated.

**Finish, extraction, and abort:** `send_eof` follows pending writes; both output streams must reach EOF before finishing. `finish` awaits close acknowledgment. `into_session` returns the connection only afterward and otherwise preserves channel ownership in its error. Native failure prevents reuse. Abort rejects further work, while subsequent drop disposes ownership; pool capacity must await disposal. Exit status and early cleanup are not presented as proof of Git success or reusable health.

## Residual risks and next action

This is a public-surface review supported by two local fixture tests, not an implementation audit. It does not establish production trust/authentication, agent behavior, pool/message-pump integration, network-entry activation, publication, or platform parity.

**Next action:** Record Surface GO for this exact tuple. No Surface remediation is required.
