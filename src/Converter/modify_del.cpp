/**
* @file modify_del.cpp
* @brief modify_del 
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>

namespace JPMRspace{
namespace MeshConvLib{




/*
//=======================================================
// ● 選択初期化
//=======================================================*/
/** 
 * @brief select element init.
*/
void MeshConverter::select_inits(ConvMesh& mesh, vector<bool>& active_node, vector<bool>& active_ele){
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	active_node.resize(n_size);
	active_ele.resize(e_size);
	for(convint i = 0 ; i < n_size ; i++){
		active_node[i] = false;
	}
	for(convint i = 0 ; i < e_size ; i++){
		active_ele[i] = false;
	}
}
/*
//=======================================================
// ● 選択要素に対応した節点を選択
//=======================================================*/
/** 
 * @brief fix select nodes
*/
void MeshConverter::select_finish(ConvMesh& mesh, vector<bool>& active_node, vector<bool>& active_ele){
	/* 選択された要素に含まれる節点を選択する */
	const convint e_size = active_ele.size();
	for(convint i = 0 ; i < e_size ; i++){
		if(active_ele[i]){
			const int nn = mesh.element[i].getNodeNum();
			for(int j = 0 ; j < nn ; j++){
				convint n_id = mesh.element[i].getNode(j);
				active_node[n_id] = true;
			}
		}
	}
}

/*
//=======================================================
// ● 選択されている要素を削除
//=======================================================*/
/** 
 * @brief delete selected elements
*/
void MeshConverter::delete_ele(ConvMesh& mesh, vector<bool>& active_node, vector<bool>& active_ele){
#ifdef PRINT_CONV_INFO
	cout << "Delete Selected Elements " << endl << endl;
#endif
	const convint n_size = mesh.node.size();
	const convint ed_size = mesh.edge.size();
	const convint e_size = mesh.element.size();
#ifdef PRINT_CONV_INFO
	cout << "original mesh size " << n_size << ", " << ed_size << ", " << e_size << endl;
#endif
	/* 削除フラグを作成・初期化 */
	bool *dead_n = new bool[n_size];
	bool *dead_e = new bool[e_size];
	for(convint i = 0 ; i < n_size ; i++){
		dead_n[i] = true;
	}
	for(convint i = 0 ; i < e_size ; i++){
		dead_e[i] = false;
	}
	/* 選択済み要素の削除フラグをonに */
	for(convint i = 0 ; i < e_size ; i++){
		if(active_ele[i]){
			dead_e[i] = true;
		}
	}
	/* 選択された要素に含まれる節点は生き残る */
	for(convint i = 0 ; i < e_size ; i++){
		if( !(dead_e[i]) ){
			const int nn = mesh.element[i].getNodeNum();
			for(int j = 0 ; j < nn ; j++){
				convint n_id = mesh.element[i].getNode(j);
				dead_n[n_id] = false;
			}
		}
	}

	/* リナンバする */
	convint *new_id_n = new convint[n_size];
	convint *new_id_e = new convint[e_size];
	convint count_n=0, count_ed=0, count_e=0;
	for(convint i = 0 ; i < n_size ; i++){
		if( !(dead_n[i]) ){
			new_id_n[i] = count_n;
			count_n++;
		}else{
			new_id_n[i] = -99;
		}
	}
	for(convint i = 0 ; i < e_size ; i++){
		if( !(dead_e[i]) ){
			new_id_e[i] = count_e;
			count_e++;
		}else{
			new_id_e[i] = -99;
		}
	}

	for(convint i = 0 ; i < n_size ; i++){
		if( !(dead_n[i]) ){
			if(new_id_n[i] < 0){
				cout << "delete renumber Error! -- N"<<endl;
			}
			mesh.node[i].setID(new_id_n[i]);
		}
	}
	for(convint i = 0 ; i < e_size ; i++){
		if( !(dead_e[i]) ){
			if(new_id_e[i] < 0){
				cout << "delete renumber Error! -- E"<<endl;
			}
			mesh.element[i].setID(new_id_e[i]);
			/* 要素の節点番号もリナンバ */
			const int nn = mesh.element[i].getNodeNum();
			for(int j = 0 ; j < nn ; j++){
				convint ori_n = mesh.element[i].getNode(j);
				convint new_n = new_id_n[ori_n];
				if(new_n < 0){
					cout << "delete renumber Error! -- E's node"<<endl;
				}
				mesh.element[i].setNodes(j, new_n);
			}
		}
	}
	delete[] new_id_n;
	delete[] new_id_e;

	/* 削除開始 */
	vector<ConvNode> temp_N;
	vector<ConvElement> temp_E;
	temp_N.reserve(count_n);
	for(int i = 0 ; i < n_size ; i++){
		if( !(dead_n[i]) ){
			temp_N.push_back(mesh.node[i]);
		}
	}
	mesh.node.swap(temp_N);
	temp_N.clear();

	temp_E.reserve(count_e);
	for(int i = 0 ; i < e_size ; i++){
		if( !(dead_e[i]) ){
			temp_E.push_back(mesh.element[i]);
		}
	}
	mesh.element.swap(temp_E);

	mesh.edge.clear();
	delete[] dead_n;
	delete[] dead_e;

#ifdef PRINT_CONV_INFO
	cout << "original mesh size " << mesh.node.size() << ", " << mesh.element.size() << endl;
#endif
}





