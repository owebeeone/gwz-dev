# Windows HTTPS integration — Safety P3 closure

**Date:** 2026-10-04  
**Scope:** Read-only text closure of Safety P3-1; no broader review or implementation acceptance.

**Reviewed tuple:**

| Repository | Revision |
|---|---|
| root | `2dd4ebf1c24d36c728d2ad60c257fd479de22cbf` |
| gwz-core | `c011aaee864fbe56c12a30b17664c099b8e67512` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `1e2798ad9e017947a521dbacf3680944693d5e86` |

**Verdict: GO — P3-1 CLOSED.** The prior Safety GO remains valid, with zero open Safety findings.

The corrected draft §3 and remediation plan now identify `TransportRuntime::open_request`. The acceptance text and core `GWZDesign.md`/`GWZRequirements.md` use the same accurate reference.

Pinned core source at `src/transport_host/mod.rs:265–297,348–361` confirms both convergence paths:

- `request` calls `open_request(..., None)`.
- `request_with_token` calls `open_request(..., Some(token))`.
- `open_request` contains the cited host-bound backend assignment.

This satisfies P3-1’s required correction and source-trace closure check. The historical `request_kind` mention in acceptance describes the corrected error; it does not designate an implementation method.

References and source were inspected using `git show` and targeted `rg`. All tuple revisions matched at the beginning and end. No files were written; no tests, builds or remote operations ran. Private fixture work was excluded.

This closes a documentation finding only. WH1 implementation and subsequent runtime qualification remain subject to their existing gates. Windows activation and release remain NO-GO.