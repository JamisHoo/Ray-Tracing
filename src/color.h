/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: color.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 16, 2014
 *  Time: 17:36:25
 *  Description: color class, includes RGB value and wegiht.
 *****************************************************************************/
#ifndef COLOR_H
#define COLOR_H

#include "color.h"

template < class ELEMTYPE, class COUNTTYPE >
class RGBColor {
    ELEMTYPE _weight;
    ELEMTYPE _red;
    ELEMTYPE _green;
    ELEMTYPE _blue;

public:
    RGBColor(const ELEMTYPE r, const ELEMTYPE g, const ELEMTYPE b, const ELEMTYPE w = 0):
        _weight(w), _red(r), _green(g), _blue(b) {
    }

    RGBColor(const RGBColor& c, const ELEMTYPE w) {
        _red = c.red();
        _blue = c.blue();
        _green = c.green();
        if (c.weight() > 0) 
            _weight = w * c.weight();
        else 
            _weight = w;
    }

    ELEMTYPE red() const { return _red; }
    ELEMTYPE green() const { return _green; }
    ELEMTYPE blue() const { return _blue; }
    ELEMTYPE weight() const { return _weight; }

    ELEMTYPE operator[](const COUNTTYPE k) const {
        assert(k >= 0 && k < 3);

        switch (k) {
            case 0:
                return _red;
            case 1:
                return _green;
            case 2:
                return _blue;
            default:
                assert(0);
        }
    }

    RGBColor operator+(const RGBColor& c) const {
        assert(c.weight() >= 0 && weight() >= 0);
        ELEMTYPE totWeight = weight() + c.weight();
        ELEMTYPE totRed = red() * weight() + c.red() * c.weight();
        ELEMTYPE totGreen = green() * weight() + c.green() * c.weight();
        ELEMTYPE totBlue = blue() * weight() + c.blue() * c.weight();
        totRed /= totWeight;
        totGreen /= totWeight;
        totBlue /= totWeight;

        return RGBColor(totRed, totGreen, totBlue, totWeight);
    }

    RGBColor operator+=(const RGBColor& c) {
        ELEMTYPE totWeight = weight() + c.weight();
        assert(c.weight() >= 0 && weight() >= 0);
        ELEMTYPE totRed = red() * weight() + c.red() * c.weight();
        ELEMTYPE totGreen = green() * weight() + c.green() * c.weight();
        ELEMTYPE totBlue = blue() * weight() + c.blue() * c.weight();
        totRed /= totWeight;
        totGreen /= totWeight;
        totBlue /= totWeight;

        _red = totRed;
        _green = totGreen;
        _blue = totBlue;
        _weight = totWeight;
        return RGBColor(totRed, totGreen, totBlue, totWeight);
    }
};

#ifdef DEBUG
template < class ELEMTYPE, class COUNTTYPE >
std::ostream& operator<<(std::ostream& out, const RGBColor<ELEMTYPE, COUNTTYPE>& c) {
    out << "(" << c.red() << ", " << c.green() << ", " << c.blue() << ", " << c.weight() << ")";
    return out;
}
#endif

#endif /* COLOR_H */
