/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: common.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 14:50:09
 *  Description: This file includes all basic declaration, statement...
 *****************************************************************************/
#ifndef COMMON_H
#define COMMON_H

#ifdef DEBUG
#include <iostream>
#include <cstdio>
using std::endl;
using std::cout;
using std::cerr;
using std::flush;
#endif

#include "vector3.h"
#include "color.h"
#include <cmath>
#include <assert.h>
#include <cfloat>
#include <limits>

namespace RayTracing {
    using ELEMTYPE = double;
    using COUNTTYPE = int;
    using Vector = Vector3<ELEMTYPE, COUNTTYPE>;
    using Point = Vector3<ELEMTYPE, COUNTTYPE>;
    using Color = RGBColor<ELEMTYPE, COUNTTYPE>;

    const ELEMTYPE PI = acos(-1);
    constexpr ELEMTYPE EPSILON = 1e-5;
    const Color blackColor(0, 0, 0);
    const Color whiteColor(255, 255, 255);
    constexpr ELEMTYPE DOUBLE_MAX = std::numeric_limits<double>::max();
    constexpr COUNTTYPE maxRecursionDepth = 4;
    constexpr ELEMTYPE ignoreWeight = 1e-1;
    constexpr ELEMTYPE shadowDarkness = 3;
}

#endif /* COMMON_H */
