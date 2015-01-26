/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: ray.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 14:50:33
 *  Description: Ray is a single light.
 *****************************************************************************/
#ifndef RAY_H
#define RAY_H

#include "common.h"

using namespace RayTracing;

class Ray {
    Point _origin;
    Vector _direction;
    ELEMTYPE _refractiveIndex; // the refractiveIndex of the medium the ray in
    ELEMTYPE _intensity;
public:
    Ray(const Point& o, const Vector& d, 
        const ELEMTYPE r = 1.0, const ELEMTYPE i = 1.0): 
        _origin(o), _direction(d), _refractiveIndex(r), _intensity(i) {
        // direciton is unit vector
        _direction = _direction.normalize();
    } 
    void setIntensity(const ELEMTYPE i) { _intensity = i; }
    void setRefractiveIndex(const ELEMTYPE r) { _refractiveIndex = r; }
    Vector origin() const { return _origin; }
    Vector direction() const { return _direction; }
    ELEMTYPE intensity() const { return _intensity; }
    ELEMTYPE refractiveIndex() const { return _refractiveIndex; }

};

#endif /* RAY_H */
