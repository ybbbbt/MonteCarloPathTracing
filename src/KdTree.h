#pragma once

#include "AbstractObject.h"
#include "AABB.h"
#include <vector>

/// @class KdTree
class KdTree : public AbstractObject {
public:
    /// axis of KD Tree
    enum AXIS {
        AXIS_X, AXIS_Y, AXIS_Z
    };
    /// @brief default constructor of KdTree
    explicit KdTree() = default;

    /// @brief constructor of KdTree
    /// @param objects object to build
    explicit KdTree(std::vector<AbstractObject *> &objects);

    /// @brief default destructor of KdTree
    ~KdTree() override = default;

    /// @brief build KdTree with given objects
    /// @param objects given objects
    void BuildTree(std::vector<AbstractObject *> &objects);

    /// @brief get bounding box of kdtree
    AABB GetBoundingBox() override;

    /// @brief hit test to this KdTree
    /// @param ray the given ray to test hit
    /// @param the intersection, which contains all algorithm's need of intersection
    bool Hit(Ray &ray, Intersection &intersection) override;

    /// @brief get longest axis for split
    /// @param obj_vec given objects
    KdTree::AXIS GetLongestAxis(std::vector<AbstractObject *> &obj_vec);

private:
    /// AABB box of KdTree
    AABB m_box;
    /// split axis of this tree
    int m_split_axis = 0;
    /// left and right child of this tree
    AbstractObject *m_left = nullptr, *m_right = nullptr;
};
