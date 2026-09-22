from pathlib import Path
import re,subprocess,json
root=Path('/Users/owebeeone/limbo/gwz-dev')
template=Path('/Users/owebeeone/.claude/skills/review-loop/references/review-prompt-template.md').read_text()
body=re.search(r'## Prompt body\s+```text\n(.*?)\n```',template,re.S).group(1)
report=re.search(r'## Report template.*?```markdown\n(.*?)\n```',template,re.S).group(1)
repos=['.','gwz-core','gwz-transport','taut','gwz-cli','gwz-py','gwz-git','git2-rs','libgit2','gwz-core-evidence']
tuple_={r:subprocess.check_output(['git','-C',str(root/r),'rev-parse','HEAD'],text=True).strip() for r in repos}
for axis in ['Code','State']:
 role=re.search(r'```text\n(AXIS: '+axis.upper()+r' .*?)\n```',template,re.S).group(1)
 output='dev-docs/GwzRemoteTransportQualification-Review'+axis+'.md'
 text=body.replace('{AXIS NAME — one of Code / State / Consistency / Safety; role section below}',axis+'\n'+role).replace('{REPORT FILENAME}',output)
 a=text.index('- {REPO}:'); b=text.index('\nAUTHORITY AND DEFERRALS',a)
 text=text[:a]+'\n'.join('- '+r+': '+sha for r,sha in tuple_.items())+'\n- Object: Q6 batch A core diff e03cd3cadc458b09e72281cd22bfe28fc7afdd04..HEAD; evidence diff 3302b5d03f56590a6d521b1b52db302861775e15..HEAD; root Q6 checkpoint/discovery/remediation records.\n- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportQualification.md at '+tuple_['gwz-core']+'\n- Out of scope: four old untracked N2b prompt files; Q6 prompt/report outputs.\n'+text[b:]
 a=text.index('- Process authority:'); b=text.index('\nREVIEW AREAS',a)
 text=text[:a]+'''- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md; CurrentProgramCheckpoint.md; review-loop skill. Reuse retained reviewers per operator instruction.
- Controlling documents: Q6 report; GwzRemoteTransportPlan Phase6; GwzGitLibraryQualification; H2/H1 and placement accepted lifecycle contracts. EVIDENCE.md for provenance.
- Bounded batch acceptance only: no claim Phase6 complete, Windows integrated adapter implemented, missing hardware qualified, defaults tuned, source published or installed-consumer qualification. Physical wire/iroh remains explicitly outside current cycle. No activation/release or real-account use. These outcomes remain open, their present API/lifecycle shape stays reviewable.
''' + text[b:]
 a=text.index('{Bulleted, package-specific:'); b=text.index('\nCOMMANDS',a)
 common='''- Review the small session lifecycle correction plus deterministic regression, private runner diagnostics correction, native execution evidence and bounded measurement/report claims as one aggregate. The retired mux request must never become unretired, while physical pending cleanup and failed logical retirement remain accounted for. No timeout or protocol relaxations.
- Read dev-docs/GwzRemoteTransportQualification-RemPlan.md. This new Q6 object starts after H2 acceptance; pre-gate corrections do not reset prior H2 findings. State verifies its discovery counterexample and Q5 State P3-1. Include a closure table. Do not read the other axis current report/prompt.
- Validate source/runner/input hashes, instrumented external copy adaptations, native versus historical/unimplemented matrix distinctions, ignored tests, failed-attempt preservation, and statistical attribution. Product builds must not depend on the private archive. No expected-test-count gate pins.
- Raw sources and results: gwz-core-evidence/campaigns/transport-qualification/runs/2026-09-22-q6-a; runner/local_perf.rs. Selected Git-binding tests exercise exact patched sources but do not prove remote distribution. Perf fixtures measure loopback selected-key SSH and anonymous TLS HTTPS; no WAN extrapolation, real Gh or full-workspace claim.
'''
 extra=('- Code emphasis: one-shot mux finish, registration ownership, architecture invariants, native source selection/graphs, benchmark timings and reproducibility.' if axis=='Code' else '- State emphasis: old five-second completed-registration counterexample, logical versus physical cleanup deadlines, result stability, healthy shared carrier preservation, command output/status retained even when post-run config rejects; falsifiable bounded parity/measurement claims.')
 text=text[:a]+common+extra+'\n'+text[b:]
 a=text.index('{The exact inspection/gate commands'); b=text.index('\nSEVERITY AND VERDICT CONTRACT',a)
 text=text[:a]+'''Working directory /Users/owebeeone/limbo/gwz-dev. Read-only rg/sed/cat/git show/diff/log/status/rev-parse and private evidence reads allowed. No build or dependency resolution. Optional targeted execution of already-built binary /tmp/gwz-placement-b-backend-target/debug/deps/gwz_core-93f14a6e615c0a84 with filters completed_request_retirement_cannot_expire_the_shared_session, shutdown_reports_blocked_physical_disposal_then_eventual_zero or q6_ and --test-threads=1 allowed; it contains the exact three corrected core files plus private perf instrumentation. Python runner test execution using -B and discover is allowed (temporary fixtures outside repositories). Source/manifests must not mutate. Verify all ten HEADs at start/end. Return full concise report only; owner files it verbatim. No peer messages/reports. Larger historical design rereads are unnecessary unless changed-range consequence requires them.
''' +text[b:]
 (root/('dev-docs/GwzRemoteTransportQualification-Prompt'+axis+'.md')).write_text(text+'\n\nMANDATED REPORT FORMAT\n'+report+'\n')
Path('/tmp/gwz-q6-review-tuple.json').write_text(json.dumps(tuple_,indent=2)+'\n')
print(json.dumps(tuple_,indent=2))
