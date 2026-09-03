# R4b remediation R1 filesystem/interface settled-tree re-review

Date: 2026-08-14

Reviewed immutable tuple:

- workspace root `07d3bc5ed9ce746fbedbe25ff48b217968ec6bf3`;
- `gwz-core` `1bc07c4b35d6d158aa8daf25f924acf438f20698`; and
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f`.

Controlling documents:

- `dev-docs/GwzM5-8R4bR1Interface-ReviewFS.md`;
- `dev-docs/GwzM5-8R4bR1Interface-RemPlan.md`; and
- `dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md`.

## Verdict

**NO-GO for R2.** There is no P0 finding. Three P1 findings remain; there is
no separate P2 finding.

## Findings

### [P1-1] Pre-catalog authority issuance and revalidation are not owner-private or structurally ordered

`PreCatalogPreflightV1` is visible throughout `checked_artifact`, and its
default `preflight` method constructs the supposedly sealed permit. Any sibling
can implement the trait and issue permits from its own observations.

More importantly, `CatalogBootstrapV1::recover_or_create` receives only the
permit. It cannot structurally invoke the provider's `revalidate` immediately
before mutation. The test performs revalidation separately before calling
catalog bootstrap, leaving the required ordering as caller convention and
allowing a stale-permit race.

Evidence:

- `gwz-core/src/checked_artifact/capability/pre_catalog.rs:157-217`;
- `gwz-core/src/checked_artifact/bootstrap.rs:44-52`; and
- `gwz-core/src/checked_artifact/interface_tests/capability_permit.rs:119-128`.

Required correction: place the raw provider behind an owner-private boundary
and make catalog bootstrap consume an owner-bound transaction that performs
revalidation at the mutation boundary.

### [P1-2] The frozen namespace wrapper cannot execute its indexed barrier and managed-bootstrap slots

`ActionNamespace` derives `BarrierSlots`, `BootstrapGenerationSlots`, and
`BootstrapComponentSlots`, but its only mutation forwarding methods are:

- base-slot publication;
- cleanup retirement; and
- namespace barrier.

There is no wrapper operation that publishes or retires:

- barrier active/retired intent slots;
- retired roaming-anchor aliases;
- bootstrap active/retired generation slots; or
- retired bootstrap-marker slots.

The raw backend is correctly private, so R2 cannot implement these accepted
restart protocols without widening or changing the supposedly frozen
interface.

Evidence:

- `gwz-core/src/checked_artifact/namespace/mod.rs:220-269`;
- `gwz-core/src/checked_artifact/namespace/roles.rs:99-240`; and
- `gwz-core/src/checked_artifact/namespace/backend.rs:384-425`.

Required correction: add role-specific forwarding operations for every indexed
transition, each revalidating the admitted directory and reservation before
invoking the private backend.

### [P1-3] Managed-bootstrap successor evidence does not enforce the promised parent identity/mode revalidation

Installed-component acceptance checks only that the observed path has the
previous path as a prefix and ends with the expected component name. It does
not verify that the appended component's parent identity and mode equal the
intent's retained parent identity and mode.

Marker-retirement acceptance is weaker: `retired_marker_identity` and
`retired_parent_mode` are never checked, and the path need only end in the
expected final name. A substituted same-profile directory or changed case mode
can therefore produce evidence that advances the durable successor chain.

Evidence:

- `gwz-core/src/checked_artifact/namespace/backend.rs:329-372`; and
- `gwz-core/src/checked_artifact/protocol/managed_bootstrap_record/transition.rs:106-147`.

This contradicts the accepted rule that substitution or mode change is
ambiguity and that marker retirement re-proves every final identity and mode.

Required correction: bind installation evidence to the exact prior retained
parent identity/mode/path, and bind retirement evidence to the exact installed
directory and retained marker identity before issuing a successor.

### [P3-1] Independent semantic canonical vectors are still absent

The committed checked-artifact corpus remains generator-produced shape data.
For example, its durable-identity value selects one variant while populating
unrelated optional fields, so it is not a valid semantic vector. Rust tests
construct valid values and round-trip them, but there is no independently
authored exact-byte fixture covering every closed variant and both maximum
schedules.

The prior P3 recommendation therefore remains open.

## Closed prior findings

The remediation did materially fix:

- exact managed-plan/reservation binding;
- raw namespace backend bypass;
- aggregate root-bound permit data;
- the missing Taut durable record family;
- persisted component identity/mode/domain fields;
- profile-specific durable identities; and
- the independently exact-pinned 161-key fault vocabulary.

## Verification

- 61 checked-artifact interface tests passed.
- 29 public protocol tests passed.
- Protocol regeneration check passed.
- Public protocol/generated files and dispatcher remain unchanged.
- The reviewed tuple was clean before this review artifact was written.

