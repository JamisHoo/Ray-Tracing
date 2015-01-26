/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing 
 *  Filename: octree.h 
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 26, 2014
 *  Time: 14:36:01
 *  Description: An octree used to accelerate intesection.
 *****************************************************************************/
#ifndef OCTREE_H
#define OCTREE_H
#include <vector>
#include <assert.h>
#include <cmath>


// DATATYPE: referenced data
// ELEMTYPE: type of coordinate, such as int, float, double...
// INTTYPE: integer type, such as int, long ...
// REALTYPE: type fof coordinate for use of calculation, such as float, double...


template < class DATATYPE, class ELEMTYPE, class INTTYPE, class REALTYPE, INTTYPE MAXOBJCOUNT >
class TreeNode {
    class LeafNode {
        std::vector< const DATATYPE* > objects;

    public:
        INTTYPE size() const { return objects.size(); }
        void insert(const DATATYPE* obj) {
            objects.push_back(obj);
        }
        const DATATYPE* operator[](const INTTYPE i) const {
            assert(i < size());
            return objects[i];
        }
    };

    LeafNode* leafnode;
    TreeNode* subtree[8];
    TreeNode* parentTree;
    TreeNode* rootNode;
    ELEMTYPE minX, minY, minZ, maxX, maxY, maxZ;
    enum Surface { None = -1, Front = 0, Back, Left, Right, Up, Down };
    constexpr static ELEMTYPE EPSILON = 1e-5;

    bool intersectRange(const ELEMTYPE minx, const ELEMTYPE miny, const ELEMTYPE minz,
                        const ELEMTYPE maxx, const ELEMTYPE maxy, const ELEMTYPE maxz) const {
        return std::max(minx, minX) < std::min(maxx, maxX) &&
               std::max(miny, minY) < std::min(maxy, maxY) && 
               std::max(minz, minZ) < std::min(maxz, maxZ);
    }
    
    bool inRange(const ELEMTYPE x, const ELEMTYPE y, const ELEMTYPE z) const {
        return (x >= minX && x <= maxX && 
                y >= minY && y <= maxY &&
                z >= minZ && z <= maxZ);
    }

    bool intersectSurface(const ELEMTYPE pointX, const ELEMTYPE pointY, const ELEMTYPE pointZ,
                          const ELEMTYPE direcX, const ELEMTYPE direcY, const ELEMTYPE direcZ,
                          ELEMTYPE& interX, ELEMTYPE& interY, ELEMTYPE& interZ,
                          const Surface surface) const {
        assert(std::abs(pow(direcX, 2) + pow(direcY, 2) + pow(direcZ, 2) - 1) < EPSILON);
        ELEMTYPE n[3]; // normal of the surface
        ELEMTYPE a[3]; // any vertex of the surface
        switch (surface) {
            case Front:
                n[0] =  1, n[1] = n[2] = 0;
                a[0] = maxX, a[1] = minY, a[2] = minZ;
                break;
            case Back:
                n[0] = -1, n[1] = n[2] = 0;
                a[0] = minX, a[1] = minY, a[2] = minZ;
                break;
            case Left:
                n[1] = -1, n[0] = n[2] = 0;
                a[0] = minX, a[1] = minY, a[2] = minZ;
                break;
            case Right:
                n[1] =  1, n[0] = n[2] = 0;
                a[0] = minX, a[1] = maxY, a[2] = minZ;
                break;
            case Up:
                n[2] =  1, n[0] = n[1] = 0;
                a[0] = minX, a[1] = minY, a[2] = maxZ;
                break;
            case Down:
                n[2] = -1, n[0] = n[1] = 0;
                a[0] = minX, a[1] = minY, a[2] = minZ;
                break;
            default:
                // should never happen
                assert(0);
        }

        REALTYPE PPapos = REALTYPE(a[0] - pointX) * n[0] + 
                          REALTYPE(a[1] - pointY) * n[1] + 
                          REALTYPE(a[2] - pointZ) * n[2];

        REALTYPE PDapos = PPapos / (direcX * n[0] + direcY * n[1] + direcZ * n[2]);
        ELEMTYPE D[3];
        // D[3] is intersection point
        D[0] = PDapos * direcX + pointX;
        D[1] = PDapos * direcY + pointY;
        D[2] = PDapos * direcZ + pointZ;
        interX = D[0], interY = D[1], interZ = D[2];

        switch (surface) {
            case Front:
                assert(std::abs(D[0] - maxX) < EPSILON);
                if (D[1] >= minY && D[1] <= maxY && D[2] >= minZ && D[2] <= maxZ) 
                    return 1;
                else return 0;
            case Back:
                assert(std::abs(D[0] - minX) < EPSILON);
                if (D[1] >= minY && D[1] <= maxY && D[2] >= minZ && D[2] <= maxZ) 
                    return 1;
                else return 0;
            case Left:
                assert(std::abs(D[1] - minY) < EPSILON);
                if (D[0] >= minX && D[0] <= maxX && D[2] >= minZ && D[2] <= maxZ) 
                    return 1;
                else return 0;
            case Right:
                assert(std::abs(D[1] - maxY) < EPSILON);
                if (D[0] >= minX && D[0] <= maxX && D[2] >= minZ && D[2] <= maxZ) 
                    return 1;
                else return 0;
            case Up:
                assert(std::abs(D[2] - maxZ) < EPSILON);
                if (D[0] >= minX && D[0] <= maxX && D[1] >= minY && D[1] <= maxY) 
                    return 1;
                else return 0; 
            case Down:
                assert(std::abs(D[2] - minZ) < EPSILON);
                if (D[0] >= minX && D[0] <= maxX && D[1] >= minY && D[1] <= maxY) 
                    return 1;
                else return 0; 
            default:
                assert(0);
        }
    }

public:
    TreeNode(const ELEMTYPE minx, const ELEMTYPE miny, const ELEMTYPE minz,
             const ELEMTYPE maxx, const ELEMTYPE maxy, const ELEMTYPE maxz,   
             TreeNode* parent): 
        minX(minx), minY(miny), minZ(minz), 
        maxX(maxx), maxY(maxy), maxZ(maxz),
        parentTree(parent), leafnode(0) {
        assert(minx < maxx && miny < maxy && minz < maxz);
        memset(subtree, 0, sizeof(TreeNode*) * 8);
        leafnode = new LeafNode;
        rootNode = (parent? parent -> rootNode: this);
    }

