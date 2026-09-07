# Gwz workspace-operation gaps: SSH identity selection, push ordering, and root-scoped tags

Date: 2026-09-06. Status: **proposal, not adopted.** No code change is implied by this
document. Written after two live incidents on the same workspace (`sdax-wz` + `sdax-rs` +
`sdax-v1` + a read-only Python `sdax` member): pushing it to GitHub (§§ 1-4), and tagging it
`code-complete` (§ 5).

Owner ruling already recorded (2026-09-06): a **git-CLI transport fallback is a
non-starter**; additional `gwz-cli` options are acceptable. This document therefore proposes
options and library support, not a shell-out.

---

## 1. What happened

Pushing the workspace to three new GitHub repositories surfaced two distinct things. One was
operator error and gwz reported it correctly. The other is a real gap.

The operator's machine holds two GitHub identities: `gripd` (work) and `owebeeone`
(personal). The keys **on disk** under `~/.ssh` (`id_rsa`, `id_ed25519`) both authenticate as
`gripd`; the key loaded in the **agent** authenticates as `owebeeone`, which owns the target
repositories. The `git` CLI can express that with a four-line `~/.ssh/config` host alias.
Gwz cannot express it at all.

Two invocations differed only in environment:

```sh
# fails: GIT_SSH_COMMAND is meaningless to libgit2/libssh2
GIT_SSH_COMMAND="ssh -o IdentityAgent=... -o IdentityFile=/dev/null" gwz push
#   RemoteRejected: SSH key authentication failed (no usable identity in the ssh-agent);
#   run `ssh-add` or check your SSH setup

# succeeds
SSH_AUTH_SOCK=/Users/…/.ssh/agent/s.… gwz push
```

**Gwz's diagnostics were good** and should not change: the message named the exact cause and
the remedy. The failure above was the operator's, not gwz's.

## 2. The gap: gwz can only ever use the ssh-agent

`gwz-core/src/git/gitbackend/transport_support.rs:65` is the whole of remote credential
resolution:

```rust
pub(crate) fn remote_credential(
    url: &str,
    username_from_url: Option<&str>,
    allowed_types: git2::CredentialType,
    credential_helpers: CredentialHelperPolicy,
    ssh_attempts: &mut u32,
) -> Result<git2::Cred, git2::Error> {
    let username = username_from_url.unwrap_or("git");
    if allowed_types.is_ssh_key() {
        // Offer the ssh-agent once. If libgit2 asks again, that attempt was rejected and
        // we have nothing else — return an error so it stops rather than looping forever.
        *ssh_attempts += 1;
        if *ssh_attempts > 1 { return Err(/* … */); }
        return git2::Cred::ssh_key_from_agent(username);
    }
    // … username / credential helper / default …
}
```

`git2::Cred::ssh_key_from_agent` is the only SSH path. Consequences:

| the git CLI can | gwz can |
|---|---|
| `~/.ssh/config` host aliases | no — libgit2 never reads ssh_config |
| `IdentityFile` (a key not in the agent) | no |
| `IdentitiesOnly` (pin one identity) | no |
| pick between two identities for one host | no |

`git2::Cred::ssh_key(username, publickey, privatekey, passphrase)` exists and is unused
anywhere in the tree (`rg 'Cred::ssh_key\b'` → no hits). The capability is one call away.

### 2.1 The hazard this creates

The incident was benign because the agent held exactly **one** key. With two identities
loaded — the normal state for someone who works and pushes personal repositories from one
machine — libssh2 offers agent keys **in agent order** and the first accepted one wins.
There is no way to pin the identity, and nothing in gwz's output names which identity
authenticated. The failure mode is not an error: it is a **successful push under the wrong
account**, discovered later in the commit attribution or an audit log. For a private
repository pushed under a work identity, or the reverse, that is the expensive kind of quiet.

### 2.2 Proposed: explicit identity, per remote and per invocation

Two layers, both additive and both defaulting to today's behaviour.

**Configuration**, alongside the existing `remotes` entry in `gwz.yml`, which already carries
`name`, `url`, `fetch`:

```yaml
remotes:
- name: origin
  url: git@github.com:owebeeone/sdax-rs.git
  fetch: true
  identity: ~/.ssh/id_owebeeone      # optional; private key path
```

**CLI**, for one-off and for the workspace-wide case:

```
--identity <path>          use this private key for every selected remote
--identity <name>=<path>   per remote name, repeatable
```

Resolution order, most specific first: `--identity name=path`, `--identity path`, the
remote's configured `identity`, then today's agent-only behaviour unchanged. When an identity
is given, offer **only** that key — the equivalent of `IdentitiesOnly yes` — so the wrong
account cannot win by ordering.

