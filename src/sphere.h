/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: sphere.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 14:50:41
 *  Description: Class Sphere is sphere object.
 *****************************************************************************/
#ifndef SPHERE_H
#define SPHERE_H

#include "common.h"
#include "object.h"

using namespace RayTracing;

class Sphere: public Object {
    Point _center;
    ELEMTYPE _radius;

public:
    Sphere(const Point& c, const ELEMTYPE r, 
           const Material* mat, const Texture* tex):
        Object(mat, tex), _center(c), _radius(r) { 
    }

    virtual ~Sphere() { }

    ELEMTYPE lowerBoundX() const { return _center[0] - _radius; }
    ELEMTYPE upperBoundX() const { return _center[0] + _radius; }
    ELEMTYPE lowerBoundY() const { return _center[1] - _radius; }
    ELEMTYPE upperBoundY() const { return _center[1] + _radius; }
    ELEMTYPE lowerBoundZ() const { return _center[2] - _radius; }
    ELEMTYPE upperBoundZ() const { return _center[2] + _radius; }
    void setCenter(const Point& center) { _center = center; }
    Point center() const { return _center; } 
    void setRadius(const ELEMTYPE radius) { _radius = radius; }
    ELEMTYPE radius() const { return _radius; }

    Color color() const { return _material -> color(); }

    Color texture(const Point& p) const { 
        // if there's not texture
        if (!_texture) return color(); 

        Vector n = normal(p);
        //theta in [0, pi]
        //phi in [0, 2pi]
        ELEMTYPE angleTheta = acos(n[2] / n.norm()); 
        ELEMTYPE anglePhi = n[1] > 0? acos(n[0] / sqrt(n[0] * n[0] + n[1] * n[1])):
                             2 * PI - acos(n[0] / sqrt(n[0] * n[0] + n[1] * n[1]));

        //x, y in [0, 1]
        ELEMTYPE x = asin(2 * angleTheta / PI - 1) / PI + 0.5;
        ELEMTYPE y = asin(anglePhi / PI - 1) / PI + 0.5;
        
        return _texture -> getPixel(x, y, 1, 1);

        //return color();
    }

    Vector normal(const Point& p) const {
        Vector n = p - _center;
        return n.normalize();
    }

    INTERSECTED_TYPE isIntersected(const Ray& ray, ELEMTYPE& distance) const {
        // distance is positive infinity
        distance = DOUBLE_MAX;

        // algorithm:
        // Sc: center of sphere
        // r: radius of sphere
        // Ro: origin of ray
        // Rl: unit vector of ray direction
        // l^2 = (Sc - Ro) * (Sc - Ro)
        // if (l^2 < r^2), Ro is in the sphere
        // if (l^2 > r^2) ,Ro is out of the sphere
        // tp = l * Rl
        // if (tp < 0), no intersection
        // d^2 = l^2 - r^2
        // if (d^2 > r^2), no intersection
        // t'^2 = r^2 - d^2
        // distance = t = tp +(-) t'

        Vector l = _center - ray.origin();
        ELEMTYPE lSqr = l * l;
        ELEMTYPE tp = l * ray.direction();
        ELEMTYPE rSqr = _radius * _radius;
        if (lSqr > rSqr && tp < 0) return MISS;

        ELEMTYPE tpSqr = tp * tp;
        ELEMTYPE dSqr = lSqr - tpSqr;
        if (dSqr > rSqr) return MISS;
        
        ELEMTYPE tApos = sqrt(rSqr - dSqr); // t'

        if (lSqr > rSqr) { // origin of ray is out of the sphere
            distance = tp - tApos;
            return INTERSECTED;
        }
        else if (lSqr < rSqr) { // origin of ray is in the sphere
            distance = tp + tApos;
            return INTERSECTED_IN;
        }
        else { // on the surface of the ball
            assert(0);
            return MISS;
        }
    }

};

#endif /* SPHERE_H */
