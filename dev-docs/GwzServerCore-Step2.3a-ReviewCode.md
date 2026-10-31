# Step 2.3a (peer sandbox probes): Code-axis review

**Review object:** gwz-core 9261b0ef in lane p2-3a; evidence 2026-10-31-p2-3a
**Baseline:** I read `git -C gwz-core show 9261b0ef` and the whole post-change `peer.rs`, `unix.rs` (peer, hold and `check_peer` sections), `windows.rs` (token, hold, `judge`, accept, connect), `sandbox_rows.rs`, `local_rows.rs` helpers, the guard and its unit tests, the parity JSON and meta.json. I read the controlling documents at the lane root. I built and ran the rows in an `rsync -aL` copy (`build-targets/review-p2-3a-code-src`, with its own target dirs), on the Mac and in my own Linux image `review-p2-3a-code-img` (privileged, root, OrbStack aarch64, kernel 7.2.12). I mutated only that copy. The copy, the target dirs, the image tag and the volume are now removed.
**Date:** 2026-10-31
**Axis:** Code. Architecture, interfaces, call graphs, compatibility of the peer sandbox rule's probes on all three platforms.
**Verdict: GO.** 0 P0, 0 P1, 0 P2, 4 P3. This depends on Windows receipt `p2-3a-20261031-a`, which the commit's trailer names but which does not exist yet (see §0). The Windows arm has never been compiled as a whole or run. A red receipt reopens this verdict.

## 0. Evidence base

- **Tuple.** Lane root 32ad237a and gwz-core HEAD f2b1bc71 (a merge whose first parent is 9261b0ef), at the start and at the end. gwz-core had a clean working tree both times. `git diff 9261b0ef f2b1bc71` touches none of the object's files (`spawn_gate*`, `https_tests.rs`, the pin, the clippy allow-list), so the object holds at HEAD unchanged.
- **Mac (my copy, at HEAD):** `cargo test --lib sandbox`, candidate shape, `TMPDIR=/Volumes/gwztmp`: 8 of 8 pass.
- **Linux (my container, root, privileged):** the same command: 11 of 11 pass. That includes the seccomp, user-namespace and mount-namespace children as client, as listener and as the caller.
- **Guard:** `check_carrier_peer_rule.py` passes, and its unit tests pass (18 of 18).
- **My mutations (copy only):**

| Mutation | Result | Meaning |
| --- | --- | --- |
| M-a: `if cfg!(debug_assertions) { return Ok(SandboxFindings::default()); }` at the top of `linux::probe_dir` | Passes `check_carrier_peer_rule`, `check_cfg_boundaries`, `check_process_globals` and `check_candidate_switches` | Finding C-1 |
| M-b: `#[cfg_attr(debug_assertions, path = …)]` on `mod linux` | The peer guard passes it; `check_cfg_boundaries` catches it (NEW) | Covered by the other gate |
| M-c: `macos::of_peer` asks by `Subject::Pid(hold.pid())` instead of the token | Mac rows 8 of 8 still pass | Finding C-2 |
| M-d: `check_peer` runs `hold.confirm` before `of_peer` | Linux rows 11 of 11 still pass; the guard passes too | Finding C-2 |

- **Drafter evidence:** the README and `raw/` hold 17 red mutation runs (Mac and Linux) and the green logs. Windows was not run. Only the FFI part of the AppContainer fixture was type-checked, standing alone (`raw/windows-ffi-typecheck.log`).
- **Windows receipt:** `build-targets/p2-3a-receipts` does not exist. `p2-3a-logs/remote-a.log` shows the lane owner's remote gates still running when I read it (`run_tests` START at 02:15:56Z). I have no WSL or Windows results.
- **Windows arm, read by hand.** I checked every call in the arm against windows-sys 0.61.2 in the registry: `CreateProcessW`, `InitializeProcThreadAttributeList`, `UpdateProcThreadAttribute`, `DeleteProcThreadAttributeList`, `TerminateProcess`, `STARTUPINFOEXW`, `SECURITY_CAPABILITIES`, `PROCESS_INFORMATION`, `HANDLE = *mut c_void`, `PSID`, and the flag constants. The features are present: `Win32_Security` and `Win32_System_Threading` are in the production list. The rest uses crate items, all reachable on Windows: `judge` and `ProcessHold` (re-exported under `all(test, windows)`), `ProcessHold.identity`, `try_from_identity`, `PeerRefusal: PartialEq + Debug`, `LocalRefusal: Debug`. The shared rows' helpers (`dir`, `dir_runtime`, `listener`, `connected`, `accepted`, `eventually`, `decode_session_error`) exist on Windows, and `LocalListener::bind(&RuntimeDir, …)` has the same signature there. I found no compile error and no unused import.
- **Contract:** §15 has no "step 2.1" row in this lane or on root main (`grep 'Recorded at step 2.1'` gives 0). The object adds no public item; every new item is `pub(crate)`. So no contract row is owed. Row 2.2c is unaffected.

