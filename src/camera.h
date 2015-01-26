/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: camera.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 11, 2014
 *  Time: 15:31:24
 *  Description: This is a camera class which includes the view point,
 *               direction of view(spherical coordinate system, phi, theta), 
 *               size of retina, distance from view point to retina(rho).
 *****************************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "ray.h"
#include <vector>

class Camera {
    Point _viewPoint;

    // three parameters below are used for depth of field
    ELEMTYPE _focalLength; // length from retina to focal plane
    ELEMTYPE _apertureSize; // radius 
    COUNTTYPE _numRays;

    // spherical coordinate system(radian)
    ELEMTYPE _distanceR; // distance from retina to view Point
    ELEMTYPE _angleTheta; // angle between z-axis and view direction
    ELEMTYPE _anglePhi; // angle between x-axis and the projection of view direction on x-y plane

    // default refractive index
    const ELEMTYPE _refractiveIndex;
    // retina size
    const ELEMTYPE _retinaLength;
    const ELEMTYPE _retinaWidth;
    // resolution ratio
    const COUNTTYPE _resolutionLength;
    const COUNTTYPE _resolutionWidth;

    // four vertives of retina
    Point vertives[4];
    enum { topLeft = 0, topRight, bottomLeft, bottomRight };

    void retinaChanged() {
        vertives[topLeft] = Point(-_retinaWidth, -_retinaLength, distanceR());
        vertives[topRight] = Point(-_retinaWidth, _retinaLength, distanceR());
        vertives[bottomLeft] = Point(_retinaWidth, -_retinaLength, distanceR());
        vertives[bottomRight] = Point(_retinaWidth, _retinaLength, distanceR());

        for (int i = topLeft; i <= bottomRight; ++i) 
        // first rotate around y-axis
        // right-multiply matrix [ cos(theta) 0 -sin(theta) ]
        //                       [          0 1           0 ]
        //                       [ sin(theta) 0  cos(theta) ]
        // then rotate around z-axis
        // right-multiply matrix [  cos(phi) sin(phi) 0 ]
        //                       [ -sin(phi) cos(phi) 0 ]
        //                       [         0        0 1 ]
        // after the rotations, vector(x, y, z) changes to 
        // (x * cos(theta) * cos(phi) + z * sin(theta) * cos(phi) - y * sin(phi),
        //  x * cos(theta) * sin(phi) + z * sin(theta) * sin(phi) + y * cos(phi),
        //  z * cos(theta) - x * sin(phi))
            vertives[i] = Point(vertives[i][0] * cos(angleTheta()) * cos(anglePhi()) 
                                    + vertives[i][2] * sin(angleTheta()) * cos(anglePhi())
                                    - vertives[i][1] * sin(anglePhi()), 
                                vertives[i][0] * cos(angleTheta()) * sin(anglePhi())
                                    + vertives[i][2] * sin(angleTheta()) * sin(anglePhi()) 
                                    + vertives[i][1] * cos(anglePhi()),
                                vertives[i][2] * cos(angleTheta()) 
                                    - vertives[i][0] * sin(angleTheta())
                                );
    }
        
public:
    Camera(const Point& vp, const ELEMTYPE d, 
           const COUNTTYPE rh, const COUNTTYPE rw, 
           const ELEMTYPE rScale, const ELEMTYPE ri):
        _viewPoint(vp), _distanceR(d), 
        _retinaLength(rh * rScale), _retinaWidth(rw * rScale),
        _resolutionLength(rh), _resolutionWidth(rw), _refractiveIndex(ri),
        _angleTheta(0), _anglePhi(0), _numRays(1) {
        // disable DOF by default
        retinaChanged();
    }

    // non-DOF function, deprecated
    Ray getRay(const COUNTTYPE x, const COUNTTYPE y) const {
        assert(0);
        assert(x >= 0 && x < resolutionLength());
        assert(y >= 0 && y < resolutionWidth());

        Vector direction = vertives[topLeft]
                           + ELEMTYPE(x) / resolutionLength() * (vertives[topRight] - vertives[topLeft])
                           + ELEMTYPE(y) / resolutionWidth() * (vertives[bottomLeft] - vertives[topLeft]);
        return Ray(viewPoint(), direction);
    }

    // DOF version
    std::vector<Ray> getRays(const COUNTTYPE x, const COUNTTYPE y) const {
        /* 
         * x and d' are inclined, from view point to focal point
         * |<-------------x--------------->|
         * |<--d'---->|               focal plane
         *                                 |
         *            |                    |. focal point
         *            |                    |
         * view point |                    |
         * .__________|____________________|
         *            |                    |
         *            |                    |
         *         retina                  |
         *                                 |
         * |<---d---->|<--------f--------->|                                
         *
         *
         * x = d' * (d + f) / d
        */
        assert(x >= 0 && x < resolutionLength());
        assert(y >= 0 && y < resolutionWidth());

        std::vector<Ray> rays;

        Vector direction = vertives[topLeft]
                           + ELEMTYPE(x) / resolutionLength() * (vertives[topRight] - vertives[topLeft])
                           + ELEMTYPE(y) / resolutionWidth() * (vertives[bottomLeft] - vertives[topLeft]);
        rays.push_back(Ray(viewPoint(), direction, _refractiveIndex));
        ELEMTYPE dApos = direction.norm();
        direction = direction.normalize();

        //Point retinaPoint = viewPoint() + dApos * direction;
        Point focalPoint = viewPoint() + (dApos * (distanceR() + focalLength()) 
                                       / distanceR()) * direction;

        for (COUNTTYPE i = 0; i < numberRays() - 1; ++i) {
            ELEMTYPE randRadius = ELEMTYPE(rand()) / RAND_MAX * apertureSize();
            ELEMTYPE randAngle1 = ELEMTYPE(rand()) / RAND_MAX * 2 * PI;
            ELEMTYPE randAngle2 = ELEMTYPE(rand()) / RAND_MAX * 2 * PI;

            Point randPoint(viewPoint()[0] + randRadius * sin(randAngle1) * cos(randAngle2),
                            viewPoint()[1] + randRadius * sin(randAngle1) * sin(randAngle2),
                            viewPoint()[2] + randRadius * cos(randAngle1));

            rays.push_back(Ray(randPoint, focalPoint - randPoint, _refractiveIndex));
        }

        return rays;
    }

    ELEMTYPE retinaScale() const { return _retinaLength / _resolutionLength; }
    COUNTTYPE resolutionLength() const { return _resolutionLength; }
    COUNTTYPE resolutionWidth() const { return _resolutionWidth; }

    void setViewPoint(const Point& p) { _viewPoint = p; }
    Point viewPoint() const { return _viewPoint; }
    void setAngleTheta(const ELEMTYPE theta) { _angleTheta = theta; retinaChanged(); }
    ELEMTYPE angleTheta() const { return _angleTheta; }
    void setAnglePhi(const ELEMTYPE phi) { _anglePhi = phi; retinaChanged(); }
    ELEMTYPE anglePhi() const { return _anglePhi; }
    void setDistanceR(const ELEMTYPE r) { _distanceR = r; retinaChanged(); }
    ELEMTYPE distanceR() const { return _distanceR; }
    void setFocalLength(const ELEMTYPE fl) { _focalLength = fl; }
    ELEMTYPE focalLength() const { return _focalLength; }
    void setApertureSize(const ELEMTYPE as) { _apertureSize = as; }
    ELEMTYPE apertureSize() const { return _apertureSize; }
    void setNumberRays(const COUNTTYPE numRays) { _numRays = numRays; }
    COUNTTYPE numberRays() const { return _numRays; }


};

#endif /* CAMERA_H */
