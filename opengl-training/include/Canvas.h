#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "DrawObjects.h"

class Canvas
{
public:
	Canvas(int width, int height);
	~Canvas();
	void DrawRectangle(Rectangle &rectangle, float rotation);
	void ClearRectangle(Rectangle &rectangle);
	void Clip(Rectangle &rectangle, float rotation);
	void DrawText(Point &point, std::string text);

private:
	unsigned int m_fbo;
	unsigned int m_texture;
	Rectangle m_clip;

};

#endif // _CANVAS_H_
