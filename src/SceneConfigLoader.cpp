#include "Scene.h"
#include "PathTracer.h"
#include "SceneViewer.h"
#include "LuaConfigParser.h"
#include "SceneConfigLoader.h"
#include <iostream>
#include <cassert>

using namespace std;

void SceneConfigLoader::RenderSceneByConfig(std::string config_file) {
    LuaConfigureParser parser;
    if (!parser.LoadConfigFile(config_file)) {
        cerr << "can not open config file " << config_file << endl;
    }

    int width = parser.Get<int>("width"), height = parser.Get<int>("height");
    float fov_x = parser.Get<float>("fov");

    Model model(parser.GetString("model_file"));

    Vec3f center = model.m_bbox.Center();
    center += parser.GetVec3f("center_offset");
    Vec3f eye = center + parser.GetVec3f("eye_offset");;
    Vec3f up = parser.GetVec3f("up");;

    Scene scene(width, height);
    scene.AddModel(&model);
    scene.InitializeKdTree();

    scene.SetViewPoint(eye, center, up, fov_x);

    scene.m_ambient = parser.GetVec3f("scene_ambient");

    int external_light_num = parser.Get<int>("external_light_num");
    for (int i = 0; i < external_light_num; ++i) {
        Vec3f origin = parser.GetVec3f("el_origin_" + std::to_string(i));
        Vec3f dx = parser.GetVec3f("el_dx_" + std::to_string(i));
        Vec3f dy = parser.GetVec3f("el_dy_" + std::to_string(i));
        Vec3f emission = parser.GetVec3f("el_emission_" + std::to_string(i));
        scene.m_lights.emplace_back(ExternalLight(origin, dx, dy, emission));
    }

    PathTracer path_tracer(&scene);
    path_tracer.m_max_depth = parser.Get<int>("max_depth");
    path_tracer.m_result_directory = parser.GetString("result_directory");
    path_tracer.m_save_per_frame = parser.Get<int>("save_per_frame");

    SceneViewer display(&path_tracer, width, height);
    bool is_view = parser.Get<bool>("view");
    bool save_image = parser.Get<bool>("save_image");
    display.ViewScene(is_view, save_image);
}