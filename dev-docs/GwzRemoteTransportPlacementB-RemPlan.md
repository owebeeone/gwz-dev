# Placement B — consolidated correction 1

Status: correction implemented and focused gates passed; reviewer closure pending. Initial tuple root ba32b3af,
core 6b9be8a2. Code two P2; State two P2; Surface GO. Code P2-1 and State
P2-1 independently converge on the same fetch preflight root (three roots total).

| Finding | Disposition | Closure |
|---|---|---|
| Code P2-1 / State P2-1 | Accept: validate every contactable fetch target's effective identity before dry-run/fan-out using the existing cached backend selection. | Ordinary serialized two-target CLI fetch, valid first and missing last repo-configured key, upstream advancement: no opens and unchanged tracking refs. |
| Code P2-2 | Accept: nonblocking regular-file admission; timeout/cancel publishes one logical terminal independently of retained physical disposal. | FIFO without writer rejects promptly; injected blocked check produces one timeout and remains charged until released. |
| State P2-2 | Accept: endpoint policy checks before queue/arithmetic; checked deadline derivation. | Maximum admitted integer fields through bound session fail without SSH effects, release waiter, and preserve subsequent processing. |

One patch and settled aggregate checkpoint, then retained Code/State focused
re-verdicts and unchanged Surface same-tuple confirmation. No API/schema change,
carrier construction, production activation, platform/source qualification or push.
