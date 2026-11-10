#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace utf8 {

constexpr uint32_t REPLACEMENT_CHAR = 0xFFFD;
constexpr uint32_t MAX_CODE_POINT = 0x10FFFF;

struct Decoded {
    uint32_t codePoint;
    size_t length;
    bool valid;
};

inline bool isContinuation(unsigned char b) {
    return (b & 0xC0) == 0x80;
}

inline bool isValidScalar(uint32_t cp) {
    return cp <= MAX_CODE_POINT && !(cp >= 0xD800 && cp <= 0xDFFF);
}

inline size_t sequenceLength(unsigned char lead) {
    if (lead < 0x80) return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;
}

inline Decoded decodeAt(const char* data, size_t size, size_t pos) {
    unsigned char lead = static_cast<unsigned char>(data[pos]);
    if (lead < 0x80) return {lead, 1, true};

    size_t need = sequenceLength(lead);
    if (need == 1 || pos + need > size) return {REPLACEMENT_CHAR, 1, false};

    uint32_t cp = (need == 2) ? (lead & 0x1F) : (need == 3) ? (lead & 0x0F) : (lead & 0x07);
    for (size_t i = 1; i < need; ++i) {
        unsigned char b = static_cast<unsigned char>(data[pos + i]);
        if (!isContinuation(b)) return {REPLACEMENT_CHAR, 1, false};
        cp = (cp << 6) | (b & 0x3F);
    }

    uint32_t minValue = (need == 2) ? 0x80 : (need == 3) ? 0x800 : 0x10000;
    if (cp < minValue || !isValidScalar(cp)) return {REPLACEMENT_CHAR, 1, false};
    return {cp, need, true};
}

inline Decoded decodeAt(const std::string& s, size_t pos) {
    return decodeAt(s.data(), s.size(), pos);
}

inline void encode(uint32_t cp, std::string& out) {
    if (!isValidScalar(cp)) cp = REPLACEMENT_CHAR;
    if (cp < 0x80) {
        out += static_cast<char>(cp);
    } else if (cp < 0x800) {
        out += static_cast<char>(0xC0 | (cp >> 6));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        out += static_cast<char>(0xF0 | (cp >> 18));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

inline std::string encode(uint32_t cp) {
    std::string out;
    encode(cp, out);
    return out;
}

inline size_t countCodePoints(const char* data, size_t size) {
    size_t count = 0;
    size_t pos = 0;
    while (pos < size) {
        pos += decodeAt(data, size, pos).length;
        count++;
    }
    return count;
}

inline size_t countCodePoints(const std::string& s) {
    return countCodePoints(s.data(), s.size());
}

inline size_t countCodePoints(const std::string& s, size_t from, size_t to) {
    if (to > s.size()) to = s.size();
    if (from >= to) return 0;
    return countCodePoints(s.data() + from, to - from);
}

inline size_t byteOffsetOf(const char* data, size_t size, size_t index) {
    size_t pos = 0;
    size_t count = 0;
    while (pos < size && count < index) {
        pos += decodeAt(data, size, pos).length;
        count++;
    }
    return pos;
}

inline bool isIdentifierCodePoint(uint32_t cp) {
    if (cp < 0x80) return false;
    if (cp < 0xC0) return cp == 0xAA || cp == 0xB5 || cp == 0xBA;
    if (cp == 0xD7 || cp == 0xF7) return false;
    if (cp == 0x200C || cp == 0x200D) return true;
    if (cp >= 0x2000 && cp <= 0x2BFF) return false;
    if (cp >= 0x3000 && cp <= 0x303F) return cp == 0x3005 || cp == 0x3006;
    if (cp >= 0xE000 && cp <= 0xF8FF) return false;
    if (cp >= 0xFE00 && cp <= 0xFE0F) return false;
    if (cp == 0xFEFF || cp == 0xFFFD) return false;
    if (cp >= 0xFFF0 && cp <= 0xFFFF) return false;
    if (cp >= 0x1F000 && cp <= 0x1FAFF) return false;
    return isValidScalar(cp);
}

}