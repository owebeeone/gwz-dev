# GWZ Connection: communication and reuse remediation

Date: 2026-10-19. Status: **accepted as a remediation plan only at root
`468975076f94cd761aa07ba4b47c2c4af797a892` after
[Consistency](GwzConnectionRemediationPlan-ReviewConsistency.md) and
[Safety](GwzConnectionRemediationPlan-ReviewSafety.md) reported GO.**
No replacement API, repository, source implementation or release is approved by
this plan acceptance. See the [verdict](GwzConnectionRemediationPlan-Verdict.md).

This is one of four plans under the
[remediation process](GwzTransportReleaseRemediationPlan.md). Its companions are
the [Git boundary](GwzGitBoundaryRemediationPlan.md),
[anti-pattern repair and prevention](GwzAntiPatternRemediationPlan.md), and
[release convergence](GwzTransportReleaseConvergencePlan.md) plans.

## 1. Outcome and authorized scope

Create a small, independently testable, Git-independent communication boundary
that makes persistent connection ownership explicit. The working name is
**GWZ Connection**, with a proposed principal owner named `ConnectionHost`.
Names here describe responsibilities, not a settled Rust signature.

The operator requested SSH and HTTP/HTTPS, reusable connections, authentication
interfaces, platform-independent contract tests, and structural prevention of
the ownership mistakes found in the audit. Merely moving endpoint files or
retaining today's whole application runtime is insufficient.

This draft may propose changes to gwz-core endpoint/transport-host composition,
gwz-transport pool ownership, gwz-git integration, and CLI/Python callers. Native
adapters may involve gwz-sspi. Implementation requires an approved boundary and
explicit authorization. git2-rs, libgit2, Rust standard-library sources and other
dependencies are reference-only until a demonstrated blocker is separately
authorized. No dependency upgrade, repository creation, new carrier, daemon or
whole-session migration is a prerequisite established by this plan.

## 2. Baseline and failure being repaired

The read-only audit covered core `35df1cbb`, Python `271d73c3` and later
documentation-only Python `4ec4f24`, transport `a27817b9`, and Git `a9d7ee09`.
The subsequent archive commits changed documents, not these implementation facts.

- `gwz-py/native/src/route/transport.rs` explicitly gives each network operation
  its own runtime. `gwz-core/src/transport_host/cancellable.rs` owns its shutdown.
  A client-owned native authentication host is not a client-owned transport pool.
- `gwz-core/src/transport_host/driver_tests.rs` contains reuse tests, but an
  internal test retaining a runtime cannot establish real Python caller reuse.
- Existing connection reuse design separates endpoint instances and operation
  bindings. Reuse that design's applicable constraints; reconcile its amendments
  rather than treating its entire later session programme as a prerequisite.
- A long-lived owner must also solve bounded admission, result retention and
  identifier turnover. Keeping a finite operation runtime alive indefinitely
  without those policies is not a valid implementation.

## 3. Required responsibility boundaries

| Component | Owns | Must not own |
| --- | --- | --- |
| gwz-transport | Discrete stream protocol, ordering, credit/backpressure, stream/mux/pool state machines | Git commands, application calls, OS authentication policy |
| GWZ Connection | Reusable endpoint resources, operation bindings, connection eligibility, admission, fault scope and cleanup coordination | Workspace membership, Git refs, application result registries |
| Authentication providers/native adapters | Explicit credential acquisition, trust checks and native mechanism execution under a stated lifetime | Unbounded ambient credential discovery or application operation ownership |
| gwz-git | Single-repository Git behavior and libgit2/binding adaptation, consuming communication ports | Workspace fan-out or independent duplicate connection hosts |
| gwz-core | Workspace coordination, request/result projection, composition and policy supplied through explicit ports | Reimplementing the communication owner or leaking Git binding details through the new boundary |
| CLI / Python | Owning the host at the intended application lifetime and issuing operations | Reconstructing pools per operation on the supported persistent path |

Keep the existing gwz-transport charter: it has no physical wire transport and
does no socket or subprocess I/O. Network adapters belong outside that pure kernel.
The proposed dependency direction is core -> gwz-git -> communication API ->
gwz-transport, with adapter interfaces arranged to avoid a dependency back into
core. This is a target to validate, not permission to create those Cargo edges.

Git-specific upload-pack/receive-pack selection, HTTP Git service semantics,
repository routing and Git errors belong in gwz-git. Generic channel opening,
HTTP connection/authentication mechanics and reuse must be separable from them.
Decide which layer owns each part of HTTP before moving any implementation.

## 4. Required API properties

### Ownership and operation isolation

1. Construct the persistent owner explicitly. Supported operation entry points
   require an owner or a capability issued by it; no successful constructor
   silently creates a private pool that a later builder call must repair.
2. Define process-safety resources, host, operation, connection and stream scopes
   independently. There is exactly one authoritative owner for each resource.
3. Finishing or cancelling one operation closes its bindings and releases its
   leases; it does not shut down the shared host or another operation.
4. Only host shutdown revokes all operations. State when shutdown is complete,
   what cleanup remains pending, and who retains capacity until cleanup is known.
5. Give every admission/registration outcome explicit ownership provenance.
   Reuse existing settled registration rules where they apply; do not recreate
   the earlier ambiguity around cancellation after insertion.

### Reuse, identity and configuration

6. Match connections by an explicit compatibility decision. The user-facing SSH
   identity remains username plus host; effective port, endpoint and security
   inputs may partition reuse without inventing per-repository identities.
7. Distinguish transport compatibility, authenticated identity, trust settings,
   proxy route and operation-local configuration. Freeze each operation's inputs;
   no subordinate constructor silently recaptures a different environment.
