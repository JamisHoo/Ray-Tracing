/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: parser.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: May. 31, 2014
 *  Time: 10:47:03
 *  Description: obj file parser
 *****************************************************************************/
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "scene.h"
#include "camera.h"
#include "triangle.h"
#include "rectangle.h"
#include "sphere.h"
#include "lightsource.h"
#include "material.h"
#include "texture.h"

std::string removeSpaces(const std::string& str);

class ObjParser {
    class MtlParser {
        std::map<std::string, std::pair<Material*, Texture*> > mtl;
        std::pair<Material*, Texture*> _activeMtl;
        
        void parse(const std::string& line) {
            // delete space characters
            std::string str = removeSpaces(line);
            if (!str.length()) return;
            if (str[0] == '#') return; //comments

            //new meterial and texture
            if (str.length() >= 6 && str.substr(0, 6) == "newmtl") {
                std::istringstream strs(str);
                std::string mtlname;
                ELEMTYPE colorR, colorG, colorB, ac, dr, sr, s, r, rlw, rrw;
                bool tp;
                
                //parameters of material
                bool res = strs >> mtlname >> mtlname >> colorR >> colorG >> colorB
                                >> ac >> dr >> sr >> s >> r >> rlw >> rrw >> tp;
                assert(res);
                assert(mtl.find(mtlname) == mtl.end());
                
                Material* material = new Material(Color(colorR, colorG, colorB), 
                                                  ac, dr, sr, s, r, rlw, rrw, tp);

                Texture* texture = nullptr;
                std::string textureFilename;
                //new texture, optional
                if (strs >> textureFilename) {
                    texture = new Texture(0, 0);
                    loadTextureFromPic(texture, textureFilename);
                    ELEMTYPE textureScale;
                    if (strs >> textureScale) 
                        if (textureScale > 0) 
                            texture -> setScale(textureScale);
                        else if (textureScale == 0)
                            texture -> setFillMode(Texture::STRETCH);
                        else assert(0);
                }
            
                mtl[mtlname] = std::make_pair(material, texture);
                return;
            }
            assert(0);
        }

        void loadTextureFromPic(Texture* texture, const std::string& filename) {
            texture -> clear();
            cv::Mat_<cv::Vec3b> textureImage = cv::imread(filename.c_str());

            //if (!textureImage.data) return;   
            assert(textureImage.data);

            texture -> setLength(textureImage.cols);
            texture -> setWidth(textureImage.rows);

            for (auto ite: textureImage) 
                texture -> setPixel(Color(ite[2], ite[1], ite[0]));
        }

    public:
        MtlParser(): _activeMtl(std::make_pair(nullptr, nullptr)) { }

        std::pair<Material*, Texture*> activeMtl() const {
            assert(_activeMtl.first);
            return _activeMtl;
        }

        void useMtl(const std::string& mtlname) {
            auto ite = mtl.find(mtlname);
            assert(ite != mtl.end());
            _activeMtl = ite -> second;
        }

        void loadMtlFile(const std::string& filename) {
            std::ifstream fin(filename.c_str());
            assert(fin.is_open());

            std::string line;
            while (getline(fin, line)) parse(line);
        }

    };

    std::vector<Point> vertices;
    std::vector<Object*> objects;
    std::vector<LightSource*> lights;
    std::ifstream fin;
    Scene& scene;
    MtlParser mtlParser;

public:
    ObjParser(const std::string& filename, Scene& s): scene(s) {
        fin.open(filename.c_str());
        assert(fin.is_open());

        std::string line;
        while (getline(fin, line)) parse(line);
    }

    ~ObjParser() { 
        fin.close(); 
        for (auto ite: objects) delete ite;
        for (auto ite: lights) delete ite;
    }

