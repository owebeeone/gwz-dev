# GwzUrlSchemePushPlan — remediation plan, round 1

Date: 2026-09-14. Lane owner: the main session.

**Reviewed object.** `dev-docs/GwzUrlSchemePushPlan.md` at root `b64f691`,
with gwz-core at `c9c7a98`.

**Reports, both filed verbatim:**

- `GwzUrlSchemePushPlan-ReviewConsistency.md`: NO-GO. P2-1 to P2-5 block;
  P3-1 to P3-5 do not.
- `GwzUrlSchemePushPlan-ReviewSafety.md`: NO-GO. P1-1 and P2-1 to P2-4 block;
  P3-1 to P3-4 do not.

**Revision.** One patch to the plan, committed together with this file and both
reports. No code exists, so every closure is a corrected rule plus the test
that the revised plan now requires. Finding IDs below carry an axis prefix: `C-`
for Consistency, `S-` for Safety.

## Blind convergence

The two reviewers could not see each other's reports. They independently landed
on:

- **`--force` is not a forced push:** C-P2-1 and S-P3-1. Merged at P2.
- **The design's proof promise is not named for update:** C-P2-5 and S-P3-4.
  Merged at P2.
- **Push `Noop` is not "as today":** C-P2-5 (second half) and S-P2-4.

## Owner decisions taken in this round (2026-09-14)

- **D10, from S-P1-1 and the false-composition half of S-P2-1.** When a push
  publishes the root, every root-lock dependency is proven by this operation:
  by a read before any transfer (reused per D9) or by an accepted push (D8).
  Last-known refs never prove a dependency. The operator chose this over
  prune-aware tracking refs with provenance records, and over accepting the
  risk.
- **D11, from S-P2-4.** A `Noop` row's reason is carried in `planned.message`.
  There is no typed field, so this is not a second protocol change.

## Dispositions

Every blocking finding has exactly one disposition and one closure test. P3
findings are folded into the same patch.

### Consistency

