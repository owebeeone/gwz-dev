# GwzLocalClone — CONSISTENCY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzLocalCloneDesign.md` (UNTRACKED; SHA-256 `579c9fd82e32877aee9b02b59b4693d4bf5b7cca38e04adc7c42d49bb08b52ba`; design-only DRAFT, revision 2, 2026-09-03)
**Baseline:** gwz-dev HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b` (start and end of review; object digest unchanged)
**Date:** 2026-09-03
**Axis:** CONSISTENCY — the document against its controlling graph (internal contradictions; verbatim agreement with cited/controlling contracts; exactness of superseded-clause lists; satisfiability of its own examples and protocol sketch; unstated impacts on uncited live contracts)
**Verdict: NO-GO** — one P0 and seven P2 findings are open. I pre-commit to GO on a revision that resolves P0-1 and P2-1 through P2-7 as specified.

---

## 0. Evidence base

Tuple verified twice (`git rev-parse HEAD`; `shasum -a 256 dev-docs/GwzLocalCloneDesign.md`): HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`, object digest `579c9fd82e32877aee9b02b59b4693d4bf5b7cca38e04adc7c42d49bb08b52ba`. Neither moved.

Read (object and controlling graph):

- Object: `dev-docs/GwzLocalCloneDesign.md` (entire file, 347 lines).
- `dev-docs/GwzWtDesign.md` status banner + remaining W-* rows (working-tree UNACCEPTED banner only, as authorized).
- `dev-docs/GwzWtPlan.md` status banner + body authority sentences.
- `dev-docs/GwzM5-8I2RecordContract.md` §1 location/sidecars, §2–§3 path/workspace identity, §6 immutability.
- `.cursor/rules/transactional-apis.mdc` (full).
- Live protocol: `gwz-core/protocol/gwz.taut.py` (`CloneWorkspaceRequest` :1442–1447, `PullHeadRequest` :1610–1611, `PushRequest` :1619–1624, `MergeRequest` :1655–1670, `OperationPolicy.remote` :827–828, `ActionKind` :142–169); `gwz-core/docs/Protocol.md` Evolution.
- CLI bindings: `gwz-cli/src/clirequest/workspace.rs` (`CloneArgs.url: String` required), `gwz-cli/src/clirequest/invocation.rs` (`--dry-run` refused for clone), `gwz-cli/src/globalargs/parser.rs` (global `--remote`), `gwz-cli/src/clone_long.rs`.
- Merge start actually resolves `source_ref` as a git ref: `gwz-core/src/workspace_ops/merge/plan.rs` :174–179, :361–362 (`merge_analysis(..., source)`).
- Workspace discovery: `gwz-core/src/workspace/mod.rs` :6–8, :77–94 (`gwz.conf/gwz.yml`; `.gwz` is `RUNTIME_DIR`).
- Mutator lock: `gwz-core/src/operation/workspace_mutator_lock.rs` :25–28 (advisory lock file; remaining unlocked file is not stale).
- Tracked remotes: `gwz.conf/gwz.yml` (every member `remotes[0].name: origin`; member `path:` values are workspace-relative, no host paths).
- Process: `dev-docs/AgentProcessRules.md` §7.2 exact status/supersession language; L1-08 amend-don’t-reinterpret.
- Catalog path: `gwz-core/src/git/gitbackend/preservation_image.rs` `CATALOG_PRIVATE_PATH = ".gwz/catalog-final"` (confirms the design’s named exclude is a real object).

Out of scope as briefed: `GwxMergeFindingsDraft.md`, `GwzAiReqDd.md`; `--filesystem-strict` / ship (1); production code change; writing a plan; GwzWt implementation. GwzWt files were read only to test the object’s unaccept/supersede claim.

Deferred on instruction (not reported as findings): implementation, a phased plan, `--filesystem-strict`, building GwzWt.

---

## 1. Findings

### [P0-1] Verbatim create from `root` copies the family index; dest satisfies the “I am root” criterion

