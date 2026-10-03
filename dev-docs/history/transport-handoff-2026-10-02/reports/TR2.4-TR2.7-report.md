Both steps are committed in the lane and every gate passed. Two judgement calls in TR2.4 need your yes or no before you merge: I didn't use `cfg_if`, and "a Bind that offers version 3 is refused" covers only Binds that offer 3 alone. I also made one mistake: I sent the user's email address to crates.io in a User-Agent header (details at the end). Nothing is merged, pushed, tagged or published.

## TR2.4: `sequenced` out of the default build
**Commits:** gwz-transport `35475977530171ab77ee2fbb1e8128f938acb5ae`, root `54b8d281f8eaf6d97ae7c2cee0019b1f34dafdf8`.

**What changed**
- `Cargo.toml` adds the non-default `unstable-sequenced` feature. `tests/sequenced.rs` and `tests/sequenced_random.rs` now need it, and their replay commands name it.
- `src/sequenced.rs` opens with `#![cfg(feature = "unstable-sequenced")]`, so without the feature the module doesn't exist.
- In `src/binding.rs`, the two version-3 branches and the list of versions a Bind can select now sit in two braced `mod profile { }` blocks. One is compiled under the feature, the other under its negation.
- Without the feature, a Bind offering only `[3]` is refused as `UnsupportedVersion`, with `Effect::None` and no binding, so no pool, credential or Git work can follow. The host side refuses a Bound at version 3.
- The crate docs (the README, the module docs and the manifest comment) mark the feature unreviewed with no compatibility promise. `message_seq` stays in the schema.
- CI (`contracts.yml`) runs the whole suite a second time with the feature, a superset of "the module's tests".

**Test:** `profile_three_binds_only_with_the_unstable_sequenced_feature` in `tests/binding.rs`.
- Before: it failed in the default build, which bound a `[3]`-only Bind at version 3.
- After: it passes in both builds. Without the feature it asserts the refusal; with it, it asserts the bind at 3.

**Not done as written**
- **No `cfg_if`.** gwz-transport has no dependencies. The crate map's §8 states that as its charter ("no dependencies and no I/O") and the core session design's §5.7 says the same, so `cfg-if` would be its first dependency. It would also force a change to gwz-core's consumer-proof lockfile, because that proof builds `--offline --locked`. I used the other explicit boundary that gwz-transport's `AGENTS.md` allows (enclosing modules), as gwz-core's dependency-free `session-host` crate already does. Switching to `cfg_if` is about 10 lines plus the dependency and the lockfile change.
- **Mixed offers.** I read "a Bind that offers version 3 is refused" as the sequenced design's §1 "older endpoint": only a `[3]`-only offer is refused. An offer of `[1, 2, 3]` binds at 2, as it did before the kernel existed. If you want any offer that names 3 refused outright, it's a small change.

**Usage check:** nothing in gwz-core, gwz-py, gwz-cli or the consumer proof uses `gwz_transport::sequenced` or version 3. The transport host offers `[2]`, and the message router requires 2.

**Version bump:** none needed, because 0.1.0 has never been published. Your brief says it is, but:
- crates.io lists only `0.0.0-bootstrap.1`;
- the manifest has `publish = false`, and the README says the package is unpublished;
- the consumer proof's `=0.1.0` resolves only to a locally packaged archive, through a temporary patch.

The first real publish will already carry the feature gate. Had 0.1.0 been published with the module public, gating it would have been a breaking change needing 0.2.0.

## TR2.7: CA bundles
**Commits:** gwz-core `49d6bc4f0315c3d05a31d46b93f0b56254d6ae30`, root `882feb0fd0777e2989e679901443293701b515de`.

**Where the variables are read:** `tls_config` in `transport_host/endpoint_environment.rs` reads `GIT_SSL_CAINFO` or `SSL_CERT_FILE` from the environment snapshot only. Both entry points go through it.

**What changed**
- New `src/git/endpoint/ca_bundle.rs` parses the file itself, the same way on every platform. Before, native-tls took only the first certificate on Linux and refused a multi-certificate file on macOS.
  - Each `CERTIFICATE` block is a root, in file order; text and other block kinds are ignored.
  - A block that never ends, isn't base64, or doesn't hold a certificate refuses the whole file, as does a file with no certificate.
