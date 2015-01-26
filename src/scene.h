/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: scene.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 19:17:45
 *  Description: This is scene class, 
 *               which includes all objects and light sources.
 *               providing all related functions.
 *****************************************************************************/
#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "octree.h"
#include "object.h"
#include "ray.h"
#include "lightsource.h"
#include <vector>
#include <algorithm>

using namespace RayTracing;

class Scene {
    std::vector<LightSource*> lights;
#ifdef OCTREE
    Octree<Object, ELEMTYPE, COUNTTYPE, ELEMTYPE, 5> objects;
#else
    std::vector<Object*> objects; 
#endif

#ifdef OCTREE
    const Object* findClosestObject(const Ray& ray, ELEMTYPE& minDistance, bool ignoreTransparentObj = 0) const {
        minDistance = DOUBLE_MAX;
        const Object* minDistanceObj = nullptr;
        
        auto callBackFunction = [&](const Object* obj, ELEMTYPE& dist) -> bool {
            ELEMTYPE distance = DOUBLE_MAX;
            if (obj -> isIntersected(ray, distance) && !(ignoreTransparentObj && obj -> isTransparent())){
                if (distance < minDistance) 
                    minDistance = distance, minDistanceObj = obj;
                dist = distance;
                return 1;
            }
            else return 0;
        };

        bool find = objects.search(ray.origin()[0], ray.origin()[1], ray.origin()[2], 
                                   ray.direction()[0], ray.direction()[1], ray.direction()[2],
                                   callBackFunction);
        // assert(find == bool(minDistanceObj));
        return minDistanceObj;
    }

#else
    const Object* findClosestObject(const Ray& ray, ELEMTYPE& minDistance, bool ignoreTransparentObj = 0) const { 
        minDistance = DOUBLE_MAX;
        const Object* minDistanceObj = nullptr;
        for_each(objects.begin(), objects.end(), [=, &minDistanceObj, &minDistance](const Object* const obj) {
            ELEMTYPE distance = DOUBLE_MAX;
            if (obj -> isIntersected(ray, distance) && distance < minDistance && !(ignoreTransparentObj && obj -> isTransparent())) {
                minDistance = distance;
                minDistanceObj = obj;
            }
        } );
        return minDistanceObj;
    }
#endif
    Color phong(const Ray& ray, const ELEMTYPE distance, const Object* obj) const { 
        Color rtvColor(0, 0, 0);
        // lambert diffuse reflection
        rtvColor += Color(whiteColor, obj -> diffuseReflectivity());
        
        Point reflectPoint = ray.origin() + distance * ray.direction();
        Vector reflectPointNorm = obj -> normal(reflectPoint); 
        Vector view = ELEMTYPE(-1.0) * ray.direction();
        // specular reflection
        for (auto ite: lights) {
            Vector incidentLight = (reflectPoint - ite -> position()).normalize();

            ELEMTYPE blockObjDistance;
            // blocked by other objects
            const Object* blockObj = findClosestObject(Ray(ite -> position(), incidentLight), blockObjDistance, 1);
            if (blockObj && 
                blockObjDistance < (reflectPoint - ite -> position()).norm() - EPSILON) {
                rtvColor += Color(0, 0, 0, shadowDarkness);
                continue;
            }

            Vector reflectedLight = incidentLight - 2 * (incidentLight * reflectPointNorm) * reflectPointNorm; // assert in and n are unit vectors
            assert(std::abs(reflectedLight.norm() - 1) < EPSILON); // reflectedLight should be a unit vector
            
            ELEMTYPE rCv = reflectedLight * view; // cross product of direciton of reflected light and direction of view
            if (rCv >= 0) 
                rtvColor += Color(whiteColor, obj -> specularReflectivity() * 
                                              pow(rCv, obj -> shininess()) / 
                                              std::abs(reflectPointNorm * incidentLight));

        }
        return rtvColor;
    }
    
