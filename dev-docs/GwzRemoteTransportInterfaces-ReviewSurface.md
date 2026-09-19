# Transport Interface Freeze — Surface Review

**Date:** 2026-09-19  
**Axis:** Surface  
**Verdict:** **GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 1

The public interface presents a coherent ownership and cleanup model, states its defaults, and assigns admission, timer, progress, and disposal duties explicitly. No blocking surface defect was found. One documentation defect prevents a fresh checkout from following the consumer proof instructions without guessing its Python setup.

## Object and exact tuple

Phase 1/2 transport interface candidate:

| Repository | Reviewed commit |
|---|---|
| root | `9bb74ace2fca56f770af34b6ec1bdf6bab4e695e` |
| gwz-core | `ce3bb967f9696b1791fcbefb65b6ccdd0c8ab618` |
| gwz-transport | `163feebe439edd5d1fbaf11e163882e80d4f2257` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

All four commits matched at both the start and end of review. The three member working trees were clean. Root had only the three permitted untracked current-round reviewer prompts. Their names were visible in status; peer prompt contents were not read.

## Evidence base

Read in full:

- `AGENTS_GWZ.md` and the supplied Surface review prompt, for process instructions.
- [Transport README](/Users/owebeeone/limbo/gwz-dev/gwz-transport/README.md).
- [Consumer README](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_consumer/README.md).

No implementation source, internal design, plan, checkpoint, or peer report was read. Test names and results came from permitted public commands.

Commands actually executed:

| Command and directory | Result |
|---|---|
| `git rev-parse HEAD` and `git status --short`, root and each named member, at start and end | Exact tuple retained; reviewed members clean |
| `git -C gwz-core status --short --ignored -- protocol/.regen-venv` | `!! protocol/.regen-venv/` |
| `git diff --check`, each named member | No output |
| `cargo test --locked --test pool_stream endpoint_returns_lease_only_after_exchange_cleanup -- --exact`, transport directory | 1 passed |
| `cargo test --locked --test stream`, transport directory | 12 passed |
| `cargo test --locked --test io_clock`, transport directory | 6 passed |
| `cargo test --locked`, transport directory | 85 passed; 2 extended campaigns intentionally ignored |
| `python3 scripts/regen.py --check`, transport directory | Stopped with `ModuleNotFoundError: No module named 'taut'` |
| Documented consumer regeneration command, workspace root | Passed |
| `cargo package --manifest-path gwz-transport/Cargo.toml`, workspace root | Packaged and verified successfully |
| Documented consumer archive proof command, workspace root | 16 tests passed offline; expected digest and revision accepted |

The consumer regeneration command was:

```sh
gwz-core/protocol/.regen-venv/bin/python \
  gwz-core/tests/transport_consumer/protocol/regen.py \
  --owner-schema gwz-transport/protocol/transport.ir.json \
  --taut-source taut/src --check
```

The archive proof command was:

```sh
gwz-core/protocol/.regen-venv/bin/python \
  gwz-core/tests/transport_consumer/package_proof.py \
  --archive gwz-transport/target/package/gwz-transport-0.1.0.crate \
  --archive-sha256 8c7d91d54f0e176109f286beec65c24544f35f23506380939fdb081e6cef87a6 \
  --source-revision 163feebe439edd5d1fbaf11e163882e80d4f2257
```

No files were edited and no commits were made. Permitted test and packaging commands produced build outputs.

## Finding P3-1 — Consumer walkthrough requires an unexplained local Python environment

**Location:** [Consumer README, regeneration command](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_consumer/README.md:13) and [archive proof command](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_consumer/README.md:35).

**Root cause:** Both command blocks invoke `gwz-core/protocol/.regen-venv/bin/python`, but the public instructions do not explain how to create that environment, what it must contain, or whether another interpreter is supported.

**Violated invariant:** A first-day reproducibility walkthrough must state the setup necessary to execute its commands without relying on undocumented local state.

**Reproduction sequence:**

1. Start with a checkout without a previously created `gwz-core/protocol/.regen-venv`.
2. Follow either consumer command block.
3. Execution stops before the proof runner starts because the named interpreter does not exist.
4. Neither allowed public README supplies the missing environment creation procedure.

