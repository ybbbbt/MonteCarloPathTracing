#include "Scene.h"
#include <iostream>
#include <cassert>

using namespace std;

namespace {
const float PI = 3.1415926f;
}

Scene::Scene(int width, int height) : m_width(width), m_height(height) {
    m_colors.resize(3 * m_width * m_height);
    m_colors.assign(m_colors.size(), 0);
}

Scene::~Scene() {
}

void Scene::SetViewPoint(const Vec3f &eye, const Vec3f &center, const Vec3f &up,
                         float fov_x) {
    m_eye = eye;
    m_direction = (center - eye).normalized();
    m_right = (m_direction.cross(up)).normalized();
    m_up = (m_right.cross(m_direction)).normalized();
    m_fov_x = fov_x;
    m_view_x = m_right * 2 * tanf(m_fov_x * PI / 360);
    m_view_y = m_up * 2 * tanf(m_fov_x * (float(m_height) / m_width) * PI / 360);
    m_view_z = m_direction;
}

void Scene::AddModel(AbstractObject *model) {
    m_scene_models.emplace_back(model);
}

void Scene::InitializeKdTree() {
    m_kd_tree.BuildTree(m_scene_models);
}

bool Scene::IntersectScene(Ray &ray, Intersection &intersection) {
    return m_kd_tree.Hit(ray, intersection);
}

Ray Scene::GetRay(int x, int y) {
    const float noise_amp = 0.5f;
    float randomX = (Rand01() - 0.5f) * noise_amp;
    float randomY = (Rand01() - 0.5f) * noise_amp;
    Vec3f direction = m_view_z
                      + ((x + randomX) / m_width - 0.5f) * m_view_x
                      + ((y + randomY) / m_height - 0.5f) * m_view_y;
    return Ray(m_eye, direction);
}
