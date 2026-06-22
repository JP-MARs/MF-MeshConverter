/**
* @file ConvMeshCommon.cpp
* @brief define common neibhros
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
 * @brief make mesh info (edge, facet, neighb, min, etc)
*/
void ConvMesh::makeMeshInfo(){
	/* 3Dの有無を調査 */
	have_3D = false;
	const convint e_size = element.size();
	for(convint l = 0; l < e_size; l++){
		bool is3D = element[l].is3D();
		if(is3D){
			have_3D = true;
			break;
		}
	}
	/* 基本節点隣接を作成 */
	makeEleInfo();
	/* 3Dのとき、辺と面を作り、境界抽出 */
	if(have_3D){
		makeEdge();
		makeFacet();
		searchBoundEdgeNode();
	/* 2Dのとき、辺だけ作り、境界節点を抽出 */
	} else{
		makeEdge_2D();
		searchBoundNode_for2D();
	}
	/* 辺の長さ計算 */
	this->calc_min_edge_length_2D();
	this->calc_min_edge_length_3D();

	/* 情報作成フラグon */
	info_maked = true;
}


/*
//===============================================================
// ○ 節点と要素情報から、辺を共有する隣接データ(ElementInfo)を生成
//===============================================================*/
/** 
 * @brief make mesh info (node neighbouriing)
*/
void ConvMesh::makeEleInfo(){
	/* 2D, 3Dを分けてデータ作成 */
	const convint e_size = element.size();
	eleInfo.clear();
	eleInfo.resize( e_size );
	for(convint i = 0; i < e_size; i++){
        eleInfo[i].numNeighborEle = 0;
        eleInfo[i].neighborEle.clear();
    }
	makeEleInfo_sub(0);
	makeEleInfo_sub(1);
}

/*
//===============================================================
// ○ 節点共有要素のデータ(ElementInfo)を生成: mode=0:3Dを対象,  mode=1:2Dを対象
//===============================================================*/
/** 
 * @brief make mesh info subrutin
*/
void ConvMesh::makeEleInfo_sub(int mode){
	const convint n_size = node.size();
	const convint ele_size = element.size();
	convint target_ele = 0;
	/* 一旦，節点を含む要素リストを作成*/
	vector< vector<convint> > elmtList;
	elmtList.resize(n_size);
	for(convint l = 0; l < ele_size; l++){
		/*要素タイプ*/
		ConvEType type = element[l].getType();
		bool is3D = element[l].is3D();
		if(mode == 0){
			/* 2D要素は無視 */
			if( !is3D ){
				continue;
			}
		}else{
			bool is1D = element[l].is1D();
			/* 3D要素は無視 */
			if( is3D || is1D ){
				continue;
			}
		}
		target_ele++;
		/* forループ数セット(節点ｉに含まれる要素のID) */
		int num1 = element[l].getNodeNum();
		for(int k = 0; k < num1; k++){
			convint inode = element[l].getNode(k);
			if(inode >= n_size){
				cout << "Over node! "<<l << " => " << inode<<endl;
				exit(1);
			}
			elmtList[inode].push_back(l);
		}
	}
	vector<convint> neighborNode; 	/*節点を共有する隣接要素リスト*/
	vector<convint> neighbor;       /*辺を共有する隣接要素リスト*/

	/* 対象の次元の要素がないなら終了 */
	if(target_ele == 0){
		return;
	}
#ifdef PRINT_CONV_INFO
	cout << "** start making element Info " << mode << endl;
#endif
	/*節点を隣接する要素リストを作成*/
	for(convint l = 0; l < ele_size; l++){
		neighborNode.resize(0);
		/*要素タイプ*/
		ConvEType type = element[l].getType();
		bool is3D = element[l].is3D();
		if(mode == 0){
			/* 2D要素は無視 */
			if( !is3D ){
				continue;
			}
		}else{
			bool is1D = element[l].is1D();
			/* 3D要素は無視 */
			if( is3D || is1D ){
				continue;
			}
		}
		/* forループ数セット */
		int num1 = element[l].getNodeNum();
		for(int k = 0; k < num1; k++){
			convint nd = element[l].getNode(k);
			convint nborSize = elmtList[nd].size();
			for(convint i = 0; i < nborSize; i++){
				convint li = elmtList[nd][i];
				if(li < l){ /*自分より若い番号の要素のみ*/
					neighborNode.push_back(li);
				}
			}
		}
		/* 番号順にソート */
		sort(neighborNode.begin(), neighborNode.end());

		convint nbNodeSize = neighborNode.size();
		if(nbNodeSize <= 0){
			//cout << "N zero!? " << l << endl;
			continue;
		}
		
		/*同じ番号が2個以上含まれている＝辺を共有している要素を探す*/
		neighbor.resize(0);
		bool sameFlag = false;
		for(convint i = 0; i < nbNodeSize-1; i++){
			convint elmi  = neighborNode[i];
			convint elmii = neighborNode[i+1];
			if(elmi == elmii){
				if(sameFlag == false){
					neighbor.push_back(elmi);
					sameFlag = true;
				}
			}else{
				sameFlag = false;
			}
		}

		convint nbSize = neighbor.size();
		eleInfo[l].numNeighborEle = nbSize;
		eleInfo[l].neighborEle.clear();
		for(int i = 0; i < nbSize; i++){
			eleInfo[l].neighborEle.push_back(neighbor[i]);
		}
	}
}

