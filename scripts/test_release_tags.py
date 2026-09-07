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
