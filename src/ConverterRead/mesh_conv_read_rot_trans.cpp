/**
* @file mesh_conv_read_rot_trans.cpp
* @brief mesh_conv_read_rot_trans (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{

/*
//===============================================================
// ○ 回転移動
//===============================================================*/
void mesh_rot_move_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double rot_deg = modify["deg"].get<double>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.Rotation_node(mesh, axis, rot_deg, xmin, xmax, ymin, ymax, zmin, zmax, true);
}

/*
//===============================================================
// ○ 回転移動・rtz指定
//===============================================================*/
void mesh_rot_move_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double rot_deg = modify["deg"].get<double>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.Rotation_node(mesh, axis, rot_deg, rmin, rmax, tmin, tmax, zmin, zmax, false);
}

/*
//===============================================================
// ○ 鏡面移動
//===============================================================*/
void mesh_mirror_move_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.MIRROR_move_node(mesh, axis, xmin, xmax, ymin, ymax, zmin, zmax, true);
}

/*
//===============================================================
// ○ 鏡面移動・rtz指定
//===============================================================*/
void mesh_mirror_move_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.MIRROR_move_node(mesh, axis, rmin, rmax, tmin, tmax, zmin, zmax, true);
}

/*
//===============================================================
// ○ 回転コピー
//===============================================================*/
void mesh_rot_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double deg = modify["deg"].get<double>();
	int cp_times = modify["times"].get<int>();
	int node_del = modify["node_del"].get<int>();
	int map_make = modify["map_make"].get<int>();
	int inc = modify["inc"].get<int>();
	int plus = modify["plus"].get<int>();
	converter.RotCopy(mesh, axis, deg, cp_times, node_del, map_make, inc, plus);
}

/*
//===============================================================
// ○ 鏡面コピー
//===============================================================*/
void mesh_mirror_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	int dir = modify["dir"].get<int>();
	int cp_times = modify["times"].get<int>();
	int node_del = modify["node_del"].get<int>();
	int map_make = modify["map_make"].get<int>();
	converter.MirrorCopy(mesh, axis, dir, node_del, map_make);
}


/*
//===============================================================
// ○ 軸指定鏡面コピー
//===============================================================*/
void mesh_rotmirror_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double deg = modify["deg"].get<double>();
	int node_del = modify["node_del"].get<int>();
	int map_make = modify["map_make"].get<int>();
	converter.MirrorCopyRot(mesh, axis, deg, node_del, map_make);
}


/*
//===============================================================
// ○ 軸指定鏡面コピー+回転コピー
//===============================================================*/
void mesh_mirror_rot_multi_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int axis = modify["axis"].get<int>();
	double deg = modify["deg"].get<double>();
	int rot_times = modify["times"].get<int>();
	int node_del = modify["node_del"].get<int>();
	int map_make = modify["map_make"].get<int>();
	converter.MirrorCopyRot_MULTI(mesh, axis, deg, node_del, rot_times, map_make);
}


/*
//===============================================================
// ○ 平行移動コピー
//===============================================================*/
void mesh_move_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int dir = modify["dri"].get<int>();
	double move_val = modify["move"].get<double>();
	int cp_times = modify["times"].get<int>();
	int node_del = modify["node_del"].get<int>();
	int map_make = modify["map_make"].get<int>();
	int inc = modify["inc"].get<int>();
	int plus = modify["plus"].get<int>();
	converter.MoveCopy(mesh, dir, move_val, cp_times, node_del, map_make, inc, plus);
}



/* end of namespace*/
};
};
