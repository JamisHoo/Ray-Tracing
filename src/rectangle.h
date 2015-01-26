/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: rectangle.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 17, 2014
 *  Time: 22:57:30
 *  Description: A rectangle object.
 *****************************************************************************/
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "common.h"
#include "object.h"
#include "triangle.h"

class Rectangle: public Object {
    Point _vertices[4];
    Triangle _tri0;
    Triangle _tri1;
public:
    Rectangle(const Point& p0, const Point& p1, const Point& p2, const Point& p3, 
              const Material* mat, const Texture* tex): 
        Object(mat, tex), _tri0(p0, p1, p2, mat, tex), _tri1(p0, p2, p3, mat, tex) { 
        _vertices[0] = p0;
        _vertices[1] = p1;
        _vertices[2] = p2;
        _vertices[3] = p3;
        assert((Vector(p0 - p1) - Vector(p3 - p2)).norm() < EPSILON);
        assert((Vector(p0 - p3) - Vector(p1 - p2)).norm() < EPSILON);
        assert(Vector(p0 - p1) * Vector(p0 - p3) < EPSILON);
    }

    virtual ~Rectangle() { }

    ELEMTYPE lowerBoundX() const { 
        ELEMTYPE lbx = std::min(std::min(_vertices[0][0], _vertices[1][0]), std::min(_vertices[2][0], _vertices[3][0])); 
        ELEMTYPE ubx = std::max(std::max(_vertices[0][0], _vertices[1][0]), std::max(_vertices[2][0], _vertices[3][0])); 
        if (lbx == ubx) return lbx - EPSILON;
        return lbx;
    }
    ELEMTYPE upperBoundX() const { 
        ELEMTYPE lbx = std::min(std::min(_vertices[0][0], _vertices[1][0]), std::min(_vertices[2][0], _vertices[3][0])); 
        ELEMTYPE ubx = std::max(std::max(_vertices[0][0], _vertices[1][0]), std::max(_vertices[2][0], _vertices[3][0])); 
        if (lbx == ubx) return ubx + EPSILON;
        return ubx;
    }
    ELEMTYPE lowerBoundY() const { 
        ELEMTYPE lby = std::min(std::min(_vertices[0][1], _vertices[1][1]), std::min(_vertices[2][1], _vertices[3][1])); 
        ELEMTYPE uby = std::max(std::max(_vertices[0][1], _vertices[1][1]), std::max(_vertices[2][1], _vertices[3][1])); 
        if (lby == uby) return lby - EPSILON;
        return lby;
    }
    ELEMTYPE upperBoundY() const { 
        ELEMTYPE lby = std::min(std::min(_vertices[0][1], _vertices[1][1]), std::min(_vertices[2][1], _vertices[3][1])); 
        ELEMTYPE uby = std::max(std::max(_vertices[0][1], _vertices[1][1]), std::max(_vertices[2][1], _vertices[3][1])); 
        if (lby == uby) return uby + EPSILON;
        return uby;
    }
    ELEMTYPE lowerBoundZ() const { 
        ELEMTYPE lbz = std::min(std::min(_vertices[0][2], _vertices[1][2]), std::min(_vertices[2][2], _vertices[3][2])); 
        ELEMTYPE ubz = std::max(std::max(_vertices[0][2], _vertices[1][2]), std::max(_vertices[2][2], _vertices[3][2])); 
        if (lbz == ubz) return lbz - EPSILON;
        return lbz;
    }
    ELEMTYPE upperBoundZ() const { 
        ELEMTYPE lbz = std::min(std::min(_vertices[0][2], _vertices[1][2]), std::min(_vertices[2][2], _vertices[3][2])); 
        ELEMTYPE ubz = std::max(std::max(_vertices[0][2], _vertices[1][2]), std::max(_vertices[2][2], _vertices[3][2])); 
        if (lbz == ubz) return ubz + EPSILON;
        return ubz;
    }

    Point& vertices(const COUNTTYPE k) {
        assert(k >= 0 && k < 4);
        return _vertices[k];
    }

    Color texture(const Point& p) const {
        // if there's not texture
        if (!_texture) return color();
        
        Vector xAxis = (_vertices[1] - _vertices[0]);
        Vector yAxis = (_vertices[3] - _vertices[0]);

        ELEMTYPE pX = (p - _vertices[0]) * xAxis.normalize();
        ELEMTYPE pY = (p - _vertices[0]) * yAxis.normalize();

        return _texture -> getPixel(pX, pY, xAxis.norm(), yAxis.norm());
    }

    Color color() const { return _material -> color(); }

    Vector normal(const Point&) const {
        Vector tmp = crossProduct(_vertices[1] - _vertices[0],
                                    _vertices[2] - _vertices[0]).normalize();
        Vector tmp1 = crossProduct(_vertices[2] - _vertices[0],
                                     _vertices[3] - _vertices[0]).normalize();
        assert((tmp - tmp1).norm() < EPSILON);
        return tmp;
    }

    INTERSECTED_TYPE isIntersected(const Ray& ray, ELEMTYPE& distance) const {
        INTERSECTED_TYPE firstTri = _tri0.isIntersected(ray, distance);
        if (firstTri) 
            return firstTri;
        return _tri1.isIntersected(ray, distance);
    }
};


#endif /* RECTANGLE_H */
