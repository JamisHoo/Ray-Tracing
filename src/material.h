/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: material.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: May. 27, 2014
 *  Time: 17:28:00
 *  Description: material quality of object
 *****************************************************************************/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"

using namespace RayTracing;

class Material {
    Color _color;

    ELEMTYPE _reflectionWeight;
    ELEMTYPE _refractionWeight;

    ELEMTYPE _ambientCoefficient; // ambient reflection constant, 
                                  // the ratio of reflection of the ambient term present in all points in the scene rendered 
    ELEMTYPE _diffuseReflectivity; // diffuse reflection constant, 
                                   // the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance)
    ELEMTYPE _specularReflectivity; // specular reflection constant, 
                                    // the ratio of reflection of the specular term of incoming light 
    ELEMTYPE _refractiveIndex;  
    ELEMTYPE _shininess; // shininess constant for this material, 
                         // is larger for surfaces that are smoother and more mirror-like. 
                         // When this constant is large the specular highlight is small.
    bool _transparent;

public:
    Material(const Color& c,
             const ELEMTYPE ac, const ELEMTYPE dr, 
             const ELEMTYPE sr, const ELEMTYPE s,
             const ELEMTYPE r = 1.0, const ELEMTYPE rlw = 0.0,
             const ELEMTYPE rrw = 0.0, const bool tp = 0):
        _color(c),
        _ambientCoefficient(ac), _diffuseReflectivity(dr),
        _specularReflectivity(sr), _shininess(s),
        _refractiveIndex(r), _reflectionWeight(rlw),
        _refractionWeight(rrw), _transparent(tp) {
    }

    void setColor(const Color& c) { _color = c; }
    Color color() const { return _color; }
    void setReflectionWeight(const ELEMTYPE w) { _reflectionWeight = w; }
    ELEMTYPE reflectionWeight() const { return _reflectionWeight; }
    void setReflactionWeight(const ELEMTYPE w) { _refractionWeight = w; }
    ELEMTYPE refractionWeight() const { return _refractionWeight; }
    void setAmbientCoefficient(const ELEMTYPE w) { _ambientCoefficient = w; }
    ELEMTYPE ambientCoefficient() const { return _ambientCoefficient; }
    void setDiffuseReflectivity(const ELEMTYPE w) { _diffuseReflectivity = w; }
    ELEMTYPE diffuseReflectivity() const { return _diffuseReflectivity; }
    void setSpecularReflectivity(const ELEMTYPE w) { _specularReflectivity = w; }
    ELEMTYPE specularReflectivity() const { return _specularReflectivity; }
    void setRefractiveIndex(const ELEMTYPE w) { _refractiveIndex = w; }
    ELEMTYPE refractiveIndex() const { return _refractiveIndex; }
    void setShininess(const ELEMTYPE w) { _shininess = w; }
    ELEMTYPE shininess() const { return _shininess; }
    void setTransparent(const bool t) { _transparent = t; }
    bool isTransparent() const { return _transparent; }
};

#endif /* MATERIAL_H */
