#pragma once
#include <string>

bool DownloadFile(const std::wstring& remoteUrl, const std::wstring& localFile);
bool UploadFile(const std::wstring& localFile, const std::wstring& remoteUrl);