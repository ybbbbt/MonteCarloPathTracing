#pragma once

#include "AABB.h"
#include "Ray.h"

/// @struct Material
struct Material {
    /// emission
    Vec3f Ke = Vec3f(0, 0, 0);
    /// shiness
    float Ns = 60;
    /// refraction
    float Ni = 1.0;
    /// transmission filter
    Vec3f Tf = Vec3f(0, 0, 0);
    /// ambient
    Vec3f Ka = Vec3f(0, 0, 0);
    /// diffuse
    Vec3f Kd = Vec3f(0, 0, 0);
    /// specular
    Vec3f Ks = Vec3f(0, 0, 0);
};

/// @struct Intersection
struct Intersection {
    /// pointer to object material
    Material *m_material = nullptr;
    /// point of intersection
    Vec3f m_point = Vec3f(0, 0, 0);
    /// normal of intersection
    Vec3f m_normal = Vec3f(0, 0, 0);
};

/// @struct AbstractObject
class AbstractObject {
public:
    /// the material of object
    Material m_material;

    /// @brief default constructor
    AbstractObject() = default;

    /// @brief default destructor
    virtual ~AbstractObject() = default;

    /// @brief get AABB bounding box of this object
    virtual AABB GetBoundingBox() = 0;

    /// @brief hit test of the ray with this object
    /// @param ray the given ray to test hit
    /// @param the intersection, which contains all algorithm's need of intersection
    virtual bool Hit(Ray &ray, Intersection &intersection) = 0;
};

