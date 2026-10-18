# GWZ Remote Transport HTTPS Design — CONSISTENCY-AXIS REVIEW

**Review object:** Documentation-only HTTPS design at root `c17b93b1e6197b1f40e2bb7f25ade8c478971c64`, core `bcc2ebf6f6652d4f37429c5cbb6e68f987006b94`, `gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md`; DRAFT pending review, 2026-09-22.  
**Baseline:** root `c17b93b1e6197b1f40e2bb7f25ade8c478971c64`; core `bcc2ebf6f6652d4f37429c5cbb6e68f987006b94`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Sources were read from immutable Git objects.  
**Date:** 2026-09-22  
**Axis:** Consistency—the document against its controlling requirements, designs, plan, checkpoint, and implemented interfaces. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — three P2 contract contradictions block design admission. I pre-commit to GO on a revision that resolves P2-1, P2-2, and P2-3 as specified.

---

## 0. Evidence base

I read the complete HTTPS design and its Phase 5/checkpoint additions; Requirements G4/C3/C8/P4; Transport Design §§3, 4, 6, 7, 9, 10, and 11; Placement Design §§4–8; the generated transport protocol, policy compatibility matrix and pool API; the pinned git2-rs smart-subtransport bridge and libgit2 RPC call graph; current per-remote core integration; and the owner-linked [gh credential-helper source](https://github.com/cli/cli/blob/trunk/pkg/cmd/auth/gitcredential/helper.go).

The nine commits and trees were verified at review start and end and did not move. Member worktrees were clean; root contained only the permitted generated prompts. No build, test, probe, or write was performed.

## 1. Findings

### [P2-1] Anonymous-to-Gh discovery retry contradicts the accepted no-replay contract

**Location:** HTTPS Design §4, lines 106–115 and §7 line 264; Requirements C3, lines 210–214; Placement Design §6, line 320.

The new design automatically retries a transmitted discovery GET once under `Gh` after an anonymous 401 or 404. C3 requires a failure after transmission to surface without automatic replay, and Placement Design says no failed exchange is automatically retried. The HTTPS document neither supersedes those clauses nor defines the first stream as a nonterminal authentication transition.

Reproduction: a helper-enabled `UploadPackLs` sends anonymous GET, receives 404, terminates that transport stream, then silently opens a second Gh stream. Implementers can either follow the new retry or the retained no-replay rule, producing incompatible behavior, observations, and budget accounting.

Either select Anonymous/Gh before the first Open, or explicitly amend the exact controlling clauses with a bounded prepublication discovery-auth exception. The amendment must define first-attempt terminal/facts handling, cumulative budgets, observation retention, and why POST/network-failure replay remains forbidden. Close with tests for anonymous 401/404→Gh, helpers-disabled single attempt, cumulative deadline exhaustion, and zero POST replay.

### [P2-2] HTTP status broadens `RepositoryRefused` beyond its frozen authority

**Location:** HTTPS Design §7, lines 258–264; Placement Design §6, lines 312–320 and §7 lines 346–354.

The HTTPS design maps final 403/404 responses to existing `RepositoryRefused`. The controlling placement contract restricts that code to the N3 SSH canonical-refusal proof: complete recognized stderr, no stdout, and completed command status. Core then maps this code to `RemoteRejected` and private-member suppression. This is a semantic wire-contract expansion, despite the new design claiming no protocol amendment.

Reproduction: a final HTTPS 403 produces code 13 and can enter private-member suppression even though the currently authorized proof predicate cannot hold for HTTP. A conforming implementation following Placement Design must instead reject that classification.

Amend Placement Design’s authoritative predicate to define scheme-specific HTTPS conditions, including eligible final statuses, required `Facts.http_status`, authentication state, effect, and exclusions for Trust/Io/Protocol failures; alternatively use a non-`RepositoryRefused` disposition. Regression coverage must distinguish 401, 403/404, malformed success, trust failure, network loss, anonymous retry intermediates, and final private-member behavior.

### [P2-3] Redirect query acceptance conflicts with the categorical destination grammar

**Location:** HTTPS Design §5, lines 158–173; Requirements G4, lines 179–185; Transport Design §3.3, lines 185–204 and acceptance matrix line 807; `GWZRequirements.md:45` and `GWZDesign.md:47`.

The accepted contracts reject all HTTPS query components, including redirects. The new design permits a redirect Location containing exactly the generated `service=` query and strips it to derive a repository base. That server-supplied Location is still a redirect with a query; the detailed document silently narrows the categorical rule.

Reproduction: `Location: https://new.example/repo.git/info/refs?service=git-upload-pack` must be accepted by the HTTPS design and rejected by every cited controlling clause.

Choose one grammar. Either reject every queried Location and append the service query only after validating a query-free redirected base, or amend all controlling clauses and the acceptance matrix with the exact single-pair exception. Tests must cover absent, exact, duplicate, unrelated, encoded, and fragment-bearing redirect queries.

## 2. Invariant analysis

Other attacked boundaries held. Existing `Scheme::Https`, `AuthPolicy::{Anonymous,Gh}`, four `GitService` actions, structured `Destination`, `Facts.http_status`, and the generic pool can express H1/H2 without new fields. RPC-mode git2 creates a fresh stream for each advertisement/exchange action, matching the proposed mapping. Opened-before-body/response ordering avoids POST deadlock; EndWrite remains the only body terminator. Endpoint ownership, host/port pool keys, request-scoped credentials, redirect-origin isolation, bounded helper jobs, cleanup accounting, and local/CLI placement agree with the retained architecture. Candidate-only dependency/configuration work and later public Surface activation are clearly separated.

## 3. Risks and next action

Library, platform, selected-source, native parity, physical carrier, activation, and release evidence remain correctly deferred. Correct the three authority conflicts in one documentation patch before H1; no implementation or schema change is needed to close this review.
