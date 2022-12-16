#ifndef _HALFEDGE_H
#define _HALFEDGE_H

#include <vector>
#include <map>
#include <unordered_set>
#include <iostream>

typedef long index_t;

struct point_t {                    //顶点
    index_t index;                  //顶点的索引
    float x, y, z;                  //顶点的坐标
};

struct edge_t {                     //三角网格中的边
    index_t v[2];                   //存储两个顶点
    index_t& start() {
        return v[0];
    }

    index_t& end() {
        return v[1];
    }

    edge_t() {
        v[0] = v[1] = -1;           //初始化两个顶点的索引
    }
};

struct triangle_t {                 //三角形
    point_t v[3];                   //三角形三个顶点
    const point_t& i() const {
        return v[0];
    }
    const point_t& j() const {
        return v[1];
    }
    const point_t& k() const {
        return v[2];
    }
};

/**
 * build()方法需要有edges，如果没有edges的时候就调用这个方法
 * @param numTriangles
 * @param triangles
 * @param edgesOut
 */
int unorderedEdgesFromTriangles(const std::vector<triangle_t>& triangles, std::vector<edge_t>& edgesOut);

class trimesh_t {
public:
    typedef long index_t;
    struct halfedge_t {
        index_t toVertex;                       //半边所指向的顶点
        index_t face;                           //半边所存储的面（若为边界，存储的面为空）
        index_t edge;                           //所在的无向边
        index_t prev;                           //前一个半边
        index_t oppositeHe;                     //与自己方向相反的半边
        index_t nextHe;                         //下一条半边（逆时针方向）
        halfedge_t() :toVertex(-1),
                    face(-1),
                    edge(-1),
                    oppositeHe(-1),
                    nextHe(-1) {}
    };
    /**
    * 根据给定的三角面和边构建HalfEdge数据结构
    * Note:通过调用上方的unorderedEdgesFromTriangles()，可以使triangles产生edges，如果调用者没有edge的话可采用这个方法
    * Note：triangles和edges在调用build()以后就没有用了，应该被销毁
    * @param numVertices
    * @param numTriangles
    * @param triangles
    * @param numEdges
    * @param edges
    */
    int build(const std::vector<point_t >& points, const std::vector<edge_t>& edges, const std::vector<triangle_t>& triangles);

    /**
     * 清空
     */
    int clear() {
        mHalfEdges.clear();
        mVertexHalfEdges.clear();
        mFaceHalfEdges.clear();
        mEdgeHalfEdges.clear();
        mDirectedEdge2heIndex.clear();
        return 0;
    }

    const halfedge_t& halfedge(const index_t i) const {
        return mHalfEdges[i];
    }

    /**
     * 给定一个halfEdge的索引，返回相应的有向边
     * @param heIndex
     * @return
     */
    std::pair<index_t , index_t> heIndex2directdEdge(const index_t heIndex) const {
        const halfedge_t& he = mHalfEdges[heIndex];
        return std::make_pair(mHalfEdges[he.oppositeHe].toVertex, he.toVertex);
    }

    index_t directedEdge2heIndex(const index_t i, const index_t j) const  {
        auto result = mDirectedEdge2heIndex.find((std::make_pair(i, j)));
        return result == mDirectedEdge2heIndex.end() ? -1 : result->second;
    }

    /**
     * 在result中返回vertexIndex的邻居
     * @param vertexIndex
     * @param result
     */
    int vvNeighbors(const index_t vertexIndex, std::vector<index_t >& result) const {
        result.clear();
        const index_t startHei = mVertexHalfEdges[vertexIndex];                     //获取顶点所在的半边索引
        index_t hei = startHei;
        while (true) {
            const halfedge_t& he = mHalfEdges[hei];
            result.push_back(he.toVertex);                                          //半边所对的结点为一个邻居
            hei = he.prev;                                                          //（he.opposite.toVertex）
            if(hei == startHei) break;
        }
        return 0;
    }

    std::vector<index_t > vvNeighbors(const index_t vertexIndex) const {
        std::vector<index_t > result;
        vvNeighbors(vertexIndex, result);
        return result;
    }

