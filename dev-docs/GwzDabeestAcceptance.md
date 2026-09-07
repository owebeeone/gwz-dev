# Dabeest Windows acceptance — 2026-09-07

**The local clone / commit / named merge / ordinary disposal cycle passed on the
real eight-repository GWZ development workspace. Windows network publication
remains blocked by credential acquisition.** This is a focused acceptance run,
not completion of the full Windows release matrix.

## Published source and native build

The Mac committed and published the debt-recovery work through GWZ. Root
`474a3cb626e1eb66bee0d16fbf7e819d13cd28fc` pinned CLI `1cb315a`, core `6593b3b`
and Python `133f5fd`. The editor swap file was excluded and preserved.

Dabeest's existing `gwz 0.14.0` cloned the published workspace into
`C:\Users\gianni\git\gwz-dev`. All seven member revisions matched the lock.
Rustup / Rust 1.98.1 and Visual Studio 2022 Build Tools with the C++ workload
were installed. Cargo output lives outside the workspace at `~/git/gwz-build`.

The first native Windows compilation found an actual portability defect:
`git2::Cred::credtype()` returns a signed C enum on Windows, while
`CredentialType::from_bits_truncate` requires `u32`. The conversion now preserves
the credential bitmask explicitly, with a scoped allowance for the redundant
cast on Unix. The failing Windows build is the red check; macOS `cargo check
-p gwz-core --lib --locked` and the subsequent Windows build pass.

The fix was committed and published as root
`8f40897f2e50d1180904a4d4f05832acae59b78a`, core
`4d35577c84ffe653accb21396ab23ae31b6cdb23`. Dabeest pulled that committed source
and rebuilt with `cargo build -p gwz --locked`. The acceptance executable was
always `C:\Users\gianni\git\gwz-build\debug\gwz.exe`, not the old installed one.
Its compiled provenance reports both repositories clean:

- CLI: `1cb315a887fab4cc331de3399ca4941531730207`, source SHA-256
  `22ebcd6a11b96f4d27baa207663b898a0ba68ab478e80741d6099aa99e12b4ec`.
- Core: `4d35577c84ffe653accb21396ab23ae31b6cdb23`, source SHA-256
  `06dc44098056d12b759d1bf5a6a237d2e349429c5c9a42c1a2e9c925bd555cd6`.

## Executed lifecycle

| Check | Result |
| --- | --- |
| `local clone untouched ../gwz-dev-lane-untouched`, immediately `local dispose untouched` | Passed; directory deleted without force. 3,043 files, eight repositories, 17,976 reachable objects verified. |
| `--all branch --create codex/dabeest-acceptance-20260907 --switch` and GWZ commit of generated branch configuration | Passed; production `main` branches were not modified by acceptance work. |
| `local clone work ../gwz-dev-lane`, add proof files in root and core, `gwz add`, `gwz commit` | Passed. Root and member commits created. |
| Ordinary disposal before merge | Correctly refused `UnwaivedHazard` for root and core unpreserved history; nothing removed. |
| `gwz --remote root push` from the lane | Refused because no Git remote named `root` exists. Family push is still unimplemented; the current response is `GitCommandFailed`, not the older documented `MissingRemote`. |
| `gwz --target mem_gwz_core --target @root push` to origin | Failed acquiring write credentials; root publication was withheld after member failure. Not a successful Windows publication test. |
| From root: `gwz merge --remote work` | Passed with all eight participants, root and core fast-forwarded, six up to date. No second root-selected merge. |
| Verify imported history, proof-file contents and lock | Passed. Lane root `4b72ccefd14a1c9a482db9c9db4c28e4d177e767` is an ancestor of root's final merge-publication commit `66ff5da`; core HEAD and lock both record `bf58914476c6d2bf0aa28c8242c80d4c75b94039`. |
| `gwz local dispose work` after merge | Passed; lane directory removed without force. |
| Clone `retained`, then `local dispose retained --keep` | Passed; family pointer removed and proof files retained in an ordinary workspace. |
| `local list`, `local disband`, final root status | Only root remained; disband succeeded and root is clean. |

The initial harness incorrectly demanded equality between final root HEAD and
the imported lane HEAD. The merge legitimately creates a successor commit to
publish its lock. The corrected assertion checks ancestry plus the committed
member identity and proof-file contents. No product change was needed for this.

## Remaining observations and preserved state

- Dabeest rewrites GitHub SSH URLs to HTTPS through existing Git configuration.
  Git Credential Manager is configured, but GWZ could not acquire write
  credentials. Configure/verify Dabeest's GitHub write authentication, then retry
  publication from its acceptance branch. No keys or tokens were copied there.
- On the Mac, an empty agent and a default key belonging to a different GitHub
  account first caused refusals. Explicit selection of the existing `id_rsa.no`
  identity authenticated as the repository owner and both real pushes passed.
  These attempts also exercised root withholding after member rejection.
- `~/git/gwz-dev` and `~/git/gwz-dev-lane-retained` remain on the isolated
  acceptance branch. The untouched and work lanes were deleted. The detached
  lane's status shows only `gwz.conf/markers/conf-integrity.yml` modified; retain
  this derived-marker observation for UX follow-up, rather than claiming that
  detach produced a completely clean status.
- The old installed executable is unchanged. The new binary and external build
  directory remain available. Existing GPU/model services were untouched.
- Raw per-step evidence is on Dabeest at `~/git/gwz-acceptance-evidence`, with
  the three executed PowerShell scripts beside it under `~/git`. Copies are on
  the Mac at `/tmp/gwz-dabeest-evidence`; aggregate runner logs are
  `/tmp/gwz-dabeest-{smoke,work,finish}.log`.
- Existing command prose/help still contains stale claims about root selection,
  ordinary family merge and family push support. This run establishes the actual
  behavior above; a documentation alignment pass remains necessary.
