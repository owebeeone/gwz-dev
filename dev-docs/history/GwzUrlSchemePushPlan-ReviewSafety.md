# GwzUrlSchemePushPlan — SAFETY-AXIS REVIEW

**Review object:** `dev-docs/GwzUrlSchemePushPlan.md` at gwz-dev root `b64f691`. Status: plan; decisions D1–D9 taken 2026-09-14; no code; dated 2026-09-14. At the end of the review the working-tree file was byte-identical to the commit (`git diff --quiet b64f691 -- dev-docs/GwzUrlSchemePushPlan.md`).
**Baseline:**

| repository | commit | state |
|---|---|---|
| gwz-dev | `b64f691` | clean |
| gwz-core | `c9c7a98` | tracked files clean; two untracked documents out of scope |
| gwz-cli | `2c4c0ea` | |
| gwz-py | `334f53f` | |
| gwz-core-evidence | `b2bf115` | tuple only |
| taut | `7a5f616` | tuple only |
| taut-shape | `74f375c` | tuple only |
| taut-shape-rs | `df13036` | tuple only |
| taut-shape-py | `864ca0b` | tuple only |

The tuple was the same at start and end. Sources were read from the working trees while the tuple held, so they equal the commits. libgit2 1.9.7 was read from the vendored `libgit2-sys-0.18.8+1.9.7` crate that gwz-core's `Cargo.lock` pins.
**Date:** 2026-09-14
**Axis:** SAFETY — what the text lets go wrong: degraded and mixed-version paths, irreversible steps, how widely data or credentials are exposed, stuck states, and whether the "never worse than today" claims survive concrete interleavings. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P1 and four P2 findings block. Four P3 findings do not block. I pre-commit to GO on a revision that resolves P1-1, P2-1, P2-2, P2-3 and P2-4 as specified.

**How severity was graded.** The object is a plan, so nothing it describes is running yet. Owner decision D5 already accepts one way to false composition (§3.7, second bullet). A finding that lets the default publish a root naming a commit its member remote does not advertise, beyond that accepted case, is graded by how much it widens the exposure:
- persistent and reachable in ordinary workflows: P1;
- narrower preconditions: P2.

---

## 0. Evidence base

**Commands run.**
- Tuple check at start and end (the command given in the brief): identical output both times. The object file equals `b64f691`.
- `gwz push --help` (installed 1.0.12): no `--refspec`, no `--url-scheme`. `--force` is described as "Allow destructive behavior when required".
- Greps:
  - no `destructive` or `partial` read in `push_member.rs` or `publication.rs`;
  - no `prune` in gwz-core transport code;
  - `deny_unknown_fields` only in `src/stash/mod.rs`;
  - the only duplicate checks in `src/artifact` are member ids, the member path set, and remote names within one member.
- Nothing was built, tested, or sent over the network.

**Documents read.**
- The object: all 756 lines.
- `GwzUrlSchemePlan.md`: all (§1, §2.2, §2.3, §2.8 checked against).
- `GwzUrlSchemeAcceptance-2026-09-12.md`: all.
- `GwzProcessOptimization.md`: all.
- `AgentProcessRules.md`: L1-19 and L1-20.
- Prior review format: `GwzLocalClone-ReviewSafety-3.md`.
- gwz-core `dev-docs/GWZDesign.md`: 80–105 (publication), 675–700, 1840–1860 (Push), 2025–2060 (family remote token).

**gwz-core source at `c9c7a98`.**
- `src/workspace_ops/publication.rs`, all:
  - `ReadPreflight` 11–35;
  - `freeze_root_request` 37–66;
  - `checked_root_request` 68–108; its proof accepts an advertised target, or a local ancestor of one (86–99);
  - `dependency_was_published` 110–133;
  - `preflight_dependencies_with_reads` 187–216 checks read access only, not availability;
  - `root_dependencies` 218–301.
- `src/workspace_ops/push_member.rs`, all:
  - capture 174–220;
  - reads before any transfer 222–265;
  - parallel member pushes 275–317;
  - the root proof runs only when the root is pushed (318–376);
  - `published` holds only `Ok` rows (331–340);
  - aggregate status 742–770;
  - a successful push always sets `Ok` (784–803), so push never produces `Noop` today.
- `src/workspace_ops/handle_tag.rs`, all (the tag proof reads every dependency with an empty published map, 287–298).
- `src/git/gitbackend/push_plan.rs`, all (reads the remote for `:` and shorthand destinations, 31–43 and 76–87).
- `src/git/gitbackend/transport.rs`, all:
  - `fetch` 69–104 uses the configured refspecs and passes no prune option;
  - `ls_remote_url` 166–185;
  - `push_prepared` 453–474 pushes through the named remote when its configured URL matches the captured one;
  - `perform_push` 476–526 turns any per-ref rejection into an error.
