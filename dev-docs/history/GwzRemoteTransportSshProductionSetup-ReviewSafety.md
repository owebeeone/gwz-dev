# SSH production setup design — Safety-axis review

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportSshProductionSetup.md` at `b03d53c902a585da8dae6da316cdc499ba99bec1`, DRAFT design, 2026-09-21  
**Baseline:** root `af0a12391fb2aa524652ca32d77c3632e3d06cee`; `gwz-core` `b03d53c902a585da8dae6da316cdc499ba99bec1`; `gwz-transport` `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`. Committed inputs were read with `git show`; the generated Safety prompt was read from the allowed untracked root file.  
**Date:** 2026-09-21  
**Axis:** What the design permits to go wrong under degraded paths, irreversible effects, compatibility constraints, and bounded failure. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 compatibility/trust-policy finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete canonical prompt and controlling production-setup draft; `GwzRemoteTransportRequirements.md` G1/G2 and P6–P9; `GwzRemoteTransportDesign.md` §§6–7, deadline clarification, acceptance matrix, and qualification boundaries; the accepted SSH worker and agent design/A2/A3 documents; `CurrentProgramCheckpoint.md`; and the directly interacting A2/A3 source contracts.

For native parity I traced the pinned path: core’s locked proof dependency is `libgit2-sys 0.18.8+1.9.7` with `libssh2-sys 0.3.3`; pinned libgit2 `ssh_libssh2.c:426-466` loads trust through `libssh2_knownhost_readfile`. The locked libssh2 source at `knownhost.c:851-989` uses a 4,092-byte `fgets` buffer and passes each resulting chunk separately to `libssh2_knownhost_readline`.

No builds, tests, network operations, or writes were performed. The tuple matched at start and end. Core and transport were clean; root contained only the two explicitly excluded generated prompts.

## 1. Findings

### [P2-1] The 16 KiB trust-line rule permits a native-parity expansion

**Location:** `GwzRemoteTransportSshProductionSetup.md:40-55`, especially the 16 KiB physical-line limit at lines 48-51; conflict with `GwzRemoteTransportRequirements.md:145-152` G1 and the draft’s own “preserve current libgit2 trust policy” claim.

The secure descriptor workflow necessarily reads and bounds the file itself, then feeds lines to native parsing. The draft permits a complete known-host line up to 16 KiB, while the current pinned path reads at most 4,091 characters per call and parses every chunk as a separate line.

A concrete counterexample is a valid target-host/key entry followed by a comment padded so the physical line is about 5 KiB. It is below the proposed 16 KiB limit. The current reader parses the first chunk, then treats the comment continuation as another host line without a key and returns `LIBSSH2_ERROR_KNOWN_HOSTS`; setup refuses. Feeding the complete bounded line to `libssh2_knownhost_readline` accepts the valid key and comment, allowing trust and authentication to proceed. The draft therefore permits an input that the current endpoint rejects, despite G1 and the stated native-parity rule.

This does not make the listed key cryptographically untrusted, but it changes an authentication boundary without identifying or authorizing that compatibility change.

**Required correction:** explicitly select one policy:

- preserve pinned behavior by specifying equivalent physical-line/chunk admission; or
- declare the broader parser behavior as an intentional G1 exception and obtain the required authority for that change.

The N1 gate must include differential fixtures around the pinned boundary: valid target entries with padded comments and host lists below, at, and above 4,091 characters, through both the baseline native reader and the new descriptor reader. Unapproved deltas must refuse before TCP, agent access, or file-key offer. The 4 MiB total-file refusal should likewise be recorded as an intentional bounded compatibility limit rather than implied to be exact native parity.

## 2. Invariant analysis

The remaining attacks held.

Non-preemptible DNS and regular-file calls stay inside the existing supervised helper ownership. Logical timeout suppresses subsequent effects but does not pretend to cancel kernel work; helper, pool charge, and cleanup reservation remain owned until join and destruction. The separate 64-helper and 64-endpoint-cleanup caps prevent recreation or replacement from making retention unbounded, and permanent slot loss is explicitly accepted as a fail-closed availability cost.

One original absolute deadline covers DNS, trust loading, TCP, handshake, trust validation, and authentication. Checks occur before and after OS/native calls and before credential access or publication. Address fallback is allowed only before SSH negotiation, preventing a failed negotiated peer from silently redirecting authentication elsewhere.

Trust uses the logical host and effective port, precedes agent or file-key access, rejects missing/unknown/mismatched input, and preserves known-host-aware algorithm selection. The opened trust file must be regular, bounded, and parsed with cancellation checks; the approved host-key bytes are rechecked by A2 before agent I/O.

Explicit identity admission is correctly separated into N2: it precedes pool lookup, uses one immutable validated snapshot for compatibility and authentication, prohibits pathname reopen and ambient fallback, keeps paths/private bytes out of proofs and observations, and requires bounded endpoint-local token ownership before implementation.

The N1→N2→N3 order prevents local N1 work from activating an incomplete credential or backend path. Platform/source qualification remains an explicit prerequisite to capability activation.

## 3. Risks and next action

This design review does not certify N1 implementation, platform behavior, selected sources, explicit-key interfaces, backend attachment, or production activation.

Amend the known-host line policy and its N1 differential gate, then obtain a focused Safety re-verdict on that changed boundary.
