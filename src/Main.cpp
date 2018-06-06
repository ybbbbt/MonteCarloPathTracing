#include <iostream>
#include "SceneConfigLoader.h"
using namespace std;


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Suggested Usage: ./path_tracing scene_conf.lua" << endl;
        cout << "Use default config file from  ./config/scene01.lua " << endl;
        SceneConfigLoader::RenderSceneByConfig("./config/scene01.lua");
//        SceneConfigLoader::RenderSceneByConfig("./config/scene02.lua");
//        SceneConfigLoader::RenderSceneByConfig("./config/scene_vase.lua");
    } else {
        SceneConfigLoader::RenderSceneByConfig(argv[1]);
    }
    return 0;
}