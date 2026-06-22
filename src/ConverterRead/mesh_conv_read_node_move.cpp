/**
* @file mesh_conv_read_node_move.cpp
* @brief mesh_conv_read_node_move (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{

/*
//===============================================================
// ○ 節点移動（xyzで範囲指定）dir: (0:X, 1:Y, 2:Z,  4:Xの移動位置指定,  5:Yの移動位置指定,  6:Zの移動位置指定)
//===============================================================*/
void node_move_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	double move_val = modify["value"].get<double>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_xyzdir(mesh, dir, move_val, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//===============================================================
// ○ 節点移動（rtzで範囲指定）dir: (0:T, 1:theta, 2:Z,  4:Rの移動位置指定,  5:Thetaの移動位置指定,  6:Zの移動位置指定)
//===============================================================*/
void node_move_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	double move_val = modify["value"].get<double>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_rtzdir(mesh, dir, move_val, rmin, rmax, tmin, tmax, zmin, zmax);
}
/*
//===============================================================
// ○ 節点移動（指定したIDの節点を平行移動）dir: 0=xyz, 1=rtz
//===============================================================*/
void node_move_id(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	int num =  modify["num"].get<int>();

	convint* target_ids = new convint[num];
	double* move_1 = new double[num];
	double* move_2 = new double[num];
	double* move_3 = new double[num];
	for(int i = 0 ; i < num ; i++){
		target_ids[i] = modify["data"][i][0].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			target_ids[i]--;
		}
		move_1[i] = modify["data"][i][1].get<double>();
		move_2[i] = modify["data"][i][2].get<double>();
		move_3[i] = modify["data"][i][3].get<double>();
	}
	converter.move_node_id_xyz(mesh, dir, num, target_ids, move_1, move_2, move_3);

	delete[] target_ids;
	delete[] move_1;
	delete[] move_2;
	delete[] move_3;
}
/*
//===============================================================
// ○ 節点移動（範囲を指定して線形補間、xyzで範囲指定）dir: (0:X, 1:Y, 2:Z
//===============================================================*/
void node_move2_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	double min_move = modify["move"][0].get<double>();
	double max_move = modify["move"][1].get<double>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_xyz_type2(mesh, dir, min_move, max_move, xmin, xmax, ymin, ymax, zmin, zmax);
}
/*
//===============================================================
// ○ 節点移動（範囲を指定して線形補間、rtzで範囲指定）dir: (0:R, 1:Th, 2:Z
//===============================================================*/
void node_move2_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	double min_move = modify["move"][0].get<double>();
	double max_move = modify["move"][1].get<double>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_rtz_type2(mesh, dir, min_move, max_move, rmin, rmax, tmin, tmax, zmin, zmax);
}

/*
//===============================================================
// ○ 節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）dir:  (0:X, 1:Y, 2:Z
//===============================================================*/
void node_move3_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	int axis = modify["axis"].get<int>();
	double min_v = modify["axis_range"][0].get<double>();
	double max_v = modify["axis_range"][1].get<double>();
	double min_move = modify["move"][0].get<double>();
	double max_move = modify["move"][1].get<double>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_xyz_type3(mesh, dir, axis, min_move, max_move, min_v, max_v, xmin, xmax, ymin, ymax, zmin, zmax);
}

/*
//===============================================================
// ○ 節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、rtzで範囲指定）dir: (0:R, 1:Th, 2:Z
//===============================================================*/
void node_move3_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	int axis = modify["axis"].get<int>();
	double min_v = modify["axis_range"][0].get<double>();
	double max_v = modify["axis_range"][1].get<double>();
	double min_move = modify["move"][0].get<double>();
	double max_move = modify["move"][1].get<double>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_rtz_type3(mesh, dir, axis, min_move, max_move, min_v, max_v, rmin, rmax, tmin, tmax, zmin, zmax);
}
/*
//===============================================================
// ○ 節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）
//===============================================================*/
void node_move3_mat_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	int axis = modify["axis"].get<int>();
	int mat1 = modify["mat"][0].get<int>();
	int mat2 = modify["mat"][1].get<int>();
	double min_v = modify["axis_range"][0].get<double>();
	double max_v = modify["axis_range"][1].get<double>();
	double min_move = modify["move"][0].get<double>();
	double max_move = modify["move"][1].get<double>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_xyz_type3_mat(mesh, dir, mat1, mat2, axis, min_move, max_move, min_v, max_v, xmin, xmax, ymin, ymax, zmin, zmax);
}

/*
//===============================================================
// ○ 節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、rtzで範囲指定）
//===============================================================*/
void node_move3_mat_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir){
	int axis = modify["axis"].get<int>();
	int mat1 = modify["mat"][0].get<int>();
	int mat2 = modify["mat"][1].get<int>();
	double min_v = modify["axis_range"][0].get<double>();
	double max_v = modify["axis_range"][1].get<double>();
	double min_move = modify["move"][0].get<double>();
	double max_move = modify["move"][1].get<double>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.move_node_rtz_type3_mat(mesh, dir, mat1, mat2, axis, min_move, max_move, min_v, max_v, rmin, rmax, tmin, tmax, zmin, zmax);
}

/* end of namespace*/
};
};
