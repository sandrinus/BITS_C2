#pragma once
#include <string>

void DownloadFile(const std::wstring& remoteUrl, const std::wstring& localFile);
void UploadFile(const std::wstring& localFile, const std::wstring& remoteUrl);
