# Remote transport release readiness — operator pause

Date: 2026-09-22. Status: **paused to conserve weekly GPT quota**.
Do not resume agents, tests or implementation until requested. No reviews were
started for Q6. No push, publication, activation, tag or release was performed.

## What is implemented

Accepted candidate packages cover reusable message streams/backpressure/pooling,
per-remote Git binding, SSH endpoint/key/agent support, endpoint placement, same-process
CLI/core and Python/core message embedding, and Gh-only authenticated HTTPS.
H2 was accepted by retained Code/State reviewers; its implementation remains a
private/candidate integration, not a claim that the release routes are activated.

Q6 batch A subsequently ran portable transport tests on macOS ARM64, Linux x86-64
(WSL), and Windows x86-64. All pass. Corrected integrated host tests pass on Mac
and Linux; current Linux endpoint tests and Windows selected native Git-binding
fixtures pass. Exact local source admission/guards and Windows runner diagnostic
regressions pass. Complete results/limitations are in
[the Q6 report](../gwz-core/dev-docs/GwzRemoteTransportQualification.md).

The longer workload found a real post-acceptance lifecycle P2: repeatedly finishing
an already retired mux request eventually closed a healthy shared session after
five seconds. Core now records logical retirement once, independently of physical
cleanup. Its deterministic regression failed before the fix and passes afterward;
repeated 4 MiB HTTPS clones now pass beyond the old deadline on Mac and Linux.
This fix is committed but **not yet accepted by review**. Q5 State P3-1 diagnostic
retention also has a tested correction awaiting retained verification.

Loopback SSH cold→pooled medians: Mac 137→74ms; Linux 157→66ms. HTTPS also reuses
connections and improves locally. No WAN speedup or optimal-default claim. A Mac
process RSS sample includes fixtures; sustained-backpressure memory is still open.

## Required before releasing the new transport as supported functionality

1. **Accept Q6 corrections.** Settle the saved evidence and exact tuple; reuse
   retained Code and State reviewers for one aggregate gate. Check the monotonic
   retirement fix, physical-cleanup accounting, runner diagnostic fix and bounded
   measurement claims. No need to rerun passing suites absent changes/findings.
2. **Finish Phase 6 qualification and tuning.** Measure workspace fetch, push and
   post-push reads, both placements, long-lived reuse, large packs and sustained
   backpressure memory. Compare immediate emission with coalescing including 100 ms;
   choose defaults within frozen caps. Preserve the agreed 60-second idle default.
   Existing measurements are a baseline, not this exit gate.
3. **Close supported-platform gaps.** Native Windows portable code passes, but
   integrated host/SSH adapters are Unix-gated and need implementation plus native
   qualification. Intel Mac and Linux ARM64 were not executed. Either complete
   the promised support matrix or explicitly agree and document a narrower release
   scope; missing rows cannot silently count as passes.
4. **Make selected sources reproducibly buildable for release.** Qualify pinned
   git2-rs/libgit2 and transport sources through the intended distribution route,
   clean independent builds and actual CLI/core/Python consumers on supported
   targets/features. Local path-patched source bundles are insufficient proof.
   Publish/push dependencies only under the separately authorized release sequence.
5. **Activate the production construction and routes.** Wire accepted runtimes,
   endpoint ownership and dependency pins into the actual shipped CLI/core/Python
   paths; preserve credential locality and gh-only authenticated HTTPS. Validate
   selected identities, compatibility/refusal behavior, cleanup and existing
   configuration/help through real consumers. This is a separate reviewed change;
   same-process embedding proof alone is not frontend activation.
6. **Run the release gate and publish.** Complete normal repository CI/package
   checks for supported targets, installation smoke tests, migration/release notes
   (especially HTTPS authentication policy), and aggregate activation/release review.
   Then perform the separately authorized version/tag/build/publish sequence.

The existing HTTPS qualification still distinguishes fixture authentication from
real-account/provider/TLS/proxy compatibility; any capability advertised by the
release must have the corresponding evidence. Do not treat synthetic fixtures as
proof of the real GH environment.

A physical CLI/core byte carrier, iroh and separate-process wire testing remain
explicitly outside this development cycle. They are not prerequisites for the
agreed same-process message integration. Endpoint-management expansion likewise
remains outside scope. No new overarching design is required merely to review
Q6; platform adaptation or changed release scope may need a bounded design.

## Resume inputs

Read CurrentProgramCheckpoint.md first, then the Q6 public report and
GwzRemoteTransportQualification-RemPlan.md. The retained discovery report is
GwzRemoteTransportQualification-DiscoveryState.md. Reviewers are
`/root/transport_consistency` (Code) and `/root/transport_safety` (State).
Review-loop skill: /Users/owebeeone/.claude/skills/review-loop/SKILL.md.
No Q6 aggregate review/remediation round has run; H2 history/caps remain unchanged.

Product/source checkpoint before pause annotations:

- .: `e3be7adc26c2733737b781c8cc98f0c6743c683f`
- gwz-core: `a2a7878df976c1d0b2e5880414afcb56aed55219`
- gwz-transport: `aa40936d0805e8cb60f8027615abe20d4f2045e4`
- taut: `bcf98b64d465fc54841121b6d1a2d46940f81a3c`
- gwz-cli: `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`
- gwz-py: `d07d55dacb1725d9306be9c04d157ac29a78e000`
- gwz-git: `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`
- git2-rs: `ce78628308e11b4e8901d5061602619109bce21a`
- libgit2: `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`

Private evidence: gwz-core-evidence/campaigns/transport-qualification/runs/2026-09-22-q6-a.
Its checkpoint commit is the companion to this pause; resolve its HEAD with gwz
status when resuming. inputs/ preserves Mac and Linux candidate manifests/locks;
correction-sources.json records the three exact corrected file hashes. Raw failure
and green logs, all performance samples, command supplements and derived statistics
are durable. The initial evidence baseline is 3302b5d03f56590a6d521b1b52db302861775e15.

External runtimes/caches remain intact; do not delete them as cleanup:
- Mac instrumented core: /tmp/gwz-q6-perf-core-a; target /tmp/gwz-placement-b-backend-target.
- Mac portable target: /tmp/gwz-h1-transport-target.
- WSL: /mnt/d/gwz-tests/gwz-q6-linux-20260922-a.
- Windows selected source: D:/gwz-tests/gwz-q6-native-sources-b; target sibling gwz-q6-native-target-b.
- Native SSH commands must disable inherited forwards (ClearAllForwardings=yes).

/tmp/gwz-q6-prompts.py is a prepared canonical-template generator, not yet run.
Its durable copy is dev-docs/GwzRemoteTransportQualification-PromptGenerator.py;
regenerate prompts against the then-current exact settled tuple. Do not use old
H2 prompt tuples or infer approval from prior H2 GO reports. No agent or build
needs to be left running during the quota pause. Local disk is tight (~0.8GiB),
so prefer preserved external builds and avoid unrequested cache deletion.
