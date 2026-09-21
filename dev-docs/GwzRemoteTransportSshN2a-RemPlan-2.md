# SSH N2a — evidence-only remediation 2

Baseline root f92a34d9e097b2f947e8d8c544c3bf748fd2d068, core
dbc7616f7163de4d2bd78b839a50f7a2bc3c92a9, evidence
f90ce95e83fd17e87be18bf18937f5e470472fc4; other pins unchanged.
Code and State both report GO for production. State P3-2 identifies a nonblocking
test/evidence defect: returning true at predicate call128 ends the scan before
the next Control checkpoint, so the regression does not establish causality.
Code's independent report missed that test defect; no dual-axis convergence.

Accept P3-2. Change the predicate to false after its cancellation barrier and send
the scanner's own result to the parent. Assert that result is ConnectionAborted
and exactly128 predicate invocations occurred. Do not rely on Job arbitration or
a later explicit Control check to manufacture the observed error. The focused
test must fail the old true-predicate version (scanner returns Some(127)).

No production code changes. Preserve previous logs/report testimony, add corrected
focused evidence with exact source hashes, and ask retained State to close its
counterexample and Code to endorse the test-only delta on the final tuple. The
prior full suite remains the production baseline; no redundant full rerun is
needed for this targeted test correction. This is the second merged remediation,
with no new architectural cause and no additional scope or activation.
