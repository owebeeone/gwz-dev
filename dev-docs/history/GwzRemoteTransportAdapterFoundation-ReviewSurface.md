# Adapter Foundation — Surface Review

**Date:** 2026-09-20  
**Axis:** Surface  
**Object:** Phase 3b package-distribution and blocking-bridge foundation.

**Verdict: GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Exact tuple

| Repository | Commit |
|---|---|
| root | `687e2d3c21a5fca0cfff81216eff4bdac9f855ca` |
| gwz-core | `46bbc932ac25d9b1762c77351293ea1c0ac7dcbb` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

All commits matched at start and end. Member working trees remained clean. Root contained only the three authorized current-round reviewer prompts; peer contents were not read.

## Evidence base

Read the public consumer README, distribution README, and observable `distribution/fork.py --help`. Process instructions came from `AGENTS_GWZ.md` and the supplied Surface prompt. No implementation, internal design, plan, checkpoint, or peer report was read.

Executed from the workspace root:

```sh
RUSTUP_TOOLCHAIN=1.95.0 python3 gwz-core/tests/transport_consumer/package_proof.py \
  --archive gwz-transport/target/package/gwz-transport-0.1.0.crate \
  --archive-sha256 986033108eab2967028dc52c69f94e859ed6cbb78384648f03e88d9703383191 \
  --source-revision 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
```

**Result:** 21 tests passed, including three blocking-I/O tests. The runner printed the accepted archive provenance and:

```text
core_bridge_sha256=3450bdf09ce6fc76fed7e6285bcd0a0e2d013f69a8e18cd43edc485c56104e39
```

Also executed:

```sh
python3 gwz-core/tests/transport_native/distribution/fork.py \
  --git2-archive /Users/owebeeone/.cargo/registry/cache/index.crates.io-1949cf8c6b5b557f/git2-0.21.0.crate \
  --output /tmp/gwz-git2-review-surface-20260920
```

**Result:** Seven native-binding tests passed against prepared source and seven passed against packaged contents. The runner qualified `gwz-git2` version `0.21.0-gwz.1`, reporting:

```text
archive_sha256=2c0544413ee18231fb9185ad29cb82ffa34c223245cd044523be1515897f68af
```

No network fetching, edits, commits, or remote actions were performed.

## Findings

None. The public documentation and exercised commands did not reveal a concrete surface defect.

## First-day walkthrough and invariants

- **Setup and discovery:** Both READMEs identify prerequisites and the workspace-root working directory. Distribution help describes the archive, requires a new output directory, and states that fetching is opt-in with an offline default. Multiple-registry archive selection is addressed.
- **Use and provenance:** Consumer instructions distinguish unpublished archive proof from future registry resolution. They identify the copied core bridge and its printed digest. Distribution qualification covers both prepared source and packaged contents without claiming publication.
- **Threading:** Blocking calls require independent message delivery and timer service. The README explicitly prohibits blocking the sole host worker and explains why small writes still require host timers.
- **Read/write behavior:** `Read`, partial `Write`, and standard `write_all` are named. The bridge adds no buffer, carrier, thread, socket, or implicit deadline; supplied Stream configuration governs defaults.
- **Errors:** EOF, timeout, delivery loss, cancellation, protocol errors, and caller misuse have distinct documented behavior. Cancellation avoids `Interrupted`, and structured underlying errors remain available for downcasting.
- **Lifecycle:** Clones share one exchange. Half-close preserves reads; close awaits cleanup but does not itself release a physical connection. Cancellation wakes blocked calls. Final-owner drop requests cancellation without blocking cleanup or inferring reuse.
- **Cleanup:** Distribution instructions identify both the selected output directory and retained build directory. No service, remote fork, or credential setup requires reversal. Cleanup was reviewed, not executed against shared build outputs.
- **Package identity:** The proposed alias preserves the `git2` library name while selecting `gwz-git2`. Coordinated adoption and return-to-upstream obligations are explicit.

## Residual risks and next action

This review validates the public surface and local proof walkthrough, not implementation correctness. It establishes neither active SSH integration nor production dependency activation, publication, or native-platform parity.

**Next action:** Record Surface GO for this exact foundation tuple. No Surface remediation is required.
