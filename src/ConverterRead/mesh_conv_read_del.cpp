/**
* @file mesh_conv_read_del.cpp
* @brief mesh_conv_read_del (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ 要素削除（要素タイプ指定）
//===============================================================*/
void del_ele_type(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int type = modify["ele_type"].get<int>();

	ConvEType e_type;
	if(type == 0){
		e_type = ConvEType::Tri;
	}else if(type == 1){
		e_type = ConvEType::Quad;
	}else if(type == 2){
		e_type = ConvEType::Tetra;
	}else if(type == 3){
		e_type = ConvEType::Hex;
	}else if(type == 4){
		e_type = ConvEType::Prizm;
	}else if(type == 5){
		e_type = ConvEType::Pyramid;
	}else if(type == 6){
		e_type = ConvEType::Tri2;
	}else if(type == 7){
		e_type = ConvEType::Quad2;
	}else{
		e_type = ConvEType::Tri;
	}
	converter.delete_ele_type(mesh, e_type);
}
/*
//===============================================================
// ○ 要素削除（1D）
//===============================================================*/
void del_ele_1D(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	converter.delete_ele_type(mesh, ConvEType::Line);
	converter.delete_ele_type(mesh, ConvEType::Point);
}
/*
//===============================================================
// ○ 要素削除（2D）
//===============================================================*/
void del_ele_2D(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	converter.delete_ele_2D(mesh);
}

/*
//===============================================================
// ○ 要素削除（ID指定）
//===============================================================*/
void del_ele_ID(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int num = modify["num"].get<int>();
	vector<convint> start;
	vector<convint> end;
	start.resize(num);
	end.resize(num);
	for(int i = 0 ; i < num ; i++){
		start[i] = modify["range"][i][0].get<convint>();
		end[i] = modify["range"][i][1].get<convint>();
		/* ID補正 */
		if(READ_ID_MODE){
			start[i]--;
			end[i]--;
		}
	}	
	converter.delete_ele_id(mesh, num, start, end);
}

/*
//===============================================================
// ○ 要素削除（xyzで範囲指定）
//===============================================================*/
void del_ele_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.delete_ele_xyz(mesh, xmin, xmax, ymin, ymax, zmin, zmax);
}

/*
//===============================================================
// ○ 要素削除（rtzで範囲指定）
//===============================================================*/
void del_ele_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.delete_ele_RTHZ(mesh, rmin, rmax, tmin, tmax, zmin, zmax);
}
/*
//===============================================================
// ○ 要素削除（材料で範囲指定）
//===============================================================*/
void del_ele_mat(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int mat1 = modify["range"][0].get<int>();
	int mat2 = modify["range"][1].get<int>();
	converter.delete_ele_MAT(mesh, mat1, mat2);
}

/*
//===============================================================
// ○ 要素削除（材料とxyz範囲で範囲指定）
//===============================================================*/
void del_ele_mat_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int mat1 = modify["mat"][0].get<int>();
	int mat2 = modify["mat"][1].get<int>();
	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.delete_ele_MAT_xyz(mesh, xmin, xmax, ymin, ymax, zmin, zmax, mat1, mat2);
}
/*
//===============================================================
// ○ 要素削除（材料とrtz範囲で範囲指定）
//===============================================================*/
void del_ele_mat_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int mat1 = modify["mat"][0].get<int>();
	int mat2 = modify["mat"][1].get<int>();
	double rmin = modify["range"][0].get<double>();
	double rmax = modify["range"][1].get<double>();
	double tmin = modify["range"][2].get<double>();
	double tmax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();
	converter.delete_ele_MAT_RTHZ(mesh, rmin, rmax, tmin, tmax, zmin, zmax, mat1, mat2);
}



/* end of namespace*/
};
};