## Problem, requirement provenance, and scope

- **Requested outcome.** The parked server design §4 (L257-296) and split doc §4.3 "Peers" say the sandbox rule applies in both directions. The convergence plan's carried paragraph under 2.3 makes 2.3 own "the sandbox rule's platform checks". Plan 2.3a and the brief (§5) set the deliverables:
  - the Linux seccomp and user/mount-namespace probe;
  - the macOS `sandbox_check_by_audit_token`/`sandbox_check` probe through `dlsym`, with an injectable resolver;
  - the closed `Cause` list;
  - `current_user` refusing a sandboxed caller;
  - the guard extension;
  - real-process rows.

  Each mechanism in the object traces to one of these items.
- **Necessity.**
  - Putting `SandboxFindings` inside `PeerIdentity` is required by the brief ("`verify` stays a pure function of identities"), and it is the right design. It lets one decision table cover all three platforms.
  - Splitting `from_own` out of `current_user` is needed so that the test constructor `try_from_identity` can apply the caller rule to a hand-built identity. It is test-only, sits under `cfg(test)` in `peer.rs`, and the guard forbids naming it elsewhere (`\w*from_identity`).
  - The `dlsym` resolver is a function parameter. It is not a build switch, and production passes only `dlsym_default`.
- **Scope.** The edits stay inside the brief's file list with one exception, `windows.rs`. The brief allows edits there "only for a gap the Windows row finds". The `sandbox:` field was forced by the struct literal. Making `judge` and `ProcessHold.identity` `pub(crate)` serves the row, not a gap (C-4).
- **The drafter's notes:**
  - *Budget:* the implementation is about 310 lines against the plan's ~350, and the rows are 824 against ~350. The brief said "report first if it cannot fit", but the drafter reported after. This is a process item for the lane owner, not a defect. The rows file is under the 1000-line split threshold.
  - *Windows row partly type-checked:* see §0. Reading it found no compile error or semantic fault, but it stays unexecuted until the receipt.
  - *Blind spots (Landlock, LSM, cgroup/capabilities, seccomp on a non-first thread, macOS undocumented):* these are limitations, not defects.
    - The design defines the rule by exactly `/proc/<pid>/status` `Seccomp`, the `ns/user` inode and the mount/user namespace comparison.
    - It names Landlock explicitly as `server_unavailable` territory.
    - It marks the macOS answer "inferred".
    - A seccomp filter on one thread without TSYNC is not a process boundary, since that thread shares the address space of an unfiltered thread.
    - One documentation gap: the README says only for macOS that a sandbox entered after the check is not seen. The same holds on Linux (seccomp or `unshare` after connect).
  - *Pipe DACL keeps AppContainer clients out:* this follows from Windows access-check semantics. An AppContainer token also needs an ACE for its package SID or ALL APPLICATION PACKAGES, and the pipe DACL grants only the user SID. The row reaches the rule through `judge`, the single function behind both `accept` (windows.rs:817) and `connect` (:874), so the "client and listener" relations exercise the true path. The claim that an AppContainer process cannot create a pipe in the global namespace is unverified here. It does not matter, because `judge` refuses any AppContainer pid in either relation.

## 1. Findings

### C-1 (P3). The peer guard does not see the `cfg!(...)` expression switch

- **Root cause.** The guard's switch scan matches `cfg\s*\(` (`check_carrier_peer_rule.py:136`). `cfg!(` does not match, and `check_cfg_boundaries` does not flag it either. The regex is from 2.1. The 2.3a extension applies it to the new probe files and claims they "may switch on the platform only as sandbox.rs lists".
- **Location:** `scripts/checks/check_carrier_peer_rule.py:136`.
- **Violated invariant.** Split doc §4.3 says "no test-only override exists in production code (2.1 source check)". The guard's docstring says "no build variant has a different rule". Brief item 4 says the guard "still means what it says … add a test that a bypass spelling still fails".
- **Reproduction:** mutation M-a in §0. A debug-only, err-open Linux probe passes all four source guards. The rows would not catch it either, because they assert refusals under the debug profile… [correction: they run in the test profile, where `debug_assertions` is on, so the rows *would* go red on this specific spelling. A `cfg!(not(test))` or feature-keyed spelling would still pass the guard silently.]
- **Impact.** No current defect. It is a hole in the enforcement tool that guards the peer rule against later edits.
- **Correction.** Also scan `\bcfg!\s*\(` and apply the same allow-list. Add a unit test with `if cfg!(debug_assertions) { … }` and `cfg!(not(test))` in `local/sandbox/linux.rs`.
- **Closure test:** M-a fails the guard.

