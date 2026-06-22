/**
* @file modify_info_out.cpp
* @brief modify_info_out
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>
#include <fstream>

namespace JPMRspace{
namespace MeshConvLib{



/*
//=======================================================
// ● 指定した要素IDの要素の面積を計算する（2D要素のみ）
//=======================================================*/
/** 
 * @brief calc element area of 2D
*/
void MeshConverter::calc_area_ele(ConvMesh& mesh, const vector<convint>& target_ele_ids, map<convint, double>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "Calc Ele surface area " << endl;
#endif

	int id_num = target_ele_ids.size();
	vector<convint> id_data = target_ele_ids;

	result.clear();
	double *area_val = new double[id_num];
	for(int i = 0 ; i < id_num ; i++){
		convint id = id_data[i];
		double area = mesh.element[id].calcArea(mesh.node);
		area_val[i] = area;
		result[id] = area;
	}
	if(is_print){
		std::fstream fp("Ele_area.csv", std::ios::out);
		for(int i = 0 ; i < id_num ; i++){
			fp << id_data[i] << ", " << std::scientific << area_val[i] << endl;
		}
		fp.close();
	}
}


/*
//=======================================================
// ● 指定した材料IDの要素の面積を計算し、その合計を計算する（2D要素のみ） 
//=======================================================*/
/** 
 * @brief calc element target area 
*/
void MeshConverter::calc_area_mat(ConvMesh& mesh, const vector<int>& target_mat, map<convint, double>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "Calc Ele surface area MAT" << endl;
#endif
	int id_num = target_mat.size();

	vector<int> id_data = target_mat;
	vector<double> area_val;
	area_val.resize(id_num, 0.0);
	
	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < id_num ; i++){
		const convint id = id_data[i];
		for(int e = 0 ; e < element_num ; e++){
			const int mat = mesh.element[e].getMat();
			if(mat == id){
				double area = mesh.element[e].calcArea(mesh.node);
				area_val[i] += area;
			}
		}
	}

	result.clear();
	for(int i = 0 ; i < id_num ; i++){
		result[id_data[i]] = area_val[i];
	}
	if(is_print){
		std::fstream fp("Ele_areaMAT.csv", std::ios::out);
		for(int i = 0 ; i < id_num ; i++){
			fp << id_data[i] << ", " << std::scientific << area_val[i] << endl;
		}
		fp.close();
	}
}


/*
//=======================================================
// ● 指定した要素IDの要素の体積を計算する（3D要素のみ）
//=======================================================*/
/** 
 * @brief calc volume of element
*/
void MeshConverter::calc_volume_ele(ConvMesh& mesh, const vector<convint>& target_ele_id, map<convint, double>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "Calc Ele surface area " << endl;
#endif
	int id_num = target_ele_id.size();
	vector<convint> id_data = target_ele_id;

	double *area_val = new double[id_num];
	for(int i = 0 ; i < id_num ; i++){
		convint id = id_data[i];
		double area = mesh.element[id].calcVolume(mesh.node);
		area_val[i] = area;
	}

	result.clear();
	for(int i = 0 ; i < id_num ; i++){
		result[id_data[i]] = area_val[i];
	}
	if(is_print){
		std::fstream fp("Ele_volume.csv", std::ios::out);
		for(int i = 0 ; i < id_num ; i++){
			fp << id_data[i] << ", " << std::scientific << area_val[i] << endl;
		}
		fp.close();
	}
	delete[] area_val;

}
/*
//=======================================================
// ● 指定した材料IDの要素の体積を計算し、その合計を計算する（3D要素のみ） 
//=======================================================*/
/** 
 * @brief calc volume of target ele
*/
void MeshConverter::calc_volume_mat(ConvMesh& mesh, const vector<int>& target_mat_id, map<convint, double>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "Calc Ele surface area MAT" << endl;
#endif
	int id_num = target_mat_id.size();

	vector<int> id_data = target_mat_id;
	double *area_val = new double[id_num];
	for(int i = 0 ; i < id_num ; i++){
		area_val[i] = 0;
	}

	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < id_num ; i++){
		const convint id = id_data[i];
		for(int e = 0 ; e < element_num ; e++){
			const int mat = mesh.element[e].getMat();
			bool is3D = mesh.element[e].is3D();
			if(is3D){
				if(mat == id){
					double area = mesh.element[e].calcVolume(mesh.node);
					area_val[i] += area;
				}
			}
		}
	}
	result.clear();
	for(int i = 0 ; i < id_num ; i++){
		result[id_data[i]] = area_val[i];
	}
	if(is_print){
		std::fstream fp("Ele_volumeMAT.csv", std::ios::out);
		for(int i = 0 ; i < id_num ; i++){
			fp << id_data[i] << ", " << std::scientific << area_val[i] << endl;
		}
		fp.close();
	}
	delete[] area_val;
}


