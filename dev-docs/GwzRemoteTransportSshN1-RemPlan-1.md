# SSH N1 — merged remediation 1

Date: 2026-09-21. Status: implementation and focused re-review pending.
Base: root `b759ced42b18efd674aca8699819a6eafdba2f7b`, core
`9b3d596f6f230b99ceabf9337d9c17376c2e9762`, evidence
`16a9aef10b9216cc0988d3af7e0e483948bf03d2`; other pins unchanged.

Retained Code reports NO-GO with two P2 findings. Retained State reports GO
with no findings. Both full reports are filed verbatim beside this plan.
Owner independently reproduced the CR mismatch outside the settled tree while
both reviewers were blind; this corroborates Code P2-2, not dual-axis convergence.
No acceptance or production activation occurred before discovery.

## Dispositions and closure tests

| Finding | Correction | Required closure |
| --- | --- | --- |
| Code P2-1: socket error kinds conflate network failure with Control expiry/cancel | After any pre-handshake connection error, query the original Control; stop only if it is terminal, otherwise advance to the next address. Keep handshake outside any retry. | Inject socket TimedOut and ConnectionAborted with live Control and prove a second-address success; cancel/expire Control during first attempt and prove no second attempt. Retained Code verifies. |
| Code P2-2 and owner CR differential: input normalization broadens trust | Preserve all CR bytes passed to native parsing. Keep CR/LF exclusion solely for the documented line-size measurement. Do not introduce another G1 exception. | Native differential for no-comment and commented key lines, no ending/LF/CR/CRLF/repeated CR. Endpoint acceptance equals native acceptance for every case. Retained Code verifies. |

P2-2 refinement: the review suggests stripping at most one CR, with any standard
CRLF delta documented. The owner's native matrix proves even a single trailing
CR on a no-comment key is a mismatch under this Unix native reader. Therefore
strip none when parsing; the size check's CRLF terminator allowance does not
normalize trust data. The stronger correction restores native behavior without
an additional compatibility exception.

One merged patch owns both findings. TDD retains the original reds and adds
focused/full green results in a new private evidence run. N2 authority/admission,
N3 backend attachment, production routing, and platform/source qualification
remain excluded. Production is350/350 lines in one file. With N2/N3 still excluded, test allowance
is refined660→770 for the deterministic regressions (actual764). No unrelated
scope is added. The connector loop seam preserves the existing establish_with
loader/resolver API and leaves handshake outside retry.
Retained Code re-verdicts its own two findings. State performs a focused changed-
range confirmation so both final verdicts name the same corrected tuple. This
is the first N1 remediation round; the two-round cap remains.

## Local result

Full focused locked/offline Rust1.95 suite passes91 executions. The private
network-n1-rem-1 run retains socket/CR/boundary reds, intermediate failed attempts,
final green and a native sshd penalty diagnostic. Parse-time length rechecking
was removed because pre-resolution admission already measures the exact bound;
preserving CR bytes must not count the CRLF terminator a second time. No changes
to the accepted compatibility policy. Re-verdicts remain required.
