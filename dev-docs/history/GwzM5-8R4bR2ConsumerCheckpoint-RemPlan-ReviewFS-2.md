# R4b R2 consumer-checkpoint remediation — second independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

The correction closes the owner-class/action-purpose/physical-plan binding
finding from the prior filesystem review. I found no recurrence of that
defect. Two P2 interface/gate defects remain: archive authority can be issued
without the required terminal/source-only arbitration, and the checked-entry
gate is lexical rather than fail-closed and misses a real successful-writer
bypass.

P0: 0. P1: 0. P2: 2. P3: 0.

## Exact tuple reviewed

- workspace root: `d0f61af45c0c97243793de5760e5c292571a2bd7`
- `gwz-core`: `04d2c1f35eef466445abfad9f8d5333de8441970`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean at dispatch. The
commit identities remained unchanged during source inspection and focused
verification. Another independent review report later appeared as an
untracked workspace-root document; it changed no reviewed source or commit
identity and was not an input to this review.

## Closure of the prior P2 finding

The prior owner-class/action-purpose/physical-plan substitution is closed.

- `CheckedManagedActionV1` seals a checked action and its exact managed request.
  The merge-start constructor fixes the two pre-record purposes; durable-merge
  and archive construction derive a record owner from an opaque record
  observation (`checked_artifact/coordinator/identity.rs:241-302`).
- Production managed preflight accepts that sealed value and derives the
  action digest and owner binding internally. The old loose scalar preflight
  is test-only (`checked_artifact/bootstrap/managed/owner.rs:42-68`).
- Reservation derivation defensively compares the plan's action digest, owner
  binding, and declared-purpose mask with the checked action
  (`checked_artifact/coordinator/schedule.rs:17-31`).
- The record observation keeps decoded IDs and hashed bytes inseparable; its
  fields are private, production construction starts from one canonical leaf,
  and record bytes are bounded (`workspace_ops/merge/record_wire/checked_owner.rs:5-81,121-125`).
- Focused substitution, owner-class, literal schedule, and ID-boundary tests
  pass. The corrected production interface no longer permits the composition
  described in the prior review.

## Findings

### [P2-1] Archive authority is minted without terminal/source-only arbitration

**Where:**

- `gwz-core/src/checked_artifact/coordinator/identity.rs:267-280`
- `gwz-core/src/checked_artifact/bootstrap/managed.rs:244-267`
- `gwz-core/src/workspace_ops/merge/record_wire/checked_owner.rs:55-81`
- `gwz-core/src/checked_artifact/interface_tests/coordinator_remediation.rs:23-27,154-164`

**Violated contract.** RemPlan section 6 requires the archive prerequisite to
be issued only after merge-store arbitration proves exactly one valid terminal
source, an absent destination, matching identity, and exact source bytes. The
opaque prerequisite is intended to carry that proof, not merely identify a
record.

`CheckedManagedActionV1::for_archive` currently accepts the same generic record
observation used for ordinary durable-merge work. It derives an owner, then
constructs `ValidatedArchiveSourceV1` itself from the owner binding and source
hash. That prerequisite constructor checks only that the hash is nonzero. The
record observation bounds and decodes one leaf and checks its filename, but it
does not prove a terminal lifecycle state, that the leaf is the open source,
or that the archived destination is absent.

The new positive test demonstrates the defect directly: `v0_bytes` encodes
`state: executing`, yet `CheckedManagedActionV1::for_archive(&record)` succeeds.
Consequently a non-terminal open record can authorize and schedule creation of
`.gwz/merge/done` before archive arbitration has established that archive is a
legal next action.

**Impact.** The sealed owner chain is still bypassable specifically at the
archive authority boundary. Parallel R2 archive work could admit a durable
parent action whose owner proves only “these bytes were read,” while recovery
interprets the same action as proof that terminal source/destination
arbitration already occurred. That ambiguity is exactly what the opaque
prerequisite was meant to prevent.

