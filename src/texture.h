/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: texture.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 18, 2014
 *  Time: 14:36:47
 *  Description: texture, including a picture. 
 *****************************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "vector"

using namespace RayTracing;

class Texture {
public:
    enum FILLMODE { STRETCH = 0, PRESERVEASPECTFIT = 1, PRESERVEASPECTCROP = 2,
                    TILE = 3 };
private:
    COUNTTYPE _length;
    COUNTTYPE _width;
    std::vector< std::vector<Color> > _pixels;
    COUNTTYPE pixelsNum;
    ELEMTYPE _scale;
    FILLMODE _fillMode;
public:
    Texture(const COUNTTYPE l, const COUNTTYPE w, const ELEMTYPE s = 1): 
        _length(l), _width(w), pixelsNum(0), _scale(s), _fillMode(TILE){
    }

    COUNTTYPE length() const { return _length; }
    COUNTTYPE width() const { return _width; }
    ELEMTYPE scale() const { return _scale; }
    FILLMODE fillMode() const { return _fillMode; }
    void setFillMode(const FILLMODE m) { _fillMode = m; }
    void setScale(const ELEMTYPE s) { _scale = s; }
    void setLength(const COUNTTYPE l) { _length = l; }
    void setWidth(const COUNTTYPE w) { _width = w; }

    void clear() { 
        _pixels.clear(); 
        pixelsNum = _length = _width = 0;
        _scale = 1;
        _fillMode = TILE;
    }

    void setPixel(const Color& c) {
        assert(pixelsNum < _length * _width);
        while (_pixels.size() <= pixelsNum / _length) _pixels.push_back(std::vector<Color>());
        _pixels[pixelsNum / _length].push_back(c);
        ++pixelsNum;
    }

    Color getPixel(const ELEMTYPE x, const ELEMTYPE y, 
                   const ELEMTYPE l, const ELEMTYPE w) const  {
    // if using tile mode, l and w are useless
        assert(pixelsNum == _length * _width);
        switch (_fillMode) {
            case STRETCH: {
                ELEMTYPE xx = x;
                ELEMTYPE yy = y;
                while (xx >= l) xx -= EPSILON;
                while (yy >= w) yy -= EPSILON;
                return _pixels[COUNTTYPE(_width * yy / w)][COUNTTYPE(_length * xx / l)];
            }
            case PRESERVEASPECTFIT:
            case PRESERVEASPECTCROP:
                // TODO: implement these if necessary
            case TILE: {
                COUNTTYPE xx = COUNTTYPE(y / _scale) % _width;
                COUNTTYPE yy = COUNTTYPE(x / _scale) % _length;
                if (xx < 0) xx += _width;
                if (yy < 0) yy += _length;
                assert(xx >= 0 && yy >= 0);
                return _pixels[xx][yy];
            }
            default:
                assert(0); // should never happen
        }
    }
};

#endif /* TEXTURE_H */
