# ARM64 EBADF substrate package — adversarial review (round 1)

- **Date:** 2026-08-22
- **Mandate:** single-axis adversarial reviewer, combined code + durability mandate, round 1 (auto-escalation on P0/P1/P2 per program rules — none triggered).
- **Object:** working-tree state of `gwz-core/src/checked_artifact/capability/pre_catalog/provider/platform/linux.rs` (+210/−4) and `gwz-core/src/checked_artifact/platform.rs` (+44/−1) over base `c13f773`, plus the diagnosis memo `dev-docs/GwzArm64EbadfDiagnosis.md`.
- **Tree caution honored:** all non-object files read via `git show c13f773:<path>`; the only file written by this review is this report.

## Verdict

**GO.** Zero P0, zero P1, zero P2. Three P3 documentation-accuracy findings, none behavioral, each with a one-line remediation. The fix is correct in shape and in durability semantics, masks nothing, stays inside its declared scope, and requires no Track-P spike.

---

## 1. Attribution — "O_PATH by construction" and the kernel contract

**Claim attacked:** cap-std 4.0.2 opens every directory capability `O_PATH` on Linux by construction; the kernel refuses fsync/ioctl on `O_PATH` files with `EBADF` at descriptor lookup while fstatfs/statx/openat accept them.

**Verified against the locked registry sources** (`~/.cargo/registry/src/index.crates.io-1949cf8c6b5b557f/`, cap-std/cap-primitives 4.0.2, rustix 1.1.4 — versions confirmed in `gwz-core/Cargo.lock`):

- `cap-primitives-4.0.2/src/rustix/fs/dir_utils.rs:105-152` — `open_ambient_dir_impl` hard-codes `options.custom_flags((OFlags::DIRECTORY | target_o_path()).bits() as i32)`, and `target_o_path()` is `OFlags::PATH` for `target_os = "linux"`. Ambient opens: O_PATH, unconditionally. Memo citation exact.
- `cap-primitives-4.0.2/src/rustix/fs/oflags.rs:42-51` — `compute_oflags`: `if options.dir_required { oflags |= OFlags::DIRECTORY; if !options.readdir_required && !options.write && !options.append { oflags |= target_o_path(); } }`. Every plain read-only directory-capability open gets O_PATH. Memo citation exact.
- **Both checked paths consume the same flags:** the `openat2` fast path (`src/rustix/linux/fs/open_impl.rs:67` — `let oflags = compute_oflags(options)?`) and the manual fallback (`src/rustix/fs/open_unchecked.rs:17`). The property holds regardless of openat2 availability/seccomp, as the memo claims.
- **No alternate Dir-construction route dodges it in gwz-core:** the only `Dir::from_std_file` in the crate at c13f773 is `platform.rs:193`, inside the `#[cfg(windows)]` variant of `open_dir_share_delete`; the `#[cfg(not(windows))]` variant (`platform.rs:159-163`) routes through `open_dir_nofollow` → `compute_oflags` → O_PATH. `try_clone` (dup) preserves O_PATH. No `reopen_dir`/other constructors present.