    Ray getReflectedRay(const Ray& ray, const ELEMTYPE distance, const Object* obj) const {
        Point reflectPoint = ray.origin() + ray.direction() * (distance - EPSILON); 
        Vector incidentLight = ray.direction();
        Vector reflectPointNorm = obj -> normal(reflectPoint);
        if (reflectPointNorm * incidentLight > 0)
            reflectPointNorm *= -1;
        assert(reflectPointNorm * incidentLight < 0);
        Vector reflectedLight = incidentLight - 2 * (incidentLight * reflectPointNorm) * reflectPointNorm;
        
        return Ray(reflectPoint, 
                   reflectedLight, 
                   ray.refractiveIndex(), 
                   ray.intensity() * obj -> reflectionWeight());

    }
    
    Ray getRefractedRay(const Ray& ray, const ELEMTYPE distance, const Object* obj) const {
        // algorithm:
        // n: normal, points to refraction point
        // l: incident light
        // t: refracted light
        // eta: relative refractive index
        // cos(theta1) = n * l
        // cos(theta2) = sqrt(1 - (1 - (cos^2(theta1)) / eta ^ 2)
        // t = l / eta + (cos(theta1) / eta - cos(theta2)) * n
        Point refractPoint = ray.origin() + ray.direction() * (distance + EPSILON);
        Vector incidentLight = ray.direction();
        Vector refractPointNorm = obj -> normal(refractPoint);
        ELEMTYPE cosIncidentAngle = refractPointNorm * incidentLight;
        ELEMTYPE objRefractiveIndex = obj -> refractiveIndex();
        if (cosIncidentAngle < 0) {
            cosIncidentAngle *= -1;
            refractPointNorm *= -1;
            objRefractiveIndex = 1;
        }
        ELEMTYPE relativeRefractiveIndex = objRefractiveIndex / ray.refractiveIndex();
        ELEMTYPE cosRefractionAngle = 1 - (1 - pow(cosIncidentAngle, 2)) / pow(relativeRefractiveIndex, 2);

        if (cosRefractionAngle < 0) { // total reflection
            Ray reflectedRay = getReflectedRay(ray, distance, obj);
            reflectedRay.setIntensity(ray.intensity() * obj -> refractionWeight());
            return reflectedRay;
        }

        cosRefractionAngle = sqrt(cosRefractionAngle);
        
        Vector refractDirection = incidentLight * (ELEMTYPE(1) / relativeRefractiveIndex) - 
                                  (cosIncidentAngle / relativeRefractiveIndex - cosRefractionAngle) * refractPointNorm; 

        assert(std::abs(refractDirection.norm() - 1) < EPSILON); 
        Ray refractedRay(refractPoint, 
                         refractDirection, 
                         objRefractiveIndex, 
                         ray.intensity() * obj -> refractionWeight());
        return refractedRay;
    }
    

public:
#ifdef OCTREE
    // TODO: the range need changing if coordinate of objects are changed
    Scene(): objects(-10000, -10000, -10000, 10000, 10000, 10000) { }
    void insert(Object* obj) { 
        objects.insert(obj); 
    }
#else 
    Scene() { }
    void insert(Object* obj) { objects.push_back(obj); }
#endif

    ~Scene() { }
    void insert(LightSource* l) { lights.push_back(l); }

    void rayTrace(const Ray& ray, Color& color, 
                  const COUNTTYPE recursionDepth = 0) const {
        if (recursionDepth > maxRecursionDepth) return;
        // the light is too weak
        if (ray.intensity() < ignoreWeight) return;

        // find the closest object
        ELEMTYPE objDistance;
        const Object* closestObj = findClosestObject(ray, objDistance);
        // if not found
        if (!closestObj) return;
        
        // if found
        // ambient occlusion
        color += Color(closestObj -> texture(ray.origin() + objDistance * ray.direction()), 
                       ray.intensity() * closestObj -> ambientCoefficient());
        // local illumination model
        color += Color(phong(ray, objDistance, closestObj), ray.intensity());
        
        // calculate reflect ray and refract ray recursive trace
        if (closestObj -> reflectionWeight()) {
            Ray reflectedRay = getReflectedRay(ray, objDistance, closestObj);
            if (reflectedRay.intensity()) rayTrace(reflectedRay, color, recursionDepth + 1);
        }
        if (closestObj -> refractionWeight()) {
            Ray refractedRay = getRefractedRay(ray, objDistance, closestObj);
            if (refractedRay.intensity()) rayTrace(refractedRay, color, recursionDepth + 1);
        }
    }
};

#endif /* SCENE_H */
