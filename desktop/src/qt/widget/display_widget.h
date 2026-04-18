#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include "core/pokewalker/pocketwalker.h"

#define SCREEN_W 96
#define SCREEN_H 64
#define MARGIN 4
#define SCALE 8

class DisplayWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit DisplayWidget(QWidget* parent = nullptr);

    void setEmulator(PocketWalker* emulator);

    QSize sizeHint() const override
    {
        return QSize((SCREEN_W + MARGIN * 2) * SCALE, (SCREEN_H + MARGIN * 2) * SCALE);
    }

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void drawPixels(const uint8_t* pixels);

    PocketWalker* emulator = nullptr;
    uint8_t splash[1536] = {};
    QOpenGLShaderProgram shader;
    GLuint fb_texture = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    bool was_last_frame_power_save = false;
};