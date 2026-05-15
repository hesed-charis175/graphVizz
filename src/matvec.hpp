#pragma once
#include <cmath>
#include <iostream>

struct Vec2 {
  float x, y;

  Vec2() : x(0), y(0) {}
  Vec2(float x, float y) : x(x), y(y) {}
  Vec2 operator+(const Vec2 &other) const {
    return Vec2(x + other.x, y + other.y);
  }
  Vec2 operator-(const Vec2 &other) const {
    return Vec2(x - other.x, y - other.y);
  }
  Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
  float length() const { return std::sqrt(x * x + y * y); }

  float distance(const Vec2 &other) const {
    return std::sqrt((x - other.x) * (x - other.x) +
                     (y - other.y) * (y - other.y));
  }
  float dot(const Vec2 &other) const { return x * other.x + y * other.y; }
  Vec2 normalise() {
    float len = length();
    return (len > 0) ? *this * (1.0f / len) : *this;
  }

  void print() const { std::cout << "(" << x << ", " << y << ")\t"; }
};

struct Mat2 {
  float m[2][2];

  Mat2(float a = 1, float b = 0, float c = 0, float d = 1) {
    m[0][0] = a;
    m[0][1] = b;
    m[1][0] = c;
    m[1][1] = d;
  }

  Vec2 operator*(const Vec2 &vec) const {
    return Vec2(m[0][0] * vec.x + m[0][1] * vec.y,
                m[1][0] * vec.x + m[1][1] * vec.y);
  }

  static Mat2 rotate(float angle_deg) {
    float angle_rad = angle_deg * (M_PI / 180.0f);
    return Mat2(std::cos(angle_rad), -std::sin(angle_rad), std::sin(angle_rad),
                std::cos(angle_rad));
  }

  static Mat2 scale(float scale_x, float scale_y) {
    return Mat2(scale_x, 0.0f, 0.0f, scale_y);
  }
};
