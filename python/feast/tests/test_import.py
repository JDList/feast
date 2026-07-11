import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root))

import feast

assert feast.canonical_face_names() == ("lowx", "highx", "lowy", "highy", "lowz", "highz")
print("import ok")
