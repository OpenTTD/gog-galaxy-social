import json
import hashlib
import os
import sys

from cryptography.hazmat.primitives.asymmetric import ed25519

private_key_raw = os.getenv("OPENTTD_PLUGIN_PRIVATE_KEY")

if not private_key_raw:
    sys.stderr.write("Warning: OPENTTD_PLUGIN_PRIVATE_KEY not set; cannot sign signature file\n")
else:
    private_key = ed25519.Ed25519PrivateKey.from_private_bytes(bytes.fromhex(private_key_raw))

content = {
    "files": [],
    "signature": "",
}

filenames = sys.argv[1:]
# CMake delivers the files as a single argument, separated by semicolons.
if len(filenames) == 1 and ";" in filenames[0]:
    filenames = filenames[0].split(";")

for filename in filenames:
    with open(filename, "rb") as f:
        data = f.read()
        checksum = hashlib.blake2b(data, digest_size=32)

    content["files"].append({
        "filename": os.path.basename(filename),
        "checksum": "1$" + checksum.hexdigest().upper(),
    })

if private_key_raw:
    signature = private_key.sign(json.dumps(content["files"], separators=(",", ":"), sort_keys=True).encode("utf-8"))
    content["signature"] = "1$" + signature.hex().upper()

print(json.dumps(content, indent=4))
