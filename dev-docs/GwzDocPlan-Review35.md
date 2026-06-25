# GWZ Documentation Plan — Comprehensive Technical Review (v0.3.0 Surface)

- **Reviewer:** Claude 3.5 (Agent Mode)
- **Date:** June 25, 2026
- **Target Document:** `dev-docs/GwzDocPlan.md` (GWZ Documentation Plan)
- **Status:** Review Completed & Recommendations Formulated

---

## 1. Executive Summary

The **GWZ Documentation Plan** is an exceptionally thorough, robust, and well-structured plan. It lays out a comprehensive strategy to transition GWZ's documentation into a mature, production-grade state. 

### Key Strengths
1. **Clear Scope Boundary:** By strictly targeting the v0.3.0 surface (tag redesign, `ls`, and `forall`) and explicitly deferring out-of-scope features (such as branch and stash support), the plan prevents scope creep and focuses resources on documenting what is actually implemented.
2. **Parallelizable, Agent-Friendly Structure:** The division of tasks into a structured matrix of independent work packages (`D0a` through `X1`) is outstanding. The explicit input/output definitions and clear acceptance criteria are ideal for multi-agent execution or modular human team distribution.
3. **Decoupled Architectural Slices:** The plan clearly distinguishes user-facing CLI behavior, Rust API entry points, Taut protocol layers, and disk serialization schemas, preventing confusing bleed-through between internal implementation details and user-facing workflows.

This review provides structural feedback, resolves the listed "Open Decisions" with practical technical designs, and highlights a few newly identified operational gaps that should be addressed during implementation.

---

## 2. In-Depth Analysis of Core Documentation Artifacts

### 2.1. `AGENTS_GWZ.md` (The LLM Bootstrap File)
*   **Concept Evaluation:** The addition of a lightweight, machine-readable bootstrap file is a highly forward-thinking design. In a world where AI agents frequently interact with repositories, having an explicit "how to bootstrap this GWZ workspace" file prevents agents from having to guess CLI patterns or read lengthy human guides.
*   **Safety & Overwrites:** The plan highlights that the file must be safe to overwrite. To ensure this, we must have a mechanism that detects manual modifications (see Section 3.2 for the recommended checksum/marker-line design).
*   **Agent Discovery Path:** If an agent clones only the root repository first, `AGENTS_GWZ.md` is positioned exactly where they need it (at the root of the workspace) to understand that they are inside a GWZ-managed super-workspace and need to run `gwz materialize` to clone and link individual member repositories.

### 2.2. Root `README.md`
*   **Workspace dogfooding:** The plan notes that the root workspace is itself GWZ-managed through tracked `gwz.conf/` metadata. The root `README.md` should explicitly highlight this "dogfooding" aspect. It is a fantastic educational example for new developers to see how GWZ manages its own development environment.

### 2.3. `gwz-cli/docs` Set
*   **Porcelain vs Plumbing:** The inclusion of `MachineOutput.md` is critical. It must formally document the exact schema of JSON/JSONL outputs, status porcelain formats, and standard exit codes. For instance, what is the exit code of `gwz` when some members succeed but one fails in `--partial` mode? (e.g., standardizing on a non-zero partial-success code or a specific `2` for partial failures).
*   **`forall` Command Nuances:** `forall` is a very powerful utility. The documentation must clearly outline how environment variables like `GWZ_MEMBER_ID` and `GWZ_MEMBER_PATH` are exposed to child processes, allowing developers to write resilient workspace-wide scripts.

### 2.4. `gwz-core/docs` Set & Message Catalog
*   **Git Tag Refactor Clarification:** The fact that `gwz tag` now operates on native Git refs (`refs/tags/*`) instead of tracked `gwz.conf/tags` artifacts is a major improvement that simplifies metadata sync. However, legacy developers or AI agents reading older historical files under `dev-docs/` might still assume `gwz.conf/tags` is the source of truth. Clearly pointing this out in `WorkspaceArtifacts.md` is essential to break old assumptions.
*   **Taut Source Generator (`P1`):** Generating the message catalog directly from `gwz-core/protocol/gwz.taut.py` is the correct engineering path. Since `gwz.taut.py` is the single source of truth for schema tags and message fields, this prevents manual maintenance drift.