    ~TreeNode() {
        delete leafnode;
        leafnode = 0;
        for (INTTYPE i = 0; i < 8; ++i) { delete subtree[i]; subtree[i] = 0; }
    }

    bool isRoot() const { return !parentTree; }
    
    bool isLeafNode() const {
        bool hasSubtree = 0;
        for (INTTYPE i = 0; i < 8; ++i) if (subtree[i]) hasSubtree = 1;
        assert(bool(leafnode) ^ hasSubtree);
        return leafnode;
    }

    INTTYPE size() const {
        if (leafnode) 
            return leafnode -> size();
        INTTYPE sz = 0;
        for (INTTYPE i = 0; i < 8; ++i)
            sz += subtree[i] -> size();
        return sz;
    }

    bool insert(const DATATYPE* obj) {
    // returns false if obj doesn't intersect with my range
    // returns true if inserting successfully
        ELEMTYPE minx = obj -> lowerBoundX();
        ELEMTYPE maxx = obj -> upperBoundX();
        ELEMTYPE miny = obj -> lowerBoundY();
        ELEMTYPE maxy = obj -> upperBoundY();
        ELEMTYPE minz = obj -> lowerBoundZ();
        ELEMTYPE maxz = obj -> upperBoundZ();

        if (!intersectRange(minx, miny, minz, maxx, maxy, maxz)) return 0;
        // try to insert to subtrees
        if (!isLeafNode()) {
            bool rtv = 0;
            for (INTTYPE i = 0; i < 8; ++i) 
                if (subtree[i] -> insert(obj)) {
                    rtv = 1;
                }
            assert(rtv);
            return 1;
        }
        else if (leafnode) {
            leafnode -> insert(obj);
            if (leafnode -> size() < MAXOBJCOUNT) return 1;
            // if exceed size upper bound, split
            ELEMTYPE midX = REALTYPE(minX + maxX) / 2;
            ELEMTYPE midY = REALTYPE(minY + maxY) / 2;
            ELEMTYPE midZ = REALTYPE(minZ + maxZ) / 2;
            subtree[0] = new TreeNode(minX, minY, minZ, midX, midY, midZ, this);
            subtree[1] = new TreeNode(midX, minY, minZ, maxX, midY, midZ, this);
            subtree[2] = new TreeNode(minX, midY, minZ, midX, maxY, midZ, this);
            subtree[3] = new TreeNode(midX, midY, minZ, maxX, maxY, midZ, this);
            subtree[4] = new TreeNode(minX, minY, midZ, midX, midY, maxZ, this);
            subtree[5] = new TreeNode(midX, minY, midZ, maxX, midY, maxZ, this);
            subtree[6] = new TreeNode(minX, midY, midZ, midX, maxY, maxZ, this);
            subtree[7] = new TreeNode(midX, midY, midZ, maxX, maxY, maxZ, this);
            for (INTTYPE i = 0; i < 8; ++i)
                for (INTTYPE j = 0; j < leafnode -> size(); ++j)
                    subtree[i] -> insert((*leafnode)[j]);
            delete leafnode;
            leafnode = 0;
            return 1;
        }

        // should never happen
        assert(0);
    }

