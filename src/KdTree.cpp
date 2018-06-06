#include "KdTree.h"
#include <algorithm>
#include <iostream>

using namespace std;

KdTree::KdTree(vector<AbstractObject *> &objects) {
    BuildTree(objects);
}

void KdTree::BuildTree(vector<AbstractObject *> &objects) {
    m_split_axis = GetLongestAxis(objects);
    auto length = objects.size();

    if (length == 1) {
        m_left = objects[0];
        m_right = nullptr;
        m_box = m_left->GetBoundingBox();
    } else if (length == 2) {
        m_left = objects[0];
        m_right = objects[1];
        m_box = AABB::Merge(m_left->GetBoundingBox(), m_right->GetBoundingBox());
    } else {
        vector<AbstractObject *> left_vec;
        vector<AbstractObject *> right_vec;

        float min_val, max_val;
        min_val = FLT_MAX;
        max_val = -FLT_MAX;

        // get min and max of all objects
        for (unsigned int i = 0; i < objects.size(); i++) {
            AABB bbox = objects[i]->GetBoundingBox();
            min_val = min(min_val, bbox.m_min(m_split_axis));
            max_val = max(max_val, bbox.m_max(m_split_axis));
        }

        // middle of min and max
        float middle = (max_val + min_val) / 2;

        for (unsigned int i = 0; i < objects.size(); i++) {
            AABB bbox = objects[i]->GetBoundingBox();
            float center = bbox.Center(m_split_axis);
            if (center < middle) {
                left_vec.emplace_back(objects[i]);
            } else {
                right_vec.emplace_back(objects[i]);
            }
        }

        // fix some extreme case when partition failed
        size_t index;
        if (right_vec.size() == objects.size()) {
            float min_center = FLT_MAX;
            for (size_t i = 0; i < right_vec.size(); i++) {
                AABB bbox = right_vec[i]->GetBoundingBox();
                float center = bbox.Center(m_split_axis);
                if (center < min_center) {
                    min_center = center;
                    index = i;
                }
            }
            left_vec.emplace_back(right_vec[index]);
            right_vec.erase(right_vec.begin() + index);

        } else if (left_vec.size() == objects.size()) {
            float min_center = -FLT_MAX;
            for (unsigned int i = 0; i < left_vec.size(); i++) {
                AABB bbox = left_vec[i]->GetBoundingBox();
                float center = bbox.Center(m_split_axis);
                if (center > min_center) {
                    min_center = center;
                    index = i;
                }
            }
            right_vec.emplace_back(left_vec[index]);
            left_vec.erase(left_vec.begin() + index);
            std::cout << "happen2 " << std::endl;

        }

        m_left = left_vec.size() > 0 ? new KdTree(left_vec) : nullptr;
        m_right = right_vec.size() > 0 ? new KdTree(right_vec) : nullptr;

        if (m_left == nullptr) {
            m_box = m_right->GetBoundingBox();
        } else if (m_right == nullptr) {
            m_box = m_left->GetBoundingBox();
        } else {
            m_box = AABB::Merge(m_left->GetBoundingBox(), m_right->GetBoundingBox());
        }
    }
}


AABB KdTree::GetBoundingBox() {
    return m_box;
}

bool KdTree::Hit(Ray &ray, Intersection &intersection) {
    if (!m_box.Intersect(ray)) {
        return false;
    }

    bool hit = false;

    if (ray.m_direction(m_split_axis) >= 0) {
        if (m_left != nullptr) {
            hit |= m_left->Hit(ray, intersection);
        }
        if (m_right != nullptr) {
            hit |= m_right->Hit(ray, intersection);
        }
    } else {
        if (m_right != nullptr) {
            hit |= m_right->Hit(ray, intersection);
        }
        if (m_left != nullptr) {
            hit |= m_left->Hit(ray, intersection);
        }
    }

    return hit;
}

KdTree::AXIS KdTree::GetLongestAxis(vector<AbstractObject *> &obj_vec) {
    AABB aabb;
    for (auto &obj : obj_vec) {
        aabb = aabb.Merge(aabb, obj->GetBoundingBox());
    }
    Vec3f diff = aabb.m_max - aabb.m_min;
    if (diff.x() > diff.y() && diff.x() > diff.z()) {
        return AXIS_X;
    }
    if (diff.y() > diff.x() && diff.y() > diff.z()) {
        return AXIS_Y;
    }
    return AXIS_Z;
}
