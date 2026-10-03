# HTTPS SSPI composition — merged implementation remediation

2026-10-04. Round 1, one correction patch under the accepted architecture.
Implementation is **NO-GO** at root `b72dccf813816f41a508eb0fc2f9b2f071b323a0`
and the product tuple in the three filed reviews. Code and State independently
converged on pending-Start ownership, reaper visibility and lint attribution.
Code additionally found the CBT adapter and local-native-error projection.
Surface is GO with one bounded documentation finding.

## Dispositions and closure obligations

| Finding | Disposition | Required closure |
|---|---|---|
| Code P2-1 | Correct raw TLS digest → prefixed SSPI CBT inside a fixed initialized wiping owner; wipe dependency storage immediately, reject unsupported shapes, retain real validator. | Actual TLS producer/request construction must cross the real SSPI request validation boundary. Supported 32/48/64-byte digests admit; raw/malformed/unsupported representations refuse. A nonempty-only fake check is insufficient. |
| Code P2-2 / State P2-1 | Install owned Start retention before await. Abort revokes publication, transfers Start/outcome/receipt plus actual operation dependency and endpoint slot into existing retained cleanup; late Conversation is cancelled. | Abort production preparation while Start is pending after registration and separately before registration. Pending/Unknown retain both charges, seal blocks reacquisition, only confirmed/no-resource outcome releases; no completed future repoll. |
| Code P2-3 / State P2-2 | Make outstanding accounting include entries claimed outside the lock by a reaper; retain polling/proof/final drops outside locks. | Barrier-controlled reaper with concurrent real cleanup count/retirement observers and a second insertion. Pending/Unknown cannot expose zero; confirmed disposal makes zero legal. |
| Code P2-4 | Keep local Sspi identity/capture/launch refusal visible through existing request/Git clone/materialize projection; do not classify it as suppressible remote access refusal without remote proof. | Production native local refusal crosses the existing model/Git/private-materialize consumers without quiet skip or source fallback. Preserve accepted helper-absence and actual repository-refusal suppression; no application schema or secret diagnostic expansion. |
| Code P3-1 / State P3-1 | Correct introduced serve guard lint; replace inaccurate RED47/all-baseline claim with refreshed attributable evidence, retaining full-core RED if debt remains. | Refreshed strict output omits introduced guard warning; compare remaining diagnostics against range baseline, don't infer complete baseline proof from unchanged primary span alone. |
| Surface P3-1 | Supply exact synchronous/future teardown signatures and receipt fields plus a compiled caller cleanup recipe in the existing guide/Supervision docs. | Compiled cancel→receipt→cleanup_status→bounded shutdown accounting recipe; original Surface reviewer repeats its cold teardown walkthrough. |

No finding is disputed or self-closed. Original Code/State reviewers verify their
counterexamples on the corrected settled tuple; Surface verifies the bounded doc
correction. This is one merged patch, not independent per-finding packages.

Owner approves a private cross-crate fixture for the missing real CBT admission
seam: the synthetic public TLS certificate's produced binding crosses an SSPI
unit-test entry invoking the existing private request validator. No new public
validation API, protocol or production dependency. The probe must be bounded,
use external build targets, fail rather than silently skip when unavailable,
and be reproducible from public fixture/prepared inputs. Standalone SSPI tests
retain no neighbor/private-evidence dependency. This is product composition
qualification, not a compiler diagnostic or source-mutation probe.

## Scope and budget before remediation

No new wire field, public protocol, Supervisor/runtime owner, capacity domain,
activation, credential-source option, provider fallback or application schema.
The accepted physical lease, fixed D, source consent and wiping contracts remain.
Implement corrections in the existing owners. The actual inherited error consumers
were omitted from the source estimate: allow up to three additional existing
core files for projection and its private-materialize regression (37 handwritten
source/test files total, plus the authorized Python ledger), only if necessary.
No test/qualification obligation is descoped; broader platform work remains
deferred as before. The 3,500 added-line and eight API-document ceilings remain.
Stop before exceeding them and report the concrete required correction size.

Before ledger edits, owner permits one additional administrative reconciliation
in core `scripts/candidate_switch_inventory.txt`: the new private materialize
test site, plus correcting the existing Runtime site's syntactic owner after
the owning declaration moved. This is separate from the 37 source/test files
and the already authorized Python ledger; no semantic candidate-switch or
activation expansion. The actual guard must pass after reconciliation.

The first prompts mistakenly named a nonexistent member-local SSPI design path;
the root `dev-docs/GwzSspiDesign.md` §6 is the actual authority. Correct prompt
references at re-review. Preserve all initial reports verbatim.

## Gates and settlement

TDD regressions at the real producer/consumer and ownership boundaries, focused
affected core tests and caller doctests, source guards and strict changed-code
diagnostic attribution; refresh the supported host artifact after final core
source. Do not repeat unaffected transport/schema/SSPI broad gates without a
relevant source change. Record exact receipts and final manifest in the existing
implementation checkpoint, then stop editing for owner GWZ settlement.

One completed initial implementation review; zero completed remediation reviews.
The original reviewers classify any new architectural cause. The review-loop cap
remains two remediation rounds. Windows qualification, activation and release
remain NO-GO independently of this implementation correction. No push/tag/publish.