**Root cause.** §4.1’s copy-exclude list is `catalog-final` / live catalog only. §3 places the family index at `root/.gwz/local-family.yml` and a pointer at `clone/.gwz/family-root`. Register-before-copy (§4 :80) means the index already exists on the source when the copy runs. Root has the index and does not have `family-root`. Verbatim therefore plants a second index at dest and does not plant a pointer.

**Location.** Object §3 :42–46, :67; §4 :80–81; §4.1 :83–88, :89–98; §8.1 :210–216.

**Violated invariant.** §3: “The index lives only at `root`. Everyone else stores a pointer to `root` and reads the index from there.” §2: names are unique in one family; the graph of who cloned from whom is not the namespace — which is false if dest is a second root with its own copy of the namespace.

**Reproduction.** Follow §8.1 literally:

1. `cd ~/limbo/gwz-dev` (this is `root`).
2. `gwz merge feature/x` (optional; not required for this defect).
3. `gwz clone --local --name A ../gwz-dev-A` (verbatim default).
4. Observe dest `.gwz/`: `local-family.yml` is present (copied; register-before-copy already listed `A`); `family-root` is absent (source root had none). Dest matches the §3 root layout.
5. From A, `gwz clone --local --name B ../gwz-dev-B` registers `B` in A’s copied index, not in the original root’s index. Root’s index still has `A` and not `B`. Two namespaces, one `family_id`.

**Impact.** False composition of family identity. Subsequent `list` / `dispose` / `--remote` / merge-from-name diverge by cwd. Dispose on one “root” cannot sweep the other. This is the primary example, not an edge dest.

**Required correction.** State a closed dest-`.gwz` grammar, distinct from “copy `.gwz/`”:

- never copy `local-family.yml`;
- never copy `catalog-final`;
- always write dest `family-root` as a dest-relative path to the real root (not a source→dest string substitution);
- copy `.gwz/merge/` only under the verbatim merge rule.

Name the post-copy check: dest must not contain `local-family.yml`; dest must contain `family-root` whose target is the registering root.

**Closure / regression test.** Fixture: root with an existing index row; verbatim `--name A` to a sibling dest. Assert dest has `family-root` and no `local-family.yml`; `gwz local list` from dest and from root return the same membership; a second clone `--name B` from A appends only to root’s index.

---

### [P2-1] §7 protocol sketch is not the live taut contract, so “reuse clone / additive / no version bump” is unsatisfiable

**Root cause.** §7 names types and fields that do not exist, and treats required live fields as optional add-ons.

**Location.** Object §7 :165–191, sketch :174–186; framing :11–13. Live: `gwz-core/protocol/gwz.taut.py` `CloneWorkspaceRequest` :1442–1447 (`url=F(2, STR)`, `target=F(3, STR)` — neither optional); `MergeRequest.source_ref` :1658–1659 (there is no `source`); no `PullRequest` (only `PullHeadRequest` :1610–1611 with `meta` only, and `PushRequest.remote` :1622); preferred remote is `OperationPolicy.remote` :827–828. CLI: `CloneArgs.url: String` required (`gwz-cli/src/clirequest/workspace.rs` :8–10). Schema identity `gwz.protocol/v0` (`gwz-core/docs/Protocol.md` :4, Evolution :89–93: keep field tags stable; prefer additive optional fields).

**Violated invariant.** The object’s own layer table: “No protocol **version** bump if we only add optional fields and new request kinds the old binary never sends.” Additive optional fields cannot express a local clone that has no URL. Reusing `clone_workspace` requires changing `url` from required STR to optional — a message-shape change, not an optional add. The sketch’s type names are not the closed gwz-cli / gwz-core / gwz-py IR it claims to extend.

**Reproduction.** Attempt to encode `gwz clone --local --name A ../gwz-dev-A` as `CloneWorkspaceRequest` without a `url`: the taut message is illegal. Encode `gwz pull --head --remote A` as `PullRequest`: the type does not exist (the CLI flag already maps to `meta.policy.remote`, which the sketch does not name). Encode `MergeRequest.source = "A"`: the field is `source_ref`.

