# R4b R2 consumer-checkpoint remediation — third independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 2. P3: 0.

The third correction closes the previously reported archive location-shape
substitution, general checked-capability escape, and missing invocation-gate
defects. It does not yet prove the two claims on which R2 conversion depends:
archive authority can still be issued from a contradictory terminal envelope,
and the mandatory source gate still accepts a compiling raw-writer alias in a
checked adapter.

## Exact tuple reviewed

- workspace root: `c5a01316d79fcd9a80acb6727c61a4028544b373`
- `gwz-core`: `8ba1d52c5a6dbf99f76e8411e1db721397929ac9`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean at dispatch. The
commit identities did not change during the review. Another independent review
report appeared later as an untracked workspace-root document; it changed no
reviewed source and was not an input to this independent review.

## Closure of the prior findings

### Prior archive source/destination arbitration: closed except for terminal semantic validity

The physical and identity parts of the prior archive finding are closed:

- canonical acquisition reads the open and archived leaves from retained real
  parent identities, then rereads both leaves and rejects parent, identity,
  content, appearance, or disappearance changes
  (`workspace_ops/merge/record_wire/location.rs:102-162`);
- the archive issuer requires an exact `Open` leaf, an absent archive leaf,
  filename/decoded merge-ID agreement, and exact source bytes
  (`workspace_ops/merge/record_wire/checked_owner.rs:112-138`);
- `CheckedArchiveSourceObservation` has private fields and can be created only
  by record-wire arbitration; and
- `CheckedManagedActionV1::for_archive` consumes that archive-specific opaque
  type rather than promoting a generic owner observation.

Focused tests reject nonterminal states, no source, destination-only,
both-present/equal, both-present/different, wrong-kind, source replacement,
archive appearance, and real-parent replacement. I found no route that
substitutes an archived leaf for the source or separates the observed bytes
from the derived owner.

The remaining semantic-validity defect is P2-1 below.

### Prior checked-capability escape and missing gate invocation: closed

The general `CheckedArtifact`, `CheckedArtifactFact`,
`CheckedArtifactTransition`, and policy types are private to
`checked_artifact` (`checked_artifact/mod.rs:64-112`). Purpose-specific entry
operations return facts, transitions, booleans, or completed results; no
general filesystem capability crosses the entry boundary
(`checked_artifact/entry.rs:13-149`). Rust visibility therefore prevents the
previous direct capability escape.

The committed boundary gate runs on pull requests and pushes to `main`
(`.github/workflows/checked-artifact-boundary.yml:6-25`), in Linux release CI
(`.github/workflows/release.yml:62-78`), and in the local release script before
the full Rust tests (`scripts/release.py:339-366`). The publish-workflow tests
pin those invocations. This closes the missing PR/push and pre-publication
execution defect.

The remaining fail-closed/source-analysis defect is P2-2 below.

## Findings

### [P2-1] Archive authority accepts a terminal label without validating the terminal envelope

**Where:**

- `gwz-core/src/workspace_ops/merge/record_wire/checked_owner.rs:74-90,112-138`
- `gwz-core/src/workspace_ops/merge/record_wire/decode.rs:77-107`
- `gwz-core/src/workspace_ops/merge/record_wire/archive/mod.rs:45-107`
- `gwz-core/src/checked_artifact/interface_tests/coordinator_remediation.rs:25-33,179-187`

**Violated contract.** RemPlan sections 1 and 6 require archive authority to
come from one *valid terminal* open source. The source issuer calls the general
production-v0 decoder and then checks only `!record.state.is_open()`. That
decoder proves strict YAML/header/body decoding, but it does not validate the
cross-field terminal record invariants. The production archive decoder does:
it projects through the v0 archive validator and rejects contradictory state,
participant, publication, candidate, and terminal evidence.

The committed positive archive-authority fixture is itself too weak. It sets
`state: completed` while leaving `selected_targets` and `participants` empty
and omitting a valid completed publication/evidence shape. The archive-source
issuer accepts those bytes as terminal authority.

I reproduced the disagreement in an exported copy of the exact core commit. A
temporary unit probe passed the same minimal `completed` bytes to both paths:

```text
decode_archived_v0: ArchivedRecordUnreadable: archive envelope or terminal
state is contradictory
observe_checked_archive_source_v0: Ok(CheckedArchiveSourceObservation)
```