### C-2 (P3). No row proves that the probe is bound to the hold

- **Root cause.** The pinning properties are true in the code but carried by statement order and by a literal, not by a type or a row. The two properties are:
  - the macOS token form is what reaches the peer;
  - the Linux and macOS probe runs before `hold.confirm`.
- **Locations:** `sandbox/macos.rs:of_peer` and `unix.rs:check_peer` (`peer.sandbox = …; verify; confirm`).
- **Violated invariant.** Design §4: "A check reads a process only through a hold on it." Plan 2.3a: "each probe mutation-red".
- **Reproduction:** mutations M-c and M-d (§0). Both keep every row green, and M-d also passes the guard. Under M-c, if the macOS peer exits and its pid is reused between `Hold::take` and the probe, the probe judges another process. `confirm` checks only the token on the socket and `process_alive(pid)`, and a reused pid passes both. Under M-d, the Linux probe reads `/proc/<pid>` after the bracket closed.
- **Impact.** Only a later edit could open these. The race window is narrow. The current code is correct.
- **Correction:**
  - Make `Hold::confirm(self, fd)` consume the hold, so that a probe taking `&Hold` cannot follow it. The compiler then enforces the order.
  - Split `of_peer` into `of_peer_with(resolve, hold)`, plus a resolver row that records which symbol was called and asserts the hold's token reached it.
- **Closure test:** M-c is red in the resolver row, and M-d fails to compile.

### C-3 (P3). A refused caller is told "the peer" is sandboxed

- **Root cause.** The caller's own refusal reuses the peer-subject texts.
  - `current_user` returns `LocalRefusal::Peer(PeerRefusal::Sandboxed(c))`, which displays as "the peer was refused: the peer is sandboxed by a seccomp filter".
  - The Linux probe's `Unreadable` texts for `of_self` say "the peer's process status" and "the peer's user namespace".
- **Locations:** `peer.rs` Display, `sandbox/linux.rs:probe_dir`, `local/mod.rs:141`.
- **Violated invariant.** Design §4 "Sandboxed callers": the refusal names the cause, here of *this* process. There is no peer: the refusal comes before any connection or start.
- **Reproduction:** `own_is_refused` prints `Err(Peer(Sandboxed(Seccomp)))`. Its Display is the peer text above. A user in a Docker container running `server start` would read that a peer is sandboxed.
- **Impact.** Misleading diagnosis. It is latent today because nothing in production calls `current_user` yet (`grep` finds only tests). It becomes user-visible when 2.3d or 2.3k wire it in.
- **Correction.** Keep the type the brief mandates, but give `Sandboxed` and `Unreadable` texts without a subject ("sandboxed by …", "could not read the process status"). Alternatively, let `current_user` wrap its refusal with "this process".
- **Closure test:** a row asserting that `current_user`'s Display under seccomp does not contain "peer".

### C-4 (P3). `windows.rs` visibility was widened for the row

- **Root cause.** The row reads the hold's identity directly, instead of through `judge` or a test-only accessor.
- **Location:** `windows.rs` (`pub(crate) fn judge`, `pub(crate) identity` on `ProcessHold`).
- **Violated invariant.** The brief allows `windows.rs` edits "only for a gap the Windows row finds". The identity read through the hold also becomes mutable crate-wide.
- **Impact.** It is encapsulation only. `judge` is still the sole `verify` call site on Windows.
- **Correction.** Keep `judge` re-exported only under `all(test, windows)`, as it is now. Replace the `pub(crate)` field with a `#[cfg(test)] fn identity(&self) -> &PeerIdentity`, which the guard allows in `windows.rs`.
- **Closure test:** the guard and the Windows receipt are green with the field private.

## 2. Invariant analysis