    void parse(const std::string& line) {
        // delete space characters
        std::string str = removeSpaces(line);
        if (!str.length()) return;
        if (str[0] == '#') return; // comments

        if (str[0] == 'v') {  // vertex
            std::istringstream strs(str);
            strs.get();

            ELEMTYPE x, y, z;
            bool res = strs >> x >> y >> z;

            assert(res);

            vertices.push_back(Point(x, y, z));
            return;
        }

        if (str[0] == 'f') { //face
            std::istringstream strs0(str), strs1(str);
            strs0.get();
            strs1.get();

            COUNTTYPE v0, v1, v2, v3;
            ELEMTYPE r;
            
            COUNTTYPE numParas = 0;
            std::string tmp;
            while (strs0 >> tmp) ++numParas;
            
            switch (numParas) {
                case 1: {
                    strs1 >> v0;
                    LightSource* light = new LightSource(vertices[v0], 
                                             mtlParser.activeMtl().first -> color());
                    lights.push_back(light);
                    scene.insert(light);
                    break;
                }
                case 2: {
                    strs1 >> v0 >> r;
                    Sphere* obj = new Sphere(vertices[v0], r, 
                                             mtlParser.activeMtl().first, 
                                             mtlParser.activeMtl().second);
                    objects.push_back(obj);
                    scene.insert(obj);
                    break;
                }
                case 3: {
                    strs1 >> v0 >> v1 >> v2;
                    Triangle* obj = new Triangle(vertices[v0], vertices[v1], vertices[v2], 
                                                 mtlParser.activeMtl().first, 
                                                 mtlParser.activeMtl().second);
                    objects.push_back(obj);
                    scene.insert(obj);
                    break;
                }
                case 4: {
                    strs1 >> v0 >> v1 >> v2 >> v3;
                    Rectangle* obj = new Rectangle(vertices[v0], vertices[v1], 
                                                   vertices[v2], vertices[v3],
                                                   mtlParser.activeMtl().first,
                                                   mtlParser.activeMtl().second);
                    objects.push_back(obj);
                    scene.insert(obj);
                    break;
                }
                default:
                    assert(0);
            }
            return;
        }

        // material and texture lib
        if (str.length() >= 6 && str.substr(0, 6) == "mtllib") {
            mtlParser.loadMtlFile(removeSpaces(str.substr(6)));
            return;
        }
        
        if (str.length() >= 6 && str.substr(0, 6) == "usemtl") {
            mtlParser.useMtl(removeSpaces(str.substr(6)));
            return;
        }
    
        assert(0);
    }
};


class CmrParser {
    Point viewPoint;
    ELEMTYPE focalLength;
    ELEMTYPE apertureSize;
    COUNTTYPE numRays;
    ELEMTYPE distanceR;
    ELEMTYPE angleTheta;
    ELEMTYPE anglePhi;
    ELEMTYPE refractiveIndex;
    COUNTTYPE resolutionLength;
    COUNTTYPE resolutionWidth;
    ELEMTYPE retinaRatio;
    COUNTTYPE AARatio;
    Camera* camera;
public:
    CmrParser(const std::string& filename) {
        std::ifstream fin(filename.c_str());
        std::stringstream strs;
        std::string line;
        while (getline(fin, line)) 
            if (line.length() && line[0] == '#') continue;
            else strs << removeSpaces(line) << ' ';
        fin.close();

        bool res = strs >> viewPoint[0] >> viewPoint[1] >> viewPoint[2]
                        >> angleTheta >> anglePhi 
                        >> refractiveIndex
                        >> distanceR 
                        >> resolutionLength >> resolutionWidth 
                        >> retinaRatio 
                        >> focalLength >> apertureSize >> numRays 
                        >> AARatio;
        assert(res);
        camera = new Camera(viewPoint, distanceR, 
                            resolutionLength * AARatio,
                            resolutionWidth * AARatio,
                            retinaRatio / AARatio,
                            refractiveIndex);
        camera -> setAngleTheta(angleTheta * PI / 180);
        camera -> setAnglePhi(anglePhi * PI / 180);
        camera -> setFocalLength(focalLength);
        camera -> setApertureSize(apertureSize);
        camera -> setNumberRays(numRays);
    }

    Camera* getCamera() const {
        return camera;
    }

    COUNTTYPE aaRatio() const { return AARatio; }

    ~CmrParser() { delete camera; }
};

std::string removeSpaces(const std::string& str) {
    // delete space characters
    std::string spaces = "\n\r\t\v\f ";
    auto s = str.find_first_not_of(spaces);
    auto e = str.find_last_not_of(spaces);
    if (s == std::string::npos || e == std::string::npos) return "";
    return str.substr(s, e - s + 1);
}

#endif /* PARSER_H */
