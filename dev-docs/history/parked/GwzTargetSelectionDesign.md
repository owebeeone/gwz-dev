# GWZ target selection — design

Status: **DRAFT 2026-09-06 revision 1.** Design only; no phasing, no
implementation, no acceptance claimed. Written by the lane owner after an
operator report that `@all` and `@root` "don't work universally", and a
read of every selection call site at gwz-core `bea84c1`.

## 1. The problem, as measured

Selection is not one mechanism. It is **five**, and which one a verb happens
to use decides whether `@root` works, is refused, or is silently swallowed.

| Path | Verbs | Default | `@root` |
|---|---|---|---|
| Modern resolver, `All`/`Allow` | commit, stage, push, diff, status, log | root + members | works |
| Modern resolver, `Members`/`Allow` | ls, merge, family merge | members only | works when named |
| Legacy members-only (`resolve_manifest_selection` → `resolve_member_ids`) | branch, tag, materialize, stash | members only | **refused**, generically |
| No consultation at all | local clone, list, dispose, disband | — | **ignored** |
| Delegated through another verb | forall (builds an `LsRequest` and uses its member list) | inherits `ls` | **cannot survive the round trip** |

Three further facts:

- **`@all` does include the root.** `all_targets` is the root followed by
  every active member. Any prose or example implying otherwise — including
  the `--target @all --target @root` spelling in the local-clone plan's MVP
  workflow — is redundant at best and teaches the wrong model.
- **The refusal says nothing useful.** An explicitly selected root on a
  legacy-path verb is `invalid_request`: "selected command does not support
  `@root`". It names no verb, no reason, and no alternative. The tag handler
  carries a comment admitting the truth: root behaviour "is not specified in
  the target-selection rollout". That is undecided, not designed.
- **The wire has four overlapping ways to say the same thing.**
  `Selection` carries `all: BOOL`, `member_ids: [STR]`, `paths: [STR]` and
  `targets: [STR]`, plus `exclude`. The token form subsumes the other three.
  Each generation of the feature added a field instead of replacing one, and
  the extra resolvers exist to serve them.

There is also a dead wrapper: `resolve_member_targets` has no production
callers.

## 2. What went wrong, structurally

Selection policy is expressed as **arguments passed at nine call sites**
rather than as a property of a verb. Nothing forces a new verb to declare a
policy, so the two ways to get it wrong are both silent: inherit the legacy
path and reject the root by accident, or consult nothing and swallow the
flag. Both happened. The local-clone verbs are the most recent example, and
they were written this week by people reading the same documents.

A per-call-site convention cannot survive parallel development. This is the
same lesson the program already learned about house conventions and gates.

## 3. Principles

1. **One vocabulary.** `@default`, `@all`, `@root`, a member id, a member
   path. Unknown `@token` is an error. This is already true and stays.
2. **One resolver.** Every verb resolves through the same function. There is
   no second path and no verb-local re-derivation.
3. **Policy is declared, not passed.** A verb's selection behaviour is a row
   in one table, next to every other verb's, where an omission is visible and
   a reviewer can read the whole surface at once.
4. **Silence is not a policy.** A verb either honours selection or refuses it
   with a reason. Accepting a flag and ignoring it is the one outcome the
   design forbids.
5. **Refusals name the verb and the alternative.** A generic message is a
   defect, because the operator's next action is what a refusal is for.

## 4. The model

### 4.1 One declared policy per verb

```text
SelectionPolicy {
    default: All | Members,
    root:    Allow | Undefined(reason),
}
```

plus a third whole-verb state for commands that operate on the workspace as
an indivisible thing:

```text
TakesNoSelection(reason)
```

Every verb has exactly one of these, declared in a single table keyed by
`ActionKind`. The resolver takes the policy; it is never passed ad hoc
constants at a call site. A verb added without a row does not compile.

`Undefined(reason)` and `TakesNoSelection(reason)` both carry the sentence
the operator sees, so the message lives with the decision rather than in the
resolver.

### 4.2 Resolution

```text
resolve(manifest, selection, policy) -> Vec<SelectedTarget> | Refusal
```

Order: normalise the wire form into tokens; expand includes (`@default`
expands per the policy's default); expand excludes; subtract; then apply the
root policy to what survived. Deduplication and ordering are the resolver's,
so every verb reports targets in the same order.

**The root is expanded, not special-cased.** `@all` yields the root plus
members because that is what the workspace is. A verb that cannot act on the
root refuses an explicit `@root`, but `@all` on such a verb means its members
— because the operator asked for everything the verb can do, not for a
refusal. This is the one asymmetry in the design and it is deliberate:
naming the root is a request, `@all` is a scope.

### 4.3 The refusal grammar

Three kinds, one shape: what was asked, why this verb cannot, what to do.

- **Root undefined for this verb** — names the verb and the reason from the
  policy row.
- **Verb takes no selection** — names the verb and states that it operates
  on the whole workspace.
- **Unknown token / unresolvable member** — unchanged; already specific.

Each is a distinct error code so a driver can render them differently and a
script can branch on them.

### 4.4 The wire

`targets` and `exclude` are the general form and the only form core resolves.
`all`, `member_ids` and `paths` are normalised into tokens at the protocol
edge and documented as legacy spellings. No behaviour changes for an existing
caller; there is simply one code path behind them. Removing them is a
separate compatibility decision and not part of this design.

## 5. The verb table

Recommended rows. Two need an operator ruling and are marked.

| Verb | Default | Root | Note |
|---|---|---|---|
| status, diff, log, commit, stage, push | All | Allow | unchanged |
| ls | Members | Allow | must be able to *report* the root when named — see §6 |
| branch, tag, stash | Members | **Allow** | the root is a real repository with branches, tags and stashable changes; currently refused only because it was never specified |
| materialize | Members | **Undefined** | materialising the root is meaningless: it is already present, and members are what a lock records |
| merge | Members → **operator ruling** | Allow | see §6 |
| pull | Members | Allow | unchanged |
| forall | Members | Allow | must resolve directly, not through `ls` |
| local clone, list, dispose, disband | — | — | `TakesNoSelection`: a lane is a whole workspace |

## 6. Two decisions that are not mine

**The merge default.** `merge` defaults to members only. That is why a lane's
root commits are never integrated by `gwz merge --remote A`, and therefore
why disposing that lane afterwards refuses on unpreserved history: the lock
commit every `gwz commit` writes to the root has no route home. Changing the
default to `All` fixes the workflow and changes what a bare `gwz merge` does
in every existing workspace. Leaving it makes the family workflow require a
selector the user has no reason to guess. This is a behaviour change either
way and belongs to the operator.

**`ls` and the root.** `ls` allows `@root` but answers with a member list,
which cannot represent the root — which is also why `forall @root` runs in
nothing. Either the response gains a root entry, or `ls` declares the root
`Undefined`. The first is honest and unblocks `forall`; the second is smaller
and makes the current behaviour explicit.

## 7. Invariants a gate can hold

- Every `ActionKind` has exactly one policy row.
- No production code outside the resolver module expands a selector token,
  reads `Selection` fields directly, or derives targets from another verb's
  response.
- Exactly one resolver entry point exists; no members-only variant.
- Every `Undefined` and `TakesNoSelection` row carries a non-empty reason.
- Each refusal kind has a distinct error code and at least one test that
  fires it through a driver.

The first two are what would have caught all five paths.

## 8. What this design does not cover

Removing the legacy wire fields; declared `@set` selectors, which the wire
comment already anticipates; whether `@all` should ever mean the root alone
for a verb with no members; and the local-clone family's own naming, which is
a separate namespace resolved at operation time and deliberately not a target
selector.
