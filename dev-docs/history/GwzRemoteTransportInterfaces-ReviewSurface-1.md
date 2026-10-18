# Transport Interface Freeze — Surface Closure Review

**Date:** 2026-09-19  
**Axis:** Surface  
**Object:** Phase 1/2 transport interface candidate, remediation 1; focused closure of P3-1.

**Verdict: GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Exact tuple

| Repository | Commit |
|---|---|
| root | `9d0dc7ef5c616d64d52c296ea2fa34d83d21d73e` |
| gwz-core | `ace269896ad80aee923e2e8fd31e565c43de57ed` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

Verified all four commits at start and end. Reviewed member working trees remained clean. Root contained only the three untracked current-round review prompts; peer contents were not read.

## Prior-finding closure

| Finding | Original defect | Correction and closure evidence | Status |
|---|---|---|---|
| P3-1 | Consumer commands required an undocumented local Python environment. A fresh checkout could not follow the walkthrough without guessing its setup. | The consumer README now specifies Python, venv/pip, pinned `taut-proto`, Rust/rustfmt, working directory, canonical generator source, Cargo cache prerequisite, and temporary-interpreter substitution. A newly created external environment successfully ran both regeneration checks and the archive proof. | **Closed** |

**Location:** [Consumer README — Prerequisites](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_consumer/README.md:10).

The violated invariant—public reproducibility commands must disclose their required setup—is now satisfied. No further correction is required for P3-1.

## Evidence and commands

Read the current public transport and consumer READMEs in full, the supplied Surface closure prompt, and the previous Surface finding retained in this review’s context. No source, internal design, plan, checkpoint, or peer report was read.

Created `/tmp/gwz-surface-closure.kOfKDt` with `python3 -m venv`, then used its interpreter to run:

```sh
python -m pip install taut-proto==0.9.1
```

Installation succeeded. The documented command:

```sh
rustup toolchain install 1.96.0 --component rustfmt
```

confirmed the pinned toolchain and formatter were available. Subsequent commands used `RUSTUP_TOOLCHAIN=1.96.0`.

| Executed check | Result |
|---|---|
| Temporary interpreter: `gwz-core/tests/transport_consumer/protocol/regen.py --owner-schema gwz-transport/protocol/transport.ir.json --taut-source taut/src --check`, from workspace root | Consumer regeneration verified |
| Temporary interpreter: `scripts/regen.py --check`, from transport directory | Four generated artifacts verified |
| `cargo package --manifest-path gwz-transport/Cargo.toml`, from workspace root | Package built and verified |
| Temporary interpreter: documented consumer `package_proof.py` command with the current archive, digest, and source revision | **18 tests passed**, offline |

The archive proof accepted:

```text
source revision: 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
archive SHA-256: 986033108eab2967028dc52c69f94e859ed6cbb78384648f03e88d9703383191
```

The temporary environment was removed afterward. The existing repository environment was neither used nor modified. No source edits or commits were made.

## Walkthrough and invariant analysis

The revised walkthrough now provides a complete sequence: prepare an interpreter, install the pinned dependency and formatter, verify generation against the canonical source, package the owner, and run the isolated consumer proof. Temporary environment substitution worked as documented.

The README continues to distinguish unpublished package proof from registry resolution. Its documented stream/lease ownership, disposal acknowledgment, defaults, independent timer service, and binding-before-Open responsibilities remain explicit. This focused closure found no new surface defect.

## Residual risks and next action

This review verifies documentation and executable setup; it does not independently audit implementation correctness. The full Rust suite was not repeated. Physical adapters, production dispatch, registry publication, native-platform qualification, and remote CI remain outside this closure.

**Next action:** Record P3-1 as closed. No Surface finding remains open at the verified tuple.
