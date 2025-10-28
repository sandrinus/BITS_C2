#!/usr/bin/env python3
import os
import subprocess
import argparse

def run(cmd):
    print(f"> {cmd}")
    res = subprocess.run(cmd, shell=True)
    if res.returncode != 0:
        raise SystemExit(f"Command failed: {cmd}")

def setup_server(share_dir, client_network):
    os.makedirs(share_dir, exist_ok=True)
    os.chmod(share_dir, 0o777)  # adjust permissions for your needs

    run("apt update")
    run("apt install nfs-kernel-server -y")

    # Append export line if not exists
    export_line = f"{share_dir} {client_network}(rw,sync,no_subtree_check)"
    with open("/etc/exports", "r+") as f:
        lines = f.read().splitlines()
        if export_line not in lines:
            f.write(export_line + "\n")
            print(f"Added export: {export_line}")

    run("exportfs -ra")
    run("systemctl restart nfs-kernel-server")
    print(f"✅ NFS share {share_dir} exported for {client_network}")

def setup_client(server_ip, remote_share, mount_point):
    os.makedirs(mount_point, exist_ok=True)

    run("apt install nfs-common -y")

    # Mount once
    run(f"mount {server_ip}:{remote_share} {mount_point}")

    # Add to fstab if not already present
    fstab_line = f"{server_ip}:{remote_share} {mount_point} nfs defaults 0 0"
    with open("/etc/fstab", "r+") as f:
        lines = f.read().splitlines()
        if fstab_line not in lines:
            f.write(fstab_line + "\n")
            print(f"Added fstab entry: {fstab_line}")

    print(f"✅ NFS share {remote_share} mounted on {mount_point}")

def main():
    parser = argparse.ArgumentParser(description="Simple NFS deploy script")
    parser.add_argument("--mode", choices=["server", "client"], required=True, help="Deploy as server or client")

    # Server args
    parser.add_argument("--share", help="Directory to share (server mode)")
    parser.add_argument("--network", help="Client network to allow (server mode)")

    # Client args
    parser.add_argument("--server-ip", help="NFS server IP (client mode)")
    parser.add_argument("--remote-share", help="Remote share path on server (client mode)")
    parser.add_argument("--mount", help="Local mount point (client mode)")

    args = parser.parse_args()

    if os.geteuid() != 0:
        raise SystemExit("This script must be run as root (use sudo)")

    if args.mode == "server":
        if not args.share or not args.network:
            raise SystemExit("Server mode requires --share and --network")
        setup_server(args.share, args.network)
    elif args.mode == "client":
        if not args.server_ip or not args.remote_share or not args.mount:
            raise SystemExit("Client mode requires --server-ip, --remote-share, --mount")
        setup_client(args.server_ip, args.remote_share, args.mount)

if __name__ == "__main__":
    main()
