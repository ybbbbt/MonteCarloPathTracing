#pragma once

#include <vector>
#include <string>
#include <array>

#include "Types.h"
#include "AABB.h"
#include "AbstractObject.h"
#include "KdTree.h"
#include "Scene.h"

/// @class Model
class Model: public AbstractObject {
public:
    /// vertices of models
    std::vector<Vec3f> m_vertices;
    /// normals pf triangles
    std::vector<Vec3f> m_normals;
    /// AABB of model
    AABB m_bbox;

    /// @brief constructor of Model
    explicit Model(std::string obj_path);

    /// @brief default destructor of Model
    ~Model();

    /// @brief load obj by string path
    /// @param obj_path given obj path
    bool Load(std::string obj_path);

    /// @brief load mtl by string path
    /// @param path given mtl path
    /// @param material_table store mtl information in this table
    bool LoadMaterial(std::string path, std::map<std::string, Material> &material_table);

    /// @brief get AABB bounding box of this model
    AABB GetBoundingBox() override;

    /// @brief hit test to this Model
    /// @param ray the given ray to test hit
    /// @param the intersection, which contains all algorithm's need of intersection
    bool Hit(Ray &ray, Intersection &intersection) override;

private:
    /// triangles of models
    std::vector<AbstractObject *> m_triangles;
    /// KdTree of model
    KdTree m_kd_tree;
};

/// @class Triangle
class Triangle: public AbstractObject {
public:
    /// @brief constructor of triangle
    /// @param model pointer to corresponding model
    /// @param vertex_idx index of 3 vertices
    /// @param normal_idx index of 3 normal
    explicit Triangle(Model *model, std::array<int, 3> &vertex_idx, std::array<int, 3>  &normal_idx);

    /// @brief default destructor of triangle
    ~Triangle() = default;

    /// @brief get AABB bounding box of this triangle
    AABB GetBoundingBox() override;

    /// @brief hit test to this triangle
    /// @param ray the given ray to test hit
    /// @param the intersection, which contains all algorithm's need of intersection
    bool Hit(Ray &ray, Intersection &intersection) override;

    /// @brief get intersection's normal by interpolation of 3 normals
    /// @param point intersection point
    Vec3f GetIntersectionNormal(const Vec3f &point);

private:
    /// corresponding model
    Model *m_model;
    /// vertices of triangle
    Vec3f m_p[3];
    /// normals of triangle vertices
    Vec3f m_normal[3];
    /// two edge of triangle
    Vec3f m_edge1, m_edge2;
    /// for fast barycentric calculation
    float m_d00, m_d01, m_d11, m_d;
    /// bounding box of triangle
    AABB m_box;
};
