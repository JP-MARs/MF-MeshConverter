/**
* @file modify_mat.cpp
* @brief modify_mat 
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>

namespace JPMRspace{
namespace MeshConvLib{



/*
//=======================================================
// ● 材料番号を変更する(range_type... 0:全範囲、1:xyzで指定、2:rtzで指定、3：xyzで指定しold材料無視、4：rtzで指定しold材料無視) 
//=======================================================*/
/** 
 * @brief change mat id base function
*/
void MeshConverter::change_matID(ConvMesh& mesh, int range_type, int old_mat, int new_mat, double min1, double max1, double min2, double max2, double min3, double max3){
#ifdef PRINT_CONV_INFO
	cout << "Change mat ID" << endl << endl;
#endif

	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();
	/* 全範囲ならそのまま普通にやる */
	if(range_type == 0){
		for(auto i = 0 ; i < e_size ; i++){
			int mat = mesh.element[i].getMat();
			if(mat == old_mat){
				mesh.element[i].setMat(new_mat);
			}
		}
		return;
	}

	/* 対象要素選択開始 */
	vector<convint> selected_ele;
	for(convint i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double pos1, pos2, pos3;
		if(range_type == 0 || range_type == 1 || range_type == 3){
			pos1 = grav[0]; 
			pos2 = grav[1];
			pos3 = grav[2];
		}else{
			pos1 = sqrt( grav[0]*grav[0] + grav[1]*grav[1] ); 
			pos2 = atan2(grav[1], grav[0]);
			pos2 *= 180.0/DEF_CONV_PI;
			pos3 = grav[2];
		}
		bool inX = (pos1 >= min1 && pos1 <= max1);
		bool inY = (pos2 >= min2 && pos2 <= max2);
		bool inZ = (pos3 >= min3 && pos3 <= max3);
		if(inX && inY && inZ){
			selected_ele.push_back(i);
		}
	}
	/* 選らんだ要素の物性が指定されたものなら変更 */
	const auto the_size = selected_ele.size();
	for(convint i = 0 ; i < the_size ; i++){
		const convint e_id = selected_ele[i];
		if(range_type == 3 || range_type == 4){
			mesh.element[e_id].setMat(new_mat);
		}else{
			int mat = mesh.element[e_id].getMat();
			if(mat == old_mat){
				mesh.element[e_id].setMat(new_mat);
			}
		}
	}
}
/*
//=======================================================
// ● 材料番号を変更する(全範囲) 
//=======================================================*/
/** 
 * @brief change mat ID with mat id range
*/
void MeshConverter::change_matID(ConvMesh& mesh, int old_mat, int new_mat){
	this->change_matID(mesh, 0, old_mat, new_mat, -1, 1, -1, 1, -1, 1);
}
/*
//=======================================================
// ● 材料番号を変更する(xyzで指定)
//=======================================================*/
/** 
 * @brief change mat ID within xyz range
*/
void MeshConverter::change_matID_xyz(ConvMesh& mesh, int old_mat, int new_mat, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
	this->change_matID(mesh, 1, old_mat, new_mat, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//=======================================================
// ● 材料番号を変更する(rtzで指定)
//=======================================================*/
/** 
 * @brief change mat ID within rtz range
*/
void MeshConverter::change_matID_rtz(ConvMesh& mesh, int old_mat, int new_mat, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
	this->change_matID(mesh, 2, old_mat, new_mat, rmin, rmax, thmin, thmax, zmin, zmax);
}


/*
//=======================================================
// ● 材料番号を変更する(range_type... 0:全範囲、1:xyzで指定、2:rtzで指定、3：xyzで指定しold材料無視、4：rtzで指定しold材料無視) 
//=======================================================*/
/** 
 * @brief change mat ID within range and matid
*/
void MeshConverter::change_matID2(ConvMesh& mesh, int range_type, int new_mat, int mat1, int mat2, double min1, double max1, double min2, double max2, double min3, double max3){
#ifdef PRINT_CONV_INFO
	cout << "Change mat ID type2" << endl << endl;
#endif

	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();
	/* 全範囲ならそのまま普通にやる */
	if(range_type == 0){
		for(convint i = 0 ; i < e_size ; i++){
			int mat = mesh.element[i].getMat();
			if(mat1 <= mat && mat <= mat2){
				mesh.element[i].setMat(new_mat);
			}
		}
		return;
	}

	/* 対象要素選択開始 */
	vector<convint> selected_ele;
	for(int i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double pos1, pos2, pos3;
		if(range_type == 0 || range_type == 1 || range_type == 3){
			pos1 = grav[0]; 
			pos2 = grav[1];
			pos3 = grav[2];
		}else{
			pos1 = sqrt( grav[0]*grav[0] + grav[1]*grav[1] ); 
			pos2 = atan2(grav[1], grav[0]);
			pos2 *= 180.0/DEF_CONV_PI;
			pos3 = grav[2];
		}
		bool inX = (pos1 >= min1 && pos1 <= max1);
		bool inY = (pos2 >= min2 && pos2 <= max2);
		bool inZ = (pos3 >= min3 && pos3 <= max3);
		if(inX && inY && inZ){
			selected_ele.push_back(i);
		}
	}
	/* 選らんだ要素の物性が指定されたものなら変更 */
	const convint the_size = selected_ele.size();
	for(convint i = 0 ; i < the_size ; i++){
		const convint e_id = selected_ele[i];
		if(range_type == 3 || range_type == 4){
			mesh.element[e_id].setMat(new_mat);
		}else{
			int mat = mesh.element[e_id].getMat();
			if(mat1 <= mat && mat <= mat2){
				mesh.element[e_id].setMat(new_mat);
			}
		}
	}
}
/*
//=======================================================
// ● 材料番号を変更する(全範囲) 
//=======================================================*/
void MeshConverter::change_matID2(ConvMesh& mesh, int new_mat, int mat1, int mat2){
	this->change_matID2(mesh, 0, new_mat, mat1, mat2, -1, 1, -1, 1, -1, 1);
}
/*
//=======================================================
// ● 材料番号を変更する(xyzで指定)
//=======================================================*/
void MeshConverter::change_matID2_xyz(ConvMesh& mesh, int new_mat, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
	this->change_matID2(mesh, 1, new_mat, mat1, mat2, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//=======================================================
// ● 材料番号を変更する(rtzで指定)
//=======================================================*/
void MeshConverter::change_matID2_rtz(ConvMesh& mesh, int new_mat, int mat1, int mat2, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
	this->change_matID2(mesh, 2, new_mat, mat1, mat2, rmin, rmax, thmin, thmax, zmin, zmax);
}



/*
//=======================================================
// ● 材料番号を変更する・加算ver(range_type... 0:全範囲、1:xyzで指定、2:rtzで指定)
//=======================================================*/
/** 
 * @brief plus mat ID
*/
void MeshConverter::change_plus_matID(ConvMesh& mesh, int range_type, int plus_id, int mat1, int mat2, double min1, double max1, double min2, double max2, double min3, double max3){
#ifdef PRINT_CONV_INFO
	cout << "Change mat ID plus " << range_type << endl << endl;
#endif

	const auto e_size = mesh.element.size();
	/* 対象要素選択開始 */
	vector<convint> selected_ele;
	for(convint i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double pos1, pos2, pos3;
		if(range_type == 0 || range_type == 1){
			pos1 = grav[0]; 
			pos2 = grav[1];
			pos3 = grav[2];
		}else{
			pos1 = sqrt( grav[0]*grav[0] + grav[1]*grav[1] ); 
			pos2 = atan2(grav[1], grav[0]);
			pos2 *= 180.0/DEF_CONV_PI;
			pos3 = grav[2];
		}
		bool inX = (pos1 >= min1 && pos1 <= max1);
		bool inY = (pos2 >= min2 && pos2 <= max2);
		bool inZ = (pos3 >= min3 && pos3 <= max3);
		if(inX && inY && inZ){
			selected_ele.push_back(i);
		}
	}
	/* 選らんだ要素の物性が指定されたものなら変更 */
	const convint the_size = selected_ele.size();
	for(convint i = 0 ; i < the_size ; i++){
		const convint e_id = selected_ele[i];
		int mat = mesh.element[e_id].getMat();
		if(mat1 <= mat && mat <= mat2){
			mesh.element[e_id].setMat(mat+plus_id);
		}
	}
}
/*
//=======================================================
// ● 材料番号を変更する 加算ver(全範囲) 
//=======================================================*/
void MeshConverter::change_plus_matID(ConvMesh& mesh, int plus_id, int mat1, int mat2){
	this->change_plus_matID(mesh, 0, plus_id, mat1, mat2, -1, 1, -1, 1, -1, 1);
}
/*
//=======================================================
// ● 材料番号を変更する 加算ver(xyzで指定)
//=======================================================*/
void MeshConverter::change_plus_matID_xyz(ConvMesh& mesh, int plus_id, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
	this->change_plus_matID(mesh, 1, plus_id, mat1, mat2, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//=======================================================
// ● 材料番号を変更する 加算ver(rtzで指定)
//=======================================================*/
void MeshConverter::change_plus_matID_rtz(ConvMesh& mesh, int plus_id, int mat1, int mat2, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
	this->change_plus_matID(mesh, 2, plus_id, mat1, mat2, rmin, rmax, thmin, thmax, zmin, zmax);
}



/*
//=======================================================
// ● 指定した要素番号の要素の物質番号を指定した番号にする
//=======================================================*/
/** 
 * @brief set matID to ele ID
*/
void MeshConverter::change_matID_eid(ConvMesh& mesh, int new_mat, convint ele_id1, convint ele_id2){
#ifdef PRINT_CONV_INFO
	cout << "Change mat ID from ele ID" << endl << endl;
#endif

	/* 番号変更 */
	for(convint i = ele_id1 ; i <= ele_id2 ; i++){
		mesh.element[i].setMat(new_mat);
	}
}

/*
//=======================================================
// ● 材料番号を変更する(指定した材料IDのものを、要素ID+INCに) 
//=======================================================*/
/** 
 * @brief change matID to eleID+x
*/
void MeshConverter::change_mat_toEID(ConvMesh& mesh, int inc_num, int target_num, int* target_matid){
#ifdef PRINT_CONV_INFO
	cout << "Change mat-ID to Ele-ID" << endl;
#endif
	
	/* 要素ループ */
	const auto e_size = mesh.element.size();
	for(convint i = 0 ; i < e_size ; i++){
		int mat = mesh.element[i].getMat();
		bool bl = false;
		for(int j = 0 ; j < target_num ; j++){
			bl |= (mat == target_matid[j]);
		}
		if(bl){
			convint id  = mesh.element[i].getID();
			mesh.element[i].setMat(id + inc_num);
		}
	}
}

/*
//=======================================================
// ●  隣接要素同士でつながっている要素群を抜出、それらすべてを指定した材料番号に変更させる
//=======================================================*/
/** 
 * @brief set mat IDs for neighbouring relations
*/
void MeshConverter::change_neighbour_eles(ConvMesh& mesh, int num_start, convint* start_ele, int target_mat_num, int* target_matid, int after_mat, int inc){
#ifdef PRINT_CONV_INFO
	cout << "Change neighbouring Elements' mat-ID" << endl;
#endif

	/* まず、隣接要素群を作成 */
	if( !(mesh.info_maked) ){
		mesh.makeMeshInfo();
	}
	const auto e_size = mesh.element.size();
	bool is_3d = mesh.have_3D;
	vector<vector<convint>> Ele_neighbour;
	if(is_3d){
		mesh.make3D_FullNeighbour(Ele_neighbour);
	}else{
		mesh.make2D_FullNeighbour(Ele_neighbour);
	}

	bool *checked = new bool[e_size];
	for(convint i = 0 ; i < e_size ; i++){
		checked[i] = false;
	}
	/* 連結要素記憶vector */
	for(int eee = 0 ; eee < num_start ; eee++){
		vector<convint> changed_ele_IDs;
		convint ele_pos = start_ele[eee];
		/* すでに別ので変更済みなら、ムダだとして無視 */
		if(checked[ele_pos]){
			continue;
		}
		changed_ele_IDs.push_back(ele_pos);

		int nei_num_e = Ele_neighbour[ele_pos].size();
		checked[ele_pos] = true;
		for(int i = 0 ; i < nei_num_e ; i++){
			convint id = Ele_neighbour[ele_pos][i];
			change_neighbour_eles_subrutin(mesh, id, target_mat_num, target_matid, Ele_neighbour, changed_ele_IDs, checked);
		}

		/* 記憶した要素IDの材料変更 */
		const convint result_num = changed_ele_IDs.size();
		for(convint i = 0 ; i < result_num ; i++){
			convint id = changed_ele_IDs[i];
			mesh.element[id].setMat(after_mat+inc*eee);
		}
	}
	delete[] checked;
}

/*
//=======================================================
// ●  [サブルーチン]隣接要素同士でつながっている要素群を抜出、それらすべてを指定した材料番号に変更させる
//=======================================================*/
void MeshConverter::change_neighbour_eles_subrutin(ConvMesh& mesh, convint ele_id, int num_mat, int *target_ids, const vector<vector<convint>>& Ele_neighbour, vector<convint>& changed_ele_IDs, bool *checked){
	bool is_tar_mat = false;
	checked[ele_id] = true;
	for(int nn = 0 ; nn < num_mat; nn++){
		int mat = mesh.element[ele_id].getMat();
		is_tar_mat |= (mat == target_ids[nn]);
	}
	if(is_tar_mat){
		changed_ele_IDs.push_back(ele_id);
		convint nei_num_e = Ele_neighbour[ele_id].size();
		for(convint i = 0 ; i < nei_num_e ; i++){
			convint id = Ele_neighbour[ele_id][i];
			/* すでに調査済みなら無視 */
			if(checked[id]){
				continue;
			}
			change_neighbour_eles_subrutin(mesh, id, num_mat, target_ids, Ele_neighbour, changed_ele_IDs, checked);
		}
	}
}



/**/
};
};


