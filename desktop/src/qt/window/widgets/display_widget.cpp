#include "display_widget.h"
#include <cstring>
#include <QFile>

#define WIN_W (SCREEN_W * SCALE + MARGIN * 2)
#define WIN_H (SCREEN_H * SCALE + MARGIN * 2)

#define X0 (static_cast<float>(MARGIN) / WIN_W * 2.0f - 1.0f)
#define X1 (X0 + static_cast<float>(SCREEN_W * SCALE) / WIN_W * 2.0f)
#define Y0 (static_cast<float>(MARGIN) / WIN_H * 2.0f - 1.0f)
#define Y1 (Y0 + static_cast<float>(SCREEN_H * SCALE) / WIN_H * 2.0f)

DisplayWidget::DisplayWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFixedSize(WIN_W, WIN_H);

    if (QFile splash_file(":/data/splash.bin"); splash_file.open(QIODevice::ReadOnly))
    {
        const QByteArray data = splash_file.readAll();
        memcpy(splash, data.constData(), sizeof(splash));
    }
}

void DisplayWidget::setEmulator(PocketWalker* emu)
{
    emulator = emu;
    was_last_frame_power_save = false;
}

void DisplayWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_W, SCREEN_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/framebuffer.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/framebuffer.frag");
    shader.link();

    constexpr float verts[] = {
        X0, Y1, 0.0f, 0.0f,
        X1, Y1, 1.0f, 0.0f,
        X1, Y0, 1.0f, 1.0f,
        X0, Y1, 0.0f, 0.0f,
        X1, Y0, 1.0f, 1.0f,
        X0, Y0, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void DisplayWidget::drawPixels(const uint8_t* pixels)
{
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_W, SCREEN_H, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    shader.bind();
    shader.setUniformValue("uTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    shader.release();
}

void DisplayWidget::paintGL()
{
    glClearColor(LCD_PALETTE_0 / 255.0f, LCD_PALETTE_0 / 255.0f, LCD_PALETTE_0 / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    uint8_t pixels[SCREEN_W * SCREEN_H * 4];

    if (!emulator)
    {
        for (int y = 0; y < SCREEN_H; y++)
        {
            const int page_offset = (y / 8) * 96 * SSD1854_COLUMN_SIZE;
            const int bit = y % 8;
            for (int x = 0; x < SCREEN_W; x++)
            {
                const int base = SSD1854_COLUMN_SIZE * x + page_offset;
                const uint8_t idx = (((splash[base]     >> bit) & 1) << 1) |
                                     ((splash[base + 1] >> bit) & 1);
                const int i = (y * SCREEN_W + x) * 4;
                pixels[i] = pixels[i+1] = pixels[i+2] = LCD_PALETTE[idx];
                pixels[i+3] = 255;
            }
        }
        drawPixels(pixels);
        return;
    }

    SSD1854DrawInfo* draw_info = emulator->GetDrawInfo();

    if (draw_info->power_save_mode)
    {
        if (!was_last_frame_power_save)
        {
            memset(pixels, LCD_PALETTE_0, sizeof(pixels));
            for (int i = 3; i < static_cast<int>(sizeof(pixels)); i += 4)
                pixels[i] = 255;
            drawPixels(pixels);
        }
        was_last_frame_power_save = true;
    }
    else
    {
        for (int y = 0; y < SCREEN_H; y++)
        {
            const int page = y / 8 + draw_info->page_offset;
            const int page_offset = page * SSD1854_TOTAL_COLUMNS * SSD1854_COLUMN_SIZE;
            const int bit_offset = y % 8;
            for (int x = 0; x < SCREEN_W; x++)
            {
                const int base = SSD1854_COLUMN_SIZE * x + page_offset;
                const uint8_t palette_index = (((draw_info->vram.Read8(base) >> bit_offset) & 1) << 1) | ((draw_info->vram.Read8(base + 1) >> bit_offset) & 1);
                const int idx = (y * SCREEN_W + x) * 4;
                pixels[idx] = pixels[idx+1] = pixels[idx+2] = LCD_PALETTE[palette_index];
                pixels[idx+3] = 255;
            }
        }
        drawPixels(pixels);
        was_last_frame_power_save = false;
    }
}

void DisplayWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}