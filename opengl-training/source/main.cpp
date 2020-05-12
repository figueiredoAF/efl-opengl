#include <iostream>
#include <thread>
#include <chrono>
#include "string.h"

#include "Canvas.h"
#include "GLRender.h"
#include "DrawObjects.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static bool terminateApp = false;

void onExitKey()
{
	std::cout << "onExitKey" << std::endl;
	terminateApp = true;
}

int main(int argc, const char *argv[])
{
	std::cout << "Hello world" << std::endl;
	GLRender::GetInstance().SetOnExit(onExitKey);
	Canvas *canvas = new Canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
	Canvas *canvas2 = new Canvas(SCREEN_WIDTH, SCREEN_HEIGHT);


	Rectangle rec0 = Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	rec0.topLeft.color = Color(1.0, 1.0, 1.0, 0.0);
	rec0.bottomLeft.color = Color(1.0, 1.0, 1.0, 0.0);
	rec0.topRight.color = Color(1.0, 1.0, 1.0, 0.0);
	rec0.bottomRight.color = Color(1.0, 1.0, 1.0, 0.0);

	Rectangle rec1 = Rectangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	rec1.topLeft.color = Color(0.0, 1.0, 0.0, 0.8);
	rec1.bottomLeft.color = Color(0.0, 1.0, 0.0, 0.8);
	rec1.topRight.color = Color(0.0, 1.0, 0.0, 0.8);
	rec1.bottomRight.color = Color(0.0, 1.0, 0.0, 0.8);

	Rectangle rec2 = Rectangle(0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	rec2.topLeft.color = Color(1.0, 0.0, 1.0, 0.5);
	rec2.bottomLeft.color = Color(1.0, 0.0, 1.0, 0.5);
	rec2.topRight.color = Color(1.0, 0.0, 1.0, 0.5);
	rec2.bottomRight.color = Color(1.0, 0.0, 1.0, 0.5);

	// canvas->SetViewPort(rec2);
	// canvas->DrawRectangle(rec0);

	// canvas->ClearRectangle(rec0);
	// canvas2->ClearRectangle(rec0);
	
	// canvas2->Clip(rec1, 0.0);
	// canvas2->ClearRectangle(rec1);
	std::string text("RENDER TEXT OPENGL");
	Point point = Point(1.0, 1.0);
	// canvas2->DrawText(point, text);
	GLRender::GetInstance().DrawText();

	// canvas->Clip(rec2, 0.0);
	// canvas->ClearRectangle(rec2);

	while (!terminateApp){glfwPollEvents();}

	delete canvas;
	delete canvas2;
	return 0;
}
