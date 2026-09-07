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
        for name in ['local-clone.log', 'publication.log', 'rust-driver.log']:
            (self.root / name).write_text('test result: ok. 1 passed; 0 failed; 0 ignored;\n')
        (self.root / 'python-driver.log').write_text('5 passed in 1s')
        (self.root / 'compatibility.log').write_text("AGGREGATE: this selection's mechanical gates pass")
        for name in ['toolchain.txt', 'cargo-version.txt']:
            (self.root / name).write_text('identified')

    def test_complete_evidence_passes(self):
        evidence.check(self.root)

    def test_zero_test_command_refuses(self):
        (self.root / 'publication.log').write_text('test result: ok. 0 passed; 0 failed;')
        with self.assertRaises(ValueError): evidence.check(self.root)

    def test_missing_required_command_refuses(self):
        (self.root / 'local-clone.log').unlink()
        with self.assertRaises(FileNotFoundError): evidence.check(self.root)

    def test_failed_python_suite_refuses(self):
        (self.root / 'python-driver.log').write_text('1 failed, 5 passed')
        with self.assertRaises(ValueError): evidence.check(self.root)

if __name__ == '__main__': unittest.main()
