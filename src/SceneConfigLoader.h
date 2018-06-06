#pragma once
#include <string>

/// @class SceneConfigLoader
class SceneConfigLoader {
public :
    /// @brief given config lua script, render scene
    /// @param config_file path to lua config file
    static void RenderSceneByConfig(std::string config_file);
};