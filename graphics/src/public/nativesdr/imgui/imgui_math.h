//
// Created by Elec332 on 19/11/2021.
//

#ifndef NATIVESDR_IMGUI_MATH_H
#define NATIVESDR_IMGUI_MATH_H

#ifndef IMGUI_VERSION
#error Must include imgui.h before imgui_math.h
#endif

#ifndef IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) {
    return {lhs.x * rhs, lhs.y * rhs};
}

static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) {
    return {lhs.x / rhs, lhs.y / rhs};
}

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}

static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}

static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

#endif

static inline ImVec2 operator+(const ImVec2& lhs, float rhs) {
    return {lhs.x + rhs, lhs.y + rhs};
}

static inline ImVec2 operator-(const ImVec2& lhs, float rhs) {
    return {lhs.x - rhs, lhs.y - rhs};
}

static inline ImVec2& operator+=(ImVec2& lhs, float rhs) {
    lhs.x += rhs;
    lhs.y += rhs;
    return lhs;
}

static inline ImVec2& operator-=(ImVec2& lhs, float rhs) {
    lhs.x -= rhs;
    lhs.y -= rhs;
    return lhs;
}

#endif //NATIVESDR_IMGUI_MATH_H
