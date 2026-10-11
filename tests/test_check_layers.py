import unittest
import tempfile
import os
import shutil
import subprocess
import sys

class TestCheckLayers(unittest.TestCase):
    def setUp(self):
        self.test_dir = tempfile.mkdtemp()
        self.script_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "scripts", "check_layers.py"))

    def tearDown(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)

    def test_repo_has_zero_violations(self):
        res = subprocess.run([sys.executable, self.script_path], capture_output=True, text=True)
        self.assertEqual(res.returncode, 0, f"Script failed with output: {res.stdout}\n{res.stderr}")
        self.assertIn("SUCCESS", res.stdout)

if __name__ == "__main__":
    unittest.main()
