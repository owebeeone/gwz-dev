# Gate fixes — CODE-AXIS REVIEW (re-verdict round 3, short form)

**Review object:** gwz-core `b0ac8de7..423605ec` (HEAD `423605ec12f1ddfcd54639c77d3d60eaefb966d0`, parent `b0ac8de7`; amended from `9d901d77`). The range is `9d901d77..423605ec`: `scripts/windows_lane_check.py` and `scripts/test_windows_lane_check.py`, plus the commit-message trailer (now `Windows-receipt: gate-fixes-20261030-c`).
**Baseline:** `9d901d77`, the round-2 GO (`GwzGateFixes-ReviewCode-3.md`).
**Date:** 2026-10-30 **Axis:** Code (filed as `GwzGateFixes-ReviewCode-4.md`)
**Verdict: GO** — P0 0, P1 0, P2 0, P3 0 open. The round-2 GO carries to `423605ec`. The classification is confirmed: non-architectural, a one-flag probe defect.

## Closure table

| Item | Status | Evidence |
|---|---|---|
| Round-2 condition: real receipt must show `owner_export_line_endings: "crlf"` | Not yet met, but the cause is found and fixed. Receipt `-c` will confirm. | Receipt `-b` (commit `9d901d77`, read from `receipt.json`): `transport.host_autocrlf` is `{true, file:.git/config}`, `matches_pin` is true, and the `hosted-like` entry has `owner_export_line_endings: "lf"` and `line_ending_failure` set. It had 4309 passed and 3 failed, with the 3 failures all named gaps, none unexplained. The `ordinary` and `transport` shapes exited 0. Your host probe (CR 2402, LF 2402) is consistent with this. |
| Cause of the `lf` reading | Confirmed. | The EOL probe used plain `grep -q`. Git for Windows' grep strips CR in text mode, so it can never report `crlf`. This was the alternative hypothesis in my round-2 report (section 3), and my round-2 closure of P2-3 rested on mechanism tests that could not see it. |
| Fix | Verified. | `grep -qU` (binary mode). On the Mac, `grep -qU "$(printf '\r')"` reads `crlf` for a CRLF file and `lf` for an LF file. The Mac's grep cannot reproduce the host's strip, so the host-side proof is receipt `-c`. |

## Changed-range analysis (`9d901d77..423605ec`)

- **Probe line in `taut_lines`:** the only behavioural change is `grep -q` to `grep -qU`. The redundant-looking `-U` is the whole fix.
- **Other CR-sensitive greps:** none depend on CR detection. The `tr -d '\r'` uses and `grep -a` log scans are untouched. The pin read operates on LF files archived from gwz-core.
- **Comment on `TRANSPORT_HOST_CONFIG`:** now says the 19 earlier LF owner exports were the probe's `grep` without `-U`, not the host's Git settings. This corrects the premise behind round-1 P2-1 (the "regression" framing) and round-2 P2-3.
- **What still stands from those findings:** the Mac's repo-local `autocrlf=false` beats a host global value (shown by the round-1 probe), and the receipt now states `core.autocrlf = true` itself. The recorded `host_autocrlf` is `true` from `.git/config`. The earlier "dabeest has no autocrlf" claim is withdrawn: it was never established. P2-3's finding stands only as the real gate-vs-probe defect, now fixed.
- **Test:** the new `test_the_eol_probe_reads_bytes_because_msys_grep_strips_carriage_returns_in_text_mode` asserts `grep -[a-zA-Z]*U` in the probe text. It is a text assertion, not a behavioural one, because the Mac cannot reproduce the strip. The existing probe-execution test still runs the probe on a CRLF and an LF file. Together they are adequate.
- **Results:** 130 tests OK under 3.13 and 123 OK under 3.11. HEAD was `423605ec` at start and at end, with a clean tree.

## Classification check

Non-architectural is right. The defect is one flag in a single probe line, pre-existing since step 2.2, and it was already named as an alternative hypothesis. It is not a new root cause of the gate design. It does expose a process gap I should name: a verdict that depends on the host must be conditioned on an observation, as round 2 did. That is why the condition mattered.

## Evidence limits and remaining check

- Receipt `-c`: no `gate-fixes-20261030-c` receipt directory exists yet. I cite no result.
- **Pending confirmation on receipt `-c`:**
  - Its `commit` is `423605ec…`.
  - The `hosted-like` entry has `owner_export_line_endings: "crlf"`, no `line_ending_failure`, and `passed: true`.
  - The three known Python-codec gaps appear as named gaps.
  - The three session crates passed on native Windows, as they did in `-b`.
- If `-c` records `lf` again, the cause is not the grep flag, and this GO should be reopened.
