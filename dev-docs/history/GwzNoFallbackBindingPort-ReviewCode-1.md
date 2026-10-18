# L2-A qualified member binding port — CODE-AXIS REVIEW, REMEDIATION 1

**Review object:** Corrected L2-A source-admission package at core `5eb29f073a536901b23f96c3d4b1d05ac59ac01c`; fork binding remains `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; candidate dated 2026-09-20  
**Baseline:** root `a02f91f66b4efe483e44c3fa4e85dfb6d94c19e0`; core `5eb29f073a536901b23f96c3d4b1d05ac59ac01c`; git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Immutable committed blobs were reviewed.  
**Date:** 2026-09-20  
**Axis:** Focused Code review of corrected source admission and its changed-range interactions. Independent, adversarial, read-only; nothing relies on the parallel axis.

**Verdict: GO** — zero open P0–P3 findings.

---

## 0. Evidence base

The tuple and trees were identical at start and end: root `7dc6a7324bdf8d59ea0838dd95c597693af5e6a5`, core `d3d894c6a95d269a34bfb65db833de26c69a0c98`, git2-rs `25d5d7354ca204b805f448c294ed8bd62fdecd86`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, taut `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`. Authorized concurrent core lane edits were excluded. Working-tree diff checks were clean.

I read the merged remediation and complete `f9794d55..5eb29f07` diff. The permitted focused guard suite passed: seven tests, zero failures. Native modes were not repeated; their recorded seven-test results were inspected.

## 1. Prior-finding closure

| Finding | Status | Evidence |
|---|---|---|
| State P2-1 — checkout-local attributes could make `git archive` conceal a missing release file | **Closed** | Baseline construction now uses `git --no-replace-objects ls-tree -rz` at the fixed release and reads each blob with `cat-file`. The real-repository regression installs `info/attributes` with `export-ignore`, deletes the hidden file, and confirms refusal. |

## 2. Changed-range analysis

The correction is one direct child commit and changes only `prove.py`, `test_prove.py`, and the package report. Binding sources, manifest, pin, patch, lock, README, help surface, and archive mode are unchanged.

`ls-tree` recursively enumerates the fixed release’s blobs and sole permitted gitlink omission. Mode/type validation accepts only regular executable/non-executable files and symlinks. `cat-file` obtains raw object bytes without attributes, filters, working-tree conversions, or replacement objects. The existing full checkout-set comparison therefore exposes omitted files before copying. Copy-before-build isolation and content/hash checks remain intact.

The final package remains within budget: 88 changed runner lines and 98 test additions.

## 3. Risks and next action

No changed-range interaction weakens archive compatibility, locked-graph enforcement, checkout isolation, or exact binding provenance. Production activation and platform qualification remain deferred.

Record the focused State closure. On GO, accept L2-A at this exact tuple.