Passphrase-protected keys stay the agent's job; gwz should not prompt for or hold a
passphrase. If a configured identity is encrypted and absent from the agent, fail with a
message that says so.

There is an existing `credential_ref: Option<String>` on `OperationAttribution`
(`gwz-core/src/model/mod.rs:400`) and in the wire protocol
(`protocol/generated.rs:2096`). Whether the new identity should reuse that field or sit
beside it is an open question for whoever picks this up; it is named here so the two are not
designed twice.

### 2.3 Report the identity that was used

Independent of selection, and cheap: after a successful push, gwz knows which credential it
supplied. Naming it in the result (`--json` included) turns a silent wrong-account push into
something a person or a CI check can notice. Today's output says only `Ok`.

## 3. The multi-repository "transaction" — mostly already solved by ordering

The concern raised was that `gwz push` walks N repositories with no cross-repository
transaction, and GitHub offers none, so a partial push is possible.

That is true and unfixable. It is also **much less dangerous than it sounds, because of an
ordering property gwz already has.** The root's lock is a *pointer* to member commits, so the
only question that matters is which side of the pointer is published first:

| partial state | consequence |
|---|---|
| members pushed, root not | a cloner gets an older but wholly coherent workspace: every commit the lock names exists. A re-run converges. **Harmless.** |
| root pushed, members not | the lock names commits that exist on no remote. `gwz clone` fails or yields a broken skeleton. **The bad one.** |

Publishing the pointer last makes the bad state unreachable from a clean start. This is the
same discipline as writing data before the index.

**Observed:** three consecutive `gwz push` runs in the incident emitted `mem_sdax_v1`,
`mem_sdax_rs`, `@root` — members first, root last, every time.

