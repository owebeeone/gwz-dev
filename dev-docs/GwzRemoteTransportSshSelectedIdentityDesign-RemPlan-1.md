# SSH N2 selected identity design — remediation 1

Date: 2026-09-21. Status: focused retained re-review pending.
Base root `6cb03c3049aae85b66a254208a6084e41d04a027`, core
`120fb9f1379025c66ea00cbdfd5af7f5815859db`; other pins unchanged.

## Owner P2-O1: unique concurrent candidate tokens defeat first-fan-out reuse

Original design §1 required different tokens for concurrent unproven snapshots.
Concrete sequence: admit A and B for the same Key and identical selected bytes
before either authenticates, assigning T1 and T2. Pool capacity is one. A creates
and authenticates C1 with T1. B waits with T2. When A releases C1, allocation.rs
requires identity equality, so it cannot give C1 to B; at capacity it evicts C1
and creates C2. Extend to a whole member-repository fan-out and the design pays
one connection per repository, defeating the central performance objective.
This is an owner finding before acceptance, not a reviewer or escaped-code finding.

Disposition: canonicalize live same-Key/exact-byte candidates as well as proven
entries. An opaque token names admitted immutable material; only the independently
authenticated reusable physical resource can supply a lease. Promotion remains
joined, live and worker-owned. Each request/connection keeps independent cancellation
and deadline; a failed sharer cannot poison or promote another sharer's authority.
No change to public/wire/pool shapes, input limits or budgets.

Closure: both retained axes trace the revised compatibility/proof boundary and
its agreement with pool state. N2b must admit a same-Key identical-snapshot batch
behind a native authentication barrier at capacity one, then prove a single
physical connection serves all successful exchanges. Also prove no early lease,
changed-key isolation and independent failure/cancellation of sharers.

This is one merged design remediation, with unchanged N2a/N2b implementation
gates and the two-round cap. Initial reports and revised prompts are filed verbatim.

## Safety P2-1: unsupported encrypted native KDF can occupy all helpers

Disposition: add a bounded framing-only preflight before interning/checkout and
native calls. It accepts a single unambiguous allowed unencrypted container and
rejects encrypted PKCS#8, encrypted traditional PEM, encrypted OpenSSH, unknown
or mixed forms. Fixed256-byte scratch is reserved with the read buffer; streaming
base64/framing has no work-factor loop or parameter-sized allocation. Native
parsing remains the accepted unencrypted key usability authority. G1 enumerates
the compatibility restriction. No cryptographic implementation or decryptor.

Closure: Safety verifies the rejection boundary against its exact KDF sequence.
N2a tests include extreme valid PBKDF2/bcrypt work factors and native-auth call
observers proving zero invocation; pressure tests prove helper/byte recovery,
truthful disposal and active-stream progress. Normal unencrypted forms still
complete native authentication. Input parsing boundaries/multiple-block ambiguities
are explicit adversarial cases. Do not execute the extreme KDFs to prove refusal.

N3/backend/platform work remains excluded. N2a production stays600 lines across
three modules; its focused test allowance800→900 is for this adversarial matrix.
Initial Consistency GO/no findings and Safety NO-GO/one P2 are filed verbatim.
Owner P2-O1 is independent, not blind reviewer convergence. No design acceptance
preceded either finding. Both retained axes re-review this single corrected tuple.
