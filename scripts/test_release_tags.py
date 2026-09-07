"""Both release entry points must accept RC tags before performing any effects."""
import importlib.util
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class ReleaseTags(unittest.TestCase):
    def test_release_tag_validation(self):
        for member in ('gwz-core', 'gwz-cli'):
            spec = importlib.util.spec_from_file_location(member, ROOT/member/'scripts/release.py')
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            for tag in ('v1.0.0', 'v1.0.0-rc.1', 'v1.0.0-rc.12'):
                with self.subTest(member=member, tag=tag):
                    self.assertEqual(module.release_version(tag), tag[1:])
            for tag in ('v01.0.0', 'v1.0.0-rc.0', 'v1.0.0-rc.01', 'v1.0.0-rc', 'v1.0.0;false', '1.0.0'):
                with self.subTest(member=member, tag=tag), self.assertRaises(SystemExit):
                    module.release_version(tag)

    def test_cli_release_updates_bazel_versions_with_cargo(self):
        import tempfile
        spec = importlib.util.spec_from_file_location('cli_release', ROOT/'gwz-cli/scripts/release.py')
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root/'Cargo.toml').write_text('version = "0.2.0-dev"\ngwz-core = { git = "https://example.invalid/core", tag = "v0.14.0" }\n')
            (root/'BUILD.bazel').write_text('rust_library(\n    version = "0.2.0-dev",\n)\nrust_binary(\n    version = "0.2.0-dev",\n)\n')
            self.assertTrue(module.reconcile_cargo_toml(root, 'v1.0.0-rc.1', '1.0.0-rc.1'))
            self.assertEqual((root/'BUILD.bazel').read_text().count('version = "1.0.0-rc.1"'), 2)
            self.assertFalse(module.reconcile_cargo_toml(root, 'v1.0.0-rc.1', '1.0.0-rc.1'))
