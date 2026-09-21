# SSH Production Setup Design — Consistency-Axis Review

**Review object:** `gwz-core` `b03d53c902a585da8dae6da316cdc499ba99bec1`, `dev-docs/GwzRemoteTransportSshProductionSetup.md`, DRAFT dated 2026-09-21  
**Baseline:** root `af0a12391fb2aa524652ca32d77c3632e3d06cee`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`. Sources were read from immutable objects with `git show`; retained source pins were used only to verify cited behavior.  
**Date:** 2026-09-21  
**Axis:** Consistency — internal agreement and agreement with the controlling requirements, transport design, and accepted SSH worker/A1/A2/A3 contracts. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 consistency defects block the design gate. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

I read the complete 123-line production-setup draft and checked it against:

- `GwzRemoteTransportRequirements.md` G1–G2, C1–C6, P6–P9, D3–D4, D9, D12, and D15.
- `GwzRemoteTransportDesign.md` §§6–8 and its identity/trust acceptance row.
- The accepted SSH worker boundary and production call-site map.
- `GwzRemoteTransportSshAgentDesign.md` §§2–3, 6–8 and accepted A1/A2/A3 ownership, cancellation, authentication, and cleanup contracts.
- `CurrentProgramCheckpoint.md:3-34`.
- Pinned libgit2 1.9.7 `ssh_libssh2.c:426-474,477-590,633-769`, which establishes the current single user `known_hosts` load, native host-key preference, and trust check behavior without the draft’s proposed 4 MiB/16 KiB admission limits.

No builds, tests, writes, network operations, implementation drafts, or current peer material were used.

The tuple was unchanged at both boundaries. Final trees were root `4a13f1846ccfac9829edacca6d700721ead7ce0c`, core `d1bd98c7e12579bb2e4e1e1cc90d3014c61535ce`, and transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`. The repositories were clean; only the two allowed generated production-setup prompts were untracked at root.

## 1. Findings

### [P2-1] New trust-store refusals lack authority under the controlling compatibility contract

`GwzRemoteTransportSshProductionSetup.md:40-52` says the implementation preserves current libgit2 trust policy, but then requires refusal of a `known_hosts` input over 4 MiB, any line over 16 KiB, and malformed or unsupported input. Requirements G1 (`GwzRemoteTransportRequirements.md:145-152`) requires preservation of current Git outcomes and gives an explicit list of intentional compatibility changes; stricter trust-store admission is absent. The transport design also requires native timeout and failure contracts to be retained (`GwzRemoteTransportDesign.md:575-577`).

A concrete counterexample is a syntactically valid 5 MiB regular `~/.ssh/known_hosts` whose matching target entry appears after many unrelated entries. Pinned libgit2 passes the file to `libssh2_knownhost_readfile` without either proposed cap and can accept the target; the draft mandates refusal before trust evaluation. Thus an existing successful SSH operation becomes a failure while the draft simultaneously claims preservation and cites no superseding authority.

The design must either preserve previously admissible trust files or explicitly amend the controlling compatibility rule to enumerate bounded trust-store admission as an intentional behavior change, including its error classification and affected local/driver endpoints. The closure evidence should characterize current success for an over-threshold valid file, then prove whichever revised compatibility disposition is selected at, below, and above each boundary.

### [P2-2] The blanket authentication-termination rule contradicts accepted A2 next-key behavior

`GwzRemoteTransportSshProductionSetup.md:31-35` prohibits “authentication retries” and says that once handshake begins, “a failure ends this setup attempt.” Accepted A2 is more specific and remains cited as authority: `GwzRemoteTransportSshAgentA2.md:10-15` enumerates agent identities once, attempts each key once in order, and advances to the next key only on explicit native `AUTHENTICATION_FAILED`. Its accepted evidence includes a rejected first key followed by successful authentication (`:48-53`). The production draft also requires composing N1 with A2 at lines 91-97 and does not declare this behavior superseded.

For an agent offering keys A then B where the server rejects A with the permitted explicit result and accepts B, A2 succeeds. Applying the draft literally ends setup after A. Preserving A2 instead violates the draft’s unqualified no-retry and post-handshake-failure rules. The contract is therefore not implementable consistently.

N1 must distinguish connection-attempt replay from A2’s bounded within-session identity progression: forbid moving to another resolved address or restarting the whole setup after SSH negotiation begins, while expressly retaining one ordered attempt per enumerated agent key and the accepted error-specific advancement rule. Closure requires an N1-path regression where A is explicitly rejected and B succeeds, plus a separate regression proving that handshake or terminal authentication failure does not try a second resolved address.

## 2. Invariant analysis

The non-preemptible OS-call admission model otherwise agrees with A1/A3: DNS and regular-file calls run only on the capped helper; timeout requests cancellation but does not claim kernel preemption; late ownership remains charged; overrun stops endpoint admission; and no replacement or detached resolver evades the two 64-owner bounds.

The whole-attempt timing rule is coherent. One original absolute deadline covers DNS, trust loading, connection, handshake, authentication, and publication; checks surround OS/native calls; zero disables only the network deadline; cleanup and cancellation remain active.

Trust-before-authentication also holds structurally. The draft loads endpoint-local trust before handshake preference selection, checks the logical hostname and effective port, validates the negotiated key before opening an agent or file key, and has A2 recheck the approved bytes. Pooled authenticated sessions correctly retain established trust until disposal.

The N1/N2/N3 dependency order is otherwise viable. N1 can extend the existing Setup Job without blocking the shared worker or changing pool/wire semantics. N2 explicitly reserves its snapshot/token bounds and pre-pool asynchronous admission interface for a separate checkpoint, while already fixing no-fallback and immutable-snapshot invariants. N3 remains the later backend attachment gate.

## 3. Risks and next action

Platform/selected-source qualification, N2 concrete interfaces and limits, and N3 backend attachment are legitimately deferred and are not findings. This review certifies no implementation or native capability.

Before N1 implementation begins, revise the design/control graph to authorize the trust-store compatibility disposition and disambiguate A2 identity progression from prohibited whole-attempt replay, then run the focused retained re-verdict on those two changes.
