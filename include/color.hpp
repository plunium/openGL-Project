#ifndef CLEAR_COLOR_HPP
#define CLEAR_COLOR_HPP

#include <GL/gl.h>

struct Color {
    float r, g, b, a;

    constexpr Color(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
};

#endif