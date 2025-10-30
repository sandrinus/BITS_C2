#!/usr/bin/python3

import os
import cmd

BASE_DIR = "./clients"

class TeamCLI(cmd.Cmd):
    intro = "Team deployment and management CLI. Type 'help' or '?' to list commands."
    prompt = "team-cli> "

    #region ---------------- Core commands ----------------
    def do_deploy(self, arg):
        """deploy <num_teams> <file1,file2,...>  - Create team folders and add files"""
        try:
            parts = arg.strip().split()
            if len(parts) < 2:
                print("Usage: deploy <num_teams> <file1,file2,...>")
                return
            num_teams = int(parts[0])
            file_names = parts[1].split(",")

            if not os.path.exists(BASE_DIR):
                os.makedirs(BASE_DIR)

            for i in range(1, num_teams + 1):
                team_name = f"team{i:02d}"
                team_path = os.path.join(BASE_DIR, team_name)
                os.makedirs(team_path, exist_ok=True)
                for file_name in file_names:
                    file_name = file_name.strip()
                    if file_name:
                        file_path = os.path.join(team_path, file_name)
                        if not os.path.exists(file_path):
                            with open(file_path, "w"):
                                pass
            print(f"Created {num_teams} teams with {len(file_names)} files each.")
        except ValueError:
            print("Invalid number of teams.")

    def do_list(self, arg):
        """list - Show all existing teams"""
        teams = sorted([d for d in os.listdir(BASE_DIR) if os.path.isdir(os.path.join(BASE_DIR, d))]) if os.path.exists(BASE_DIR) else []
        for i, t in enumerate(teams, 1):
            print(f"{i}. {t}")
        if not teams:
            print("No teams found.")

    def do_file(self, arg):
        """
        file <team> <filename(s)|*> - Read last command(s) run
        Examples:
        file team01 file1.txt              # show one file
        file team01 file1.txt,file2.ps1    # show multiple files
        file team01 *                      # show all files in the team folder
        """
        parts = arg.strip().split(maxsplit=1)
        if len(parts) != 2:
            print("Usage: file <team> <filename(s)|*>")
            return

        team, file_arg = parts
        team_path = os.path.join(BASE_DIR, team)

        if not os.path.isdir(team_path):
            print(f"Team '{team}' not found.")
            return

        # Determine which files to read
        if file_arg.strip() == "*":
            file_list = [f for f in os.listdir(team_path) if os.path.isfile(os.path.join(team_path, f))]
        else:
            file_list = [f.strip() for f in file_arg.split(",") if f.strip()]

        if not file_list:
            print(f"No files found in {team}.")
            return

        for filename in file_list:
            file_path = os.path.join(team_path, filename)
            if not os.path.isfile(file_path):
                print(f"⚠️  File '{filename}' not found in team '{team}'.")
                continue

            print(f"\n=== {team}/{filename} ===")
            try:
                with open(file_path, "r", encoding="utf-8", errors="replace") as f:
                    content = f.read().strip()
                    if content:
                        print(content)
                    else:
                        print("(empty file)")
            except Exception as e:
                print(f"Error reading {filename}: {e}")

    def do_write(self, arg):
        """write <team(s)|*> <filename(s)|*> <powershell|cmd|download> <command>
        Overwrite file(s) in one, multiple, or all teams with the specified command.

        Team syntax:
        - Single: team01
        - Multiple: team01,team03
        - Range: team01-03   (expands to team01, team02, team03)
        - Combined: team01,team05-07
        - All: *

        File syntax:
        - Single: file1.txt
        - Multiple: file1.txt,file2.ps1
        - All: *

        Examples:
        write team01 file1.txt powershell Get-Process
        write team01,team02 file1.txt,file2.ps1 cmd echo test
        write team01-03 * powershell whoami
        write * * cmd echo Hello
        """
        parts = arg.strip().split(maxsplit=3)
        if len(parts) < 4:
            print("Usage: write <team(s)|*> <filename(s)|*> <powershell|cmd|download> <command>")
            return

        teams_arg, files_arg, mode, command = parts

        # Validate mode
        if mode not in ["powershell", "cmd", "download"]:
            print("Mode must be 'powershell', 'cmd', or 'download'.")
            return

        command = command.replace("\"", "\\\"")
        # Prefix actual execution wrapper
        if mode == "powershell":
            command = f"powershell.exe -Command \"{command}\""
        elif mode == "cmd":
            command = f"cmd.exe /c \"{command}\""
        elif mode == "download":
            command = f"download {command}"

        # --- Expand team targets ---
        def expand_teams(arg_str):
            if arg_str.strip() == "*":
                return [t for t in os.listdir(BASE_DIR)
                        if os.path.isdir(os.path.join(BASE_DIR, t))]

            result = []
            for part in [p.strip() for p in arg_str.split(",") if p.strip()]:
                if "-" in part:
                    # Example: team05-07 → prefix=team, start=5, end=7
                    prefix = ''.join(ch for ch in part if not ch.isdigit() and ch != '-')
                    nums = ''.join(ch if ch.isdigit() or ch == '-' else '' for ch in part).split('-')
                    if len(nums) == 2:
                        start, end = int(nums[0]), int(nums[1])
                        for n in range(start, end + 1):
                            result.append(f"{prefix}{n:02d}")
                    else:
                        print(f"Invalid range format: {part}")
                else:
                    result.append(part)
            return result

        teams = expand_teams(teams_arg)
        total_modified = 0

        for team in teams:
            team_path = os.path.join(BASE_DIR, team)
            if not os.path.isdir(team_path):
                print(f"[WARN] Team not found: {team}")
                continue

            # --- Determine target files ---
            if files_arg == "*":
                target_files = [f for f in os.listdir(team_path)
                                if os.path.isfile(os.path.join(team_path, f))]
            else:
                target_files = [f.strip() for f in files_arg.split(",") if f.strip()]

            modified_count = 0
            for filename in target_files:
                file_path = os.path.join(team_path, filename)
                if not os.path.exists(file_path):
                    print(f"[WARN] Missing file: {team}/{filename}")
                    continue
                with open(file_path, "w") as f:
                    f.write(command)
                print(f"[OK] {team}/{filename}")
                modified_count += 1

            print(f"Modified {modified_count} file(s) in {team}.")
            total_modified += modified_count

        print(f"\nTotal modified files: {total_modified}")




    def do_exit(self, arg):
        """exit - Quit the CLI"""
        print("Exiting...")
        return True

    def do_EOF(self, arg):
        """Ctrl+D to exit"""
        print()
        return True

#region Main
if __name__ == "__main__":
    TeamCLI().cmdloop()
