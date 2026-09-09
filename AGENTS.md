Read and follow `AGENTS_GWZ.md` before doing any work in this workspace.

## Explicit scope for control flow and conditional compilation

Standing rule across all projects and languages, effective immediately:

- In C-style languages, every control-flow body must be a compound statement
  (`{ ... }`), including single-statement and empty bodies of `if`, `else`,
  `for`, `while`, and `do`. An `else if` chain is allowed with braced branch bodies.
- Conditional compilation must have an explicit enclosing boundary. In Rust,
  use `cfg_if::cfg_if!` blocks or enclosing platform modules for conditional
  sections. Do not place `#[cfg(...)]` or a conditional `cfg_attr` directly on
  individual imports or other unbraced declarations. Keep unconditional imports
  outside the conditional section. Ordinary nonconditional attributes are not
  prohibited by this rule.
- When deleting or moving a declaration, treat its attributes and owning scope
  as part of the edit. A condition must never silently transfer to the next
  declaration. Apply this rule to new and modified code now; record broader
  existing-code migrations explicitly rather than claiming they are complete.
- Prefer fast syntax-aware lint/source checks to enforce these rules. The checks
  must inspect disabled platform branches too, without compiling every variant.

Reason: GWZ commit `cfa14b8` deleted a conditional `FileSystem` import but left
`#[cfg(not(windows))]` behind. It attached to the following `OsStr` import in two
files, passed macOS checks, and broke the v1.0.5 Windows build. Explicit boundaries
prevent this particular silent reassociation during edits and textual merges.

## Campaign evidence

Read `EVIDENCE.md` before creating or relocating experimental evidence. Keep raw runs and campaign-only runners in the appropriate private evidence member; keep build outputs outside it and public CI dependencies public.
