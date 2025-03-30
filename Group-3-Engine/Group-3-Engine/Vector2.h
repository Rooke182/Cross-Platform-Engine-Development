#pragma once
#include <iostream>
#include <json.hpp>
#include <cmath>
#include "glm/gtc/type_ptr.hpp"


class Vector2 {
public:
    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}

    // Public member variables
    float x, y;

    // Vector operations
    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    // Vector operations
    Vector2 operator+=(const Vector2& other) const
    {
        Vector2 v;
        v.x += other.x;
        v.y += other.y;
        return v;
    }

    Vector2 operator-(const Vector2& other) const 
    {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const 
    {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const 
    {
        return Vector2(x / scalar, y / scalar);
    }

    bool isEqual(Vector2 other)
    {
        if (other.x == x && other.y == y)
        {
            return true;
        }

        return false;
    }

    float dot(const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

    float magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalized() const
    {
        float mag = magnitude();
        if (mag != 0.0f) {
            return *this * (1.0f / mag);
        }
        return *this;
    }

    Vector2 GetReverse() const
    {
        Vector2 reverse;
        reverse.x = x * -1;
        reverse.y = y * -1;

        return reverse;
    }

    glm::vec2 toGlmVec2() const {
        return glm::vec2(x, y);
    }
    glm::vec3 toGlmVec3() const {
        return glm::vec3(x, y, 0);
    }

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Vector2& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }
};

void to_json(nlohmann::json& json, const Vector2& vec);
void from_json(const nlohmann::json& json, Vector2& vec);