/*
//=======================================================
// ● 指定した材料番号の要素を抜出して、IDをファイルに書き出す（ゼロ始まりで）
//=======================================================*/
/** 
 * @brief output element IDs
*/
void MeshConverter::output_elementID(ConvMesh& mesh, int mat_id, const string& filename, vector<convint>& result_id, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "Write Element ID" << endl;
#endif
	//int mat_id;
	//string filename;

	vector<convint> target_e;
	/* 材料番号が一致する要素を検索 */
	const convint e_size = mesh.element.size();
	for(convint i = 0 ; i < e_size ; i++){
		int mat = mesh.element[i].getMat();
		if(mat == mat_id){
			target_e.push_back(i);
		}
	}

	result_id = target_e;

	/* 抜き出した要素番号を書き出す */
	if(is_print){
		std::fstream fp(filename, std::ios::out);
		const convint target_num = target_e.size();
		fp << target_num << endl;
		for(convint i = 0 ; i < target_num ; i++){
			fp << target_e[i] << endl;
		}
		fp.close();
	}
}

/*
//=======================================================
// ● 指定した範囲のを抜出して、IDをファイルに書き出す（ゼロ始まりで）
//=======================================================*/
/** 
 * @brief output element IDs from pos.
*/
void MeshConverter::output_elementID_POS(ConvMesh& mesh, double min1, double max1, double min2, double max2, double min3, double max3, int sort_dir,bool mode, vector<convint>& result_id, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "Write Element ID within Pos" << endl;
#endif
	//double min1, max1, min2, max2, min3, max3;
	//int sort_dir;

	vector<convint> selected_ele;
	vector<double> sort_val;
	const convint e_size = mesh.element.size();
	for(convint i = 0 ; i < e_size ; i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double pos1, pos2, pos3;
		if(mode){
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

			double vals[3] = {pos1, pos2, pos3};
			if(!mode){
				vals[1] += 180;
			}
			sort_val.push_back(vals[sort_dir]);
		}
	}
	/* IDソード */
	const convint target_num = selected_ele.size();
	for(convint i=0 ; i < target_num-1 ; i++){
		for(convint j=target_num-1 ; j > i ; j--){
			if(sort_val[j-1] > sort_val[j]){
				double temp = sort_val[j];
				sort_val[j] = sort_val[j-1];
				sort_val[j-1] = temp;
				convint temp2 = selected_ele[j];
				selected_ele[j] = selected_ele[j-1];
				selected_ele[j-1] = temp2;
			}
		}
	}

	result_id = selected_ele;

	/* 抜き出した要素番号を書き出す */
	if(is_print){
		std::fstream fp;
		if(mode){
			fp.open("mat_infoPXYZ.csv", std::ios::out);
		} else{
			fp.open("mat_infoPRTZ.csv", std::ios::out);
		}
		fp << "num " << target_num << endl;
		for(int i = 0 ; i < target_num ; i++){
			fp << selected_ele[i] << endl;
		}
		fp.close();
	}
}


