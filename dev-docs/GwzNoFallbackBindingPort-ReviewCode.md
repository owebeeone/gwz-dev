# L2-A qualified member binding port — CODE-AXIS REVIEW

**Review object:** Seven-file L2-A candidate: core `0fedb1a6..f9794d55a7c9e483fbf5c503a5e48dd1117b8507`, fork `dffaf272eb0e62ac15b74283c4e488252db9afc3..e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; controlling draft `gwz-core/dev-docs/GwzNoFallbackBindingPort.md`; 2026-09-20  
**Baseline:** root `bc4cddcfe9da7a44494d3c6632b41e3a87190165`; core `f9794d55a7c9e483fbf5c503a5e48dd1117b8507`; git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed blobs were read with immutable Git commands.  
**Date:** 2026-09-20  
**Axis:** Code architecture, source admission, dependency identity, isolation, and compatibility. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2, or P3 findings.

---

## 0. Evidence base

The complete tuple was verified at start and end. Final trees were root `133679540ca4bdfa0866cd193dbb1bae4ef51672`, core `586c2faa7a857fce54a5c86edae9b9f7a91f4977`, git2-rs `25d5d7354ca204b805f448c294ed8bd62fdecd86`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, and taut `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`.

The fork was clean. Concurrent core characterization and wiring edits were present exactly as the prompt permits and were not read as review inputs. Current peer prompt/report files were not inspected.

I inspected all seven committed files, the unchanged binding pin and patch, the native fixture manifest/lock, the release and candidate trees, and the root member lock. I ran the permitted focused command:

```text
python3 -m unittest discover -s gwz-core/tests/transport_native -p test_prove.py
......
Ran 6 tests — OK
```

No native build or seven-test campaign was repeated; their exact recorded evidence was inspected.

## 1. Invariant analysis

- **Exact source:** candidate `e883be38...` has sole parent `dffaf272...`. Its diff is exactly `Cargo.toml`, `src/remote_callbacks.rs`, and `src/transport.rs`. Root membership pins that commit and branch.
- **Exact binding:** release hashes are `cabd55...` and `cec1af...`; candidate hashes are the pinned `c5c75b...` and `f60a7c...`. The retained patch hash is `51586e...`. The public API and FFI ownership transfer are byte-identical to the previously accepted patch.
- **Native edge:** the only manifest change replaces release path dependency `libgit2-sys` 0.18.4 with registry `=0.18.8`. The fixture lock remains pinned to `0.18.8+1.9.7`, and whole-lock comparison permits only removal of registry provenance from the locally patched git2 package. The member’s old C checkout is neither selected nor copied.
- **Admission:** source mode derives its expected tree using fixed object `dffaf272...` with replacement objects disabled. It compares the complete admitted file set, regular/symlink type, executable mode, symlink target, and content. Only `.git`, root build output, and the unused C submodule checkout are omitted. The only accepted byte deltas are the exact manifest replacement and two pinned binding hashes.
- **Isolation:** admitted bytes are retained in memory and copied into a temporary tree before Cargo runs. Cargo receives that copy through its patch configuration. No subsequent operation reads build inputs from the member checkout, and registry caches and checked-in fixture state remain untouched.
- **Failure handling:** missing, extra, ignored, partially patched, content-drifted, and symlink-substituted inputs fail admission. The focused tests exercise positive isolation and these refusal paths. Unsupported release entry types and unexpected manifest structure also fail closed.
- **Compatibility:** `--git2-archive` retains its digest, safe extraction, patch application, resulting-hash, locked-graph, and test path. The two input modes are mutually exclusive and required. Source mode adds no default or implicit checkout selection.
- **Scope:** the committed delta is exactly seven files and remains within every checkpoint ceiling. No production GWZ manifest, lock, runtime route, package identity, C pin, or protocol changed.

## 2. Risks and next action

This qualifies a local external-consumption candidate on the recorded macOS host. Publication, package naming, full consumer/platform qualification, production activation, and SSH/pool routing remain separate gates.

Record the independent State and Surface verdicts. On GO from both, close L2-A with its exact evidence and retain this commit unchanged until the later distribution and activation packages.
