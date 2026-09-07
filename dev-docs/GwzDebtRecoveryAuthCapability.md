# Native SSH identity capability investigation

2026-09-07. Source inspection, six library cases and ten production-backend
SSH cases passed on macOS. File identity is implemented; exact encrypted-agent
selection remains unsupported. These are local results, not platform acceptance.

The current core lock selects git2 0.21.0, libgit2-sys 0.18.8+1.9.7 (libgit2
1.9.7), and libssh2-sys 0.3.2. Inspection used the downloaded sources named by
that lock, rather than assuming behavior from newer API documentation.

`git2/src/cred.rs::Cred::ssh_key_from_agent` takes only a username. In the
pinned libgit2 `src/libgit2/transports/ssh_libssh2.c::ssh_agent_auth`, the agent
lists identities, then calls `libssh2_agent_userauth` for each identity in order
until one succeeds. GWZ's current one-callback-attempt limit prevents repeated
callbacks; it does not restrict the keys tried within that single agent attempt.
It therefore cannot implement an explicit identity promise.

`Cred::ssh_key` accepts a private-key path and an optional public-key path and
passphrase. The pinned transport uses `libssh2_userauth_publickey_fromfile` when
that private-key field is present. That path does not select a key from an agent.
An encrypted-file failure must not fall back to `ssh_key_from_agent`.

libgit2 also implements `GIT_CREDENTIAL_SSH_CUSTOM`, passing an exact public key
and signing callback to `libssh2_userauth_publickey`. The pinned libssh2 header
provides `libssh2_agent_sign`. However, git2's public safe `Cred` API does not
expose a custom SSH constructor or a public raw-pointer constructor. Its raw
binding constructor is internal. Exact encrypted-agent support therefore needs
a reviewed library extension or a dedicated agent protocol signer integrated
through an appropriate safe binding. Adding a file-key constructor alone does
not solve the reported multi-identity incident.

The fixture demonstrates two different agent identities in both orders,
logging public fingerprints and server acceptance. The remaining extension must test
an exact-key signing route against the same server. No broad agent fallback may
be presented as explicit-key support. If the safe-library extension cannot be
completed in this recovery, ship an explicit unsupported result for that mode
and retain the original incident as an open capability limitation.

`OperationAttribution.credential_ref` is already a driver-local attribution
handle. It is not authority to load a private key. New transport selection must
use its own taut-defined option, with local machine configuration and a sanctioned
configuration command. No passphrase or private-key bytes belong in the request,
workspace artifacts, operation records, or diagnostics.

## Controlled results

`gwz-core/examples/ssh_identity_probe.rs` was built against the current locked
libraries. `scripts/checks/ssh_identity_probe.py` ran a temporary loopback sshd,
a separate ssh-agent socket, freshly generated keys and an exact generated
host-key SHA-256 check. The operator's agent, private keys and known-hosts file
were not used or modified. All fixture processes and keys were cleaned up.

| Case | Observed result |
|---|---|
| Agent A then B, both authorized | One credential callback; server authenticated A. |
| Agent B then A, both authorized | One credential callback; server authenticated B. |
| Explicit unencrypted A with B first in agent | Server authenticated A. |
| Explicit unauthorized key with authorized B in agent | Authentication failed; B was never accepted. |
| Explicit missing key with authorized B in agent | Authentication failed; no fallback. |
| Encrypted file key with that key unlocked in agent | File authentication failed; the constructor did not select the agent key. |

The server's accepted public fingerprints established which key authenticated;
the probe did not infer identity merely from callback success. Raw output is
`/tmp/gwz-debt-ssh-capability.json` in this local run. The example is intentionally
a library probe, not a new supported public transport path or a bypass of
production host-key checks.

Exact-agent capability remains separate work: expose libgit2's custom SSH
credential/sign callback through a safe git2 API, implement bounded exact-key
agent signing and lifecycle/error handling, then run these same fixtures with
encrypted identities in both orders, wrong/missing identities, rejection and
cancellation. File-key support cannot be marked as closing that incident.

## Production integration and remaining estimate

The same runner's `--product` mode invokes Git2Backend's real read-advertisement
path, including its identity resolver, callbacks, host validation and observations.
Ten cases pass: both ambient agent orders, explicit A with both agent orders,
per-remote A and locally configured A with B first, wrong key, missing key,
encrypted file with an unlocked agent key, and host-key mismatch. Success rows
report offered/authenticated and agree with server fingerprints; host mismatch
reports no credential offer. Evidence: `/tmp/gwz-debt-product-ssh-evidence.json`.
The fixture's libgit2 home override is confined to the child probe; it does not
change the operator's HOME, known-hosts file or agent. Human/JSON and native
Python bridge tests are separate driver evidence, not an end-to-end SSH CLI run.

A planning estimate for exact-agent support is 5–10 engineering days, excluding
upstream review and release latency: 1–2 days for the safe custom-credential API
and ownership contract, 2–4 for bounded agent enumeration/exact-key signing and
error/cancellation handling, and 2–4 for adversarial fixtures and macOS/Linux
validation. This is an estimate from the inspected API gaps, not measured work.
Windows agent/provider integration needs its own capability investigation before
an estimate or support promise. The extension must verify exact public-key
selection with both agent orders, encrypted keys, missing/rejected identities,
host rejection, cancellation and concurrent independent requests. Until then,
`exact_agent_identity=false` is the accurate public capability.