    template < class CALLBACKFUNC >
    // (ox, oy, oz) starting point coordinate
    // (dx, dy, dz) direction
    // assert (ox, oy, oz) in its range
    bool search(const ELEMTYPE ox, const ELEMTYPE oy, const ELEMTYPE oz,
                const ELEMTYPE dx, const ELEMTYPE dy, const ELEMTYPE dz,
                CALLBACKFUNC func) const {
    // call back function
    // returns false if not found, continue seaching.
    // returns ture if found, stop searching at once.
        // if starting point isn't in my range
        if (!inRange(ox, oy, oz)) return 0;
        // traverse all objects stored in me if I'm a leaf node
        if (isLeafNode()) {
            // traverse all objects stored in leaf node
            bool found = 0;
            for (INTTYPE i = 0; i < leafnode -> size(); ++i) {
                ELEMTYPE distance;
                if (func((*leafnode)[i], distance) &&  // if found
                    inRange(ox + dx * distance, // intersection point is in my range
                            oy + dy * distance,
                            oz + dz * distance)) 
                // have to traverse every objects,
                // because their overlapping sequence is uncertain.
                    found = 1;
            }
            if (found) 
                return 1;
            // not found after traversing
            // find the next cube the vector passes through
            // and search in that cube
            
            // find out the vector intersects with which surface
            ELEMTYPE interPoint[3];
            Surface surfaces[3];
            surfaces[0] = dx > 0? Front: (dx < 0? Back: None);
            surfaces[1] = dy > 0? Right: (dy < 0? Left: None);
            surfaces[2] = dz > 0? Up: (dz < 0? Down: None);

            for (INTTYPE i = 0; i < 3; ++i) {
                if (surfaces[i] == None) continue;
                bool intersected = intersectSurface(ox, oy, oz, dx, dy, dz,
                                                    interPoint[0],
                                                    interPoint[1],
                                                    interPoint[2],
                                                    surfaces[i]);
                if (intersected) {
                    return rootNode -> search(interPoint[0] + EPSILON * dx,
                                              interPoint[1] + EPSILON * dy,
                                              interPoint[2] + EPSILON * dz,
                                              dx, dy, dz, func);
                }
            }
            // doesn't intersect with any surface, should never happen
            assert(0);
        }
        else {
            // traverse all children nodes
            for (INTTYPE i = 0; i < 8; ++i) 
                if (subtree[i] -> search(ox, oy, oz, dx, dy, dz, func))
                    return 1;
        }

        return 0;
    }

};


template < class DATATYPE, class ELEMTYPE, class INTTYPE, class REALTYPE, INTTYPE MAXOBJCOUNT >
class Octree {
    TreeNode<DATATYPE, ELEMTYPE, INTTYPE, REALTYPE, MAXOBJCOUNT>* root;
    ELEMTYPE boundaryMinX, boundaryMaxX, 
             boundaryMinY, boundaryMaxY, 
             boundaryMinZ, boundaryMaxZ;
public:
    Octree(const ELEMTYPE minx, const ELEMTYPE miny, const ELEMTYPE minz,
           const ELEMTYPE maxx, const ELEMTYPE maxy, const ELEMTYPE maxz): 
        boundaryMinX(minx), boundaryMaxX(maxx), boundaryMinY(miny),
        boundaryMaxY(maxy), boundaryMinZ(minz), boundaryMaxZ(maxz),
        root(new TreeNode<DATATYPE, ELEMTYPE, INTTYPE, REALTYPE, MAXOBJCOUNT>
                 (minx, miny, minz, maxx, maxy, maxz, 0)) {
    }

    ~Octree() { delete root; }

    INTTYPE size() const { return root -> size(); }

    void clear() {
        delete root;
        root = new TreeNode<DATATYPE, ELEMTYPE, INTTYPE, REALTYPE, MAXOBJCOUNT>
                   (boundaryMinX, boundaryMinY, boundaryMinZ,
                    boundaryMaxX, boundaryMaxY, boundaryMaxZ, 0);
    }

    bool insert(const DATATYPE* obj) {
        ELEMTYPE minx = obj -> lowerBoundX();
        ELEMTYPE maxx = obj -> upperBoundX();
        ELEMTYPE miny = obj -> lowerBoundY();
        ELEMTYPE maxy = obj -> upperBoundY();
        ELEMTYPE minz = obj -> lowerBoundZ();
        ELEMTYPE maxz = obj -> upperBoundZ();

        assert(minx >= boundaryMinX && maxx < boundaryMaxX &&
               miny >= boundaryMinY && maxy < boundaryMaxY &&
               minz >= boundaryMinZ && maxz < boundaryMaxZ);

        return root -> insert(obj);
    }

    template < class CALLBACKFUNC >
    bool search(const ELEMTYPE ox, const ELEMTYPE oy, const ELEMTYPE oz,
                const ELEMTYPE dx, const ELEMTYPE dy, const ELEMTYPE dz,
                CALLBACKFUNC func) const {
        return root -> search(ox, oy, oz, dx, dy, dz, func);
    }
};

#endif /* OCTREE_H */
