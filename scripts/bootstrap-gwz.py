#!/usr/bin/env python3
"""Install the verified v0.14.0 bootstrap into an explicitly supplied directory."""
import hashlib
import io
import os
from pathlib import Path
import platform
import sys
import tarfile
import urllib.request

ASSETS = {
    ("Darwin", "arm64"): ("aarch64-apple-darwin", "6ac604a943d21a8f4ea314630eed00c08e370afceaf8b59b711563a2dd493ba5"),
    ("Darwin", "x86_64"): ("x86_64-apple-darwin", "0507b28c4a6dfbbea522762e9f20ea78967cdc5baf5a77bbc3202474a6eab39e"),
    ("Linux", "x86_64"): ("x86_64-unknown-linux-gnu", "caae78ff7bb2029203607cc13002375a987a87cf043d0cf8af898eb0ab407d94"),
    ("Linux", "aarch64"): ("aarch64-unknown-linux-gnu", "ec9be651de4ca96bcf45b1d0f59749cd5a6355883abce005926da1c2b1e248dd"),
}


def main():
    triple, expected = ASSETS[(platform.system(), platform.machine())]
    url = f"https://github.com/owebeeone/gwz-cli/releases/download/v0.14.0/gwz-{triple}.tar.xz"
    data = urllib.request.urlopen(url, timeout=60).read()
    if hashlib.sha256(data).hexdigest() != expected:
        raise ValueError("bootstrap release archive digest mismatch")
    destination = Path(sys.argv[1]).resolve()
    destination.mkdir(parents=True, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(data), mode="r:xz") as archive:
        matches = [item for item in archive if item.isfile() and Path(item.name).name == "gwz"]
        if len(matches) != 1:
            raise ValueError("bootstrap archive must contain exactly one executable")
        content = archive.extractfile(matches[0]).read()
    executable = destination / "gwz"
    executable.write_bytes(content)
    executable.chmod(0o755)
    print(executable)


if __name__ == "__main__": main()
