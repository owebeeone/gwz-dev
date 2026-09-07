import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import yaml

spec = importlib.util.spec_from_file_location('tuple_gate', Path(__file__).with_name('workspace-tuple.py'))
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)

class TupleTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        conf = self.root / 'gwz.conf'
        conf.mkdir()
        manifest = {'members': [{'id': name, 'path': name} for name in ('core', 'cli')]}
        lock = {'members': {name: {'path': name, 'source_kind': 'git', 'commit': 'a' * 40} for name in ('core', 'cli')}}
        (conf / 'gwz.yml').write_text(yaml.safe_dump(manifest))
        (conf / 'gwz.lock.yml').write_text(yaml.safe_dump(lock))

    def test_candidate_does_not_float_sibling(self):
        rows = gate.resolve(self.root, 'core', 'b' * 40)
        self.assertEqual({'core': 'b' * 40, 'cli': 'a' * 40}, {row['path']: row['tested_revision'] for row in rows})
        self.assertTrue(all(row['locked_revision'] == 'a' * 40 for row in rows))

    def test_materialized_branch_heads_are_pinned_to_every_resolved_revision(self):
        rows = gate.resolve(self.root, 'core', 'b' * 40)
        heads = {'core': 'c' * 40, 'cli': 'd' * 40}
        calls = []
        def command(path, *args):
            calls.append((path.name, args))
            if args[1:3] == ('rev-parse', 'HEAD'):
                return heads[path.name]
            if args[1:3] == ('checkout', '--detach'):
                heads[path.name] = args[3]
            return ''
        with patch.object(gate, 'command', side_effect=command):
            gate.pin_revisions(self.root, rows)
        self.assertEqual(heads, {'core': 'b' * 40, 'cli': 'a' * 40})
        self.assertIn(('cli', ('git', 'fetch', '--no-tags', 'origin', 'a' * 40)), calls)

    def test_partial_unknown_and_floating_candidates_refuse(self):
        for member, sha in [('core', ''), ('', 'b' * 40), ('unknown', 'b' * 40), ('core', 'main')]:
            with self.subTest(member=member, sha=sha), self.assertRaises(ValueError):
                gate.resolve(self.root, member, sha)

    def test_missing_lock_fails_closed(self):
        (self.root / 'gwz.conf/gwz.lock.yml').unlink()
        with self.assertRaises(FileNotFoundError):
            gate.resolve(self.root, '', '')

if __name__ == '__main__': unittest.main()
