#include "GLRender.h"

#include <iostream>
#include <stdio.h>
#include <thread>
#include "string.h"
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define X_USER_TO_DEVICE(x, w) (((2 * x) / w) - 1)
#define Y_USER_TO_DEVICE(y, h) (1 - ((2 * y) / h))

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

static std::map<GLchar, Character> Characters;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
static float RecVertices[] = {
    // positions          // colors
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f, 0.0f,  // top left 
};

static unsigned int RecIndex[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

GLRender::GLRender():
    m_OnExit(NULL)
{
    std::cout << "GLRender constructor" << std::endl;
    m_Init();

}

GLRender::~GLRender()
{
    std::cout << "GLRender destructor" << std::endl;

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);

    // delete m_textRender;
}

GLRender& GLRender::GetInstance()
{
    static GLRender instance;
    return instance;
}

void GLRender::DrawRectangle(Rectangle &rectangle, float rotation)
{
    std::cout << "DrawRectangle: " << std::endl; 
    Point topLeft(rectangle.topLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    topLeft.color = rectangle.topLeft.color;
    Point topRight(rectangle.topRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    topRight.color = rectangle.topRight.color;
    Point bottomLeft(rectangle.bottomLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    bottomLeft.color = rectangle.bottomLeft.color;
    Point bottomRight(rectangle.bottomRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    bottomRight.color = rectangle.bottomRight.color;

    std::cout << "topRight Color: " << topRight.color.r << ", " << topRight.color.g << ", " << topRight.color.b << ", " << topRight.color.a << std::endl;
    std::cout << "topLeft Color: " << topLeft.color.r << ", " << topLeft.color.g << ", " << topLeft.color.b << ", " << topLeft.color.a << std::endl;
    std::cout << "bottomRight Color: " << bottomRight.color.r << ", " << bottomRight.color.g << ", " << bottomRight.color.b << ", " << bottomRight.color.a << std::endl;
    std::cout << "bottomLeft Color: " << bottomLeft.color.r << ", " << bottomLeft.color.g << ", " << bottomLeft.color.b << ", " << bottomLeft.color.a << std::endl;


    float vertices[] = {
    // positions                    // colors
    topRight.x,     topRight.y,     0.0f,   topRight.color.r,       topRight.color.g,       topRight.color.b,       topRight.color.a,  // top right
    bottomRight.x,  bottomRight.y,  0.0f,   bottomRight.color.r,    bottomRight.color.g,    bottomRight.color.b,    bottomRight.color.a,  // bottom right
    bottomLeft.x,   bottomLeft.y,   0.0f,   bottomLeft.color.r,     bottomLeft.color.g,     bottomLeft.color.b,     bottomLeft.color.a,  // bottom left
    topLeft.x,      topLeft.y,      0.0f,   topLeft.color.r,        topLeft.color.g,        topLeft.color.b,        topLeft.color.a,  // top left 
    
    // // positions          // colors
    //  0.5f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       0.1,  // top right
    //  0.5f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       0.1, // bottom right
    // -1.0f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       0.1, // bottom left
    // -1.0f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       0.1, // top left 
    };

    // ------
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    // create transformations
    glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    transform = glm::rotate(transform,glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

    // get matrix's uniform location and set matrix
    m_shader->use();
    unsigned int transformLoc = glGetUniformLocation(m_shader->ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, vertices, sizeof(vertices));
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindVertexArray(m_vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(m_window);
    glBindVertexArray(0);

}

void GLRender::DrawText(std::string &text, Point &origin)
{
    std::cout << "GLRender::DrawText:  " << text.c_str() << std::endl;
    m_textRender->RenderText(text, origin);
    glfwSwapBuffers(m_window);
}

void GLRender::DrawText()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Shader shader = Shader("/home/alex/git-repos/personal/trainning/opengl/shader-text.vs", "/home/alex/git-repos/personal/trainning/opengl/shader-text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), static_cast<GLfloat>(SCREEN_HEIGHT), 0.0f);
    shader.use();;
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/home/alex/git-repos/personal/trainning/opengl/res/fonts/arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;


    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 56);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    int maxBearingY = 0;
    int maxHeight = 0;
    int maxDescent = 0;
    // Load first 128 characters of ASCII set
    FT_UInt charIndex = 0;
    int charCount = 0;
    int glyphCount = 0;
    for (FT_ULong c = FT_Get_First_Char(face, &charIndex); charIndex != 0; c = FT_Get_Next_Char(face, c, &charIndex))
    {
        charCount++;
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        glyphCount++;
        
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        std::cout << "GLYPH HEIGHT: " << face->glyph->metrics.height/64 << " = " << face->glyph->bitmap.rows << std::endl;
        std::cout << "GLYPH BEARING Y: " << face->glyph->metrics.horiBearingY/64 << " = " << face->glyph->bitmap_top << std::endl;
        std::cout << "GLYPH BEARING X: " << face->glyph->metrics.horiBearingX/64 << " = " << face->glyph->bitmap_left << std::endl;
        std::cout << "GLYPH ADVANCE X: " << face->glyph->metrics.horiAdvance/64 << " = " << face->glyph->advance.x / 64 << std::endl;
        std::cout << "GLYPH ADVANCE Y: " << face->glyph->metrics.vertAdvance/64 << " = " << face->glyph->advance.y / 64 << std::endl;
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        int descent = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY) / 64;
        if (face->glyph->metrics.horiBearingY/64 > maxBearingY) maxBearingY = face->glyph->metrics.horiBearingY/64;
        if (face->glyph->metrics.height/64 > maxHeight) maxHeight = face->glyph->metrics.height/64;
        if (descent > maxDescent) maxDescent = descent;
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    std::cout << "MAX BEARING Y: " << maxBearingY << std::endl;
    std::cout << "MAX HEIGHT: " << maxHeight << std::endl;
    std::cout << "MAX DESCENT: " << maxDescent << std::endl;
    std::cout << "Glyph Count: " << glyphCount << std::endl;
    std::cout << "Character Count: " << charCount << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // Render Text
    std::string text("This is sample text");
    GLfloat x = 0.0f;
    GLfloat y = SCREEN_HEIGHT;
    GLfloat scale = 1.0f;
    glm::vec3 color(0.5, 0.8f, 0.2f);

    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        // GLfloat ypos = y + (maxBearingY - ch.Bearing.y); // TOP Allignment
        // GLfloat ypos = y - (ch.Bearing.y + maxDescent); // BOTTOM Allignment
        GLfloat ypos = y - ch.Bearing.y; // Baseline

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos,   0.0, 0.0 },            
            { xpos,     ypos + h,       0.0, 1.0 },
            { xpos + w, ypos + h,       1.0, 1.0 },

            { xpos,     ypos,   0.0, 0.0 },
            { xpos + w, ypos + h,       1.0, 1.0 },
            { xpos + w, ypos,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);



    glfwSwapBuffers(m_window);
}

void GLRender::ClearRectangle(Rectangle &rectangle, Rectangle &clip)
{
    void *ptr = NULL;
    // m_Clip(clip, 0.0);

    std::cout << "ClearRectangle: " << std::endl; 
    Point topLeft(rectangle.topLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    topLeft.color = rectangle.topLeft.color;
    Point topRight(rectangle.topRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    topRight.color = rectangle.topRight.color;
    Point bottomLeft(rectangle.bottomLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    bottomLeft.color = rectangle.bottomLeft.color;
    Point bottomRight(rectangle.bottomRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    bottomRight.color = rectangle.bottomRight.color;

    float vertices[] = {
    // positions                    // colors
    topRight.x,     topRight.y,     0.0f,   topRight.color.r,       topRight.color.g,       topRight.color.b,       topRight.color.a,  // top right
    bottomRight.x,  bottomRight.y,  0.0f,   bottomRight.color.r,    bottomRight.color.g,    bottomRight.color.b,    bottomRight.color.a,  // bottom right
    bottomLeft.x,   bottomLeft.y,   0.0f,   bottomLeft.color.r,     bottomLeft.color.g,     bottomLeft.color.b,     bottomLeft.color.a,  // bottom left
    topLeft.x,      topLeft.y,      0.0f,   topLeft.color.r,        topLeft.color.g,        topLeft.color.b,        topLeft.color.a  // top left 
    
    // // positions          // colors
    //  0.5f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0,  // top right
    //  0.5f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0, // bottom right
    // -1.0f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0, // bottom left
    // -1.0f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0 // top left 
    };

    float verticesClip[] = {   
    // // positions          // colors
     0.5f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0,  // top right
     0.5f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0, // bottom right
    -1.0f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0, // bottom left
    -1.0f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0 // top left 
    };









    // // Initialize Stencil TEST
    // // glEnable(GL_STENCIL_TEST);
    // // glClear(GL_STENCIL_BUFFER_BIT);

    // m_Clip(clip, 30.0);

    glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    m_shader->use();
    unsigned int transformLoc = glGetUniformLocation(m_shader->ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    // // Setup stencil buffer
    glEnable(GL_STENCIL_TEST);
    // glStencilMask(0xFF);
    // glClearStencil(1);
    // glClear(GL_STENCIL_BUFFER_BIT);
    // glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // memcpy(ptr, verticesClip, sizeof(verticesClip));
    // glUnmapBuffer(GL_ARRAY_BUFFER);
    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, vertices, sizeof(vertices));
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glStencilMask(0x00);
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(m_window);
    glBindVertexArray(0);

    // // // Setup stencil buffer
    // // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // // glStencilMask(0xFF);
    // // glBindVertexArray(m_vao);
    // // glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // // ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // // memcpy(ptr, verticesClip, sizeof(verticesClip));
    // // glUnmapBuffer(GL_ARRAY_BUFFER);
    // // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // // Draw using stencil TEST
    // // glDisable(GL_STENCIL_TEST);
    // // glDisable(GL_DEPTH_TEST);
    // // glClearStencil(1);
    // // glStencilMask(0xFF);
    // // glClear(GL_STENCIL_BUFFER_BIT);
    // // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // // glStencilMask(0x00);
    // ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // memcpy(ptr, vertices, sizeof(vertices));
    // glUnmapBuffer(GL_ARRAY_BUFFER);
    // // glEnable(GL_BLEND);
    // // glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
    // // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfwSwapBuffers(m_window);
    // glBindVertexArray(0);
}

void GLRender::Clip(Rectangle &rectangle, float rotation)
{
    m_Clip(rectangle, rotation);
}

void GLRender::Compose(unsigned int textureId, Rectangle &source, Rectangle &dest)
{
    Point topLeftSource(source.topLeft.toTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
    Point topRightSource(source.topRight.toTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
    Point bottomLeftSource(source.bottomLeft.toTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
    Point bottomRightSource(source.bottomRight.toTexture(SCREEN_WIDTH, SCREEN_HEIGHT));

    Point topLeftDest(dest.topLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    Point topRightDest(dest.topRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    Point bottomLeftDest(dest.bottomLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    Point bottomRightDest(dest.bottomRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));

    float composeVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        topLeftDest.x,      topLeftDest.y,      topLeftSource.x,        topLeftSource.y,
        bottomLeftDest.x,   bottomLeftDest.y,   bottomLeftSource.x,     bottomLeftSource.y,
        bottomRightDest.x,  bottomRightDest.y,  bottomRightSource.x,    bottomRightSource.y,

        topLeftDest.x,      topLeftDest.y,      topLeftSource.x,        topLeftSource.y,
        bottomRightDest.x,  bottomRightDest.y,  bottomRightSource.x,    bottomRightSource.y,
        topRightDest.x,     topRightDest.y,     topRightSource.x,       topRightSource.y
    };

    m_composeShader->use();
    glBindVertexArray(m_composeVao);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindBuffer(GL_ARRAY_BUFFER, m_composeVbo);
    void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, composeVertices, sizeof(composeVertices));
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(m_window);
    glBindVertexArray(0);    
}
void GLRender::Compose(unsigned int textureId)
{

    float composeVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    m_composeShader->use();
    glBindVertexArray(m_composeVao);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(m_window);
    glBindVertexArray(0);

}

void GLRender::SetOnExit(void (*OnExit)(void))
{
    m_OnExit = OnExit;
}

void GLRender::CreateOffScreenBuffer(int width, int height, unsigned int *fboId, unsigned int *textureId)
{

    // Generate framebuffer
    glGenFramebuffers(1, fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, *fboId);

    // Generate FBO texture color buffer
    glGenTextures(1, textureId);
    glBindTexture(GL_TEXTURE_2D, *textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Bind texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *textureId, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate FBO stencil and depth buffer
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    // Bind stencil and color buffer to framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    else
        std::cout << "FRAMEBUFFER successfully created, ID:" << *fboId << std::endl;
    // Unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void GLRender::DeleteOffScreenBuffer(unsigned int fboId)
{
    glDeleteFramebuffers(1, &fboId); 
}

void GLRender::BindOffScreenBuffer(unsigned int fboId)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
}

void GLRender::m_Init()
{
    std::cout << "GLRender::m_Init" << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return;
    }
    glfwMakeContextCurrent(m_window);
    
    glfwSetKeyCallback(m_window, (GLFWkeyfun) &GLRender::m_OnInputEvent);
    glfwSetFramebufferSizeCallback(m_window, (GLFWframebuffersizefun) &GLRender::m_OnframebufferSizeEvent);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // m_textRender = new TextRender();
	// m_textRender->LoadFont(std::string("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf"), 48);

    m_shader = new Shader("/home/alex/git-repos/personal/trainning/opengl/shader-basic.vs", "/home/alex/git-repos/personal/trainning/opengl/shader-basic.fs");
    m_composeShader = new Shader("/home/alex/git-repos/personal/trainning/opengl/shader-compose.vs", "/home/alex/git-repos/personal/trainning/opengl/shader-compose.fs");
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    // VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecVertices), RecVertices, GL_DYNAMIC_DRAW);

    // EBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RecVertices), RecIndex, GL_STATIC_DRAW);

    // Setup VBO attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float composeVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    // screen quad VAO
    glGenVertexArrays(1, &m_composeVao);
    glGenBuffers(1, &m_composeVbo);
    glBindVertexArray(m_composeVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_composeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(composeVertices), &composeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    m_composeShader->use();
    m_composeShader->setInt("composeTexture", 0);

    glBindVertexArray(0);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glfwSwapBuffers(m_window);


    // std::thread renderThread([this]()
    // {
    //     this->m_Execute();
    // });
    // renderThread.join();

    std::cout << "GLRender::m_Init - Initialized" << std::endl;
}

void GLRender::m_Terminate()
{
    std::cout << "GLRender::m_Terminate" << std::endl;
    glfwTerminate();
}

void GLRender::m_OnInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "GLRender::m_OnInputEvent - key " << key << ", action " << action << std::endl;
    if (key == 9 && action == 0)
    {
        std::cout << "Exit pressed" << std::endl;
        // if (m_OnExit != NULL)
        // {
        //     m_OnExit();
        // }
    }
}

void GLRender::m_OnframebufferSizeEvent(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void GLRender::m_Clip(Rectangle &rectangle, float rotation)
{
    Point topLeft(rectangle.topLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    topLeft.color = rectangle.topLeft.color;
    Point topRight(rectangle.topRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    topRight.color = rectangle.topRight.color;
    Point bottomLeft(rectangle.bottomLeft.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    bottomLeft.color = rectangle.bottomLeft.color;
    Point bottomRight(rectangle.bottomRight.toDevice(SCREEN_WIDTH, SCREEN_HEIGHT));
    bottomRight.color = rectangle.bottomRight.color;


    float vertices[] = {
    // positions                    // colors
    topRight.x,     topRight.y,     0.0f,   topRight.color.r,       topRight.color.g,       topRight.color.b,       topRight.color.a,  // top right
    bottomRight.x,  bottomRight.y,  0.0f,   bottomRight.color.r,    bottomRight.color.g,    bottomRight.color.b,    bottomRight.color.a,  // bottom right
    bottomLeft.x,   bottomLeft.y,   0.0f,   bottomLeft.color.r,     bottomLeft.color.g,     bottomLeft.color.b,     bottomLeft.color.a,  // bottom left
    topLeft.x,      topLeft.y,      0.0f,   topLeft.color.r,        topLeft.color.g,        topLeft.color.b,        topLeft.color.a,  // top left 
    
    // // positions          // colors
    //  0.5f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0,  // top right
    //  0.5f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0, // bottom right
    // -1.0f, -0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0, // bottom left
    // -1.0f,  0.5f, 0.0f,   1.0,      1.0,       1.0,       1.0 // top left 
    };

    glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    transform = glm::rotate(transform,glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    m_shader->use();
    unsigned int transformLoc = glGetUniformLocation(m_shader->ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    // // Setup stencil buffer
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClearStencil(1);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, vertices, sizeof(vertices));
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glEnable(GL_STENCIL_TEST);
    // glStencilMask(0xFF);
    // glClearStencil(1);
    // glClear(GL_STENCIL_BUFFER_BIT);

    // // Setup stencil buffer
    // glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // memcpy(ptr, vertices, sizeof(vertices));
    // glUnmapBuffer(GL_ARRAY_BUFFER);
    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // // Back to stencil config for drawing
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindVertexArray(0);
}

void GLRender::m_Execute()
{
    std::cout << "GLRender::m_Execute" << std::endl;
    while (!glfwWindowShouldClose(m_window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    m_Terminate();
}
