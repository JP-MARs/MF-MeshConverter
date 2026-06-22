/**
* @file mesh_conv_read_mat_change.cpp
* @brief mesh_conv_read_mat_change (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{

/*
//===============================================================
// ○ 材料ID変更・材料IDの単独指定
//===============================================================*/
void mat_change_singleID(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int old_mat = modify["old"].get<int>();
	int new_mat = modify["new"].get<int>();
	converter.change_matID(mesh, old_mat, new_mat);
}
/*
//===============================================================
// ○ 材料ID変更・材料IDの範囲指定
//===============================================================*/
void mat_change_interval(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int new_mat = modify["new"].get<int>();
	int start = modify["mat"][0].get<int>();
	int end = modify["mat"][1].get<int>();
	converter.change_matID2(mesh, 0, new_mat, start, end, -1, 1, -1, 1, -1, 1);
}
/*
//===============================================================
// ○ 材料ID変更・xyzで範囲指定
//===============================================================*/
void mat_change_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int old_mat = modify["old"].get<int>();
	int new_mat = modify["new"].get<int>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.change_matID_xyz(mesh, old_mat, new_mat, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//===============================================================
// ○ 材料ID変更・rtzで範囲指定
//===============================================================*/
void mat_change_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int old_mat = modify["old"].get<int>();
	int new_mat = modify["new"].get<int>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.change_matID_rtz(mesh, old_mat, new_mat, rmin, rmax, tmin, tmax, zmin, zmax);
}
/*
//===============================================================
// ○ 材料ID変更・材料ID範囲指定・xyzで範囲指定
//===============================================================*/
void mat_change_xyz2(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int new_mat = modify["new"].get<int>();
	int start = modify["mat"][0].get<int>();
	int end = modify["mat"][1].get<int>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.change_matID2_xyz(mesh, new_mat, start, end, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//===============================================================
// ○ 材料ID変更・材料ID範囲指定・rtzで範囲指定
//===============================================================*/
void mat_change_rtz2(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int new_mat = modify["new"].get<int>();
	int start = modify["mat"][0].get<int>();
	int end = modify["mat"][1].get<int>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.change_matID2_rtz(mesh, new_mat, start, end, rmin, rmax, tmin, tmax, zmin, zmax);
}
/*
//===============================================================
// ○ 材料ID変更・材料ID加算・xyzで範囲指定
//===============================================================*/
void mat_change_plus_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int plus = modify["plus"].get<int>();
	int start = modify["mat"][0].get<int>();
	int end = modify["mat"][1].get<int>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.change_plus_matID_xyz(mesh, plus, start, end, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//===============================================================
// ○ 材料ID変更・材料ID加算・rtzで範囲指定
//===============================================================*/
void mat_change_plus_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int plus = modify["plus"].get<int>();
	int start = modify["mat"][0].get<int>();
	int end = modify["mat"][1].get<int>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.change_plus_matID_rtz(mesh, plus, start, end, rmin, rmax, tmin, tmax, zmin, zmax);
}
/*
//===============================================================
// ○  材料ID変更・要素IDで指定
//===============================================================*/
void mat_change_ele(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int new_mat = modify["new"].get<int>();	
	convint start = modify["ele_id"][0].get<convint>();
	convint end = modify["ele_id"][1].get<convint>();
	/* ID補正 */
	if(READ_ID_MODE){
		start--;
		end--;
	}
	converter.change_matID_eid(mesh, new_mat, start, end);
}

/*
//===============================================================
// ○  材料ID変更・指定した材料IDの要素について、材料IDを自身の要素ID+INCに変更する
//===============================================================*/
void mat_change_ele_id_to_mat(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int inc = modify["inc"].get<int>();
	int num = modify["target_num"].get<int>();
	int *target_matid = new int[num];
	for(int i = 0 ; i < num ; i++){
		target_matid[i] = modify["mat"][i].get<int>();
	}
	converter.change_mat_toEID(mesh, inc, num, target_matid);
	delete[] target_matid;
}
/*
//===============================================================
// ○  材料ID変更・隣接一括
//===============================================================*/
void mat_change_neighbour_eles(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num_start = modify["num_start"].get<int>();
	convint* start_ele = new convint[num_start];
	for(int i = 0 ; i < num_start ; i++){
		start_ele[i] = modify["ele_id"][i].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			start_ele[i]--;
		}
	}
	int num_mat = modify["num"].get<int>();
	int* mat_ids = new int[num_mat];
	for(int i = 0 ; i < num_mat ; i++){
		mat_ids[i] = modify["mat_id"][i].get<int>();
	}
	int new_mat = modify["new_mat"].get<int>();
	int diff = modify["diff_id"].get<int>();
	converter.change_neighbour_eles(mesh, num_start, start_ele, num_mat, mat_ids, new_mat, diff);
	delete[] start_ele;
	delete[] mat_ids;
}



/* end of namespace*/
};
};
