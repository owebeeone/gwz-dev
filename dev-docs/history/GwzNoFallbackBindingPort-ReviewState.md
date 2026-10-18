# L2-A binding member port — STATE-AXIS REVIEW

**Review object:** Seven-file L2-A candidate: core `0fedb1a6..f9794d55a7c9e483fbf5c503a5e48dd1117b8507`; git2-rs `dffaf272..e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; candidate awaiting review; 2026-09-20  
**Baseline:** root `bc4cddcfe9da7a44494d3c6632b41e3a87190165`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed files were read with immutable `git show`/`git diff`; concurrent working changes were excluded.  
**Date:** 2026-09-20  
**Axis:** State — source-admission state, isolation, partial/interrupted states, and fail-closed provenance. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks L2-A. I pre-commit to GO on a revision that resolves P2-1 as specified while retaining the other verified boundaries.

---

## 0. Evidence base

I inspected all seven changed files, the exact core and fork diffs, binding pins, accepted patch, locked fixture graph, checkpoint obligations, and L2-A report.

The two fork binding hashes are exactly the accepted values:

- `remote_callbacks.rs`: `c5c75b6e...e897dd45`
- `transport.rs`: `f60a7cf9...be09dc7`

The patch hash is `51586ea3...d6c3e6`; the sole manifest change replaces the release path edge with registry `libgit2-sys = "=0.18.8"`.

The permitted Python guard suite passed: **6 tests**. No costly native tests were repeated. Diff checks passed.

The tuple remained unchanged at start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `bc4cddcfe9da7a44494d3c6632b41e3a87190165` | `133679540ca4bdfa0866cd193dbb1bae4ef51672` |
| core | `f9794d55a7c9e483fbf5c503a5e48dd1117b8507` | `586c2faa7a857fce54a5c86edae9b9f7a91f4977` |
| git2-rs | `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68` | `25d5d7354ca204b805f448c294ed8bd62fdecd86` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

## 1. Findings

### [P2-1] Hidden Git attributes can redefine the supposedly immutable admission baseline

**Location:** `tests/transport_native/prove.py:70-97`, especially the `git archive` call at line 72; `test_prove.py:75-80`.

`copy_member` derives its expected release file set and bytes by running `git archive` inside the supplied checkout. Git archive honors repository-local `$GIT_DIR/info/attributes`, but `copy_member` deliberately excludes `.git` from inspection.

Concrete sequence:

1. Add `CONTRIBUTING.md export-ignore` to the checkout’s `.git/info/attributes`.
2. Delete working-tree `CONTRIBUTING.md`.
3. Run source mode.
4. `git archive` omits that file from `expected`; `os.walk` also does not see it, so the file-set equality check passes.
5. The copied candidate lacks a release file, while the native tests can still pass because that document is not a build input.

Thus hidden, unadmitted repository state can redefine the baseline and produce a false “exact release tree” qualification. `--no-replace-objects` does not neutralize archive attributes. The unit tests mock `git archive`, so they cannot detect this path.

**Required correction:** derive expected paths, modes, symlink targets, and blob bytes from attribute-independent object plumbing such as `git --no-replace-objects ls-tree` plus `cat-file`, explicitly handling only the known unused gitlink, or compare against a separately pinned trusted archive whose contents are not generated through checkout-local metadata.

**Closure test:** create a real temporary repository containing the pinned release, set `.git/info/attributes` to `export-ignore` a non-build file, delete that file, and verify admission rejects it. Retain positive member admission and archive-mode tests.

## 2. Invariant analysis

The remaining attacks held: verified bytes are copied before Cargo runs; symlink, executable-mode, missing, extra, ignored, manifest, and partial-patch drift are otherwise rejected; the fixture lock permits only the git2 source/provenance transition; archive mode remains intact; temporary source cleanup is automatic; and no production manifest, checkout file, registry source, publication, or runtime route is changed.

## 3. Risks and next action

Replace the attribute-sensitive baseline construction and add the real-repository regression above. Then rerun the six admission guards and both documented source/archive qualification modes; no broader program review is required.
