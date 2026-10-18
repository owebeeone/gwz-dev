# Git Library Q2 Candidate — CODE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzGitLibraryCandidate.md` and Q2 implementation at core `ea059ba89b1b61201b75c26708a99c7ca780d2a9`; private campaign at evidence `c82e38394947611b3848c9e73701ac378ae2917c`; bounded local admission/composition checkpoint dated 2026-09-21  
**Baseline:** root `7c017a5f1c3db1a743e5e1a0a62413fa5130755b`; core `ea059ba89b1b61201b75c26708a99c7ca780d2a9`; evidence `c82e38394947611b3848c9e73701ac378ae2917c`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; taut-shape-rs `df1303656583121e043f7a925c02958571951735`. Sources and evidence were read from immutable commits.  
**Date:** 2026-09-21  
**Axis:** Code — source-key portability, graph/source/feature composition, isolation, and claim accuracy. Independent, adversarial, read-only. The other axis ran independently; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

Inspected:

- Core diff `f70f4a74037bf291c9c6e1e72a7fb29676e4ab3f..ea059ba89b1b61201b75c26708a99c7ca780d2a9`, limited to the candidate report, Q1 pointer, verifier, and verifier tests.
- `prove.py` source traversal/admission logic and all twelve `test_prove.py` guards.
- Private `campaigns/git-library/runner/compose.py`, campaign README, frozen per-run runners, before/after locks, results, feature sets, command records, and relevant raw outputs for local-a, local-b, python-c, python-d, and metadata-e.
- Pinned manifests and workspace layout for all five independent candidate roots.

The only allowed execution passed: `PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s gwz-core/tests/transport_native -p test_prove.py` ran 12 tests successfully.

Start and end tuple checks matched, including root tree `b96977d95018ac52a46bbf5b74ec581be489f184`, core tree `0c5190acdf76588ef811f13593729db00f84330a`, and evidence tree `9ee1ac14fe7e0bd7e1efbd1227d54f1b4c4d63a6`. Only the two declared generated prompts were untracked.

## 2. Invariant analysis

The verifier now derives comparison keys with `Path.as_posix()` while retaining native `Path` objects for filesystem access. The regression exercises Windows-flavoured nested keys, directory and file forms of nested `.git`, unexpected and missing files, content drift, symlink substitution, and POSIX filenames containing literal backslashes. Existing mode, content, symlink, binding, sys-tree, C-gitlink, revision, and lock-provenance guards remain active.

The campaign exports immutable tracked blobs and modes, rejects Cargo configuration and escaping symlinks, admits the exact Rust/sys/C source tuple before and after each run, and keeps targets outside source and evidence repositories. Each consumer uses its own copied workspace and lock. Candidate lock comparison permits only the documented gwz-git package/edge, git2/sys source replacement, and native version alignment; unrelated package or edge changes fail.

The final metadata-e run records all five workspace roots with one path-sourced git2, one path-sourced `links = "git2"` provider, exact versions, and the expected feature unions. Local-b independently records successful workspace, core, CLI, and library builds plus their claimed smoke/tests. Python-d uses the corrected isolated manifest patch and records maturin wheel build plus direct extension import/health. Earlier lock-refresh, raw Python-link, and duplicate-provider failures remain visibly failed.

The public report accurately separates graph admission, local build/smoke evidence, and deferred native identity/platform qualification. No product manifest, lock, runtime, call site, or API changed.

## 3. Risks and next action

This checkpoint does not prove linked native identity, object format, remote-only reconstruction, publication readiness, or any platform beyond local macOS arm64. Those limitations are stated accurately.

Proceed to the separately reviewed per-consumer native identity/object-format and platform qualification gate before proposing production dependency activation.
