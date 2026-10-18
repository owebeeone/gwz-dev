# Binding port merged remediation

One round; original core f9794d55, fork e883be38, root bc4cddcf.
Code/Surface GO; State NO-GO for one P2. No blind convergence.

State P2-1 accepted: checkout-local info/attributes changes git archive's output,
so excluded metadata can conceal missing release files. Replace source-mode
baseline construction with fixed-release ls-tree/cat-file object reads, with
replacement objects disabled. Explicitly omit only the existing native gitlink.
Add a real-repository regression for export-ignore plus a deleted non-build
file; run the same attack against a temporary clone containing the actual
release. Preserve positive admission, both native modes and existing graph guards.
No Rust binding, API, manifest, compatibility policy or activation scope change.
The original State reviewer verifies closure; Code receives the corrected tuple.
Surface contract unchanged, its original GO remains applicable to those bytes.
