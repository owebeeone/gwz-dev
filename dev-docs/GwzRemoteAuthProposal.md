# Gwz remote authentication: SSH identity selection, push preflight, and the ordering invariant

Date: 2026-09-06. Status: **proposal, not adopted.** No code change is implied by this
document. Written after a live incident pushing a three-repository workspace
(`sdax-wz` + `sdax-rs` + `sdax-v1`) to GitHub.

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

## 5. Explicitly rejected

**A git-CLI transport fallback** (`--transport=cli`, shelling out so `~/.ssh/config` is
honoured for free). Owner ruling, 2026-09-06: non-starter. Recorded so it is not re-proposed.

## 6. Summary

| # | item | value | cost | rank |
|---|---|---|---|---|
| 1 | SSH identity selection (config + CLI, `IdentitiesOnly` semantics) | removes the only class that can push under the wrong account silently | one `git2::Cred::ssh_key` call plus config, CLI and resolution order | **first** |
| 2 | Report the identity used | makes a wrong-account push visible after the fact | small | with 1 |
| 3 | Pin push's root-last ordering with a comment and a test | protects an invariant that already holds and is currently unguarded | very small | **cheapest** |
| 4 | Push preflight | one clear refusal instead of N failures | moderate | after 1 |
| 5 | git-CLI transport | — | — | rejected |

Nothing here is urgent. Item 3 is nearly free and guards something real. Item 1 is the one
with a genuine failure mode behind it.