**Kernel contract (reviewer's own verification):** `fsync(2)` (fs/sync.c) and `ioctl(2)` (fs/ioctl.c) resolve the descriptor via the `fdget` family that masks out `FMODE_PATH` files — the lookup fails with `EBADF` before any filesystem code runs. `fstatfs` (fdget_raw, allowed on O_PATH since Linux 3.12), `statx`/`name_to_handle_at` with `AT_EMPTY_PATH`, `openat` with the fd as anchor, `dup`, and `fcntl` are all documented-permitted on O_PATH descriptors. This exactly reproduces the recorded diagnostic signature (fstatfs succeeds, fsync/ioctl EBADF on the same descriptor) and confirms the `EBADF` reveals nothing about the mount.

**Descriptor-consuming inventory is complete:** at c13f773 the crate contains exactly two `libc::ioctl` sites (linux.rs:54 `FS_IOC_GETFLAGS`, linux.rs:126 `FS_IOC_GETSFSUUID` — both repaired) and no `fdatasync`/`syncfs`; every directory fsync routes through `platform::sync_parent` (observation.rs:44, transition.rs:409, mutation.rs:396/:412/:426, directory_mutation.rs:729, and `private_barrier` platform.rs:273) — all covered by the one-seam repair — except the deliberately-excluded preservation twin (§5).

**Ruling: the by-construction claim is correct everywhere; no open path yields a non-O_PATH directory capability on Linux. The necessity analysis stands. No finding.**

## 2. Fix shape

### (a) TOCTOU / same-object question — ruled precisely

The dev/ino pin is **test-only** (`descriptor_query_fd_names_the_same_directory_object`, linux.rs:285-295); production performs **no identity compare across the reopen**. That is **correct**, because `openat(dirfd, ".")` is structurally same-object:

- Path resolution starts at the dirfd's own `(vfsmount, dentry)` pair, fixed when the capability was opened.
- The single component `.` is `LAST_DOT`: the kernel's `handle_dots` performs **no name lookup** (only `LAST_DOTDOT` acts) and **no mount traversal** (mount crossing happens only when stepping *into* a walked component). The open completes on exactly the anchor's mount+dentry.
- Consequently: concurrent rename of the directory is irrelevant (resolution is object-anchored, not name-based); a filesystem later mounted *on top of* the directory is **not** entered; concurrent rmdir yields either a successful open of the same (dead) inode or a hard error — never a different object. Using an O_PATH fd as the `openat` anchor is a documented O_PATH capability.

A production dev/ino re-check would be dead code falsifiable only by a kernel bug; placing the pin as a substrate-sentinel test is the right design. The reopened descriptor therefore also reports the identical superblock (`require_ext4`), UUID, and file handle as the capability's object — there is no window in which fsync or the ioctls can land on a different object. **No finding.**

### (b) fd discipline

`descriptor_query_fd` returns `OwnedFd`; every caller binds it to a scope-lived local dropped on all paths, including early `?` returns (`dir_identity` linux.rs:39-44 — dropped after `identity(...)` returns and on the `dir_metadata` error path; `parent_mode` linux.rs:53-66 — dropped on the `require_ext4` and ioctl error paths). `sync_parent`'s `flushable` (platform.rs:377-384) drops on both `?` paths and on success. `O_CLOEXEC` is set on every reopen. The dead-fd test deliberately uses `BorrowedFd::borrow_raw` (never `OwnedFd`), so no bogus close occurs. No raw fd is re-wrapped anywhere — no double-close surface. **No leaks; no finding.**

### (c) Error mapping

- Reopen failures never route through `query_error`: `descriptor_query_fd` maps every `Errno` to `CheckedFsError::io("reopen Linux directory for descriptor-consuming queries", …)` (linux.rs:114-119) — dead descriptor → hard `EBADF`, recycled non-directory → hard `ENOTDIR` (the `O_DIRECTORY` flag enforces it), permission-stripped directory → hard `EACCES`. `sync_parent` propagates the raw OS error via `From<Errno> for io::Error` (parity with the old bare `io::Error` shape; callers attach their labels as before).
- **`query_error` byte-identical, verified mechanically:** `diff <(git show c13f773:...linux.rs | sed -n '173,181p') <(sed -n '205,213p' ...linux.rs)` → empty. The `EOPNOTSUPP | ENOSYS | ENOTTY | EINVAL` allowlist did not grow.
- EACCES-hard is the correct durability posture: on Linux nothing regresses (every one of these calls hard-failed `EBADF` before the fix on all inputs), and macOS fails the same exec-only-directory class earlier, at capability open (cap-std opens dirs O_RDONLY there). Fail-closed, never a skipped barrier. **No finding.**

### (d) fsync durability semantics

`fsync` is inode-scoped, not fd-history-scoped: flushing a freshly opened `O_RDONLY` descriptor for the directory commits the same directory-inode metadata/journal transaction that a dup-of-a-real-descriptor fsync would (this is the canonical open-parent-then-fsync crash-consistency recipe). On Linux `File::sync_all` is `fsync`, so the new arm is semantically identical to what the other-unix arm does on a real descriptor — and strictly stronger than the pre-fix Linux behavior, where the barrier always failed. No weakening; the barrier executes for the first time. **No finding.**

## 3. Scope discipline and the freeze §4.4 ruling

**Diff confinement verified** (full diff read; scoped `git status` shows exactly the two object files modified, memo untracked-new):

- `file_identity` untouched (real `File` descriptor; never affected). `require_ext4`, `rename_domain`, `persistent_handle` **function bodies** untouched — only the fd *argument* at the two repaired call-sites changed. `rename_domain` still queries the capability directly (statx is traversal-class). Windows `sync_parent` no-op untouched with its doctrine comment (platform.rs:392-397). The other-unix arm's body is untouched; its cfg gate necessarily narrowed from `not(windows)` to `not(any(windows, target_os = "linux"))` — the minimal possible edit, and the three arms still exactly partition all targets.
- No dependency change needed: rustix 1.1.4 was already a direct dependency (`rustix = { version = "1", features = ["fs"] }`) and already used in this same linux.rs (`rename_domain`'s statx). Edition 2024 / rust-version 1.95 covers the `c"."` literal (C-string literals already present at base, e.g. `persistent_handle`).
- The push-lane boundary checker polices only `open_rename_source`/`rename_open_source`/`rename_relative` tokens; the diff introduces none.

**Freeze ruling (requested):** GwzM5-8R2DInterfaceFreeze.md §4.4's verdict is "New platform primitive required: NO … Linux shares the macOS arm on P1/P2/P3/P5 and has its own identity child," and §4.1 names `platform::sync_parent` as P2's non-Windows arm, `private_barrier` as P5's, and `provider/platform/linux.rs` as P3's Linux identity child. This package modifies the **internals of those existing arms**: a descriptor re-query composed from `openat`+`fsync` — syscalls already in the platform layer's vocabulary — private to the platform layer, serving the already-admitted families (P2 parent flush, P5 dirent barrier via `private_barrier` → `sync_parent`, P3 identity). No seam signature, caller set, durable record, or public vocabulary item changes; the §4.4 extension classes (recheck-arm C-2, observer-grammar C-3) are protocol-level and untouched. **Ruling: not a new platform primitive; no spike required; scope discipline holds.** One consequence recorded as P3-3 below: the freeze's "Linux shares the macOS arm" sentence goes stale for P2/P5, since Linux now has its own `sync_parent` body.

## 4. Tests

All ten exist as inventoried (nine in `linux.rs::tests`, inside the pre_catalog gate path; one in `platform::linux_tests`, outside the gate filter, exactly as the memo discloses).

**Compile evidence (this host cannot run them — all `cfg(target_os = "linux")`):**

- The implementer's seam-mirror crate (`scratchpad/linux-seam-check`) audited line-by-line: `sync_parent`, `descriptor_query_fd`, and `query_error` bodies are textually identical to the object code; the `CheckedFsError`/`PlatformCapability` stubs copy the real variant shapes verbatim (verified against `capability.rs:51` — `Io { operation: &'static str, source: io::Error }`, `Unsupported { capability, detail }`); dependency pins are `=4.0.2`/`=1.1.4`/`=0.2.189`, matching gwz-core's lock exactly.
- **Independently re-run by this review:** `cargo check --locked --offline --target x86_64-unknown-linux-gnu --all-targets` in the mirror — **clean** (7.18s). The full-crate cross-check is confirmed blocked exactly as the memo states (attempted: dies at openssl-sys needing a Linux sysroot — the libgit2/openssl/zlib C-native class).
- macOS side re-run on this working tree: `cargo test --locked --lib checked_artifact::capability::pre_catalog` → **73 passed / 0 failed** (matches the implementer's 73); `rustfmt --check` clean on both object files.
- Mirror coverage gap recorded as P3-2 below (tests 8-9 and the exact test-10 body are not in the mirror); residual risk is near zero — the omitted code composes only mirror-proven constructs plus the unchanged `identity()` signature — and the ubuntu-24.04 boundary lane's `cargo clippy --all-targets --all-features -- -D warnings` will compile the Linux cfg code (test targets included) at land, before any matrix dispatch.

**Dead-descriptor RLIMIT trick — race-free as claimed:** kernel fd allocation ranges over `[0, rlim_cur)`, so `rlim_cur` itself is unallocatable; the `i32::MAX - 8` fallback (taken when `rlim_cur` doesn't fit, e.g. RLIM_INFINITY) exceeds the kernel's `nr_open` ceiling (≈ 2 147 483 584 < 2 147 483 639), so it is unallocatable too; no code in the crate calls `setrlimit` (verified base + working tree), so the ceiling cannot rise mid-test. The one theoretical hole — an inherited fd above a later-lowered limit — has no instantiation in this process. Using `BorrowedFd::borrow_raw` on a never-allocatable number stretches the doc-contract's "must remain open" phrasing but satisfies its purpose (no aliasing possible) and is argued in the SAFETY comment; syscall-level behavior is total (EBADF). Acceptable test-scoped construction; note only.

**The two allowlist pins do pin:** both write thread-local `errno` and immediately call `query_error`, whose byte-identical match arms are the object under test; `ENOTTY` must take the `Unsupported` arm and `EBADF` must fall through to hard `Io` — a change to the allowlist in either direction fails one of them. Test parallelism is immune (errno is thread-local; no intervening call). Tests 8-9 are correctly substrate-conditional: they assert only the class invariant (`EBADF` impossible on a live capability) and tolerate `Unsupported` on non-ext4/pre-6.8 substrates.

## 5. Memo accuracy

- **"Never executed on any Linux host":** platform-matrix.yml runs exactly `macos-14` + `ubuntu-24.04-arm` (verified in the workflow — no x86 Linux leg). release.yml verifies checked-out tag trees on `ubuntu-22.04` + `windows-2022`; the newest tag is `v0.10.5`, whose tree has **no cap-std in Cargo.lock and no `src/checked_artifact/platform.rs`** (verified via `git show v0.10.5:…`), so no released tree contains the substrate. The identity probe is raw sudo Python over an ext4 remount (no cargo). **Near-miss found and cleared:** `checked-artifact-boundary.yml` *does* run cargo on `ubuntu-24.04` — `cargo clippy --all-targets` (compile only, no execution) and one filtered test, `workspace_ops::merge::v1_lifecycle::tests::no_ff_wire`; that module imports only merge/model code (no `cap_std`, no `checked_artifact`), so the seams were indeed never *executed* on Linux, and "the only leg … that runs the **full** Rust suite on Linux" is accurate as worded. retained-readers.yml contains zero cargo. Claim verified.
- **Untouched twin:** `git show c13f773:src/git/gitbackend/preservation_root/parent.rs` lines 250-252 are exactly `fn sync_dir(dir: &Dir) … dir.try_clone().and_then(|value| value.into_std_file().sync_all())` under `#[cfg(unix)]`, wrapped by that subsystem's own private `sync_parent` (:287) — same class, different lane, correctly scoped out, with the surfacing prediction recorded as residue risk #1. Verified.
- **Arithmetic, resolved honestly:** the classification record states "143 direct `Bad file descriptor (os error 9)` panics across exactly three sites — … observation.rs:44 ×79, transition.rs:413 ×13 … linux.rs:126 ×50". 79+13+50 = **142**; the off-by-one is internal to the *record* ("exactly three sites" vs the 143 headline). The memo does not paper over it — it states "itemized counts sum to 142" and offers the latent `parent_mode` surface (base linux.rs:54-58, one of only two ioctl sites in the crate) as the plausible 143rd while repairing all four candidate surfaces regardless of which hypothesis is true. Correct resolution; the residual unknown is immaterial to the repair.
- Counts cross-checked in the record: `1094/266/1` ARM leg, macos-14 `1359/0/1` (run-13 sibling 31921196151), and the record's "cheapest lead" (grow the errno allowlist) is precisely what the memo's masking analysis rejects — with the right reasons (indistinguishable from lifecycle defects; would not fix `sync_parent`; would no-op the durability barrier).
- One inaccuracy found (P3-1) and one evidence overclaim (P3-2), below.

---

## Findings

**P0 — none.**

**P1 — none.**

**P2 — none.**

**P3-1 (memo accuracy — descriptor attribution of the traversal helpers).** Memo, "The fix": "`require_ext4`, `rename_domain`, `persistent_handle`, and the metadata read stay on the capability descriptor (traversal-class, `O_PATH`-safe)." In the code, `dir_identity` passes the **reopened** descriptor into `identity()` (linux.rs:39-41), so `require_ext4`, `filesystem_uuid`, and `persistent_handle` all run on the reopened fd (linux.rs:126-128), and `parent_mode` runs `require_ext4` on the reopened fd (linux.rs:53-54). Only `rename_domain` (:70-75) and the `dir_metadata` read (:42) stay on the capability. Behaviorally neutral (same object; a real descriptor carries every traversal-class operation), but the sentence will mislead a future maintainer reasoning about which helpers must stay O_PATH-safe. *Remediation:* correct the memo sentence to "`rename_domain` and the metadata read stay on the capability descriptor; the identity/parent-mode helpers now run on the reopened descriptor, which carries both operation classes."

**P3-2 (memo accuracy — seam-mirror evidence overclaim).** Memo, "Local evidence": the mirror "compiles the exact new Linux-only constructs — `sync_parent`, `descriptor_query_fd`, `query_error`, **and the full test module**". The mirror (audited and re-run clean by this review) contains 7 of the 9 linux.rs tests verbatim, a *variant* of test 10, and one extra metadata test; it **omits tests 8-9** (`dir_identity`/`parent_mode` pins — they would need `identity`/`require_ext4`/`persistent_handle` mirrored) and therefore also does not compile the `dir_identity`/`parent_mode` call-site bodies. Residual Linux-compile risk is near zero (every construct in the omitted code is individually mirror-proven or pre-existing, and the ubuntu boundary lane's clippy compiles all targets at land), but the evidence sentence overstates the artifact. *Remediation:* reword to "…and the test module's syscall surface (7 of 9 linux.rs tests verbatim plus a sync_parent test; tests 8-9 compose only mirrored constructs and land under the ubuntu clippy lane)."

**P3-3 (freeze-table drift, forward-looking).** GwzM5-8R2DInterfaceFreeze.md §4.1 (P2 row: "`platform::sync_parent` `platform.rs:367-370` (`cfg(not(windows))`, directory fsync)") and §4.4 ("Linux shares the macOS arm on P1/P2/P3/P5") become stale for P2/P5 once this lands: Linux now has its own `sync_parent` body (platform.rs:367-385). Same family, same contract, no new primitive (ruling in §3) — but the program's reviews navigate by these tables. *Remediation:* one-line drift annotation in the next checkpoint/freeze-refresh entry (per the freeze's own §7 drift convention); do not edit the frozen document retroactively.

---

## Evidence appendix (commands executed by this review)

- `git -C gwz-core diff -- …linux.rs …platform.rs` (full object diff); scoped `git status --porcelain` (exactly the two files modified; memo untracked-new).
- `diff <(git show c13f773:…linux.rs | sed -n '173,181p') <(sed -n '205,213p' …linux.rs)` → byte-identical.
- Locked-source reads: cap-primitives 4.0.2 `dir_utils.rs`, `oflags.rs`, `linux/fs/open_impl.rs`, `open_unchecked.rs`, `open_options.rs`; lock greps for cap-std/cap-primitives/rustix/libc versions (gwz-core and mirror).
- `git grep` sweeps at c13f773: `sync_all|fsync|sync_parent`, `fdatasync|syncfs|libc::ioctl`, `from_std_file|Dir::from|reopen_dir`, `setrlimit`, `no_ff_wire`.
- Workflow reads: platform-matrix.yml (os list), release.yml (os list + tag resolution + v0.10.5 special case), linux-identity-probe.yml (python-only probe), checked-artifact-boundary.yml (ubuntu clippy + filtered wire test), retained-readers.yml (zero cargo), windows-matrix.yml (windows-2022).
- `git show v0.10.5:Cargo.lock | grep -c cap-std` → 0; `git show v0.10.5:src/checked_artifact/platform.rs` → absent.
- Mirror re-run: `cargo check --locked --offline --target x86_64-unknown-linux-gnu --all-targets` → clean, 7.18s (CARGO_TARGET_DIR in review scratchpad).
- Full-crate cross-check attempt: `cargo check --locked --target x86_64-unknown-linux-gnu --lib` → blocked at openssl-sys (Linux sysroot), confirming the memo's stated limit.
- macOS gate: `cargo test --locked --lib checked_artifact::capability::pre_catalog` → 73 passed / 0 failed / 1392 filtered, 11.47s.
- `rustfmt --check --edition 2024` on both object files → clean.
- Freeze doc §4.1/§4.4 and classification-record reads (counts, correction section, cheapest-lead sentence).

**Verdict: GO** (zero P0/P1; escalation not triggered).
