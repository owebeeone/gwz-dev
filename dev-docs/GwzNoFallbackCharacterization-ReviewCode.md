# L4-A filtered-history characterization — CODE-AXIS REVIEW

**Review object:** L4-A test characterization, design-input report, and bounded private-helper wiring at core `c63f497df29d51ad5d864738fa0056b513c3ab7d`; candidate dated 2026-09-20  
**Baseline:** root `9592320ddf95fe53eef0619398b1f8170943a4cc`; core `c63f497df29d51ad5d864738fa0056b513c3ab7d`; git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed blobs were read with immutable Git commands.  
**Date:** 2026-09-20  
**Axis:** Code correctness, fixture fidelity, helper ownership, and scope. Independent, adversarial, read-only. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0–P2 findings; one nonblocking P3 documentation finding.

---

## 0. Evidence base

The tuple and trees remained unchanged at start and end: root `102d3f1288d712aec374c1c82739d7c7e1197af2`, core `81483f2da70f4d906a12b140c910b104db37a21b`, git2-rs `25d5d7354ca204b805f448c294ed8bd62fdecd86`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, and taut `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`. Members were clean; root contained only the authorized prompts.

I inspected the complete L4 test module and report, the three-line integrator wiring handoff, reused private helpers, current `PathWalk` implementation, checkpoint amendment, controlling plan clauses, and package diff from core `5eb29f07`. The recorded focused result was three passed and 2,150 filtered. It was not repeated.

## 1. Findings

### [P3-1] The report’s diff-check claim is not reproducible

`GwzNoFallbackHistoryDesign.md:3-4,76-77` states that `git diff --check` passes for both owned files. The committed package range reports trailing whitespace on lines 3 and 4, caused by Markdown hard-break spaces.

A later auditor or range-based gate therefore receives a failure despite the recorded clean claim. Remove the trailing spaces or document an intentional checker exception and stop claiming an unqualified pass. Closure is:

```text
git diff --check 5eb29f073a536901b23f96c3d4b1d05ac59ac01c..c63f497df29d51ad5d864738fa0056b513c3ab7d \
  -- dev-docs/GwzNoFallbackHistoryDesign.md \
     src/operation/commit_log/path_characterization.rs \
     src/operation/commit_log/tests.rs
```

## 2. Invariant analysis

The substantive attacks held:

- The package adds 148 test lines and 92 documentation lines, below the 350/250 ceilings. The only third file is the integrator-owned three-line child-module wiring.
- Wiring is unconditional inside the existing test-only module, reuses private helpers through `super::*`, and changes no helper visibility, runtime module, API, dependency, or protocol.
- Attribute tests distinguish set, unset, exact-value, mismatch, and unspecified forms. They freeze both native results and the current `--git-dir` worktree-context gap.
- The long-form test verifies root routing and preserves the complete `top,attr:` envelope.
- Merge, range, first-parent, and no-merges cases compare complete ordered OID sequences against native Git rather than sets or timestamps.
- Git is used only for fixture construction and the existing oracle boundary. The product subprocess remains unchanged; no replacement implementation is claimed.
- The report explicitly leaves member attribute routing, bare/shallow/promisor combinations, and the replacement traversal design pending. It does not silently promote root-only evidence into full acceptance.

## 3. Risks and next action

This package supplies bounded design evidence only. It does not qualify a new traversal or Git-free runtime.

Correct P3-1 as routine documentation cleanup, record L4-A as accepted characterization, and use its pending rows to scope the separately reviewed replacement design and budget.
