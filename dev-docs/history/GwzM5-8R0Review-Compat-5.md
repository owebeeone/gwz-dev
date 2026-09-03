# GWZ M5–M8 R0 Cross-platform Fixture-contract Review

Date: 2026-08-02

Scope:

- exact-SHA retained-reader CI run `30749238987` for
  `7d1d86512bdda3b6473d9d7d35cb2081097a8bbf`;
- the proposed optional `.git/info/exclude` continuation mutation;
- the proposed semantic retained-reader fixture identity;
- false-positive, false-negative, and retained-old-reader compatibility risk;
  and
- the concurrent uncommitted draft in `gwz-core/scripts/retained_readers`.

## Verdict

The diagnosis is correct, and a semantic fixture identity is the right repair,
but the current draft is **not ready to commit or rebaseline**. It has two P2
contract holes and one P3 portability hole.

There is no evidence here of a new released-reader compatibility defect. On
both Linux architectures the only failing continuation mutation was
`.git/info/exclude`, while the durable semantic postconditions passed. Windows
fixture generation now succeeds, but the pre-execution identity gate rejects
all generated fixtures because the frozen identity includes host-specific Git
administration files. Thus the exact-SHA CI result remains incomplete platform
evidence, not evidence that the old readers produced an incompatible merge.

Implementation may continue after adopting the interface requirements below.
Commit, contract regeneration, evidence regeneration, push, and R0 acceptance
remain blocked until the P2 findings are closed and independently re-reviewed.

## Findings by severity

- **P0:** none.
- **P1:** none.
- **P2:** two findings.
- **P3:** one finding.

### [P2-1] Optional exclude-path classification does not constrain the resulting boundary

**Where:** `gwz-core/scripts/retained_readers/cases.json:39-43` and
`:125-129`; `gwz-core/scripts/retained_readers/retained_reader_fixture.py:285-297`.

The draft adds a `0..1` dynamic mutation for `text:.git/info/exclude`. The
mutation evaluator checks only the path and count. It does not check the final
file content. `normalized_mutations` records a content digest for evidence, but
that digest is not an acceptance predicate on Linux or Windows and therefore
does not close the hole.

I reproduced the issue directly: replacing the exclude file with `!/*` was
accepted with no evaluation error by a contract containing the proposed
optional path. Deleting the file or replacing it with another arbitrary rule
would be classified the same way. This creates a false negative: a retained
reader could expose member repositories, hide unrelated files, or destroy the
workspace boundary and still pass.

**Required correction:** optionality must apply only to whether the file's
physical bytes changed. Both continuation cases must unconditionally verify
the final boundary content. Prefer an EOL-normalized, ordered-line digest with
an explicit small allowlist of reviewed complete contents if the retained
readers genuinely produce more than one representation. Do not use a broad
ignore-rule equivalence heuristic: ignore order, negation, escaping, and
directory anchoring are behavior-bearing. A byte change is allowed only when
the resulting complete ordered rule set is one of the reviewed boundary
representations.

The case-schema interface should therefore express both:

1. optional `0..1` mutation occurrence for `.git/info/exclude`; and
2. an always-run final-content postcondition for `.git/info/exclude`.

Add a negative test that substitutes a syntactically valid but wrong exclude
file and proves the case fails, plus positive tests for the unchanged canonical
file and every explicitly allowed retained-reader representation.

### [P2-2] The draft identity is still physical-storage identity with a broad `.git` ignore gap

**Where:** `gwz-core/scripts/retained_readers/retained_reader_fixture.py:109-148`;
`gwz-core/scripts/retained_readers/test_retained_reader_adversarial.py:200-249`.

The draft filters known noisy `.git` entries, but it still uses the raw path of
each loose object, loose ref, and `packed-refs` file as the identity key. It also
identifies a loose object by its filename OID without verifying its contents,
and silently drops every unclassified Git administration entry.

Four independent probes demonstrate both directions of error:

```text
corrupt loose object bytes: identity_equal=True
unclassified MERGE_HEAD: identity_equal=True
loose-to-packed object storage: identity_equal=False
loose-to-packed refs: identity_equal=False
```

The first two are false negatives: corrupt object data and an active Git merge
pseudoref can be introduced without changing fixture identity. The last two are
false positives: storage-only repacking changes identity even though the
repository's logical refs and objects are unchanged. The current adversarial
test that merely adds a valid loose object does not exercise corruption,
packing, or ignored control state.

**Required correction:** compute one repository-level semantic record for each
generated repository rather than retaining physical `.git` entries as identity
keys. At minimum, that record must contain:

- Git object format and repository format;
- HEAD state: symbolic target versus detached/unborn, plus its resolved OID
  where applicable;
- the complete ref-name/OID mapping obtained through Git plumbing, independent
  of loose versus packed storage;
- the complete staged index tuple stream `(path bytes, mode, stage, OID)`;
- every readable object OID and type, including unreachable objects needed by
  pending recovery records and independent of loose versus packed storage;
- canonical config and `.git/info/exclude` content; and
- the durable non-`.git` tree, including file/symlink type, contents or target,
  and directory presence, with only deliberately normalized platform metadata.

