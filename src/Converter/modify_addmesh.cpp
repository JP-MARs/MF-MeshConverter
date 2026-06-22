/**
* @file modify_addmesh.cpp
* @brief modify_addmesh
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>
#include <fstream>

namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ 別のメッシュを読み取って結合
//===============================================================*/
/** 
 * @brief connect two meshes
*/
void MeshConverter::add_mesh(ConvMesh& mesh, ConvMesh& mesh2, int kmat, int knod, int surf, double f_val){
#ifdef PRINT_CONV_INFO
	cout << "add mesh "<<endl<<endl;
#endif
	/* メッシュ情報を作っていなかったら作成する */
	if( !(mesh.info_maked) ){
		mesh.makeMeshInfo();
	}

	/* 結合法の読み取り */
	/*int kmat, knod, ftype;
	string filename2;
	int surf;
	double f_val;*/

	/* 合体させる節点・辺・要素データ作成 */
	/* メッシュ情報を作っていなかったら作成する */
	if( !(mesh2.info_maked) ){
		mesh2.makeMeshInfo();
	}

	/* 材料IDを更新 */
	if(kmat == 0){
		add_mesh_mat_mod(mesh, mesh2);
	}
	/* 重複節点削除して、メッシュ合体 */
	add_mesh_connect_and_del_node(mesh, mesh2, surf, f_val, knod);

	/* 作成済み情報はリセット */
	mesh.info_maked = false;
}

/*
//===============================================================
// ○ メッシュ結合サブルーチン１：材料更新
//===============================================================*/
void MeshConverter::add_mesh_mat_mod(ConvMesh& mesh, ConvMesh& mesh2){
	const convint n_size1 = mesh.node.size();
	const convint e_size1 = mesh.element.size();
	const convint n_size2 = mesh2.node.size();
	const convint e_size2 = mesh2.element.size();
	/* 元メッシュの材料ID最大値をサーチ */
	int max_ID=-999;
	for(convint i = 0 ; i < e_size1 ; i++){
		int mat = mesh.element[i].getMat();
		if(mat > max_ID){
			max_ID = mat;
		}
	}
	max_ID++;
	/* 付け足す側の材料IDにmaxを足す */
	for(convint i = 0 ; i < e_size2 ; i++){
		int mat = mesh2.element[i].getMat();
		mesh2.element[i].setMat(mat+max_ID);
	}
}

