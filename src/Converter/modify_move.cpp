/**
* @file modify_move.cpp
* @brief modify_move
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>


namespace JPMRspace{
namespace MeshConvLib{




/*
//=======================================================
// ● 該当範囲の節点を移動する(0:X, 1:Y, 2:Z,  4:Xの移動位置指定,  5:Yの移動位置指定,  6:Zの移動位置指定)
//=======================================================*/
/** 
 * @brief move node positions (after pos set)
*/
void MeshConverter::move_node_xyzdir(ConvMesh& mesh, int move_mode, double move_val,
	double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir by cor. XYZ !" << endl << endl;
#endif

	/* まずすべて非選択 */
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(convint i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		bool inX = (xx >= xmin && xx <= xmax);
		bool inY = (yy >= ymin && yy <= ymax);
		bool inZ = (zz >= zmin && zz <= zmax);
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 選らんだ節点を移動する */
	const convint the_size = selected_node.size();
	for(convint i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		if(move_mode == 0){
			xx += move_val;
		}else if(move_mode == 1){
			yy += move_val;
		}else if(move_mode == 2){
			zz += move_val;
		/**/
		}else if(move_mode == 4){
			xx = move_val;
		}else if(move_mode == 5){
			yy = move_val;
		}else if(move_mode == 6){
			zz = move_val;
		}
		mesh.node[n_id].set(xx, yy, zz);
	}
}

/*
//=======================================================
// ● 該当範囲の節点を移動する(0:T, 1:theta, 2:Z,  4:Rの移動位置指定,  5:Thetaの移動位置指定,  6:Zの移動位置指定)
//=======================================================*/
/** 
 * @brief move node positions (RTZ)
*/
void MeshConverter::move_node_rtzdir(ConvMesh& mesh, int move_mode, double move_val,
	double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for RTZ-dir by cor. RTZ" << endl << endl;
#endif

	/* まずすべて非選択 */
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(convint i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;
		bool inR = (rr >= rmin && rr <= rmax);
		bool inTH = (theta >= thmin && theta <= thmax);
		bool inZ = (zz >= zmin && zz <= zmax);
		if(inR && inTH && inZ){
			selected_node.push_back(i);
		}
	}
	/* 選らんだ節点を移動する */
	const convint the_size = selected_node.size();
	for(convint i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;
		if(move_mode == 0){
			rr += move_val;
		}else if(move_mode == 1){
			theta += move_val;
		}else if(move_mode == 2){
			zz += move_val;
		/**/
		}else if(move_mode == 4){
			rr = move_val;
		}else if(move_mode == 5){
			theta = move_val;
		}else if(move_mode == 6){
			zz = move_val;
		}
		theta *= DEF_CONV_PI/180.0;
		xx = rr*cos(theta);
		yy = rr*sin(theta);
		mesh.node[n_id].set(xx, yy, zz);
	}
}


/*
//=======================================================
// ● 指定したIDの節点を平行移動する（0:XYZで移動、1:RTZで移動）
//=======================================================*/
/** 
 * @brief move node positions (tareget ID)
*/
void MeshConverter::move_node_id_xyz(ConvMesh& mesh,int move_mode, int num_ids, convint* target_ids, double* move_1, double* move_2, double* move_3){
#ifdef PRINT_CONV_INFO
	cout << "Move Target ID Node" << endl << endl;
#endif
	/* 移動させる */
	for(int i = 0 ; i < num_ids ; i++){
		const convint id = target_ids[i];
		double xx = mesh.node[id].getX();
		double yy = mesh.node[id].getY();
		double zz = mesh.node[id].getZ();
		/* xyz方向へ移動 */
		if(move_mode == 0){
			xx += move_1[i]; yy += move_2[i]; zz += move_3[i];
			/* RTZ方向へ移動 */
		}else{
			double rr = sqrt(xx*xx + yy*yy);
			double theta = atan2(yy, xx);
			theta *= 180.0/DEF_CONV_PI;
			rr += move_1[i]; theta += move_2[i]; zz += move_3[i];
			theta *= DEF_CONV_PI/180.0;
			xx = rr*cos(theta);
			yy = rr*sin(theta);
		}
		mesh.node[id].set(xx, yy, zz);
	}
}


/*
//=======================================================
// ● 該当範囲の節点のx座標を指定する(XminとXmaxを指定し、あとは線形補間)
//=======================================================*/
/** 
 * @brief move node positions (linear interpolation) XYZ
*/
void MeshConverter::move_node_xyz_type2(ConvMesh& mesh, int move_mode, double min_move, double max_move, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir interpolation type by cor. XYZ2" << endl << endl;
#endif

	/* まずすべて非選択 */
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(convint i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
		double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
		bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 線形の差分を求める */
	double move_grad;
	if(move_mode == 0){
		move_grad = (max_move - min_move) / (xmax - xmin);
	}else if(move_mode == 1){
		move_grad = (max_move - min_move) / (ymax - ymin);
	}else if(move_mode == 2){
		move_grad = (max_move - min_move) / (zmax - zmin);
	}
	/* 選らんだ節点を移動する */
	const convint the_size = selected_node.size();
	for(convint i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		if(move_mode == 0){
			double move_val = (xx - xmin) * move_grad + min_move;
			xx += move_val;
		}else if(move_mode == 1){
			double move_val = (yy - ymin) * move_grad + min_move;
			yy += move_val;
		}else if(move_mode == 2){
			double move_val = (zz - zmin) * move_grad + min_move;
			zz += move_val;
		}
		mesh.node[n_id].set(xx, yy, zz);
	}
}


/*
//=======================================================
// ● 該当範囲の節点のR座標を指定する(RminとRmaxを指定し、あとは線形補間) 
//=======================================================*/
/** 
 * @brief move node positions (linear interpolation) RTZ
*/
void MeshConverter::move_node_rtz_type2(ConvMesh& mesh, int move_mode, double min_move, double max_move, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir interpolation type by cor. RTZ" << endl << endl;
#endif

	/* まずすべて非選択 */
	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(auto i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;
		double diff_rmin = fabs(rr-rmin); double diff_rmax = fabs(rr-rmax);
		double diff_thmin = fabs(theta-thmin); double diff_thmax = fabs(theta-thmax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (rr >= rmin || diff_rmin < eps_diff) && (rr <= rmax || diff_rmax < eps_diff) );
		bool inY = ( (theta >= thmin || diff_thmin < eps_diff) && (theta <= thmax || diff_thmax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 線形の差分を求める */
	double move_grad;
	if(move_mode == 0){
		move_grad = (max_move - min_move) / (rmax - rmin);
	}else if(move_mode == 1){
		move_grad = (max_move - min_move) / (thmax - thmin);
	}else if(move_mode == 2){
		move_grad = (max_move - min_move) / (zmax - zmin);
	}
	/* 選らんだ節点を移動する */
	const auto the_size = selected_node.size();
	for(auto i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;
		if(move_mode == 0){
			double move_val = (rr - rmin) * move_grad + min_move;
			rr += move_val;
		}else if(move_mode == 1){
			double move_val = (theta - thmin) * move_grad + min_move;
			theta += move_val;
		}else if(move_mode == 2){
			double move_val = (zz - zmin) * move_grad + min_move;
			zz += move_val;
		}
		theta *= DEF_CONV_PI/180.0;
		xx = rr*cos(theta);
		yy = rr*sin(theta);
		mesh.node[n_id].set(xx, yy, zz);
	}
}

/*
//=======================================================
// ● 該当範囲の節点のxyz座標を指定する(線形補間を計算する軸と座標範囲を指定して移動)
//=======================================================*/
/** 
 * @brief move node positions (linear interpolation) XYZ (other axis)
*/
void MeshConverter::move_node_xyz_type3(ConvMesh& mesh, int move_mode, int axis, double min_move, double max_move, double min_v, double max_v,
	double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir interpolation type by cor. XYZ -Type 3" << endl << endl;
#endif

	/* まずすべて非選択 */
	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(auto i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
		double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
		bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 線形の差分を求める */
	double move_grad;
	move_grad = (max_move - min_move) / (max_v - min_v);
#ifdef PRINT_CONV_INFO
	cout << "move gd " << move_grad << endl;
#endif
	/* 選らんだ節点を移動する */
	const auto the_size = selected_node.size();
	for(auto i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		double move_val;
		if(axis == 0){
			move_val = (xx - min_v) * move_grad + min_move;
			if(xx < min_v) move_val = min_move;
			if(xx > max_v) move_val = max_move;
		}else if(axis == 1){
			move_val = (yy - min_v) * move_grad + min_move;
			if(yy < min_v) move_val = min_move;
			if(yy > max_v) move_val = max_move;
		}else{
			move_val = (zz - min_v) * move_grad + min_move;
			if(zz < min_v) move_val = min_move;
			if(zz > max_v) move_val = max_move;
		}
		if(move_mode == 0){
			xx += move_val;
		}else if(move_mode == 1){
			yy += move_val;
		}else if(move_mode == 2){
			zz += move_val;
		}
		mesh.node[n_id].set(xx, yy, zz);
	}
}

/*
//=======================================================
// ● 該当範囲の節点のrtz座標を指定する(線形補間を計算する軸と座標範囲を指定して移動)
//=======================================================*/
/** 
 * @brief move node positions (linear interpolation) RTZ (other axis)
*/
void MeshConverter::move_node_rtz_type3(ConvMesh& mesh, int move_mode, int axis, double min_move, double max_move, double min_v, double max_v, 
	double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir interpolation type by cor. RTZ" << endl << endl;
#endif

	/* まずすべて非選択 */
	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(auto i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;
		double diff_rmin = fabs(rr-rmin); double diff_rmax = fabs(rr-rmax);
		double diff_thmin = fabs(theta-thmin); double diff_thmax = fabs(theta-thmax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (rr >= rmin || diff_rmin < eps_diff) && (rr <= rmax || diff_rmax < eps_diff) );
		bool inY = ( (theta >= thmin || diff_thmin < eps_diff) && (theta <= thmax || diff_thmax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 線形の差分を求める */
	double move_grad;
	move_grad = (max_move - min_move) / (max_v - min_v);

	/* 選らんだ節点を移動する */
	const auto the_size = selected_node.size();
	for(auto i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;

		double move_val;
		if(axis == 0){
			move_val = (rr - min_v) * move_grad + min_move;
			if(rr < min_v) move_val = min_move;
			if(rr > max_v) move_val = max_move;
		}else if(axis == 1){
			move_val = (theta - min_v) * move_grad + min_move;
			if(theta < min_v) move_val = min_move;
			if(theta > max_v) move_val = max_move;
		}else{
			move_val = (zz - min_v) * move_grad + min_move;
			if(zz < min_v) move_val = min_move;
			if(zz > max_v) move_val = max_move;
		}
		if(move_mode == 0){
			rr += move_val;
		}else if(move_mode == 1){
			theta += move_val;
		}else if(move_mode == 2){
			zz += move_val;
		}
		theta *= DEF_CONV_PI/180.0;
		xx = rr*cos(theta);
		yy = rr*sin(theta);
		mesh.node[n_id].set(xx, yy, zz);
	}
}



/*
//=======================================================
// ● 該当範囲の節点のxyz座標を指定する・材料範囲も指定(線形補間を計算する軸と座標範囲を指定して移動)
//=======================================================*/
/** 
 * @brief move node positions (mat range)
*/
void MeshConverter::move_node_xyz_type3_mat(ConvMesh& mesh, int move_mode, int mat1, int mat2, int axis, double min_move, double max_move, 
	double min_v, double max_v, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir interpolation type by cor. XYZ -Type 3 MAT" << endl << endl;
#endif

	/* まずすべて非選択 */
	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();
	/* 要素が使っているかチェック*/
	bool *ele_ok_node = new bool[n_size];
	for(auto i = 0 ; i < n_size ; i++){
		ele_ok_node[i] = false;
	}
	for(auto e = 0 ; e < e_size ; e++){
		int mat = mesh.element[e].getMat();
		if(mat1 <= mat && mat <= mat2){
			int n_num = mesh.element[e].getNodeNum();
			for(int i = 0 ; i < n_num ; i++){
				convint n_id = mesh.element[e].getNode(i);
				ele_ok_node[n_id] = true;
			}
		}
	}

	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(auto i = 0 ; i < n_size ; i++){
		if( !ele_ok_node[i] ) continue;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
		double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
		bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 線形の差分を求める */
	double move_grad;
	move_grad = (max_move - min_move) / (max_v - min_v);
#ifdef PRINT_CONV_INFO
	cout << "move gd " << move_grad << endl;
#endif
	/* 選らんだ節点を移動する */
	const auto the_size = selected_node.size();
	for(auto i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		double move_val;
		if(axis == 0){
			move_val = (xx - min_v) * move_grad + min_move;
			if(xx < min_v) move_val = min_move;
			if(xx > max_v) move_val = max_move;
		}else if(axis == 1){
			move_val = (yy - min_v) * move_grad + min_move;
			if(yy < min_v) move_val = min_move;
			if(yy > max_v) move_val = max_move;
		}else{
			move_val = (zz - min_v) * move_grad + min_move;
			if(zz < min_v) move_val = min_move;
			if(zz > max_v) move_val = max_move;
		}
		if(move_mode == 0){
			xx += move_val;
		}else if(move_mode == 1){
			yy += move_val;
		}else if(move_mode == 2){
			zz += move_val;
		}
		mesh.node[n_id].set(xx, yy, zz);
	}
	delete[] ele_ok_node;
}


/*
//=======================================================
// ● 該当範囲の節点のrtz座標を指定する・材料範囲も指定(線形補間を計算する軸と座標範囲を指定して移動)
//=======================================================*/
/** 
 * @brief move node positions mat and XYZ range
*/
void MeshConverter::move_node_rtz_type3_mat(ConvMesh& mesh, int move_mode, int mat1, int mat2, int axis, double min_move, double max_move, double min_v, double max_v, 
	double rmin, double rmax, double thmin, double thmax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Move Node for XYZ-dir interpolation type by cor. RTZ  -Type 3 MATT" << endl << endl;
#endif

	/* まずすべて非選択 */
	const auto n_size = mesh.node.size();
	const auto e_size = mesh.element.size();

	/* 要素が使っているかチェック*/
	bool *ele_ok_node = new bool[n_size];
	for(auto  i = 0 ; i < n_size ; i++){
		ele_ok_node[i] = false;
	}
	for(auto e = 0 ; e < e_size ; e++){
		int mat = mesh.element[e].getMat();
		if(mat1 <= mat && mat <= mat2){
			int n_num = mesh.element[e].getNodeNum();
			for(int i = 0 ; i < n_num ; i++){
				convint n_id = mesh.element[e].getNode(i);
				ele_ok_node[n_id] = true;
			}
		}
	}

	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(auto i = 0 ; i < n_size ; i++){
		if( !ele_ok_node[i] ) continue;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;
		double diff_rmin = fabs(rr-rmin); double diff_rmax = fabs(rr-rmax);
		double diff_thmin = fabs(theta-thmin); double diff_thmax = fabs(theta-thmax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (rr >= rmin || diff_rmin < eps_diff) && (rr <= rmax || diff_rmax < eps_diff) );
		bool inY = ( (theta >= thmin || diff_thmin < eps_diff) && (theta <= thmax || diff_thmax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		if(inX && inY && inZ){
			selected_node.push_back(i);
		}
	}
	/* 線形の差分を求める */
	double move_grad;
	move_grad = (max_move - min_move) / (max_v - min_v);

	/* 選らんだ節点を移動する */
	const auto the_size = selected_node.size();
	for(auto i = 0 ; i < the_size ; i++){
		const convint n_id = selected_node[i];
		double xx = mesh.node[n_id].getX(); double yy = mesh.node[n_id].getY(); double zz = mesh.node[n_id].getZ();
		double rr = sqrt(xx*xx + yy*yy);
		double theta = atan2(yy, xx);
		theta *= 180.0/DEF_CONV_PI;

		double move_val;
		if(axis == 0){
			move_val = (rr - min_v) * move_grad + min_move;
			if(rr < min_v) move_val = min_move;
			if(rr > max_v) move_val = max_move;
		}else if(axis == 1){
			move_val = (theta - min_v) * move_grad + min_move;
			if(theta < min_v) move_val = min_move;
			if(theta > max_v) move_val = max_move;
		}else{
			move_val = (zz - min_v) * move_grad + min_move;
			if(zz < min_v) move_val = min_move;
			if(zz > max_v) move_val = max_move;
		}
		if(move_mode == 0){
			rr += move_val;
		}else if(move_mode == 1){
			theta += move_val;
		}else if(move_mode == 2){
			zz += move_val;
		}
		theta *= DEF_CONV_PI/180.0;
		xx = rr*cos(theta);
		yy = rr*sin(theta);
		mesh.node[n_id].set(xx, yy, zz);
	}
	delete[] ele_ok_node;
}
/**/
};
};


