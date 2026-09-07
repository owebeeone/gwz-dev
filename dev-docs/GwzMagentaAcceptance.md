# Magenta Windows publication acceptance — 2026-09-07

**Passed:** native Windows build, local lane work, authenticated publication of
all eight repositories, fresh checkout of the published workspace, one default
family merge, and ordinary lane disposal without force. This closes Dabeest's
missing Windows publication evidence for explicit-file authentication. It does
not certify every Windows authentication mode or the full release matrix.

## Source and environment

Machine: `gianni@10.1.1.238`, Windows, Rust 1.96.0. Existing GWZ checkouts and the
installed `gwz 0.8.2` were left unchanged. The Mac first published root
`2cb988b4aeb16ac75121b56aecffa65e3becc4c9`, with core
`bf58914476c6d2bf0aa28c8242c80d4c75b94039`. A copy of Dabeest's known Windows
binary bootstrapped the clone; Magenta then built its own executable using
`cargo build -p gwz --locked` and an external target directory.

Acceptance executable:
`C:\Users\gianni\git\gwz-magenta-build-20260907\debug\gwz.exe`.
Its compiled provenance reports both source repositories clean:

- CLI `1cb315a887fab4cc331de3399ca4941531730207`, source digest
  `22ebcd6a11b96f4d27baa207663b898a0ba68ab478e80741d6099aa99e12b4ec`.
- Core `bf58914476c6d2bf0aa28c8242c80d4c75b94039`, source digest
  `06dc44098056d12b759d1bf5a6a237d2e349429c5c9a42c1a2e9c925bd555cd6`.

Magenta's existing SSH agent refused to sign with its RSA identity. OpenSSH
with `IdentityAgent=none` and the existing private-key file authenticated as
`owebeeone`. GWZ used `--identity C:/Users/gianni/.ssh/id_rsa` for clone,
publication and materialization; transport observations report file credentials
and successful authentication. No private key was copied, and no agent or
persistent authentication configuration was changed.

## Executed cycle

1. Clone the published workspace into `~/git/gwz-magenta-acceptance-20260907`.
2. Create/switch all targets to `codex/magenta-acceptance-20260907`; commit the
   generated branch configuration through GWZ.
3. Create `magenta-work` at `~/git/gwz-magenta-lane-20260907`. Commit distinct
   proof files in root and core using `gwz add` and `gwz commit`.
4. Run ordinary `gwz --identity ... --ssh-timeout 30 push`. All seven members
   and the root report `Ok`; the root publishes after member transfers.
5. Clone the published root acceptance branch from GitHub into
   `~/git/gwz-magenta-fresh-20260907`, then use the newly built GWZ to
   `materialize --lock`. GWZ's one-shot clone has no root-branch argument, so
   this uses the documented Git root-clone + GWZ materialize form.
6. Verify fresh root HEAD equals `32db07ea3fc02728451a09dde940a0712d969908`,
   fresh core HEAD and lock equal `203c385c6d6ea6137c981fb450a329e7934f0249`,
   all seven members are clean and match their lock entries, and both proof
   files have the expected contents.
7. In the original receiver, run `gwz merge --remote magenta-work`, then
   `gwz local dispose magenta-work`. Both pass; no second root merge or force
   waiver. The lane directory is absent. Disband the now root-only family.

The acceptance branches were published in all eight GitHub repositories and
are retained as evidence. Test proof commits were not merged into production
`main`. The original receiver and fresh clone remain on Magenta; the work lane
was deleted. The receiver's final status is clean.

## Qualifications and follow-up

- Git's root clone honors Magenta's existing `core.autocrlf=true`, yielding CRLF
  for the root proof file. An initial LF-only assertion failed; inspection
  confirmed only the expected line ending conversion. The corrected assertion
  normalizes CRLF, while exact commit identities and every member lock remain
  independently verified. No product change was needed.
- Fresh materialization leaves a staged change to
  `gwz.conf/markers/conf-integrity.yml`. All member locks match and proof content
  is correct, but fresh root status is not completely clean. This corroborates
  the remaining derived-marker UX follow-up; do not describe this run as proof
  that marker-status churn is resolved.
- The Mac also exposed a staged derived-marker refresh following publication of
  the imported Dabeest commit. It was produced through GWZ, not a hand edit.
- Raw step logs and expected commit IDs: Magenta
  `~/git/gwz-magenta-evidence-20260907`; copies on the Mac at
  `/tmp/gwz-magenta-evidence`. Harness scripts remain beside the remote evidence
  directory. Aggregate Mac logs are `/tmp/gwz-magenta-work.log`,
  `/tmp/gwz-magenta-verify.log` and `/tmp/gwz-magenta-verify-resume.log`.