- `src/git/gitbackend/transport_support.rs`, all (fetch options without prune 66–90; push rejection callback 92–110; credentials 112–235).
- `src/git/gitbackend/transport_support/identity.rs`, all (per-remote override refused for a non-SSH URL, 200–205; an invocation default ignores https, 206–207).
- `src/git/gitbackend.rs` 215–234 (`validate_url_identity` goes through `identity::for_remote`).
- `src/git/gitbackend/backend.rs`, all.
- `src/git/gitbackend/contract.rs` 215–300, 365–395, 950–1010.
- `src/git/gitbackend/refs.rs` 285–296 (`is_ancestor` passes errors through).
- `src/git/gitbackend/fake_repository.rs` 665–683.
- `src/git/url_scheme.rs`, all; `src/workspace_ops/url_scheme_state.rs`, all (refusal remedy text 109–117).
- `src/operation/par_map_per_host.rs`, all.
- `src/workspace_ops/tests/g01/tracking_backend.rs`, all; `src/workspace_ops/tests/g08.rs` 1–240 and 900–1081.
- `src/workspace_ops/pull_head_member_preflight.rs` 770–800 (pull fetches, then reads `refs/remotes/<remote>/<branch>`).
- `src/local_clone/adapters/git_config.rs`, all (a local clone keeps copied `refs/remotes/<name>/*` when install removes a URL, 14–16).
- `src/workspace_ops/target_selection.rs` 72–135.
- `src/workspace_ops/handle_create_repo.rs:779`.
- `protocol/gwz.taut.py`: `PlannedChange` 1617–1623, `MemberResponse` 1626–1645, `PushRequest` 1896–1901.

**libgit2 1.9.7, vendored.**
- `push.c` 164–212 (`git_push_update_tips`):
  - skips refs that carry a rejection message (178);
  - maps the destination through the named remote's fetch refspec (182–189);
  - writes the tracking ref with reflog "update by push".
  - It never looks at the push URL.
- `remote.c`:
  - 509–533: lookup applies insteadOf and pushInsteadOf;
  - 582–586: prune configuration is `remote.<name>.prune`, then `fetch.prune`;
  - 1420–1430: prune runs only when requested or configured.
- `graph.c` 177–183: equal object ids are not descendants.

**gwz-cli and gwz-py.**
- gwz-cli `src/clirequest/invocation.rs`: 100–110 (`--force` becomes `policy.destructive = Allow`); 244–259 (`PushRequest` sent with `refspec: None`).
- gwz-cli `src/globalargs/dispatch.rs` 268–274 and `src/globalargs/render_exit.rs` 82–95.
- gwz-cli `docs/MachineOutput.md` 616–626 (`Noop` exits 0) and `docs/Concepts.md` 100–116.
- gwz-py `src/gwz/cli_mutation.py` 40–60 and 205–220 (the push handler passes only `remote`).
- gwz-py `src/gwz/client.py` 780–815 (`push` accepts `refspec`).
- gwz-py `scripts/check_protocol_drift.py`, header.

**Object facts checked and found accurate.** Every §2.1 citation: `push_plan.rs:12-17`; `push_member.rs:306-311`, `233-237`, `222-257`, `533-541`, `764`; `transport.rs:485-490`, `459-472`, `471`; `publication.rs:289-297`, `143-153`, `192-216`, `68-108`, `11-35`, `116-133`, `92`, `187-191`; `url_scheme.rs:161-177`; `url_scheme_state.rs:72-91`; `handle_create_repo.rs:779`; `handle_tag.rs:125-130`, `151`, `163-175`, `287-298`; `transport_support.rs:195-235`; `backend.rs:44`; `identity.rs:200-204`; `tracking_backend.rs:421-423`, `453-467`, `469-477`. The one wrong fact is in P3-1.

## 1. Findings

### [P1-1] The default trusts remote-tracking refs that no gwz fetch removes

A branch deleted on the remote keeps "proving" availability and keeps its member from being pushed. §3.7's "any fetch closes the gap" is false.

**Location.**
- §3.5, definition of "Last-known ref" (lines 236–240).
- §3.5 rule 2: the "equal" table row and the dependency bullet (256–267).
- §3.7, second bullet (293–296).
- §7, "Stale last-known refs" (739–740).
- Step 3.5 tests (596–606); step 4.1 (643–646).

