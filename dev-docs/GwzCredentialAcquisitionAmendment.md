# Credential acquisition boundary amendment

Date: 2026-10-19. Status: **DRAFT for joint Code/State review; not accepted and
no production implementation is authorized before that review passes.**

## 1. Requirements reconciliation and authorized scope

The operator requested a first real explicit-key provider, independent cheap
provider/platform tests, and public hosting. After a controlled-stall source
probe exposed the synchronous-admission gap, the operator explicitly directed:
"So fix the acquisition boundary" and "It appears we've underspecified the
requirements." API size is an implementation choice, not a requirement to
preserve an underspecified signature. This amendment reconciles existing intent
before selecting the corrected lifecycle.

| Requirement | Provenance and status |
| --- | --- |
| Original caller/policy capture at the executing endpoint before fan-out/detach | Existing Credential §§4–5 and boundary §3; unchanged |
| Explicit file snapshot admitted before every pool lookup, exact bytes rather than path; no agent fallback | Existing Connection §7.1 and Credential §7.2 steps 1/5; unchanged, previously unrepresented in the portable API |
| Source acquisition may block; filesystem reads cannot run inline in prompt-return selection/polls | Existing provider prompt-return/retention obligations plus executed source_admission probe; now explicit |
| Admission before effects; absolute deadline/cancel; independent retained progress/cleanup after caller loss | Existing Credential §5/boundary §6; acquisition coverage now explicit |
| Source observation before a physical peer exists must not invent peer/generation facts | Derived from source-before-lookup timing and existing verified-peer contract; now explicit |
| Helper/native acquisition must preserve verified-origin/challenge-dependent timing | Existing provider/HTTPS policy and trust-before-secrets; this amendment does not force every source into eager prelookup acquisition |
| Selection after successful acquisition is synchronous, pure and immutable | Newly explicit boundary chosen to satisfy the preceding requirements |
| Authentication remains bound to actual peer/generation; reuse requires its existing independent authorization/trust proof | Existing Credential §§4.1/5 and Connection §7.1; unchanged |
| No owner-replacement capacity bypass, detached job or unobserved stalled source | Existing shared-retainer/native-owner rules; now exercised by source-provider traces |

Authorized edits: this scoped amendment and exact acquisition clauses in the
three accepted root API/boundary documents; gwz-credential API, implementation,
first explicit-file source, focused tests/CI and member docs; GWZ-managed public
remote metadata and scoped private native logs. Connection/core/CLI/Python/Git/
transport/SSPI sources and existing native supervisors remain reference-only.
No dependency upgrade, new general supervisor/process mechanism, signing system,
wire message, pool, placement change or broader application migration is included.

## 2. Concrete failure and smallest-route assessment

At gwz-credential `f6df6be69d54927b3eed6140aa59f92bb2ab13a4`,
`tests/source_admission.rs` holds a source read behind a barrier inside current
Provider::select. Closing owner/operation and progressing the retainer returns
promptly to the closing callers, but cannot progress the selecting thread: its
selection remains charged and there is no registered retained work or source
observation. This deliberately nonconforming adapter demonstrates why a blocking
source cannot be put behind the current synchronous select contract. It is not
a claim that the generic kernel violates its conforming-provider contract.

Core's existing ssh_key_snapshot::Registry::start reserves before its supervised
Job reads; intern publishes only after joined admission and a live check.
regular_file::open rejects special files. This is concrete evidence for bounded
source ownership, not permission to import core or replace its supervisor.

Putting acquisition in existing auth begin/start would reuse retention, but
would read after a possible pool lookup and requires physical PeerContext before
source identity exists. It fails explicit-key timing. Preloading in a provider
constructor bypasses per-request controls/admission. Neither is accepted.
Internal registration/progress/accounting mechanisms may be shared, but source
and authentication observations and publication stages remain distinct.

## 3. Corrected lifecycle and public surface

Capture, acquisition, immutable selection, peer authentication and reusable scope
are separate stages. Capture performs no credential-source I/O. Acquisition
creates one bounded immutable source snapshot/capability for one prospective
selection. Selection consumes that acquired result; it never reads a path,
consults an agent/helper or starts native work. Authentication begins only with
real verified peer facts. An accepted connection-bound ScopeLease remains the
existing separate stage and never substitutes for fresh explicit-key acquisition.