**Impact.** An implementer following §7 will mint the wrong IR, cannot keep v0 without a shape change or a new method, and will disagree with gwz-py/generated.rs field numbers. “Reuse `gwz clone`” is CLI-verb reuse only; the protocol reuse claim is false.

**Required correction.** Rewrite §7 against the live messages: either (a) a new `clone` method/request (new `ActionKind`, old core refuses typed) whose local form has no `url`, or (b) optional `url` plus local fields on `CloneWorkspaceRequest`, explicitly as a v0 shape change with a retained-reader gate. Name `source_ref`, `PushRequest.remote`, and `OperationPolicy.remote` / `PullHeadRequest` correctly. Stop calling the type `CloneRequest` / `PullRequest`.

**Closure / regression test.** A table in the design mapping each CLI example in §8 to an exact taut message name + field tags as they exist in `gwz.taut.py` after the chosen option (a) or (b), with a note of which live required fields become optional. A reviewer must be able to tick each row against the schema file.

---

### [P2-2] Merge source is specified both as a family name and as a git ref; §6’s `UnknownLocal` rule makes §8.1/§8.3 illegal and silently reinterprets frozen `source_ref`

**Root cause.** One token position (`gwz merge <token>`, `MergeRequest.source_ref`) is given two incompatible denotations, and the only resolution rule (§6) is the family-index lookup.

**Location.** Object §2 :34–35; §6 :152–159 (“`--remote A` and merge source `A` mean: load the index… A name that is not in the index is `UnknownLocal`.”); §7 :185; §8.1 :212 `gwz merge feature/x`; §8.2 :266–272 `gwz merge A` / `gwz merge C:lane/agent-17`; §8.3 :285–286 `gwz merge feature/x` / `feature/y`; §8.4 :300 `gwz merge C` → `UnknownLocal`; §11.2 default spelling. Live: `MergeRequest.source_ref` “Required only for start; independently resolved in every participant” (`gwz.taut.py` :1658–1659); `plan.rs` :174–179, :361–362 passes that string to `merge_analysis` as a git ref. I2 record identity includes `source_ref` as that git-ref string (record contract / fixtures). AgentProcessRules L1-08: do not silently reinterpret a frozen field.

**Violated invariant.** Internal: §8.1 and §8.3 are merge starts whose tokens are not family names; under §6 they are `UnknownLocal`. External: `source_ref` is already a git ref independently resolved per participant; “now also a family name” is a semantic change of a frozen field, not an additive optional. The object claims (framing :13, §7 table :169, §10 :333) it is not a new merge-record version — true of YAML fields — while changing the merge *request* denotation that those records persist.

**Reproduction.** After §8.1 has created family `{root, A, …}`:

- `gwz merge feature/x` (as §8.1 and §8.3 write) — `feature/x` ∉ index → §6 `UnknownLocal`. Today it is a legal start.
- `gwz merge A` (as §8.2 write) — `A` ∈ index → family-name merge. Today it looks up git ref `A` in each member (`plan.rs` :362).
- After `dispose C`, `gwz merge C` is `UnknownLocal` even if branch `C` exists.

No dispatch rule distinguishes `A` (name) from `A` (branch) when both exist.

**Impact.** Either existing merge-from-ref breaks inside a family, or family-name merge is not what §6 says, or both. Recorded `source_ref` values become ambiguous across dest continue of a verbatim-copied record (`source_ref: feature/x`) versus a name-merge record (`source_ref: A`). Unstated amendment of the I2 protocol’s append-only rule (“no existing … message shape … changes”) for `MergeRequest`.

**Required correction.** Split the denotations in the design (and, when implemented, on the wire): keep `source_ref` as a git ref; add an optional family field (e.g. `local_source_name`) or require the `name:ref` / `--remote <name> <ref>` form for family merges. State the dispatch: a bare token is a git ref unless a named flag/field selects the family index. Strike §6’s application of `UnknownLocal` to bare merge tokens that are git refs. Align §8.2/§11.2 with that dispatch; keep §8.1/§8.3 as git refs.

