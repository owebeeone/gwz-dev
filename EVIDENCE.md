# GWZ evidence campaigns

Raw evidence and experiment-specific runners belong in the private
[`gwz-core-evidence`](https://github.com/owebeeone/gwz-core-evidence) workspace member.
Organize by campaign, with separate named/dated runs; follow its README and
REPLAY.md before adding or rerunning a campaign. It is an archive, not a product
build target. Keep compiled outputs and runtime checkouts outside it.

Evidence follows the project under test: GWZ campaigns belong to
`gwz-dev/gwz-core-evidence`; SDAX campaigns belong to `sdax-wz/sdax-core-evidence`.
An attribution or shared-runner reference does not change the subject of a run.

Public repositories retain product tests, CI-required fixtures/runners,
contracts, design documents and concise reports. Public reports link to the
private raw evidence and label the access requirement. Do not make a public
build, test or release gate depend on access to this member.

The 2026-09-10 migration preserved hashes, original paths and source revisions
in the archive's `provenance/migrations/` directory. It did not rewrite public
Git history or modify remote experiments. Frozen paths in old records remain
historical; use a fresh replay checkout for new runs. Windows fixtures belong
under `D:/gwz-tests/<unique-name>/`, never C:.

Existing build caches were moved outside these repositories to
`/Users/owebeeone/limbo/evidence-build-cache/2026-09-10/`, not committed or copied.
The archive verifier rejects build/cache directories inside the archive.

The member has `private: true` in the managed manifest. Use a GWZ build with
private-member support: inaccessible private members are quietly skipped during
workspace clone and lock materialization. Repository names and URLs remain
visible in the manifest. Membership changes must go through GWZ, never a manual
edit to `gwz.conf/`.