**Required correction.** Separate generic record ownership from archive
authority. A closed merge-store arbitration function should consume the
canonical open/archived location pair, validate one exact terminal source and
an absent destination, and issue an opaque archive-authority value bound to
those exact source bytes. `for_archive` must consume that value; it must not be
able to manufacture the prerequisite from a generic record observation.

Add positive tests for one terminal open source plus absent destination, and
negative tests for every open state, both absent, destination-only, and both
present/equal or contradictory. Each rejection must occur before provider
execution, admission, or filesystem mutation.

### [P2-2] The claimed fail-closed entry gate misses a real raw successful bypass

**Where:**

- `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:41-50,77-118`
- `gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:36-91`
- `gwz-core/src/checked_artifact/entry.rs:1-67`
- `gwz-core/.github/workflows/release.yml:72-76`

**Violated contract.** RemPlan sections 7 and 9 require an exhaustive
checked/ordinary boundary, exact selected-merge reachability, no raw successful
bypass, and failure on every new unclassified call. The amendment specifically
claims a negative fixture for a raw successful bypass and a mandatory CI gate.

The checker is a regular-expression token inventory. Entry calls are detected
only when spelled literally as `checked_artifact::entry::<name>`, and raw
filesystem writers are detected only under a short list of literal spellings.
The negative fixtures append comments containing those spellings; they do not
insert compiling calls. Thus the tests prove that comments are matched, not
that executable aliases or re-exports are classified.

I copied the reviewed `src` tree, inserted this executable writer into the
already allowed `workspace_ops/merge/root/artifact_facts.rs` adapter, and ran
the committed checker:

```rust
use std::fs::write as unchecked_write;
unchecked_write(root.join(relative), bytes).unwrap();
```

The checker exited zero and printed:

```text
checked-artifact boundary: ok (5 entries, 5 classified callers)
```

The same lexical limitation permits aliased/re-exported entry calls. The
checker also contains no owner-constructor or call-graph assertion: its
“required reachability” fixture merely replaces one literal entry function
name with another. It cannot prove that preservation, publication, rollback,
or recovery reaches an exact record owner as required by RemPlan section 7.

The two scripts are wired into the Linux release job, which is useful and did
run successfully here. CI execution does not compensate for a gate that
accepts the prohibited program shape.

**Impact.** A future consumer can add a second successful writer inside a
checked adapter, or route an ordinary helper through an alias, while the
mandatory gate remains green. This defeats the checkpoint's main protection
against independently developed R2 lanes reintroducing raw filesystem paths.

**Required correction.** Make the boundary structural. Prefer Rust visibility
and typed entry capabilities that make direct writers and wrong-owner calls
fail to compile. Supplement that with compile-pass/compile-fail fixtures that
contain real calls, not comments. If a source inventory remains, parse Rust
syntax and resolve imports/aliases/re-exports, enumerate the complete
production caller graph, and explicitly assert each selected merge path's
owner constructor. Retain the release-CI invocation after the gate itself is
made fail-closed.

## Verification

On the reviewed core commit:

- `cargo test -p gwz-core checked_artifact::interface_tests -- --test-threads=1`:
  93 passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed;
- `python3 scripts/checks/test_check_checked_artifact_boundaries.py`: six
  tests passed; and
- an independent copied-tree alias-bypass probe, described in P2-2, was
  incorrectly accepted by the boundary checker.

The lead-supplied settled-tree evidence also records 1,223 unit tests passed
(one ignored), 45 integration tests passed, 93 interface tests passed, 32
preservation tests passed, and green Clippy, formatting, protocol, document,
compatibility, and committed boundary checks.

## Gate decision

Do not begin R2 production conversion on this tuple. Correct archive authority
issuance and replace the lexical boundary assertion with a structural,
real-call fail-closed gate. Commit one new settled workspace/core/CLI tuple and
repeat both independent re-reviews. No public GWZ protocol change or new
durability phase is required.
