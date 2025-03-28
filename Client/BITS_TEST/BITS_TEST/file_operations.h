#pragma once
#include <string>

std::wstring GenerateRandomFilename();
std::wstring readCommentFromFile(const std::wstring& filePath);
void ExecuteCommandFromFile(const std::wstring& localFile);
bool IsDownloadCommand(const std::wstring& command);
