#pragma once

#include "Types.h"
#include <cfloat>


namespace {
const float EPS = 1.0e-4f;
}

/// @class Ray
class Ray {
public:
    /// ray source type
    enum TYPE { NONE, DIFFUSE, SPECULAR, TRANSMISSION };
    /// ray origin
    Vec3f m_origin = Vec3f(0, 0, 0);
    /// ray direction
    Vec3f m_direction = Vec3f(0, 0, 0);
    /// ray source type
    TYPE m_source;
    /// ray inverse direction, for intersection calculation
    Vec3f m_inverse_direction = Vec3f(0, 0, 0);
    /// min and max range control of ray
    float m_tmin, m_tmax;

    /// @brief construction of Ray
    /// @param origin origin of Ray
    /// @param direction direction of Ray
    /// @param source source type of Ray
    explicit Ray(Vec3f origin = Vec3f(0, 0, 0),
                 Vec3f direction = Vec3f(0, 0, 0),
                 TYPE source = TYPE::NONE) {
        m_origin = origin;
        m_direction = direction.normalized();
        m_source = source;
        m_tmin = EPS;
        m_tmax = FLT_MAX;
        m_inverse_direction = Vec3f(1.0f / m_direction(0),
                                    1.0f / m_direction(1),
                                    1.0f / m_direction(2));
    }

    /// @brief calculate reflection direction by give normal
    /// @param given normal
    /// @retval reflection direction
    Vec3f Reflect(const Vec3f &normal);

    /// @brief calculate reflaction direction by given normal and eta
    /// @param given surface normal
    /// @param eta n1 / n2
    /// @param refract_dir refraction direction
    /// @retval success refraction or not
    bool Refract(const Vec3f &normal, float eta, Vec3f &refract_dir);
};