    /**
     * @param vertexIndex
     * @return 返回vertexIndex的邻居数量
     */
    int vertexValence(const index_t vertexIndex) const {
        return static_cast<int>(vvNeighbors(vertexIndex).size());
    }

    /**
     * 返回与给定顶点接壤的面的索引
     * @param vertexIndex
     * @param 在result中以索引形式返回面邻居
     */
    int vfNeighbors(const index_t vertexIndex, std::vector<index_t >& result) const {
        result.clear();
        std::unordered_set<index_t > temp;
        const index_t startHei = mVertexHalfEdges[vertexIndex];
        index_t hei = startHei;
        while (true) {
            const halfedge_t& he = mHalfEdges[hei];
            if(he.face != -1) temp.insert(he.face);
            hei = he.prev;
            if (hei == startHei) break;
        }
        result = std::vector<index_t >(temp.begin(), temp.end());
        return 0;
    }

    std::vector<index_t > vfNeighbors(const index_t vertexIndex) const {
        std::vector<index_t > result;
        vfNeighbors(vertexIndex, result);
        return result;
    }
    /**
     * 给定一个边，求出与边相邻的面
     * @param vi
     * @param vj
     * @param result
     * @return
     */
    int efNeighbors(const index_t vi, const index_t vj, std::vector<index_t >& result) {
        bool flag = false;
        result.clear();
        if(mDirectedEdge2heIndex.find(std::make_pair(vi, vj)) != mDirectedEdge2heIndex.end()) {
            if(mHalfEdges[mDirectedEdge2heIndex[std::make_pair(vi, vj)]].face != -1){
                result.push_back(mHalfEdges[mDirectedEdge2heIndex[std::make_pair(vi, vj)]].face);
                flag = true;
            }
        }
        if (mDirectedEdge2heIndex.find(std::make_pair(vj, vi)) != mDirectedEdge2heIndex.end()) {
            if(mHalfEdges[mDirectedEdge2heIndex[std::make_pair(vj, vi)]].face != -1) {
                result.push_back(mHalfEdges[mDirectedEdge2heIndex[std::make_pair(vj, vi)]].face);
                flag = true;
            }
        }
        if(!flag) {
            std::cout << "this edge does not exist" << std::endl;
            return -1;
        }
        return 0;
    }

    std::vector<index_t > efNeighbors(const index_t vi, const index_t vj) {
        std::vector<index_t > result;
        efNeighbors(vi, vj, result);
        return result;
    }

     int boundaryVertices(std::vector<index_t >& v) const;

    int boundaryEdges(std::vector<std::pair<index_t , index_t > >& result);

    const index_t getPointSize() const {
        return static_cast<const index_t>(mPoints.size());
    }

    const index_t getHalfEdgeSize() const {
        return static_cast<const index_t>(mHalfEdges.size());
    }

    const index_t getFaceSize() const {
        return static_cast<const index_t>(mFaceHalfEdges.size());
    }

    const index_t getEdgeSize() const {
        return static_cast<const index_t>(mEdgeHalfEdges.size());
    }

    const std::vector<point_t> &getMPoints() const;

    const std::vector<halfedge_t> &getMHalfEdges() const;

    const std::vector<index_t> &getMVertexHalfEdges() const;

    const std::vector<index_t> &getMFaceHalfEdges() const;

    const std::vector<index_t> &getMEdgeHalfEdges() const;

    const std::vector<triangle_t> &getMTriangles() const;

private:
    std::vector<point_t> mPoints;                                                    //点集
    std::vector<triangle_t> mTriangles;                                              //三角形网格集合
    std::vector<halfedge_t> mHalfEdges;                                              //半边集合
    std::vector<index_t > mVertexHalfEdges;                                          //根据顶点索引获取半边索引
    std::vector<index_t > mFaceHalfEdges;                                            //根据面的索引获取半边索引
    std::vector<index_t > mEdgeHalfEdges;                                            //根据无向边的索引获取半边的索引
    typedef std::map<std::pair<index_t, index_t>, index_t > directedEdge2indexMap_t;
    directedEdge2indexMap_t mDirectedEdge2heIndex;                              //根据有向边获取半边索引
};


#endif