#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include "core/pokewalker/pocketwalker.h"

#define SCREEN_W 96
#define SCREEN_H 64
#define SCALE    8
#define MARGIN   (4 * SCALE)

#define LCD_PALETTE_0 0xCC
#define LCD_PALETTE_1 0x99
#define LCD_PALETTE_2 0x66
#define LCD_PALETTE_3 0x33

static constexpr uint8_t LCD_PALETTE[4] = {LCD_PALETTE_0, LCD_PALETTE_1, LCD_PALETTE_2, LCD_PALETTE_3};

class DisplayWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit DisplayWidget(PocketWalker& emulator, QWidget* parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    PocketWalker& emulator;
    QOpenGLShaderProgram shader;
    GLuint fb_texture = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    bool was_last_frame_power_save = false;
};