**Violated invariant.** A root is published only when every member commit its committed lock names is available from that member's committed fetch URL. The code defines "available" as advertised, or an ancestor of an advertised ref (`publication.rs:86-99`). D5 accepted only a remote "rewound or deleted since the last fetch or push", bounded by "The option, or any fetch, closes the gap".

**Mechanism.**
- gwz's fetch sets no prune option (`transport.rs:89-99`, `transport_support.rs:74-90`).
- libgit2 prunes only when asked, or when `remote.<name>.prune` or `fetch.prune` is set (`remote.c:582-586`, `1420-1430`). Both are unset by default.
- So `gwz pull`, gwz fetch and a plain `git fetch` all leave `refs/remotes/origin/<branch>` behind after the branch is deleted remotely.
- `gwz pull` then compares against that ref and reports the member up to date (`pull_head_member_preflight.rs:783-787`).
- The object never says which tracking ref proves a dependency. "A last-known ref of the remote" (265–267) reads as any ref under `refs/remotes/<remote>/`, which makes every never-pruned ref evidence.

**State sequence** (default mode; nothing is rewound anywhere):
1. Member `m` has `origin` = `git@github.com:o/m.git`, the standard fetch refspec and no prune setting. On branch `spike` the operator commits F and runs `gwz push --target m`.
   - The push creates `refs/heads/spike` on o/m.
   - libgit2 writes `refs/remotes/origin/spike = F` (`push.c:182-189`).
2. A teammate deletes `spike` on o/m (web UI, or `git push --delete` from another clone). There is no pull request, so no other ref advertises F.
3. The operator runs `gwz pull`. The fetch does not prune: `refs/remotes/origin/spike = F` survives, and pull reports `m` up to date.
4. The operator records `m@F` in the root lock, commits the root, and runs `gwz push`.
   - `m`: local `spike` (F) equals the last-known ref (F). Result: `Noop` "up to date with origin/spike as of the last fetch or push", not contacted.
   - root: ahead, so contacted. Dependency `m@F` is "at or behind a last-known ref", so it is proven without a read.
   - The root is published naming F, which o/m does not advertise.
5. Today (`c9c7a98`) the same push re-pushes `refs/heads/spike` (`push_member.rs:306-311`). o/m accepts it, and the shortcut or the post-push read proves F. That publication is sound. The plan makes it unsound, and also skips the member push that would have repaired it.

The deletion happened before the last fetch, so the accepted bullet does not cover this case, and the fetch in step 3 did not close the gap. gwz's own commands never repair this state. Only `--check-remotes`, a pruning fetch, or pushing the branch again ends it.

**Impact.**
- False composition, published to everyone who uses the root: a fresh `gwz clone` cannot materialize `m` at F.
- A member push the operator expects is silently skipped, and its stated reason claims an up-to-date state the remote does not have.
- Undoing it takes a forced root push or recreating the branch.

**Required correction.** Pick one option, write it into §3.5 and §3.7, and add the test below.
- (a) Never use last-known refs to prove root dependencies: read every dependency whenever the root is published. Member `Noop` decisions may still use last-known refs.
- (b) Make the evidence prune-aware:
  - gwz's fetch and pull prune the tracking refs of the remote they fetch;
  - the default trusts a tracking ref only if a gwz pruning fetch or a gwz push has refreshed it since it was written.
- (c) Record the persistent-deletion case as an explicit owner-accepted risk (amend D5):
  - replace §3.7's "any fetch closes the gap" with what is true;
  - document `fetch.prune` and `--check-remotes` in step 4.1.

Under every option, §3.5 must say exactly which tracking ref may prove a dependency.

**Closure test** (for options a and b; step-3.5 seam plus one native fixture):
- Setup: a bare remote; a member branch pushed through gwz, which writes the tracking ref.
- The branch is deleted in the bare remote from another clone, then a non-pruning fetch runs.
- The root lock records the member commit, and default `gwz push` runs.
- Expected: either the member is contacted or the dependency is read. The root is refused with the existing "cannot prove member ..." message, and the bare root ref is unchanged.

For option (c), a test instead pins that `--check-remotes` refuses the root in the same state.

### [P2-1] The push-URL condition is checked only against today's configuration

As a result, tracking refs written from a different repository are trusted as the committed repository's state.

**Location.**
- §3.5 "Last-known ref" (236–240: "gwz uses it only when the remote's push URL is absent or is the same repository as its fetch URL").
- Step 3.5 Backend bullet (587–590).
- Step 3.5 tests (603–604: "a renamed remote, and a push URL that is a different repository all contact the remote").

**Violated invariant.** The same as P1-1. D5 accepts a remote that rewound or deleted a commit it once held, not a remote that never held it.

