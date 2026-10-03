# HTTPS caller capture — implementation caller guide

Accepted caller contract, 2026-10-04, with design tuple and scope in
[acceptance](GwzSspiHttpsCompositionAcceptance.md). The implementation now adds
the APIs below; its separate implementation review is pending. The owner selected
timeout-zero native refusal under the operator's directive. This guide supplies
no authority to activate Windows HTTPS transport. Native Windows integration is
not yet qualified, and Digest remains refused before native credential/context
work.

## Caller capture API

`gwz_sspi::CallerCapture` is an opaque owned type with private fields. It is
Send + Sync and implements neither Clone nor Debug. It provides no getters,
raw native handles or credential data. These are the public signatures:

```rust
pub struct CallerCapture { /* private fields */ }

impl Supervisor {
    pub fn capture_caller(&self) -> Result<CallerCapture, Error>;

    pub fn start_captured(
        &self,
        caller: &CallerCapture,
        request: AuthRequest,
        deadline: Deadline,
        cancellation: Cancellation,
    ) -> impl Future<Output = Result<Conversation, Failure>> + Send + use<>;
}
```

`capture_caller` synchronously retains the current original caller thread and
checks its primary SID/LUID/session against this Supervisor. It refuses caller
impersonation. It performs synchronous OS metadata and handle capture; no hard
OS time bound is promised. It creates no worker or record and reserves no worker
permit. Capture it on the original caller before moving work to member,
operation or endpoint threads.

`start_captured` owns the AuthRequest and retains its own private origin reference
during the synchronous call. It performs no metadata query, native-handle
duplication or worker/thread creation. Its returned future is Send + 'static,
borrowing neither CallerCapture nor Supervisor. Poll performs no metadata or
provider query, native creation, IPC, process wait or thread join.

The capture belongs to the exact Supervisor that issued it. Share the one
nonClone capture through `Arc<CallerCapture>` for an operation with concurrent
member Opens. Each Start uses its own one-use admission ticket under the same
Supervisor capacity. An eligible retry creates another Start from the same
capture; it does not recapture the executor identity or reuse a failed
conversation. At launch the library rechecks original-thread liveness,
impersonation and primary identity. An exited original thread still refuses.

## Existing inputs and defaults

Construct the Supervisor using a trusted absolute WorkerExecutable and its
matching compiled artifact-set fingerprint. CLI supplies its trusted self
executable; Python supplies the dedicated worker selected relative to its actual
loaded extension image. No runtime PATH/env/cwd selection or fingerprint fallback
is available. Supervisor construction remains synchronous and may perform OS
metadata and dedicated control-thread creation.

Retain descriptor/Supervisor/capture refusal in the host's native-availability
result when ordinary transport work can still proceed. Do not require successful
native setup merely to issue an anonymous request or use SSH.

Options still has only `max_workers`: default eight, checked range 1..=64.
Quarantined/pending workers retain their capacity. There is no new timeout,
identity-source, authentication or mechanism option. Native operation remains
Windows-only; a portable descriptor is not proof of usable Windows HTTPS auth.

AuthRequest, SecretBytes, SecretText, Identity, Package and TokenLimit keep their
existing contracts. Core supplies the selected source, verified final-origin
channel binding and `HTTP/<canonical-host>` target. Tokens and credentials are
owned zeroizing values, not diagnostic strings. TokenLimit is required, has no
default, and accepts 1..=65,536 raw bytes after accounting for HTTP header/base64
overhead. Native token completion is not authenticated HTTP success.

Deadline remains the immutable finite absolute monotonic timestamp made with
`Deadline::new(existing_instant)`. No timeout is chosen by the library. The
core source is one logical HTTPS Open setup deadline captured before
first checkout/adoption from the effective positive existing connect aggregate,
normally 30 seconds. Reused leases, discovery redirects, challenges, helper work,
capacity, launch, Hello, native rounds and IPC retain that same timestamp. Do not
construct `now + 30 seconds` at each native start or challenge. It cannot be
paused, reset or extended; an earlier enclosing cancellation signals the supplied
Cancellation. Existing active-HTTP-IO accounting remains separate.

Timeout zero refuses native selection before tokens when no finite setup
deadline exists. It does not silently restore a finite timeout.
Expired positive deadlines produce Timeout. Anonymous, existing Basic and SSH
do not require a native worker or caller capture; missing native availability
is retained and reported only if native authentication is selected.

## Caller recipe

The following recipe uses the caller capture API. Obtain the trusted descriptor and Supervisor in
the explicit host owner. On each original CLI/Python operation caller, before
any detach or thread handoff:

```rust
let caller = supervisor.capture_caller().map(std::sync::Arc::new);
```

Retain this Result with the registered operation. A capture error becomes a
fixed native refusal only if a native scheme is selected; it does not fail an
anonymous/Basic/SSH path. Share the successful Arc across concurrent Opens.
When core knows the actual
final origin, concrete TLS binding, selected identity and request, the endpoint
can construct an owned Start without substituting its identity:

```rust
use std::{future::Future, time::Instant};
use gwz_sspi::{
    AuthRequest, CallerCapture, Cancellation, Conversation, Deadline, Failure,
    Supervisor,
};

fn start_for_origin(
    supervisor: &Supervisor,
    caller: &CallerCapture,
    request: AuthRequest,
    fixed_open_deadline: Instant,
    cancellation: Cancellation,
) -> impl Future<Output = Result<Conversation, Failure>> + Send + use<> {
    supervisor.start_captured(
        caller,
        request,
        Deadline::new(fixed_open_deadline),
        cancellation,
    )
}
```

The Start can be moved/polled after these borrowed references are released.
After verified Hello, `Conversation::step(None)` sends Begin; later steps own
the bounded server challenge. Keep the same exclusive HTTP lease and origin
through all rounds, at most eight. Verify remote acceptance separately from
native Complete, then use `finish` under the original deadline. Failure/cancel
revokes publication and discards the challenged lease; never replay a partial or
complete POST for authentication or switch credentials after rejection.

## Errors and disposal

| Operation or condition | Fixed result |
|---|---|
| capture after admission closes, including closure during capture | ErrorKind::Closed |
| thread-handle capture, impersonation or primary verification failure | ErrorKind::IdentityMismatch, no native status |
| foreign Supervisor capture or invalid AuthRequest | Start Failure InvalidRequest before registration |
| closed Start admission | Start Failure Closed |
| observed cancellation / fixed deadline expiry | Start/step Failure Cancelled / Timeout |
| original thread exited, impersonated, or primary changed at launch recheck | Failure IdentityMismatch |
| worker unavailable / Hello fingerprint mismatch | Existing WorkerUnavailable / WorkerMismatch |

Other existing provider, containment and protocol classifications remain
unchanged. A pre-registration Start refusal has no RecordId and reports Confirmed
cleanup for that Start; it does not dispose the caller-owned capture. Registered
failures retain existing RecordId and Pending/Confirmed cleanup semantics.

Dropping CallerCapture releases its own origin reference. Starts already created
retain theirs independently. The last origin reference may synchronously dispose
its held native handle outside state locks, including on completed refusal or
Drop; no hard OS bound is promised for this accepted disposal exception. A
retained completed refused Start does not keep its request/private ticket alive.
Dropping a Conversation cancels; pending native cleanup remains charged and
owned by supervision. `cleanup_status` can report Pending, Confirmed or Unknown;
Unknown is not confirmation. Explicit `shutdown(deadline)` closes admission and
observes outstanding records until its separate shutdown bound. Dropping its
future does not abandon retained cleanup. Dropping a capture cannot reopen a
closed Supervisor or release another conversation's permit.