**Established for commit, but not evidently for push.** `gwz commit` documents and tests the
property: `workspace_ops/handle_commit.rs:15` ("root last"), `:19`, `:240` ("Commit the root
last"), with `workspace_ops/tests/g13.rs:125`
`commit_fans_out_to_members_then_commits_root_last`. A search for the equivalent on the push
path (`workspace_ops/push_member.rs`) finds no such comment and no ordering test.

**Recommendation, and the cheapest item in this document:** if push's root-last ordering is
deliberate, state it where the code says it and pin it with a test named like the commit one.
If it is incidental, make it deliberate. It is doing real work for workspace integrity and
nothing currently prevents a refactor from reordering it.

## 4. Preflight — ergonomics, not correctness

Originally proposed here as a fix for partial pushes. After § 3 that framing is wrong:
ordering already makes partial pushes converge safely, so preflight buys **diagnosis**, not
integrity.

Its real value is that credential and permission failures are *systematic*, not random. A
wrong identity fails on every remote. Checking one selected remote's auth and reachability
before touching any turns N failed pushes and N error lines into one refusal that names the
cause. On a five-member workspace that is the difference between a clear message and a wall.

Ranked **below** § 2 for that reason. Worth doing when the identity work is done, since both
touch the same credential path.

## 5. Tags cannot reach the workspace root, and three statements say they can

Found 2026-09-06 tagging the `sdax-wz` workspace `code-complete`. Unlike § 2, this one is
**not** operator error and gwz's diagnostics do not cover it.

### 5.1 What was observed

The milestone needed the tag in two places: on `sdax-rs`, and on the workspace root, whose
commit is the thing that pins the member set. The member tag was one command. **The root tag
could not be created with gwz at all.**

| invocation | result |
|---|---|
| `gwz tag <name> --target mem_sdax_rs` | tags the member. Root **not** tagged |
| `gwz tag <name> --target sdax-rs` (by path) | same |
| `gwz tag <name>` (default selection) | tags all three members. Root **not** tagged |
| `gwz tag <name> --target @root` | `InvalidRequest: selected command does not support @root` |
| `gwz tag <name> --target @all` | same error |
| every member deselected via `--no-target` | `status: Ok`, nothing tagged |

`--target` itself is fine: it selects members correctly by id and by path. There is simply no
selector that names the root for this verb.

### 5.2 The implementation is deliberate; its documentation is wrong

`workspace_ops/handle_tag.rs:60` builds the repo set and the root never enters it:

```rust
let repos: Vec<PathBuf> = member_roots.clone();
```

Selection at `handle_tag.rs:47` goes through `resolve_locked_selection`, which hard-codes
`RootSelectionPolicy::Reject` and then filters `SelectedTarget::Root` out anyway
(`target_selection.rs:62-70`). The handler says so itself at `handle_tag.rs:49`:

> Root tag behavior is not specified in the target-selection rollout. Local and remote tag
> operations span selected members only; explicit `@root` is rejected by selection.

That is a coherent decision. **Three statements contradict it**, one of them user-facing:

| where | claim | true? |
|---|---|---|
| `handle_tag.rs:11-12` (doc comment) | tags are "fanned out to the selected members + the root" | no |
| `gwz tag --help` | "Local operations (create, list, delete) span the selected members plus the workspace root" | no |
| `handle_tag.rs:146` (comment on the local-list arm) | "count every tag across root + members" | no — it iterates `repos`, which is `member_roots` |

The third is disprovable in one command. With `code-complete` present on the root and on one
member, `gwz tag --list` reports `code-complete (1 member)`: gwz cannot see a root tag, let
alone create one.

**Free to fix, alongside § 3.** The behaviour need not change for the help text to stop being
wrong.

### 5.3 `@all` is collateral damage, and this part is a bug

`target_selection.rs:41-51` expands the includes, applies the excludes (`:44`), and *then*
rejects if the resolved set still contains the root (`:46`). Since `@all` expands to root plus
members, **any verb with the `Reject` policy refuses `@all` outright**:

| verb | `--target @all` |
|---|---|
| `tag`, `branch`, `stash`, `materialize` (via `resolve_locked_selection`) | `InvalidRequest` |
| `status`, `ls`, `commit` (`RootSelectionPolicy::Allow`) | accepted |

For a verb that by design never touches the root, `@all` most usefully means *every member*.
Refusing it teaches the user that `@all` is unreliable rather than that tags are member-scoped.

A workaround exists today and follows from the ordering above — excludes are applied before the
check, so the root can be subtracted back out:

```sh
gwz tag --list --target @all --no-target @root   # accepted
```

That works, and nothing says so. At minimum the error should name it.

### 5.4 What it cost

The root tag was created and published with plain git inside the root repo:

```sh
git tag -a code-complete -m '...'
git push origin code-complete
```

That is precisely the per-repo git operation a workspace tool exists to remove, and
`AGENTS_GWZ.md` tells contributors not to do it. Note that remote tag operations are
members-only by design as well (`--push` skips the root), so a root tag created by any other
means still could not be pushed by gwz.

### 5.5 Proposed

1. **Decide root tag behaviour.** The code calls it unspecified. Either fan tags out to the
   root like `gwz commit` does, or state that tags are member-scoped — but state it once, in
   the place users read.
2. **Fix the three comments** regardless of which way 1 goes. Free, and one of them is
   `--help`.
3. **Make `@all` mean "every member" for root-rejecting verbs**, or have the error name the
   `--no-target @root` subtraction instead of only refusing.

## 6. Explicitly rejected

**A git-CLI transport fallback** (`--transport=cli`, shelling out so `~/.ssh/config` is
honoured for free). Owner ruling, 2026-09-06: non-starter. Recorded so it is not re-proposed.

## 7. Summary

| # | item | value | cost | rank |
|---|---|---|---|---|
| 1 | SSH identity selection (config + CLI, `IdentitiesOnly` semantics) | removes the only class that can push under the wrong account silently | one `git2::Cred::ssh_key` call plus config, CLI and resolution order | **first** |
| 2 | Report the identity used | makes a wrong-account push visible after the fact | small | with 1 |
| 3 | Pin push's root-last ordering with a comment and a test | protects an invariant that already holds and is currently unguarded | very small | **cheapest** |
| 4 | Push preflight | one clear refusal instead of N failures | moderate | after 1 |
| 5 | Correct the three "members + the root" statements about tags (§ 5.2), `--help` included | stops the tool describing a capability it does not have | a comment and a help string | **free** |
| 6 | `@all` to mean "every member" on root-rejecting verbs, or name the `--no-target @root` subtraction in the error (§ 5.3) | `@all` currently fails on `tag`, `branch`, `stash`, `materialize` | small, in `target_selection.rs` | with 5 |
| 7 | Decide root tag behaviour — fan out like `gwz commit`, or declare tags member-scoped (§ 5.5) | today a workspace milestone cannot be tagged or pushed without dropping to per-repo git | a design call, then either implementation or one sentence | after 5 and 6 |
| 8 | git-CLI transport | — | — | rejected |

Nothing here is urgent. Items 3 and 5 are nearly free and each guards something real. Item 1
is the one with a genuine failure mode behind it. Item 7 is the only one needing a decision
before it can be sized: the code itself records root tag behaviour as unspecified.
