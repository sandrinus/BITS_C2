#include "bits_operations.h"
#include "file_operations.h"
#include <windows.h>
#include <vector>
#include <iostream>

std::wstring remoteUrl = L"/team01/dns.txt";
std::wstring localDir = L"C:\\Users\\alexm\\Desktop";
std::wstring fullUrl;

std::vector<std::wstring> remoteUrls = {
    L"http://localhost:8080/clients",
    L"http://localhost:8080/clients",
};

int wmain(int argc, wchar_t* argv[]) {
    // Ensure the local directory path ends with a backslash (if not already)
    if (localDir.back() != L'\\') {
        localDir += L'\\';
    }

    std::wstring localFile = localDir + GenerateRandomFilename();  // Append random filename to the directory path

    wprintf(L"Using URL: %ls\n", remoteUrl.c_str());
    wprintf(L"Generated local file: %ls\n", localFile.c_str());

    // Start downloading logic
    while (true) {
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
    return 0;
}
