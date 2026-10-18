# Discarded Validation Proof at a Component Boundary

**Finding date:** 2026-09-08  
**Affected repository:** `gwz-core`  
**Affected path:** V1 merge-record decoding and checked-store loading  
**Status:** Corrected

## Finding

The V1 record decoder proved that an untrusted durable record was structurally
and semantically valid, but discarded the type carrying that proof before
returning to its caller. The checked-store reader therefore validated the same
record again.

This was safe in the narrow sense: invalid records were still rejected. It was
an architectural failure because the boundary between decoding and storage
understated the guarantee already established by the decoder. The downstream
component had no typed way to distinguish an arbitrary deserialized record from
one that had already passed the complete V1 validator.

The defect was not apparent with a handful of tests or ordinary interactive use.
It became expensive when thorough restart, fault, ambiguity and successor
matrices repeatedly crossed the journal boundary. Those matrices are exactly the
kind of coverage an agent can generate at scale. A duplicated operation that is
negligible in one test became hours of aggregate test CPU time when multiplied
across hundreds of scenarios and repeated state transitions.

## Exact code path that caused the problem

`src/workspace_ops/merge/record_wire/decode.rs` decoded an input in this order:

1. Parse strict YAML.
2. Read and classify the record envelope.
3. Deserialize `MergeOperationRecordV1`.
4. Call `validate_v1_record`.
5. Construct the canonical projection and return the ordinary record.

Before the correction, the relevant decoder code was:

```rust
let record: MergeOperationRecordV1 = serde_yaml::from_value(raw.clone())?;
let validated =
    validate_v1_record(record.clone()).map_err(|error| RecordDecodeError::Validation {
        header: header.clone(),
        error: Box::new(error),
    })?;

Ok(DecodedV1Record {
    raw,
    header,
    record,
    canonical: CanonicalMergeRecord::from(validated),
    unknown_fields,
})
```

The validation proof was consumed while building `CanonicalMergeRecord`. The
returned `record` field was once again only a plain `MergeOperationRecordV1`.

`src/workspace_ops/merge/v1_lifecycle/checked.rs` then did this in
`StoredV1Record::from_open_bytes`:

```rust
let decoded = decode_production_v1(bytes)?;

Ok(Self {
    typed: validate_v1_record(decoded.record)?,
    raw: decoded.raw,
    unknown_fields: decoded.unknown_fields,
    // ...
})
```

That second call did not establish a new fact. It validated the same typed model
that `decode_production_v1` had just validated from the same bytes.

The effective call path was:

```text
StoredV1Record::from_open_bytes
    └── decode_production_v1
          └── decode_v1_body
                └── validate_v1_record      first validation
    └── validate_v1_record                  duplicate validation
```

## What the validator does

`validate_v1_record` is not a cheap schema check. It establishes the semantic
integrity of the complete durable merge state. It checks:

- envelope, schema and writer-version consistency;
- workspace, merge, operation, target, participant, path, branch and commit
  identities;
- selected-target ordering and agreement with the participant map;
- baseline manifest and lock contents, hashes and participant correspondence;
- pending action legality for the current operation mode and lifecycle state;
- participant states and legal forward, recovery, rollback and preservation
  combinations;
- accepted workspace lock and manifest contents and their digests;
- publication candidate, evidence, composition commit and handoff consistency;
- journal recovery context and compatibility of pending operations.

Some checks parse embedded lock, manifest and marker YAML and recalculate
cryptographic hashes. Repeating this validator therefore repeats substantial
parsing, allocation, state-machine checking and hashing.

## Why the repetition was amplified

A checked journal commit intentionally observes three different durable states:

1. Reopen the current record immediately before rewriting it, detecting source
   drift.
2. Reopen the synchronized temporary file, proving the serialized replacement.
3. Reopen the published file after rename and directory synchronization, proving
   that publication retained the staged bytes and meaning.