**Closure / regression test.** Design-level matrix, later a protocol test: in a family that contains clone `A` and also a branch `A`, `gwz merge A` (git-ref spelling) vs `gwz merge --remote A` / `gwz merge A:HEAD` (family spelling) are distinct requests; `gwz merge feature/x` remains a git-ref start and is not `UnknownLocal`.

---

### [P2-3] Bare hub cannot be a GWZ workspace under “only `.git` folders”, yet must host `.gwz/family-root` and answer `local list` / `dispose`

**Root cause.** §4.3’s on-disk shape contradicts §3’s pointer contract and workspace discovery.

**Location.** Object §3 :44–46; §4.3 :122–129; §8.1 :233, :251; §8.4 :296. Live discovery: `discover_workspace_root` returns the directory containing `gwz.conf/gwz.yml` (`gwz-core/src/workspace/mod.rs` :6–8, :77–94). `RUNTIME_DIR` is `.gwz`, not `.git`.

**Violated invariant.** §3: every clone stores `.gwz/family-root`. §4.3: “only `.git` folders — no checkouts”. A checkout-free root has no `gwz.conf/gwz.yml` and no `.gwz/`. §4.3 still lists `local list` and `dispose` as working on the bare clone.

**Impact.** The push-hub product either is not in the family (no pointer, not listable/disposable as a member) or is not bare in the stated sense. §8.4’s sweep of remotes on hub has no gwz process to run *on* hub.

**Required correction.** State the bare exception set explicitly: which non-`.git` paths exist at the hub root (minimum: `.gwz/family-root`, and either a stub `gwz.conf/gwz.yml` or a discovery rule that does not require one). If hub is git-only and not a gwz workspace, strike `local list` / `dispose` from the verbs that work *in* a bare clone; those verbs run only in checkouts and treat hub as an index row + git URLs.

**Closure / regression test.** After `--bare --name hub`, `discover_workspace_root(hub)` either succeeds under the documented stub or the design forbids running `gwz local *` there; `gwz local list` from a checkout still shows `hub bare <path>`; `family-root` exists at hub or the index-only model is stated.

---

### [P2-4] Family names are git remote names; only `root` is reserved; `origin` (and every tracked remote name) can be taken and then swept

**Root cause.** §6 installs/refreshes a git remote named exactly the family name. §3/§8 reserve only `root`. Tracked manifests already use `origin`.

**Location.** Object §2 :28–30; §3 :67; §5 :145–146; §6 :152–156; §8.1 :239–240. `gwz.conf/gwz.yml` :13–16, :23–26, … every member `remotes: [{name: origin, url: git@github.com:...}]`.

**Violated invariant.** §10 :335 “Not putting host paths in `gwz.yml`” keeps remotes as names+URLs in the committed manifest; those names are live git remotes. Reusing them as family names overwrites publication topology. §5 dispose “delete git remotes named `C`” is name-equality, not “remotes this family installed”.

**Reproduction.** `gwz clone --local --name origin ../gwz-dev-origin` is not the reserved-name refusal (only `root` is). §6 bind: each member’s git remote `origin` is refreshed to `{family member origin}/{member.path}` (a local path), replacing `git@github.com:owebeeone/…`. `gwz local dispose origin` then deletes remotes named `origin` from root, A, B, D, hub.

**Impact.** Loss of the committed upstream remote binding (reconstructible from `gwz.yml` only if dispose did not also confuse the operator into editing remotes). `gwz pull --remote origin` after the bind no longer means GitHub. Collision with any existing remote name (`upstream`, etc.) is the same bug.

**Required correction.** Reserve at least `origin` plus every remote name already present in any member’s git config / `gwz.yml` at create time; or namespace installed remotes (`gwz-local/<Name>`). Dispose must delete only remotes it installed (recorded in the index), never “any remote whose name equals the family name”. Collision with an existing remote name refuses create, typed, naming the holder.

**Closure / regression test.** Create `--name origin` refuses, naming the existing `origin` remote and a member path. Create `--name A` records in the index that it installed git remote `A` (or `gwz-local/A`); dispose A removes those and leaves `origin` URLs intact.

