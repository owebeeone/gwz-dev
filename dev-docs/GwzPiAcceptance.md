# Raspberry Pi ARM64 / ext4 acceptance — 2026-09-07

**Passed:** native ARM64 Linux build, untouched lane disposal, root/member work,
one strict-filesystem family merge, ordinary disposal without force, and
separate detach/retain. This is focused real-workspace acceptance, not a full
Linux CI census, power-loss test or network publication test.

## Environment and build

Machine: `gianni@10.1.1.236` (`weftpi`), Debian 13, aarch64,
Linux `6.12.62+rpt-rpi-2712`, ext4 mounted `rw,noatime`, 8 GB RAM.
Rust 1.96.0, C compiler, pkg-config and OpenSSL development support were already
available. Existing workloads were left running.

The official `v0.14.0` aarch64 Linux release archive was downloaded into a
separate bootstrap directory and verified against its published SHA-256 file.
It cloned root `2cb988b4aeb16ac75121b56aecffa65e3becc4c9` into
`~/git/gwz-pi-acceptance-20260907`, materializing all seven members.
A task-specific `XDG_CONFIG_HOME` supplied public HTTPS GitHub URL rewriting
and commit identity; existing user Git configuration was not modified.

The current source was built with `cargo build -p gwz --locked`, one job,
`nice -n 15`, debug symbols disabled and incremental compilation disabled.
Output is outside the workspace at `~/git/gwz-pi-build-20260907`.
Build time was **3 minutes 12 seconds**. The acceptance executable was always
`~/git/gwz-pi-build-20260907/debug/gwz`.

Compiled provenance matches Magenta's source and reports clean repositories:

- CLI `1cb315a887fab4cc331de3399ca4941531730207`, digest
  `22ebcd6a11b96f4d27baa207663b898a0ba68ab478e80741d6099aa99e12b4ec`.
- Core `bf58914476c6d2bf0aa28c8242c80d4c75b94039`, digest
  `06dc44098056d12b759d1bf5a6a237d2e349429c5c9a42c1a2e9c925bd555cd6`.

## Executed checks

| Check | Result |
| --- | --- |
| Clone untouched lane, immediately dispose | Passed; eight repositories and 18,015 reachable objects verified; directory deleted without force. |
| Create/switch `codex/pi-acceptance-20260907` across all targets | Passed; generated branch configuration committed through GWZ. |
| Clone `pi-work`, add root and core proof files, GWZ stage/commit | Passed; independent lane work created. |
| Dispose before merge | Correctly refused `UnwaivedHazard`, preserving the lane. |
| `gwz merge --filesystem-strict --remote pi-work` | Passed: archived v1 record, supported-persisted acceptance, eight participants, two fast-forwarded, six up to date, publication complete. |
| Verify root ancestry, member HEAD, lock and exact proof bytes | Passed. Root lane commit `b3396ce43be36a2b3509d59ec9f1ae641194734f` is preserved; core HEAD and lock record `590ae7f9097be7c87f399dce3abe791a4a2fab9e`. |
| Ordinary disposal after the single merge | Passed; lane removed without a force waiver or second root merge. |
| Clone retained lane, `dispose --keep` | Passed; proof files retained, family pointer removed. |
| Disband and final receiver status | Passed; receiver clean on the isolated test branch. |

The copy path used ordinary copies on ext4; native copy/reflink counts were zero.
No clone-family behavior depended on reflink support. Strict merge success is
not a claim that actual crash/power-loss recovery was injected and tested.

## Preserved state and evidence

- Receiver: `~/git/gwz-pi-acceptance-20260907`.
- Detached ordinary workspace: `~/git/gwz-pi-retained-20260907`.
- Untouched and work lanes were deleted. No acceptance branches or proof commits
  from the Pi were pushed to GitHub.
- Task Git configuration: `~/git/gwz-pi-config-20260907`; only the task's commands
  select it through `XDG_CONFIG_HOME`.
- Raw evidence: `~/git/gwz-pi-evidence-20260907`; harness:
  `~/git/gwz-pi-cycle-20260907.sh`; external build and bootstrap directories remain.
- Mac evidence copy: `/tmp/gwz-pi-evidence`; aggregate transcript:
  `/tmp/gwz-pi-cycle.log`.