Those are three separate observations and must remain separate. Before the fix,
each observation validated its decoded model twice. One normal commit therefore
performed three full decodes and six full validations before counting any
additional service load or reload.

The broad recovery tests then repeated that commit path across phase prefixes,
injected failures, restarts, rejection checks and convergence runs. For example,
the root ambiguity matrix covers ten request/owner tests, each walking 22
preservation phases. The root successor matrix covered two owners across the same
22 phase targets plus every legal handoff form.

Function-level sampling of the unoptimized fake phase found:

- `CheckedV1Store::commit` in 42,100 inclusive samples;
- `validate_v1_record` in 29,487 inclusive samples;
- `decode_production_v1` in 26,473 inclusive samples.

The fake phase consumed 615.37 user CPU-seconds and took 85.72 seconds of wall
time while using all 12 test threads. Earlier, broader matrix runs took several
minutes. Across repeated development and release cycles, this kind of hidden
multiplication becomes hours of test time.

The important scaling relationship is:

```text
cost = scenarios × transitions × durable observations × duplicated proof cost
```

A handful of tests leaves every multiplier small. Thorough generated matrices
make the architectural duplication visible.

## Correction

`DecodedV1Record` now retains the `ValidatedV1Record` produced by
`decode_production_v1`:

```rust
pub(crate) struct DecodedV1Record {
    pub(crate) raw: Value,
    pub(crate) header: MergeRecordHeader,
    pub(crate) validated: ValidatedV1Record,
    pub(crate) canonical: CanonicalMergeRecord,
    pub(crate) unknown_fields: UnknownFieldManifest,
}
```

`StoredV1Record::from_open_bytes` consumes that proof directly:

```rust
Ok(Self {
    typed: decoded.validated,
    raw: decoded.raw,
    unknown_fields: decoded.unknown_fields,
    // ...
})
```

The strict YAML parse, version dispatch, typed decode, unknown-field extraction,
semantic validation and exact-byte checks remain. Each distinct durable
observation still gets decoded and validated. Only the second validation of the
same decoded observation was removed.

The representative 22-phase ambiguity test fell from 2.27 seconds to 1.59
seconds under the optimized test profile. The complete migrated fake phase fell
from 16.61–18.01 seconds to 14.02 seconds after this correction. Test-profile
optimization and independently scheduled matrix rows provided the larger
surrounding reductions; this change removed the confirmed duplicate production
work itself.

## Architectural failure mode

The generic failure mode is **discarded proof at a component boundary**:

1. A component performs an expensive check and establishes an invariant.
2. Its return type does not carry evidence of that invariant.
3. A downstream component cannot safely rely on the undocumented fact that the
   check already ran.
4. The downstream component either trusts an unsafe convention or repeats the
   expensive check.
5. Local testing hides the duplication; broad combinatorial, fault or property
   testing multiplies it until it dominates runtime.

The corresponding design rule is:

> When a boundary establishes an expensive invariant, return a type that carries
> that proof and require downstream consumers to accept the proved type.

The proof-carrying type also documents ownership. Here, the wire decoder owns
validation of untrusted durable bytes. The checked store owns location, source
digest and exact-observation checks. Preserving `ValidatedV1Record` across that
boundary makes those responsibilities explicit and prevents accidental
revalidation.

## Review questions for similar code

- Does a parser, verifier or authorization function return the same ordinary
  type it received or constructed after proving a stronger property?
- Is a validated, canonical, authenticated or normalized wrapper created and
  then discarded before the next component boundary?
- Do adjacent layers call the same validator, parser, hash or policy engine over
  the same input?
- Is repeated checking protecting distinct observations, or merely recomputing
  an invariant for one unchanged observation?
- What happens when the call path is multiplied by hundreds of generated fault,
  restart or state-machine cases?
- Can the compiler enforce the boundary by requiring a proof-carrying wrapper?

Defensive validation is useful at trust boundaries. Revalidating an unchanged
value because the preceding boundary discarded its proof is avoidable defensive
work and a sign that the interface expresses the wrong guarantee.