---

### [P2-5] `family-root` rewrite is specified as source-path → dest-path; the pointer names `root`, and dest need not be a sibling

**Root cause.** The only rewrite rule is substitution of the *source workspace path* with the *dest workspace path*. `family-root` names root, not source. Dest may be any path (`../elsewhere` in §8.1).

**Location.** Object §3 :64–66, :68–70; §4 :78–81; §4.1 :89–91; §8.1 :236 `gwz clone --local --name A ../elsewhere`; dest default `../<root-dirname>-<Name>` (§4 :79, §11.3).

**Violated invariant.** §3: a clone resolves names by opening `family-root`, then the index — never by asking its creator. A copied relative pointer that is valid for A as a sibling of root is invalid at `/tmp/my-clone` or `../elsewhere`. Source→dest substitution does not recompute dest-relative path-to-root; if the pointer contains the source path (A), substitution would point at dest, which is worse.

**Reproduction.** From A (`~/limbo/gwz-dev-A`, `family-root` relative `../gwz-dev`): `gwz clone --local --verbatim --name T /tmp/T`. Copied pointer `../gwz-dev` resolves to `/tmp/gwz-dev`, not `~/limbo/gwz-dev`. `gwz local list` in T fails or reads a foreign index. Same for §8.1’s `../elsewhere`.

**Impact.** Non-sibling dests (explicitly allowed) leave the family. Combined with P0-1, verbatim from root has no pointer at all.

**Required correction.** After every create, write `family-root` as a freshly computed dest-relative (or absolute, if you choose and then keep it untracked) path to the registering root. Do not copy source’s pointer. State that dest may be any filesystem path not nested in another family member (see P3-1).

**Closure / regression test.** Verbatim and clean creates to (a) sibling default dest, (b) `../elsewhere`, (c) an absolute non-sibling path; in each, `family-root` resolves to the same root index.

---

### [P2-6] §8.3 is illegal under §8.1 leftover state and under ordinary one-open-merge rules

**Root cause.** §8 is one family narrative. §8.1 leaves A with a copied open merge. §8.3 starts another merge in A.

**Location.** Object §2 :36–37; §4.1 :83–88; §8.1 :210–216 (“root is unchanged (still dirty, merge still open)”; A has “the same `.gwz/merge`”); §8.3 :282–287. Live start already refuses a second open merge; `plan.rs` :337–345 refuses a member that `has a merge in progress`.

**Violated invariant.** “Two clones may each have an open merge” is already illustrated by §8.1 (root and A). §8.3’s `gwz merge feature/x` in A is a new start, not status. One open merge per workspace remains the merge contract (object: “Ordinary merge rules, D’s own `.gwz/`” at §8.2 :268; I2 store `discover_open` multiplicity).

**Reproduction.** Execute §8.1 then §8.3 without an intervening abort/continue. A still has copied `.gwz/merge/<id>.yaml` and copied member `MERGE_HEAD`. `gwz merge feature/x` in A refuses.

**Impact.** The document’s own evidence section is not satisfiable as written. A reader who uses §8 as the spec will believe A can start `feature/x` while holding the copied merge.

**Required correction.** Either isolate §8.3 as a separate timeline (“abort/continue the copied merge first, or start from clean clones”), or replace A’s command with `gwz merge --status` demonstrating the copied merge, and start `feature/y` only on a clone that has no open merge (C). If the intent is two *distinct* merges on A and C, A must not still hold §8.1’s copy.

**Closure / regression test.** A numbered example script that a later plan can run: after 8.1, 8.3’s exact commands either appear with an abort step or are changed so each `gwz merge` start is legal.

---

### [P2-7] Create/dispose leak ordered filesystem steps and use `exists()` as authority, against the transactional-API contract

**Root cause.** The object specifies create and dispose as porcelain sequences with path existence as liveness, not as one commit point plus `recover()` of recorded intent.