---

## 3. Recommended Resolutions for Open Decisions

To make this plan immediately actionable, the following concrete architectural and technical designs are recommended to resolve the eight listed "Open Decisions":

### 3.1. `gwz init --update` Scope
*   **Recommendation:** By default, `gwz init --update` should update the bootstrap files (`AGENTS_GWZ.md`, etc.) **only for the active workspace root** (and not cascade into member repositories unless requested).
*   **Syntax & Flags:** 
    *   `gwz init --update` updates the current repository's bootstrap files.
    *   `gwz init --update --recursive` updates the root workspace *and* iterates through all active, materialized member repositories to update theirs.

### 3.2. User-Edited `AGENTS_GWZ.md` Detection
*   **Recommendation:** Use a custom comment header containing a content hash to determine if a file is safe to overwrite.
*   **Implementation Design:** When `gwz` generates or updates `AGENTS_GWZ.md`, it prepends a header:
    ```markdown
    <!-- gwz-managed-file: md5=<hash_of_template_body_only> -->
    ```
    During `gwz init --update`:
    1. If the file is missing, it is written.
    2. If the file exists and has the header, `gwz` computes the MD5 hash of the current file's body (excluding the header line itself) and compares it with the hash in the header. 
       * If they match, the file is unedited and is safe to overwrite automatically.
       * If they do not match, or if the header is completely absent, the user has edited the file. `gwz` must skip the update and print a warning (or require a `--force` flag).

### 3.3. `CLI.md` Generation Model
*   **Recommendation:** A **hybrid generation model** is highly recommended. 
*   **Structure:**
    *   The structural synopsis, options table, and subcommand argument flags should be fully generated from Clap command definitions using a CLI command like `gwz-cli-gen-docs`. This ensures 100% accurate drift detection.
    *   The deeper conceptual workflows and hand-authored examples should reside in individual command pages under `gwz-cli/docs/commands/*.md`.
    *   `CLI.md` should serve as a generated master index that links to these command family files, keeping the reference list clean and automated.

### 3.4. Message Catalog Publication
*   **Recommendation:** The generated Taut Message Catalog should live in `gwz-core/docs/MessageCatalog.md` in the repository, but a CI/CD task should automatically convert and bundle it alongside static release artifacts (e.g., published on GitHub Pages or zipped with release bundles). Since Taut is "the substrate under Glade" and intended for multi-language consumers, having this catalog public and searchable is highly beneficial for external integrators.

### 3.5. Artifact Schema Examples Generation
*   **Recommendation:** Yes, artifact schema examples (like `gwz.yml` and `gwz.lock.yml`) **must be generated from Rust test fixtures**.
*   **Design:** In `gwz-core/src/model/tests.rs` (or similar), a test case should construct standard workspace and lock objects, serialize them to YAML using the active serializers, and compare them against files in `gwz-core/docs/examples/`. 
    *   During local testing, a flag or environment variable (e.g., `UPDATE_EXPECT=1 cargo test`) can rewrite these example files.
    *   In standard CI runs, any drift between serialized objects and the documented examples will fail the build, guaranteeing the documentation remains structurally exact.

### 3.6. CLI-Local `Exec*` Messages Location
*   **Recommendation:** CLI-local `Exec*` messages (which are used for `forall` executions but bypass the core daemon) should be documented in `gwz-core/docs/Protocol.md` under a dedicated **"CLI-Local and Transport-Specific Messages"** section. 
*   **Justification:** This keeps the service methods clean while documenting how the CLI and core layers interact. It also clearly outlines why these messages do not map to standard core handlers.

### 3.7. `forall` Machine-Output Mode
*   **Recommendation:** Keep the v0.3.0 behavior simple by strictly rejecting `--json`/`--jsonl` flags on `forall`, as described in the plan.
*   **Future Roadmap Annotation:** The documentation should explicitly state that machine-readable output for `forall` is deferred to a future release, but outline the proposed target schema (such as streaming JSON lines mapping standard out/err per member):
    ```json
    {"member": "gwz-core", "stream": "stdout", "line": "running tests..."}
    ```

