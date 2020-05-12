#include "Shader.h"

#include <mutex>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DrawObjects.h"
#include "TextRender.h"

class GLRender
{
public:
    void DrawRectangle(Rectangle &rectangle, float rotation);
    void DrawText(std::string &text, Point &origin);
    void DrawText();
    void ClearRectangle(Rectangle &rectangle, Rectangle &clip);
    void Clip(Rectangle &rectangle, float rotation);
    static GLRender &GetInstance();
    void SetOnExit(void (*OnExit)(void));
    void CreateOffScreenBuffer(int width, int height, unsigned int *fboId, unsigned int *textureId);
    void DeleteOffScreenBuffer(unsigned int fboId);
    void BindOffScreenBuffer(unsigned int fboId);
    void Compose(unsigned int textureId, Rectangle &source, Rectangle &dest);
    void Compose(unsigned int textureId);

private:
    GLRender();
    ~GLRender();

    void m_Init();
    void m_Terminate();
    void m_OnInputEvent(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods);
    void m_OnframebufferSizeEvent(GLFWwindow* window, int width, int height);
    void m_Execute();
    void m_Clip(Rectangle &rectangle, float rotation);


    std::mutex m_lock;
    GLFWwindow* m_window;
    void (*m_OnExit)(void);
    Shader *m_shader;
    Shader *m_composeShader;

    unsigned int m_vbo;
    unsigned int m_vao;
    unsigned int m_ebo;
    unsigned int m_composeVbo;
    unsigned int m_composeVao;
    TextRender *m_textRender;
};