```rust,ignore
impl CredentialContext {
    pub fn acquire(&self, selection: ExchangeSelection,
                   purpose: AcquisitionPurpose, control: AuthControl)
        -> Result<CredentialAcquiring, AuthError>;
    pub fn select(&self, acquired: AcquiredCredential)
        -> Result<PreparedCredential, SelectError>;
}
impl CredentialAcquiring {
    pub fn observation(&self) -> AcquisitionObservation;
    pub async fn wait(self) -> Result<AcquiredCredential, AuthError>;
    pub fn cancel(self) -> AcquisitionClosing;
}
impl PreparedCredential {
    pub fn source_identity(&self) -> SourceIdentity;
    pub fn begin(self, peer: PeerContext, control: AuthControl) -> AuthOpening;
}
impl SourceIdentity {
    pub fn same_material(&self, other: &SourceIdentity) -> bool;
}
```

`AcquisitionPurpose` is source-specific: `BeforeLookup` has no peer; a
`VerifiedRequest` carries actual verified PeerContext and bounded offer/challenge
when the existing provider policy requires those facts before source access.
Explicit file keys require BeforeLookup. Helper/native providers retain their
existing origin/challenge/caller prerequisites; BeforeLookup is not a generic
permission to contact them early. A source that can only return a captured
capability without I/O may complete acquisition immediately, still under the
same bounds and publication checks. This chunk implements only explicit files;
other providers require their own conformance qualification.

`acquire` validates captured policy, endpoint/caller, purpose and live controls,
reserves both provider-declared snapshot/work maxima and one retained record,
and registers source observation BEFORE any source effect. Admission failure
returns synchronously with no effect. It returns promptly after registration;
only the independently owned retainer may start/progress source work. No physical
connection is reserved and no authentication is started by acquisition.

The same original caller and captured endpoint base resolve native paths; no
worker recapture, process cwd lookup, lossy UTF-8 conversion, endpoint switch or
fallback is permitted. Relative explicit paths are resolved against a bounded
explicit endpoint base carried in captured input; absolute native paths are
preserved. This is necessary operation input missing from the portable form,
not permission to import the application's environment snapshot.

AcquiredCredential is unique/non-Clone and contains immutable selection and
provider material plus its charged ownership. `select` checks that it belongs
to this exact live provider/operation, consumes it and produces one independent
PreparedCredential. It performs no I/O and no native authentication. A denied
selection drops/zeroizes its material once all legitimate pins disappear.
The acquisition control's deadline governs acquisition through result transfer;
begin receives the original exchange's still-current absolute deadline, never a
restarted duration. A snapshot does not extend an expired request.

SourceIdentity is an opaque endpoint-local equality capability, not a secret
getter, authorization receipt or reusable auth scope. It retains the same charged
snapshot, never a duplicate key buffer. For explicit keys same_material compares
EXACT admitted bytes and required endpoint/target/source-kind/provider lineage;
it does not compare paths, normalized key encodings or collision-prone hints.
Changing path to equal bytes may remain compatible under existing policy; changed
bytes do not. Full original-caller/policy, possession and fresh host trust checks
remain separate existing reuse prerequisites. No standalone equality result
permits checkout or credential publication. A revoked owner cannot supply a fresh
selection even when an older identity capability is still retained for equality.

## 4. Source observation, public-stage Drop and retained ownership

AcquisitionObservation identifies provider namespace, operation and acquisition
serial plus the captured endpoint/target/source kind. It has no physical peer,
network generation, release, cancellation or selection authority. Its terminal
record is immutable `{ reason, material_published }`; a separate monotonic
revoked fact handles owner revocation after successful transfer. Its cleanup
receipt reports Confirmed/Pending independently. No key bytes/path/diagnostic
dump is exposed. Observation payloads and all clones remain charged until their
last owner disappears, as with current AuthObservation.

