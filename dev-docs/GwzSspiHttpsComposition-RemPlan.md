# HTTPS SSPI composition — bounded remediation

2026-10-04. Status: correction applied, independent closure pending; acceptance and implementation remain
NO-GO. The reviewed root is `fe40ba9b23e31da95358441ae5214a7cadb31b31`;
member HEADs are unchanged from the exact tuple in the filed reports.

## Review disposition

- [Consistency](GwzSspiHttpsComposition-ReviewConsistency.md): NO-GO, one P2.
- [Surface](GwzSspiHttpsComposition-ReviewSurface.md): GO, no findings.
- Safety: attempt ended at the account usage limit without a report or verdict.
  It is an incomplete review, not GO, NO-GO or a completed remediation round.

No blind convergence or escaped implementation defect is established. This is
an unaccepted documents-only proposal; no HTTP composition source/schema was
changed. The existing native worker and host acceptances remain separate.

## Merged correction

| Finding | Disposition | Closure obligation |
|---|---|---|
| Consistency P2-1: native facts conflate mechanism authority with token completion | Preserve the actual mechanism observation's authoritative flag, including Continue; track native Complete independently in the core producer's success check. Correct both validity and success clauses in one text patch. | Original reviewer must recheck its authoritative-NTLM Continue counterexample and all five requested projection/publication cases on the revised committed draft. Future bridge tests and independent wire vectors must enforce them before implementation acceptance. |

The lane owner applies this bounded documentation correction because the usage
limit prevented completion of the review cycle. That is not independent closure
or self-GO. No new API, schema field, runtime owner, dependency or clock choice
is introduced by this correction; Surface's reviewed caller guide is unchanged.

## Resume boundary

The correction/audit checkpoint is committed at root `1468517adcfcaa88c8185d674ed7c6d09db877c6`.
The operator has now directed the owner to finish the reviews, settle timeout
zero and implement after acceptance. The owner chooses native refusal at zero;
this matches the previously reviewed proposed behavior without a new allowance.
Reuse Consistency for its focused re-verdict and complete the missing independent
Safety review on the corrected exact tuple. Generate revised canonical prompts;
do not reuse a prompt naming the original root against corrected HEAD.
No dependent implementation starts before the required verdicts. Full Windows
activation and release remain separately gated; the directive does not waive them.
