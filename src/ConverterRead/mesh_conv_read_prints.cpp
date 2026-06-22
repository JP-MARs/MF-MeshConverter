/**
* @file mesh_conv_read_prints.cpp
* @brief mesh_conv_read_prints (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ 要素面積書き出し
//===============================================================*/
void mesh_ele_area_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num = modify["num"].get<int>();
	vector<convint> ele_ids;
	for(int i = 0 ; i < num ; i++){
		convint id = modify["ele"][i].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			id--;
		}
		ele_ids.push_back(id);
	}
	map<convint, double> result;
	converter.calc_area_ele(mesh, ele_ids, result, true);
}

/*
//===============================================================
// ○ 要素面積書き出し・材料IDごと
//===============================================================*/
void mesh_ele_area_mat_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num = modify["num"].get<int>();
	vector<int> target_mat;
	for(int i = 0 ; i < num ; i++){
		int id = modify["mat_id"][i].get<int>();
		target_mat.push_back(id);
	}
	map<convint, double> result;
	converter.calc_area_mat(mesh, target_mat, result, true);
}

/*
//===============================================================
// ○ 材料番号の要素を抜出して、IDをファイルに書き出す
//===============================================================*/
void mesh_eleids_in_mat_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int mat_id = modify["mat_id"].get<int>();
	string filename = modify["filename"].get<string>();
	vector<convint> result_id;
	converter.output_elementID(mesh, mat_id, filename, result_id, true);
}

/*
//===============================================================
// ○ 指定した範囲の要素IDを抜出して、IDをファイルに書き出す
//===============================================================*/
void mesh_ele_output_range_xyz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	int dir = modify["dir"].get<int>();
	vector<convint> result_id;
	converter.output_elementID_POS(mesh, xmin, xmax, ymin, ymax, zmin, zmax, dir, true, result_id, true);
}
/*
//===============================================================
// ○ 指定した範囲の要素IDを抜出して、IDをファイルに書き出す
//===============================================================*/
void mesh_ele_output_range_rtz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	int dir = modify["dir"].get<int>();
	vector<convint> result_id;
	converter.output_elementID_POS(mesh, rmin, rmax, tmin, tmax, zmin, zmax, dir, false, result_id, true);
}

/*
//===============================================================
// ○ 指定した範囲の節点をサーチし、指定した座標値をDをファイルに書き出す / xyz
//===============================================================*/
void mesh_node_output_range_xyz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	int type = modify["type"].get<int>();
	vector<double> result_id;
	converter.output_node_values(mesh, xmin, xmax, ymin, ymax, zmin, zmax, type, true, result_id, true);
}

/*
//===============================================================
// ○ 指定した範囲の節点をサーチし、指定した座標値をDをファイルに書き出す / rtz
//===============================================================*/
void mesh_node_output_range_rtz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	int type = modify["type"].get<int>();
	vector<double> result_id;
	converter.output_node_values(mesh, rmin, rmax, tmin, tmax, zmin, zmax, type, true, result_id, true);
}


/*
//===============================================================
// ○ 要素重心をファイルに書き出す
//===============================================================*/
void mesh_ele_grav_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int mat_id = modify["mat"].get<int>();
	string filename = modify["filename"].get<string>();
	map<convint, vector<double>> result;
	converter.output_ele_grav(mesh, mat_id, filename, result, true);
}


/*
//===============================================================
// ○ 材料IDごとの要素たちの重心を書き出す
//===============================================================*/
void mesh_ele_grav_sum_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int mat_id = modify["mat"].get<int>();
	string filename = modify["filename"].get<string>();
	map<convint, vector<double>> result;
	converter.output_ele_grav_SUM(mesh, mat_id, filename, result, true);
}


/*
//===============================================================
// ○ 要素体積書き出し
//===============================================================*/
void mesh_ele_volume_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num = modify["num"].get<int>();
	vector<convint> ele_ids;
	for(int i = 0 ; i < num ; i++){
		convint id = modify["ele"][i].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			id--;
		}
		ele_ids.push_back(id);
	}
	map<convint, double> result;
	converter.calc_volume_ele(mesh, ele_ids, result, true);
}

/*
//===============================================================
// ○ 要素体積書き出し・材料IDごと
//===============================================================*/
void mesh_ele_volume_mat_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num = modify["num"].get<int>();
	vector<int> target_mat;
	for(int i = 0 ; i < num ; i++){
		int id = modify["mat_id"][i].get<int>();
		target_mat.push_back(id);
	}
	map<convint, double> result;
	converter.calc_volume_mat(mesh, target_mat, result, true);
}

/*
//===============================================================
// ○ 指定した材料の要素をサーチし、その回転対称位置にある要素とのペアを調べる
//===============================================================*/
void mesh_rot_search_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int times = modify["times"].get<int>();
	double deg = modify["deg"].get<double>();
	double range_deg1 = modify["range"][0].get<double>();
	double range_deg2 = modify["range"][1].get<double>();
	int target_mat = modify["mat"].get<int>();
	int rot = modify["rot"].get<int>();
	map<convint, vector<convint>> result_ids;
	converter.output_rot_search(mesh, times, deg, range_deg1, range_deg2, target_mat, rot, result_ids, true);
}

/*
//===============================================================
// ○ 指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べる
//===============================================================*/
void mesh_mirror_search_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	int target_mat = modify["mat"].get<int>();
	int mirror = modify["mirror"].get<int>();
	map<convint, convint> result_ids;
	converter.output_mirror_search(mesh, axis, target_mat, mirror, result_ids, true);
}

/*
//===============================================================
// ○ 指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べる
//===============================================================*/
void mesh_rot_and_mirror_search_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double range_deg1 = modify["range"][0].get<double>();
	double range_deg2 = modify["range"][1].get<double>();
	int target_mat = modify["mat"].get<int>();
	int mirror = modify["mirror"].get<int>();
	int rot_num = modify["rot_num"].get<int>();
	double deg = modify["def"].get<int>();
	map<convint, vector<convint>> result_ids;
	converter.output_mirror_rot_search(mesh, axis, range_deg1, range_deg2, target_mat, mirror, rot_num, deg, result_ids, true);
}




/* end of namespace*/
};
};