8. Set finite per-endpoint capacity and idle reaping. The original proposed idle
   default was 60 seconds; reconcile current accepted timeout clauses before
   freezing a value. Idle means not allocated to a stream, with precise activity
   and cleanup accounting defined in the API contract.
9. Describe credential/configuration changes explicitly. An idle timeout does
   not bound credential revocation on a continuously busy authenticated connection.
   Use the agreed SSH/client semantics; do not invent stronger revocation promises.
10. EOF, reset, authentication expiry and partially failed conversations must
    invalidate the correct lease. Never return an uncertain connection as healthy.

### Messages, deadlines and bounded resources

11. Expose asynchronous discrete message delivery and file-like stream adapters.
    Ordering and terminal/stale-event behavior belong in the virtual-stream
    protocol, not in accidental host scheduling order.
12. Transport delivery remains independent of the application request/response
    lifecycle. Python must service transport messages while calls wait or cancel.
13. Prove embedding in current in-process core/CLI/Python message paths. A future
    byte carrier or iroh integration remains an extension point, not a new task.
14. A deadline is fixed once established; authentication rounds and retries do
    not extend it. Name separate setup, operation, idle and cleanup bounds and
    resolve timeout-zero behavior against the accepted contracts.
15. Bound active work, waiting admissions, helper/native work, terminal records
    and buffered bytes. Queuing an OS thread before admission is not a bound.
16. Define generation/identifier retirement without dropping stale-message
    protection. Test substantially beyond the current runtime's lifetime-ID bound.
17. Distinguish bytes accepted, delivered and acknowledged from Git operation
    success. Transport must not infer that a Git push succeeded or safely retry
    an ambiguous mutation from stream accounting alone.

## 5. Authentication and secret-handling decisions

The communication design must name explicit ports for credential sources, SSH
agent/key signing, certificate/trust validation, proxy credentials and native
Windows authentication. Reuse gwz-sspi's contained worker; do not reimplement SSPI
in this library. Authentication results must state mechanism and identity with
the existing disclosure restrictions.

Evaluate a separate authentication package only if concrete interfaces and
consumers justify it. Start from existing adapters rather than creating a generic
credential framework. The user's early HTTPS restriction was gh-only; later
accepted helper/native decisions expanded specific cases. Reconcile that decision
trail and preserve the approved matrix, rather than treating this draft as new
permission for arbitrary credential sources.

The existing core crate-map rules keep GWZ application encoding and environment
snapshots in core, with named endpoint and SSPI exceptions for secret material.
A Git-independent library cannot simply import core's snapshot or secret types.
Before review, list the minimum opaque capabilities/owned inputs each adapter
needs and the exact placement rules that need amendment. No new secret placement
exception is assumed by this plan. Redaction, zeroization where applicable,
credential helper execution and diagnostic exposure need explicit owners.

## 6. Repository and API decision package

Compare these options using the same public contract and tests:

| Placement | Benefit | Question to resolve |
| --- | --- | --- |
| Separate crate in gwz-transport repository | Near the pure kernel; independent crate tests | Preserve the no-I/O kernel and prevent dependencies on core crates |
| Separate gwz-connection repository | Explicit consumer/release boundary | Justify another publication/pinning lifecycle and native adapter placement |
| Core-hosted independent crate | Easier atomic migration with existing adapters | Prove it builds/tests without core and does not remain an unused extraction |

The recommendation must show package dependencies, native adapter locations,
secret ownership, test commands and the migration of actual consumers. No option
is selected merely because the working name sounds like a repository.

Settle a small API sketch covering host construction, operation admission,
registration, stream acquisition, lease disposition, cancellation, observation
and shutdown. Include an invariant table identifying type-enforced properties,
test-detected properties and remaining caller obligations. Resolve the process
spawn hazard separately; a per-host gate cannot claim process-wide exclusion.

## 7. Implementation sequence after review and authorization

1. **Contract and placement:** freeze the decisions above, affected authoritative
   clauses and the cross-repository implementation boundary. Review consistency,
   lifecycle safety, secret handling, consumer fit and requirement provenance.
2. **Small proof:** one explicit owner, deterministic fake connector/auth provider,
   two independent operations and one reusable connection. No whole-session host.
3. **Failure and sustained proof:** overlap/cancellation, failed connection,
   incompatible inputs, bounded waiting/results/generations and truthful shutdown.
4. **Native adapters:** adapt existing SSH/HTTPS mechanics behind the settled
   ports; qualify the affected primitives independently on macOS/Linux/Windows.
5. **Actual consumers:** gwz-git and core use the same API; a long-lived Python
   client and CLI workspace operations pass the release scenarios. Remove replaced
   owner paths only after equivalence; coordinate file splits between packages.

Review complete slices, not every mechanical file move. If the proof requires
the full future session programme, report the exact failed assumption and smallest
alternative before broadening scope.

## 8. Acceptance and handoff

- Fast tests require neither a Git repository, network, helper process nor core.
- Seeded Monte Carlo tests vary write/read chunk sizes, message interleavings,
  credit, cancellation and terminal delivery. Log seed/configuration/trace for
  exact reproduction; respect the documented delivery/duplication contract.
- Tests prove two operations reuse, cancellation isolates, credential/config
  partitions are honored, stale traffic is rejected and resources stay bounded.
- Native tests prove adapter/platform claims; real-client tests prove composition.
  Neither replaces the other. Measure actual connection creation, not just a hit counter.
- Completion requires production consumers and independent tests, not an unused
  published library. Release qualification follows the convergence plan.

**Next handoff:** draft contract, placement comparison, explicit amendments and
small-proof test specification ready for review. No source work has begun under
this plan.