/*
//===============================================================
// ○ メッシュ結合サブルーチン２：重複削除しでメッシュ合体
//===============================================================*/
void MeshConverter::add_mesh_connect_and_del_node(ConvMesh& mesh, ConvMesh& mesh2, int surf, double f_val, int knod){

	const convint n_size1 = mesh.node.size();
	const convint e_size1 = mesh.element.size();
	const convint n_size2 = mesh2.node.size();
	const convint e_size2 = mesh2.element.size();
	/* 削除フラグと更新後IDを初期化 */
	bool *dead_n = new bool[n_size2];
	convint *new_ID = new convint[n_size2];
	for(convint i = 0 ; i < n_size2 ; i++){
		dead_n[i] = false;
	}
	/* 重複削除の場合 */
	if(knod == 0){
		vector<convint> surf_node1;
		vector<convint> surf_node2;
		/* 重複を探す表面の節点を持ってくる */
		if(surf != -1){
			for(convint i = 0 ; i < n_size1 ; i++){
				double diff;
				if(surf == 0){
					double val = mesh.node[i].getX();
					diff = fabs(val - f_val);
				}else if(surf == 1){
					double val = mesh.node[i].getY();
					diff = fabs(val - f_val);
				}else if(surf == 2){
					double val = mesh.node[i].getZ();
					diff = fabs(val - f_val);
				}else if(surf == 3){
					double val1 = mesh.node[i].getX();
					double val2 = mesh.node[i].getY();
					double val = sqrt(val1*val1 + val2*val2);
					diff = fabs(val - f_val);
				}else{
					diff = 0.0;
				}
				if(diff < eps_diff*1000.0){
					surf_node1.push_back(i);
				}
			}
		}else{
			/* 境界節点だけ選ぶ */
			const convint b_n_size1 = mesh.boundNodeList.size();
			for(convint i = 0 ; i <b_n_size1 ; i++){
				const convint id0 = mesh.boundNodeList[i];
				surf_node1.push_back(id0);
			}
		}
		if(surf != -1){
			for(convint i = 0 ; i < n_size2 ; i++){
				double diff;
				if(surf == 0){
					double val = mesh2.node[i].getX();
					diff = fabs(val - f_val);
				}else if(surf == 1){
					double val = mesh2.node[i].getY();
					diff = fabs(val - f_val);
				}else if(surf == 2){
					double val = mesh2.node[i].getZ();
					diff = fabs(val - f_val);
				}else if(surf == 3){
					double val1 = mesh2.node[i].getX();
					double val2 = mesh2.node[i].getY();
					double val = sqrt(val1*val1 + val2*val2);
					diff = fabs(val - f_val);
				}else{
					diff = 0.0;
				}
				if(diff < eps_diff*1000.0){
					surf_node2.push_back(i);
				}
			}
		}else{
			/* 境界節点だけ選ぶ */
			const convint b_n_size2 = mesh2.boundNodeList.size();
			for(int i = 0 ; i <b_n_size2 ; i++){
				const convint id0 = mesh2.boundNodeList[i];
				surf_node2.push_back(id0);
			}
		}
		/* 重複探索開始 */
#ifdef PRINT_CONV_INFO
		cout << "start delete lapped nodes"<<endl;
#endif
		convint del_num=0;
		const convint nf_size1 = surf_node1.size();
		const convint nf_size2 = surf_node2.size();
		for(convint i = 0 ; i < nf_size1 ; i++){
			convint n_id1 = surf_node1[i];
			double xx1 = mesh.node[n_id1].getX();
			double yy1 = mesh.node[n_id1].getY();
			double zz1 = mesh.node[n_id1].getZ();
			/* ノード２で同じものを探す */
			for(convint j = 0 ; j < nf_size2 ; j++){
				convint n_id2 = surf_node2[j];
				double xx2 = mesh2.node[n_id2].getX();
				double yy2 = mesh2.node[n_id2].getY();
				double zz2 = mesh2.node[n_id2].getZ();
				double dx = (xx1-xx2)*(xx1-xx2); double dy = (yy1-yy2)*(yy1-yy2); double dz = (zz1-zz2)*(zz1-zz2);
				double dx2 = sqrt(dx); double dy2 = sqrt(dy); double dz2 = sqrt(dz);
				bool judge1 = (dx2 < eps_diff) && (dy2 < eps_diff) && (dz2 < eps_diff);
				/* 重複していたら削除フラグonし、nodeでのIDを記憶 */
				if(judge1){
					dead_n[n_id2] = true;
					new_ID[n_id2] = n_id1;
					del_num++;
					//cout << del_num << endl;
					if(del_num % 500 == 0){
#ifdef PRINT_CONV_INFO
						cout << "Del lapped num : " << del_num << endl;
#endif
					}
					break;
				}
			}
		}
#ifdef PRINT_CONV_INFO
		cout << "Del total : " << del_num << endl;
#endif
	}
	/* 新IDをセット */
	convint count_n2=0;
	for(convint i = 0 ; i < n_size2 ; i++){
		if( !(dead_n[i]) ){
			new_ID[i] = count_n2 + n_size1;
			count_n2++;
			mesh2.node[i].setID(new_ID[i]);
		}
	}
	/* 要素2のIDと節点番号も更新	*/
	for(convint i = 0 ; i < e_size2 ; i++){
		/* 要素ID更新 */
		convint e_id = mesh2.element[i].getID();
		mesh2.element[i].setID(e_id + e_size1);
		/* 節点更新 */
		convint nodes[30];
		mesh2.element[i].getNodes(nodes);
		int num0 = mesh2.element[i].getNodeNum();
		for(int j = 0 ; j < num0 ; j++){
			convint old_id = nodes[j];
			nodes[j] = new_ID[old_id];
		}
		mesh2.element[i].setNodes(nodes);
	}

	vector<ConvNode> empt_n;
	empt_n.reserve(0);
	/* ２つのメッシュ合体 */
	for(convint i = 0 ; i < n_size2 ; i++){
		if( !(dead_n[i]) ){
			mesh.node.push_back(mesh2.node[i]);
		}
	}
	mesh2.node.swap(empt_n);
	vector<ConvElement> empt_e;
	empt_e.reserve(0);
	for(convint i = 0 ; i < e_size2 ; i++){
		mesh.element.push_back( std::move(mesh2.element[i]) );
	}
	mesh2.element.swap(empt_e);

	delete[] dead_n;
	delete[] new_ID;
}


/**/
};
};


