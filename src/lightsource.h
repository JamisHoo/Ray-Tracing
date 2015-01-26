/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: lightsource.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 14:50:48
 *  Description: light source class
 *****************************************************************************/
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "common.h"

using namespace RayTracing;
class LightSource {
    Color _color; 
    Point _position;
public:
    LightSource(const Point& p, const Color& c):
        _color(c), _position(p) { }
    Color color() const { return _color; }
    Point position() const { return _position; }
    // TODO: change color, change position, change brightness
    // implement those functions if necessary
};

#endif /* LIGHTSOURCE_H */
