#!/usr/bin/python3

import os

# Set the base directory containing team folders
BASE_DIR = "./clients"

def deploy_teams():
    """Create a specified number of team directories and add specified files."""
    try:
        num_teams = int(input("Enter the number of teams to create: ").strip())
        file_names = input("Enter file names (comma-separated): ").strip().split(",")

        if not os.path.exists(BASE_DIR):
            os.makedirs(BASE_DIR)

        for i in range(1, num_teams + 1):
            team_name = f"team{i:02d}"  # Format as team01, team02, etc.
            team_path = os.path.join(BASE_DIR, team_name)

            os.makedirs(team_path, exist_ok=True)  # Create team directory if it doesn't exist

            for file_name in file_names:
                file_name = file_name.strip()  # Remove extra spaces
                if file_name:
                    file_path = os.path.join(team_path, file_name)
                    if not os.path.exists(file_path):
                        with open(file_path, "w") as f:
                            pass  # Create an empty file

        print(f"Created {num_teams} teams with {len(file_names)} files in each folder.")

    except ValueError:
        print("Invalid input. Please enter a valid number of teams.")

def list_teams():
    """List all available teams in the clients folder."""
    teams = sorted([d for d in os.listdir(BASE_DIR) if os.path.isdir(os.path.join(BASE_DIR, d))])
    
    print("\nAvailable Teams:")
    for idx, team in enumerate(teams, start=1):
        print(f"{idx}. {team}")
    
    return teams

def select_team():
    """Prompt the user to select a team from the list."""
    teams = list_teams()
    if not teams:
        print("No teams available.")
        return None

    while True:
        try:
            choice = int(input("Enter the number of the team: ").strip())
            if 1 <= choice <= len(teams):
                return teams[choice - 1]
            else:
                print("Invalid choice. Please select a valid team number.")
        except ValueError:
            print("Invalid input. Please enter a number.")

def list_files_in_team(team):
    """List files in a specific team folder."""
    team_path = os.path.join(BASE_DIR, team)
    if not os.path.exists(team_path):
        print(f"Team '{team}' not found!")
        return []

    files = os.listdir(team_path)
    
    print(f"\nFiles in {team}:")
    for idx, file in enumerate(files, start=1):
        print(f"{idx}. {file}")

    return files

def select_file(team):
    """Prompt the user to select a file from the list."""
    files = list_files_in_team(team)
    if not files:
        return None

    while True:
        try:
            choice = int(input("Enter the number of the file: ").strip())
            if 1 <= choice <= len(files):
                return files[choice - 1]
            else:
                print("Invalid choice. Please select a valid file number.")
        except ValueError:
            print("Invalid input. Please enter a number.")

def modify_file(file_path, command):
    """Modify a file by writing a command into it."""
    with open(file_path, "w") as f:
        f.write(command)
    print(f"Updated: {file_path}")

def modify_specific_files():
    """Modify specific files in a selected team."""
    team = select_team()
    if not team:
        return

    files = list_files_in_team(team)
    if not files:
        return

    choices = input("Enter file numbers to modify (comma-separated): ").strip().split(",")
    command = input("Enter the command to write: ")

    for choice in choices:
        if choice.isdigit() and 1 <= int(choice) <= len(files):
            modify_file(os.path.join(BASE_DIR, team, files[int(choice) - 1]), command)
        else:
            print(f"Invalid choice: {choice}")

def modify_same_file_in_multiple_teams():
    """Modify a specific file in multiple teams."""
    teams = list_teams()
    if not teams:
        return

    choices = input("Enter team numbers (comma-separated): ").strip().split(",")

    # Use the first selected team to list available files
    first_team = teams[int(choices[0]) - 1] if choices[0].isdigit() and 1 <= int(choices[0]) <= len(teams) else None
    if not first_team:
        print("Invalid selection.")
        return

    filename = select_file(first_team)
    if not filename:
        return

    command = input("Enter the command to write: ")

    for choice in choices:
        if choice.isdigit() and 1 <= int(choice) <= len(teams):
            team = teams[int(choice) - 1]
            file_path = os.path.join(BASE_DIR, team, filename)
            if os.path.exists(file_path):
                modify_file(file_path, command)
            else:
                print(f"File '{filename}' not found in {team}")

def modify_all_files():
    """Modify all files in all teams."""
    command = input("Enter the command to write: ")
    for team in list_teams():
        team_path = os.path.join(BASE_DIR, team)
        for file in os.listdir(team_path):
            modify_file(os.path.join(team_path, file), command)

def main():
    while True:
        print("\nMenu:")
        print("1. Deploy teams")
        print("2. List all teams")
        print("3. Modify specific files in a team")
        print("4. Modify a file in multiple teams")
        print("5. Modify all files in all teams")
        print("6. Exit")

        choice = input("Enter your choice: ").strip()

        match choice:
            case "1":
                deploy_teams()
            case "2":
                list_teams()
            case "3":
                modify_specific_files()
            case "4":
                modify_same_file_in_multiple_teams()
            case "5":
                modify_all_files()
            case "6":
                print("Exiting...")
                break
            case _:
                print("Invalid choice! Please try again.")

if __name__ == "__main__":
    main()
