# R4b-P third remediation filesystem-interface focused re-review 2

Date: 2026-08-12

Reviewed:

- the revised `GwzM5-8R4bP1P2-RemPlan-3.md`, especially sections 3.1-3.4,
  R1/R2, and exit criteria 1-5;
- every finding in `GwzM5-8R4bP1P2-RemPlan-3-ReviewFS.md`; and
- the settled checked-artifact/platform implementation only where needed to
  verify that the revised interface is implementable.

This was a focused read-only design review. I did not modify the plan or
production/test code.

## Verdict

**NO-GO for R1 implementation.**

The revision materially closes the full-width Windows `FileIdInfo`, bounded
private protocol, scratch ownership, pre-residue rename-domain check,
durability ordering, and cleanup-concurrency findings. No v1 wire change is
required. One original P1 namespace defect remains, however, and two P2
restart/platform contradictions prevent sections 3.1-3.4 from being a closed,
portable implementation contract.

## Findings

### [P1-1] Parent replacement still changes the artifact-family lookup key and hides the old authority

**Where:** RemPlan-3 lines 131-152, especially lines 139-143.

**Unclosed prior finding:** ReviewFS P2-2 required a stable artifact-family key
derived from artifact-root identity plus the platform-equivalent canonical
managed **path**, with retained-parent object identity held only inside the
authority. The revision instead says canonical path identity “uses the
retained parent identity plus the filesystem-equivalent leaf name.” Because
`artifact_family_key` incorporates that canonical path identity, replacing the
parent creates a different family key.

**Failure scenario.** Publish authority and a durable staged goal for
`dir/leaf`, then stop. Move `dir` aside and create a new directory at `dir`.
Reacquisition observes a new parent object identity, computes a new canonical
path/family key, and cannot enumerate the old family. It may publish a second
authority and mutate a missing or same-byte foreign leaf in the replacement
parent. That is the exact staged-goal rebinding defect reported as settled-tree
Faults P1-1; the new authority record contains the old parent identity, but it
cannot reject the mismatch if lookup never finds that record.

**Required correction.** Make `artifact_family_key` stable across parent
object replacement. Its path component must be a canonical, filesystem-
equivalent root-relative component sequence independent of the current parent
object identity. Persist and compare `retained_parent_identity` only after the
stable family has been found. The path-equivalence seam may use a parent handle
to query case/normalization rules, but it must not place that handle's object
identity in the family key. Add the exact stop-after-authority/goal, replace-
parent, reacquire-by-same-and-alias-spelling cases; each must find the old
family, classify `Ambiguous`, retain all names, and perform no managed
mutation.

### [P2-1] Persisted mount/device identities are not defined to survive the machine failures for which the barriers exist

**Where:** RemPlan-3 lines 73-100, 115-128, 161-175, 630-644.

**New contradiction.** The authority persists Linux `mount_id` and Unix
`device` as exact identity facts. Linux mount IDs are namespace-lifetime
identifiers and can change after unmount/remount or reboot; Unix device numbers
are not universally stable filesystem identities across boot. A macOS `fsid`
and the Windows alternative “volume-guid-or-serial” likewise need an explicit
stability/equality contract. The plan calls the authority restart-closed and
spends substantial machinery on storage durability, but does not say whether
a machine restart that changes these ephemeral identifiers must recover or is
allowed to become permanent ambiguity.

If equality with the persisted numeric mount ID is required, a normal reboot
can make the exact authority non-current even though the same source, parent,
private root, and durable names survived. The C7 promise of restart to terminal
exhaustion then cannot hold. If equality is not required, recording the value
does not bind the durable authority as exit criterion 1 claims. A Windows
volume serial alone also cannot be assumed to prove a rename domain without a
stated uniqueness guarantee or a pre-residue operation proof.

**Required correction.** Separate the invocation-local
`RenameDomainProof`—Linux mount ID or equivalent used only to prove the two
currently opened handles share an atomic rename domain—from the durable
filesystem/object identity persisted in authority. Freeze whether the latter
is guaranteed stable across the supported crash/reboot model. Use a durable
filesystem identifier plus full object identifier where the platform provides
one, or reject the filesystem before authority publication if automatic
post-reboot identity recovery cannot be guaranteed. If machine reboot is
deliberately outside the recovery promise, say so explicitly and narrow the
durability/restart exit criteria; silently converting a valid reboot into
`Ambiguous` is not consistent with the current terminal-restart claim. Windows
must use a volume GUID or a demonstrated same-domain probe rather than an
unqualified “serial” alternative.