/*
//===============================================================
// ○ 辺作成
//===============================================================*/
/** 
 * @brief make edge data
*/
void ConvMesh::makeEdge(){
	/* 四面体の辺 */
	const int tet_e[6][2] = {
		{0,1},{1,2},{0,2},
		{0,3},{1,3},{2,3}
	};
	/* プリズムの辺 */
	const int pri_e[9][2] = {
		{0,1},{1,2},{0,2},
		{3,4},{4,5},{3,5},
		{0,3},{1,4},{2,5}
	};
	/* 六面体の辺 */
	const int hex_e[12][2] = {
		{7,6},{4,5},{0,1},{3,2},
		{5,6},{1,2},{0,3},{4,7},
		{2,6},{3,7},{0,4},{1,5},
	};
	/* ピラミッドの辺 */
	const int pyramid_e[8][2] = {
		{0,1},{3,2},{0,3},{1,2},
		{0,4},{1,4},{2,4},{3,4}
	};

	convint n1, n2, n1a, n2a;
	int  findflag;
	int esignflag;
	
	const convint ne = element.size();
#ifdef PRINT_CONV_INFO
	cout << "** start making geom data" << endl;
#endif	
	/* alloc edge */
	edge.resize(0);
	edge.reserve((convint)1.3*ne); /*辺の数は要素数の1.3倍程度なのであらかじめメモリ予約*/

	convint jl, ek;
	int numNeighbor;
	ConvEdge edgetmp;
	
	/* define edges */
	convint iecnt = 0;
	for(convint il = 0; il < ne; il++){
		/*要素タイプ*/
		bool is3D = element[il].is3D();
		/* 2D要素以下は無視 */
		if( !is3D ){
			continue;
		}
		const ConvEType type = element[il].getType();
		/* 辺のforループ数セット */
		int node_num1 = element[il].getNodeNum();
		int edge_num1 = element[il].getEdgeNum();
		vector<convint> inode;
		element[il].getNodes(inode);
		for(int ik = 0; ik < edge_num1; ik++){
			/* 始点と終点 */
			/* 四面体 */
			if(type == ConvEType::Tetra){
				n1 = inode[tet_e[ik][0]];
				n2 = inode[tet_e[ik][1]];
			/* プリズム */
			}else if(type == ConvEType::Prizm){
				n1 = inode[pri_e[ik][0]];
				n2 = inode[pri_e[ik][1]];
			/* 六面体 */
			}else if(type == ConvEType::Hex){
				n1 = inode[hex_e[ik][0]];
				n2 = inode[hex_e[ik][1]];
			/* ピラミッド */
			}else if(type == ConvEType::Pyramid){
				n1 = inode[pyramid_e[ik][0]];
				n2 = inode[pyramid_e[ik][1]];
			}
			
			/* 重複確認 */
			findflag  = 0;
			esignflag = 0; /*この初期化は意味ないけどdebug用*/
			ek        = -1;/*この初期化は意味ないけどdebug用*/
			
			/* 隣接要素を探す */
			numNeighbor = eleInfo[il].numNeighborEle;

			for(int j = 0; j < numNeighbor && findflag == 0; j++){
				jl = eleInfo[il].neighborEle[j];
				/* お隣さんのタイプもゲット */
				bool is3Db = element[jl].is3D();
				/* 2D要素は無視 */
				if( !is3Db ){
					continue;
				}
				const ConvEType type2 = element[jl].getType();
				/* お隣さんの辺のforループ数セット */
				int edge_num2 = element[jl].getEdgeNum();
				vector<convint> iedge;
				element[jl].getEdges(iedge);
				for(int k = 0; k < edge_num2; k++){
					ek  = iedge[k];
					if(ek < 0){
						//cout << "error minus EDGE!!!"<<endl;
						continue;
					}
					n1a = edge[ek].getSID();
					n2a = edge[ek].getEID();
					if ( (n1 == n1a) && (n2 == n2a) ){
						findflag  = 1;
						esignflag = 1;
						break;
					}
					if ( (n1 == n2a) && (n2 == n1a) ){
						findflag  =  1;
						esignflag = -1;
						break;
					}
				}
			}
			/* 重複がなければ、辺を追加 */
			if (findflag == 0){
				edgetmp.setID(iecnt);
				/* 始点の方が小さいIDになるようにセット(全体座標では) */
				if(n1 < n2){
					edgetmp.setSID(n1);
					edgetmp.setEID(n2);
				}else{
					edgetmp.setSID(n2);
					edgetmp.setEID(n1);
				}
				edgetmp.setIs2D(false);
				edge.push_back( std::move(edgetmp) );
				ek = iecnt;
				esignflag = 1;
				iecnt++;
			}
			/* 要素にローカル辺番号をセット */
			element[il].setEdges(ik, ek);
		}

	}
	convint neg = iecnt;
	/***debug ****/
#ifdef PRINT_CONV_INFO
	if (neg != edge.size()){
		cout << "neg =" << neg << "edge.size=" << edge.size() << endl;
	}
	cout << "** number of edges=" << neg << endl;
#endif
}

