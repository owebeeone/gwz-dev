"""Disposable native Windows CLI recovery checks; all fixtures under D:/ or E:/."""
import json
import os
from pathlib import Path
import subprocess
import sys

binary = 'D:/gwz-tests/capability-gate/target/debug/gwz.exe'
base = Path(sys.argv[1])
base.mkdir(parents=True, exist_ok=True)
rows = []
env = dict(os.environ, GIT_CONFIG_COUNT='2', GIT_CONFIG_KEY_0='core.autocrlf', GIT_CONFIG_VALUE_0='false', GIT_CONFIG_KEY_1='core.eol', GIT_CONFIG_VALUE_1='lf', GIT_AUTHOR_NAME='Capability Fixture', GIT_AUTHOR_EMAIL='fixture@example.invalid', GIT_COMMITTER_NAME='Capability Fixture', GIT_COMMITTER_EMAIL='fixture@example.invalid')

def run(root, args, ok=True):
    result = subprocess.run(args, cwd=root, env=env, text=True, capture_output=True)
    rows.append(dict(cwd=str(root), args=args, code=result.returncode, stdout=result.stdout, stderr=result.stderr))
    (base / 'results.json').write_text(json.dumps(rows, indent=2))
    if ok and result.returncode:
        raise RuntimeError(rows[-1])
    return result

def gwz(root, *args, ok=True):
    return run(root, [binary, '--root', str(root), *args], ok)

def commit(root, text):
    gwz(root, '--target', '@root', 'add', '.')
    gwz(root, '--target', '@root', 'commit', '-m', text)

for action in ['clean', 'continue', 'abort']:
    root = base / action
    root.mkdir()
    gwz(root, 'init')
    (root / 'fixture.txt').write_bytes(b'base\n')
    commit(root, 'fixture baseline')
    branch = run(root, ['git', 'branch', '--show-current']).stdout.strip()
    gwz(root, '--target', '@root', 'branch', '--create', 'fixture-feature', '--switch')
    (root / 'fixture.txt').write_bytes(b'feature\n')
    commit(root, 'fixture feature')
    run(root, ['git', 'switch', branch])
    if action != 'clean':
        (root / 'fixture.txt').write_bytes(b'main\n')
        commit(root, 'fixture main')
    baseline = run(root, ['git', 'rev-parse', 'HEAD']).stdout.strip()
    result = gwz(root, '--json', '--target', '@root', 'merge', 'fixture-feature', '--no-ff', '--filesystem-strict', ok=False)
    assert json.loads(result.stdout)['merge']['crash_recovery']['supported'] is True, rows[-1]
    if action == 'clean':
        assert result.returncode == 0, rows[-1]
    else:
        # The first CLI process is now gone; resume the persisted conflict in a new process.
        status = gwz(root, 'merge', '--status', ok=False)
        assert 'awaiting-resolution' in status.stdout, rows[-1]
        if action == 'continue':
            (root / 'fixture.txt').write_bytes(b'resolved\n')
            gwz(root, '--target', '@root', 'add', 'fixture.txt')
        gwz(root, 'merge', '--' + action)
    head = run(root, ['git', 'rev-parse', 'HEAD']).stdout.strip()
    if action == 'abort':
        assert head == baseline
        assert (root / 'fixture.txt').read_bytes() == b'main\n'
    else:
        # Root publication may add a single-parent composition commit above the merge.
        merges = run(root, ['git', 'rev-list', '--min-parents=2', baseline + '..HEAD']).stdout.split()
        assert merges, 'expected a merge commit in the completed ancestry'
        run(root, ['git', 'merge-base', '--is-ancestor', 'fixture-feature', 'HEAD'])
        assert (root / 'fixture.txt').read_bytes() == (b'feature\n' if action == 'clean' else b'resolved\n')
    gwz(root, 'status')
    assert not list((root / '.gwz' / 'merge').glob('*.yaml'))
print('PASS: strict clean merge, process-restart continue and abort; final HEAD, parents, contents and records verified')
