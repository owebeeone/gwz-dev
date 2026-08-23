# R2-D settled gate — CODE-axis review (round 1)

Date: 2026-08-23. Axis: **Code** (adversarial, round 1 of a two-round cap;
program-level dual gate, peer-blind — the State report was not read and its
existence in the directory listing was deliberately not opened).
Object: **the settled tuple tree, gwz-core `d45458d`** ("Land R2-D Step 5.1:
the settled tuple's gate train, evidence, and last test debt") — verified
`git rev-parse HEAD` = `d45458d820093c841257c62ea8ee92e752e835f3`, working
tree clean. Contract set read end to end: `GwzM5-8R2DSettledTuple.md` (the
claims under verification), `GwzM5-8R2DInterfaceFreeze.md` §3.5 with its full
annotation stack and §4.3/§4.4 as cited, `GwzM5-8R2D-Plan.md`,
`CurrentProgramCheckpoint.md` RESUMED section including the PHASE 4 SETTLED
escalation ledger and the Step 5.1 landing record.

Mandate honored as issued: the five second-axis scrutiny items (2.4's
cfg-compile + provenance territory; the caller-supplied-restatement class
audit; the anchor package + 4.2 retirement; the 3.1/3.1b ownership-token
boundary; the `runtime.*` characterization) are §3–§7 below. The R2-E
re-reservations (38 keys) were treated as sanctioned records; their
non-activation code claims were spot-verified, not re-litigated.

All commands ran on this host at `d45458d` with
`CARGO_TARGET_DIR=<scratchpad>/dual-code-target` (clippy in a fresh
`<scratchpad>/dual-clippy`, making it effectively from-clean). Line numbers
cite the settled tree.

---

## 1. Verdict

**GO.** No P0, no P1, no P2. Six P3 findings (§9), all record-, comment-, or
R2-E-obligation-class; none blocks the settled gate or the A1 path. Eleven of
the twelve tuple gates were independently re-executed on this host and every
tail reproduced exactly (§2); the twelfth (clippy) reproduced from a fresh
target directory. The five mandated second-axis items close clean: the
platform-cfg compile class is extinct on every target the program supports
(§3); the caller-supplied-restatement class audit finds **no unprotected
instance on any production-reachable path**, with one caller-asserted seam
named for R2-E (§4); the anchor protocol's ordinal retirement is convergent
with no fixed point and the ExactInterior no-op is sound for its single
production caller (§5); the ownership-token boundary is uncrossed (§6); and
the `runtime.*` characterization for the State axis's adjudication is filed
at §7 without adjudicating the freeze row here.

---

## 2. Gate reproduction on this host

Every gate below was re-run at `d45458d`, not transcribed. Tuple §8's tails
are reproduced verbatim-equal unless noted.

| # | Gate | Tuple claims | Reproduced here |
| --- | --- | --- | --- |
| 1 | `cargo fmt --all -- --check` | clean | clean |
| 2 | Focused `cargo test --lib checked_artifact::` | 400 / 0 (1170 filtered) | **400 / 0, 1170 filtered** (38.71s) |
| 3 | Full `cargo test --lib` | 1569 / 0, 1 ignored | **1569 / 0, 1 ignored** (724.12s) |
| 4 | Integration binaries ×4 | 50 / 0 | **10+29+9+2 = 50 / 0** |
| 5 | Protocol currentness (3 by name) | 3 / 0 | inside the `protocol` binary's 29 / 0 (regenerate-and-byte-compare tests passing) |
| 6 | Boundary checker | ok (15 visible, 5 modules) | **ok (15 visible entries, 5 classified modules)** |
| 7 | Checker unit suite | 65 / 0 after §2.3 repair | **Ran 65 — OK** (562.7s) |
| 8 | Release-boundary suite | 6 / 0 | **Ran 6 — OK** (the "pushed main + v1.2.3" stdout line is a mocked-runner argv assertion, verified against `test_release_boundary.py:118-143`; no tag exists, tree untouched) |
| 9 | Merge-doc gate + suite | ok / 3 / 0 | **ok (11 sources, 147 assertions)**; suite OK |
| 10 | Merge-compat gate + suite | ok / 14 / 0 | **validated 7 migration rules and 7 runtime bindings**; suite OK |
| 11 | L2-04 retained readers (86 + validate + gate-ready) | 86 / 0, 24 tuples | **Ran 86 — OK; `{"status": "valid", "tuple_count": 24}`; `{"status": "manifest-ready", "tuple_count": 24}`** |
| 12 | Clippy all-targets/all-features | zero diagnostics from clean | **zero diagnostics** in a fresh CARGO_TARGET_DIR (equivalent to from-clean for gwz-core) |

Identity and arithmetic claims, all verified by direct measurement:

- Census: `fault_v1.rs` holds exactly **165** keys; per-family counts
  19+16+25+11+11+30+11+13+18+11 = 165; `EXPECTED_KEY_COUNT = 165`
  (`fault_expected_keys.rs:174`). Tuple §4.8's 107+2+38+18 = 165 reconciles
  (19+11+11+13+28+25 = 107 tabled).
- Matrix constants match the tuple's tables exactly:
  `ADMISSION_MATRIX: [Fault; 19]`, `NAMESPACE_MATRIX: [Fault; 11]`,
  `DURABLE_LEAF_MATRIX: [(Fault, LeafArmV1); 14]`, `RECORD_MATRIX: [Fault; 13]`
  (with the 9/4 partition test `the_repeatability_taxonomy_accounts_for_every_boundary`),
  `MANAGED_MATRIX: [Fault; 8]`, `MANAGED_INTENT_MATRIX: [Fault; 15]`,
  `MANAGED_WRITER_MATRIX: [Fault; 5]`.
- Injection-site line numbers spot-verified at 20+ rows of tuple §4 (e.g.
  `admission_mutation.rs:51/:68/:354/:410`, `leaf_observation.rs:174/:387/:389`,
  `namespace_mutation.rs:226/:242`, `managed_mutation.rs:433/:493/:519/:615/:923/:950/:1005-1007/:1044-1045`,
  `authority_record.rs:359/:365/:377/:388`,
  `authority_record_binding.rs:456/:503/:524/:552`) — every sampled site is
  the exact claimed key at the exact claimed line.
- Re-reserved families: sampled variants (`CleanupWorklistScratchCreate`,
  `BarrierIntentPublish`, plus six more incl. eight `runtime.*` keys and the
  `preflight`/`plan_complete` pair) return **zero** references outside
  `fault_v1.rs`.
- §5.4 outliers: 1251 (`managed_mutation.rs`), 884 (`platform.rs`), 5351
  (`src/protocol/generated.rs`), 1362, 1188, 1097, 1006, 993 — all exact.
- §5.3 blanket coverage: exactly **103 of 160 files** and **34,941 covered
  lines = 63.6%** recomputed independently (total-line denominator measured
  54,962 here vs the tuple's 54,956 — noise-level, see [P3-3]).
- §7 delta: 3 files, +60/−7, `coordinator/execution.rs` hunk is
  **comment-only**; zero `cfg(` lines in the whole delta. The corrected audit
  note's blanket list (bootstrap, capability, entry, fault_v1, leaf,
  namespace, protocol; `coordinator` absent) matches `checked_artifact/mod.rs`
  exactly.
- §11.3.7 verified: **zero** of the six workflows under `.github/workflows/`
  references `check_merge_docs.py` or its suite ([P3-6]).
- A1 fail-closed re-verified: `recover_or_create` has definition + sealed
  re-export only; no production caller.

## 3. Mandate 1 — the platform-cfg compile class, tree-wide

Census of cfg arms under `src/checked_artifact/`: 130 non-test-gating
platform predicates across 29 files (42 `unix`, 35 `windows`, 27
`not(windows)`, 12 `any(windows, test)`, 6 `target_os="linux"`,
4 `target_os="macos"`, the rest compound).

**Extinctness argument, three legs:**

1. **Native compile proof for every supported target.** Runs 19–21 compiled
   and ran the full suite natively on Windows, macOS-14 and ubuntu-24.04-arm
   at `9c454ce`/`51a9cba`/`514f8e6` (nine green arm-runs). `cargo test`
   compiles every `#[cfg(windows)]`/`#[cfg(unix)]`/target_os arm of lib and
   test code on its own OS, so every arm selectable on a supported target has
   been type-checked *and executed* natively at `514f8e6`. The `d45458d`
   delta carries zero cfg lines and its only production hunk is a doc
   comment, so the proof carries to the settled tree.
2. **The never-CI-compiled residue, audited by hand.** Ten arms are
   selectable on no CI platform (`not(any(unix, windows))` ×6,
   `not(any(linux, macos, windows))` ×2, `all(unix, not(linux|macos))`,
   `all(not(unix), not(windows))`): `identity.rs:598`'s fallback module,
   `platform.rs:412/:455`'s two `rename_relative` fallbacks,
   `advisory.rs:161/:169`, `alias.rs:84/:114/:150`, `index.rs:160`,
   `provider/platform.rs:17` (the `unsupported.rs` imp). Every one was read
   in full: all are typed-refusal stubs or trivial delegations using only
   unconditionally-imported items (`cap_std`, `std`, the file's own shared
   helpers `error`/`io_error`). No const-fn hazard, no missing import, no
   target-only API. (Direct cross-`cargo check` for
   `x86_64-pc-windows-msvc` and `x86_64-unknown-linux-gnu` was attempted and
   is blocked **in the C dependencies** — libz-sys needs Windows headers,
   openssl-sys needs a Linux OpenSSL — never in gwz-core Rust code; recorded
   as an environmental limit, with CI's native matrix as the stronger
   substitute.)
3. **The 2.4 fix pattern is the norm where it matters.** The one place two
   platform behaviours differ only by a boolean is
   `FOREIGN_EXACT_DURABLE_IS_WEAKER: bool = cfg!(windows)`
   (`authority_record_binding.rs:321`) — the always-compiled body, with its
   own doc stating exactly why. Everywhere else the `#[cfg]` arms differ in
   *implementation* (e.g. `flush_observed_leaf`'s `sync_all` vs documented
   no-op), which is the correct use of arms, and all of those compile
   natively per leg 1.

**E9 gate validity:** not in doubt, so the forced-flip proof was not
re-reproduced. Grounds: the constant is compile-time and always-compiled; the
gate (`require_authority_strength`) is four lines and was read; and the
Windows canary
(`the_leaf_flush_is_a_documented_no_op_on_a_read_only_handle`,
`leaf_observation.rs:658-674`) asserts the raw `sync_all` fails with
**os error 5** natively, so the no-op's justification goes red the day
Windows ever accepts a read-only flush. The canary executed in run 21.

**Verdict: the cfg-compile class is extinct tree-wide** on every target the
program supports, and the unsupported-target fallbacks are trivially
well-formed.

## 4. Mandate 2 — the named class audit (caller-supplied restatement)

Definition used: a gate that copies a **binding field** from a
caller-supplied argument into an issued record/capability/authorization,
rather than deriving it from what the gate itself observed or refusing on
mismatch with an observation. The class fired at 2.4 (P1: proof/parent
provenance) and 3.3 (P1: authorize_write bound a caller-chosen reservation);
one seam was hardened at 4.3 (settle item 8).

**Audited function list** (every issue/observe/validate/admit/authorize/bind
gate in the protocol, capability and coordinator layers; each read at its
definition):

| Gate | Binding-field origin | Status |
| --- | --- | --- |
| `CheckedAuthorityObservationOwnerV1::observe` (`protocol/authority_record/owner.rs:77`) | compares the transaction's own `action_digest` and `request_owner_binding` against the reservation **before** issuance; refuses typed | **hardened (4.3), verified** |
| `CheckedAuthorityObservationV1::owner_issue` (`authority_record.rs:93`) | copies four reservation fields — but is module-private and its sole caller is the gated seam above, post-equality | protected |
| `AuthorityFactsIssuerV1::issue` (`owner/host.rs:74`) | `action_digest` is an argument with the derivation obligation **on the signature**; the production transaction supplies `proof.action()`, whose slot names the stream physically read | sound; obligation stated where a future author will look |
| `observe_streamed_payloads` → `StreamedPayloadProofV1` (`authority_record_binding.rs:190/:227-243`) | `artifact_root`/`retained_parent_identity` minted from the retained capability; `action` is the caller's argument bound physically by slot-name derivation (a wrong digest's slots don't exist in the parent → typed refusal) and re-checked at the seam and at the join | sound; comment overstatement → [P3-2] |
| `validate_terminal_relation` (`authority_record_binding.rs:424`) | refuses a proof taken under a different retained directory (identity + path profile + action), refuses record/payload mismatch, refuses over-bound records — before the boundary key is announced | verified |
| `AdmittedCheckedActionV1::authorize_write` (`coordinator/execution.rs:248`) | the decisive third check compares the record's **observed** `retained_parent_identity` against `admitted.directory_identity()`; restated fields are named as restated in the doc | **the 3.3 fix, verified as landed** |
| `ScheduledCheckedActionV1::admit` (`execution.rs:137`) | belt-and-braces equality over the driver's own refusal | verified |
| `CatalogAdmissionOwnerV1::{classify_handoff, admit}` (`protocol/admission/owner.rs:86/:114`) | `AdmittedActionV1.directory_identity` comes from `exact_identity_for` — the **observed** final directory, gated on a byte-exact resident reservation and zero extra children; the copied `reservation` is therefore provably equal to the observed resident record | verified |
| `observe_action_directory` / `observe_action_interior` (`provider/interior.rs:504/:549`) | physical `dir_identity` + byte-compare of the resident record against the expected encoding; `Exact` is only minted on equality | verified |
| admission driver `resume_or_admit`/`resolve` (`admission/driver.rs:35/:169`) | decisions from observations only; capacity refused pre-first-write (`:111`) and re-proved in-window by `AdmissionCatalogInterior` (`publication.rs:234-275`) | verified |
| `ActionNamespace::validate_operation` (`namespace/mod.rs:297`) | binding equality **plus** physical `revalidate_action_directory` against the admitted identity per operation | verified |
| `ManagedParentBootstrapOwnerV1::bind` (`bootstrap/managed/owner.rs:179`) | plan digest must equal the resident schedule's `managed_plan_digest` AND the plan must **reproduce** the resident schedule (`try_from_managed_plan` + equality); provider revalidates plan against durable state | verified |
| `read_and_bind_managed_bootstrap_intent` / `matches_bound_plan` (`managed_bootstrap_record/codec.rs:189`, `managed_bootstrap_record.rs:398`) | binds reservation, purpose, plan digest, spec digest, ordinals, ranges, initial-parent identity/mode/path, component list, generation and predecessor | verified |
| `ManagedInstallRequestV1::bind` / `ManagedMarkerRetirementRequestV1::bind` (`namespace/managed.rs:71/:159`) | every field from the bind-verified intent ("none of them is caller-chosen") | verified |
| `HostActionNamespaceV1::validate_generation_slots` (`namespace/host.rs:337`) | slots minted only by `ActionNamespace` from the resident schedule; equality re-proved | verified |
| `CatalogPermitV1::owner_issue` / `MissingCatalogParentPermitV1::owner_issue` (`capability/pre_catalog.rs:484/:560`) | revalidate the lease-bound observation before minting | verified (R2-C shape) |
| `CatalogAttemptBindingV1::owner_issue`, `CatalogBootstrapRecordV1::owner_issue`, `InfrastructureRecordV1::owner_issue_for_catalog` | sole production callers derive every field from their own physical observations (`provider.rs:148`, `interior.rs:279/:352`) | verified (R2-C shape) |
| `BarrierIntentV1::issue` (`protocol/barrier.rs:39`) via `ActionNamespace::barrier_intent` (`namespace/mod.rs:205`) | reservation-derived binding fields; target identity/profile from the retained slots bound to the admitted action; **`catalog_anchor_identity`, `private_home_parent_identity`, `private_home_name` are caller-asserted with no observation check at the seam** | **[P3-1]** — no production caller exists (compile-proof and interface tests only; the `NamespaceBarrierAuthority` witness is mintable only inside `namespace/mod.rs`), so nothing reachable restates today; named as the R2-E obligation |

**Affirmative statement:** with the single exception named in the last row —
which is production-unreachable on this tree — **no remaining gate in the
settled tree copies a binding field from a caller-supplied argument without
either verifying it against an observation or deriving it from the retained
capability.** The class, as it fired at 2.4 and 3.3, has no live instance.

## 5. Mandate 3 — the anchor package and the 4.2 retirement

**ExactInterior no-op.** Exactly one production call site passes
`DirentBarrierClass::ExactInterior`: `namespace_mutation.rs:326` (E14; E10
reaches the same barrier through `host.rs`'s pin to the retained action
directory). All seven `AnchoredPrivateArea` sites (cleanup.rs ×3,
transition.rs ×1, residue.rs ×3) take the round trip. The no-op's soundness
chain was verified in code, not prose: every exact-interior row writer opens
through `durable_write_options` which sets `FILE_FLAG_WRITE_THROUGH` on
Windows (`directory_mutation.rs:703-718`; users: mutation.rs,
directory_mutation.rs, managed_mutation.rs ×2, admission_mutation.rs,
authority_record_binding.rs); rows move only by the sealed exact-handle
rename; a foreign row cannot be admitted (`extra_children: 0`,
`admission/owner.rs:29-38`); and the one consumer class that could have
leaned on the removed residual is refused outright by
`require_authority_strength` with the native os-error-5 canary keeping the
refusal non-vacuous. Sound for every current caller; the class is a caller
fact by design, so a future mis-classed caller is R2-E review surface.

**Ordinal retirement.** `smallest_free_ordinal` (`platform/anchor.rs:371`)
is sort/dedup/first-gap: `{0,2}` → 1, `{}` → 0 — no fixed point, unlike
count/max+1, exactly as the [P2-2] closure records. The chosen ordinal is by
construction non-resident; a racing occupant makes the no-replace publication
refuse and the next survey re-picks. Uncapped by design with the pigeonhole
bound; every `AnchorState` arm has a typed exit (`Missing`+family-state and
`Invalid` refuse; `NeedsReturn` republishes onto the identity-derived home;
`NeedsRetireAlias` retires then round-trips). Legacy nonce orphans
(`.ca1-anchor-scratch-<hex>`, dotted) fall through every survey class and are
ignored — tolerated, never reclaimed, exactly as recorded. Two nits (parse
laxity on retired ordinals; the unbounded legacy read in `verify`) are
[P3-5].

**Twin drift check.** `publish_verified_leaf_no_replace` (`platform.rs:199`)
vs `publish_verified_no_replace` (`publication.rs:132`), read side-by-side:
identical verification skeleton on the shared surface — open through
`open_rename_source`, identity re-read **through the retained handle**,
`try_reserve_exact(len+1)` + `take(len+1)` + byte-exact compare (the +1
catches appended bytes on both sides), then `rename_open_source` with
`replace` hardcoded false. The differences are exactly the documented ones:
identity vocabulary (legacy `ObjectIdentity` vs `HostPlatform`
`DurableObjectIdentityV1` — the closed-support-table narrowing the 4.1
review refuted), no directory/recheck arms on the legacy twin (its four
edges are regular files with `DestinationRecheckV1::None` semantics), and
error taxonomy (`ModelError` vs `CheckedFsError`). **No verification-logic
drift.** The unification stays correctly filed at R2-F (§11.2).

**E16 (2.3's deferred second axis).** The cross-parent atomicity record is
inline at the mechanism (`managed_mutation.rs:722-768`): rename is the
commit point; the three post-crash states map to executed matrix rows
(sites :718/:788/:794 verified); EXDEV and foreign removal refuse typed;
`observe_installed`'s interior check refuses the off-table wedge state
permanently. Matches the Branch-A ruling on the record.

## 6. Mandate 4 — the ownership token and the 3.1b chain read-back

The deterministic token (`provider.rs:564-573`) hashes only durable
bound-plan facts (plan digest, action, reservation, schedule, owner binding)
under a domain separator — first-generation seed only; every later drive
takes the token from the resident record. The boundary is stated at the
module header (`provider.rs:51-61`) in exactly the settled form:
self-consistency, **not** adoption or exclusion, with the re-litigation
trigger named.

**The boundary is uncrossed.** Verified three ways: (1) `resume_intent`
walks the chain link-by-link from the row's first generation, each link
refused unless `matches_bound_plan` + expected generation + expected
predecessor all hold — bind, never adopt; (2) `advance_one`'s
installed-resident short-circuit re-proves the marker byte-exact against the
intent's own issuance (`recover_installed_bootstrap_component` →
`observe_managed_component_interior(...).is_exact()`), so a foreign
directory at the final leaf is a typed refusal, not an adoption; (3) the
provider's product leaves the coordinator only as `ManagedParentFacadeV1`
(no path), and **no consumer beyond `coordinator/execution.rs` exists on the
tree** — R2-E has not arrived, so nothing makes determinism load-bearing for
exclusion.

## 7. Mandate 5 — `runtime.*`: what the six-variant mechanism does and does not cover

Filed as input to the State axis's adjudication of tuple §4.6 / §11.3.3.
The freeze row itself is not adjudicated here.

**What exists** (all verified in code and re-executed: the runtime suites
pass 31/0 on this host):

- `RuntimeBootstrapFault` (`bootstrap/runtime/fault.rs`) is a **six-variant,
  thread-local, single-shot callback hook** — schedule one callback, run it
  when the named point is crossed. All six production `fault::run` sites are
  `#[cfg(test)]`-gated (`runtime/mod.rs:139-145`, `catalog_lease.rs:101/:135`,
  `catalog_lease/target.rs:346-352`).
- The eight driving tests (verified present by name and passing) are
  **substitution/drift-rejection tests**: they mutate state inside a named
  window (identity swap between open and lock, parent substitution after
  lock, linked-membership drift between phases, git-lock replacement,
  target substitution) and assert the typed refusal.

**What this covers:** TOCTOU-class rejection at six lease/association
windows of the workspace-runtime bootstrap and the catalog lease — the
acquisition seams. These are real, executed, native-CI-green properties.

**What this does not cover, relative to the 18 declared-Executed keys:**

1. **No name correspondence.** The 18 keys name fine-grained durable edges
   (guard open/lock/reobserve/release, `.gwz`/locks directory
   create/reobserve, lease file open/reobserve-before/lock/reobserve-after/
   release, path_walk, collision_scan, capability_proof). The 6 variants
   name lease windows only; at most ~6 of the 18 edges are even *near* a
   variant, and 12 have no test naming their window at all. Eight sampled
   key variants have zero references anywhere outside `fault_v1.rs`.
2. **No interruption/restart/convergence form.** The mechanism runs a
   callback and continues (or the test asserts refusal); it cannot express
   "crash here, restart, converge" — there is no restart driver, no
   convergence census, no repeated-boundary crash row, for any of the 18.
3. **No variant pairing** in the L1-16/L2-14 form (the association tests
   exercise git-target membership, but no key has the paired
   workspace/git-directory matrix shape the other five executed families
   carry).

**Code-facts summary for the ruling:** "Executed" for `runtime.*` today
means *substitution-rejected at six acquisition windows*, not per-key edge
evidence. A mitigating code fact the ruling may weigh: the runtime bootstrap
edges are open-or-create/idempotent infrastructure creation under an
advisory lock, so several of the 18 keys name edges whose crash-recovery
story is structural idempotence rather than resident-record resumption —
but nothing on the tree *demonstrates* that per key, which is precisely the
gap the tuple reports. Both of the tuple's options (re-reserve like
`cleanup.*`, or restate the declaration to what the mechanism actually
proves) are consistent with the code; per-key evidence to the other
families' standard would require new injection sites (census-neutral but
code-touching) and a restart driver for the bootstrap path.

