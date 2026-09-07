#!/usr/bin/env python3
"""Opt-in real privileged integration tests; run as the authorized ordinary user.

Creates and destroys only the helper's fixed disposable image. Refuses to start
if an image already exists. Requires passwordless sudo, xfsprogs and e2fsprogs.
The executable is NOT installed setuid; setpriv simulates its real/effective UIDs.
"""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main():
    if len(sys.argv) != 3 or sys.argv[1] != '--disposable-image-tests':
        raise SystemExit('usage: test_helper.py --disposable-image-tests /absolute/binary')
    if os.geteuid() == 0:
        raise SystemExit('Run as the ordinary benchmark user; only helper calls use sudo.')
    binary = str(Path(sys.argv[2]).resolve(strict=True))
    root = Path(f'/var/lib/gwz-fs-bench-{os.getuid()}')
    mount = root / 'mnt'
    image = root / 'image.img'
    passed = []

    def run(*arguments, ok=True, prefix=()):
        result = subprocess.run([*prefix, binary, *arguments], text=True,
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if (result.returncode == 0) != ok:
            raise AssertionError((arguments, result.returncode, result.stdout, result.stderr))
        return result

    def helper(*arguments, ok=True):
        return run(*arguments, ok=ok, prefix=('sudo', '-n'))

    def sudo(*arguments):
        subprocess.run(['sudo', '-n', *map(str, arguments)], check=True)

    def snapshot():
        return json.loads(helper('status').stdout)

    def record(name):
        passed.append(name)
        print('PASS', name, flush=True)

    run('--help')
    run('create', 'ext4', '/dev/does-not-belong-to-the-helper', ok=False)
    run('create', 'btrfs', ok=False)
    run('status', ok=False)
    record('help, invalid grammar and unprivileged refusal')
    run('status', ok=False, prefix=('sudo', '-n', '/usr/bin/setpriv',
        '--ruid=65534', '--euid=0', '--rgid=65534', '--egid=0', '--clear-groups'))
    record('unauthorized real UID refused even with effective UID zero')
    before = snapshot()
    if before['image_exists'] or before['mounted']:
        raise SystemExit('Existing benchmark image/mount found; no test cleanup was attempted.')
    run('status', prefix=('sudo', '-n', '/usr/bin/setpriv',
        f'--ruid={os.getuid()}', '--euid=0', f'--rgid={os.getgid()}', '--egid=0', '--clear-groups'))
    record('authorized setuid-shaped invocation')

    results = {}
    with tempfile.TemporaryDirectory(prefix='gwz-helper-test-') as scratch:
        scratch = Path(scratch)
        poison_hit = scratch / 'poison-hit'
        fake = scratch / 'mkfs.ext4'
        fake.write_text(f'#!/bin/sh\ntouch "{poison_hit}"\nexit 97\n')
        fake.chmod(0o755)
        bad_config = scratch / 'mke2fs.conf'
        bad_config.write_text('this is deliberately not a valid mke2fs configuration\n')
        for filesystem in ('ext4', 'xfs'):
            if filesystem == 'ext4':
                run('create', filesystem, prefix=('sudo', '-n', '/usr/bin/env',
                    f'PATH={scratch}', f'MKE2FS_CONFIG={bad_config}', 'IFS=/'))
                assert not poison_hit.exists()
                record('fixed formatter and scrubbed environment')
            else:
                helper('create', filesystem)
            baseline = snapshot()
            assert baseline['mounted'] and baseline['filesystem'] == filesystem
            assert baseline['image_bytes'] == 10 * 1024**3
            assert mount.stat().st_uid == os.getuid()
            options = subprocess.check_output(
                ['findmnt', '-n', '-o', 'OPTIONS', '-T', str(mount)], text=True).strip().split(',')
            assert {'nodev', 'nosuid', 'noexec', 'noatime'} <= set(options), options
            record(f'{filesystem}: 10 GiB mounted for ordinary user with restrictions')
            source = mount / 'source.bin'
            clone = mount / 'clone.bin'
            source.write_bytes(os.urandom(2 * 1024**2))
            digest = hashlib.sha256(source.read_bytes()).hexdigest()
            helper('create', 'ext4', ok=False)
            assert hashlib.sha256(source.read_bytes()).hexdigest() == digest
            record(f'{filesystem}: existing image cannot be reformatted')
            for operation in (lambda: image.open('rb'), lambda: image.unlink()):
                try:
                    opened = operation()
                except PermissionError:
                    pass
                else:
                    if opened is not None:
                        opened.close()
                    raise AssertionError('ordinary user could access/remove raw image')
            cp = subprocess.run(['cp', '--reflink=always', str(source), str(clone)],
                                capture_output=True, text=True)
            assert (cp.returncode == 0) == (filesystem == 'xfs'), cp.stderr
            if filesystem == 'ext4':
                shutil.copyfile(source, clone)
            after_clone = snapshot()
            assert source.read_bytes() == clone.read_bytes()
            with clone.open('r+b') as file:
                file.write(b'changed-copy' * 4096)
            assert hashlib.sha256(source.read_bytes()).hexdigest() == digest
            after_write = snapshot()
            record(f'{filesystem}: copy independence and expected reflink capability')
            results[filesystem] = {'baseline': baseline, 'after_clone': after_clone,
                                   'after_write': after_write}
            holder = subprocess.Popen(['sleep', '60'], cwd=mount)
            try:
                result = helper('destroy', ok=False)
                assert 'busy' in result.stderr.lower(), result.stderr
                assert snapshot()['mounted']
            finally:
                holder.terminate()
                holder.wait(timeout=5)
            record(f'{filesystem}: busy unmount retains image and mount')
            helper('destroy')
            assert not snapshot()['image_exists'] and not snapshot()['mounted']
            record(f'{filesystem}: unmounted, loop detached and image removed')

        victim = scratch / 'victim'
        victim.write_text('preserve me')
        sudo('/usr/bin/ln', '-s', victim, image)
        try:
            helper('create', 'ext4', ok=False)
            helper('destroy', ok=False)
            assert victim.read_text() == 'preserve me'
        finally:
            sudo('/usr/bin/unlink', image)
        record('image symlink refused without touching its target')
        # Refuse mountpoint redirection and hidden files before allocating an image.
        sudo('/usr/bin/rmdir', mount)
        sudo('/usr/bin/ln', '-s', scratch, mount)
        try:
            helper('create', 'ext4', ok=False)
            helper('destroy', ok=False)
            assert not image.exists() and victim.read_text() == 'preserve me'
        finally:
            sudo('/usr/bin/unlink', mount)
            sudo('/usr/bin/install', '-d', '-o', 'root', '-g', 'root', '-m', '0755', mount)
        record('mountpoint symlink refused without touching target')
        hidden = mount / '.preserve'
        sudo('/usr/bin/install', '-m', '0600', victim, hidden)
        try:
            helper('create', 'ext4', ok=False)
            assert not image.exists()
        finally:
            sudo('/usr/bin/unlink', hidden)
        record('nonempty mountpoint refused before image allocation')
        sudo('/usr/bin/install', '-o', 'root', '-g', 'root', '-m', '0600', '/dev/null', image)
        helper('destroy')
        assert not image.exists()
        record('incomplete zero-length image cleanup')
        hardlink_victim = root / 'test-hardlink-victim'
        sudo('/usr/bin/install', '-o', 'root', '-g', 'root', '-m', '0600', victim, hardlink_victim)
        sudo('/usr/bin/ln', hardlink_victim, image)
        try:
            helper('destroy', ok=False)
        finally:
            sudo('/usr/bin/unlink', image)
            sudo('/usr/bin/unlink', hardlink_victim)
        record('multiply linked image refused')
        sudo('/usr/bin/chmod', '0777', root)
        try:
            helper('status', ok=False)
        finally:
            sudo('/usr/bin/chmod', '0755', root)
        record('writable privileged metadata directory refused')
    print(json.dumps({'passed': len(passed), 'allocation_observations': results}, indent=2))


if __name__ == '__main__':
    main()
