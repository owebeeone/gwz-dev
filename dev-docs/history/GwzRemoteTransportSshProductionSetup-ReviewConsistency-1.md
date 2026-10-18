# SSH Production Setup Design Remediation 1 — Consistency-Axis Review

**Review object:** `gwz-core` `9acf508aefe4ef974e52f19016f33ecf4bf56b34`, revised `dev-docs/GwzRemoteTransportSshProductionSetup.md` and `GwzRemoteTransportRequirements.md`; remediation 1 dated 2026-09-21  
**Baseline:** root `eadf8dc25f93b3f8d9d9c4f3660732367861559f`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`. Sources were read from immutable objects with `git show` and `git diff`.  
**Date:** 2026-09-21  
**Axis:** Consistency — focused closure of original P2-1/P2-2 and their changed-range interactions. Independent, adversarial, read-only. The current peer axis was not consulted. Filed verbatim by the lane owner.

**Verdict: GO** — both original P2 findings are closed. One new nonblocking P3 wording defect remains; it should be corrected before treating the document as final implementation guidance.

---

## 0. Evidence base

I read:

- The complete canonical remediation prompt and committed `GwzRemoteTransportSshProductionSetup-RemPlan-1.md`.
- The exact core diff `b03d53c902a585da8dae6da316cdc499ba99bec1..9acf508aefe4ef974e52f19016f33ecf4bf56b34`.
- Revised `GwzRemoteTransportRequirements.md:145-161`.
- Revised `GwzRemoteTransportSshProductionSetup.md:1-153`.
- The accepted A2 ordered-key contract and original trust-file counterexample evidence retained from the first review.
- Revised `CurrentProgramCheckpoint.md:3-14`.

No builds, tests, writes, implementation drafts, or current peer output were used.

The tuple was unchanged at both boundaries. Final trees were root `9db347741b6a7eb741a811c989a1b17d157ff203`, core `7e6c44d338158e5002c4b3ba13cef571dda80284`, and transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`. Repositories were clean; only the two permitted generated remediation prompts were untracked at root.

## 1. Prior-finding closure

| Original finding | Status | Closure evidence |
|---|---|---|
| P2-1 — trust-store limits lacked compatibility authority | **Closed** | Requirements G1 now expressly authorizes regular UTF-8/NUL-free stores capped at 4 MiB and physical lines capped at 16 KiB, for both placements. It records both stricter large-store refusal and broader complete-line admission, assigns `InvalidRequest`, preserves host/key/port matching, and requires native differential gates. The 5 MiB counterexample is now an explicit intentional refusal rather than an unlisted regression. |
| P2-2 — blanket no-retry language contradicted A2 | **Closed** | Production Setup lines 31-40 now prohibit whole-connection/address replay after negotiation while expressly preserving A2’s single enumeration, one ordered attempt per key, and advancement only on explicit `AUTHENTICATION_FAILED`. Lines 126-128 require both rejected-first/accepted-second success and proof that terminal failure does not move to another address. |

### [P3-1] The revised pre-DNS refusal guarantee conflicts with the document’s earlier operation order

`GwzRemoteTransportSshProductionSetup.md:11-14` lists the helper sequence as name resolution followed by known-host loading. Revised lines 61-62 now promise that trust size/encoding admission refuses before DNS, TCP, or credential access.

With an invalid UTF-8 or oversized trust store and an observable hostname, an implementation following lines 11-14 performs DNS before rejecting the file; one following lines 61-62 must load and admit trust before resolution. This does not reopen either original P2, but it leaves the N1 side-effect order ambiguous and can make the claimed no-DNS gate fail.

Choose one order explicitly. If the stronger guarantee is intended, reorder the earlier sequence to load/admit trust before name resolution and assert zero resolver calls for size/encoding rejection. Otherwise remove “DNS” from line 62 and align the remediation plan and evidence language with refusal before TCP/credentials.

## 2. Invariant analysis

The compatibility authority now agrees across the graph. G1 identifies exact limits, encoding, placements, expected native deltas, error class, and the preserved cryptographic matching boundary. The draft narrows “preserve” to host/key/port and credential-access policy instead of claiming exact trust-input parity. Its below/at/above and 4,091-byte native differential cases are satisfiable as design gates.

The retry contract is also coherent. Sequential resolved-address attempts remain available only before SSH negotiation. After negotiation begins, the connection is not replayed elsewhere, while A2 may still progress through its bounded identity list under its already accepted error-specific rule.

The unchanged ownership and deadline contracts still hold: non-preemptible DNS/file work stays on the capped helper; one original deadline spans setup; late completion cannot resume effects; retained helper, pool, and cleanup ownership remain charged; trust precedes authentication; and no pool, wire, or public API semantics change.

N2 snapshot/token bounds and lifecycle interfaces, N3 backend attachment, and platform/selected-source qualification remain expressly separate gates. The remediation does not silently claim any of them.

## 3. Risks and next action

This is a design re-verdict only. Native complete-line behavior, boundary cases, error mapping, resolver ordering, and A2 composition still require the specified N1 implementation evidence.

Correct P3-1’s operation-order wording, then the design is ready to authorize the bounded N1 implementation checkpoint.