## 8. Discretionary re-verifications

- **E17 resident chain**: read in full (`resume_intent`,
  `settle_generation`, `retire_generation`); publish-before-retire makes the
  three durable states disjoint; classification exact, bounded by the
  scheduled generation range.
- **Seam refusals**: the 4.3-landed seam gate (`owner.rs:89-98`) plus the
  join guard's four clauses; `install`/`read`/`retire` have **no production
  consumer yet** (test drivers only) — consistent with "wires machinery,
  does not convert consumers".
- **Admission capacity refusal**: both halves present — driver pre-write
  (`driver.rs:111-115`) and the in-window re-proof with the 65th-row
  refusal + occupancy + retired-record equality
  (`publication.rs:240-275`).
- **§2.3 repair**: the drifted companion now asserts
  `"protected source tree changed: checked_artifact/platform.rs"` — pin
  named, class named; `platform.rs` sits exactly once in
  `PROTECTED_SOURCE_TREE_DIGESTS` (its `:198` reappearance is the separate
  rename-token count map, not a second pin); the 65-test suite is green
  against the landed pins.
- **§6 discharge**: the two multi-component tests exist, run
  `RowShapeV1::TwoComponent` over the same five boundaries per variant,
  deliberately assert no partition and no probe (correct — the single-crossing
  criterion is false on this shape by construction), and pass here (2/0).
