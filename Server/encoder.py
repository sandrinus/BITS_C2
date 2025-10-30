# encoder_base64.py
# Produces Base64 strings compatible with decoder_base64.cpp

import base64

def encode_base64_list(strings):
    """
    Takes a list of normal URLs/strings and returns Base64 ASCII strings
    that can be pasted into your C++ encodedURLs array.
    """
    out = []
    for s in strings:
        b64 = base64.b64encode(s.encode("utf-8")).decode("ascii")
        out.append(b64)
    return out


if __name__ == "__main__":
    urls = [
        "http://192.168.145.158:8080/windowsupdate/v10/handlers/secure/enroll/mssecure/download/update/",
        # add more URLs here if you need
    ]

    for encoded in encode_base64_list(urls):
        print(f'"{encoded}",')  # ready to paste into C++ source
