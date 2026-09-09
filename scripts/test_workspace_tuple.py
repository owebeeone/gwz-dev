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

    def test_private_members_are_excluded_from_materialization_and_candidates(self):
        manifest_path = self.root / 'gwz.conf/gwz.yml'
        manifest = yaml.safe_load(manifest_path.read_text())
        manifest['members'].append({'id': 'evidence', 'path': 'evidence', 'private': True})
        manifest_path.write_text(yaml.safe_dump(manifest))
        lock_path = self.root / 'gwz.conf/gwz.lock.yml'
        lock = yaml.safe_load(lock_path.read_text())
        lock['members']['evidence'] = {
            'path': 'evidence', 'source_kind': 'git', 'commit': 'c' * 40,
        }
        lock_path.write_text(yaml.safe_dump(lock))

        self.assertEqual(['evidence'], gate.private_member_paths(self.root))
        self.assertEqual({'core', 'cli'}, {row['path'] for row in gate.resolve(self.root, '', '')})
        with self.assertRaisesRegex(ValueError, 'private workspace member'):
            gate.resolve(self.root, 'evidence', 'b' * 40)
        with patch.object(gate, 'command') as command:
            gate.materialize_public_members(self.root, '/tmp/gwz', ['evidence'])
        command.assert_called_once_with(
            self.root, '/tmp/gwz', '--no-target', 'evidence', 'materialize', '--lock'
        )

    def test_candidate_derives_the_lock_but_baseline_does_not(self):
        lock = self.root / 'Cargo.lock'
        lock.write_text('baseline\n')
        with patch.object(gate, 'command') as command:
            baseline = gate.cargo_lock_state(self.root, False)
        self.assertFalse(baseline['cargo_lock_derived'])
        self.assertEqual(baseline['cargo_lock_sha256'], gate.hashlib.sha256(b'baseline\n').hexdigest())
        command.assert_not_called()

        def update_lock(root, *args):
            self.assertEqual(('cargo', 'update', '--workspace'), args)
            (root / 'Cargo.lock').write_text('candidate\n')
            return ''

        with patch.object(gate, 'command', side_effect=update_lock) as command:
            candidate = gate.cargo_lock_state(self.root, True)
        self.assertTrue(candidate['cargo_lock_derived'])
        self.assertEqual(candidate['cargo_lock_sha256'], gate.hashlib.sha256(b'candidate\n').hexdigest())
        command.assert_called_once_with(self.root, 'cargo', 'update', '--workspace')

    def test_missing_lock_fails_closed(self):
        (self.root / 'gwz.conf/gwz.lock.yml').unlink()
        with self.assertRaises(FileNotFoundError):
            gate.resolve(self.root, '', '')

if __name__ == '__main__': unittest.main()