/*
//=======================================================
// ● 指定した材料IDの要素の重心を計算し、書き出す
//=======================================================*/
/** 
 * @brief output element gravity points
*/
void MeshConverter::output_ele_grav(ConvMesh& mesh, int target_mat_id, const string& str, map<convint, vector<double>>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "output ele grav" << endl;
#endif
	//int target_mat_id;	
	//string str;

	result.clear();

	std::fstream fp;
	if(is_print){
		fp.open(str, std::ios::out);
	}
	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < element_num ; i++){
		int mat = mesh.element[i].getMat();
		if(mat == target_mat_id){
			double grav[3];
			mesh.element[i].getGrav(grav, mesh.node);
			result[i].resize(3);
			result[i][0] = grav[0];
			result[i][1] = grav[1];
			result[i][2] = grav[2];
			if(is_print){
				fp << i << ", " << std::scientific << grav[0] << ", " << std::scientific << grav[1] << ", " << std::scientific << grav[2] << endl;
			}
		}
	}
	if(is_print){
		fp.close();
	}
}
/*
//=======================================================
// ● 指定した材料IDの要素の重心を計算し、書き出す（総和）
//=======================================================*/
/** 
 * @brief output element gravity of traget mat ele.
*/
void MeshConverter::output_ele_grav_SUM(ConvMesh& mesh, int target_mat_id, const string& str, map<convint, vector<double>>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "output ele grav" << endl;
#endif
	//int target_mat_id;	
	//string str;

	map<int, double> mat_grav_x;
	map<int, double> mat_grav_y;
	map<int, double> mat_grav_z;
	map<int, int> counter_id;

	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < element_num ; i++){
		int mat = mesh.element[i].getMat();
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		mat_grav_x[mat] += grav[0];
		mat_grav_y[mat] += grav[1];
		mat_grav_z[mat] += grav[2];
		counter_id[mat] += 1;
	}

	result.clear();
	std::fstream fp;
	if(is_print){
		fp.open(str, std::ios::out);
	}
	for(auto itr : counter_id){
		int mat = itr.first;
		int ccc = itr.second;
		mat_grav_x[mat] /= (1.0*ccc);
		mat_grav_y[mat] /= (1.0*ccc);
		mat_grav_z[mat] /= (1.0*ccc);
		if(is_print){
			fp << mat << ", " << std::scientific << mat_grav_x[mat] << ", " << std::scientific << mat_grav_y[mat] << ", " << std::scientific << mat_grav_z[mat] << endl;
		}
		result[mat].resize(3);
		result[mat][0] = mat_grav_x[mat];
		result[mat][1] = mat_grav_y[mat];
		result[mat][2] = mat_grav_z[mat];
	}	
	if(is_print){
		fp.close();
	}
}

/*
//=======================================================
// ● 指定した範囲の節点をサーチし、指定した座標値の一覧をファイル出力する(mode true:XYZ) 
//=======================================================*/
/** 
 * @brief output node positions
*/
void MeshConverter::output_node_values(ConvMesh& mesh, double min1, double max1, double min2, double max2, double min3, double max3, int type, bool mode, vector<double>& result, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "output node coordinates" << endl;
#endif
	//double min1,max1, min2,max2, min3, max3;
	//int type;

	vector<double> cood_data;
	const convint node_num = mesh.node.size();
	for(convint i = 0 ; i < node_num ; i++){
		double xx = mesh.node[i].getX();
		double yy = mesh.node[i].getY();
		double zz = mesh.node[i].getZ();
		double rr = sqrt( xx*xx + yy*yy);
		double deg = atan2(yy, xx);
		deg *= 180.0/DEF_CONV_PI;

		bool bl;
		if(mode){
			bl =  (min1 <= xx && xx <= max1);
			bl &= (min2 <= yy && yy <= max2);
			bl &= (min3 <= zz && zz <= max3);
		}else{
			bl =  (min1 <= rr && rr <= max1);
			bl &= (min2 <= deg && deg <= max2);
			bl &= (min3 <= zz && zz <= max3);
		}
		if(bl){
			double target;
			if(type == 0){
				target = (mode ? xx : rr);
			}else if(type == 1){
				target = (mode ? yy : deg);
			}else{
				target = zz;
			}
			/* かぶっていないかチェック */
			const int the_size = cood_data.size();
			if(the_size == 0){
				cood_data.push_back(target);
			}else{
				bool check = false;
				for(int j = 0 ; j < the_size ; j++){
					double diff = fabs(cood_data[j] - target);
					check |= ( diff < 1.0e-9 );
				}
				if(!check){
					cood_data.push_back(target);
				}
			}
		}
	}

	const int the_size2 = cood_data.size();
	if(is_print){
		std::fstream fp("Node_Coordinates.csv", std::ios::out);
		if(mode){
			if(type == 0){
				fp << "[X]" << endl;
			} else if(type == 1){
				fp << "[Y]" << endl;
			} else{
				fp << "[Z]" << endl;
			}
		} else{
			if(type == 0){
				fp << "[R]" << endl;
			} else if(type == 1){
				fp << "[TH]" << endl;
			} else{
				fp << "[Z]" << endl;
			}
		}
		for(int i = 0 ; i < the_size2 ; i++){
			fp << std::scientific << cood_data[i] << endl;
		}
		fp.close();
	}
	result = cood_data;
}


