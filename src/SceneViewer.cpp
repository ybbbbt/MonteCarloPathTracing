#include "SceneViewer.h"
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#ifdef __APPLE__
#include <gl/freeglut.h>
#else
#include <GL/freeglut.h>
#endif
#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>

using namespace std;

namespace {
    int g_width, g_height;
    std::mutex g_write_color;
    std::vector<float> g_colors;
}

PathTracer *SceneViewer::s_path_tracer = nullptr;


SceneViewer::SceneViewer(PathTracer *path_tracer, int width, int height) {
    s_path_tracer = path_tracer;
    g_width = width;
    g_height = height;
    g_colors.resize(g_width * g_height * 3);
}

void SceneViewer::RenderFunc() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, g_width, 0, g_height);
    glRasterPos2i(0, 0);
    g_write_color.lock();
    glDrawPixels(g_width, g_height, GL_RGB, GL_FLOAT, (GLvoid *)g_colors.data());
    g_write_color.unlock();
    glutSwapBuffers();
    glutPostRedisplay();
}

void SceneViewer::IdleFunc() {
    glutPostRedisplay();
}

void timer(int) {
    glutPostRedisplay();
    glutTimerFunc(1000/60, timer, 0);
}

void SceneViewer::ViewScene(bool view, bool save_img) {
    s_path_tracer->m_save_img = save_img;
    std::thread render_thread([this]() {
        while(true) {
            auto colors = s_path_tracer->RenderScene();
            g_write_color.lock();
            g_colors = colors;
            g_write_color.unlock();
        }
    });
    if (view) {
        int val = 1;
        char *ch = nullptr;
        glutInit(&val, &ch);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowSize(g_width, g_height);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("Path Tracing");
        glutIdleFunc(IdleFunc);
        glutDisplayFunc(RenderFunc);
        glutTimerFunc(1000.0/5, timer, 0);
        glutMainLoop();
    }
    render_thread.join();
}