| ID | disposition | where in the revision | closure test |
|---|---|---|---|
| C-P2-1 (with S-P3-1) | **Fix.** §2.1 says `--force` sets `OperationPolicy.destructive`, which push ignores, and that a forced push exists only as a `+` refspec from API callers. §3.5 defines "forced" by that prefix and says `--force` does not affect classification. D7's rationale is corrected. Step 3.1 keys on the prefix. | §2.1, §3.5 definitions, D7, steps 3.1, 3.5, 3.7 | Step 3.5: `destructive = Allow` with `refspec: None` on a branch behind its last-known ref gives `Noop` and no transport calls; the same state with a `+` refspec contacts the remote. Step 3.7: `push --force` parses to `refspec: None`. |
| C-P2-2 | **Fix.** Step 3.2's fan-out lists `handle_tag.rs`, `tests/g08.rs`, gwz-cli `invocation.rs:255` and gwz-py's two constructor calls. Commits: gwz-core, gwz-cli, gwz-py. The gate names the gwz-cli build and a gwz-py run without `GWZ_RUST_BIN`. | step 3.2 | After step 3.2, `cargo build --locked --bin gwz` from the gwz-dev root succeeds, and gwz-py `run_tests.py` passes without `GWZ_RUST_BIN`. |
| C-P2-3 | **Fix.** Step 1.2 enumerates the seam: committed files, resolved sources, per-path materialization, heads, ancestry with errors, per-URL advertisements that pushes can move, recorded reads and pushes, and a read overlap counter. Identity-option tests move to the native backend. The last-known-ref override moves to step 3.5. The Git2 mapping gets native tests in the new step 3.6. Step 1.2 is re-budgeted to about 450 LOC. | §2.1 seam facts, steps 1.2, 2.1, 3.5, 3.6 | Test 1.2(a) records exactly N+1 `ls_remote_url` calls and N+1 `push_prepared` calls. Step 3.6: a remote whose `pushurl` names a different repository, with an equal tracking ref, is contacted. |
| C-P2-4 | **Fix.** Acceptance case 4 runs on a fresh SSH clone of the published workspace, and every case runs `--dry-run` first. | step 4.2 | The acceptance note records the case-4 clone path, its root commit, and a dry run showing nothing to publish before the socket-polled push. |
| C-P2-5 (with S-P3-4; S-P2-4 in part) | **Fix.** Under D10 the default still proves root dependencies within the operation, so `GWZDesign.md:96-98` keeps holding. Changes: step 4.1 updates it unconditionally, naming the proof sources and that unchanged repositories are not contacted; step 3.7 edits the push help source behind `docs/commands/push.md` "Publication and authentication"; step 3.3 updates `publication.rs:1-3`; §3.6's "as today" becomes an explicit machine-output compatibility note, carried into `MachineOutput.md`. | §2.1, §3.6, steps 3.3, 3.7, 4.1 | Step 4.1's diff qualifies `GWZDesign.md:96-98` and `MachineOutput.md`; step 3.7's diff changes the push help source; a grep for "available at their destinations" finds no unqualified statement. |
| C-P3-1 | **Fix.** Step 4.1 depends on 2.1, 3.5 and 3.7, and the summary says the same. | step 4.1, dependency summary | Every "Depends on" line matches the summary. |
| C-P3-2 | **Fix.** Step 3.2 names `docs/generate_message_catalog.py` and gwz-py's `scripts/regen_protocol.py`. | step 3.2 | After step 3.2, the catalog's `PushRequest` table lists `remote_check`. |
| C-P3-3 | **Fix.** Tests and §6 rows added for: the read-URL refusal text (2.1); `--dry-run` (3.5); `noop` reasons and aggregate (3.5, plus the 3.7 JSON goldens); the gwz-py summary line (3.8). | steps 2.1, 3.5, 3.7, 3.8, §6 | Every bullet of §3.3, §3.5 and §3.6 maps to a §6 row. |
| C-P3-4 | **Fix.** Kept advertisements key an unmaterialized dependency with no identity repo, so rule 3 and D9 read it once. | §3.5 definitions, step 3.3 | Under `always`, one unmaterialized dependency is read exactly once. |
| C-P3-5 | **Fix.** A forced push is always contacted, including when its branch equals the last-known ref. | §3.5 rule 2 table | Step 3.5: a `+` refspec, an equal last-known ref and an advertisement at a descendant give contacted and pushed. |

### Safety