/*
//=======================================================
// ● 指定した材料の要素をサーチし、その回転対称位置にある要素とのペアを調べ、書き出す
//=======================================================*/
/** 
 * @brief detect and output node relation of symetric position
*/
void MeshConverter::output_rot_search(ConvMesh& mesh, int rot_num, double rot_deg, double target_deg1, double target_deg2, int target_mat, int rot_sym_mat, map<convint, vector<convint>>& result_ids, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "output rot search" << endl;
#endif
	/*int rot_num;
	double rot_deg;
	double target_deg1, target_deg2;
	int target_mat, rot_sym_mat;*/

	vector<convint> target_eles;
	vector<convint> rot_sym_eles;

	/* 元要素を選択 */
	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < element_num ; i++){
		int mat = mesh.element[i].getMat();
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double g_rad = atan2(grav[1], grav[0]);
		const double g_deg = 180.0*g_rad / DEF_CONV_PI;
		bool in_range = (target_deg1 <= g_deg && g_deg <= target_deg2);
		/* 対称元 */
		if(target_mat == -1 && in_range){
			target_eles.push_back(i);
		}else if(mat == target_mat && in_range){
			target_eles.push_back(i);
		}
		/* 対称先 */
		if(rot_sym_mat == -1 && !in_range){
			rot_sym_eles.push_back(i);
		}else if(mat == rot_sym_mat && !in_range){
			rot_sym_eles.push_back(i);
		}
	}

	/**/
	const convint element_num_target = target_eles.size();
	const convint element_num_rot_sym = rot_sym_eles.size();
	map<convint, convint> *rot_sym_results = new map<convint, convint>[rot_num];

	for(convint i = 0 ; i < element_num_target ; i++){
		const convint e_ID = target_eles[i];
		double grav1[3];
		mesh.element[e_ID].getGrav(grav1, mesh.node);
		for(int aaa=0; aaa < rot_num ; aaa++){
			const double rot_rad = rot_deg*(1.0+aaa) * DEF_CONV_PI / 180.0;
			/* 回転 */
			double rotMAT[3][3];
			rotMAT[0][0] = cos(rot_rad);        rotMAT[0][1] = -1.0*sin(rot_rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rot_rad);        rotMAT[1][1] = cos(rot_rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
			double newX = rotMAT[0][0]*grav1[0] + rotMAT[0][1]*grav1[1] + rotMAT[0][2]*grav1[2];
			double newY = rotMAT[1][0]*grav1[0] + rotMAT[1][1]*grav1[1] + rotMAT[1][2]*grav1[2];
			double newZ = rotMAT[2][0]*grav1[0] + rotMAT[2][1]*grav1[1] + rotMAT[2][2]*grav1[2];
			/**/
			for(int j = 0 ; j < element_num_rot_sym ; j++){
				const convint e_ID2 = rot_sym_eles[j];
				double grav2[3];
				mesh.element[e_ID2].getGrav(grav2, mesh.node);
				double diff_X = newX-grav2[0];
				double diff_Y = newY-grav2[1];
				double diff_Z = newZ-grav2[2];
				double diff = sqrt(diff_X*diff_X + diff_Y*diff_Y + diff_Z*diff_Z);
				/* 一致したら確定する */
				if(diff < eps_diff){
					rot_sym_results[aaa][e_ID] = e_ID2;
					break;
				}
			}
		}
	}
	/* 結果書き出し */
	result_ids.clear();
	std::fstream fp;
	if(is_print){
		fp.open("Rot_sym.csv", std::ios::out);
	}
	for(convint i = 0 ; i < element_num_target ; i++){
		/* 対称要素の数を確認 */
		const convint e_ID = target_eles[i];
		vector<convint> rot_syms;
		for(int aaa=0; aaa < rot_num ; aaa++){
			int count = rot_sym_results[aaa].count(e_ID);
			if(count != 0){
				rot_syms.push_back(rot_sym_results[aaa][e_ID]);
			}
		}
		result_ids[e_ID].clear();
		/* 書き出す */
		const convint the_size = rot_syms.size();
		if(the_size > 0){
			if(is_print){
				fp << e_ID << ", ";
				for(int aaa=0; aaa < the_size-1 ; aaa++){
					fp << rot_syms[aaa] << ", ";
				}
				fp << rot_syms[the_size-1] << endl;
			}

			result_ids[e_ID].resize(the_size);
			for(int aaa=0; aaa < the_size ; aaa++){
				result_ids[e_ID][aaa] = rot_syms[aaa];
			}
		}
	}
	if(is_print){
		fp.close();
	}
	delete[] rot_sym_results;
}