**Mechanism.** For every accepted ref of a push through a named remote, `git_push_update_tips` (`push.c:164-212`) writes `refs/remotes/<remote>/<branch>` using that remote's fetch refspec, and never checks the push URL. gwz pushes through the named remote whenever the configured destination matches the captured one (`transport.rs:459-472`). Nothing records which URL wrote a tracking ref, so §3.5's condition, evaluated when gwz classifies, cannot see refs written under an earlier configuration.

**State sequence** (a fork push URL that is later removed):
1. Member `m` has `remote.origin.url = git@github.com:o/m.git` (the committed URL) and `remote.origin.pushurl = git@github.com:alice/m.git` (a fork).
2. The operator commits G on `main` and runs `gwz push --target m`.
   - G is pushed to the fork, and libgit2 sets `refs/remotes/origin/main = G`.
   - While the push URL stays configured, §3.5 correctly refuses to trust that ref.
3. The operator gains write access to o/m and runs `git config --unset remote.origin.pushurl`.
4. The operator records `m@G` in the root lock and runs `gwz push` before any fetch.
   - `m`: G equals the last-known ref, so `Noop`, not pushed. (Today it would be pushed to o/m, which publishes G.)
   - root: dependency `m@G` is at or behind `refs/remotes/origin/main`, so it is proven. The root is published naming G, which o/m never had.

**Other writers the condition cannot see.**
- `git remote set-url origin <another repository>` after pushing to or fetching from the old one.
- `git remote rename fork origin`, which moves the fork's tracking refs under `origin`.
- A second remote whose fetch refspec writes into `refs/remotes/origin/*`.
- A repository installed by a local clone that keeps copied tracking refs after install removed a filesystem URL (`git_config.rs:14-16`), then later gains a network URL.
- A fetch refspec whose destination is outside `refs/remotes/<remote>/` (for example `+refs/heads/*:refs/heads/*`). The "last-known ref" is then the local branch itself: every member is "equal" forever, and every local commit proves itself.

The next fetch repairs an existing branch. Branches the committed repository lacks stay, through P1-1's mechanism.

**Impact.** False composition for commits the committed repository never held, plus skipped member pushes. This is outside D5's acceptance.

**Required correction.** Tie trust to where the ref came from.
1. Accept a tracking ref only when all of these hold:
   - it lies under `refs/remotes/<remote>/`;
   - the named remote's fetch refspecs map exactly into that namespace;
   - no other configured remote's fetch refspec writes into it.
2. Then either:
   - record provenance whenever gwz writes or refreshes tracking refs (for example, a gwz-owned record of the same-repository class of the URL each fetch or push used) and check it when classifying; or
   - list these cases in §3.7 as an owner-accepted risk.
3. Step 3.5's "renamed remote" test must say which rename it covers.

**Closure tests.**
- (i) The sequence above on a native fixture, with two bare remotes standing in for upstream and fork. Expected: member contacted, dependency read, root refused if upstream lacks G.
- (ii) A second remote whose fetch refspec targets `refs/remotes/origin/*`. Expected: no last-known ref.
- (iii) A `+refs/heads/*:refs/heads/*` fetch refspec. Expected: no last-known ref.

### [P2-2] `--check-remotes` marks an already-published root `Noop` and never proves its dependencies

So the option misses the unsound root that today's push refuses.

**Location.**
- §3.5 rule 1, first bullet (245–246), and rule 3 (270–272).
- D5 (323–324: "an option that keeps today's check of every remote").
- §3.7, second bullet ("The option … closes the gap", 295–296).
- Step 3.3 (547–555) and its tests (557–565).
- Step 4.1 (645–646: use `--check-remotes` "before a release").

**Violated invariant.** The option is the stated remedy for the accepted risk and promises today's check. Today every `gwz push` that selects the root proves the root's committed lock after the member transfers (`push_member.rs:318-361`, `publication.rs:68-108`), even when the remote already holds the root commit. Step 0.2 confirms that every row, root included, was captured and pushed (9 push rows).

**State sequence.**
1. Root S is published and sound. Its lock names `m@L`, and `m`'s remote `main` = T contains L.
2. Someone force-pushes `m`'s main to R0, which drops L (a history rewrite). No other ref advertises L.
3. The operator fetches and runs `git -C m reset --hard origin/main` (or pulls with `--sync reset`). Local `m` is now R0; the root is still S.
4. The operator runs `gwz push --check-remotes`, as step 4.1 advises before a release.
   - `m`: read, main = R0 = source, so `Noop` "already on origin".
   - root: read, main = S = source, so `Noop` "already on origin". The root is not pushed, so `checked_root_request` never runs.
   - Rule 3's dependency reads do happen, but the text asks nothing of them. `preflight_dependencies_with_reads` checks read access only (`publication.rs:187-216`). Step 3.3 says it "answers from a kept advertisement" but not that it refuses.
   - Result: aggregate `Noop`, exit 0.