/*
//===============================================================
// ○ 辺要素データ作成（2Dの要素対象）
//===============================================================*/
/** 
 * @brief make edge data for 2D
*/
void ConvMesh::makeEdge_2D(){

	/* 三角形四面体の辺 */
	const int tri_e[3][2] = {{0,1},{1,2},{0,2}};
	const int tri_e2[6][2] = {{0,3},{3,1},{1,4},{4,2},{0,5},{0,2}};
	/* 四角形の辺 */
	const int sq_e[4][2] = {
		{0,1},{3,2},{0,3},{1,2}
	};
	const int sq_e2[8][2] = {
		{0,4},{4,1},{3,6},{6,2},
		{0,7},{7,3},{1,5},{5,2}
	};

	convint n1, n2, n1a, n2a;
	int  findflag;
	int esignflag;
	
	const convint ne = element.size();
#ifdef PRINT_CONV_INFO
	cout << "** start making geom data 2D" << endl;
#endif	
	convint jl, ek;
	int numNeighbor;
	ConvEdge edgetmp;
	
	/* define edges */
	convint iecnt = edge.size();

	/* 線要素IDを覚えておく用vector */
	vector<convint> beam_ele;

	for(convint il = 0; il < ne; il++){
		/*要素タイプ*/
		const ConvEType type = element[il].getType();
		bool is3D = element[il].is3D();
		bool is1D = element[il].is1D();
		/* 3D要素&線要素は無視 */
		if(is3D || is1D){
			/* ただし線要素は覚えておく */
			if(type == ConvEType::Line){
				beam_ele.push_back(il);
			}
			continue;
		}

		/* 辺のforループ数セット */
		int edge_num1 = element[il].getEdgeNum();
		vector<convint> inode;
		element[il].getNodes(inode);

		for(int ik = 0; ik < edge_num1; ik++){
			/* 始点と終点 */
			/* 三角形 */
			if(type == ConvEType::Tri){
				n1 = inode[tri_e[ik][0]];
				n2 = inode[tri_e[ik][1]];
			/* 四角形 */
			}else if(type == ConvEType::Quad){
				n1 = inode[sq_e[ik][0]];
				n2 = inode[sq_e[ik][1]];
			/* 2次要素Tri2 */
			}else if(type == ConvEType::Tri2){
				n1 = inode[tri_e2[ik][0]];
				n2 = inode[tri_e2[ik][1]];
			/* 2次要素Sq2 */
			}else if(type == ConvEType::Quad2){
				n1 = inode[sq_e2[ik][0]];
				n2 = inode[sq_e2[ik][1]];
				/* 線要素 */
			}else if(type == ConvEType::Line){
				n1 = inode[0];
				n2 = inode[1];
			}
			
			/* 重複確認 */
			findflag  = 0;
			esignflag = 0; /*この初期化は意味ないけどdebug用*/
			ek        = -1;/*この初期化は意味ないけどdebug用*/
			/* 2Dもサーチ(3Dで見つかっていてもう１つ作っちゃう？) */
			if(findflag == 0){
				/* 2Dで隣接要素を探す */
				numNeighbor = eleInfo[il].numNeighborEle;
				for(int j = 0; j < numNeighbor && findflag == 0; j++){
					jl = eleInfo[il].neighborEle[j];
					/* お隣さんのタイプもゲット */
					//const ConvEType type2 = element[jl].getType();
					bool is3Db = element[jl].is3D();
					bool is1Db = element[jl].is1D();
					/* 3D要素&線要素以下は無視 */
					if(is3Db || is1Db){
						continue;
					}
					/* お隣さんの辺のforループ数セット */
					int edge_num2 = element[jl].getEdgeNum();
					vector<convint> iedge;
					element[jl].getEdges(iedge);

					for(int k = 0; k < edge_num2; k++){
						ek  = iedge[k];
						if(ek < 0){
							continue;
						}
						n1a = edge[ek].getSID();
						n2a = edge[ek].getEID();
						if ( (n1 == n1a) && (n2 == n2a) ){
							findflag  = 1;
							esignflag = 1;
							break;
						}
						if ( (n1 == n2a) && (n2 == n1a) ){
							findflag  =  1;
							esignflag = -1;
							break;
						}
					}
				}
			}
			/* 重複がなければ、辺を追加 */
			if (findflag == 0){
				edgetmp.setID(iecnt);
				/* 始点の方が小さいIDになるようにセット(全体座標では) */
				if(n1 < n2){
					edgetmp.setSID(n1);
					edgetmp.setEID(n2);
				}else{
					edgetmp.setSID(n2);
					edgetmp.setEID(n1);
				}
				edgetmp.setIs2D(true);
				edge.push_back( std::move(edgetmp) );
				ek = iecnt;
				esignflag = 1;
				iecnt++;
			}
			
			/* 要素にローカル辺番号をセット */
			element[il].setEdges(ik, ek);
		}

	}
	convint neg = iecnt;
#ifdef PRINT_CONV_INFO
	cout << "** number of edges=" << neg << endl;
#endif
}


