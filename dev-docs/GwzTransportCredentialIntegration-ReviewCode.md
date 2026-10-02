# GwzTransportCredentialIntegration — Code Review

Date: 2026-10-03  
Axis: Code  
Verdict: **GO** for the interior test-only integration correction.  
Findings: **0 P0, 0 P1, 0 P2, 0 P3.**

The two test changes restore assertions for already accepted production behavior without weakening the historical wire guard, helper refusal, proxy refusal, or credential redaction checks. This verdict does not establish that the pending full Python integration suites are green.

## Reviewed object and tuple

Workspace: `/Volumes/projects/limbo/gwz-dev`

The following exact heads matched the canonical prompt at both the start and end of review:

| Member | Head |
|---|---|
| root | `5be088f50808e930cc78be8db0671486256dac4a` |
| gwz-core | `a92a7990081475e349b182a332c7553731f4fb5b` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |
| gwz-py | `5aeff4bfb11da048f1174db2b1045ed0c9b6c80a` |
| gwz-cli | `f925e1165c2b2d368a00277594450b010a95867a` |
| gwz-core-evidence | `662d89828b478a2acce8c0308834db7d17c872f7` |

The complete Python range `a0d4350f31069362b3cfbeae668666ab47567264..5aeff4bfb11da048f1174db2b1045ed0c9b6c80a` changes only:

- `src/tests/test_log_protocol.py`
- `src/tests/test_client_host_transport.py`

Root changes supply the integration remediation plan, checkpoint, and managed member capture. No production, generated protocol, native binding, transport, or CLI change is hidden in the Python range.

Tracked worktrees were clean at verification. Preserved unrelated root/core drafts and the existing private evidence run remained outside this review.

## Closure and changed-range assessment

| Cause | Status | Evidence and retained invariant |
|---|---|---|
| IG1: stale historical projection omitted accepted code 75 | **Closed** | `test_log_protocol.py:364–370` requires exact names and values for `cancelled=73`, `transport_record_limit=74`, and `credential_helper_timeout=75`, removes only those members, then checks the unchanged historical hash. |
| IG2: fixture assumed automatic gh discovery and an obsolete error string | **Closed** | `test_client_host_transport.py:318–329` configures the disposable helper through conventional Git configuration, clears inherited helpers, retains the exact invocation and one-request assertions, and checks typed `RemoteRejected` plus the accepted no-helper-credential message. |
| Previously accepted production credential implementation and runner finalization | **Unchanged; prior closure retained** | This range cannot alter SSH terminal publication, challenge projection, credential allocation, helper admission/cleanup ordering, private helper policy propagation, pool partitioning, or owned failure retention. Prior production evidence was reused within the authorized narrow scope. |

### Historical wire compatibility

I attacked the projection for an exclusion that could conceal an incompatible schema change.

The new code does not filter arbitrary error codes or replace the historical hash. Each permitted additive member must exist under its exact name and occupy its exact value; a missing or misnumbered member raises before hashing. An additional unapproved member remains in the projection and changes the hash. Existing fields, shapes, and slots continue through the unchanged historical projection.

The retained `PRE_LOG_WIRE_SHA256` is:

`4d377a496c8905293b5e9b53392b70867cf6dafccbb623841a623dbd2d555f14`

The accepted production `check_protocol_drift.py` and core additive protocol check use the same exact 73/74/75 member allowance. The correction aligns the Python test with those guards and the accepted schema.

### Configured helper and failure carrier

I traced the changed test through its candidate loader, host call/error-code helper, HTTPS fixture, and fake gh process.

The test writes `HOME/.gitconfig` inside pytest’s disposable directory. Its empty `credential.helper` entry resets the inherited helper list before `!gh auth git-credential` selects the fixture. This matches the accepted configured-helper policy; placing a fake executable on `PATH` alone no longer supplies the authorization premise.

The fake gh process records its arguments, consumes the request, emits credential-shaped secret material and stderr text, then exits unsuccessfully. The test still requires exactly `auth git-credential get` and exactly one HTTPS request. Its typed `RemoteRejected` assertion matches the accepted unauthenticated HTTPS failure carrier, while the message assertion verifies the no-credential outcome.

The later assertions are preserved:

- The authenticated unsupported proxy produces `InvalidRequest`.
- The HTTPS request count remains one, proving refusal before another connection.
- The fake secret and `fixture:` credential marker remain absent from both failures’ visible exception, machine message, and detail fields.

The source assertion concerns the no-helper-credential outcome. The prompt and focused receipt’s “M4” shorthand does not turn this fixture into a timeout test; timeout code 75 is checked separately by the wire projection.

## Evidence examined

No tests, builds, probes, source writes, or Git mutations were performed during this review. I inspected the complete two-file diff, necessary fixture/caller contracts, controlling integration documents, accepted helper/error contracts, and recorded validation evidence. No current peer report was consulted.

The supplied report hashes were verified:

| Record | SHA-256 | Recorded result |
|---|---|---|
| Pre-correction `report.json` | `a1566dab7cfeb753649c02907cc1b3f00dce8c80fa65085f7a09f53ec8659a0a` | Ordinary 991 passed/1 failed/18 skipped; both candidate 1008 passed/2 failed |
| `correction-focused/report.json` | `4338e83c729b010a429de5c0cb3aa2bc53ce9bd31dbaa4d677638b85dd5b3e14` | Ordinary protocol 3 passed; both candidate forms failed at the stale Authentication substring after the configured invocation assertion |
| `correction-focused-v2/report.json` | `8a5dafef347afb94ea86cfc38eb3c12e29d5df3848b918125d84399f403ada02` | Both candidate 4 passed; transport candidate 4 passed |

Recorded focused log hashes matched their files. The v2 runs select the complete log-protocol file and the corrected HTTPS/helper/proxy test; their four-pass results therefore reach the retained proxy and redaction assertions. The ordinary three-test result remains reusable because its test file is unchanged between focused revisions.

The final two test-file bytes match the v2 receipt’s start and end snapshots. The ordinary, both-candidate, and transport-candidate native module files also match the recorded unchanged SHA-256 values. This supports reuse of the existing modules for a test-only correction; it is not a package or release qualification claim.

## Remaining integration work

Run and record the final full ordinary, both-candidate, and transport-candidate Python suites against the settled tuple. Their results remain pending and must not be inferred from the focused passes.

Within the reviewed range, IG1 and IG2 are closed, no concrete new defect was found, and the Code verdict is **GO**.