**Location.** Object §4 :80–81 “Registers `<Name>` at `root` **before** the copy”; §3 :72–74 “if the index says `C` and `C`’s path is gone, that row is stale”; §5 :137–146 steps 1–5; §3 :68–70 pointer-break repair deferred. Controlling: `.cursor/rules/transactional-apis.mdc` :10–14 (“Do not leak porcelain files or ‘check then path-rename’ as the contract”; “One **commit point** and a **recover()** that is a function of recorded intent plus observed state”; “Fail-closed drift is data. A path string or `exists()` is not authority.”). Coordinated workspace writes cannot stay non-transactional (`:18`).

**Violated invariant.** Family create mutates identity (name), dest trees, dest pointer, and later git remotes — mutation + identity. Dispose is five steps with crash windows (name gone / tree remains; tree gone / remotes remain). Stale detection is path-gone (`exists()`). Partial copy: dest path exists, so §3 does *not* mark the row stale; other verbs treat a half-copied tree as a live member.

**Reproduction (create).** Register `A` pointing at `../gwz-dev-A`; crash mid-copy. Path exists → not the “gone” stale row; `list` shows A; `gwz merge --remote A` talks to a partial workspace. Reproduction (dispose). After step 3, crash before step 4: name gone, directory remains, leftover tree still has `family-root` and is a discoverable workspace (`gwz.conf/gwz.yml`).

**Impact.** Split-brain membership, orphan workspaces, and no specified recover that is a function of recorded intent. Interacts with P0-1 (copied index) and P2-5 (broken pointer).

**Required correction.** Specify create and dispose as: recorded intent (name, dest, mode, source, family_id) → one commit point on the root index → `recover()` from (intent, observed dest, observed pointer). Partial dest is fail-closed (not a member until complete), not “path exists ⇒ live”. Do not use “path is gone” as the sole stale predicate. Dispose’s remote sweep is part of recover, not a trailing best-effort.

**Closure / regression test.** Design-level crash table (register-no-dest; dest-partial; dest-complete-no-pointer; dispose after drop-before-rm; dispose after rm-before-sweep) each mapping to a single recover outcome. Later: a test that kills the copy and asserts `list`/`--remote` refuse A until recover completes or the name is disposed.

---

## 2. Invariant analysis

**Held.**

- Host paths are not in tracked `gwz.yml` (object §3 :48–50, §10 :335). Live `gwz.conf/gwz.yml` uses relative member paths and `origin` URLs only. Index paths are specified as untracked under `.gwz/`, which this workspace already excludes via git `info/exclude` `/.gwz/`. Display of absolute paths in `gwz local list` (§8.1 :243–251) is not a commit of host paths.
- `--bare implies --clean` (§4.2 :117–118) is consistent with `local_mode` as three values: bare is a distinct mode with clean’s no-dirt property, not a second copy of `--clean` plus leftover worktree state.
- Two clones not sharing `.gwz/` (§2 :36–37) is the right contrast with GwzWt’s shared catalog/refdb; dropping `catalog-final` (§4.1 :92–98) matches the live catalog path `.gwz/catalog-final`. Duplicate `workspace_id` in copied `gwz.yml` is how network `gwz clone <url>` already behaves; catalogs are path-local.
- I2 v1 body location (`.gwz/merge/<id>.yaml`), “no sibling acceptance file / sidecar journal” as scoped, and relative member `path` identity in lock rows are not bumped by this design. `RecoveryContextV1` has no workspace absolute path. The “not a merge-record version / not `--filesystem-strict`” claims are true of the YAML schema. The merge *request* denotation is the live break (P2-2), not a new `record_schema_version`.
- GwzWt **banners** match the object’s “GwzWt remains unaccepted” / “Not GwzWt” (working-tree `GwzWtDesign.md` :3–7, `GwzWtPlan.md` :3–5). `--filesystem-strict` is correctly out of this object.

**Broken** — see P0-1, P2-1–P2-7.

