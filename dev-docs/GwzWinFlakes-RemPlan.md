# Windows flake hardening — remediation plan, round 1

Object: gwz-core `7db78119` (lane win-flakes). Review: `GwzWinFlakes-ReviewState.md` (State axis, NO-GO: S-1 P2;
S-2..S-5 P3; pre-committed GO on a revision closing S-1 and recording S-2..S-5). Under the review-tier rule a P2 on a
single-axis interior review escalates to the second axis: the Code axis reviews the remediated revision. One patch;
the lane commit is amended (unpushed); receipt label `win-flakes-20261030-b`.

| ID | Disposition | Closure test |
|---|---|---|
| S-1 close row can pass with the property broken (product's 5 s per-open cleanup ends a held close; the row's only deadline is 120 s) | Fix: restore a one-sided bound tied to the product's 5 s (e.g. `recv_timeout` ~2.5 s from the `close()` handoff, with the constant named and justified against `OPEN_CLEANUP_MS`), keep the gate so the close really is in flight for that window; correct the "as long as the test wants" / "nothing but the test ends it" wording and document the residual 5 s read window. | Stand-in `sleep 4000 ms` before `finish()` turns the row red; 4 s sleep before the count read may stay green, 6 s stays red with the documented message (Mac probes, logs kept); dabeest loop >= 100 runs green. |
| S-2 `kill_tree` `.output()` creates pipes | Fix: `.stdout(Stdio::null()).stderr(Stdio::null()).status()` in both cfg_if variants. | The new row green on Unix; `red-killtree` red against the original on dabeest; fixed version green on dabeest. |
| S-3 kex row: completion `expect` asserts environment health; no baseline for the original row | Fix: drop the completion `expect` for a run whose cause is not a kex failure (the row asserts `settled`'s promise only). Record the measured exposure honestly in the evidence README (no baseline claim without a loop). | Row still kills the reviewer's three `settled` mutations; Mac + dabeest loops green. |
| S-4 103-minute compile unexplained | Record as accepted (follow-up list; the receipt watchdog bounds the stall). | — |
| S-5 guard misattributes a thread panic; `kill_tree` row name says "tree" | Fix: distinguish `Timeout` from `Disconnected`; add a grandchild to the `kill_tree` row or rename it. | Message check by a forced panic probe; row name/coverage matches. |
