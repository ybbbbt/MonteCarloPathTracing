#pragma once

#include "Types.h"
#include "Ray.h"
#include "Scene.h"

/// @class PathTracer
class PathTracer {
public:
    /// sample types
    enum SAMPLE_TYPE {
        LAMBERTIAN, SPECULAR
    };
    /// pointer to scene
    Scene *m_scene;
    /// iteration times counter
    int m_iter_cnt;
    /// max iterate depth per ray
    int m_max_depth;
    /// scene ambient level
    Vec3f m_scene_ambient;
    /// size of path tracer scene
    int m_width, m_height;
    /// set true to save image
    bool m_save_img = true;
    /// result directory
    std::string m_result_directory;
    /// save image per frame
    int m_save_per_frame;

    /// @brief trace ray recursive
    /// @param ray ray to trace
    /// @param depth current depth
    Vec3f TraceRay(Ray &ray, int depth = 0);

    /// @brief external light illumination
    /// @param ray ray to add external light
    /// @param the intersection of ray
    Vec3f ExternalLight(Ray &ray, const Intersection &intersection);

    /// @brief next ray sample type and direction pending
    /// @param ray ray to sample
    /// @param intersection of ray
    Ray NextRaySampling(Ray &ray, const Intersection &intersection);

    /// @brief BRDF importance sampling
    /// @param direction direction of next ray
    /// @param sample type of ray
    /// @param Ns shiness of specular type
    Vec3f BRDFImportanceSampling(const Vec3f &direction, SAMPLE_TYPE type, float Ns = 0);

    /// @brief constructor of path tracer
    /// @param scene pointer to scene
    explicit PathTracer(Scene *scene);

    /// @param default destructor of scene
    ~PathTracer() = default;

    /// @brief core render scene
    /// @note as C++11 support perfect forwarding, we can return vector without hesitation~
    std::vector<float> RenderScene();
};