- **Raw-rename surface**: checker-enforced at
  `{publication.rs: 1/1, platform.rs: 6/6/1}` — `rename_relative` has exactly
  one reference (its own non-Windows delegation), and the `replace=true`
  branch is production-dead (both sealed twins hardcode `false`).
- **Release-boundary suite side effect check**: the "pushed … origin" line
  is a mocked argv assertion; no push, no tag, tree clean afterward.

## 9. Findings

**[P3-1] `BarrierIntentV1::issue` accepts caller-asserted identity facts —
the named class's one remaining seam shape; R2-E obligation.**
`protocol/barrier.rs:39-70` copies binding fields from the reservation
(correct: the reservation is the binding root) but also accepts
`catalog_anchor_identity`, `private_home_parent_identity` and
`private_home_name` as bare arguments with no observation check;
`ActionNamespace::barrier_intent` (`namespace/mod.rs:205-228`) binds the
slots and target to the admitted action but passes the three anchor/home
facts through. Failure scenario (future): an R2-E roaming-anchor driver that
restates a stale or cross-catalog anchor identity mints a well-formed intent
against the wrong anchor — the exact shape that fired at 2.4 and 3.3. Today
this is unreachable: the `NamespaceBarrierAuthority` witness is mintable
only inside `namespace/mod.rs`, and `barrier_intent` has zero production
callers (compile-proof and interface tests only) — hence P3, not P1/P2.
Disposition: the R2-E package that gives the roaming anchor its first
admitted action must derive these three facts from a retained observation
inside the issuing transaction (the 4.3 pattern), or refuse on mismatch at
the seam; this sentence belongs in that package's brief.

