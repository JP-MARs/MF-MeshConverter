/**
* @file ConvNodeElement.hpp
* @brief define node, edge, element, facet data structure for mesh conversion
*
*/

#ifndef DEF_NANDELE_MCONV_0
#define DEF_NANDELE_MCONV_0

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

#include "ConvConstants.hpp"


namespace JPMRspace{
namespace MeshConvLib{

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::map;


/*
//=======================================================
// ■ 要素タイプ
//=======================================================*/
/**
* @enum ConvEType
* @brief Element Type Enum
*/
enum class ConvEType : char{
	Point,	/*点要素*/
	Line,	/*線要素*/
	Tri,	/*三角形*/
	Quad,	/*四角形*/
	Tri2,	/*三角形2次*/
	Quad2,	/*四角形2次*/
	Tetra,	/*四面体*/
	Prizm,	/*プリズム*/
	Hex,	/*六面体*/
	Pyramid	/* ピラミッド */
};



/*
//=======================================================
// ■ convNode
//=======================================================
// FEMの節点定義クラス
//=======================================================*/
/**
* @class ConvNode
* @brief Mesh node definition class
*/
class ConvNode{
private:
	convint id;
	double n_x;
	double n_y;
	double n_z;
public:
	ConvNode() = default;
	~ConvNode() = default;
	/** @brief node id getter */
	convint getID() const{return id;};
	/** @brief node id setter */
	void setID(convint n){id=n;};
	/** @brief X getter */
	double getX() const{return n_x;};
	/** @brief Y getter */
	double getY() const{return n_y;};
	/** @brief Z getter */
	double getZ() const{return n_z;};
	/**
	* @brief (x,y,z) setter
	*/
	void set(double xx, double yy, double zz){n_x=xx;n_y=yy;n_z=zz;};
	/** @brief X setter */
	void setX(double x){n_x=x;};
	/** @brief Y setter */
	void setY(double y){n_y=y;};
	/** @brief Z setter */
	void setZ(double z){n_z=z;};
};


/*
//=======================================================
// ■ Edge
//=======================================================
// FEMの辺定義クラス
//=======================================================*/
/**
* @class ConvEdge
* @brief Mesh edge definition class
*/
class ConvEdge{
private:
	convint id;
	convint s_ID;					/* 始点 */
	convint e_ID;					/* 終点 */
	bool edge2D;					/* 2次元要素のみが持つ辺か(Trueなら2D) */
public:
	ConvEdge();
	~ConvEdge()=default;
	/** @brief id getter */
	convint getID() const{return id;};
	/** @brief id setter */
	void setID(convint n){id=n;};
	/** @brief Start node id getter */
	convint getSID() const{return s_ID;};
	/** @brief Start node id setter */
	void setSID(convint n){s_ID=n;};
	/** @brief End node id getter */
	convint getEID() const{return e_ID;};
	/** @brief End node id setter */
	void setEID(convint n){e_ID=n;};
	/** @brief is 2D edge setter */
	void setIs2D(bool bl){edge2D=bl;};
	/** @brief is 2D edge getter */
	bool is2DEdge() const{return edge2D;};
	/** @brief IDs setter */
	void set(convint i, convint s, convint e){id=i;s_ID=s;e_ID=e;};
	/** @brief edge length calc. */
	double calc_length(const std::vector<ConvNode>& NodeData) const;	
};


/*
//=======================================================
// ■ Element
//=======================================================
// FEMの要素定義クラス
//=======================================================*/
/**
* @class ConvElement
* @brief Mesh element definition class
*/
class ConvElement{
private:
	convint id;	
	ConvEType e_type;					/* 要素タイプ*/
	std::unique_ptr<convint[]> nodes;
	std::unique_ptr<convint[]> edges;
	int mat; 
	double calcVolume_Tet(const vector<ConvNode>& NodeData) const;									/* 体積計算(四面体) */
	double calcVolume_Pri(const vector<ConvNode>& NodeData) const;									/* 体積計算(プリズム) */
	double calcVolume_Hex(const vector<ConvNode>& NodeData) const;									/* 体積計算(六面体) */
	void calcTriCoef(double& volume, double* coefB, double* coefC, double* coefD, double* coefE, const vector<ConvNode>& NodeData) const;	/* 四面体補間関数用係数計算 */
	double calc_Jacob_Pri(double *pos_rst, const vector<ConvNode>& NodeData) const;
	double calc_Jacob_Hex(double *pos_rst, const vector<ConvNode>& NodeData) const;
	double calc_area_tri(const vector<ConvNode>& NodeData) const;
	double calc_area_quad(const vector<ConvNode>& NodeData) const;
public:
	ConvElement();
	~ConvElement()=default;
	/** @brief Constructor with ele_type */
	ConvElement(ConvEType type){ initialize(type); };
	/** @brief Constructor with ele_type, node, mat */
	ConvElement(ConvEType type, int mat_, convint* nn, convint* ee){ initialize(type, mat_, nn, ee); };
	/** @brief copy const. */
	ConvElement(const ConvElement& ele){ *this = ele; };
	/** @brief move const. */
	ConvElement(ConvElement&& ele) noexcept{ *this = std::move(ele); };
	ConvElement& operator=(const ConvElement& ele);
	ConvElement& operator=(ConvElement&& ele)  noexcept;
	void initialize(ConvEType type);
	void initialize(ConvEType type, int mat_, convint* nn, convint* ee);

