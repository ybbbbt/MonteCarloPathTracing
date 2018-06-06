#include "Model.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <cstring>
#include <memory>

using namespace std;

Triangle::Triangle(Model *model, std::array<int, 3> &vertex_idx,
                   std::array<int, 3> &normal_idx) {
    m_model = model;
    for (int i = 0; i < 3; ++i) {
        m_p[i] = m_model->m_vertices[vertex_idx[i]];
        m_normal[i] = m_model->m_normals[normal_idx[i]];
    }
    m_edge1 = m_p[1] - m_p[0];
    m_edge2 = m_p[2] - m_p[0];
    // for accelerate barycentric calculation
    m_d00 = m_edge1.dot(m_edge1);
    m_d01 = m_edge1.dot(m_edge2);
    m_d11 = m_edge2.dot(m_edge2);
    m_d = m_d00 * m_d11 - m_d01 * m_d01;
    m_box.m_max = m_p[0].cwiseMax(m_p[1]).cwiseMax(m_p[2]);
    m_box.m_min = m_p[0].cwiseMin(m_p[1]).cwiseMin(m_p[2]);
}

AABB Triangle::GetBoundingBox() {
    return m_box;
}

Vec3f Triangle::GetIntersectionNormal(const Vec3f &point) {
    Vec3f v2_ = point - m_p[0];
    float d20 = v2_.dot(m_edge1);
    float d21 = v2_.dot(m_edge2);
    float v = (m_d11 * d20 - m_d01 * d21) / m_d;
    float w = (m_d00 * d21 - m_d01 * d20) / m_d;
    float u = 1 - v - w;

    Vec3f barycentric = Vec3f(u, v, w);
    barycentric.x() = abs(barycentric.x());
    barycentric.y() = abs(barycentric.y());
    barycentric.z() = abs(barycentric.z());

    Vec3f return_normal = Vec3f(
                              barycentric.x() * (m_normal[0]) +
                              barycentric.y() * (m_normal[1]) +
                              barycentric.z() * (m_normal[2])).normalized();
    return return_normal;
}


bool Triangle::Hit(Ray &ray, Intersection &intersection) {
    // Picking
    // Ref : https://www.cnblogs.com/graphics/archive/2010/08/09/1795348.html
    float t, u, v;
    Vec3f &v0 = m_p[0];
    Vec3f &v1 = m_p[1];
    Vec3f &v2 = m_p[2];
    Vec3f &E1 = m_edge1;
    Vec3f &E2 = m_edge2;
    Vec3f P = ray.m_direction.cross(E2);
    float det = E1.dot(P);
    Vec3f T;
    if ( det > 0 ) {
        T = ray.m_origin - v0;
    } else {
        T = v0 - ray.m_origin;
        det = -det;
    }
    if ( det < EPS ) {
        return false;
    }
    u = T.dot(P);
    if ( u < 0.0f || u > det ) {
        return false;
    }
    Vec3f Q = T.cross(E1);
    v = ray.m_direction.dot(Q);
    if (v < 0.0f || u + v > det ) {
        return false;
    }
    t = E2.dot(Q);
    float fInvDet = 1.0f / det;
    t *= fInvDet;
    if (t <= ray.m_tmax && t >= ray.m_tmin) {
        intersection.m_point = ray.m_origin + ray.m_direction * t;
        intersection.m_normal = GetIntersectionNormal(intersection.m_point);
        intersection.m_material = &this->m_material;
        ray.m_tmax = t;
        return true;
    } else {
        return false;
    }
}

bool Model::LoadMaterial(string path, map<string, Material> &material_table) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Fail to open mtl" << path << endl;
        return false;
    }

    bool active_mtl = false;
    string material_name;
    Material material = Material();

    string type;

    while (file >> type) {
        if (type == "newmtl") {
            if (active_mtl) {
                material_table.insert(make_pair(material_name, material));
                material = Material();
            } else {
                active_mtl = true;
            }
            file >> material_name;
        } else if (type == "Kd") {
            file >> material.Kd.x() >> material.Kd.y() >> material.Kd.z();
        } else if (type == "Ka") {
            file >> material.Ka.x() >> material.Ka.y() >> material.Ka.z();
        } else if (type == "Ks") {
            file >> material.Ks.x() >> material.Ks.y() >> material.Ks.z();
        } else if (type == "Ke") {
            file >> material.Ke.x() >> material.Ke.y() >> material.Ke.z();
            material.Ke = material.Ke * 0.99;
        } else if (type == "Ns") {
            file >> material.Ns;
        } else if (type == "Ni") {
            file >> material.Ni;
        } else if (type == "Tf") {
            file >> material.Tf.x() >> material.Tf.y() >> material.Tf.z();
        } else {
            file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
    }

    if (active_mtl) {
        material_table.insert(make_pair(material_name, material));
    }

    return true;
}