### 3.8. `GWZDesign.md` Historical Deprecation
*   **Recommendation:** **Do not rewrite design history.** Instead, prepending a clear, prominent warning/deprecation box to `GWZDesign.md` is the cleanest and most transparent path.
*   **Header Template:**
    ```markdown
    > [!WARNING]
    > **Historical Design Document Only**
    > This document represents the original architecture design and does not fully reflect the current production state. 
    > Specifically, layout schemas have migrated to `gwz.conf/gwz.yml`, snapshot paths reside in `gwz.conf/snapshots`, and tag management utilizes native Git refs instead of custom tag artifacts.
    > For the authoritative v0.3.0 schemas and implementation details, please refer to the active documentation set under:
    > - `gwz-cli/docs/`
    > - `gwz-core/docs/WorkspaceArtifacts.md`
    ```

---

## 4. Newly Identified Gaps & Operational Enhancements

During a deep review of the plan and surrounding repo-level structures (such as `gwz-cli/scripts/release.py` and `taut/dev-docs/TautModules.md`), the following key gaps and enhancements were identified and should be integrated into the respective work packages:

### 4.1. The Release Branch "Merge Gotcha" (Package `C4`/`X1`)
*   **Context:** In `gwz-cli/scripts/release.py`, we observe that the `release` branch differs from `main` in exactly one way: the `gwz-core` dependency is local on `main` (`path = "../gwz-core"`) but pinned to a git tag on the `release` branch.
*   **Gap:** If a developer merges `main` into `release` manually, they can easily overwrite this pinned dependency line, breaking the release compile.
*   **Mitigation:** `gwz-cli/docs/Releases.md` (or a dedicated section in `Install.md`) must explicitly document this special release-branch anatomy, explain why `release.py` exists, and provide clear step-by-step instructions on resolving the "merge gotcha" when cutting a CLI release.

### 4.2. Credential & Transport Security (Package `K4`)
*   **Context:** Multi-repository workflows like `materialize` or remote `tag` push/fetch operations often run in parallel or loop over multiple sub-repos.
*   **Gap:** If a user has SSH passphrase prompts or MFA enabled, parallel SSH commands can lock up, spawn overlapping prompts, or fail silently in background threads.
*   **Mitigation:** `GitBackend.md` must clearly document credential handling boundaries. It should guide users on:
    1. Pinned authentication patterns (SSH agent setup).
    2. How the core library isolates credential requests.
    3. How background threads handle blocking passphrase entries or transport connection timeouts during concurrent pulls or clones.

### 4.3. Materialization Safety & Dirty States (Package `K5` / `C3`)
*   **Gap:** When running `gwz materialize`, the system updates or checks out repositories. What happens if a developer has dirty, uncommitted files or untracked changes in one of the members?
*   **Mitigation:** The command documentation for `materialize` and `QuickStart.md` must explain the safety policies:
    *   Does it abort immediately if any selected member is dirty?
    *   Are there `--force` or `--stash` flags?
    *   How can a developer safely update their workspace topology without losing local work?

### 4.4. CI Validation of Documentation Drift (Package `V2`)
*   **Gap:** Manual drift checks are prone to human oversight.
*   **Mitigation:** Expand work package `V2` (Link and drift checks) to include a concrete CI task. For example, a GitHub Action should run:
    ```bash
    # Generate the message catalog and CLI reference
    cargo run -p gwz-cli-gen-docs
    # Fail if git detects any uncommitted changes to documentation
    git diff --exit-code
    ```
    This ensures that code changes that alter Clap subcommands or Taut schema definitions must be accompanied by their regenerated documentation files, keeping docs fresh on every commit.

---

## 5. Conclusion & Actionability

The **GWZ Documentation Plan** is highly actionable, logically structured, and ready to proceed. By implementing the technical recommendations outlined in this review (especially regarding the `AGENTS_GWZ.md` checksum mechanism, the hybrid generation model for `CLI.md`, and the automated test serialization for workspace artifacts), the engineering team can guarantee a robust, long-lasting, and self-validating documentation suite for GWZ v0.3.0.
