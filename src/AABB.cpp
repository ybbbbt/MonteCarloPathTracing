#include "AABB.h"
#include <algorithm>

#include "Types.h"

using namespace std;

AABB AABB::Merge(const AABB &box1, const AABB &box2) {
    return AABB(box1.m_min.cwiseMin(box2.m_min),
                box1.m_max.cwiseMax(box2.m_max));
}

bool AABB::Intersect(Ray &ray) {
    Vec3f &inv_dir = ray.m_inverse_direction;

    // intersect with yoz
    float t1 = (m_min.x() - ray.m_origin.x()) * inv_dir.x();
    float t2 = (m_max.x() - ray.m_origin.x()) * inv_dir.x();

    // intersect with xoz
    float t3 = (m_min.y() - ray.m_origin.y()) * inv_dir.y();
    float t4 = (m_max.y() - ray.m_origin.y()) * inv_dir.y();

    // intersect with xoy
    float t5 = (m_min.z() - ray.m_origin.z()) * inv_dir.z();
    float t6 = (m_max.z() - ray.m_origin.z()) * inv_dir.z();

    // distance of front and back intersection
    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    return !(tmax < 0 || tmin > tmax || tmin >= ray.m_tmax || tmax <= ray.m_tmin);
}