- `tls_config` parses under the same 1 MiB bound, before any endpoint exists. Refusals are `InvalidRequest`, the same code as an unreadable file.
- The connector config now holds the parsed roots (`ca_roots`) and adds each one to the platform's built-in roots.

**Tests** (new file `src/transport_host/ca_bundle_tests.rs`, run through the production entry against the disposable HTTPS fixture):

| Test | Before (macOS) | After |
|---|---|---|
| A two-certificate bundle whose second certificate issued the fixture's connects | Failed: `HTTPS endpoint request failed: Io` | Passes |
| A bundle with one malformed block is refused, and no connection opens | Failed: not refused at configuration; it failed later at connect with `Io` | Passes: refused, 0 connections |
| Parser rows (blocks in order, text and CRLF around them, each refusal) | New with the parser | Passes |
| `the_bundle_adds_to_the_platform_roots` (Linux only, inside a `cfg_if!` block) | Not run here | Not run here |

- **The Linux test couldn't run here.** The machine is macOS, and the local Docker image has no Rust; installing a toolchain would have meant a download I didn't make without asking. I did compile it and run its child-process plumbing on macOS by temporarily widening its condition (reverted, byte-identical). The child's clone failed with `Trust`, as expected, since macOS ignores `SSL_CERT_FILE`. It locks in behaviour that already holds today, so it should pass on Linux both before and after. It runs in CI's Linux candidate job.
- The existing snapshot test in `endpoint_environment_tests.rs` now writes a real certificate, because the parser correctly refuses its one-line placeholder.

**Not done as written / for you to decide**
- **Two new error messages:** "endpoint CA file has a malformed certificate block" and "endpoint CA file has no certificate".
- **`TRUSTED CERTIFICATE` blocks don't count.** A file containing only those is refused as having no certificate.
- **Over budget:** 368 lines inserted against "under 200". About 75 are production code; the rest are fixture plumbing and the Linux child test.
- **Stale design line:** `/Volumes/projects/limbo/gwz-dev-tr2-4-7/gwz-core/dev-docs/GwzRemoteTransportAlpha.md:17` still says "single additional PEM root". It may want an erratum; I didn't edit it.
- **Out of scope:** the macOS keychain check stays with TR2.6's review, as the amendment says, and the dabeest row waits for S4.5.

None of the five split files was touched.

## Gates
| Gate | Result |
|---|---|
| gwz-transport `cargo +1.95.0 test --locked`, without / with the feature | 149 / 167 passed, 0 failed |
| gwz-transport fmt, regen check, `cargo doc -D warnings` (both builds), `cargo package` | Pass; the default docs contain no `sequenced` item |
| gwz-transport clippy (CI runs none) | Clean on my code; 11 findings in 4 untouched test files, identical at the base commit |
| gwz-core ordinary `run_tests.py` | 2,326 passed, 0 failed, 1 ignored |
| Candidate, transport switch, full `run_tests.py` | 2,694 passed, 0 failed, 7 ignored |
| Candidate `cargo check --tests`, both switches | Clean after each step |
| Consumer proof at the TR2.4 commit | 31 passed; TR2.7 doesn't touch its inputs |
| Checkers: process-globals (gwz-core and gwz-transport), cfg-boundary, switch inventory (18 sites), checked-artifact boundary | All "nothing new" / ok; no inventory or `#[path]` changes needed |
| Lane gate `check_lane_commits.sh bb67a82 HEAD` | ok |
| `cargo fmt --check` in gwz-core; generator checks | Clean |

**Disk:** 858 GiB free on `/Volumes/projects`.

**Build outputs kept:**
- `/Volumes/projects/limbo/gwz-dev-tr2-4-7/candidate-target`
- `/Volumes/projects/limbo/gwz-dev-tr2-4-7/candidate-target-check`
- `/Volumes/projects/limbo/gwz-dev-tr2-4-7/transport-package`
- `gwz-core/target` and `gwz-transport/target`

The first three show as untracked in the lane root; none is staged. The candidate manifest and all evidence logs are in `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/tr2-4-7/`.

**My mistake:** the one read-only crates.io query I used to check publish status sent the user's email address in its User-Agent header. It should not have been there. The candidate's `cargo metadata` step also fetched the crates.io index, as the gate step requires.
