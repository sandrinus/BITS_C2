#include "bits_operations.h"
#include "file_operations.h"
#include <windows.h>

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

// Function to generate a random filename like 'ksfjs32dj4545kf.txt'


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
    // Check if the user provided the remote URL and local directory as parameters
    if (argc < 3) {
        wprintf(L"Usage: <program> <remoteUrl> <localDirectory>\n");
        return 1;  // Return an error if the required parameters are missing
    }

    std::wstring remoteUrl = argv[1];  // Use the first command-line argument as the remote URL
    std::wstring localDir = argv[2];   // Use the second command-line argument as the local directory

    // Ensure the local directory path ends with a backslash (if not already)
    if (localDir.back() != L'\\') {
        localDir += L'\\';
    }

    std::wstring localFile = localDir + GenerateRandomFilename();  // Append random filename to the directory path

    wprintf(L"Using URL: %s\n", remoteUrl.c_str());
    wprintf(L"Generated local file: %s\n", localFile.c_str());

    // Simulate a repeated download every minute
    while (true) {
        CoInitialize(nullptr);
        DownloadFile(remoteUrl, localFile);  // Download the file with the generated filename
        CoUninitialize();
        Sleep(60000);  // Wait for 60 seconds before downloading again
    }

    return 0;
}
