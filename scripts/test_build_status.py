import importlib.util
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location("build_status", Path(__file__).with_name("build-status.py"))
status = importlib.util.module_from_spec(spec)
spec.loader.exec_module(status)

class BuildStatusTests(unittest.TestCase):
    def test_dirty_source_edits_change_identity_again(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "src").mkdir()
            source = root / "src/lib.rs"
            source.write_text("first")
            first = status.source_digest(root)
            source.write_text("second")
            second = status.source_digest(root)
            source.write_text("third")
            self.assertEqual(3, len({first, second, status.source_digest(root)}))

    def test_local_library_sources_participate(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "crates/member/src/lib.rs"
            source.parent.mkdir(parents=True)
            source.write_text("first")
            first = status.source_digest(root)
            source.write_text("second")
            self.assertNotEqual(first, status.source_digest(root))

    def test_generated_targets_do_not_change_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first = status.source_digest(root)
            (root / "target").mkdir()
            (root / "target/generated").write_text("output")
            self.assertEqual(first, status.source_digest(root))

    def test_archive_reports_unavailable_revision(self):
        with tempfile.TemporaryDirectory() as directory:
            identity = status.identity(Path(directory))
            self.assertIn("revision=unavailable dirty=unknown", identity)

if __name__ == '__main__': unittest.main()
