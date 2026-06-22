/**
* @file ConvMeshCommonBoundNeighb.cpp
* @brief define neibhouring and boundary search
*
*/
#include <mfMeshConverter/ConvMeshCommon.hpp>
#include <fstream>

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
// ○ 境界上の辺を探す - 3Dのみ対象
//		自身の面に注目して、その面を共有する隣の要素が無い場合は
//		その面が境界であると判定し、その面を構成する辺を境界辺とする
//===============================================================*/
/** 
 * @brief make boundary edges (for 3D)
*/
void ConvMesh::searchBoundEdge(){
#ifdef PRINT_CONV_INFO
	cout << "Bound Edge make !" << endl << endl;
#endif
	const convint ele_size = element.size();
	const convint ed_size = edge.size();
	const convint fc_size = facets.size();

	vector<bool> facetFlag(fc_size, false);//:falseは境界面*/

	/*for(convint il = 0; il < ele_size; il++){
		cout << " ele " << il << " >>> ";
		const int num_f1 = element[il].getFacetNum();
		for(int a = 0; a < num_f1; a++){
			cout << ele_facets_id[il][a] << ", ";
		}
		cout << endl;
	}
	getchar();*/

	/*境界面を探す*/
	for(convint il = 0; il < ele_size; il++){
		const int num_f1 = element[il].getFacetNum();

		/*(自分より番号が若い)隣接要素数*/
		int numNeighbor = eleInfo[il].numNeighborEle;
		for(int j = 0; j < numNeighbor; j++){
			const convint jl = eleInfo[il].neighborEle[j];
			const int num_f2 = element[jl].getFacetNum();

			for(int a = 0; a < num_f1; a++){
				convint f_id1 = ele_facets_id[il][a];
				bool bl = false;
				/* 隣の面と一致するか判定 */
				for(int b = 0; b < num_f2; b++){
					convint f_id2 = ele_facets_id[jl][b];
					bl |= facets[f_id1].id_equal(facets[f_id2]);
				}
				/* 一致か記憶 */
				bool tmp = facetFlag[f_id1];
				facetFlag[f_id1] = (tmp || bl);
			}
		}
	}

	boundEdgeList.clear();
	boundFacetList.clear();
	/* 共有されていない面があれば、その面に含まれている辺が境界の辺 */
	vector<bool> edgeFlag(ed_size, false);//:true辺は境界辺*/
	convint count_e=0;
	for(convint ff = 0; ff < fc_size; ff++){
		if(!facetFlag[ff]){
			vector<convint> ee;
			facets[ff].getEdges(ee);
			for(auto id : ee){
				edgeFlag[id] = true;
			}
			count_e++;
			/* 面も保存 */
			boundFacetList.push_back(ff);
		}
	}
	/*境界辺のみをリストに追加*/
	for(convint e = 0; e < ed_size; e++){
		if(edgeFlag[e]){
			boundEdgeList.push_back(e);
		}
	}
	const convint bount_e_size = boundEdgeList.size();
#ifdef PRINT_CONV_INFO
	cout << "number of boundary Edges = " << bount_e_size << endl;
#endif
}


