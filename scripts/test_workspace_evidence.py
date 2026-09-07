import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
spec = importlib.util.spec_from_file_location('evidence', Path(__file__).with_name('check-workspace-evidence.py'))
evidence = importlib.util.module_from_spec(spec)
spec.loader.exec_module(evidence)

class EvidenceTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        (self.root / 'tuple.json').write_text(json.dumps({'workspace_revision': 'a' * 40, 'members': [{'tested_revision': 'b' * 40}]}))
        for name in ['local-clone.log', 'publication.log', 'rust-driver.log', 'protocol.log']:
            (self.root / name).write_text('test result: ok. 1 passed; 0 failed; 0 ignored;\n')
        (self.root / 'python-driver.log').write_text('5 passed in 1s')
        (self.root / 'compatibility.log').write_text("AGGREGATE: this selection's mechanical gates pass")
        for name in ['toolchain.txt', 'cargo-version.txt']:
            (self.root / name).write_text('identified')
        names = ['local_clone::tests::lifecycle', 'workspace_ops::tests::g08::publication']
        (self.root / 'core-inventory.json').write_text(json.dumps({
            'execution_checked': True, 'executed_partition': 'all', 'tests': names,
            'ignored': [], 'execution': {'executed': names, 'ignored': []},
        }))

    def test_complete_evidence_passes(self):
        evidence.check(self.root)

    def test_unexecuted_inventory_refuses(self):
        path = self.root / 'core-inventory.json'
        report = json.loads(path.read_text())
        report['execution_checked'] = False
        path.write_text(json.dumps(report))
        with self.assertRaises(ValueError): evidence.check(self.root)

    def test_omitted_inventory_test_refuses(self):
        path = self.root / 'core-inventory.json'
        report = json.loads(path.read_text())
        report['execution']['executed'].pop()
        path.write_text(json.dumps(report))
        with self.assertRaises(ValueError): evidence.check(self.root)

    def test_zero_test_command_refuses(self):
        (self.root / 'publication.log').write_text('test result: ok. 0 passed; 0 failed;')
        with self.assertRaises(ValueError): evidence.check(self.root)

    def test_full_cli_allows_only_known_empty_harnesses(self):
        log = ("Running unittests src/lib.rs (target/debug/deps/gwz-123)\n"
               "test result: ok. 178 passed; 0 failed;\n"
               "Running unittests src/main.rs (target/debug/deps/gwz-456)\n"
               "test result: ok. 0 passed; 0 failed;\n"
               "Doc-tests gwz\n"
               "test result: ok. 0 passed; 0 failed;\n")
        (self.root / 'rust-driver.log').write_text(log)
        evidence.check(self.root)
        for bad in [log.replace('178 passed', '0 passed'),
                    log.replace('src/main.rs', 'tests/local_workflows.rs'),
                    log.replace('0 passed; 0 failed', '0 passed; 1 failed')]:
            (self.root / 'rust-driver.log').write_text(bad)
            with self.assertRaises(ValueError): evidence.check(self.root)

    def test_missing_required_command_refuses(self):
        (self.root / 'local-clone.log').unlink()
        with self.assertRaises(FileNotFoundError): evidence.check(self.root)

    def test_missing_protocol_execution_refuses(self):
        (self.root / 'protocol.log').unlink()
        with self.assertRaises(FileNotFoundError): evidence.check(self.root)

    def test_failed_python_suite_refuses(self):
        (self.root / 'python-driver.log').write_text('1 failed, 5 passed')
        with self.assertRaises(ValueError): evidence.check(self.root)

if __name__ == '__main__': unittest.main()
