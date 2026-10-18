# HTTPS H1 consolidated correction 1

Status: implementation in progress; initial aggregate Code and State NO-GO.
Object: root ead57a6353daa145d7372231c22e9d058f4cf085, core
280f970a66047942b006cfecfae51db60fa0550b, transport
aa40936d0805e8cb60f8027615abe20d4f2045e4. Initial reports are filed verbatim
alongside this plan. One consolidated patch; retained reviewers re-check their
own counterexamples on the same corrected committed tuple. No finding is closed
by the implementer.

| Findings | Disposition | Causal closure test |
|---|---|---|
| Code P2-1; State P2-2 | Accept. Explicit whole-operation sealing and dependent lifetime accounting preserve write-once redirect routes. Per-remote drop cancels only its work. | Distinct LocalRpc owners A/B/C share an operation; A drops after pinning RA, B keeps RA, C cannot pin RB. Explicit retirement rejects new work and waits for dependencies. |
| Code P2-2 | Accept. Separate allocation, interaction, connection, active-I/O and cleanup budgets. Capture active-I/O configuration including zero and shortening. | Slow helper survives a short uncontended allocation allowance; contention expires separately; zero/custom I/O values and redirect/auth budget carry are exercised. |
| Code P2-3 | Accept. Classify Hyper protocol/parse failures separately from transport loss, preserving POST effect. | Actual verified TLS malformed headers/status produce Protocol; disconnected peer produces Io, including opening translation. |
| State P2-1 | Accept. Endpoint-scoped admission/cancellation owns active preparations and retained helpers. Shutdown closes admission before cancelling and accounts for all owned work. | Started helper is cancelled/reaped or reported pending; post-shutdown lookup never spawns; two endpoints retain isolated cleanup ownership. |
| State P2-3 | Accept. Compose actual HTTPS preparation with existing mux Open/Opened/OpenFailed, including anonymous failure before the permitted retry. | Status, malformed header, trust, network and deadline failures pass through actual preparation and mux; exactly one terminal opening receipt per attempt, no stream on failure. |
| State P2-4 | Accept. Credential-offered is cumulative across redirects; authentication/status remain final-attempt facts. | Origin A sees sentinel Authorization; B lookup/connect fails; terminal opening receipt retains offered=true without secret/URL exposure. |
| Code P3-1 | Accept. Enclose the new test module conditional explicitly. | Scoped syntax-aware check includes disabled branches; no wider platform qualification claim. |
| Code P3-2 | Accept. Replace pre-cancel-only evidence with a started-child barrier. | Cancel after helper starts, then prove bounded reap and admission recovery. |

Blind convergence: both axes independently found the operation-wide route
retirement defect. There are six distinct blocking roots in the initial gate.
All corrections remain inside the accepted candidate H1 contract. H2 host
embedding/shared authority injection, platform and selected-source batch,
physical wire, public construction and release remain deferred. Record new
failed and passing attempts in a separate private campaign run; preserve the
original evidence and the additional twelve-seed unchanged-source replay.

Implementation detail: operation sealing blocks new dependents; remote owners
and admitted preparations retain independent dependencies, so route deletion
waits for their final drop. Endpoint shutdown first closes admission, then
cancels helper work and pool work; held preparations and retained children are
reported separately until they retire. Future abort uses an owned-child guard
so a killed but unreaped helper retains its capacity permit.

Credential-offered facts are a monotonic OR over discovery attempts at redirect
origins. Status and authenticated describe the current/final origin only; a
failure before that origin responds has no HTTP status or authentication proof.
Secrets and raw redirect URLs remain absent from receipts.
