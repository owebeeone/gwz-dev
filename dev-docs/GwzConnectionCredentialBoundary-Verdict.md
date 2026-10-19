# Connection–Credential boundary: merged verdict

Date: 2026-10-19. **GO — boundary/interface design only.**

## Exact review object

Root `0570a27ececdc62b83b2bd31065aaf20559914aa`, boundary revision 0, composed with both accepted API designs.
All eight reference member heads are identical in both review reports and the
[generated review package](GwzConnectionCredentialBoundary-ReviewPackage.md).

Reviewed boundary SHA-256: `108f78873c9d7f7f4c26d56d1fa897c2d8fc4c3928704f9ac8f36260d8e6e0f8`.
Reviewed Git blob: `09d18f82c7e4e0c91d8992a31e4b6b9df5dfd4d8`.

Only the acceptance header/links and review records change after the review.
The substantive boundary body is identical. Neither accepted API document nor
any source member is changed by this package.

## Independent verdicts

| Axis | Verdict | Findings |
| --- | --- | --- |
| [Consistency with Surface](GwzConnectionCredentialBoundary-ReviewConsistency.md) | GO | 0 P0–P3 |
| [Safety](GwzConnectionCredentialBoundary-ReviewSafety.md) | GO | 0 P0–P3 |

Both returned GO on the initial boundary revision. Zero remediation rounds and
zero new architectural roots. The same two prior reviewers were reused under the
operator's instruction, with current-round peer blindness. No extra reviewers or
cross-model diversity is claimed. Reports are filed verbatim.

## Accepted separation

- Connection retains its four HostPorts and physical-resource authority.
- Credential is independent of Connection/core/Git/transport; optional Windows
  provider delegation to gwz-sspi remains permitted.
- A narrow executing-endpoint adapter composes them. Driver placement keeps
  CredentialContext, keys, helper tokens and original caller capture at the driver;
  core receives only the existing route/virtual-stream surface and sanitized facts.
- Peer acceptance, publication, scope revocation, native cleanup and physical
  disposal have separate outcomes and matching resource accounting.
- Fake library/composition tests do not substitute for real production bridge,
  native provider or installed placement evidence. Test-cost reduction is unproved.

No accepted API amendment, new protocol, supervisor, cleanup domain, common
contract crate or dependency upgrade is selected.

## Stop and next handoff

The operator clarified document and review BEFORE Credential implementation.
That requested deliverable is complete. There are no source changes, repositories,
crates, tests/builds, native experiments, push, tags or publication from this task.
Python remains paused until builder closeout; builder lanes were untouched.

The next proposed work is the separately scoped portable Credential implementation
with fake providers, clocks, peer facts and retained cleanup. Do not begin a full
Connection implementation, provider migration or release programme implicitly.