### [P2-2] The staged-goal and Windows barrier states contradict the literal closed classifier

**Where:** RemPlan-3 lines 177-180, 197-200, 202-233, 235-265.

**Same-identity duplicate contradiction.** Section 3.3 states that any
duplicate family goal is foreign and `Ambiguous`. Sixteen lines later it
introduces a same-full-identity crash duplicate as
`RecoverableDuplicate`, and the literal table admits a same-identity staged/
managed goal. Implementers and matrix authors cannot satisfy both rules.

**Barrier-state omission.** The Windows contract introduces a pinned
per-family barrier entry, while section 3.2 says the complete family is
enumerated and any malformed/foreign entry is `Ambiguous`. The classifier
table has no barrier-entry fact or rule. If the pinned entry survives—as
“pinned” implies—it can make an otherwise clean `Before` or `After` family
ambiguous; if it is retired, its own crash/durability states are absent from
the table and fault vocabulary. An unrelated barrier name outside the family
would avoid the table issue but then needs explicit ownership and causal
binding to the family transition.

**Required correction.** Change the duplicate rule to distinguish exactly one
authority-bound, same-full-identity source/destination alias from any
different-identity, extra, or malformed duplicate, and define its precise
operation-specific table rows. Define the Windows barrier entry as either
recognized immutable family infrastructure (add it to authority, residue
classification, cleanup order, and faults) or a separately owned platform
primitive whose lifetime cannot affect family enumeration. Pin exact bytes,
identity, creation/publication durability, reuse, and retirement semantics.
The operation-specific exhaustive tables must cover every barrier/duplicate
state that a crash at a named boundary can leave.

## Prior-finding closure audit

| ReviewFS item | Focused result |
| --- | --- |
| P1-1 full 128-bit Windows object identity | **Closed in design.** `FileIdInfo`, `[u8; 16]`, volume binding, no truncated `MetadataExt::ino`, pre-mutation unsupported fallback, synthetic encoding tests, and Windows behavioral evidence are explicit. The persistent volume-identity qualification is the separate P2-1 above. |
| P1-2 platform durability and retirement order | **Materially closed, subject to P2-2.** Destination-before-source ordering, both sides of cross-directory moves, managed-goal-before-source-cleanup, source-before-authority retirement, Windows no-`sync_all`, faults, and a stop condition are now frozen. The pinned barrier's physical family state still needs definition. |
| P2-1 pre-residue same rename domain | **Closed for one invocation.** Opened handle facts, Linux mount ID, nested/bind-mount handling, pre-residue rejection, and no copy/delete fallback are explicit. Persisting that ephemeral proof across restart is the separate P2-1 above. |
| P2-2 family/action key and path equivalence | **Open at P1.** The two keys and alias seam exist, but parent object identity is still part of the family path identity and recreates hidden residue after parent replacement. |
| P3-1 private protocol and scratch lifetime | **Closed for this gate.** Encoding/size/version behavior is bounded; scratch correctness independence and deferred lease-held GC are explicit. |
| P3-2 cleanup linearization/concurrency | **Closed in design.** Lease scope, unsupported arbitrary mutation, pre-linearization hook, full-identity recheck, handle-relative retirement, and substitution evidence are explicit. |

## Wire-model assessment

The revised architecture remains implementable without changing the v1 wire
model. Existing actions provide operation and exact expected/goal bytes; the
private authority records only reconstructible physical facts and identities.
None of the corrections above requires a new lifecycle phase or public field.
They require a stable private lookup key, a distinction between ephemeral
rename-domain proof and durable identity, and complete private residue states.

## Exit decision

Fold P1-1 and P2-1/P2-2 into sections 3.1-3.4 and repeat focused review before
production edits. Once the family key is independent of parent object identity,
the persisted identity/reboot contract is explicit, and duplicate/barrier
states are part of one contradiction-free closed classifier, this filesystem
design is ready for R1/R2 without a v1 wire change.
