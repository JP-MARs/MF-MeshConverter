/**
* @file ConvMeshCommon.hpp
* @brief define mesh model
*
*
*/
#ifndef DEF_NANDELE_MCONVMESH_1
#define DEF_NANDELE_MCONVMESH_1


#include "ConvNodeElement.hpp"
#include <unordered_set>

namespace JPMRspace{
namespace MeshConvLib{

using std::unordered_set;

/*
//=======================================================
// ■ ConvMesh
//=======================================================
// メッシュデータ
//=======================================================*/
/**
* @class ConvMesh
* @brief Mesh model class
*/
class ConvMesh{
private:
	/*  隣接データ(ElementInfo)を生成: mode=0:3Dを対象,  mode=1:2Dを対象 */
	void makeEleInfo_sub(int mode);

	/* 辺の長さ計算 */
	void calc_min_edge_length_2D();
	void calc_min_edge_length_3D();

	/**/
public:
	/* メッシュの詳細情報を作ってあるか */
	/** @brief is mesh info(neighbour, etc) maked? */
	bool info_maked;
	/* 3D要素があるか */
	/** @brief is 2D element included? */
	bool have_3D;
	
	/* メッシュ情報 */
	/** @brief node data */
	vector<ConvNode> node;
	/** @brief edge data */
	vector<ConvEdge> edge;
	/** @brief element data */
	vector<ConvElement> element;
	/** @brief minimum edge length in 2D  */
	double minumum_edge_L_2D;	/* 最小の辺の長さ2D */
	/** @brief minimum edge length in 3D  */
	double minumum_edge_L_3D;	/* 最小の辺の長さ3D */
	/* 要素の情報（節点を２つ以上共有する要素の隣接関係） */
	/** @brief element info (neighbour lower than themselves)  */
	vector<EleInfo> eleInfo;

	/* メッシュ端の節点・辺・面 */
	/** @brief Node ID list located at mesh boundary  */
	vector<convint> boundNodeList;
	/** @brief Edge ID list located at mesh boundary  */
	vector<convint> boundEdgeList;
	/** @brief Facet ID list located at mesh boundary  */
	vector<convint> boundFacetList;
	/* メッシュ端の節点-2D要素 */
	/** @brief Node (icluding 2D ele) ID list located at mesh boundary  */
	vector<convint> boundNodeList2D;

	/* facetデータ */
	/** @brief facet data  */
	vector<ConvFacet> facets;
	/* 各要素のfacetのID */
	/** @brief facet IDs used in each element  */
	vector<vector<convint>> ele_facets_id;

	/* 読み込んだ時の節点・要素IDの対応関係 */
	bool save_id_map;	/* 元のIDとの対応を保持するか（デフォルトはfalse） */
	map<convint, convint> node_map;
	map<convint, convint> ele_map;
	/*--------------------*/
	/*--------------------*/
	/*--------------------*/
	ConvMesh(){ info_maked=false; save_id_map = false; };
	void clear();		/* メッシュデータをクリア */
	void info_clear();		/* 節点と要素以外のメッシュデータをクリア */
	void copyMesh(const ConvMesh& mesh2);	/* mesh2のメッシュデータをコピー */
	/*--------------------*/
	/*--------------------*/
	/*--------------------*/
	/* 辺・面・隣接関係など、全て作成 */
	void makeMeshInfo();
	/*--------------------*/
	/* 節点と要素情報から、辺を共有する隣接データ(ElementInfo)を生成 */
	void makeEleInfo();
	/* 辺作成 */
	void makeEdge();
	void makeEdge_2D();
	void makeEdge_2D_from3D();
	/* 境界上の辺サーチ */
	void searchBoundEdge();
	/* 境界上の辺と節点サーチ */
	void searchBoundEdgeNode();
	/* 境界上の節点を探す - 2Dのみ対象 */
	void searchBoundNode_for2D();
	/*--------------------*/
	/*--------------------*/
	/*--------------------*/
	/* 要素面の作成 */
	void makeFacet();
	/*--------------------*/
	/*--------------------*/
	/*--------------------*/
	/* 全ての隣接を作成 */
	void make2D_FullNeighbour(vector<vector<convint>>& Ele_2D_neighbour)const;	/* 辺を共有する隣接要素を作成する(辺を作っておくこと！) */
	void make3D_FullNeighbour(vector<vector<convint>>& Ele_3D_neighbour)const;	/* 面を共有する隣接要素を作成する(面を作っておくこと！) */
	/*--------------------*/
	/*--------------------*/
	/*--------------------*/
	/* 基本デバッグ用 */
	/* 辺を線要素として加える */
	void addLineElemets_fromEdge();
	void addSurfLineElemets_fromEdge();
	/* 面を2D要素として加える */
	void addSurfaceElemets_fromFacet();
	/* 境界の面を2D要素として加える */
	void addBoundSurfaceElemets_fromFacet();
	/* 境界の面を、外側が法線になるように書き出す(デバック用だが、WAMITに使える) */
	void exportBoundFacet_outward(const string& filename, const vector<vector<double>>& range) const;

	/*--------------------*/
	/*--------------------*/
	/*--------------------*/
	void reset_IDMap(){node_map.clear(); ele_map.clear();};	/* 読み込んだID対応の情報を削除 */

};


/* end of namespace*/
};
};



#endif