/*
//===============================================================
// ○ 3Dの辺と一致する2D辺を探して2D要素にセット
//===============================================================*/
/** 
 * @brief make 2D edge from 3D elements
*/
void ConvMesh::makeEdge_2D_from3D(){

	/* 三角形四面体の辺 */
	const int tri_e[3][2] = {{0,1},{1,2},{0,2}};
	const int tri_e2[6][2] = {{0,3},{3,1},{1,4},{4,2},{0,5},{0,2}};
	/* 四角形の辺 */
	const int sq_e[4][2] = {
		{0,1},{3,2},{0,3},{1,2}
	};
	const int sq_e2[8][2] = {
		{0,4},{4,1},{3,6},{6,2},
		{0,7},{7,3},{1,5},{5,2}
	};

	convint n1, n2;
	
	const convint ne = element.size();
	ConvEdge edgetmp;
	
	/* define edges */
	convint iecnt = edge.size();
#ifdef PRINT_CONV_INFO
	cout << "Set 3D-Edge Data to 2D elements "<<  edge.size() << endl;
#endif

	for(convint il = 0; il < ne; il++){
		/*要素タイプ*/
		const ConvEType type = element[il].getType();
		bool is3D = element[il].is3D();
		bool is1D = element[il].is1D();
		/* 3D要素&線要素は無視 */
		if(is3D || is1D){
			continue;
		}

		/* 辺のforループ数セット */
		int edge_num1 = element[il].getEdgeNum();
		vector<convint> inode;
		element[il].getNodes(inode);

		for(int ik = 0; ik < edge_num1; ik++){
			/* 始点と終点 */
			/* 三角形 */
			if(type == ConvEType::Tri){
				n1 = inode[tri_e[ik][0]];
				n2 = inode[tri_e[ik][1]];
			/* 四角形 */
			}else if(type == ConvEType::Quad){
				n1 = inode[sq_e[ik][0]];
				n2 = inode[sq_e[ik][1]];
			/* 2次要素Tr2 */
			}else if(type == ConvEType::Tri2){
				n1 = inode[tri_e2[ik][0]];
				n2 = inode[tri_e2[ik][1]];
			/* 2次要素Sq2 */
			}else if(type == ConvEType::Quad2){
				n1 = inode[sq_e2[ik][0]];
				n2 = inode[sq_e2[ik][1]];
			/* 線要素 */
			}else if(type == ConvEType::Line){
				n1 = inode[0];
				n2 = inode[1];
			}

			/* 辺ループ */
			for(convint ee = 0  ; ee < iecnt ; ee++){
				convint s_ID = edge[ee].getSID();
				convint e_ID = edge[ee].getEID();
				bool check = (s_ID == n1 && e_ID == n2) || (e_ID == n1 && s_ID == n2);
				/* 始点終点が一致する辺があった！ */
				if(check){
					/* 要素にローカル辺番号をセット */
					element[il].setEdges(ik, ee);
					break;
				}
			}
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "Edge set End "<< endl;
#endif
}

/*
//===============================================================
// ○ 最小の辺の長さ計算
//===============================================================*/
void ConvMesh::calc_min_edge_length_2D(){
	double temp_e_L = 1.0e+20;;
	const convint ed_size = edge.size();
	for(convint i=0; i < ed_size; i++){
		bool bl = edge[i].is2DEdge();
		if(bl){
			double length = edge[i].calc_length(node);
			if(length < temp_e_L){
				temp_e_L = length;
			}
		}
	}
	minumum_edge_L_2D = temp_e_L;
}
/*
//===============================================================
// ○ 最小の辺の長さ計算
//===============================================================*/
void ConvMesh::calc_min_edge_length_3D(){
	double temp_e_L = 1.0e+20;;
	const convint ed_size = edge.size();
	for(convint i=0; i < ed_size; i++){
		bool bl = edge[i].is2DEdge();
		if(!bl){
			double length = edge[i].calc_length(node);
			if(length < temp_e_L){
				temp_e_L = length;
			}
		}
	}
	minumum_edge_L_3D = temp_e_L;
}

/*
//===============================================================
// ○ 要素面の作成
//===============================================================*/
/** 
 * @brief make facet data
*/
void ConvMesh::makeFacet(){
	/* 四面体の面の節点 */
	const int tet_f_n[4][3] = {
		{0, 1, 2}, {0, 1, 3}, {1, 2, 3}, {0, 2, 3}
	};
	/* プリズムの面の節点 */
	const int pri_f_n[5][4] = {
		{0,1,2, 0}, {3,4,5, 0},
		{0, 1, 4, 3}, {1, 2, 5, 4}, {0, 2, 5, 3}
	};
	/* 六面体の面の節点 */
	const int hex_f_n[6][4] = {
		{4, 5, 6, 7}, {0, 1, 2, 3}, {3, 2, 6, 7}, {0, 1, 5, 4},
		{1, 2, 6, 5}, {0, 3, 7, 4}
	};
	/* ピラミッドの面の節点 */
	const int pyramid_f_n[5][4] = {
		{0,1,2,3},
		{0,1,4, 0},{3,2,4, 0},{0,3,4, 0},{1,2,4, 0}
	};
	/***************************************/
	/* 四面体の面の辺 */
	const int tet_f[4][3] = {
		{0, 1, 2}, {0, 4, 3}, {1, 5, 4}, {2, 5, 3}
	};
	/* プリズムの面の辺 */
	const int pri_f[5][4] = {
		{0,1,2, 0}, {3,4,5, 0},
		{0, 3, 6, 7}, {1, 4, 7, 8}, {2, 5, 6, 8}
	};
	/* 六面体の面の辺 */
	const int hex_f[6][4] = {
		{1, 0, 7, 4}, {2, 3, 6, 5}, {3, 0, 9, 8}, {2, 1, 10, 11},
		{5, 4, 11, 8}, {6, 7, 10, 9}
	};
	/* ピラミッドの面の辺 */
	const int pyramid_f[5][4] = {
		{0,1,2,3},
		{0,5,4, 0},{1,7,6, 0},{2,4,7, 0},{3,5,6, 0}
	};
	/***************************************/
	/***************************************/

	const convint ne = element.size();
	/* facetデータ */
	facets.clear();
	/* 各要素のfacetのID */
	ele_facets_id.resize(ne);
	for(convint il = 0; il < ne; il++){
		const int num_facet = element[il].getFacetNum();
		ele_facets_id[il].resize(num_facet, 0);
	}

#ifdef PRINT_CONV_INFO
	cout << "** start making facet data" << endl;
#endif	

	/* define facets */
	convint ifcnt = 0;
	for(convint il = 0; il < ne; il++){
		/*要素タイプ*/
		bool is3D = element[il].is3D();
		/* 2D要素以下は無視 */
		if( !is3D ){
			continue;
		}
		const ConvEType e_type1 = element[il].getType();
		/* この要素の面からスタート */
		const int num_facet = element[il].getFacetNum();
		for(int ik = 0; ik < num_facet; ik++){
			/* 重複確認 */
			int findflag  = 0;

			/* 一時Facetを作成 */
			convint nodes[8];
			convint f_nodes[4];
			convint edges[12];
			convint f_edges[4];
			element[il].getNodes(nodes);
			element[il].getEdges(edges);
			ConvEType e_type1f;
			/* 四面体の面 */
			if(e_type1 == ConvEType::Tetra){
				f_nodes[0] = nodes[tet_f_n[ik][0]];
				f_nodes[1] = nodes[tet_f_n[ik][1]];
				f_nodes[2] = nodes[tet_f_n[ik][2]];
				f_edges[0] = edges[tet_f[ik][0]];
				f_edges[1] = edges[tet_f[ik][1]];
				f_edges[2] = edges[tet_f[ik][2]];
				e_type1f = ConvEType::Tri;
			/* プリズムの面（上と下は三角形に） */
			}else if(e_type1 == ConvEType::Prizm){
				f_nodes[0] = nodes[pri_f_n[ik][0]];
				f_nodes[1] = nodes[pri_f_n[ik][1]];
				f_nodes[2] = nodes[pri_f_n[ik][2]];
				f_nodes[3] = nodes[pri_f_n[ik][3]];
				f_edges[0] = edges[ pri_f[ik][0] ];
				f_edges[1] = edges[ pri_f[ik][1] ];
				f_edges[2] = edges[ pri_f[ik][2] ];
				f_edges[3] = edges[ pri_f[ik][3] ];
				e_type1f = ConvEType::Quad;
				if(ik == 0 || ik == 1){
					e_type1f = ConvEType::Tri;
				}
			/* ピラミッドの面（下は四角形に） */
			}else if(e_type1 == ConvEType::Pyramid){
				f_nodes[0] = nodes[pyramid_f_n[ik][0]];
				f_nodes[1] = nodes[pyramid_f_n[ik][1]];
				f_nodes[2] = nodes[pyramid_f_n[ik][2]];
				f_nodes[3] = nodes[pyramid_f_n[ik][3]];
				f_edges[0] = edges[ pyramid_f[ik][0] ];
				f_edges[1] = edges[ pyramid_f[ik][1] ];
				f_edges[2] = edges[ pyramid_f[ik][2] ];
				f_edges[3] = edges[ pyramid_f[ik][3] ];
				e_type1f = ConvEType::Tri;
				if(ik == 0){
					e_type1f = ConvEType::Quad;
				}
			/* 六面体の面 */
			}else if(e_type1 == ConvEType::Hex){
				f_nodes[0] = nodes[hex_f_n[ik][0]];
				f_nodes[1] = nodes[hex_f_n[ik][1]];
				f_nodes[2] = nodes[hex_f_n[ik][2]];
				f_nodes[3] = nodes[hex_f_n[ik][3]];
				f_edges[0] = edges[ hex_f[ik][0] ];
				f_edges[1] = edges[ hex_f[ik][1] ];
				f_edges[2] = edges[ hex_f[ik][2] ];
				f_edges[3] = edges[ hex_f[ik][3] ];
				e_type1f = ConvEType::Quad;
			} else{
				cout << "This element facet in not define.... " << static_cast<char>(e_type1) <<endl;
				exit(1);
			}
			/*  一時Facetを作成! */
			ConvFacet temp_facet(e_type1f, f_nodes, f_edges);

			/* 隣接要素を探す */
			int numNeighbor = eleInfo[il].numNeighborEle;
			convint jf_id;
			for(int j = 0; j < numNeighbor && findflag == 0; j++){
				convint jl = eleInfo[il].neighborEle[j];
				/* お隣さんのタイプもゲット */
				bool is3Db = element[jl].is3D();
				/* 2D要素は無視 */
				if( !is3Db ){
					continue;
				}
				const int num_facet2 = element[jl].getFacetNum();
				/* お隣さんの面のforループ数セット */
				for(int jk = 0; jk < num_facet2; jk++){
					const convint jf = ele_facets_id[jl][jk];
					/* お隣がない＝無視して次へ */
					if(jf < 0){
						continue;
					}
					/* お隣があったなら、一致か判定 */
					bool is_same_facet = (temp_facet == facets[jf]);
					if(is_same_facet){
						findflag  = 1;
						jf_id = jf;
						break;
					}
				}
			}
			/* 重複がなければ、面を追加 */
			if (findflag == 0){
				temp_facet.setID(ifcnt);
				facets.push_back( std::move(temp_facet) );
				/* 要素の面IDを記憶 */
				ele_facets_id[il][ik] = ifcnt;
				ifcnt++;
				/* 既にあった場合： */
			} else{
				ele_facets_id[il][ik] = jf_id;
			}
		}
	}
	/***debug ****/
#ifdef PRINT_CONV_INFO
	cout << "** number of facet=" << ifcnt << endl;
#endif
}


/* end of namespace*/
};
};
