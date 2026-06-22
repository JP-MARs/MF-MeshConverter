/**
* @file ConvMeshCommon_copy.cpp
* @brief define copy method 
*
*/
#include <mfMeshConverter/ConvMeshCommon.hpp>


namespace JPMRspace{
namespace MeshConvLib{


/*
//=======================================================
// ■ MeshCommon
//=======================================================
// Mesh共通作業staticクラス
//=======================================================*/


/*
//===============================================================
// ○ 辺・面・隣接関係など、全て作成
//===============================================================*/
/** 
 * @brief copy method
 * @param other mesh model
*/
void ConvMesh::copyMesh(const ConvMesh& mesh2){
	/* 基本フラグ */
	this->info_maked = mesh2.info_maked;
	this->have_3D    = mesh2.have_3D;

	/* メッシュ本体データ*/
	/* 節点 */
	const convint nn = mesh2.node.size();
	node.resize(nn);
	for(convint i = 0 ; i < nn ; i++){
		node[i] = mesh2.node[i];
	}
	/* 辺 */
	const convint ne = mesh2.edge.size();
	edge.resize(ne);
	for(convint i = 0 ; i < ne ; i++){
		edge[i] = mesh2.edge[i];
	}
	/* 要素 */
	const convint n_ele = mesh2.element.size();
	element.resize(n_ele);
	for(convint i = 0 ; i < n_ele ; i++){
		element[i] = mesh2.element[i];
	}

	/* 最小辺長 */
	this->minumum_edge_L_2D = mesh2.minumum_edge_L_2D;
	this->minumum_edge_L_3D = mesh2.minumum_edge_L_3D;

	/* 要素隣接情報 */
	const convint n_einfo = mesh2.eleInfo.size();
	eleInfo.resize(n_einfo);
	for(convint i = 0 ; i < n_einfo ; i++){
		eleInfo[i].numNeighborEle = mesh2.eleInfo[i].numNeighborEle;
		eleInfo[i].neighborEle = mesh2.eleInfo[i].neighborEle;
	}

	/* 境界情報*/
	this->boundNodeList   = mesh2.boundNodeList;
	this->boundEdgeList   = mesh2.boundEdgeList;
	this->boundFacetList  = mesh2.boundFacetList;
	this->boundNodeList2D = mesh2.boundNodeList2D;

	/* facet / 要素の facet 対応*/
	this->ele_facets_id   = mesh2.ele_facets_id;
	const convint n_facets = mesh2.facets.size();
	facets.resize(n_facets);
	for(convint i = 0 ; i < n_facets ; i++){
		facets[i] = mesh2.facets[i];
	}

	/* ID マップ関連*/
	this->save_id_map = mesh2.save_id_map;
	if(this->save_id_map){
		this->node_map = mesh2.node_map;
		this->ele_map  = mesh2.ele_map;
	} else {
		// 元のマップは保持しない -> リセット
		this->reset_IDMap();
	}
}


/*
//===============================================================
// ○ メッシュデータをクリア 
//===============================================================*/
void ConvMesh::clear(){
	/* メッシュ本体データ*/
	node.clear();
	node.shrink_to_fit();
	edge.clear();
	edge.shrink_to_fit();
	element.clear();
	element.shrink_to_fit();

	/* 要素隣接情報 */
	eleInfo.clear();
	eleInfo.shrink_to_fit();

	/* 境界情報*/
	boundNodeList.clear();
	boundNodeList.shrink_to_fit();
	boundEdgeList.clear();
	boundEdgeList.shrink_to_fit();
	boundNodeList2D.clear();
	boundNodeList2D.shrink_to_fit();
	boundFacetList.clear();
	boundFacetList.shrink_to_fit();

	/* facet / 要素の facet 対応*/
	ele_facets_id.clear();
	ele_facets_id.shrink_to_fit();
	facets.clear();
	facets.shrink_to_fit();

	/* ID マップ関連*/
	node_map.clear();
	ele_map.clear();
	
	info_maked=false; 
	save_id_map = false;
}

/*
//===============================================================
// ○ 節点と要素以外のメッシュデータをクリア
//===============================================================*/
void ConvMesh::info_clear(){
	/* メッシュ本体データ*/
	edge.clear();
	edge.shrink_to_fit();

	/* 要素隣接情報 */
	eleInfo.clear();
	eleInfo.shrink_to_fit();

	/* 境界情報*/
	boundNodeList.clear();
	boundNodeList.shrink_to_fit();
	boundEdgeList.clear();
	boundEdgeList.shrink_to_fit();
	boundNodeList2D.clear();
	boundNodeList2D.shrink_to_fit();


	/* facet / 要素の facet 対応*/
	ele_facets_id.clear();
	ele_facets_id.shrink_to_fit();
	facets.clear();
	facets.shrink_to_fit();

	/* ID マップ関連*/
	node_map.clear();
	ele_map.clear();

	info_maked=false; 
	save_id_map = false;
}



/* end of namespace*/
};
};
