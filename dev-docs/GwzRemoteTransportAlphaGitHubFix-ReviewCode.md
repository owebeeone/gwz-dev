# Alpha GitHub SSH Command Correction — Code Review

**Review date:** 2026-09-22  
**Verdict:** **GO**

**Exact tuple verified before and after review:** root `35773d77c4d045de8499aa1e63c99c50ab08ae48`; core `0904568d7e64c323f4353171ea203506360c8984`; CLI `ab59011db0ee00ab0c032fc23fc06b2578bf7b68`; evidence `da1f12b5d50a2c498e34f561f915f2a2660d2f3a`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`.

No P0–P3 findings.

The correction removes the incompatible `--` and emits canonical `git-upload-pack 'path'` or `git-receive-pack 'path'`. It preserves shell-quote escaping and separately rejects empty, oversized, NUL-containing, and option-shaped paths. This matches pinned libgit2’s command construction and its independent command-line-option rejection.

The regression covers both services and leading-dash refusal. Retained injection and connection-reuse coverage passes. Archived direct SSH evidence isolates the extra separator as GitHub’s rejection cause, while corrected alpha fetch reaches normal repository authorization behavior. The remaining private-evidence refusal also occurs through stable GWZ under the current account and does not contradict the compatibility fix.

The patch is bounded to command construction and its regression; it makes no authentication, activation, release, or broader programme claim.
