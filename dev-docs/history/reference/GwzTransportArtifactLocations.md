# Transport artifact locations — 2026-10-03

The operator requested removal of loose coordinator artifacts from the workspace
parent. The relocation is complete: 210 selected entries (175 files and 35
directories) moved without deletion. Registered GWZ family checkouts were not
moved, disposed or modified. This is storage maintenance, not source acceptance.

| Material | Current location |
| --- | --- |
| Current plans, designs, acceptance records and concise reports | Root/member `dev-docs` |
| Historical handoff reports/design revisions/prompts | [transport-handoff-2026-10-02](history/transport-handoff-2026-10-02/README.md) |
| Authentication alternative report | [Windows authentication alternative feasibility](../gwz-core/dev-docs/GwzTransportWindowsAuthAlternativeFeasibility.md) |
| Native SSPI process investigation report | [Windows worker feasibility](../gwz-core/dev-docs/GwzTransportWindowsSspiWorkerFeasibility.md) |
| Raw coordinator runs, receipts and frozen experimental runners | [private relocation run](../gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-03-coordinator-artifact-relocation/README.md), requires private-member access |
| Retired builds, runtime fixtures, generated certificates/keys, virtual environments and parked recovery copies | `/Volumes/projects/limbo/evidence-build-cache/gwz-transport/2026-10-03/external-originals/<original-name>` |
| New native SSPI process investigation runtime | Under `/Volumes/projects/limbo/evidence-build-cache/gwz-transport/sspi-worker-20261003-*` |

The private run retains 779 original files (52,962,456 bytes), SHA-256 and modes,
including failed runs and original receipts. Twelve public report copies match
their source hashes. It records all 35 directory moves and their unchanged
device/inode identities. Build outputs and generated keys were not imported into
the evidence repository. `gwz-core-split-five-20261002` and `gwz-deps` were outside
the requested list and remain untouched.

The prior `/Volumes/projects/limbo/gwz-lanes-prewarm-20261003/` records now have
durable private copies under the relocation run's `imported/prewarm/`.
Loose `gwz-tr222-*.log`, receipts and runners are under `imported/parent-files/`;
the complete historical handoff record is under `imported/handoff/`.
The original external copies are preserved under `external-originals`, including
parking manifests and backups. No cache/backup retention decision was made.

The completed authentication-alternative run is also retained byte-for-byte in
MAIN's private evidence member at
`campaigns/transport-qualification/runs/2026-10-03-tr1-8-auth-alternative/`:
71 manifest-listed artifacts plus the manifest itself. The Windows lane's
original run remains untouched. The relocation run's
`auth-alternative-import.json` records the copy, hashes and original location;
this import does not broaden the experiment's claims or constitute design GO.
The completed native SSPI process run is similarly retained at
`campaigns/transport-qualification/runs/2026-10-03-tr1-8-sspi-process-worker/`:
34 manifest-listed artifacts plus the manifest. `sspi-worker-import.json`
records its exact copy and public report hash. No native tests were rerun during
either import, and the source Windows lane's files and heads remain unchanged.

Frozen reports and receipts retain original absolute paths and hash claims.
Their bytes were not rewritten to pretend a replay continued after relocation.
Absolute symlinks inside retired runtime trees may no longer resolve. Use a fresh
replay for future execution. In particular, the unexecuted Mac trust preparation
must receive fresh exact-path/certificate preflight before any approved execution;
no trust operation ran during relocation.

Archive verification passes, every retained file matches its recorded SHA-256,
size and mode, and all public copies match. The family catalog still lists the
same five ready checkouts. Future agents must use these storage boundaries;
public CI must not depend on the private archive or external build caches.
