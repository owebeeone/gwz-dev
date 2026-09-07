#!/usr/bin/env python3
"""Refuse a workspace CI result with missing evidence or zero executed tests."""
from pathlib import Path
import json
import re
import sys


def check(directory):
    tuple_data = json.loads((directory / 'tuple.json').read_text())
    if not tuple_data.get('members') or not re.fullmatch('[0-9a-f]{40}', tuple_data.get('workspace_revision', '')):
        raise ValueError('empty or unidentified source tuple')
    for row in tuple_data['members']:
        if not re.fullmatch('[0-9a-f]{40}', row.get('tested_revision', '')):
            raise ValueError('member revision is not exact')
    for name in ['local-clone.log', 'publication.log', 'rust-driver.log', 'protocol.log']:
        output = (directory / name).read_text()
        summaries = re.findall(r'test result: (\w+)\. (\d+) passed; (\d+) failed;', output)
        if not summaries or any(state != 'ok' or int(passed) == 0 or int(failed) != 0 for state, passed, failed in summaries):
            raise ValueError(f'{name}: failed or zero-test command')
    python = (directory / 'python-driver.log').read_text()
    if not re.search(r'\b[1-9][0-9]* passed\b', python) or re.search(r'\b[1-9][0-9]* (failed|errors?)\b', python):
        raise ValueError('Python suite did not execute successfully')
    if "AGGREGATE: this selection's mechanical gates pass" not in (directory / 'compatibility.log').read_text():
        raise ValueError('complete compatibility evidence is absent')
    for name in ['toolchain.txt', 'cargo-version.txt']:
        if not (directory / name).read_text().strip():
            raise ValueError(f'{name}: identity is missing')
    inventory = json.loads((directory / 'core-inventory.json').read_text())
    execution = inventory.get('execution', {})
    tests = inventory.get('tests', [])
    executed = execution.get('executed', [])
    ignored = execution.get('ignored', [])
    if (inventory.get('execution_checked') is not True
            or inventory.get('executed_partition') != 'all'
            or not executed or len(tests) != len(set(tests))
            or len(executed) != len(set(executed))
            or set(executed) & set(ignored)
            or set(executed) | set(ignored) != set(tests)
            or set(ignored) != set(inventory.get('ignored', []))):
        raise ValueError('compiled core test census was not fully executed')
    for prefix in ['local_clone::tests::', 'workspace_ops::tests::g08::']:
        if not any(name.startswith(prefix) for name in executed):
            raise ValueError(f'required core scenario family did not execute: {prefix}')


if __name__ == '__main__':
    check(Path(sys.argv[1]))
    print('workspace execution evidence: ok (local job evidence; promotion review is separate)')