/*
//===============================================================
// ○ 境界上の辺と節点を探す -  - 3Dのみ対象
//		自身の面に注目して、その面を共有する隣の要素が無い場合は
//		その面が境界であると判定し、その面を構成する辺を境界辺とする
//===============================================================*/
/** 
 * @brief make boundary nodes and edges
*/
void ConvMesh::searchBoundEdgeNode(){
	const convint nn  = node.size();
	/* 境界辺を先に探す */
	searchBoundEdge();

	vector<bool> nodeFlag(nn, false);
	const convint boundEdgeSize = boundEdgeList.size();
	for(convint i = 0; i < boundEdgeSize; i++){
		const convint bound_id = boundEdgeList[i];
		const convint sID = edge[bound_id].getSID();
		const convint eID = edge[bound_id].getEID();
		nodeFlag[sID] = true;
		nodeFlag[eID] = true;
	}
	boundNodeList.clear();
	for(convint i = 0; i < nn; i++){
		if(nodeFlag[i]){
			boundNodeList.push_back(i);
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "number of boundary Nodes = " << boundNodeList.size() << endl;
#endif
}


/*
//===============================================================
// ○ 境界上の節点を探す - 2Dのみ対象
//		自身の辺に注目して、それを共有する隣の要素が無い場合は
//		その面が境界であると判定し、その面を構成する辺を境界辺とする
//===============================================================*/
/** 
 * @brief make boundary nodes for 2D
*/
void ConvMesh::searchBoundNode_for2D(){
	const convint ele_size  = element.size();
	const convint edge_size = edge.size();
	const convint nn = node.size();
	vector<bool> nodeFlag(nn, false);

	/* 2次元配列の動的確保 */
	vector<bool>  edgeFlag(edge_size, true);// = new int[edge_size];

	/*境界面を探す*/
	for(convint il = 0; il < ele_size; il++){
		int num_edge = element[il].getEdgeNum();
		vector<convint> edges1;
		element[il].getEdges(edges1);
		/* 2Dだけ対象 */
		ConvEType type = element[il].getType();
		bool is3D = element[il].is3D();
		bool is1D = element[il].is1D();
		if(is3D || is1D){
			continue;
		}	
		/* 重複確認 */
		int numNeighbor = eleInfo[il].numNeighborEle;/*(自分より番号が若い)隣接要素数*/

		/* 三角形 */
		if(type == ConvEType::Tri){
			for(int ik = 0; ik < 3; ik++){
				convint ei1 = edges1[ik];

				for(int j = 0; j < numNeighbor; j++){
					convint jl = eleInfo[il].neighborEle[j];
					ConvEType type2 = element[jl].getType();
					vector<convint> edges2;
					element[jl].getEdges(edges2);
					/* お隣が三角形 */
					if(type2 == ConvEType::Tri){
						for(int jk = 0; jk < 3; jk++){
							convint ej1 = edges2[jk];

							if(ei1 == ej1) {
								/*辺を共有する要素を発見＝その面は領域内*/
								edgeFlag[ei1] = false;
							}
						}
					/* 四角形 */
					}else if(type2 == ConvEType::Quad){
						for(int jk = 0; jk < 4; jk++){
							convint ej1 = edges2[jk];

							if(ei1 == ej1) {
								/*辺を共有する要素を発見＝その面は領域内*/
								edgeFlag[ei1] = false;
							}
						}
					}
				}
			}
		/* 四角形 */
		}else if(type == ConvEType::Quad){
			for(int ik = 0; ik < 4; ik++){
				convint ei1 = edges1[ik];

				/* 重複確認 */
				//int numNeighbor = eleInfo[il].numNeighborEle;/*(自分より番号が若い)隣接要素数*/
				for(int j = 0; j < numNeighbor; j++){
					convint jl = eleInfo[il].neighborEle[j];
					ConvEType type2 = element[jl].getType();
					vector<convint> edges2;
					element[jl].getEdges(edges2);
					/* お隣が三角形 */
					if(type2 == ConvEType::Tri){
						for(int jk = 0; jk < 3; jk++){
							convint ej1 = edges2[jk];

							if(ei1 == ej1) {
								/*辺を共有する要素を発見＝その面は領域内*/
								edgeFlag[ei1] = false;
							}
						}
						/* 四角形 */
					}else if(type2 == ConvEType::Quad){
						for(int jk = 0; jk < 4; jk++){
							convint ej1 = edges2[jk];

							if(ei1 == ej1) {
								/*辺を共有する要素を発見＝その面は領域内*/
								edgeFlag[ei1] = false;
							}
						}
					}
				}
			}
		}
	}

	/*境界辺から境界節点をセット*/
	for(int i = 0; i < edge_size; i++){
		if(edgeFlag[i]){
			convint sID = edge[i].getSID();
			convint eID = edge[i].getEID();
			nodeFlag[sID] = true;
			nodeFlag[eID] = true;
		}
	}
	/*境界節点のみをリストに追加*/
	boundNodeList2D.clear();
	for(int i = 0; i < nn; i++){
		if(nodeFlag[i]){
			boundNodeList2D.push_back(i);
		}
	}
}





/*
//===============================================================
// ○ 節点と要素情報から隣接データ(ElementInfo)を生成２Ｄver
　　（辺を共有する隣接データを生成）
  //===============================================================*/
/** 
 * @brief make neighbouring elements (all information ver.) fr 2D
*/
void ConvMesh::make2D_FullNeighbour(vector<vector<convint>>& Ele_2D_neighbour)const{
	const convint nn = node.size();
	const convint ed_size = edge.size();
	const convint ele_size = element.size();

	/* 一旦，辺を含む要素リストを作成*/
	vector< vector<convint> > elmtList;
	elmtList.resize(ed_size);

	for(convint l = 0; l < ele_size; l++){
		ConvEType type = element[l].getType();
		/* 2D要素のみ対象 */
		bool is3D = element[l].is3D();
		bool is1D = element[l].is1D();
		if(is3D || is1D){
			continue;
		}
		int type_size = element[l].getEdgeNum();
		for(int k = 0; k < type_size; k++){
			convint ed = element[l].getEdge(k);
			elmtList[ed].push_back(l);
		}
	}

	Ele_2D_neighbour.clear();
	Ele_2D_neighbour.resize(ele_size);

	vector<convint> neighborEdge; /*辺を共有する隣接要素リスト*/
	for(convint l = 0; l < ele_size; l++){
		neighborEdge.resize(0);
		ConvEType type = element[l].getType();
		/* 2D要素のみ対象 */
		bool is3D = element[l].is3D();
		bool is1D = element[l].is1D();
		if(is3D || is1D){
			continue;
		}
		int type_size = element[l].getEdgeNum();
		for(int k = 0; k < type_size; k++){
			convint ed = element[l].getEdge(k);
			convint nborSize = elmtList[ed].size();
			for(convint i = 0; i < nborSize; i++){
				convint li = elmtList[ed][i];
				neighborEdge.push_back(li);
			}
		}
		sort(neighborEdge.begin(), neighborEdge.end());
		/* 自身以外を除いて確定 */
		convint nbNodeSize = neighborEdge.size();
		vector<convint> NeighbourEle;
		for(convint i = 0; i < nbNodeSize; i++){
			if( l != neighborEdge[i] ){
				NeighbourEle.push_back(neighborEdge[i]);
			}
		}
		Ele_2D_neighbour[l] = std::move(NeighbourEle);
	}
}
/*
//===============================================================
// ○ 辺と要素情報から隣接データ(ElementInfo)を生成３Ｄver
　　（面を共有する隣接データを生成）
  //===============================================================*/
/** 
 * @brief make neighbouring elements (all information ver.) fr 3D
*/
void ConvMesh::make3D_FullNeighbour(vector<vector<convint>>& Ele_3D_neighbour)const{
	const convint nn = node.size();
	const convint ed_size = edge.size();
	const convint ele_size = element.size();
	const convint f_size = facets.size();

	/* 一旦，面を含む要素リストを作成*/
	vector< vector<convint> > elmtList;
	elmtList.resize(f_size);

	for(convint l = 0; l < ele_size; l++){
		ConvEType type = element[l].getType();
		/* 3D要素のみ対象 */
		bool is3D = element[l].is3D();
		if(!is3D){
			continue;
		}
		int type_size = element[l].getFacetNum();
		for(int k = 0; k < type_size; k++){
			convint fd = ele_facets_id[l][k];
			elmtList[fd].push_back(l);
		}
	}

	Ele_3D_neighbour.clear();
	Ele_3D_neighbour.resize(ele_size);

	vector<convint> neighborFacet; /*面を共有する隣接要素リスト*/
	for(convint l = 0; l < ele_size; l++){
		neighborFacet.resize(0);
		ConvEType type = element[l].getType();
		/* 3D要素のみ対象 */
		bool is3D = element[l].is3D();
		if(!is3D){
			continue;
		}
		int type_size = element[l].getFacetNum();
		for(int k = 0; k < type_size; k++){
			convint fd = ele_facets_id[l][k];
			convint nborSize = elmtList[fd].size();
			for(convint i = 0; i < nborSize; i++){
				convint li = elmtList[fd][i];
				neighborFacet.push_back(li);
			}
		}
		sort(neighborFacet.begin(), neighborFacet.end());
		/* 自身以外を除いて確定 */
		convint nbNodeSize = neighborFacet.size();
		vector<convint> NeighbourEle;
		for(convint i = 0; i < nbNodeSize; i++){
			if( l != neighborFacet[i] ){
				NeighbourEle.push_back(neighborFacet[i]);
			}
		}
		Ele_3D_neighbour[l] = std::move(NeighbourEle);
	}
}

/* end of namespace*/
};
};