| ID | disposition | where in the revision | closure test |
|---|---|---|---|
| S-P1-1 | **Fix, by D10.** Last-known refs never prove a root dependency, and publishing the root reads every dependency before transfers. §3.5 says exactly which tracking ref may classify a member. §3.7 replaces "any fetch closes the gap" with what is true: gwz does not prune; a deleted branch keeps its member `Noop` until a pruning fetch, a push or `--check-remotes`; a root that needs the member's commit is refused. Step 4.1 documents pruning. | §3.5, §3.6, §3.7, D10, steps 3.5, 3.6, 4.1 | Step 3.6, native: a branch pushed through gwz is deleted in the bare remote from another clone; a non-pruning fetch follows; the root lock names the commit; a default push refuses the root with the §3.6 message, and the bare root ref is unchanged. |
| S-P2-1 | **Fix.** D10 closes the false-composition half. The last-known ref definition now requires the standard namespace mapping, no other remote writing into that namespace, and a same-repository push URL. Tracking refs written under an earlier configuration remain a consequence for member pushes only, recorded in §3.7. | §3.5 definitions, §3.7, steps 3.5, 3.6 | Step 3.6, native: (i) a fork push URL later unset leads to a refused root; (ii) another remote's fetch refspec writing into `refs/remotes/origin/*` gives no last-known ref; (iii) a `+refs/heads/*:refs/heads/*` fetch refspec gives no last-known ref. Step 3.5's renamed-remote test names `git remote rename fork origin`. |
| S-P2-2 | **Fix.** Under `always`, a selected root that rule 1 makes `Noop` still runs the dependency proof after member transfers; a failure reports it `Rejected` with today's message. §3.7 states what the option detects. | §3.5 rule 3, §3.7, step 3.5 | Step 3.5: a published root whose dependency was rewound, under `always`, gives root `Rejected` and nothing pushed; the root-only variant gives the same; the sound variant gives root `Noop` with no extra reads. |
| S-P2-3 | **Fix.** A forced or deleting transfer to a repository in this operation invalidates every kept advertisement and every D8 proof for that repository, across the identity repos and remote names that reach it; its dependencies are read after all transfers. A kept advertisement counts only when it proves availability. D9's rationale is corrected. Because D10 removes last-known refs from the proof, sequence (b)'s snapshot question disappears. | §3.5 definitions and rule 1, D8, D9, step 3.3 | Step 3.3: sequences (a), (b) and (c) each refuse the root; the non-forced variant of (a) reuses the kept advertisement with no extra read. |
| S-P2-4 | **Fix, by D11.** §3.6 states the machine-output change (`ok` becomes `noop`, exit code unchanged), that the reason is in `planned.message`, and that a checked result needs `--check-remotes`. `MachineOutput.md` (step 4.1) and gwz-py rendering (step 3.8) follow. | §3.6, D11, steps 3.7, 3.8, 4.1 | Step 3.7: JSON goldens for an unchanged workspace under `changed` and under `always`. Step 3.8: rendering of both reasons. |
| S-P3-1 | Merged into C-P2-1. | — | — |
| S-P3-2 | **Fix.** On the push path, the malformed-file refusal names the remedies push can use: delete the file, or run `gwz materialize --url-scheme manifest`. D3's wording is narrowed to pushes with an unmaterialized dependency. | §3.3, D3, step 2.1 | Step 2.1: the refusal text on the push path. |
| S-P3-3 | **Fix.** §3.3 states that a root-only push with a per-remote identity is newly refused, and names `--identity`. | §3.3, step 2.1 | Step 2.1, native: a root-only push with `--remote-identity` and an https read URL refuses before any network, naming `--identity`. |
| S-P3-4 | Merged into C-P2-5. | — | — |

### Residual notes folded in (no finding IDs)

- §3.4's byte-identical claim is limited to member remotes that equal their
  committed URLs. (Consistency §3)
- §3.2 rule 3 says `GWZ_URL_SCHEME` is client-side and push sends no scheme.
  (Consistency §3)
- Goal 3 now says each destination is read at most once. (Consistency §3)
- Step 4.1's switching recipe notes `.gwz/url-scheme.yml`. (Consistency §3)
- §2.3 notes that `:` and shorthand refspecs read the remote before
  classification. (Safety §3)
- §7 notes that https reads of private, unmaterialized members can start
  interactive credential helpers. (Safety §3)
- Not this plan: `GWZDesign.md:1601` lists a push `--refspec` flag that gwz-cli
  lacks. This is pre-existing and recorded in §7. (Consistency §3)

## Structural changes

- **No change to the phase structure.** D10 narrows how the default proves root
  dependencies, which is a rule inside Phase 3.
- **Steps renumbered.** The new step 3.6 (native tests) splits step 3.5's
  budget, so the Rust CLI step becomes 3.7 and the gwz-py step 3.8.
- **Classification belongs to the reviewers.** Whether any disposition is a new
  architectural root cause is the reviewers' call under the two-round cap, not
  the lane owner's.

## Re-verdict

The same reviewers re-verdict their own findings on the revised tuple, and file
`GwzUrlSchemePushPlan-ReviewConsistency-2.md` and
`GwzUrlSchemePushPlan-ReviewSafety-2.md`. Each report carries a prior-finding
closure table and a changed-range analysis.