| Stage/event | Immediate decision/public behavior | Actual retained owner |
| --- | --- | --- |
| Registered, not started | reserve and expose source observation; cancellation can prevent effects | Registered acquisition record and shared reservation |
| Acquiring | nonblocking polls; absolute deadline/cancel checked by retainer and worker control | Existing bounded source/native work owner, joined/probed through record |
| Result ready but untransferred | no terminal success/publication yet; transfer races atomically with close/deadline | Record owns charged immutable result |
| Result transferred | terminal success with material_published=true; select may consume only under live owning operation | Acquired/selected/identity/auth pins own SAME snapshot reservation |
| CredentialAcquiring/wait Drop, including unconsumed success | synchronously seal acquisition with Cancelled; late result cannot transfer | Retainer drives unfinished work and discards/zeroizes late material |
| cancel(self) / operation/provider close | synchronously seal matching acquisitions; report immediately with Pending where needed | Same record, slot, work and budget survive caller loss |
| AcquisitionClosing Drop | abandon observation only | Retainer continues cleanup |
| Native work finishes after cancellation/deadline | immutable terminal reason stays; no snapshot/output publication | Join/probe, zeroize discarded result, then confirm actual cleanup |
| AcquiredCredential/PreparedCredential/SourceIdentity Drop | release only its own pin; no sibling/global shutdown | Snapshot remains charged until LAST legitimate material/native pin disappears |

No selection grant is transferred until source publication wins under the same
provider/operation revocation guard. A worker may produce a private result after
cancellation but cannot publish it or start authentication. Terminal source
failure is observable immediately even if an OS read cannot be interrupted.
Confirming disposal requires the actual native owner to finish/join or its
qualified probe, not cancellation, timeout, dropped JoinHandle or a closed channel.
The original terminal reason is never overwritten by cleanup completion.

Provider/operation close includes acquisitions in its existing Pending/Confirmed
coverage. Every unfinished source job counts against both owner and shared
retention limits, including replacement owners. Source records share the existing
finite native-record allowance with auth records; the default 64 does not become
64 acquisitions PLUS 64 conversations. No tombstone registry or completed-job
history is added. Successful material and observations remain bounded separately
until their final pins disappear. A process-lifetime stalled read can therefore
cause lasting bounded refusal; it cannot create unbounded replacement capacity.

## 5. Privileged provider/execution seam

Replace the synchronous provider select/source-I/O seam with declared acquisition
charges and a promptly-created provider acquisition session. The privileged seam
is acquisition_charge(selection, purpose), then start_acquire(captured input,
selection, purpose, absolute control), returning an owned ProviderAcquisition.
Its poll_acquire returns immutable SelectedCredential; poll_close confirms actual
source disposal. SelectedCredential supplies full trusted identity equality in
addition to the existing auth/private-key ports. Kernel select never calls a
provider source lookup. RetainedWork exposes a WorkObservation enum with distinct
Acquisition and Authentication variants; no implementation may synthesize an
AuthObservation for a source record. Both variants use the SAME RetentionPort
reservation/retain service, rather than parallel cleanup ledgers. The session supplies
nonblocking poll_acquire and poll_close operations; the existing retained record
owns it before effects. Err before session registration is effect-free; registered
failures return an owned closeable session. The completed selected credential
still provides immutable private_key and existing authentication start/session
ports. Trusted adapter equality compares full admitted identity; there is no
application material getter or new signer.

The concrete standalone file route projects the existing agent_job::Entry/Exit
ownership pattern onto an acquisition session: one standard native thread handle,
one fixed-capacity private result slot and one cancellation/deadline control,
all charged under the registered acquisition. The provider session owns that
JoinHandle until its exit is observed and it has actually been joined; it does
not start a reaper thread or a supervisor registry. The independently scheduled
RetentionPort progresses this owner even after public handles disappear. A
stored result alone is not disposal. Filesystem work and parsing happen only on
the owned native thread; polls inspect completion and transfer/discard a bounded
result. Thread-start failure is an effect-free start refusal or owned registered
failure; successful spawn immediately has the session as its durable in-memory
owner. The source worker captures no complete operation/environment object.

Potentially blocking file work therefore uses the existing bounded native-work
ownership pattern: charge/reserve first, explicit native work handle owned by the
registered acquisition session, private worker result, nonblocking completion
inspection, actual join before disposal proof, cancellation/deadline controls,
and no detached/untracked work. No replacement supervisor, global executor or
process launch mechanism is authorized. The implementation gate must
exercise delayed native exit, panic/error, lost public handles and replacement
owner admission through this exact route. This is a narrowly scoped file worker,
not extraction of core's full Job/Supervisor or a new general execution service.
Native provider qualification remains required on every supported platform.

