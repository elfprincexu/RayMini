#ifndef _OCTREE_H_
#define _OCTREE_H_

class ONode {
private:
    ONode*          m_neighbours[4];
    BoundingBox     m_region;

};

class OInterNode {
    ONode*          m_children;
};

class OLeafNode {
    Surfel*         m_data;
};

class Octree {

};

#endif // _OCTREE_H_
