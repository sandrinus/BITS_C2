#pragma once
#include <string>
#include <vector>

std::wstring Base64DecodeToWString(const std::string& encoded);
std::vector<std::wstring> DecodeBase64List(const std::vector<std::string>& encodedList);
std::wstring Base64DecodeWString(const std::wstring& encoded);