## 6. First explicit-file provider trace and conformance gates

1. Capture caller/policy and native endpoint base. Acquire K under the original
   absolute request deadline BEFORE any pool lookup. Reserve a fixed bounded
   snapshot (existing 1 MiB maximum plus bounded scratch/overheads), source record,
   native slot and observation before opening/reading. Reject nonregular files,
   unsupported/encrypted forms, oversize/empty/malformed input without native SSH.
2. Read once using the qualified source owner; preserve exact bytes, check live
   control around bounded chunks and validate existing supported container forms.
   Cancellation during a stalled read reports terminal Pending and keeps capacity.
3. Join admission and atomically transfer X only if original request is live.
   Pure select consumes the result; its source identity supplies exact-byte pool
   compatibility, with existing fresh trust/possession obligations unchanged.
4. For a new physical connection, verify endpoint-local host trust, then begin
   auth against actual peer/generation. LocalSshKeyPort borrows X; NeedNetwork
   retains X and re-entry never reopens K. Existing native connection-before-pin
   disposal remains mandatory on rejection/uncertainty.
5. The next prospective lookup acquires K again. If K now contains Y != X, the
   old identity is incompatible; no agent fallback. A path replacement after
   acquisition cannot alter X. No fake source test proves real native SSH auth.

Required permanent provider traces/faults: barrier-stalled source followed by
cancel/owner close and lost futures; immediate terminal source observation with
pending cleanup; no late source publication; reserved-slot refusal BEFORE open;
replacement-owner refusal against shared pending work; transfer-versus-close and
absolute-deadline races; result ready but unconsumed Drop; cleanup failure/panic
retains accounting; exact bytes/path replacement before each lookup; different
callers/endpoint/base/source cannot widen captured policy; native non-UTF-8 Unix
and native Windows filenames; unsupported/encrypted/oversize/special files;
no source access during pure select; exact source equality without key export;
NeedNetwork/native disposal pin tests; success material and observation retention
charged through last pin. Fake sources exercise both eager and genuinely pending
admission, not just already-loaded fixtures. Native source/provider qualification
and generic kernel review are separate acceptance evidence.

## 7. Exact supersession and bridge handoff

On acceptance only, this amendment controls these acquisition clauses:

- Credential §4 context.select(selection) becomes acquire(...).wait followed by
  select(acquired); its operation paragraphs, value table and §5 lifetime table
  gain the distinct acquisition stage defined here. §4.1 key memory-auth/pinning
  remains unchanged. §5.1 retention covers source records/observations as here;
  §5.2 record default remains 64 TOTAL. §7.2 steps 1/2/5 become this amendment's
  acquire-before-lookup, immutable select, fresh reread trace. All other clauses
  and provider policies remain unchanged.
- Boundary §3 capture/preparation and §4 per-open selection/time/continued-cleanup
  rows gain the acquisition/source observation handoff before pool lookup for
  explicit keys, with verified-request timing for providers that require it.
  §5 SSH native physical ownership and §§6–7 cleanup/qualification remain intact;
  their source-specific coverage is made explicit here.
- Connection §7 provider identity port and §7.1 explicit key reread are refined
  only to consume a successful acquired exact-byte identity before EVERY lookup.
  No Connection signature, fifth host port, dependency or pool policy is added.

The accepted historical documents remain evidence for their original tuples.
Draft pointers do not retrospectively change their prior acceptance. Upon GO/GO,
mark this amendment accepted at its exact tuple, then implement the bounded
Credential-only API/provider chunk under it. No Connection/core/CLI/Python source
work starts from this review. Later bridge work must prove source observation,
prelookup identity, auth observation and physical settlement mappings together.

Review object: the WHOLE affected Credential public lifecycle and Connection seam,
including unchanged authentication/reuse obligations, not merely the added
paragraphs or preservation of existing signatures. The portable implementation
is evidence, not authority to retain a wrong stage.

Review tier: joint dual Code/State with Surface in Code; two peer-blind reviewers,
at most two remediation rounds. Requirements/scope provenance is checked by both.