Object enumeration must include unreachable prepared trees and commits;
`rev-list --all` alone is insufficient. Use storage-independent Git plumbing
such as `cat-file --batch-all-objects` and validate object readability/content
with an appropriate full-object integrity check. An OID inferred only from a
loose filename is not sufficient.

Use a closed classification for the remaining Git administration tree. Known
non-authoritative files such as reflogs, `COMMIT_EDITMSG`, `description`, hooks,
and proven maintenance noise may be explicitly ignored. Unknown entries, and
known behavior-bearing controls such as `MERGE_HEAD`, `CHERRY_PICK_HEAD`,
`REVERT_HEAD`, shallow/alternate/graft state, replacement refs, linked-worktree
indirection, or additional configuration, must be represented semantically or
make fixture identity fail closed. A broad `else: continue` is not acceptable.

### [P3-1] Index identity passes a NUL-delimited byte protocol through text mode

**Where:** `gwz-core/scripts/retained_readers/retained_reader_fixture.py:128-132`
and the text-mode `_run_git` used by that path.

`git ls-files --stage -z` is specifically a byte-safe path protocol, but the
draft decodes it through `text=True` and then re-encodes `stdout`. That makes
identity dependent on the host decoder and cannot represent a non-UTF-8 Git
path reliably. The current generated fixtures use ASCII paths, so this is not a
failure in the present case set, but it weakens the claimed general semantic
identity and can become another platform-specific false positive or exception.

**Required correction:** retain Git's NUL-delimited index output as bytes and
hash or parse those bytes without a text round trip. Add an adversarial test
with a non-UTF-8 path on a platform that supports it, and retain a portable
byte-level unit test on all platforms.

## Required identity invariants and adversarial coverage

The identity interface should be fixed before regeneration. Its tests should
prove the following invariants separately rather than only checking fresh
generator equality.

### Differences that must change identity or fail closed

- symbolic branch name, detached/unborn state, or resolved HEAD OID;
- add, remove, rename, or retarget any ref;
- index path bytes, mode, stage, or OID;
- add, remove, corrupt, or replace any object, including an unreachable pending
  tree or commit;
- canonical config or `.git/info/exclude` content;
- any durable non-`.git` file, symlink, record, lock, or relevant directory;
- any unclassified behavior-affecting Git control file; and
- unsupported repository/object formats or Git indirection.

### Differences that must not change identity

- reflog timestamps/content;
- `COMMIT_EDITMSG` and repository `description`;
- hooks, documented maintenance noise, and internal empty-directory shape;
- loose versus packed representation of the same object set;
- loose versus packed representation of the same ref mapping;
- equivalent line endings only where the contract explicitly declares them
  non-semantic; and
- mtimes and deliberately ignored host permission noise.

The repack and `pack-refs` invariance tests are especially important because
they prove the abstraction is actually semantic rather than a filtered raw
tree. The corrupt-object and unknown-control tests prove it fails closed rather
than silently widening acceptance.

## Old-reader compatibility assessment

Changing fixture identity does not change fixture bytes, the v0 merge-record
schema, reader inputs, or released binaries. It is a harness authenticity
contract, so regenerating its digests after a reviewed identity-version change
does not itself break any old reader.

The `.git/info/exclude` allowance is also compatible when it records a proven
old-reader side effect and pins the final boundary semantics. It becomes an
invalid compatibility waiver if it merely accepts arbitrary content.

Excluding reflogs, `COMMIT_EDITMSG`, and `description` from fixture identity is
appropriate because the retained-reader operations under test do not consume
them. Reflog reachability can affect a later Git pruning operation, but this
matrix does not run `git gc`/prune; GWZ merge-record garbage collection is a
different operation. Object inventory must nevertheless include unreachable
objects because pending GWZ recovery records reference prepared objects that
need not be reachable from refs.

## Regeneration and evidence gate

After the identity implementation and adversarial tests pass:

1. regenerate `fixture-contract.json` and every `cases.json` fixture digest;
2. regenerate checked macOS arm64 evidence with the new evaluator source
   binding;
3. prove a fresh macOS arm64 run is portable-equal and byte-identical to the
   checked evidence;
4. commit only the coherent contract, implementation, tests, cases, and checked
   evidence set; and
5. rerun all required exact-SHA Windows, Linux x86_64, Linux arm64, macOS
   x86_64, and macOS arm64 lanes.

The Windows lane must reach and execute all applicable retained readers; an
identity-only pass is not enough. Both Linux continuation rows must pass the
new final exclude-content postcondition. All platform evidence and
attestations must bind the same committed evaluator and case sources before R0
can be accepted or R1 dispatched.

## Approval decision

- **Proceed with the semantic-identity implementation using the corrected
  interface above:** approved.
- **Treat the current uncommitted draft as complete:** not approved.
- **Regenerate contracts/evidence before the P2 findings are closed:** not
  approved.
- **Commit, push, or accept R0 from the current draft:** not approved.
- **Request independent re-review after remediation and before commit:**
  required.
