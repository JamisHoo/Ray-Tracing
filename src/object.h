/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: object.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 14:50:36
 *  Description: Class Object is base class of all objects.
 *****************************************************************************/
#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "ray.h"
#include "texture.h"
#include "material.h"

using namespace RayTracing;

class Object {
protected:
    const Material* _material;
    const Texture* _texture;
public:
    enum INTERSECTED_TYPE { INTERSECTED_IN = -1, MISS = 0, INTERSECTED = 1 };

    Object(const Material* mat, const Texture* tex): 
        _material(mat), _texture(tex) { }

    virtual ~Object() { }
    virtual Vector normal(const Point&) const = 0;
    virtual INTERSECTED_TYPE isIntersected(const Ray&, ELEMTYPE& distance) const = 0;

    void setTexture(const Texture* t) { _texture = t; }
    void setMaterial(const Material* mat) { _material = mat; }

    ELEMTYPE reflectionWeight() const { return _material -> reflectionWeight(); }
    ELEMTYPE refractionWeight() const { return _material -> refractionWeight(); }
    ELEMTYPE ambientCoefficient() const { return _material -> ambientCoefficient(); }
    ELEMTYPE diffuseReflectivity() const { return _material -> diffuseReflectivity(); }
    ELEMTYPE specularReflectivity() const { return _material -> specularReflectivity(); }
    ELEMTYPE refractiveIndex() const { return _material -> refractiveIndex(); }
    ELEMTYPE shininess() const { return _material -> shininess(); }
    bool isTransparent() const { return _material -> isTransparent(); }

    virtual Color texture(const Point&) const = 0;
    virtual Color color() const = 0; 
    virtual ELEMTYPE lowerBoundX() const = 0;
    virtual ELEMTYPE upperBoundX() const = 0;
    virtual ELEMTYPE lowerBoundY() const = 0;
    virtual ELEMTYPE upperBoundY() const = 0;
    virtual ELEMTYPE lowerBoundZ() const = 0;
    virtual ELEMTYPE upperBoundZ() const = 0;
    // TODO: virtual change position
    // implement if necessary
};

#endif /* OBJECT_H */
