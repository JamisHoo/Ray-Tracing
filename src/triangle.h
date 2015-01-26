/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: triangle.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 11, 2014
 *  Time: 21:49:19
 *  Description: A triangle object.
 *****************************************************************************/
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "common.h"
#include "object.h"
#include "texture.h"

using namespace RayTracing;

class Triangle: public Object {
    Point _vertices[3];

public:
    Triangle(const Point& p0, const Point& p1, const Point& p2, 
             const Material* mat, const Texture* tex):
        Object(mat, tex){
        // the triangle should be a little larger to avoid black boundary
        Point center = (p0 + p1 + p2) * (ELEMTYPE(1.0) / 3);
        _vertices[0] = p0 + EPSILON * (p0 - center);
        _vertices[1] = p1 + EPSILON * (p1 - center);
        _vertices[2] = p2 + EPSILON * (p2 - center);
    }

    virtual ~Triangle() { }

    ELEMTYPE lowerBoundX() const { return std::min(std::min(_vertices[0][0], _vertices[1][0]), _vertices[2][0]); }
    ELEMTYPE upperBoundX() const { return std::max(std::max(_vertices[0][0], _vertices[1][0]), _vertices[2][0]); }
    ELEMTYPE lowerBoundY() const { return std::min(std::min(_vertices[0][1], _vertices[1][1]), _vertices[2][1]); }
    ELEMTYPE upperBoundY() const { return std::max(std::max(_vertices[0][1], _vertices[1][1]), _vertices[2][1]); }
    ELEMTYPE lowerBoundZ() const { return std::min(std::min(_vertices[0][2], _vertices[1][2]), _vertices[2][2]); }
    ELEMTYPE upperBoundZ() const { return std::max(std::max(_vertices[0][2], _vertices[1][2]), _vertices[2][2]); }

    Point& vertices(const COUNTTYPE k) {
        assert(k >= 0 && k < 3);
        return _vertices[k];
    }

    Color texture(const Point& p) const {
        // if there isn't a texture returns the color
        if (!_texture) return color();

        Vector xAxis = (_vertices[0] - _vertices[1]).normalize();
        Vector yAxis = crossProduct(normal(p), xAxis);
        assert(std::abs(yAxis.norm() - 1) < EPSILON);

        ELEMTYPE pX = (p - _vertices[0]) * xAxis;
        ELEMTYPE pY = (p - _vertices[0]) * yAxis;

        return _texture -> getPixel(pX, pY, pX, pY);
    }
    
    Color color() const { return _material -> color(); }

    Vector normal(const Point&) const { 
        return crossProduct(_vertices[1] - _vertices[0], 
                            _vertices[2] - _vertices[0]).normalize();
    }

    INTERSECTED_TYPE isIntersected(const Ray& ray, ELEMTYPE& distance) const {
        Vector A_B = _vertices[0] - _vertices[1];
        Vector C_A = _vertices[2] - _vertices[0];
        Vector A_O = _vertices[0] - ray.origin();

        ELEMTYPE deterA = -1.0 * crossProduct(ray.direction(), C_A) * A_B;
        if (deterA == 0) return MISS;

        ELEMTYPE beta = -1.0 * crossProduct(ray.direction(), C_A) * A_O / deterA;
        ELEMTYPE gamma = crossProduct(ray.direction(), A_O) * A_B / deterA;
        ELEMTYPE t = -1.0 * crossProduct(A_O, C_A) * A_B / deterA;
        ELEMTYPE alpha = 1 - beta - gamma;

        if (t < 0 || beta < 0 || beta > 1 || gamma < 0 || gamma > 1 || alpha < 0 || alpha > 1) {
            return MISS;
        }

        distance = t;
        return INTERSECTED;
    }

};

#endif /* TRIANGLE_H */