The current workspace’s environment exists and both commands passed. Its ignored status was verified directly. A fresh environment was not created or removed during this read-only review.

**Impact:** A new contributor cannot reproduce the advertised consumer checks from the public walkthrough alone. They must infer an environment setup or seek other documentation. This affects onboarding and reproducibility, without requiring an API compatibility change.

**Required correction:** Document interpreter/environment setup and required dependencies before both commands, or use a generally available interpreter and state its prerequisites. Preserve the exact-source requirements for consumer regeneration.

**Closure test:** Starting without the local environment, follow only the corrected public instructions and successfully run both consumer regeneration and the archive proof. No preexisting private environment should be required.

## First-day walkthrough

1. **Choose the supported entry point.** The transport README clearly identifies an unpublished library, directs the reader to source-based tests, and distinguishes typed message handling from physical delivery. There is no applicable product CLI or installation/uninstallation command family.

2. **Use the fake host once.** The three documented focused commands run successfully. The lifecycle narrative identifies checkout, Connect acknowledgment, exclusive lease acquisition, stream exchange, backend cleanup, and healthy release.

3. **Understand production admission order.** The reader is directed to create a fresh session, offer/accept/verify binding, validate Open and endpoint timeout policy before effects, and construct stream endpoints after Open/Opened establishes limits. Pool checkout is explicitly a host sequencing responsibility.

4. **Keep blocked work progressing.** Both stream and pool documentation require independent clock service. They explain why a pending message/action future and a deadline snapshot do not supply a timer task.

5. **Finish and undo ownership.** Healthy reuse requires explicit lease release after cleanup. Dropping a lease discards it; dropping an unclaimed checkout cancels it. Host teardown requires shutdown, processing cleanup actions, and acknowledging disposal before dropping the driver. The test walkthrough explicitly creates no installed services or remote resources.

6. **Reproduce the packaged consumer.** Packaging and the isolated consumer proof passed with the documented digest. A fresh user encounters the environment setup gap in P3-1.

7. **Regenerate owner output.** The selected system Python lacks `taut`, so the owner drift check did not run. The transport README explicitly requires installation of `taut-proto==0.9.1` and the pinned formatter; this unmet prerequisite is not itself a finding. No dependencies were installed during this review.

## Invariant analysis

| Surface invariant | Assessment |
|---|---|
| Names and API grouping | Binding, stream, pool, codec, endpoint, and driver responsibilities are distinguishable in the public contract. No misleading placement requiring a compatibility change was identified. |
| Ownership pairs | Construction, cloning, checkout, healthy release, cancellation, discard, shutdown, and disposal acknowledgment are documented together with their consequences. |
| Defaults | Publicly described stream limits, pool ceilings, timeout budgets, initial I/O state, negotiation limits, and optional request overrides have stated defaults. Required endpoint configuration is explicitly identified as having no implicit defaults. |
| Timer/progress duties | Clock initialization, monotonic origin, independent ticks, exact-deadline behavior, genuine backend progress, and cumulative helper allowance are explicit. |
| Admission before effects | Binding verification, Open validation, endpoint policy checks, ownership checks, and bounded ingress duties precede credentials/allocation/network effects in the documented sequence. |
| Failure preservation | Received prefixes, structured peer failures, timeout effects, carrier loss, and first-error retention are distinguished. No success or replay guarantee is inferred from flush or timeout. |
| Cleanup and capacity | Logical cancellation does not imply completed disposal. Closing resources retain capacity until acknowledgment; shutdown completion depends on actual cleanup. |
| Evidence honesty | Unpublished status, registry deferral, local fake-host scope, unexecuted remote CI, and outer-wrapper allocation duties are stated explicitly. The archive proof reproduced its claimed package identity and digest. |

## Residual risks and next action

This is a public-contract review, not source verification. Passing existing tests supports the documented walkthrough but does not independently prove their assertions or implementation coverage.

Owner regeneration remains unverified under its required Python dependency and formatter setup. Extended randomized campaigns, physical adapters, production dispatch, native-platform qualification, registry publication, and remote CI were not established by this review.

**Next action:** Correct P3-1 and repeat the consumer walkthrough from a fresh environment. The current exact tuple receives **GO on the Surface axis** with that nonblocking documentation finding open.
