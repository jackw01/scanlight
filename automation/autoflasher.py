#!/usr/bin/env python3

import os
import sys
import time
import shutil
import platform
from pathlib import Path

TARGET_VOLUME_NAME = "RPI-RP2"
POLL_INTERVAL = 0.5 # seconds

handled_mounts = set()

def get_mount_points():
    system = platform.system()

    if system == "Windows":
        return get_windows_mounts()
    elif system == "Darwin":
        return get_macos_mounts()
    else:
        return get_linux_mounts()


def get_windows_mounts():
    import ctypes

    DRIVE_REMOVABLE = 2
    kernel32 = ctypes.windll.kernel32
    mounts = []
    bitmask = kernel32.GetLogicalDrives()

    for i in range(26):
        if bitmask & (1 << i):
            letter = chr(ord('A') + i)
            drive = f"{letter}:\\"

            # Skip non-removable drives
            drive_type = kernel32.GetDriveTypeW(ctypes.c_wchar_p(drive))
            if drive_type != DRIVE_REMOVABLE:
                continue

            volume_name_buffer = ctypes.create_unicode_buffer(256)

            result = kernel32.GetVolumeInformationW(
                ctypes.c_wchar_p(drive),
                volume_name_buffer,
                ctypes.sizeof(volume_name_buffer),
                None,
                None,
                None,
                None,
                0,
            )

            if result:
                volume_name = volume_name_buffer.value
                mounts.append((drive, volume_name))

    return mounts


def get_macos_mounts():
    mounts = []

    volumes_dir = Path("/Volumes")

    if volumes_dir.exists():
        for entry in volumes_dir.iterdir():
            if entry.is_dir():
                mounts.append((str(entry), entry.name))

    return mounts


def get_linux_mounts():
    mounts = []

    possible_dirs = [
        Path("/media"),
        Path("/run/media"),
        Path("/mnt"),
    ]

    for base in possible_dirs:
        if not base.exists():
            continue

        for root, dirs, _ in os.walk(base):
            for d in dirs:
                path = Path(root) / d
                mounts.append((str(path), d))

    return mounts


def copy_file_to_disk(source_file, mount_path):
    destination = Path(mount_path) / Path(source_file).name

    print(f"Copying '{source_file}' to '{destination}'")

    try:
        shutil.copy2(source_file, destination)
        print("Copy complete.")
    except Exception as e:
        print(f"Copy failed: {e}")


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <filename>")
        sys.exit(1)

    source_file = sys.argv[1]

    if not os.path.isfile(source_file):
        print(f"File not found: {source_file}")
        sys.exit(1)

    print(f"Watching for disks named '{TARGET_VOLUME_NAME}'...")
    print("Press Ctrl+C to stop.")

    try:
        while True:
            current_mounts = get_mount_points()

            active_mount_paths = set()

            for mount_path, volume_name in current_mounts:
                active_mount_paths.add(mount_path)

                if volume_name == TARGET_VOLUME_NAME:
                    if mount_path not in handled_mounts:
                        print(f"Detected {TARGET_VOLUME_NAME} at {mount_path}")

                        copy_file_to_disk(source_file, mount_path)

                        handled_mounts.add(mount_path)

            # Remove disconnected drives from handled set
            handled_mounts.intersection_update(active_mount_paths)

            time.sleep(POLL_INTERVAL)

    except KeyboardInterrupt:
        print("\nStopped.")


if __name__ == "__main__":
    main()