/*
//=======================================================
// ● 削除・全要素選択
//=======================================================*/
/** 
 * @brief delete all elements
*/
void MeshConverter::delete_ele_all(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "Select All Elements" << endl << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	select_inits(mesh, active_node, active_ele);
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	for(convint i = 0 ; i < n_size ; i++){
		active_node[i] = true;
	}
	for(convint i = 0 ; i < e_size ; i++){
		active_ele[i] = true;
	}
	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}


/*
//=======================================================
// ● 削除・要素タイプで選択
//=======================================================*/
/** 
 * @brief delete target element type
*/
void MeshConverter::delete_ele_type(ConvMesh& mesh, const ConvEType ele_type){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by Ele Type" << endl << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(int i = 0 ; i < e_size ; i++){
		ConvEType e_type = mesh.element[i].getType();
		bool bl = (e_type == ele_type);
		if(bl){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}


/*
//=======================================================
// ● 削除・番号で要素選択
//=======================================================*/
/** 
 * @brief delete target ID elements
*/
void MeshConverter::delete_ele_id(ConvMesh& mesh, const int ele_mat_num, const vector<convint>& selects1, const vector<convint>& selects2){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by Ele number" << endl << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(int i = 0 ; i < e_size ; i++){
		convint id = mesh.element[i].getID();
		convint start = selects1[i];
		convint end = selects2[i];
		if(start <= id && id <= end){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}
/*
//=======================================================
// ● 削除・要素選択-(XYZ)
//=======================================================*/
/** 
 * @brief delete elements withn target range (XYZ)
*/
void MeshConverter::delete_ele_xyz(ConvMesh& mesh, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by cor. XYZ" << endl << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(int i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		bool inX = (grav[0] >= xmin && grav[0] <= xmax);
		bool inY = (grav[1] >= ymin && grav[1] <= ymax);
		bool inZ = (grav[2] >= zmin && grav[2] <= zmax);
		if(inX && inY && inZ){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}
/*
//=======================================================
// ● 削除・要素選択-(RθZで指定)
//=======================================================*/
/** 
 * @brief delete elements withn target range (RTZ)
*/
void MeshConverter::delete_ele_RTHZ(ConvMesh& mesh, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by cor. R TH Z" << endl << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(convint i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double rr = sqrt(grav[0]*grav[0] + grav[1]*grav[1]);
		double theta = atan2(grav[1], grav[0]);
		theta *= 180.0/DEF_CONV_PI;
		bool inR = (rr>= rmin && rr <= rmax);
		bool inTH = (theta >= thmin && theta <= thmax);
		bool inZ = (grav[2] >= zmin && grav[2] <= zmax);
		if(inR && inTH && inZ){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}
/*
//=======================================================
// ● 削除・要素選択-材料番号で指定
//=======================================================*/
/** 
 * @brief delete elements withn material ID
*/
void MeshConverter::delete_ele_MAT(ConvMesh& mesh, int minMatID, int maxMatID){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by MAT ID" << endl << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(convint i = 0 ; i < e_size ; i++){
		int mat = mesh.element[i].getMat();
		bool inMAT = (mat >= minMatID && mat <= maxMatID);
		if(inMAT){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}
/*
//=======================================================
// ● 削除・要素選択-範囲と材料で指定(XYZ) 
//=======================================================*/
/** 
 * @brief delete elements withn target range (XYZ) and mt
*/
void MeshConverter::delete_ele_MAT_xyz(ConvMesh& mesh, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int minMatID, int maxMatID){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by cor. XYZ & MAT " << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(convint i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		bool inX = (grav[0] >= xmin && grav[0] <= xmax);
		bool inY = (grav[1] >= ymin && grav[1] <= ymax);
		bool inZ = (grav[2] >= zmin && grav[2] <= zmax);
		int mat = mesh.element[i].getMat();
		bool isMAT = (minMatID <= mat && mat <= maxMatID);
		if(inX && inY && inZ && isMAT){			
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}

/*
//=======================================================
// ● 削除・要素選択-範囲と材料で指定(RθZで指定) 
//=======================================================*/
/** 
 * @brief delete elements withn target range (RTZ) and mat
*/
void MeshConverter::delete_ele_MAT_RTHZ(ConvMesh& mesh, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax, int minMatID, int maxMatID){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements by cor. RTZ & MAT " << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(convint i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double rr = sqrt(grav[0]*grav[0] + grav[1]*grav[1]);
		double theta = atan2(grav[1], grav[0]);
		theta *= 180.0/DEF_CONV_PI;
		bool inR = (rr>= rmin && rr <= rmax);
		bool inTH = (theta >= thmin && theta <= thmax);
		bool inZ = (grav[2] >= zmin && grav[2] <= zmax);
		int mat = mesh.element[i].getMat();
		bool isMAT = (minMatID <= mat && mat <= maxMatID);
		if(inR && inTH && inZ && isMAT){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}

/*
//=======================================================
// ● 削除・要素選択2D要素削除
//=======================================================*/
/** 
 * @brief delete 2D elements 
*/
void MeshConverter::delete_ele_2D(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "Select Elements 2D " << endl;
#endif
	vector<bool> active_node;
	vector<bool> active_ele;
	/* まずすべて非選択 */
	select_inits(mesh, active_node, active_ele);
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 要素選択開始 */
	for(convint i = 0 ; i < e_size ; i++){
		bool is3D = mesh.element[i].is3D();
		bool is1D = mesh.element[i].is1D();
		bool is2D = !(is3D || is1D);
		if(is2D){
			active_ele[i] = true;
		}
	}
	/* 選択された要素に含まれる節点を選択する */
	select_finish(mesh, active_node, active_ele);

	/* 削除！ */
	this->delete_ele(mesh, active_node, active_ele);
}

/**/
};
};


