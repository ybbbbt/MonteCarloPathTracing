#include <cmath>
#include "Ray.h"

Vec3f Ray::Reflect(const Vec3f &normal) {
    // Ref: https://www.cnblogs.com/bluebean/p/5299358.html
    return m_direction - 2.0f * normal * (normal.dot(m_direction));
}

bool Ray::Refract(const Vec3f &normal, float eta, Vec3f &refract_dir) {
    // Ref: https://www.cnblogs.com/night-ride-depart/p/7429618.html
    const Vec3f &i = m_direction;
    const Vec3f &n = normal;
    float cosi = i.dot(n);
    float cost2 = 1.0f - eta * eta * (1.0f - cosi * cosi);
    if (cost2 >= 0.0f) {
        refract_dir = eta * i - (eta * cosi + std::sqrt(cost2)) * n;
        return true;
    } else {
        // total internal reflection
        return false;
    }
}
