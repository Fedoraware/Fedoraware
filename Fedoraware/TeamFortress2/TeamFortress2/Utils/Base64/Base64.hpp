/*
* Base64 encoding/decoding (RFC1341)
* Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

// 2016-12-12 - Gaspard Petit : Slightly modified to return a std::string 
// instead of a buffer allocated with malloc.

#pragma once
#include <string>

namespace Base64
{
    static constexpr unsigned char base64_table[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /**
    * Encodes the given data to Base64
    * @src: Data to be encoded
    * @len: Length of the data to be encoded
    * Returns: String with the encoded Base64 data
    */
    inline std::string Encode(const unsigned char* src, size_t len)
    {
	    const size_t olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

        if (olen < len) {
            return {}; /* integer overflow */
        }

        std::string outStr;
        outStr.resize(olen);
	    auto* out = reinterpret_cast<unsigned char*>(&outStr[0]);

        const unsigned char* end = src + len;
        const unsigned char* in = src;
        unsigned char* pos = out;
        while (end - in >= 3) {
            *pos++ = base64_table[in[0] >> 2];
            *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
            *pos++ = base64_table[in[2] & 0x3f];
            in += 3;
        }

        if (end - in) {
            *pos++ = base64_table[in[0] >> 2];
            if (end - in == 1) {
                *pos++ = base64_table[(in[0] & 0x03) << 4];
                *pos++ = '=';
            }
            else {
                *pos++ = base64_table[((in[0] & 0x03) << 4) |
                    (in[1] >> 4)];
                *pos++ = base64_table[(in[1] & 0x0f) << 2];
            }
            *pos++ = '=';
        }

        return outStr;
    }

    inline std::string Encode(const std::string& pSource)
    {
        return Encode(reinterpret_cast<const unsigned char*>(pSource.c_str()), pSource.length());
    }

    static const int B64index[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
    7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
    0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

    /**
    * Decodes the given data from Base64
    * @data: Data to be decoded
    * @len: Length of the data to be encoded
    * Returns: String with the decoded Base64 data
    */
    inline std::string Decode(const void* data, const size_t len)
    {
	    const auto* p = (unsigned char*)data;
	    const int pad = len > 0 && (len % 4 || p[len - 1] == '=');
        const size_t L = ((len + 3) / 4 - pad) * 4;
        std::string str(L / 4 * 3 + pad, '\0');

        for (size_t i = 0, j = 0; i < L; i += 4)
        {
            int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
            str[j++] = n >> 16;
            str[j++] = n >> 8 & 0xFF;
            str[j++] = n & 0xFF;
        }
        if (pad)
        {
            int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
            str[str.size() - 1] = n >> 16;

            if (len > L + 2 && p[L + 2] != '=')
            {
                n |= B64index[p[L + 2]] << 6;
                str.push_back(n >> 8 & 0xFF);
            }
        }
        return str;
    }

    inline std::string Decode(const std::string& pData)
    {
        return Decode(pData.c_str(), pData.length());
    }
}