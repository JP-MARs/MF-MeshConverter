/**
* @file mesh_conv_read_basic.cpp
* @brief mesh_conv_read_basic (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ 節点読み取りモード変更
//===============================================================*/
void renumber_mode(convjson& modify){
	int mode = modify["mode"].get<int>();
	if(mode == 1){
		READ_ID_MODE = true;
	}else{
		READ_ID_MODE = false;
	}
}

/*
//===============================================================
// ○ スケーリング変更
//===============================================================*/
void scaling_read(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double scale = modify["scale"].get<double>();
	converter.scaling(mesh, scale);
}


/*
//===============================================================
// ○ 2Dと3Dで分けてリナンバ
//===============================================================*/
void renumber_2D(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	converter.renumbering_3D_and_2Dele(mesh);
}

/*
//===============================================================
// ○ 重複判定距離の変更
//===============================================================*/
void set_tolera(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	double eps = modify["eps"].get<double>();
	converter.setEPS(eps);
}

/*
//===============================================================
// ○ 最小の辺の長さを表示
//===============================================================*/
void min_edge_length(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	if(!(mesh.info_maked) ){
		mesh.makeMeshInfo();
	}
	if(mesh.have_3D){
		std::cout << mesh.minumum_edge_L_3D << std::endl;
	}else{
		std::cout << mesh.minumum_edge_L_2D << std::endl;
	}
}




/* end of namespace*/
};
};
