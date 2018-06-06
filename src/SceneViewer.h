#pragma once

#include "PathTracer.h"

/// @class SceneViewer
class SceneViewer {
public:
    /// pointer to path tracer
    static PathTracer *s_path_tracer;

    /// @brief constructor of scene viewer
    /// @param path_tracer pointer to path tracer
    /// @param width scene width
    /// @param height scene height
    explicit SceneViewer(PathTracer *path_tracer, int width, int height);

    /// @brief Render Callback
    static void RenderFunc();

    /// @brief Idle CallBack
    static void IdleFunc();

    /// @brief start view scene
    /// @param view set true to enable view
    /// @param save_img set true to enable save image
    void ViewScene(bool view = true, bool save_img = true);
};
