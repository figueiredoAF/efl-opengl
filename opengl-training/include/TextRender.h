#ifndef _TEXT_RENDER_INCLUDED_
#define _TEXT_RENDER_INCLUDED_

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include "string.h"

#include "Shader.h"
#include "DrawObjects.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class TextRender
{
public:
    TextRender()
    {
        m_ftLibrary = NULL;
        m_fontFace = NULL;
        int error = FT_Init_FreeType(&m_ftLibrary);
        if (error)
        {
            std::cout << "TextRender::TextRender() - Error inititalizing freetype library" << std::endl;
            m_ftLibrary = NULL;
        }
        else
        {
            std::cout << "TextRender::TextRender() - freetype library successfully initialized" << std::endl;
        }

        std::cout << "TextRender::TextRender() - loading text shader" << std::endl;
        textShader = new Shader("/home/alex/git-repos/personal/trainning/opengl/shader-text.vs", "/home/alex/git-repos/personal/trainning/opengl/shader-text.fs");
        std::cout << "TextRender::TextRender() - text shader loaded" << std::endl;
    };
    
    ~TextRender()
    {
        if (m_fontFace != NULL)
        {
            FT_Done_Face(m_fontFace);
            m_fontFace = NULL;
        }
        if (m_ftLibrary)
        {
            FT_Done_FreeType(m_ftLibrary);
            m_ftLibrary = NULL;
        }

        delete textShader;
        textShader = nullptr;
    };

    bool LoadFont(std::string path, int size)
    {
        if (m_fontFace != NULL)
        {
            FT_Done_Face(m_fontFace);
            m_fontFace = NULL;
        }

        if (FT_New_Face(m_ftLibrary, path.c_str(), 0, &m_fontFace))
        {
            std::cout << "TextRender::LoadFont() - Error loading font" << std::endl;
            m_fontFace = NULL;
            return false;
        }
        
        if (FT_Set_Pixel_Sizes(m_fontFace, 0, size))
        {
            std::cout << "TextRender::LoadFont() - Error stting font size" << std::endl;
            return false;
        }

        std::cout << "TextRender::LoadFont() - Font successfully loaded" << std::endl;
        return true;
        
    };

    void RenderText(std::string text, Point origin)
    {
        std::cout << "TextRender::RenderText() - Point: " << origin.x << ", " << origin.y << " , Text: " << text.c_str() << std::endl;
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        // return;

        FT_GlyphSlot g = m_fontFace->glyph;

        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        textShader->use();
        GLuint tex;
        glGenTextures(1, &tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        textShader->setInt(std::string("tex"), 0);
        GLfloat white[4] = {1, 1, 1, 1};
        glUniform4fv(glGetUniformLocation(textShader->ID, "color"), 1, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

        Point pen = Point(origin.x, origin.y);

        for (char &c : text)
        {
            if (FT_Load_Char(m_fontFace, c, FT_LOAD_RENDER))
            {
                std::cout << "Unable to load character - " << c << std::endl;
                continue;
            }
            else
            {
            std::cout << c << ": width=" << g->bitmap.width << " , height=" << g->bitmap.rows << std::endl;
            }

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_BYTE,
                g->bitmap.width,
                g->bitmap.rows,
                0,
                GL_BYTE,
                GL_UNSIGNED_BYTE,
                g->bitmap.buffer);
            
            float x = pen.x + g->bitmap_left;
            float y = pen.y + g->bitmap_top;
            float w = g->bitmap.width;
            float h = g->bitmap.rows;

            Point topLeft = Point(x, y).toDevice(SCREEN_WIDTH, SCREEN_HEIGHT);
            Point topRight = Point(x + w, y).toDevice(SCREEN_WIDTH, SCREEN_HEIGHT);
            Point bottomLeft = Point(x, y + h).toDevice(SCREEN_WIDTH, SCREEN_HEIGHT);
            Point bottomRight = Point(x + w, y + h).toDevice(SCREEN_WIDTH, SCREEN_HEIGHT);

            float box[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
            };

            // float box[] = {
            //     // bottomLeft.x,  bottomLeft.y,   0, 0,
            //     // bottomRight.x, bottomRight.y,  1, 0,
            //     // topLeft.x,     topLeft.y,      0, 1,
            //     // topRight.x,    topRight.y,     1, 1
            
            //     -1.0f, -1.0f,   0.0f, 0.0f,
            //     1.0f, -1.0f,  1.0f, 0.0f,
            //     -1.0f, 1.0f,      0.0f, 1.0f,
            //     1.0f, 1.0f,     1.0f, 1.0f
            // };

            glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            pen.x += g->advance.x/64;
            pen.y += g->advance.y/64;
        }
        std::cout << std::endl;
    }

    FT_Library m_ftLibrary;
    FT_Face m_fontFace;
    Shader *textShader;
};

#endif // _TEXT_RENDER_INCLUDED_