5. Today (`c9c7a98`), the same state and `gwz push` give:
   - `m`'s push succeeds with nothing to change;
   - the dependency was not published with its exact commit (R0 ≠ L), so gwz reads;
   - L is neither advertised nor an ancestor of anything advertised;
   - the root is `Rejected`: "root publication blocked: cannot prove member m commit L is available…".
   
   A root-only `gwz push --target @root` refuses the same way today. Under the plan, `--target @root --check-remotes` returns `Noop`.

**Impact.**
- `gwz push` loses the check that caught a published root naming an unavailable member commit, including in the mode the plan recommends before releases.
- Only `gwz tag --push`, which still reads every dependency, keeps that check.
- §3.7's "the option closes the gap" fails for any root published before the option runs.

**Required correction.**
- With `remote_check: always`, a selected root runs the availability proof of its committed lock from the kept advertisements, even when rule 1 makes it `Noop`. A failure marks the root `Rejected` with today's message and starts no transfer. (Alternative: under the option, rule 1 never makes the root `Noop`.)
- §3.7 and step 4.1 must state what the option verifies and what it does not.

**Closure tests** (step-1.2 seam and a native g08 fixture):
- The sequence above with `always`: root `Rejected`, nothing pushed.
- The root-only variant: the same result.
- The same state with a sound dependency: root `Noop`, with no reads beyond rule 3's.

### [P2-3] Evidence gathered before transfers is reused after this operation's own forced push to the same repository

D9's premise, that only someone else can rewind a remote during the operation, is false.

**Location.**
- D8 (332–336).
- D9 (337–341: "whose only extra coverage is a rewind by someone else during the operation").
- §3.5 rule 1, third bullet (247–251), and rule 2's dependency bullet (265–267).
- Step 3.3 (543–555): kept advertisements are "keyed as today by identity repo, remote and read URL", and D9 applies "when this operation did not push to that destination".
- Step 3.4 (570–583).
- Step 3.3's test "a destination pushed in this operation is never proven from its pre-push advertisement" (561–562) uses the same per-key idea of a destination.

**Violated invariant.** The root proof must reflect what the member repository advertises after this operation's member transfers. `c9c7a98` reads again after transfers, except for a member published with its exact commit (`publication.rs:187-191`, `116-133`).

**Reachability.**
- A forced refspec needs the protocol field `PushRequest.refspec` starting with `+`: gwz-py `client.push(refspec=...)` (`client.py:796-804`), or the retry in `g08.rs:1001`.
- That refspec applies to every selected target, root included (`push_member.rs:180-198`).
- Nothing in the manifest stops two members naming one repository. The only duplicate checks are member ids, the member path set, and remote names within one member.

**Sequence (a): `--check-remotes`, one repository reached under two keys.**
1. Members `app` (path `app`) and `site` (path `site`) both use `git@github.com:o/r.git`. The root lock names `site@S`; on o/r, S is reachable only from `main` = T.
2. The request selects `app` and `@root`, with refspec `+refs/heads/main:refs/heads/main` and `remote_check: always`. `app`'s local main is Y, which does not contain S.
3. Pre-transfer reads use keys (`app`, origin, url) and (`site`, origin, url). Both see main = T.
4. `app`'s forced push succeeds: o/r main is now Y, and S is no longer advertised.
5. For `site`'s key, the operation "did not push to that destination", so D9 answers from the pre-push advertisement (main = T, with S as an ancestor). The root is published naming S. Today: a fresh read sees main = Y, and the root is refused.

The same happens for a single member whose push remote name differs from its committed fetch remote name while both reach one repository — for example, a push-only remote kept so that a different `--remote-identity` applies to pushes.

**Sequence (b): default mode, one member.**
1. The root lock names `m@L`. The last-known `origin/main` is T, which contains L.
2. Local `m` main was rewritten to Y, which does not contain L. The refspec is `+refs/heads/main:refs/heads/main`.
3. Classification, before any read: `m` diverged, so contacted; root ahead, so contacted; `m@L` is "at or behind a last-known ref" (T).
4. `m`'s forced push rewinds o/m to Y.
5. Rule 1 says "anything else is read", but rule 2 says "proven by that ref, without a read". The text gives no precedence, and does not say when last-known refs are sampled.
6. If the snapshot taken at classification stands, the root is published naming L. Today: refused.

