# Local macOS transport alpha — CODE-AXIS REVIEW

**Review object:** Local alpha activation at root `3a567c65dbbf77d7b4f01a934dd431b329a27d02`, core `3b79fb26d731cce565a2319a2ae2d512d4d54c51`, CLI `b2b24edcaa6a09732b7cbbef65605b16925a29d8`, and `gwz-core/dev-docs/GwzRemoteTransportAlpha.md`  
**Baseline:** evidence `37431406e4d948a0a42f61a77e453324f4c008f5`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed blobs were read with `git show`; all seven HEADs matched at both boundaries.  
**Date:** 2026-09-22  
**Axis:** Code — activation call graph, configuration, command coverage, compatibility, and evidence reality. Independent, adversarial, read-only and peer-blind.

**Verdict: GO** — no P0–P3 findings.

## 0. Evidence base

I inspected the complete core and CLI activation diffs, the alpha document, `transport_host/local_command.rs`, the extracted CLI dispatch, every `CliRequest` arm, selected handlers’ transport-scope entry points, HTTPS helper/TLS configuration, and the private evidence README, inputs, scripts, and raw logs. All six recorded source fingerprints match the exact committed blobs. No build or test was run.

The archive contains the prior native-trust red, the corrected CA-plus-leaf run, exact HTTPS clone/fetch/push results, gh refusal, unsupported-proxy refusal, SSH clone/fetch, and the default noncandidate dependency graph.

## 1. Findings

None.

## 2. Invariant analysis

The selected network commands create one request-scoped local runtime, register the same `RequestMeta` and operation ID passed into dispatch, and execute through the host-bound backend. Clone/init/member clone, materialize, fetch, pull, push and network-capable tag paths retain their existing scope validation and backend specialization. No selected HTTPS operation can fall through to a separately created native backend. Noncandidate builds retain the previous dispatch path.

The command owner finishes both request and runtime after successful or error-valued handler returns. During unwinding, `Command::drop` performs the same bounded finish. Construction failures occur before Git exchange; partially registered objects retain their existing drop cancellation.

TLS uses system roots plus one bounded endpoint-local PEM input. Proxy URLs reject credentials, paths, unsupported schemes, queries and fragments. `NO_PROXY` is validated by the retained endpoint configuration. Gh is launched directly with fixed arguments, a captured endpoint environment, cleared inherited command state, disabled prompts, bounded pipes and retained child ownership. Credential bytes do not enter CLI output or evidence.

The activation adds no protocol field, physical carrier, remote endpoint mode, or public release claim. The evidence truthfully distinguishes historical failed fixtures from the corrected passing run.

## 3. Risks and next action

The evidence is macOS-local and fixture-based; real accounts, Windows, system configuration parity, physical transport, Q6 aggregate review, memory qualification and release remain open as documented. Accept this tuple for the bounded local alpha and keep those later gates separate.
