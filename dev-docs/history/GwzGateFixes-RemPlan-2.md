# Gate fixes — remediation plan, round 2

Object: gwz-core `dc192389` (lane gate-fixes). Re-verdict: `GwzGateFixes-ReviewCode-2.md` (Code axis, NO-GO on one new
P2, labelled architectural by the reviewer; P2-1, P2-2, P3-1..P3-3 closed). This is the second and last remediation
round under the two-round cap. One patch; the lane commit is amended (unpushed); receipt label `gate-fixes-20261030-b` (lane owner: the -b receipt had not started when the commit was amended, so it stages the final tree).

| ID | Disposition | Closure test |
|---|---|---|
| P2-3 CRLF gate assumes the host's global autocrlf | Fix, option (a): `TRANSPORT_HOST_CONFIG` sets `core.autocrlf = true`, mirroring the hosted Windows runner image's system config (cite where `windows-matrix.yml`/the workflows document it), so the receipt's checkout is CRLF by construction and the gate is meaningful whatever the host's own config says. Record in the receipt the effective `core.autocrlf` and its origin (`git config --show-origin`) after the checkout. | Real-process test with the host global autocrlf UNSET and with it `false`: CR bytes in a tracked text file; existing test with global `true` still passes. The real receipt `gate-fixes-20261030-c` shows `HOSTED_LIKE_OWNER_EOL crlf`. |

The in-flight receipt `gate-fixes-20261030-b` (pre-fix tuple) is kept as the observation that confirms or refutes
P2-3 and as the first native-Windows run of the session channel and contract crates.
