import subprocess
import sys
from pathlib import Path

script = Path.home() / ".claude" / "skills" / "chrome-mcp-connector" / "scripts" / "ensure_hub.py"
print("running", script)
r = subprocess.run([sys.executable, str(script)], capture_output=False)
sys.exit(r.returncode)
