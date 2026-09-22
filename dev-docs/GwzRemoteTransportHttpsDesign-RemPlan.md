# HTTPS design — consolidated correction 1

Status: **closed after correction2**, retained Consistency/Safety/Surface GO at
root bcdca800ab19fb767f6e7d2ab8107f12dab48810 / core
2ea02835a15a9f56afdda43ccbcadec66b5b776e. Final -Review*-2 reports close all
findings. Documentary closure only; implementation oracles remain H1/H2 work.
Initial object: root c17b93b1e6197b1f40e2bb7f25ade8c478971c64 / core
bcc2ebf6f6652d4f37429c5cbb6e68f987006b94. Initial reports are filed verbatim.
No code/tests/dependencies changed. The operator requested retained reviewers.

| Finding | Disposition in one corrected object | Closure oracle |
| --- | --- | --- |
| Consistency P2-1 + Safety P2-2 | Accept; explicit narrow Requirements C3/Placement §6 exception for once-only anonymous discovery401/404 -> Gh. HTTPS §4 retains first receipt, same placement/budgets, final-only public projection. Forbid other GET retries, all POST/network-failure replay. | Re-read authority and transition; H1 counts request/helper calls for401/404/5xx/loss/POST, disabled helpers and exhausted cumulative budget. |
| Consistency P2-2 | Accept; explicitly extend Placement §6 with HTTPS §7 final discovery403/404 predicate (TLS/headers, matching status, no response bytes, Effect::None). POST failures never enter private suppression. | Re-read exact predicate and mapping; H1/H2 distinguish401,403/404, malformed success, trust/loss/timeout, intermediate404 and uncertain POST. |
| Consistency P2-3 | Accept; amend Requirements G4, Transport Design §3.3/matrix, baseline GWZ summaries and Plan. Allow only exact action-matching generated service query on discovery redirects; base URL/Destination still query-free. | Re-read one grammar throughout; H1 absent/exact/duplicate/unrelated/encoded/fragment cases. |
| Safety P2-1 | Accept; no new remote-id field. HTTPS §5 makes route write-once per operation/destination/service, installed before advertisement exposure. Conflicting discovery fails; records cannot be erased by individual stream retirement. Bound record admission. | Barrier two same-key remotes with different bases; only pinned-base discovery succeeds, losing remote sends no POST; individual cancel cannot erase/overwrite surviving route. |
| Safety P3-1 | Accept in same correction; §7 exhaustive status table, success200 only, bounded interim responses, no implicit retry, discard all terminal error connections. | H1 representative informational/204/3xx/407/429/5xx matrix checks code/effect/disposal/request count/redaction. |

Owner inspection found two related implementation constraints while consolidating:

- A redirected GET cannot truthfully expose the first connection as its final
  Opened identity. Delay GET Opened until final headers/route admission (no body
  dependency); keep POST Opened before body. Truthful reused+Gh offered facts then
  require a narrow validator/projection amendment. HTTPS §10 names it and the
  bound Open policy check explicitly; update all general SSH-only reuse wording
  and the embedding guide. Existing tags unchanged. H1 tests final connection
  attribution, reused Gh truth, and unchanged SSH refusal. Add retained Surface
  review of the revised observation semantics; constructor freeze remains later.
- Independent per-scheme full-size pools would multiply endpoint limits. §6
  explicitly requires one aggregate reservation authority across SSH+HTTPS,
  tested with mixed-scheme saturation. Physical owners may remain distinct.

Blind convergence: both initial axes independently found the no-replay conflict.
There are four distinct initial blocking roots and one nonblocking status-grammar
finding. This is the first consolidated correction. Documentary closure now means
implementable, coherent requirements; all listed behavioral tests remain H1/H2
obligations, not tests claimed to have run. Platform/selected-source checks remain
operator-deferred together; wire/iroh, production activation and release stay out.

## Correction 2 — retained Consistency P2-4

All initial findings are closed by their raising reviewers at correction1.
Consistency identified one newly introduced message-lifecycle defect: the delayed
GET Opened path named Failed during Opening. Replace it with existing v2 OpenFailed
and Failure.facts. This is a bounded protocol-name/lifecycle correction, not a
new architecture or interface. H1's added oracle sends401,403/404,5xx, malformed
headers, trust/loss and exhausted-budget cases through the real mux, accepting
OpenFailed with available facts and no Opened/Stream transition. Typed first
receipt must remain usable for the explicit authentication transition.

This is the second consolidated correction. Consistency rechecks its original
counterexample; Safety and Surface only confirm their prior GO remains applicable
at the revised tuple (public guide bytes unchanged). No implementation tests run.