bool Model::Load(string obj_path) {
    ifstream file(obj_path);
    if (!file.is_open()) {
        cerr << "Fail to open obj " + obj_path << endl;
        return false;
    }
    bool return_value = true;
    map<string, Material> material_table;

    string type;
    Vec3f v(0, 0, 0);
    Vec3f vn(0, 0, 0);
    string material_name;
    Material material = Material();
    std::array<int, 3> vertex_idx = {0, 0, 0};
    std::array<int, 3> normal_idx = {0, 0, 0};

    while (file >> type) {
        if (type == "mtllib") {
            string mtl_file_name;
            file >> mtl_file_name;
            auto pos = obj_path.find_last_of('/');
            string mtl_path = obj_path.substr(0, pos + 1) + mtl_file_name;
            if (!LoadMaterial(mtl_path, material_table)) {
                return_value = false;
                cerr << "fail to load mtl " << mtl_path << endl;
                break;
            }
        } else if (type == "usemtl") {
            file >> material_name;
            auto it = material_table.find(material_name);
            if (it != material_table.end()) {
                material = it->second;
            } else {
                cerr << "mtl name " << material_name << " not in mtl file" << endl;
                return_value = false;
                break;
            }
        } else if (type == "v") {
            file >> v.x() >> v.y() >> v.z();
            m_bbox.m_min = m_bbox.m_min.cwiseMin(v);
            m_bbox.m_max = m_bbox.m_max.cwiseMax(v);
            m_vertices.emplace_back(v);
        } else if (type == "vn") {
            file >> vn.x() >> vn.y() >> vn.z();
            m_normals.emplace_back(vn);
        }
        //f vertex-index/texture-index/normal-index
        else if (type == "f") {
            int index = 0;
            while (true) {
                auto ch = file.get();
                if (ch == ' ') {
                    continue;
                } else if (ch == '\n' || ch == EOF) {
                    break;
                } else {
                    file.putback((char)ch);
                }

                file >> vertex_idx[index];
                --vertex_idx[index];

                auto next_char = file.get();
                int black_hole;
                if (next_char == '/') {
                    next_char = file.get();
                    if (next_char == '/') {
                        file >> normal_idx[index];
                        --normal_idx[index];
                    } else {
                        file.putback((char)next_char);
                        file >> black_hole;
                        next_char = file.get();
                        if (next_char == '/') {
                            file >> normal_idx[index];
                            normal_idx[index]--;
                        } else {
                            file.putback((char)next_char);
                        }
                    }
                } else {
                    file.putback((char)next_char);
                }
                ++index;
                if (index >= 3) {
                    // decompose to triangles
                    auto *tri = new Triangle(this, vertex_idx, normal_idx);
                    tri->m_material = material;
                    m_triangles.emplace_back(tri);

                    vertex_idx[1] = vertex_idx[2];
                    normal_idx[1] = normal_idx[2];
                    index = 2;
                }
            }

        }
    }
    file.close();
    return return_value;
}

Model::Model(std::string obj_path) {
    if (Load(obj_path)) {
        cout << "Load " << obj_path << " successful" << endl;
        cout << "Model has " << m_triangles.size() << " triangles " << endl;
    } else {
        cerr << "Load " << obj_path << " fail." << endl;
    }
    m_kd_tree.BuildTree(m_triangles);
}

Model::~Model() {
    for (auto &tri : m_triangles) {
        if (tri != nullptr) {
            delete tri;
        }
    }
}

AABB Model::GetBoundingBox() {
    return m_kd_tree.GetBoundingBox();
}

bool Model::Hit(Ray &ray, Intersection &intersection) {
    return m_kd_tree.Hit(ray, intersection);
}