**[P3-2] Provenance comment overstates at the streamed-proof constructor.**
`authority_record_binding.rs:228-230` ("Provenance, taken from the
capability these payloads were actually streamed through — not from a
caller, and not re-supplied later") is literally false for the first of the
three fields it covers: `action` *is* the caller's argument. The mechanism
is sound — the digest's provenance rests on slot-name derivation plus the
seam gate and the join guard, exactly as `owner/host.rs:53-66` states with
precision — but this comment is the one place a reader is told otherwise,
in the exact defect class this audit exists for. Fix: one clause ("`action`
binds by slot-name derivation; the other two are minted from the
capability").

**[P3-3] Record-precision notes (documents, not code).** (a) Tuple §5.3's
"29 `#[allow(dead_code)]` sites across 12 files / 56 across 28" is
scan-convention-sensitive: strict `#[allow(` paren-matching gives 27/10
(51/23 crate-wide); including `cfg_attr`/inner-attribute forms gives 30/13
(59/31). The claim's substance (naive grep wrong in both directions; 0 live
warnings; 481 hidden items; 103/160 files; 34,941 lines = 63.6%) verifies
exactly — only the convention behind "29/12" is unstated. (b) The
checkpoint's PHASE 4 SETTLED line "51 keys executed program-wide" reconciles
with no §3.5 sum (the phase-end executed count was 19+11+11+13+28 = 82
R2-D-family keys, 125 with runtime+catalog_bootstrap); the tuple's own §4.8
arithmetic is correct and controlling. (c) §5.3's line denominator: 54,956
vs 54,962 measured here (covered-line numerator matches exactly). All three
are phrasing/measurement notes; per the mandate, recorded and moved past.

**[P3-4] The settled tuple's own ten multi-component rows have no native
Windows/ARM execution record.** Runs 19–21 cover the three Phase-4 trees;
`d45458d`'s +36-line test delta post-dates run 21, so
`managed_writer_multi_component_*` (both variants) have executed only on
macOS hosts (green here, 2/0). Expected-green on Windows — the harness and
boundaries are identical, only the row shape parameter differs, and the
one-component writer matrix is natively green — but the platform ledger
should carry the entry so the next push's run is recognized as their first
native execution rather than a formality.

**[P3-5] Anchor protocol nits, all inside the accepted same-user boundary.**
(a) `anchor::verify` rides the legacy `observe_leaf_exact`
(`observation.rs:193`), whose read is an unbounded `read_to_end`: a foreign
multi-gigabyte plant at an anchor name is fully read into memory before the
bytes-mismatch refusal. Inherited from the legacy family's own contract
(the R1/R2-D record family is bounded; this path is slated for R2-F
retirement with the rest of the legacy readers) — worth one line in the
R2-F relocation/removal package's brief. (b) `survey`'s retired-ordinal
parse accepts non-canonical renderings the protocol never writes ("+7",
"007" → 7), classifying foreign plants as ours instead of Invalid;
consequence is only a conservatively skipped ordinal — convergence and the
no-wedge property are unaffected. (c) `platform/unsupported.rs:8-10`
returns `LinuxExt4FsIocGetFsUuidV1` as the support profile of the
never-supported platform; harmless today because every identity operation
on that arm refuses first, but it is a lie a future
`support_profile()`-before-identity caller could trip over — a one-enum-arm
fix whenever that file is next touched.

**[P3-6] Concurrence on the tuple's own CI-wiring item (§11.3.7).**
Verified: none of the six workflows runs `check_merge_docs.py` or
`test_check_merge_docs.py`, and both are named hard gates in
`AgentProcessRules.md` §4; they pass here only because this host has the
sibling `gwz-cli` checkout. The tuple's framing is correct ("a gate that
only passes on a developer's machine is not a gate"). Disposition
recommendation: wire both into the boundary workflow (or a docs workflow in
the parent repo where the manifest's targets live) before any lane starts
relying on L2-05 mechanically; no R2-D code consequence.

## 10. Escalation-ledger closure (Code axis)

Every riding trigger's Code-flavored substance was re-examined on the
settled tree: 2.3 (E15/E16 mechanism + the inline atomicity record — §5),
2.4 (P0 class extinct §3; provenance closed §4; 9/4 partition
machine-checked), anchor (1 P1 + 4 P2 — §5: no-op arm, ordinal retirement,
probe-backed rows), 3.1 (token boundary §6), 3.1b (chain read-back §6), 4.3
(the seam landing §4, first row). The discharged-on-record items (2.2, 3.3
round 2, 4.2 round 2) were verified as landed mechanisms, not re-opened.
The named-class audit ordered at the Phase 3 settle is §4's table and its
affirmative statement.

## 11. Verdict, restated

**GO for the R2-D settled gate.** The settled tuple's claims survive
adversarial re-execution and code-level re-derivation on every axis this
mandate names. The six P3s are: one R2-E seam obligation ([P3-1]), one
comment correction ([P3-2]), record-precision notes ([P3-3]), one platform-
ledger entry ([P3-4]), anchor nits for R2-F ([P3-5]), and concurrence on the
tuple's own CI-wiring item ([P3-6]). Nothing here blocks the last catalog
gate on the A1 path. The `runtime.*` freeze-row ruling is the State axis's,
with §7 as this axis's evidence.
