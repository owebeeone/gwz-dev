# Python caller guide — Surface re-review

**Scope:** Read-only review of the committed caller guide at root `c5b497bc0934b8970d5abba7022b5da020ff7f72`, compared with its version at `ee11f44efa5a0796a71d874ffaa4bc570a611a4e` and the committed `gwz-py/README.md`. No source, internal design, amendment, or remediation plan was inspected. All four required HEADs matched the supplied tuple at both the start and end of review.

**Changed-range analysis:** The guide now gives a full `start_fetch` argument list, makes local placement the public Python path, supplies an event resume cursor, names cancellation and terminal discriminants, and defines cleanup report fields. It also changes the cleanup advice: either incomplete field now directs the caller to close the shared client. That new advice creates the blocking finding below.

| Prior finding | Re-verdict | Evidence in corrected guide |
|---|---|---|
| P2-1: late event reader silently loses history | Closed | The default cursor is captured at acceptance; late readers receive `EventCursorGap`, and `after_sequence` is documented. |
| P2-2: metadata arguments absent from signature | Closed for argument parity | The full proposed argument list and Python defaults are shown. Effective behavior for several `None` defaults remains a new P3-1 finding. |
| P2-3: cancellation and terminal discriminants absent | Closed | `OperationCancelled` and `OperationTerminalKindV1.cancelled` are shown, with the terminal kinds listed. |
| P3-1: no public path to bind `cli` placement | Closed | The guide says `cli` placement is reserved for an internally bound endpoint and offers no public binding API. |
| P3-2: incomplete cleanup has no defined recovery | Not closed | The fields and recovery instruction are now defined, but following that instruction cancels a peer operation; see P2-1 below. |

## §0 — Verdict

**NO-GO for the proposed Python caller interface.** One P2 conflict remains in the documented two-operation lifecycle. This is a surface judgment only; it makes no claim about implementation or release readiness.

## Findings

**P2-1 — Incomplete cancellation recovery breaks the promised isolation of peer operations. NEW ARCHITECTURAL root cause in the documented interface.** The `cancel()` paragraph says that if `pending_local_work > 0` **or** `peer_cleanup_confirmed is False`, the caller should await `client.close()` to join shared cleanup. The same guide says cancellation of one handle leaves peers running, while leaving or closing the client cancels outstanding work. Start two long-running fetches, cancel the first, and receive either incomplete cleanup field. Following the stated recovery path closes the shared client and cancels the second fetch before its result can be obtained. The violated invariant is that recovery from one handle’s cancellation does not silently turn into cancellation of its peer. Provide an operation-local way to finish or await that handle’s cleanup while preserving the second operation. If client-wide shutdown is unavoidable, state that limitation at `cancel()` and remove the unconditional peer-isolation promise. A regression scenario should keep a second operation live, produce an incomplete cleanup report for the first, follow the documented recovery path, and verify the stated effect on the second.

**P3-1 — Several listed defaults do not state their effective behavior.** The new signature gives `None` defaults for `all_members`, `dry_run`, `partial`, `destructive`, `sync`, and other options, then says those values retain `Client.fetch(...)` resolution rules. The permitted README does not state those rules or link to a Python fetch API reference. On a first `start_fetch()` call, a reader cannot determine from these user-facing documents which members will be selected or which policy applies. State the resolved behavior for each `None` default, or link directly to a user-facing reference that does. A documentation regression check should let a reader predict the member selection and policy of the guide’s no-argument `start_fetch()` example without inspecting source.

The install, start, event-gap recovery, result, terminal, and release steps are otherwise traceable from the corrected guide. I pre-commit to **GO** on a revision that resolves P2-1 as specified, provided it introduces no new blocking surface defect.
