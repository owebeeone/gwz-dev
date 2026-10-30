# Gate fixes — remediation round 3 (non-architectural)

Object: gwz-core `9d901d77` (GO at `GwzGateFixes-ReviewCode-3.md`, conditional on the real receipt). The receipt
`gate-fixes-20261030-b` (commit 9d901d77) recorded `owner_export_line_endings: "lf"` and so failed the CRLF gate, while
`transport.host_autocrlf` was `{true, file:.git/config}`. Lane-owner probe on the host, read-only, same receipt
directory: `transport.ir.json` held CR 2402 / LF 2402 (python byte count; `od -c` shows `\r\n`); plain
`grep -q "$(printf '\r')"` printed lf, `grep -qU` printed crlf. Git for Windows' grep strips CR in text mode unless -U.

Root cause: the `HOSTED_LIKE_OWNER_EOL` probe (pre-existing since step 2.2) used `grep -q`, so every receipt has
reported lf whatever the checkout held. Not a new architectural root cause: a one-flag probe defect, and the reviewer's
round-2 report named it as the alternative hypothesis. P2-3's premise ("the host has no autocrlf") is therefore
unproven; its fix (the receipt states `core.autocrlf = true`) stands on its own as mirroring the hosted runner.

| Item | Disposition | Closure test |
|---|---|---|
| EOL probe misreads CRLF on the host | `grep -qU`; comment on `TRANSPORT_HOST_CONFIG` corrected | `test_the_eol_probe_reads_bytes_because_msys_grep_strips_carriage_returns_in_text_mode` red then green (logs `rem3-*`); receipt `gate-fixes-20261030-c` shows `HOSTED_LIKE_OWNER_EOL crlf` |

Receipt `-b` otherwise: 4309 passed, 3 failed = the three known Python-codec host gaps; the three session crates ran
after them (fix 2 working on native Windows) with no failures.
