# GwzWindowsHttpsQualification — Code-AXIS REVIEW

**Review object:** Focused closure of Code P3-1, the owned-path process-inventory defect. Public SSPI fixture bytes and production sources are unchanged.

**Baseline:**

| Repository | Reviewed HEAD | Correction base |
|---|---|---|
| root | `71d7621c42b498a761e8be6a806899e72d964ba4` | `c1db8d490bbce380c726ac4793493aec87053a00` |
| gwz-core-evidence | `d505cddaca791ed6cadb11f9fb5ab4fd88e0fa51` | `1930542b7264bcbc5d9b10c67887c0f350798cb1` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` | Unchanged |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` | Unchanged |

All four HEADs matched at start and end. Final tracked-diff checks returned no changes. Inspection was read-only; no remote replay, mutation or report-file write occurred.

**Date:** 2026-10-04

**Axis:** Code — focused verification of the original inventory counterexample, corrected predicate, actual gate execution and receipt attribution. Independent, adversarial and read-only. No current peer re-verdict was read. Filed verbatim by the lane owner.

**Verdict: GO** — P3-1 is verified closed. No new finding or NEW ARCHITECTURAL root cause was established. Full integrated Windows HTTPS qualification, activation and release remain NO-GO.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P3-1 | Normalize Windows paths, enforce the root directory boundary, withdraw the original invalid inventory attribution and retain a characterized replacement receipt. | Retraced the corrected actual collector predicate and inspected its recorded live control: original matcher reports 0, corrected matcher reports 1, and the exact actual gate reports `ownedRemaining: 1` with exit 1 while the held child remains alive. Slash, backslash and case variants pass; the similarly prefixed sibling is excluded. Held-child termination/wait and owned-directory removal complete before the same actual gate reports zero with exit 0. The original attribution is withdrawn. | **CLOSED** |

## Changed-range analysis

The correction changes private campaign collection and evidence, root checkpoint attribution, and administrative review/settlement records. The substantive changes are:

- `collect.py` factors the actual inventory into `inventory_script()`, normalizes both operands with Windows `GetFullPath`, and appends a directory separator to the root before case-insensitive comparison.
- The live control receives that exact encoded script from the collector and invokes it while its owned executable remains alive.
- New immutable receipts retain predicate controls, actual gate refusal and the subsequent zero observation.
- The checkpoint and campaign README withdraw the original `inventory-final-v3` zero attribution.
- Historical runner versions and original raw receipts remain available.

SSPI and core HEADs are unchanged. The public completion fixture retains SHA-256 `2fcc4b14e70b474cca6fca781fc24a87c3d8a4a75784c04c3e6f10d8115358bc`.

No production owner, dependency, API, wire format, activation boundary or qualification scope changed. No new architectural cause was found.

## 0. Evidence base

I read the RemPlan, my original filed report, root checkpoint correction and the complete relevant campaign correction.

Principal inspected sources:

| Boundary | Sources |
|---|---|
| Actual inventory predicate and dispatch | Private campaign `collect.py:20–21`, `49–54` |
| Live process and predicate controls | `inventory-control.py:10–49` |
| Held-child disposal and directory removal | `inventory-control.py:50–57` |
| Native execution receipts | `raw/inventory-control-v2.*`, `raw/inventory-corrected-v2.*` |
| Source/receipt provenance | Updated manifest, control transfer receipts, retained historical collectors and original raw receipts |
| Attribution | Root checkpoint diff and campaign README correction |

Independent read-only verification established:

- All **58** manifest entries match their recorded hashes.
- The control transfer hash matches retained `inventory-control.py`.
- The corrected collector hash matches both v2 receipt records.
- Decoding both receipt command arguments produces identical inventory scripts.
- Those decoded scripts also exactly match the current collector’s `inventory_script()` result.
- Original raw receipts are byte-for-byte unchanged from the original evidence revision.
- Public fixture bytes remain unchanged.

Recorded Windows results:

| Observation | Result |
|---|---|
| Original matcher with live owned control | `oldOwned: 0` |
| Corrected matcher | `correctedOwned: 1`, `controlSeen: true` |
| Path variants | Slash, backslash and case: true; sibling: false |
| Actual inventory with live control | `ownedRemaining: 1`, gate exit 1 |
| Control teardown | Child reaped; owned control directory removed |
| Subsequent actual inventory | `ownedRemaining: 0`, exit 0; Windows build 26200, ReFS |

The control receipt returned outer exit 0 because detecting and rejecting the live process was its required result. Its source separately asserts the actual inventory gate’s exit 1 and checks that the held child remains alive afterward.

These are inspected recorded native results, not an independent remote replay. No unchanged native suite or cross-target build was rerun for this evidence-only correction.

## 2. Invariant analysis

**The original separator mismatch is removed.** Both root and executable path pass through Windows `GetFullPath`. The root is stripped only of trailing directory separators and receives one native directory separator. The case-insensitive prefix comparison therefore recognizes native backslash paths while excluding a similarly prefixed sibling directory.

**The regression crosses the actual gate.** Predicate characterization alone would not close the finding. The collector passes its actual encoded inventory script to the live control, which invokes that supplied script. Receipt decoding confirms exact equality with both the later zero-gate invocation and the reviewed implementation.

**Process disposal is observed through the held child.** The control is a freshly copied executable beneath a newly created owned directory. Both inventory calls occur before successful liveness assertions on its retained Popen child. The finally block terminates that child if necessary and waits for completion before removing the executable and directory. Successful teardown is printed only after those operations complete.

**Evidence attribution preserves history.** The original invalid zero receipt remains retained and explicitly withdrawn as proof. The corrected negative control and final zero receipt have separate labels and source hashes. The replacement is not presented as a repair of native worker behavior.

**Qualification limits remain intact.** The corrected result is an owned-path process observation. It does not prove universal process disposal, physical erasure or LSASS cancellation. The unchanged local-provider tests still do not qualify TLS/EPA-required HTTPS, integrated Git/CLI/Python execution, differing-account identity or blocked-provider cancellation.

## 3. Risks and next action

No Code finding remains open on this bounded correction. The original native-fixture assessment remains applicable because its source bytes are unchanged.

The next action is owner filing and acceptance of this inventory closure. The Windows qualification-entry/portability prerequisite and subsequent integrated qualification remain separate work; this verdict authorizes no activation or release.