The probe passed, demonstrating that the authority issuer accepted the bytes
that the production archive validator rejected. The probe was isolated under
`/tmp`; no production or committed source was changed.

**Impact.** A corrupt or partially persisted record can mint the opaque proof
that schedules creation of `.gwz/merge/done` merely by carrying a terminal
state discriminant. Downstream code can no longer distinguish a fully
validated terminal source from a syntactically decodable contradiction, even
though the opaque type is documented as that proof. This violates the
zero-mutation rejection boundary before R2 archive conversion.

**Required correction.** Issue v0 archive authority only after the exact bytes
pass the same complete terminal-envelope validation used for archive
projection, either by reusing `decode_archived_v0` or by extracting one common
validator consumed by both paths. Do not reduce terminal validity to
`OperationState::is_open`. Replace the positive fixture with a genuinely valid
completed or aborted record and add a negative fixture for a terminal-labelled
but structurally contradictory record. The rejection must precede action
construction, admission, provider execution, and filesystem mutation.

### [P2-2] A local function-pointer alias defeats the mandatory raw-writer gate

**Where:**

- `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:293-307,400-408,514-545`
- `gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:111-154`
- `gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:37-49,89-96`

**Violated contract.** RemPlan sections 1, 7, and 9 require the boundary gate
to reject executable aliases, direct and transitive raw writers, and every raw
successful bypass in selected merge adapters. The checker inventories imports,
visible items, and a *set of call spellings*. It recognizes a call only when a
token path is immediately followed by `(`. It neither resolves local bindings
nor records the symbol assigned to a function pointer.

In an exported copy of the exact core commit I inserted only these two lines
at the start of the already classified `write_checked` adapter:

```rust
let map_transition = std::fs::write;
let _ = map_transition(relative, bytes);
```

This is compiling executable Rust and an ordinary raw writer. It introduces no
import or visible item. `std::fs::write` is not followed by `(`, while the
invoked name `map_transition` is already in that adapter's accepted call set.
The committed mandatory checker incorrectly returned:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

`cargo check --lib` also completed successfully on the probe tree. The
committed alias fixture does not cover this form: it adds a new import and a
new call name, either of which the lexical inventory can detect.

**Impact.** A selected preservation, publication, rollback, or recovery
adapter can acquire a raw writer through a local binding and execute it while
every PR, push, release-CI, and local-release gate remains green. Mandatory
invocation has therefore been fixed, but the mandatory assertion is not
fail-closed and does not establish exit criterion 8.

**Required correction.** Replace call-name-set matching with a structural
mechanism that resolves executable names and complete transitive calls. The
strongest solution is a Rust visibility/type boundary or compiler-supported
lint that makes raw mutation from these adapters impossible. If a source gate
remains, it must parse Rust and resolve imports, local bindings, aliases,
re-exports, associated calls, and helper delegation to a fixed point rather
than one hand-listed helper level. Add a compiling regression fixture using
the exact same-name function-pointer form above, retain fixtures for direct and
non-`std` writers, and prove that every selected adapter's complete successful
writer graph terminates only at a purpose-specific checked entry operation.

## Verification

On the exact reviewed core commit:

- `cargo test -p gwz-core checked_artifact::interface_tests::coordinator_remediation -- --test-threads=1`:
  7 passed;
- `cargo test -p gwz-core workspace_ops::merge::record_wire::archive::tests -- --test-threads=1`:
  24 passed;
- `cargo test -p gwz-core --test publish_workflow -- --test-threads=1`:
  6 passed;
- `python3 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v`:
  13 passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed with
  15 visible entries and 5 classified modules;
- isolated copied-tree archive-validity probe: reproduced P2-1 while compiling
  and passing as a test; and
- isolated copied-tree local-alias probe: the committed checker passed and
  `cargo check --lib` passed, reproducing P2-2.

The green committed tests establish the intended location arbitration,
capability privacy, and gate wiring, but they do not contradict either
finding: one positive fixture blesses an invalid terminal envelope, and the
alias fixtures do not exercise a same-name local function-pointer binding.

## Gate decision

Do not begin R2 production conversion on this tuple. Reuse the complete v0
terminal validator at archive-authority issuance and replace the lexical
call-name inventory with a fail-closed writer boundary. Commit one new settled
workspace/core/CLI tuple and repeat both independent state and filesystem
re-reviews. No public GWZ protocol change or new durability phase is required.
