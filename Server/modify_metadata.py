# Open the .tmp file and append metadata (in this case, a comment)
cvg_file_path = "D:\\BITS test\\Server\\updates\\c7d5fbc1-2f27-4f0b-b1bd-0c6dae457414.cgv"

while True:
    action = input("Press 1 to enter a command \nPress 2 to exit\nChoice: ")
    match action.strip(): 
        case "1":
            command = input("Enter a cmd command: ")
            with open(cvg_file_path, "w") as cvg_file:
                cvg_file.write(command)
        case "2":
            print("Goodbye!")
            break
