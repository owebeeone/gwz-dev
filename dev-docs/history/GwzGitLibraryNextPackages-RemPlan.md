# Next packages scope correction

State P2-1 accepted: Q1 omitted the indirect Python consumer. Add its exact
revision, manifest/lock, graph inspection and provider/platform/package gates;
explicitly distinguish standalone CLI lock from root-workspace resolution.
Require all active members inventoried, including nonconsumers. No runtime,
manifest, lock, budget, C1 or H1 change. Closure: original State reviewer checks
those requirements against the current active member set. Code's initial GO
remains evidence for unchanged C1/H1; focused Code confirmation covers Q1 text.
One text-only remediation round; findings not self-closed by the owner.

Original State reviewer closed P2-1 at corrected core `6586768396886fe1aeb1371bbd3064377cfa70ec`; retained Code confirmed GO. Reports filed verbatim.
