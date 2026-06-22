/**
* @file mesh_conv_read_mesh_3d_connects.cpp
* @brief mesh_conv_read_mesh_3d_connects (command)
*
*/
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>

namespace JPMRspace{
namespace MeshConvLib{

/*
//===============================================================
// ○ 2D to 3D
//===============================================================*/
void mesh_2d_to_3d(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	string filename = modify["filename"].get<string>();

	std::fstream fp_in(filename, std::ios::in);

	string str_line;
	vector<string> tmp_line;
	std::getline(fp_in, str_line);
	tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');

	/**/
	/* 積み上げデータ読み取り：基本部 */
	int no_layer, no_base_mat, no_layer_data, no_z_data, coor, no_surf, dir_mode;

	no_layer = std::stoi(tmp_line[0]);
	no_base_mat = std::stoi(tmp_line[1]);
	no_layer_data = std::stoi(tmp_line[2]);
	no_z_data = std::stoi(tmp_line[3]);
	coor = std::stoi(tmp_line[4]);
	no_surf = std::stoi(tmp_line[5]);
	dir_mode = std::stoi(tmp_line[6]);

	/* データ読み取り：z座標値 */
	vector<int> layer_data;
	vector<double> layer_z_data;
	vector<double> layer_z_rate;
	/* 指定Zがゼロのとき、全指定と判断 */
	if(no_z_data == 0){
		no_z_data = no_layer;
		layer_data.resize(no_z_data+1);
		layer_z_data.resize(no_z_data+1);
		layer_z_rate.resize(no_z_data+1);
		for(int i = 0 ; i < no_z_data ; i++){
			getline(fp_in, str_line);
			tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');
			//fp_in >> z_val;
			layer_data[i] = i;
			layer_z_data[i] = std::stod(tmp_line[0]);
			layer_z_rate[i] = 1.0;
		}
	}else{
		layer_data.resize(no_z_data+1);
		layer_z_data.resize(no_z_data+1);
		layer_z_rate.resize(no_z_data+1);
		for(int i = 0 ; i < no_z_data ; i++){
			getline(fp_in, str_line);
			//cout << "stack " << i << " => " <<  str_line << endl;
			tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');
			//fp_in >> pos >> z_val >> z_rate;
			layer_data[i] = std::stoi(tmp_line[0]) - 1;
			layer_z_data[i] = std::stod(tmp_line[1]);
			layer_z_rate[i] = std::stod(tmp_line[2]);
			//cout << layer_data[i] << ", " << layer_z_data[i] << ", " << layer_z_rate[i] << endl;
		}
		getline(fp_in, str_line);
		tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');
		//fp_in >> pos >> z_val;/* 最後の位置 */
		layer_data[no_z_data] = no_layer;
		layer_z_data[no_z_data] = std::stod(tmp_line[1]);;
	}
	/* データ読み取り：2Dでの材料 */
	vector<int> mat_id_2d;
	mat_id_2d.resize(no_base_mat);
	getline(fp_in, str_line);
	tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');
	for(int i = 0 ; i < no_base_mat ; i++){
		mat_id_2d[i] = std::stoi(tmp_line[i]);
	}

	/* データ読み取り：3D化で変更する材料ID　*/
	vector<int> from_layer;
	from_layer.resize(no_layer_data);
	vector<int> to_layer;
	to_layer.resize(no_layer_data);
	vector<vector<int>> layer_mat;
	layer_mat.resize(no_layer_data);
	for(int i = 0 ; i < no_layer_data ; i++){
		layer_mat[i].resize(no_base_mat);
	}
	for(int i = 0 ; i < no_layer_data ; i++){
		getline(fp_in, str_line);
		tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');

		int from, to;
		from = std::stoi(tmp_line[0]);
		to = std::stoi(tmp_line[1]);
		from_layer[i] = from - 1; 
		to_layer[i] = to - 1; 
		for(int j = 0 ; j < no_base_mat ; j++){
			layer_mat[i][j] = std::stoi(tmp_line[2+j]);
		}
	}

	/* 面番号読み取り */
	vector<int> base_mat2D;
	base_mat2D.resize(no_surf);
	vector<int> base_mat3D;
	base_mat3D.resize(no_surf);
	vector<int> layer_pos;
	layer_pos.resize(no_surf);
	for(int i = 0 ; i < no_surf ; i++){
		getline(fp_in, str_line);
		tmp_line = ConvMeshFileReadWrite::split(str_line, ' ');
		//fp_in >> mat2 >> mat3 >> pos;
		base_mat2D[i] = std::stoi(tmp_line[0]);//mat2;
		base_mat3D[i] = std::stoi(tmp_line[1]);//mat3;
		layer_pos[i] = std::stoi(tmp_line[2]);//pos;
	}
	fp_in.close();


	converter.mesh2D_to_3D(mesh, no_layer, no_base_mat, no_layer_data, no_z_data, coor, no_surf, dir_mode,
		layer_data, layer_z_data, layer_z_rate, mat_id_2d,
		from_layer, to_layer, layer_mat,
		base_mat2D, base_mat3D, layer_pos);
}


/*
//===============================================================
// ○ メッシュ結合
//===============================================================*/
void mesh_connect(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int kmat = modify["kmat"].get<int>();
	int knod = modify["knod"].get<int>();
	string filename = modify["filename"].get<string>();
	int surf = modify["surf"].get<int>();
	double f_val = modify["val"].get<double>();

	ConvMesh mesh2;
	ConvMeshFileReadWrite::read_mesh(filename, mesh2);
	
	converter.add_mesh(mesh, mesh2, kmat, knod, surf, f_val);
}




/*
//===============================================================
// ○ 六面体の節点ローカルID変更
//===============================================================*/
void mesh_hex_local_change(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	int dir = modify["dir"].get<int>();
	int mat1 = modify["range_mat"][0].get<int>();
	int mat2 = modify["range_mat"][1].get<int>();

	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();

	converter.local_id_change_HEX(mesh, dir, mat1, mat2, xmin, xmax, ymin, ymax, zmin, zmax);
}

/*
//===============================================================
// ○ 六面体の節点ローカルIDを自分で指定して変更
//===============================================================*/
void mesh_hex_local_change_self(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	vector<convint> local_mod;
	local_mod.resize(8);
	local_mod[0] = modify["local"][0].get<convint>();
	local_mod[1] = modify["local"][1].get<convint>();
	local_mod[2] = modify["local"][2].get<convint>();
	local_mod[3] = modify["local"][3].get<convint>();
	local_mod[4] = modify["local"][4].get<convint>();
	local_mod[5] = modify["local"][5].get<convint>();
	local_mod[6] = modify["local"][6].get<convint>();
	local_mod[7] = modify["local"][7].get<convint>();

	int mat1 = modify["range_mat"][0].get<int>();
	int mat2 = modify["range_mat"][1].get<int>();

	double xmin = modify["range"][0].get<double>();
	double xmax = modify["range"][1].get<double>();
	double ymin = modify["range"][2].get<double>();
	double ymax = modify["range"][3].get<double>();
	double zmin = modify["range"][4].get<double>();
	double zmax = modify["range"][5].get<double>();

	converter.local_id_mod_HEX(mesh, local_mod, mat1, mat2, xmin, xmax, ymin, ymax, zmin, zmax);
}


/*
//===============================================================
// ○ 2次元面要素の節点並びを、面積が正になるように並び替える 
//===============================================================*/
void mesh_2d_local_changer(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	converter.local_id_mod_2DSurf(mesh);
}

/*
//===============================================================
// ○ 2次元面要素の四角形を全て三角形に
//===============================================================*/
void mesh_sq_to_tri(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	converter.div_sq_to_tri(mesh);
}

/*
//===============================================================
// ○ 3次元要素のプリズムを全て四面体に
//===============================================================*/
void mesh_pri_to_tet(ConvMesh& mesh, MeshConverter& converter, convjson& modify){
	converter.div_pri_to_tet(mesh);
}


/* end of namespace*/
};
};