- **The probe is on every path.**
  - Linux and macOS: `check_peer` is the only builder of a checked peer identity. It is called by `accept` (unix.rs:582, `ListenerChecksClient`) and `connect` (:621, `ClientChecksListener`), and in both it runs `of_peer(&hold)` before `verify`.
  - `current_user` is the only production constructor of a policy (`from_identity` and `try_from_identity` are `cfg(test)`), and it always runs `of_self`. Any future connect or start must therefore probe the caller.
  - Windows: `judge` is called by accept and connect. The AppContainer and integrity flags come from the hold's token, and `of_self` returns "nothing found", which is correct by the design's Windows rule.
  - The guard requires the `of_peer(` and `of_self(` calls to be present, not used (stated as a token scan).
- **Absolute rule.** `verify` refuses if either side has a cause (`.or(self.own…)`), and then if the namespaces differ. A namespace read on one side only counts as different. This matches design L273-282, including "siblings with equal seccomp filters". A user-namespace inequality alone can never decide, because a non-initial user namespace is already a cause on its own. That is harmless redundancy.
- **Closed `Cause` list.** There are four variants, each with a distinct static text containing no digit and no `/`, and the rows assert all of this. The wire answer is `server_peer_refused` (82) with call_id 0, then EOF, and nothing is read first. A refusing client sends nothing: the listener child's `received:` is empty.
- **Errs closed.**
  - Linux: an unreadable status, `ns/user` or `ns/mnt`, a missing or non-numeric `Seccomp:` line, or a reaped pid all give `Unreadable`.
  - macOS: a missing symbol or a negative answer gives `Unreadable`.
  - I found no path that turns an error into "not sandboxed".
- **Resolver seam.** `probe_with(resolve, subject)` is ordinary `pub(crate)` code with no `cfg` and no environment read. Production passes only `dlsym_default` (`RTLD_DEFAULT`). The variadic C functions are called through non-variadic pointer types with only their fixed arguments. That is ABI-identical on arm64 and x86-64 for a callee that reads no variadic argument, so it is acceptable. A `…, ...)` variadic pointer type would be the strict form.
- **Rows as proof.** The drafter's 17 mutations plus my M-a to M-d show:
  - the decision table, the caller rule, every cause and every probe field are each red-guarded;
  - the hold binding is not (C-2);
  - the guard's `cfg!` gap is open (C-1).
- **Repo rules.**
  - Every new `cfg` is inside `cfg_if!`, and `check_cfg_boundaries` reports nothing new.
  - No new globals or thread-locals.
  - No production code under `tests/`.
  - The braces rule does not apply to Rust beyond rustfmt.
  - Parity: 2.3a.json has 26 rows, `paired` or `platform`, `done: false` like 2.1. meta.json lists 2.3a in `steps` with a `step_notes` entry, and `check_windows_parity` passes.

## 3. Risks and next action

- **Windows receipt `p2-3a-20261031-a`.** It is the only proof that the Windows arm compiles and that the real AppContainer row passes. The lane owner should run it before merging. A red receipt reopens this review.
- **Fixes before the 2.3d/2.3k wiring, or in this lane's remediation:** C-1 (one regex plus a test) and C-3. C-2's type-state `confirm(self)` is cheap and closes M-d structurally.
- **Hosted CI (plan Q6):** the namespace rows need the user-namespace sysctl and CAP_SYS_ADMIN, and they fail rather than skip without them. The lane owner's WSL-as-root run is still pending.
- **Pre-commitment:** GO stands once the receipt is green. A revision that closes C-1 through C-4 needs no re-review on this axis beyond the closure tests named above.

## 4. Out-of-scope observations

- **Windows sandboxes below the rule's line (design-level hypothesis, not tested).** A Medium-integrity process with a restricted token, or one in a job object, is not an AppContainer and not below Medium, so `judge` would accept it as a listener. A sandboxed process that can write the per-user directory could then serve a client. This is the Windows analogue of Linux Landlock, and the design's §4 Windows rule does not name it. I suggest the design record it in the probes' "cannot see" list, and consider `TokenHasRestrictions` as a refusal. It does not block 2.3a.
- **macOS by-pid fallback.** When `sandbox_check_by_audit_token` is absent, the fallback probe is not pinned. The 2.1 `confirm` checks `process_alive(pid)`, not the token's pid version (word 7). The design allows the fallback, and current macOS always has the token symbol.
- **`sandbox.rs` default arm.** Its `else` arm covers every OS that is neither Linux nor Apple, not only Windows, and it answers "nothing found". Another Unix fails to compile anyway (unix.rs calls `of_peer`), so this is not a silent bypass. An explicit `else if #[cfg(windows)]` would make that intent visible.
- **Contract reference.** The brief names a §15 row 2.1 that does not exist in the contract. The 2.1 carrier is crate-private, so this is a reference error in the brief, not a missing row.
