# Windows flake hardening — remediation plan, round 2 (escaped CI defect)

Object: gwz-core `ee14d941` + `101243b5` on the accepted `298b0783` (lane win-flakes), the fix for the Transport
candidate failure on hosted ubuntu-24.04 at `b31222bd` (procps-ng 4.0.4 `/bin/kill -KILL -PID` signals the group
named by the PID's first digit). Review: `GwzWinFlakes-ReviewState-3.md` (State axis, NO-GO on S-1). One patch, as a
third commit: gwz has no amend.

| ID | Disposition | Closure test |
|---|---|---|
| S-1 P2 parity inventory lacks rows for `libc::{SIGKILL,kill,pid_t}` in `native/tests.rs` | Fix: three `paired` rows (owner 0.5, G24) in `windows_parity/0.5.json`; the `else` arm (`taskkill`) is the pair. | `check_windows_parity.py` at HEAD: "nothing new"; remote gates' run_tests and boundary green. |
| S-2 P3 EXIT_BOUND doc states the wrong threshold | Fix: the window is `OPEN_CLEANUP_MS - EXIT_BOUND` after the handoff; both doc comments say so. | Reviewer re-reads. |
| S-3 P3 kill_tree failure invisible | Fix: `kill_tree` returns `io::Result` on both platforms (errno; `taskkill` status); the killer `expect`s it, the validator timeout ignores it. | Row green on Mac (both thread counts), WSL and the Windows receipt. |
| S-4 P3 ready line in three writes | Fix: one `write_all` of `\n` + ready + `\n`. | Row green with `RUST_TEST_THREADS=1` (Mac). |
| S-5 P3 exit alone not timed | Record (follow-up list); no measurement in this patch. | — |

**Deviation, accepted by the operator ("Keep red intermediates", 2026-10-31).** `ee14d941` and `101243b5` are RED in
the per-commit lane gate (Windows-parity rows missing until the third commit). They stay RED in the gate's history,
as `95d292f`, `b923109` and `dc17d80c` do. The push's checked-artifact-boundary job runs the same gate over the push
range and is expected red once; the local prepush lane gate is passed over for this push only.