**GwzWt W-* residual (not separately blocking).** The object has no superseded-clause list of W-CMD/W-CMP/W-LIF/W-SAF/W-ENV/W-OBS rows. `GwzWtPlan.md` :12–14 still says, under the UNACCEPTED banner, that those W-* rows are the requirement surface. AgentProcessRules §7.2 wants `superseded for <exact scope>`. The banners already say “do not charter GwzWt builds” and “not the W-* rows below”; remaining dual authority is banner-vs-body inside those files, which were not the review object. Not raised as a P2 against this object; a later revision should still name the superseded scope in one sentence so the replacement is the exact-scope authority §7.2 requires.

**Unstated impacts (concrete, non-blocking except where folded above).**

- **Clone `--dry-run`:** `gwz-cli/src/clirequest/invocation.rs` :13–14 refuses `--dry-run` for `clone`. Local create is `gwz clone --local …` and therefore inherits an unplannable index mutation. Folded into P2-7’s recover/plan gap; not a separate ID.
- **Workspace discovery:** dest is a workspace iff it has `gwz.conf/gwz.yml`. Nested dest inside another member is not refused (GwzWt W-ENV-1 did refuse nesting). Bare hub is P2-3. Nested dest: see P3-1.
- **`gwz.lock.yml` / materialize:** network clone is clone-root + `materialize --lock`. Local `--clean -b` moves every HEAD to a new branch while dest’s cloned lock still names source branches (`gwz.yml` `desired.branch: main`, lock `branch: main`). Not a second ID: it is dest identity drift unless “then a new workspace” is defined to rewrite/capture the lock. Call out in P3-2.
- **Mutation lock:** copying `.gwz/locks/workspace-mutator.lock` is copying an unlocked advisory file; live code says that is not stale. The “rewrite lock cookie” line is unnecessary but not a contradiction.
- **`AGENTS_GWZ.md`:** copied; still system-managed per dest. No extra identity claim.

---

## 3. Risks and next action

NO-GO while P0-1 and P2-1–P2-7 are open. The object is a coherent product sketch (named family, index at root, two create modes, dispose sweep) whose controlling-graph bindings do not hold: the primary verbatim example composes two roots; the taut sketch is a different protocol than `gwz.taut.py`; merge-from-name reinterprets frozen `source_ref`; bare layout cannot carry the pointer contract; family names alias git remotes; examples are not all legal; create/dispose are non-transactional as specified.

I pre-commit to GO on a revision that resolves **P0-1, P2-1, P2-2, P2-3, P2-4, P2-5, P2-6, P2-7** as specified (closed dest-`.gwz` grammar; live taut names and a real additive-or-new-method choice; split merge-name vs git-ref; bare discovery exception; reserved/namespaced remotes; computed `family-root`; legal §8 sequences; intent+recover create/dispose). That revision should also fold P3-1 and P3-2 below; they do not independently block.

### Non-blocking

**[P3-1] Nested dest is legal and collides with discovery.** `discover_workspace_root` walks to the nearest `gwz.conf/gwz.yml`. `gwz clone --local --name N ./inside` (or dest under a member path) is not refused. Consequence: a nested dest hides or is hidden depending on cwd. Correction: refuse dest inside any family member’s directory prefix, typed. Test: dest under `root/gwz-cli/` refuses.

**[P3-2] `--clean -b` vs lock/manifest identity.** §4.2 creates `lane/agent-17` at source HEAD in every git member; dest still carries cloned `gwz.yml`/`gwz.lock.yml` pinning `main`. Consequence: dest is immediately lock-drifted; merge start / status / capture disagree with HEAD. Correction: state that clean create rewrites or recaptures the lock (and desired branches) to dest HEAD, or forbids `-b` until that rewrite exists. Test: after `--clean -b lane/x --name C`, lock `branch` equals `lane/x` in every member (or the design explicitly accepts drift).

**[P3-3] §11.6 `family_id` on the wire is not in the §7 sketch.** Default “yes” has no field tag or carrier message. Consequence: two implementers will disagree whether it rides on `CloneWorkspaceRequest`, `LocalFamilyRequest`, or the index only. Correction: put `family_id` on the exact message that binds dest to root, or strike “on the wire” if the index file is the only home. Test: the sketch lists the field tag.