/*
//=======================================================
// ● 指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べ、書き出す
//=======================================================*/
/** 
 * @brief delect and output node relation (mirror)
*/
void MeshConverter::output_mirror_search(ConvMesh& mesh, int axis, int target_mat, int mirror_mat, map<convint, convint>& result_ids, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "output mirror search" << endl;
#endif
	//int axis;
	//int target_mat, mirror_mat;	

	vector<convint> target_eles;
	vector<convint> mirror_eles;

	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < element_num ; i++){
		int mat = mesh.element[i].getMat();
		if(target_mat == -1){
			target_eles.push_back(i);
		}else if(mat == target_mat){
			target_eles.push_back(i);
		}
		if(mirror_mat == -1){
			mirror_eles.push_back(i);
		}else if(mat == mirror_mat){
			mirror_eles.push_back(i);
		}
	}
	const convint element_num_target = target_eles.size();
	const convint element_num_mirror = mirror_eles.size();
	map<convint, convint> mirror_results;
	for(convint i = 0 ; i < element_num_target ; i++){
		const convint e_ID = target_eles[i];
		double grav1[3];
		mesh.element[e_ID].getGrav(grav1, mesh.node);
		for(int j = 0 ; j < element_num_mirror ; j++){
			const convint e_ID2 = mirror_eles[j];
			double grav2[3];
			mesh.element[e_ID2].getGrav(grav2, mesh.node);
			if(axis == 0){
				grav2[1] *= -1.0;
			}else if(axis == 1){
				grav2[0] *= -1.0;
			}else{
				grav2[2] *= -1.0;
			}
			double diff_X = grav1[0]-grav2[0];
			double diff_Y = grav1[1]-grav2[1];
			double diff_Z = grav1[2]-grav2[2];
			double diff = sqrt(diff_X*diff_X + diff_Y*diff_Y + diff_Z*diff_Z);
			/* 一致したら確定する */
			if(diff < eps_diff){
				mirror_results[e_ID] = e_ID2;
				break;
			}
		}
	}

	/* 結果書き出し */
	result_ids.clear();
	std::fstream fp;
	if(is_print){
		fp.open("Mirror_sym.csv", std::ios::out);
	}
	for(auto itr : mirror_results){
		const convint ori = itr.first;
		const convint mir = itr.second;
		if(is_print){
			fp << ori << ", " << mir << endl;
		}
		result_ids[ori] = mir;
	}
	if(is_print){
		fp.close();
	}
}


