#pragma once

#include "AbstractObject.h"
#include "KdTree.h"
#include "Model.h"
#include <memory>
#include <vector>

/// @struct ExternalLight
struct ExternalLight {
public:
    /// origin center of external light
    Vec3f m_origin;
    /// dx dy expand of external light
    Vec3f m_dx, m_dy;
    /// normal of external light expand plane
    Vec3f m_normal;
    /// emission of external light
    Vec3f m_emission;
    /// area of external light
    float m_area;
    /// light sample setting
    int m_light_samples;

    /// @brief constructor of ExternalLight
    /// @param origin origin center of external light
    /// @param dx expand of external light
    /// @param dy expand of external light
    /// @param emission emission of external light
    explicit ExternalLight(Vec3f origin, Vec3f dx, Vec3f dy, Vec3f emission) {
        m_light_samples = 3;
        m_origin = origin;
        m_dx = dx;
        m_dy = dy;
        m_emission = emission;
        m_normal = m_dx.cross(m_dy);
        m_area = m_normal.norm();
        m_normal = m_normal.normalized();
    }
};

class Scene {
public:
    /// vector to store scene models
    std::vector<AbstractObject *> m_scene_models;
    /// scene KdTree
    KdTree m_kd_tree;
    /// vector to store external lights
    std::vector<ExternalLight> m_lights;
    /// scene ambient
    Vec3f m_ambient = Vec3f(0, 0, 0);
    /// vector to store current frame scene color
    std::vector<float> m_colors;
    /// scene size
    int m_width, m_height;
    /// eye of view point
    Vec3f m_eye;
    /// direction of view
    Vec3f m_direction;
    /// fov of view (x axis)
    float m_fov_x;
    /// up and right direction of view
    Vec3f m_up, m_right;
    /// view ratio for calculate ray direction
    Vec3f m_view_x, m_view_y, m_view_z;

    /// @brief constructor of Scene
    /// @param width width of Scene
    /// @param height height of Scene
    explicit Scene(int width, int height);

    /// @brief destructor of Scene
    ~Scene();

    /// @brief set view point of scene
    /// @param eye eye of view point
    /// @param center setting of objects
    /// @param up up direction of eye
    /// @param fov_x fov of view (x axis)
    void SetViewPoint(const Vec3f &eye,
                      const Vec3f &center,
                      const Vec3f &up,
                      float fov_x);

    /// @brief add model to scene
    /// @param model model to add
    void AddModel(AbstractObject *model);

    /// @brief initialize KdTree
    void InitializeKdTree();

    /// @brief intersection test to this scene
    /// @param ray the given ray to test hit
    /// @param the intersection, which contains all algorithm's need of intersection
    bool IntersectScene(Ray &ray, Intersection &intersection);

    /// @brief get ray from view plane pixel
    /// @param x ux
    /// @param y uy
    Ray GetRay(int x, int y);
};

