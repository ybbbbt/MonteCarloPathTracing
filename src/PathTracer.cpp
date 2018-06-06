#include "PathTracer.h"
//#include <omp.h>
#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <experimental/filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef _WIN32
#define STBI_MSC_SECURE_CRT
#endif

#include "stb_image_write.h"

namespace std {
    namespace fs = std::experimental::filesystem;
}

using namespace std;

namespace {
const float PI = 3.1415926f;
double time_sum = 0;

}

PathTracer::PathTracer(Scene *scene) {
    srand(0);
    m_scene = scene;
    m_iter_cnt = 0;
    m_max_depth = 6;
    m_scene_ambient = scene->m_ambient;
    m_width = scene->m_width;
    m_height = scene->m_height;
    m_result_directory = "./result/";
    m_save_per_frame = 50;
}

Vec3f PathTracer::BRDFImportanceSampling(const Vec3f &direction, SAMPLE_TYPE type, float Ns) {
    // Ref : https://inst.eecs.berkeley.edu/~cs283/sp13/lectures/283-lecture11.pdf
    float phi = Rand01() * 2 * PI;
    float theta;
    if (type == LAMBERTIAN) {
        // for cosine-weighted Lambertian
        theta = asin(sqrt(Rand01()));
    } else if (type == SPECULAR) {
        // for sampling specular term
        theta = acos(pow(Rand01(), 1 / (Ns + 1)));
    } else {
        cerr << "unknown sample type" << endl;
    }
    Vec3f sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));
    Vec3f front;
    if (fabs(direction.x()) > fabs(direction.y())) {
        front = Vec3f(direction.z(), 0, -direction.x()).normalized();
    } else {
        front = Vec3f(0, -direction.z(), direction.y()).normalized();
    }
    Vec3f right = direction.cross(front);
    return (sample.x() * right + sample.y() * direction + sample.z() * front).normalized();
}

Ray PathTracer::NextRaySampling(Ray &ray, const Intersection &intersection) {
    const Material &material = *intersection.m_material;
    Vec3f direction;

    // if has Refraction
    if (material.Ni > 1.01) {
        float n1, n2;
        float cos_in = ray.m_direction.dot(intersection.m_normal);
        Vec3f normal;
        if (cos_in > 0) {
            // out of glass
            normal = -intersection.m_normal;
            n1 = material.Ni;
            n2 = 1.0;
        } else {
            // in to the glass
            normal = intersection.m_normal;
            n1 = 1.0;
            n2 = material.Ni;
        }

        // Ref : https://en.wikipedia.org/wiki/Schlick%27s_approximation
        float f0 = (n1 - n2) / (n1 + n2);
        f0 *= f0;
        float pow_5 = 1.0f - std::abs(cos_in);
        float pow_5_2 = pow_5 * pow_5;
        pow_5 = pow_5_2 * pow_5_2 * pow_5;
        float fresnel = f0 + (1.0f - f0) * pow_5;
        if (fresnel < Rand01()) {
            if (ray.Refract(normal, n1 / n2, direction)) {
                return Ray(intersection.m_point, direction, Ray::TYPE::TRANSMISSION);
            } else {
                // only specular for refraction material
                direction = ray.Reflect(normal);
                return Ray(intersection.m_point, direction, Ray::TYPE::SPECULAR);
            }
        }
    }

    // reflection
    float kd_norm = material.Kd.norm();
    float ks_norm = material.Ks.norm();
    if (kd_norm / ks_norm < Rand01()) {
        Vec3f reflect = ray.Reflect(intersection.m_normal);
        direction = BRDFImportanceSampling(reflect, SPECULAR, material.Ns);
        return Ray(intersection.m_point, direction, Ray::TYPE::SPECULAR);
    } else {
        direction = BRDFImportanceSampling(intersection.m_normal, LAMBERTIAN);
        return Ray(intersection.m_point, direction, Ray::TYPE::DIFFUSE);
    }
}

Vec3f PathTracer::TraceRay(Ray &ray, int depth) {
    Intersection intersection;
    if (!m_scene->IntersectScene(ray, intersection)) {
        return Vec3f(0, 0, 0);
    } else {
        Material &material = *intersection.m_material;
        if (depth >= m_max_depth) {
            return material.Ke;
        }
        Ray next_ray = NextRaySampling(ray, intersection);
        Vec3f indirect_light(0, 0, 0);
        if (next_ray.m_source != Ray::TYPE::NONE) {
            indirect_light = TraceRay(next_ray, depth + 1);
            switch (next_ray.m_source) {
            case Ray::TYPE::DIFFUSE:
                indirect_light = material.Kd.cwiseProduct(indirect_light);
                break;
            case Ray::TYPE::SPECULAR:
                indirect_light = material.Ks.cwiseProduct(indirect_light);
                break;
            case Ray::TYPE::TRANSMISSION:
                indirect_light = material.Tf.cwiseProduct(indirect_light);
                break;
            default:
                break;
            }
        }
        if (m_scene->m_lights.empty()) {
            return material.Ke
                   + indirect_light
                   + material.Ka.cwiseProduct(m_scene_ambient);
        } else {
            Vec3f direct_light = ExternalLight(ray, intersection);
            return material.Ke
                   + direct_light
                   + indirect_light
                   + material.Ka.cwiseProduct(m_scene_ambient);
        }
    }
}

