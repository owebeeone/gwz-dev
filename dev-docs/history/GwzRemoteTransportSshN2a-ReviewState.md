# SSH N2a selected-key snapshots and native authentication — State-axis review

**Review object:** N2a implementation at `gwz-core` `42f90094dae33eef1cd4828d2c97cadda2761f43`, compared with `9f35c29489a6af7f35c8fd44fec11cc03a1bceee`; committed three-file implementation, two isolated test binaries, fixture dependency pin, checkpoint, and private evidence. No production routing activation.  
**Baseline:** root `c33005d576e59ef46222e465285ae1eccf7ceedd`; `gwz-core` `42f90094dae33eef1cd4828d2c97cadda2761f43`; `gwz-core-evidence` `b5375aceb57773cf79ba25b443cafa2ada38bc08`; `gwz-transport` `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects. Start and end checks matched this tuple; only the two permitted generated N2a prompts were untracked at root.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics, ownership, cancellation, races, cleanup, and fail-closed transitions. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1, or P2 findings. One bounded P3 finding remains.

---

## 0. Evidence base

I inspected:

- `GwzRemoteTransportSshN2a.md` and the N2a portions of `GwzRemoteTransportSshSelectedIdentityDesign.md`, `GwzRemoteTransportSshProductionSetup.md`, and `GwzRemoteTransportRequirements.md`.
- The complete `9f35c294..42f90094` source delta, especially:
  - `src/git/endpoint/ssh_key_snapshot.rs`
  - `src/git/endpoint/ssh_key_container.rs`
  - `src/git/endpoint/ssh_key_auth.rs`
  - `tests/transport_ssh/tests/selected_key.rs`
  - `tests/transport_ssh/tests/key_container.rs`
  - the isolated manifest and lockfile.
- The pinned libssh2 memory-key authentication and OpenSSH/PEM parsing paths used by `libssh2-sys` 0.3.3.
- Private evidence README, `sources.json`, remediation logs, and final aggregate output at evidence commit `b5375ace`. All seven recorded source hashes match the final committed blobs. The archived aggregate run reports 107 passing executions and one intentional child-only ignore.
- The permitted focused Rust 1.95 locked/offline run. All five `key_container` and eleven `selected_key` tests passed.

The production/test additions are 553/603 lines, matching the recorded scope.

## 1. Findings

### [P3-1] Armor discovery can scan a maximum-size line between cancellation checkpoints

**Location:** `ssh_key_container.rs:26-39` and `203-235`, especially the unchecked NUL scan, leading-whitespace loop, and `line()` newline search.

The accepted design requires Control checks between bounded classifier chunks. Base64 decoding and trailing-whitespace validation use 128-byte chunks, but armor discovery calls `line()`, whose `.position()` may scan nearly the complete 1 MiB input before another check. Leading whitespace and the initial NUL search have the same shape.

A concrete input is a valid BEGIN line followed by nearly 1 MiB of base64 on one physical line. If cancellation occurs just after the check at line 221, discovery scans that entire line before observing it. A maximum-size whitespace prefix similarly runs after the check at line 207. Job arbitration still returns cancellation and retains ownership correctly, so this neither interns stale bytes nor releases capacity early; the consequence is bounded cancellation/cleanup latency that exceeds the promised chunk cadence.

The current cancellation test uses a 700,000-byte payload on one line but signals cancellation immediately after worker entry. It can pass by observing cancellation before the long line scan and therefore does not prove a checkpoint within that scan.

Correct this by making NUL, outer-whitespace, and newline discovery use a fixed checked chunk cadence. Add a deterministic test hook or checked scanner test that cancels after entry into the first chunk of a near-cap single-line input and proves later chunks are not traversed before cancellation is returned.

## 2. Invariant analysis

Reservation and ownership attacks held. Each read reserves one slot and `1 MiB + 1 + 256` bytes before helper creation or file I/O. Full logical capacity remains charged through `Loaded`, interned `Entry`, native authentication, late-result disposal, and final owner drop. The 16 MiB ledger admits only 15 distinct maximum buffers. Secret text drops before its reservation; matching admissions discard their new allocation only after pinning the existing entry. No registry lock spans file I/O, native work, waiting, or a last-entry destructor.

Interning is atomic under the registry mutex. Exact bytes and exact `Key` share one candidate even under concurrent admission; changed bytes or a different key receive a distinct monotonically increasing token. Weak entries cannot retain secrets or resurrect disposed tokens. Counter exhaustion refuses rather than recycling identity.

Container admission rejects multiple/mismatched armor, auxiliary headers, invalid trailing data, malformed base64, encrypted PKCS#8, and encrypted OpenSSH framing before native authentication. DER and SSH lengths are checked against decoded bounds and fixed lookahead. Decoding retains only a 128-byte prefix plus small fixed arrays; it creates no complete decoded-key copy. Native parsing remains the stated usability authority. P3-1 is the sole gap in the classifier’s checkpoint cadence.

Authentication rechecks host trust before credential use, supplies only the immutable entry bytes, retries only libssh2 EAGAIN under the original Control, and maps terminal native failures without trying an agent, another file, address, Git path, or credential. `Verified` drops the connection before the snapshot pin. Publication rechecks original-request liveness and native authenticated state before promotion; cancellation or failed handoff leaves the entry unproven.

The isolated status is truthful. N2a provides usable snapshot/authentication seams without attaching a production route. N2b still owns admission-before-pool integration, resource pinning, combined cleanup, and the capacity-one first-fan-out proof.

## 3. Risks and next action

The remaining limitations are explicit: native parsing/signing of admitted unencrypted material lacks hard physical preemption; platform and selected-source qualification, N2b lifecycle composition, N3 attachment, and activation remain deferred.

Proceed to N2b while carrying P3-1 as a bounded correction and regression requirement before activation.