	int getNodeNum() const;	/* 節点の数を返す */
	int getEdgeNum() const;	/* 辺の数を返す */
	int getFacetNum() const;	/* 面の数を返す */
	bool is3D() const;		/* この要素が3Dか */
	bool is1D() const;		/* この要素が1Dか */
	/** @brief id getter */
	convint getID() const{return id;};
	/** @brief id setter */
	void setID(convint n){id=n;};
	/** @brief ele_type getter */
	ConvEType getType() const{return e_type;};
	/** @brief ele_type setter=initialize */
	void setType(ConvEType tt){this->initialize(tt);};

	void setNodes(convint *nn);
	void setNodes(const vector<convint>& nn);
	void setNodes(int i, convint nn){nodes[i]=nn;};
	convint getNode(int i) const{return nodes[i];};
	void getNodes(convint *nn) const;
	void getNodes(vector<convint>& nn) const;
	void swap_nodes(int i, int j);

	void setEdges(convint *ee);
	void setEdges(const vector<convint>& ee);
	void setEdges(int i, convint ee){edges[i]=ee;};
	convint getEdge(int i) const{return edges[i];};
	void getEdges(convint *ee) const;
	void getEdges(vector<convint>& ee) const;

	/** @brief mat getter */
	int getMat() const{return mat;};
	/** @brief mat setter */
	void setMat(int n){mat=n;};

	/** @brief calc. gravity of element */
	void getGrav(double*grav, const vector<ConvNode>& NodeData) const;	/* 重心計算 */
	/** @brief calc. are of  2D element */
	double calcArea(const vector<ConvNode>& NodeData) const;			/* 面積計算 */
	/** @brief calc. volume of 3D element */
	double calcVolume(const vector<ConvNode>& NodeData) const;			/* 体積計算 */
};


/*
//=======================================================
// ■ EleInfo
//=======================================================
// FEMの要素データ管理クラス(辺作成などに使う疑似隣接情報)
//=======================================================*/
/**
* @class EleInfo
* @brief Element data management class (neighbour info used for edge creation, etc)
*/
class EleInfo{
public:
	//bool is3D;						/* その要素が３Ｄか(trueで3D要素, 線要素は無視) */
	/** @brief neighbour ele IDs (only lower than self ele ID) */
	vector<convint> neighborEle;		/*その要素と、節点を2つ以上共有する要素番号配列(自分より番号が下の要素のみ)*/
	/** @brief num. of neighbour elements */
	int numNeighborEle;					/*上記隣接要素数*/
};


/*
//=======================================================
// ■ Facet
//=======================================================
// FEMの要素面クラス（隣接作成にしか使わない予定）
//=======================================================*/
/**
* @class ConvFacet
* @brief Facet info of elements (only used to detect element neighbouring)
*/
class ConvFacet{
private:
	convint id;	
	ConvEType e_type;				/* 要素タイプ（2次元のタイプのみ使う） */
	std::unique_ptr<convint[]> nodes;
	std::unique_ptr<convint[]> edges;

	double calc_area_tri(const vector<ConvNode>& NodeData, const convint n1, const convint n2, const convint n3) const;	/* 三角形の面積計算 */
public:
	ConvFacet();
	~ConvFacet(){ edges.reset(); };
	ConvFacet(ConvEType type){ initialize(type); };
	ConvFacet(ConvEType type, convint* nn, convint* ee){ initialize(type, nn, ee); };
	ConvFacet(const ConvFacet& ele){ *this = ele; };
	ConvFacet(ConvFacet&& ele) noexcept{ *this = std::move(ele); };
	ConvFacet& operator=(const ConvFacet& ele);
	ConvFacet& operator=(ConvFacet&& ele)  noexcept;
	void initialize(ConvEType type);
	void initialize(ConvEType type, convint* nn, convint* ee);
	int getNodeNum() const;	/* 節点の数を返す */
	int getEdgeNum() const;	/* 辺の数を返す */

	bool operator==(const ConvFacet f2) const;	/* 同じ面か判定 */
	bool id_equal(const ConvFacet f2) const{ return(id == f2.id); };	/* IDで同じ面か判定 */

	convint getID() const{return id;};
	void setID(convint n){id=n;};
	ConvEType getType() const{return e_type;};
	void setType(ConvEType n){e_type=n;};

	void setNodes(convint *nn);
	void setNodes(int i, convint nn){nodes[i]=nn;};
	convint getNode(int i) const{return nodes[i];};
	void getNodes(convint *nn) const;
	void getNodes(vector<convint>& nn) const;

	void setEdges(convint *ee);
	void setEdges(int i, convint ee){edges[i]=ee;};
	convint getEdge(int i) const{return edges[i];};
	void getEdges(convint *ee) const;
	void getEdges(vector<convint>& ee) const;

	void getGrav(double* grav, const vector<ConvNode>& NodeData) const;	/* 重心計算 */
	double calcArea(const vector<ConvNode>& NodeData) const;	/* 面積計算 */
};

/**/
};
};

#endif