Vec3f PathTracer::ExternalLight(Ray &ray, const Intersection &intersection) {
    Vec3f sum_rgb(0, 0, 0);
    for (auto &light : m_scene->m_lights) {
        Vec3f local_rgb(0, 0, 0);
        Material &material = *intersection.m_material;
        for (int i = 0; i < light.m_light_samples; i++) {
            float sx = Rand01() - 0.5f;
            float sy = Rand01() - 0.5f;
            Vec3f light_origin = light.m_origin + light.m_dx * sx + light.m_dy * sy;
            Vec3f light_direction = light_origin - intersection.m_point;
            float length_light = light_direction.norm();
            Ray light_ray = Ray(intersection.m_point, light_direction);
            light_ray.m_tmax = length_light;
            Intersection isct;
            if (!m_scene->IntersectScene(light_ray, isct)) {
                Vec3f light_d_n = light_direction.normalized();
                float cos_i = max(intersection.m_normal.dot(light_d_n), 0.0f);
                float cos_o = max((-light_d_n).dot(light.m_normal), 0.0f);
                float f0 = cos_i * cos_o / (length_light * length_light);
                Vec3f energy = f0 * light.m_area * light.m_emission / light.m_light_samples;
                // external light only add diffuse
                if (material.Kd != Vec3f(0, 0, 0)) {
                    float cos_i_o = light_d_n.dot(intersection.m_normal);
                    if (cos_i_o > 0.0) {
                        local_rgb += cos_i_o * material.Kd.cwiseProduct(energy) / PI;
                    }
                }
            }
        }
        sum_rgb += local_rgb;
    }
    Clamp(sum_rgb, 0, 1);
    return sum_rgb;
}

std::vector<float> PathTracer::RenderScene() {
    if (m_iter_cnt == 0) {
        // check result directory
        if (!std::fs::exists(m_result_directory)) {
            cout << "no '" << m_result_directory << "' directory, I would create it!" << endl;
            std::fs::create_directories(m_result_directory);
        }
    }
    auto t_start = std::chrono::high_resolution_clock::now();
    ++m_iter_cnt;
    #pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            auto ray = m_scene->GetRay(x, y);
            Vec3f color = TraceRay(ray, 0);
            auto FuseColor = [this](int x, int y, int iter, const Vec3f & color) {
                int idx = y * m_width * 3 + x * 3;
                auto &color_vec = m_scene->m_colors;
                for (int i = 0; i < 3; ++i) {
                    int ii = idx + i;
                    color_vec[ii] = (color_vec[ii] * (m_iter_cnt - 1) + color(i)) / m_iter_cnt;
                }
            };
            FuseColor(x, y, m_iter_cnt, color);
        }
    }

    // save image
    if (m_save_img) {
        if (m_iter_cnt % m_save_per_frame == 0) {
            std::vector<unsigned char> color_uchar8(m_scene->m_colors.size());
            auto it = m_scene->m_colors.begin();
            auto it2 = color_uchar8.begin();
            for (; it != m_scene->m_colors.end(); ++it, ++it2) {
                float val = *it;
                val = val > 1 ? 1 : val;
                val = val < 0 ? 0 : val;
                *it2 = (unsigned char)(val * 255);
            }
            std::ostringstream sstream;
            sstream << m_result_directory
                    << std::setw(4) << std::setfill('0')
                    << m_iter_cnt << ".bmp";

            stbi_flip_vertically_on_write(true);
            auto r = stbi_write_bmp(sstream.str().c_str(),
                                    m_scene->m_width,
                                    m_scene->m_height,
                                    3, color_uchar8.data());
            if (r == 1) {
                cout << "Save at " << m_iter_cnt << endl;
            } else {
                cerr << "Fail to save, please check wheather you have result directory" << endl;
            }

        }
    }

    cout << "Iteration :" + std::to_string(m_iter_cnt) << "  ";

    auto t_end = std::chrono::high_resolution_clock::now();
    double tloop = std::chrono::duration_cast<std::chrono::duration<double>>
                   (t_end - t_start).count();
    time_sum += tloop;
    cout << std::fixed << std::setprecision(4) << "Time: " << tloop << "s Sum: " <<
         time_sum << "s" << endl;
    return m_scene->m_colors;
}
