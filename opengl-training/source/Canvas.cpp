#include "Canvas.h"

#include <iostream>

#include "GLRender.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

Canvas::Canvas(int width, int height) :
    m_fbo(0),
    m_texture(0),
    m_clip(0.0, 0.0, (float)width, (float)height)
{
	std::cout << "Canvas constructor" << std::endl;
    GLRender::GetInstance().CreateOffScreenBuffer(width, height, &m_fbo, &m_texture);
}

Canvas::~Canvas()
{
	std::cout << "Canvas destructor" << std::endl;
}

void Canvas::DrawRectangle(Rectangle &rectangle, float rotation)
{
    char buffer[128];
    sprintf(buffer, "Canvas::DrawRectangle x[%f], y[%f], w[%f], h[%f]", 
            rectangle.x, rectangle.y, rectangle.w, rectangle.h);
    std::cout << buffer << std::endl;

    GLRender::GetInstance().BindOffScreenBuffer(m_fbo);
	GLRender::GetInstance().DrawRectangle(rectangle, rotation);
    GLRender::GetInstance().BindOffScreenBuffer(0);
    GLRender::GetInstance().Compose(m_texture);
}

void Canvas::DrawText(Point &point, std::string text)
{
    char buffer[128];
    sprintf(buffer, "Canvas::DrawText x[%f], y[%f], text[%s]", 
            point.x, point.y, text.c_str());
    std::cout << buffer << std::endl;

    GLRender::GetInstance().BindOffScreenBuffer(0);
	GLRender::GetInstance().DrawText(text, point);
    // GLRender::GetInstance().BindOffScreenBuffer(0);
    // GLRender::GetInstance().Compose(m_texture);

}

void Canvas::ClearRectangle(Rectangle &rectangle)
{
    char buffer[128];
    sprintf(buffer, "Canvas::ClearRectangle x[%f], y[%f], w[%f], h[%f]", 
            rectangle.x, rectangle.y, rectangle.w, rectangle.h);
    std::cout << buffer << std::endl;

    Rectangle rec0 = Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    GLRender::GetInstance().BindOffScreenBuffer(m_fbo);
	GLRender::GetInstance().ClearRectangle(rectangle, m_clip);
    GLRender::GetInstance().BindOffScreenBuffer(0);
    // GLRender::GetInstance().Compose(m_texture);
    GLRender::GetInstance().Compose(m_texture, rectangle, rectangle);
}

void Canvas::Clip(Rectangle &rectangle, float rotation)
{
    m_clip = rectangle;
        char buffer[128];
    sprintf(buffer, "Canvas::Clip x[%f], y[%f], w[%f], h[%f]", 
            rectangle.x, rectangle.y, rectangle.w, rectangle.h);
    std::cout << buffer << std::endl;
    GLRender::GetInstance().BindOffScreenBuffer(m_fbo);
	GLRender::GetInstance().Clip(rectangle, rotation);
    GLRender::GetInstance().BindOffScreenBuffer(0);
}