**Sequence (c): D8 with concurrent pushes.**
- `app` and `site` are both selected with the forced refspec, and both write `refs/heads/main` on o/r.
- `par_map_per_host` runs them concurrently on one host (the per-host limit defaults to 8), so the last writer wins.
- The losing push was still accepted, so D8 treats its lock commit as proven, though the winner's history may not contain it.
- Today's exact-commit shortcut already has this flaw in a narrower form; D8 widens it to every ancestor.

**Impact.** The operation publishes a false composition that it caused itself, straight through the barrier meant to stop exactly that. The destructive refspec came from the caller, but today the root proof catches it.

**Required correction.** Invalidate evidence per repository, not per key.
- After any transfer in this operation to a repository R (same repository by §3.1's rule, whatever the identity repo or remote name), kept advertisements of R and last-known refs of R sampled before that transfer are no longer evidence.
- A D8 proof holds only for a destination that no other forced or deleting transfer to R in this operation could have overwritten. Otherwise, read after the transfers.
- State that rule 2's dependency proof samples last-known refs after member transfers, and never applies to a repository this operation pushed to. Those go through D8 or a read.
- Correct D9's rationale.

**Closure tests** (a tracking-backend seam that knows which keys share a repository, plus one native g08 fixture):
- Sequences (a), (b) and (c) each end with the root refused and the root remote unchanged.
- A non-forced variant of (a) still uses the kept advertisement, with no extra read.

### [P2-4] Phase 3 changes push's JSON result for an unchanged workspace from `ok` to `noop`

The text says "as today", and the plan leaves no typed way to tell a verified `Noop` from an assumed one.

**Location.**
- §3.6 (278–285).
- §3.3 "Nothing else changes in JSON" (220, Phase 2).
- Non-goal "protocol changes other than the one additive push option" (49).
- Step 3.5 Output (594); steps 3.6 and 3.7.
- §6 test matrix; step 4.1's documentation list (633–647).

**Violated invariant.** Machine output of push must stay compatible for its consumers. D5's acceptance also relies on the operator knowing when a result rests on last-known refs.

**Facts.**
- Today push never produces `Noop` rows: every captured plan is pushed, and success sets `Ok` (`push_member.rs:306-311`, `784-791`). The all-`Noop` rule at `push_member.rs:764` is unreachable for push.
- Step 0.2 recorded a push of an unchanged workspace as "every member and the root `ok`" (402).
- `MemberResponse` has no field that can carry the reason for a `Noop` (`protocol/gwz.taut.py:1617-1645`). Only untyped strings (`planned.message`, `error.message`) exist; a new enum value or field would be a protocol change.
- A `Noop` aggregate still exits 0 (`render_exit.rs:82-95`, `MachineOutput.md:616-626`).

**Sequence.**
1. A script runs `gwz --json push` and requires `meta.aggregate_status == "ok"` (or every row `ok`) as its "published" gate.
2. After Phase 3, the same unchanged workspace returns `noop` rows and a `noop` aggregate, and the gate fails.
3. A script that does accept `noop` cannot tell "already on origin" (read during this operation) from "up to date with origin/main as of the last fetch or push" (assumed, and exposed to P1-1, P2-1 and §3.7). Only a free-text message would carry that difference, and the text does not say where it goes.

**Impact.**
- A silent contract change for JSON consumers.
- The machine-readable signal that separates verified from assumed state is lost, and the accepted risk depends on that signal.

**Required correction.**
- State this as a machine-output compatibility change, with the status values for each case, and add `MachineOutput.md` and gwz-py rendering to steps 4.1 and 3.7.
- Then either:
  - carry the `Noop` basis in a typed additive field (this needs an owner decision, because it conflicts with the §1 non-goal); or
  - record explicitly that JSON consumers cannot tell verified from assumed, and must use `--check-remotes` for a verified result.
- Name the field or string that carries the reason.

**Closure tests.**
- A gwz-cli JSON golden for an unchanged workspace under `changed` (rows and aggregate `noop`, reason where specified) and under `always`.
- A gwz-py rendering test for both kinds of `Noop`.

### [P3-1] §2.1 calls `gwz push --force` a forced push; it is not

So the "forced push" input used by §3.5 and step 3.1 has no defined source.

**Location.**
- §2.1 (112–113).
- The "forced push" column of the §3.5 table (256–262).
- Step 3.1 input "whether the push is forced" (518–519).
- D7's rationale uses the same assumption; the decision itself is not reopened here.

**Facts.**
- The CLI maps `--force` to `policy.destructive = Allow` (gwz-cli `invocation.rs:100-110`) and sends `PushRequest.refspec = None` (244–259).
- No push or publication code reads `destructive`.
- The Rust CLI has no `--refspec` on push, and gwz-py's push handler passes only `remote` (`cli_mutation.py:212-216`).
- The only forced push is a protocol refspec starting with `+` (`publication.rs:44`, `push_plan.rs:18`).

**Consequence.**
- An implementer following §2.1 may key step 3.1's "forced" on `policy.destructive`. Worse, they may "complete" `--force` by adding `+` to every selected refspec, which turns a global confirmation switch into rewinds of every selected remote.
- Under D6, an operator whose member is behind and who tries `gwz push --force` gets an exit-0 `Noop` "behind origin/main". Today the member row shows the remote's rejection.

**Correction.**
- Correct §2.1.
- Define "forced" as "every captured refspec for the destination starts with `+`".
- State that `--force` does not change push classification.

**Tests.**
- Step 3.1's table test keys on the refspec prefix.
- A CLI test shows that `--force` leaves a behind member `Noop`, with its reason.

### [P3-2] D3's new push refusal names a remedy push does not offer

**Location.** D3 (317–320); step 2.1 test "A malformed .gwz/url-scheme.yml gets the existing refusal" (491); §1 non-goal "a `--url-scheme` option on push" (44).

**Facts.** The existing message ends "delete the file, or run with --url-scheme manifest to clear it" (`url_scheme_state.rs:109-117`). `gwz push` has no `--url-scheme`.

**Sequence.**
1. The workspace has an unmaterialized lock member and a malformed `.gwz/url-scheme.yml`.
2. `gwz push` refuses root publication; this refusal is new under D3.
3. The operator follows the message with `gwz push --url-scheme manifest` and gets an argument error.
4. What actually works: delete the file, or run `gwz materialize --url-scheme manifest`.

**Correction.** Give the push refusal a remedy that works from push (delete the file, or `gwz materialize --url-scheme manifest`), and test the message on the push path.

### [P3-3] Phase 2 turns a working root-only push with `--remote-identity` into a refusal, and §3.3 says this cannot happen

**Location.** §3.3, second bullet (216–219: "That member's own push is already refused the same way (D4)"); step 2.1 test (492–493); step 4.1's advice on switching remotes (638–642).

**Facts.**
- Today a root-only push reads dependencies at their committed SSH URLs, and the override applies there (`publication.rs:192-216`, then `identity.rs:209-228`).
- After Phase 2, the read URL is the member's https remote, and the override is refused for a non-SSH URL (`identity.rs:200-205`, `gitbackend.rs:226-234`).
- In a root-only push the member is not selected, so "its own push" is not part of the operation.

**Sequence.**
1. The root remote is SSH. The members were switched to https, as step 4.1 advises "for the root and each member", but the root was not switched, or not yet.
2. `gwz push --target @root --remote-identity origin=~/.ssh/key`:
   - succeeds at `c9c7a98`;
   - is refused after Phase 2, with a message that names no alternative.
3. `--identity ~/.ssh/key` would work, because an invocation default does not affect https (`identity.rs:206-207`).

**Correction.** Correct §3.3, name the working remedy (`--identity`, or matching remote names) in the refusal or the docs, and add the root-only case to step 2.1's tests.

### [P3-4] Step 4.1's condition misses GWZDesign's statement that push proves availability

**Location.** Step 4.1, "Also" (650–651).

**Facts.**
- `GWZDesign.md:89-98` says push "waits for successful member transfers and proof that all member objects named by the committed root lock are available at their destinations, including partial and root-only pushes". `publication.rs:1-3` says the same.
- Under D5, the default infers availability from local refs instead.
- Step 4.1 updates GWZDesign only if it names the committed URL or describes the second read. This paragraph does neither, so the update never triggers.

**Consequence.** gwz-core's design document keeps promising a guarantee the default drops. Later work, such as the deferred deduplication for tag publication, can rely on it.

**Correction.**
- Make the update unconditional: the default proves from last-known refs within §3.7's risks, and `--check-remotes` reads.
- Update the module comment in `publication.rs` in step 3.5.
- Add a documentation grep to step 4.1's checklist.

## 2. Invariant analysis

**Attacks that failed** (what a GO would rest on once the findings are closed):

- **§3.1 same-repository rule.** `scheme_only_difference` (`url_scheme.rs:161-177`) is true only when deriving the committed URL on github.com, gitlab.com or bitbucket.org yields exactly the candidate. Path case, the `.git` suffix and nested groups are kept; nonstandard ports are refused. A true result therefore names the same repository path on the same host. The rule only chooses which URL to read, so it cannot make a proof pass that a read of the committed repository would fail. "It can only cost an extra read" holds for Phase 2.
- **§3.2 read-URL selection.**
  - A fork or `DISABLE` push URL fails the rule and falls through to rule 2 or rule 4.
  - The shortcut still requires `plan.remote == dependency.remote` and `plan.url == read_url` (`publication.rs:123-124`), so a push to a fork never counts.
  - Rule 3's derived URL stays on the same host with no identity repo, as today.
  - insteadOf and pushInsteadOf (`remote.c:509-533`) apply both to remote lookups and to the anonymous read, so they create no new mismatch.
- **Ancestry failure modes.**
  - libgit2 treats equal object ids as not descendants (`graph.c:179-180`), and `refs.rs:285-296` passes errors through.
  - Missing objects and shallow boundaries yield an error or false.
  - Every use of ancestry in the text fails toward contact or read: false or error means "diverged" or "unknown", so the member is contacted; a lock not provably behind is read; D8's "source or ancestor" falls back to a read.
  - The text should still pin error to "unknown" (step 3.1 already has that output), and test equality by object id, never by `is_ancestor`.
- **D8 and rejected refspecs.**
  - `perform_push` fails on the first per-ref rejection (`transport.rs:497-521`), and libgit2's local non-fast-forward check is also an error.
  - The row becomes `Failed`; only `Ok` rows enter `published` (`push_member.rs:331-340`); and a failed member blocks the root (318–373). A multi-refspec push cannot be half-accepted and still count.
  - Deletion refspecs publish no lock (`publication.rs:49-51`, `234-236`).
  - An annotated tag source must be peeled to its commit; an implementation that forgets falls back to a read.
  - Accepting a push to any destination is sound for any advertised destination; the concurrency hole is P2-3(c).
- **Step 3.4 concurrency.**
  - `par_map_per_host` returns only after every item finishes (`par_map_per_host.rs:71-106`), so parallel reads keep "no transfer until every read passes".
  - Push never reads `--partial`, so it cannot bypass that barrier.
  - The root proof still comes after member transfers.
- **§3.7, third bullet** ("left behind … never a skipped push"). A tracking ref behind the truth makes the member ahead or diverged, so it is contacted and goes through rule 1, costing at most one read. The claim holds for that case. The unsafe cases are refs ahead of the truth and refs never pruned (P1-1, P2-1).
- **Mixed versions.** A core that predates `remote_check` either ignores the field and contacts everything (today's behaviour) or rejects the request. Both are safe. An old client talking to a new core gets `changed`, which is D7's decided default.
- **Credentials and exposure.**
  - Reads through push URLs or https helpers stay on the committed repository's host, because derivation never changes the host.
  - These hosts advertise the same refs whichever account reads, so a different helper account can make a proof fail but not change its answer.
  - D4 fails closed, the default lowers the number of contacts, and no credential is sent to another host.
- **Tag publication** keeps reading every dependency (`handle_tag.rs:287-298`). It stays the conservative path: `gwz tag --push` still refuses a root the default published on stale refs.
- **Phase 2 alone:** no path was found that publishes a root today's code would refuse.
- **D6 divergence:** a behind member marked `Noop` lets the default publish the root, while `--check-remotes` blocks on that member's non-fast-forward rejection. Both results are valid compositions outside §3.7's risks, so this divergence is accepted, not a defect.

**Invariants that did not hold:**
- root publication soundness beyond D5: persistence (P1-1), provenance (P2-1), self-inflicted rewind (P2-3);
- the option's role as a detector (P2-2);
- compatibility of push JSON results (P2-4).

## 3. Risks and next action

**Residual risks below the finding bar.**
- A destination with no fetch refspec for its branch (single-branch clones, push-only remotes) is contacted on every push and never refreshed. The cost is one read per push, never worse than today.
- Refspec forms that make `prepare_push` read the remote (`:` and shorthand destinations, `push_plan.rs:31-43` and `76-87`) contact it before classification. "Nothing to publish makes no connections" holds only for the CLI's `refs/heads/x:refs/heads/x` form.
- Rule 3's https reads of private, unmaterialized members can start interactive credential helpers during a push. This fails closed.
- The owner-accepted §3.7 rewind case remains. Once P2-2 is fixed, `--check-remotes` and `gwz tag --push` are the ways to detect it.
- Two-round cap: all five blocking findings are corrections to the draft's rules and text. None requires redesigning the phase structure.

**Next action.** The plan owner revises §3.5, §3.6, §3.7, D9's rationale and steps 3.3–3.5 and 4.1 to resolve P1-1, P2-1, P2-2, P2-3 and P2-4, taking an owner decision wherever the chosen option requires one. Then request a bounded single-axis re-review against this report's IDs.
