## Post-acceptance qualification trace

**Classification: P2 — real shared-session lifecycle defect with a new architectural root.**

Accepted core `e03cd3cadc458b09e72281cd22bfe28fc7afdd04` retains every sealed request registration after cleanup completes. In `src/transport_host/session/driver.rs:472-513`:

1. `owner.finish(id)` initially succeeds and removes the mux request.
2. The cleanup result is stored in `Registration.result`.
3. Later driver passes revisit the same sealed registration.
4. The repeated `owner.finish(id)` now returns `InvalidRequest`, so the code reclassifies the already-retired mux request as unretired.
5. Five seconds after the original seal time, `close_state` closes the healthy shared session.

The observed timing matches exactly: the first three clones take approximately 5.62 seconds total, so sample 3 overlaps the first completed request’s five-second cleanup deadline. Its active stream then reports `CarrierLost`. The 11 warm advertisements complete well inside five seconds. The independent repeat failing at the same sample reinforces this diagnosis.

The large buffered fixture response and libgit2’s legitimate exchange abandonment affect timing and cleanup pressure, but they are not required for the failure. No late-message, mux-capacity, trust, or physical-connection failure is needed. The later SSH test uses a separate runtime, so its success does not contradict closure of the HTTPS test’s shared session.

### Focused regression

Create an end-to-end request with no streams or one clean small RPC, await `finish()` with zero pending work, then deterministically backdate its retained registration’s `sealed` instant beyond `CLEANUP`. Run one driver step and assert:

- the driver and endpoint sessions remain open;
- a new request can bind/open successfully;
- the completed request’s cleanup result remains unchanged.

A repeated large-clone test extending beyond five seconds should remain as integration coverage.

### Bounded correction

Track mux retirement as a monotonic registration fact, such as `mux_retired`. Set it once when `owner.finish(id)` succeeds, the owner is absent, or the mux/session is closed; never retry or clear it. Keep physical endpoint/preparation cleanup accounting separate:

- complete normally when `!pending && mux_retired`;
- at the cleanup deadline, close only if mux retirement never completed;
- if mux retirement completed but physical work remains, return the bounded pending count without closing the shared carrier.

This preserves the distinction between one-shot logical mux retirement and potentially longer physical cleanup. No files were changed and no builds were run.
