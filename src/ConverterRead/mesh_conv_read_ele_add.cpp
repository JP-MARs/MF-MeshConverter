/**
* @file mesh_conv_read_ele_add.cpp
* @brief mesh_conv_read_ele_add (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{

/*
//===============================================================
// ○ 線要素追加・座標指定
//===============================================================*/
void line_add(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double start_pos[3];
	double end_pos[3];
	start_pos[0] = modify["start_pos"][0].get<double>();
	start_pos[1] = modify["start_pos"][1].get<double>();
	start_pos[2] = modify["start_pos"][2].get<double>();
	end_pos[0] = modify["end_pos"][0].get<double>();
	end_pos[1] = modify["end_pos"][1].get<double>();
	end_pos[2] = modify["end_pos"][2].get<double>();
	int mat = modify["mat"].get<int>();
	converter.line_add(mesh, start_pos, end_pos, mat);
}

/*
//===============================================================
// ○ 線要素追加・座標指定・途中全部作成・複数指定
//===============================================================*/
void line_add_multi(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double start_pos[3];
	double end_pos[3];
	start_pos[0] = modify["start_pos"][0].get<double>();
	start_pos[1] = modify["start_pos"][1].get<double>();
	start_pos[2] = modify["start_pos"][2].get<double>();
	end_pos[0] = modify["end_pos"][0].get<double>();
	end_pos[1] = modify["end_pos"][1].get<double>();
	end_pos[2] = modify["end_pos"][2].get<double>();
	int mat = modify["mat"].get<int>();
	converter.line_add_M_pos(mesh, start_pos, end_pos, mat);
}

/*
//===============================================================
// ○ 線要素追加・元になる節点指定 
//===============================================================*/
void line_add_node(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	convint id1 = modify["nid"][0].get<convint>();
	convint id2 = modify["nid"][1].get<convint>();
	/* ID補正 */
	if(READ_ID_MODE){
		id1--;
		id2--;
	}
	int mat = modify["mat"].get<int>();
	converter.line_add2(mesh, id1, id2, mat);
}

/*
//===============================================================
// ○ 線要素追加・元になる節点指定・途中全部作成 
//===============================================================*/
void line_add_node2(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	convint id1 = modify["nid"][0].get<convint>();
	convint id2 = modify["nid"][1].get<convint>();
	/* ID補正 */
	if(READ_ID_MODE){
		id1--;
		id2--;
	}
	int mat = modify["mat"].get<int>();
	vector<convint> id1s{id1};
	vector<convint> id2s{id2};
	converter.line_add_M(mesh, id1s, id2s, mat);
}

/*
//===============================================================
// ○ 線要素追加・元になる節点指定・途中全部作成・複数指定
//===============================================================*/
void line_add_node2_multi(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num = modify["num"].get<int>();
	vector<convint> id1s;
	vector<convint> id2s;
	for(int i = 0 ; i < num ; i++){
		convint s_id = modify["nid"][i][0].get<convint>();
		convint e_id = modify["nid"][i][1].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			s_id--;
			e_id--;
		}
		id1s.push_back(s_id);
		id2s.push_back(e_id);
	}
	int mat = modify["mat"].get<int>();
	converter.line_add_M(mesh, id1s, id2s, mat);
}

/*
//===============================================================
// ○ 線要素追加・円弧
//===============================================================*/
void line_add_R(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double r = modify["rdata"][0].get<double>();
	double th1 = modify["rdata"][1].get<double>();
	double th2 = modify["rdata"][2].get<double>();
	double z = modify["rdata"][3].get<double>();
	double eps = modify["eps"].get<double>();
	int mat = modify["mat"].get<int>();
	converter.line_add_R(mesh, r, th1, th2, z, eps, mat);
}

/*
//===============================================================
// ○ 節点作成
//===============================================================*/
void node_make(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double pos[3];
	pos[0] = modify["pos"][0].get<double>();
	pos[1] = modify["pos"][1].get<double>();
	pos[2] = modify["pos"][2].get<double>();
	converter.make_node(mesh, pos);
}

/*
//===============================================================
// ○ 六面体作成
//===============================================================*/
void make_hex_ele(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	convint ids[8];
	for(int i = 0 ; i < 8 ; i++){
		ids[i] = modify["ids"][i].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			ids[i]--;
		}
	}
	int mat = modify["mat"].get<int>();
	converter.make_hex_ele(mesh, ids, mat);
}

/*
//===============================================================
// ○ 面要素作成
//===============================================================*/
void surf_add(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int type = modify["axis"].get<int>();
	double surf_val = modify["val"].get<double>();
	int mat = modify["mat"].get<int>();
	int smat = modify["smat"].get<int>();
	converter.surf_add2d_from3Dsurf(mesh, type, surf_val, mat, smat);
}

/*
//===============================================================
// ○ 面要素作成・複数ver
//===============================================================*/
void surf_add_multi(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int type = modify["axis"].get<int>();
	double surf_val = modify["val"].get<double>();
	int num  = modify["num"].get<int>();
	vector<int> target_mat;
	for(int i = 0 ; i < num ; i++){
		int mat = modify["mat"][i].get<int>();
		target_mat.push_back(mat);
	}
	int smat = modify["smat"].get<int>();
	int inc = modify["inc"].get<int>();
	/* 要素のうち、指定した座標面にある表面を面要素としてメッシュに加える~複数指定ver。 */
	converter.surf_add2d_from3Dsurf2(mesh, type, surf_val, target_mat, smat, inc);
}

/*
//===============================================================
// ○ 面要素作成・半径指定
//===============================================================*/
void surf_add_R(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double r_val = modify["rval"].get<double>();
	double r_eps = modify["eps"].get<double>();
	int mat = modify["mat"].get<int>();
	int smat = modify["smat"].get<int>();
	int sdir = modify["sdir"].get<int>();
	double tmin = modify["range"][0].get<double>();
	double tmax = modify["range"][1].get<double>();
	double zmin = modify["range"][2].get<double>();
	double zmax = modify["range"][3].get<double>();
	converter.surf_add2d_R(mesh, r_val, r_eps, mat, smat, sdir, tmin, tmax, zmin, zmax);

}



/* end of namespace*/
};
};
