# SSH agent A2 — remediation 1

Status: implemented; retained re-review pending. State GO; Code NO-GO on core 131989bc.

| Finding | Disposition | Closure |
|---|---|---|
| Code P2-1 | Fail closed on ambiguous PUBLICKEY_UNVERIFIED; only explicit AUTHENTICATION_FAILED may advance to another identity. Preserve a non-credential error for an ambiguous native failure. | Pause the native peer, shut down test TCP on the first native EAGAIN, observe PUBLICKEY_UNVERIFIED and assert no subsequent call with a second identity. Retain successful rejected-first-key authentication. |

The observer is exposed only to tests under an enclosing cfg block; the private
implementation takes a no-op observer from its production entry. It records the
actual native key/return boundary without replacing the native API or its errors.
No signing algorithm, ownership, allocator, wire or production scope change.
One merged patch; focused gate then full gate, same Code reviewer closes P2-1;
State verifies directly affected fail-closed behavior on the same corrected tuple.

Focused eight-test and full 62-execution gates pass. 267 source lines (including
test-only entry), 617 test/support lines. Test ceiling refined from 560 to 620
for the required disconnect regression and recorded in the controlling checkpoint.
