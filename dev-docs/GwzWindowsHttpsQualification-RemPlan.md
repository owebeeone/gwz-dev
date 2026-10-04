# Windows qualification inventory correction

2026-10-04. Original State GO/zero findings and Code GO/one P3 are filed verbatim.
One bounded nonblocking correction; no product source change or new architecture.

| Finding | Disposition | Closure |
|---|---|---|
| Code P3-1 | Correct path normalization/component boundary; withdraw attribution to original inventory-final-v3. | Live control under the owned E: runtime is missed by original slash matcher (0), detected by corrected matcher (1); actual corrected collector inventory script exits1 while control remains alive. Slash/backslash/case spellings accepted; similarly prefixed sibling excluded. Held Popen child terminated and waited, owned control directory removed; exact same actual gate then exits0 with zero processes. Original reviewer verifies. |

Correction uses Windows `[IO.Path]::GetFullPath` on both operands, trims only
directory separators from root and appends one Windows directory separator,
then compares OrdinalIgnoreCase. The control copies system ping.exe only into
its newly owned external directory, starts that exact binary, checks it remains
alive through both predicate and actual inventory calls, and removes it after
held-child wait in finally. No broad kill, existing process or product change.

Private raw receipts: inventory-control-v2, inventory-corrected-v2. The former
passes the exact encoded `inventory_script()` used by the collector's normal
inventory phase, rather than merely testing a duplicate predicate. Original
invalid receipt and all original runner/source versions remain retained. Two
intermediate runner versions were reconstructed and verified byte-for-byte
against their original receipt/transfer hashes, not silently relabeled.

Both original verdicts remain GO on the public fixture object. This record does
not self-close P3-1; the original Code reviewer must verify the corrected tuple.
Full Windows HTTPS/activation/release remain NO-GO.
