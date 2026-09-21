# SSH Production Setup Design Remediation 1 — Safety Review

**Axis:** Safety  
**Date:** 2026-09-21  
**Verdict:** **GO**  
**Findings:** None open

## 0. Reviewed object and evidence

Reviewed the committed remediation at:

- Root: `eadf8dc25f93b3f8d9d9c4f3660732367861559f`
- `gwz-core`: `9acf508aefe4ef974e52f19016f33ecf4bf56b34`
- `gwz-transport`: `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`

The review covered the merged remediation plan, the changed range from core `b03d53c902a585da8dae6da316cdc499ba99bec1` to `9acf508aefe4ef974e52f19016f33ecf4bf56b34`, and the revised production-setup design and G1 requirements. The original Safety report was used only to retrace its finding. No current-round peer prompt or report was read.

This was a read-only documentation re-review. No builds, tests, implementation acceptance, or platform qualification are claimed.

## 1. Prior-finding closure

| Finding | Original counterexample | Revised contract and required evidence | Status |
|---|---|---|---|
| **Safety P2-1 — complete-line parser contradicted native compatibility** | A valid known-host entry padded with a roughly 5 KiB comment is below the former proposed 16 KiB limit. The pinned native reader consumes at most 4,091 bytes per `fgets` call and parses each chunk independently, so it can accept the first chunk and then reject the comment continuation as a malformed line. A complete-physical-line reader can accept the same input. The original design called this native-preserving behavior without authorizing the difference. | Requirements G1, lines 151–161, now explicitly authorizes complete physical-line parsing for regular UTF-8, NUL-free stores bounded to 4 MiB and lines bounded to 16 KiB excluding CR/LF. It identifies both directions of change: oversized stores may newly refuse, while valid 4,092–16,384-byte lines may newly succeed. The Production Setup design, lines 52–64, applies the exception to both local-core and driver-hosted endpoints while preserving native host/key/port matching and fail-closed malformed-input handling. Its N1 gate, lines 122–128, requires padded-comment and host-list fixtures below, at, and above 4,091 bytes through both readers, with each permitted delta labelled and every unapproved delta refusing before TCP or credential access. | **CLOSED** |

The original counterexample is now an expressly authorized G1 compatibility exception rather than an accidental trust-boundary change.

## 2. Changed-range Safety analysis

The remediation places concrete boundaries around the admitted difference:

- The representation expansion is limited to valid complete physical lines no longer than 16 KiB. It does not broaden the hostname, effective-port, key, hashing, or trust-match rules.
- The 4 MiB store bound, 16 KiB line bound, UTF-8 requirement, and NUL prohibition are normative for both endpoint placements.
- Size and encoding failures map through `InvalidInput` to `InvalidRequest` before DNS, TCP, agent access, or file-key offering. The specific admission rule therefore governs the earlier general setup sequence.
- Native malformed content remains rejected. There is no alternate permissive parser, interactive prompt, automatic insertion, SSH-config interpretation, or fallback to an untrusted host.
- The differential suite must expose the 4,091-byte native chunk boundary rather than hide it behind a preservation claim. It also covers the newly restrictive store and line limits and requires side-effect-free refusal for unapproved differences.

The adjacent retry clarification preserves the accepted A2 state machine: identities are enumerated once and attempted once in order within one negotiated session; only explicit `AUTHENTICATION_FAILED` advances to the next key. Handshake or terminal authentication failure cannot restart the connection or try another resolved address. This does not introduce a credential replay path.

The broader safety invariants remain intact: bounded resolver output, cancellable supervised work, trust before credential use, immutable N2 setup input, no ambient credential fallback, and no activation before the deferred platform/source qualification.

## 3. Residual gates

This verdict accepts the corrected design contract only. N1 still must implement and demonstrate the required native differential fixtures, exact bounds, error mapping, and no-effects behavior. N2 must settle the concrete immutable setup interface, and N3 must prove backend attachment and observation propagation. The deferred platform/source batch remains mandatory before capability activation.

The tuple was rechecked at the end of review and remained exactly as listed above. Root contained only the two allowed untracked remediation-round prompt files; core and transport were clean.

**Final Safety verdict: GO for the remediated production-setup design.**
