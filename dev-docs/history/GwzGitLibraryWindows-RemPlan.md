# Q4 Windows qualification — remediation 1

2026-09-21. State P2-1 accepted; Code returned GO on the initial tuple.
Initial review: root 6a0e7661d66e28f27670be7717d23234dd107eba,
core 44d27ad665796cce669083bb8f03b2f194198f42,
evidence b9456f7a8ba328dd3a35ab7577c0e66d3464cdc9.

| Finding | Disposition | Closure evidence |
|---|---|---|
| State P2-1: Cargo reads unrecorded external ancestor config | Add explicit fail-closed search of each Cargo working-directory chain and fixture Cargo home for both config and config.toml. Record absence before/after every Cargo invocation, including the public Python proof that invokes Cargo in a temporary D: child. | Guard creates ancestor configs and requires refusal; fresh native Windows run repeats all final rows under corrected runner; original State reviewer verifies the counterexample and re-verdicts. |

No prior raw result is rewritten. windows-e remains a completed passing
execution whose configuration isolation is insufficient for acceptance.
The corrected native run uses new root D:/gwz-tests/gwz-git-q4-20260921-f,
unchanged public fixture/library/fork inputs, and a fresh Cargo cache. Parent
configuration is inspected, never modified. Presence, unreadable state or
unexpected filesystem errors refuse execution. No override or ignore option.
Source/config snapshots and raw commands/results remain private evidence.

Owned delta: private windows.py (still <=260 lines), new test_windows.py
(<=100 lines), private README/raw run, public scope/result and root checkpoint
annotations. No product API, native source, manifests, locks or public fixture
code changes. Focused State re-verdict closes its finding; no broader claims.
