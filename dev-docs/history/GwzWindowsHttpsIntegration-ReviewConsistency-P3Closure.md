# Windows HTTPS integration — Consistency P3 closure

**Date:** 2026-10-04  
**Scope:** Read-only text closure of Consistency P3-1. No new broad review or implementation acceptance.

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

The tuple matched at the start and end of inspection.

**Verdict: GO — P3-1 closed.** No findings remain open from this reviewer’s design review.

| Finding | Closure evidence | Status |
|---|---|---|
| P3-1: correction misnames the shared request constructor | Draft §3:105 and RemPlan-1 now name `TransportRuntime::open_request`. Acceptance records that correction; core `GWZDesign.md` and `GWZRequirements.md`:13 use the same name. | **Closed** |

Exact-revision `git show` inspection confirms that core `src/transport_host/mod.rs:272` and `:284` route `request` and `request_with_token` into `open_request`, declared at `:286`. The cited host-bound backend assignment is inside that function at `:359`. The corrected references therefore identify the real owning scope.

No files were modified, and no tests, builds or remote calls were performed. Uncommitted private fixture preparation/results were excluded. This closes the documentation finding only; no implementation, live qualification, activation or release is accepted.