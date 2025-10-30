#include "decoder.h"
#include <windows.h>
#include <vector>

// Minimal Base64 decoding table
static const unsigned char B64DecodeTable[256] = {
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,
    64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,
    64,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
};

static std::string Base64Decode(const std::string& input) {
    std::string out;
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (B64DecodeTable[c] == 64) continue;
        val = (val << 6) + B64DecodeTable[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

static std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), NULL, 0);
    std::wstring wide(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), &wide[0], size);
    return wide;
}

std::wstring Base64DecodeToWString(const std::string& encoded) {
    return Utf8ToWide(Base64Decode(encoded));
}

std::vector<std::wstring> DecodeBase64List(const std::vector<std::string>& encodedList) {
    std::vector<std::wstring> result;
    result.reserve(encodedList.size());
    for (const auto& e : encodedList)
        result.push_back(Base64DecodeToWString(e));
    return result;
}


// Base64 decode: wstring -> wstring
std::wstring Base64DecodeWString(const std::wstring& encoded) {
    std::string narrow;
    narrow.reserve(encoded.size());
    for (wchar_t wc : encoded)
        narrow.push_back(static_cast<char>(wc & 0xFF));

    std::string decoded;
    int val = 0, valb = -8;
    for (unsigned char c : narrow) {
        if (B64DecodeTable[c] == 64) continue;
        val = (val << 6) + B64DecodeTable[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    if (decoded.empty()) return L"";

    // Convert UTF-8 decoded string to wstring
    int size = MultiByteToWideChar(CP_UTF8, 0, decoded.data(), (int)decoded.size(), NULL, 0);
    std::wstring wide(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, decoded.data(), (int)decoded.size(), &wide[0], size);
    return wide;
}