/*
//=======================================================
// ● 指定した材料の要素をサーチし、その鏡面対称・回転対称の位置にある要素とのペアを調べ、書き出す
//=======================================================*/
/** 
 * @brief delect and output node relation (mirror & rot sym.)
*/
void MeshConverter::output_mirror_rot_search(ConvMesh& mesh, int axis, double start_deg, double end_deg, int target_mat, int mirror_mat, int rot_num, double rot_deg, map<convint, vector<convint>>& result_ids, bool is_print){
#ifdef PRINT_CONV_INFO
	cout << "output mirror and rot search" << endl;
#endif
	/*int axis;
	double start_deg, end_deg;
	int target_mat, mirror_mat;	
	int rot_num;
	double rot_deg;*/

	vector<convint> target_eles;
	vector<convint> mirror_eles;
	vector<convint> rot_targets;
	vector<convint> rot_targets_mirror;

	const convint element_num = mesh.element.size();
	for(convint i = 0 ; i < element_num ; i++){
		int mat = mesh.element[i].getMat();
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double g_deg = atan2(grav[1], grav[0]);
		g_deg *= 180.0/DEF_CONV_PI;
		if(start_deg <= g_deg && g_deg <= end_deg){
			if(target_mat == -1){
				target_eles.push_back(i);
				rot_targets.push_back(i);
			}else if(mat == target_mat){
				target_eles.push_back(i);
				rot_targets.push_back(i);
			}
		}else{
			if(mirror_mat == -1){
				mirror_eles.push_back(i);
			}else if(mat == mirror_mat){
				mirror_eles.push_back(i);
			}
		}
	}
	const convint element_num_target = target_eles.size();
	const convint element_num_mirror = mirror_eles.size();

	map<convint, vector<convint>> mirror_results;
	map<convint, convint> mirror_results_rev;
	for(convint i = 0 ; i < element_num_target ; i++){
		const convint e_ID = target_eles[i];
		double grav1[3];
		mesh.element[e_ID].getGrav(grav1, mesh.node);
		for(int j = 0 ; j < element_num_mirror ; j++){
			const convint e_ID2 = mirror_eles[j];
			double grav2[3];
			mesh.element[e_ID2].getGrav(grav2, mesh.node);
			if(axis == 0){
				grav2[1] *= -1.0;
			}else if(axis == 1){
				grav2[0] *= -1.0;
			}else{
				grav2[2] *= -1.0;
			}
			double diff_X = grav1[0]-grav2[0];
			double diff_Y = grav1[1]-grav2[1];
			double diff_Z = grav1[2]-grav2[2];
			double diff = sqrt(diff_X*diff_X + diff_Y*diff_Y + diff_Z*diff_Z);
			/* 一致したら確定する */
			if(diff < eps_diff){
				int ccc = mirror_results.count(e_ID);
				if(ccc == 0){
					mirror_results[e_ID].clear();
				}
				mirror_results[e_ID].push_back(e_ID2);
				rot_targets_mirror.push_back(e_ID2);
				mirror_results_rev[e_ID2] = e_ID;
				break;
			}
		}
	}

	/* 元要素とその鏡面に含まれていない要素をセット */
	vector<convint> other_eles;
	for(int i = 0 ; i < element_num ; i++){
		int ccc1 = mirror_results.count(i);
		int ccc2 = mirror_results_rev.count(i);
		if(ccc1 == 0 && ccc2 == 0){
			other_eles.push_back(i);
		}
	}
	const convint target_rot_size = rot_targets.size();
	const convint target_rot_size2 = rot_targets_mirror.size();
	const convint others_size = other_eles.size();

	/**/
	/* 次に回転探し */
	for(int rr = 0 ; rr < rot_num ; rr++){
		double rot_rad = (rr+1)*rot_deg * DEF_CONV_PI / 180.0;
		/* 元要素を探索 */
		for(int i = 0 ; i < target_rot_size ; i++){
			const convint e_ID = rot_targets[i];
			double grav[3];
			mesh.element[e_ID].getGrav(grav, mesh.node);
			/* 回転 */
			double rotMAT[3][3];
			rotMAT[0][0] = cos(rot_rad);        rotMAT[0][1] = -1.0*sin(rot_rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rot_rad);        rotMAT[1][1] = cos(rot_rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
			double newX = rotMAT[0][0]*grav[0] + rotMAT[0][1]*grav[1] + rotMAT[0][2]*grav[2];
			double newY = rotMAT[1][0]*grav[0] + rotMAT[1][1]*grav[1] + rotMAT[1][2]*grav[2];
			double newZ = rotMAT[2][0]*grav[0] + rotMAT[2][1]*grav[1] + rotMAT[2][2]*grav[2];
			/**/
			/* 元＆鏡面以外の要素と比較 */
			for(int kk = 0 ; kk < others_size ; kk++){
				const convint e_ID2 = other_eles[kk];
				double grav2[3];
				mesh.element[e_ID2].getGrav(grav2, mesh.node);
				double diff_X = newX-grav2[0];
				double diff_Y = newY-grav2[1];
				double diff_Z = newZ-grav2[2];
				double diff = sqrt(diff_X*diff_X + diff_Y*diff_Y + diff_Z*diff_Z);
				/* 一致したら確定する */
				if(diff < eps_diff){
					mirror_results[e_ID].push_back(e_ID2);
				}
			}
		}
		/* 元の鏡面位置も探索 */
		for(int i = 0 ; i < target_rot_size2 ; i++){
			const convint e_ID_rev = rot_targets_mirror[i];
			double grav[3];
			mesh.element[e_ID_rev].getGrav(grav, mesh.node);
			/* 回転 */
			double rotMAT[3][3];
			rotMAT[0][0] = cos(rot_rad);        rotMAT[0][1] = -1.0*sin(rot_rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rot_rad);        rotMAT[1][1] = cos(rot_rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
			double newX = rotMAT[0][0]*grav[0] + rotMAT[0][1]*grav[1] + rotMAT[0][2]*grav[2];
			double newY = rotMAT[1][0]*grav[0] + rotMAT[1][1]*grav[1] + rotMAT[1][2]*grav[2];
			double newZ = rotMAT[2][0]*grav[0] + rotMAT[2][1]*grav[1] + rotMAT[2][2]*grav[2];
			/**/
			/* 元＆鏡面以外の要素と比較 */
			for(int kk = 0 ; kk < others_size ; kk++){
				const convint e_ID2 = other_eles[kk];
				double grav2[3];
				mesh.element[e_ID2].getGrav(grav2, mesh.node);
				double diff_X = newX-grav2[0];
				double diff_Y = newY-grav2[1];
				double diff_Z = newZ-grav2[2];
				double diff = sqrt(diff_X*diff_X + diff_Y*diff_Y + diff_Z*diff_Z);
				/* 一致したら確定する */
				if(diff < eps_diff){
					convint ori_ID = mirror_results_rev[e_ID_rev];
					mirror_results[ori_ID].push_back(e_ID2);
				}
			}
		}
	}
	/**/
	/* 結果書き出し */
	result_ids.clear();
	std::fstream fp;
	if(is_print){
		fp.open("Mirror_rot_sym.csv", std::ios::out);
	}
	for(auto itr : mirror_results){
		const convint ori = itr.first;
		const convint size = itr.second.size();
		if(is_print){
			fp << ori << ", ";
			for(int i = 0 ; i < size ; i++){
				fp << itr.second[i] << ", " ;
			}
			fp << endl;
		}
		result_ids[ori].resize(size);
		for(int i = 0 ; i < size ; i++){
			result_ids[ori][i] = itr.second[i];
		}

	}
	if(is_print){
		fp.close();
	}
}





/**/
};
};


