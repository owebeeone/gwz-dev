# SSH channel remediation, round 1

Status: **closed by original Code/State/Surface GO at core
f03f5f79bae73d378e575273af0b9ed2a87c052d, root
6076c6153f2b4fb74da5179b0ec6ffd2765d81ad**. Original reports and closure reports
are preserved as `GwzRemoteTransportSshChannel-Review{Code,State,Surface}[-1].md`. Original reviewed root
823ffdf50686bae2a222f80d673e7ad122780684, core
b77f4fef5dbb6958789dd8160fbb74eb67a3f47e. Original Code and State independently
found the same cancellation-disposal defect; Surface GO. Preserve all reports.

| Finding | Disposition | Closure evidence |
|---|---|---|
| Code P2-1 / State P2-1 | Add a socket-owning SSH connection owner, retryable channel disposal and bounded forced termination before native objects drop; successful extraction preserves the whole connection owner. Never acknowledge disposal before ownership is released. | Native stalled-close test retains ownership across repeated WouldBlock; force termination then release, without reusable session; explicit graceful disposal and early-drop paths. Both original reviewers must close their own finding. |
| Lane-owner P2-L1 | std::io::Write::flush currently delegates to ssh2::Channel::flush, which calls libssh2_channel_flush_ex and discards unread incoming channel data. Replace with active-state check and no-op: the primitive has no outgoing application buffer, accepted writes already hand bytes to libssh2. | Native test buffers a real Git advertisement, calls Write::flush, and still reads the exact complete advertisement. Reviewer verifies pinned dependency call semantics and regression. |

The lane-owner finding is independent, found during dependency inspection after
review dispatch; it is not claimed as blind reviewer convergence. Existing tests
read the advertisement before flushing and miss this corruption path.

Keep physical ownership inside core, no wire or CLI/core API change. Introduce
only the smallest connection ownership seam needed for reliable disposal, with
no authentication or trust-policy expansion. Require all original reviewers to
recheck the changed API and counterexamples; user explicitly requested retained
reviewers. Test regressions precede correction. Existing 250-line channel budget
remains; place connection ownership/disposal helpers in a cohesive adjacent module
(up to 120 lines), with focused additional native regression coverage in its own
test module (up to 180 lines). No production activation or claim of platform parity.
