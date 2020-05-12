#ifndef _DRAW_OBJECTS_H_
#define _DRAW_OBJECTS_H_

#include <iostream>

class Color
{
public:
    Color():
        r(0.0),
        g(0.0),
        b(0.0),
        a(1.0)
    {}

    Color(float r, float g, float b, float a):
        r(r),
        g(g),
        b(b),
        a(a)
    {}

    float r, g, b, a;
};

class Point
{
public:
    Point(float x, float y):
        x(x),
        y(y),
        color(){};
    ~Point(){};
    Point toDevice(float w, float h)
    {
        std::cout << "toDEvice: " << x << ", " << y << std::endl;
        float xDevice = 2 * x / w - 1;
        float yDEvice = 1 - 2 * y / h;
        std::cout << "toDEvice Result: " << xDevice << ", " << yDEvice << std::endl;
        return Point(xDevice, yDEvice);
    };

    Point toTexture(float w, float h)
    {
        std::cout << "toTexture: " << x << ", " << y << std::endl;
        float xDevice = x / w;
        float yDEvice = 1 - y / h;
        std::cout << "toTexture Result: " << xDevice << ", " << yDEvice << std::endl;
        return Point(xDevice, yDEvice);
    };

    float x, y;
    Color color;
};

class Rectangle
{
public:
    Rectangle(float x,float y,float w,float h):
        x(x),
        y(y),
        w(w),
        h(h),
        topLeft(x, y),
        topRight(x + w, y),
        bottomLeft(x, y + h),
        bottomRight(x + w, y + h){}
    
    ~Rectangle(){}
    
    float x, y, w, h;
    Point topLeft;
    Point topRight;
    Point bottomLeft;
    Point bottomRight;
};

#endif // _DRAW_OBJECTS_H_