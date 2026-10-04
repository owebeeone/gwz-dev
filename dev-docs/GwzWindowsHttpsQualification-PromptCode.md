You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Code
AXIS: CODE — architecture, interfaces, call graphs, and compatibility reality.
Attack: interface contracts vs. actual call sites; ownership and visibility;
API/wire compatibility with retained readers and older writers; error paths
and hidden panic/allocation paths; whether the diff does what its DRAFT doc
claims and nothing it forbids.

- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzWindowsHttpsQualification-ReviewCode.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root HEAD c1db8d490bbce380c726ac4793493aec87053a00
- gwz-sspi HEAD 582ec001bd2972076ea65a7db87d81d988c6f2e7
- gwz-core-evidence HEAD 1930542b7264bcbc5d9b10c67887c0f350798cb1
- gwz-core HEAD 28f564a674574eaefa43266d3137be9a4ddc38b8
- Object: SSPI c88fa0e174b185a43e0d0d0c91660cb0957e380a..582ec001bd2972076ea65a7db87d81d988c6f2e7 (three public test/docs files); root checkpoint at exact root SHA; new private campaign at evidence SHA.
- Controlling DRAFT document: dev-docs/GwzWindowsHttpsQualificationCheckpoint.md at root c1db8d490bbce380c726ac4793493aec87053a00
- Out of scope: existing untracked SSH N2b prompts, route mapping draft, core bug report and old alpha evidence; new untracked review prompts only. No product source changes or Windows activation.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md amended by dev-docs/GwzProcessOptimization.md; root AGENTS_GWZ.md/EVIDENCE.md plus member AGENTS.md.
- Controlling documents to check the object against: root dev-docs/GwzSspiDesign.md; dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md §9; implementation acceptance; SSPI docs/NativeFixtures.md. Check qualified scope vs actual execution, not full release.
- Explicitly deferred (do not report as findings): full Windows HTTPS/CLI/Python/Git/EPA/TLS, differing-account/SID, real provider blocking, explicit-password, Kerberos, Digest, activation, broader parity and release remain open and explicitly unclaimed. Next portability package is an identified prerequisite, not implemented or accepted by this review..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- tests/native/completion.rs: native verifier FFI buffer/context/credential lifetimes, status/round handling, secret owners before copies, context token disposal, normal/unwind paths.
- Actual shared worker, fixed original deadline/cleanup, origin lifetime/impersonation self-restoration, native/refusal proof limitations.
- Native wrong binding cannot accidentally qualify HTTP EPA or token/SID equality; scrutinize receipt provenance, original failures and final source equality.
- Read only new private campaign campaigns/https-integration/runs/2026-10-04-windows-sspi-qualification. Prior claims unchanged. Avoid unrelated large archive.
- Check test-only package does not change dependency/API/wire, widen activation, or conceal need for platform implementation.
- Code prioritizes verifier API/result safety and exact implementation claims. State prioritizes native ownership/unwind/cancellation/thread restoration and scope of actual cleanup proof.

COMMANDS
From /Volumes/projects/limbo/gwz-dev: read-only git rev-parse/status/diff/show/log, cat/sed/rg/nl and file hashing permitted. Read recorded raw native receipts; no remote SSH or replay/mutation. Optional CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/qualification-review-code cargo +1.95.0 check --manifest-path gwz-sspi/Cargo.toml --all-features --tests --target x86_64-pc-windows-msvc --locked --offline; rustup run 1.95.0 rustfmt --check --edition 2024 gwz-sspi/tests/native/completion.rs. No private archive requirement for public tests. Verify exact tuple start/end. Return complete report only; write no files.

SEVERITY AND VERDICT CONTRACT
- Findings use IDs P0-n / P1-n / P2-n / P3-n:
  P0 = active corruption, data loss, credential exposure, or false composition.
  P1 = likely destructive or unrecoverable release blocker.
  P2 = concrete correctness, recovery, compatibility, parity, or
       diagnosability defect.
  P3 = bounded robustness, coverage, maintainability, or documentation defect
       with a concrete consequence.
- Verdict is GO or NO-GO. NO-GO while any P0, P1, or P2 is open.
- Each finding: ONE root cause, exact location, violated invariant, credible
  reproduction or state/interleaving sequence, impact, required correction,
  and a closure/regression test. Separate independent root causes.
- Style preferences and speculative unease are not defects. Do not pad.
  Interface shape is not style: wrong command placement, a misleading name,
  a missing half of a lifecycle pair, or an option without a default is a
  finding (P2 or P3), on every axis.
- If your verdict is NO-GO but every blocking finding has a bounded,
  text-or-code-fixable remedy, you may pre-commit: "I pre-commit to GO on a
  revision that resolves {IDs} as specified." This makes the re-verdict cheap
  and is encouraged when honest.

