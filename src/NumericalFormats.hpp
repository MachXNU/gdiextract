#pragma once
#include <cstdint>

inline uint16_t readLE16(const uint8_t* p) {
    return (uint16_t)p[0] |
           ((uint16_t)p[1] << 8);
}

inline uint16_t readBE16(const uint8_t* p) {
    return ((uint16_t)p[0] << 8) |
           (uint16_t)p[1];
}

inline uint32_t readLE32(const uint8_t* p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

inline uint32_t readBE32(const uint8_t* p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) |
           (uint32_t)p[3];
}