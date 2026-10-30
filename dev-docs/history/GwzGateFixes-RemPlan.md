# Gate fixes — remediation plan, round 1

Object: gwz-core `94f23ab2` (lane gate-fixes) plus the installed tool changes. Review: `GwzGateFixes-ReviewCode.md`
(Code axis, NO-GO: P2-1, P2-2, P3-1..P3-3; the reviewer pre-committed to GO on a revision closing P2-1 and P2-2 and
fixing or recording the P3s). One merged patch; the lane commit is amended (unpushed) and its receipt label becomes
`gate-fixes-20261030-b`.

| ID | Disposition | Closure test |
|---|---|---|
| P2-1 Mac `.git/config` shipped; host checkout stays LF | Fix. Archive gwz-transport's `.git` without `config` (and without `hooks/`, `logs/`, `info/`), write a minimal repo-local config on the host (`core.repositoryformatversion=0`, `core.bare=false`, no autocrlf/eol keys), so the host's global/system `core.autocrlf` decides, then `checkout-index -a -f`. Preflight refuses a gitfile `.git` and `objects/info/alternates` with a named message. A clean-tree receipt whose `HOSTED_LIKE_OWNER_EOL` is not `crlf` fails. | A unit test that EXECUTES the archive command and the host checkout step on a temp repo (local config autocrlf=false, global autocrlf=true via GIT_CONFIG_GLOBAL) and finds CR bytes in a tracked text file; the archive holds no `.git/config`; refusal tests for gitfile and alternates; the real receipt shows `HOSTED_LIKE_OWNER_EOL crlf`. |
| P2-2 library step runs without errexit | Fix. Run the extracted workflow script with `bash -e` (Actions' default shell for a step with no `shell:`), as one script. | Tests that run the real extracted script with a stub `cargo` shell function: first crate failing → non-zero; empty list → non-zero; `--list-present` failing → non-zero. |
| P3-1 untracked transport files staged | Fix. Stage tracked files only in `--ref` mode; in `--worktree` mode keep staging untracked-unignored files and record them (the worktree is what the lane builds). | `--ref` with a `??` entry: excluded from the archive (or refused); `--worktree`: recorded. |
| P3-2 PIN MISMATCH invisible at the end | Fix. The final receipt line says `(PASS, PIN MISMATCH allowed)`. | Output test for the allowed-mismatch case. |
| P3-3 refusal after booking the slot | Fix. `windows_lane_check.py --preflight-only` runs the transport facts and refusals with no host call; `win-receipt.zsh` (staged in `staging-gate-fixes/`) calls it before booking `BUSY.lock`. | Wrapper with an off-pin tree exits 2 without creating `BUSY.lock` (test with a fake ssh, or a dry-run mode). |
