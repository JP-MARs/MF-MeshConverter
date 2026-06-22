/**
* @file read_write_auto.cpp
* @brief read_write_auto
*
*/
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>

namespace JPMRspace{
namespace MeshConvLib{



/*
//===============================================================
// ○ 形式自動判定read
//===============================================================*/
/** 
 * @brief file read (format is automatically detected)
*/
void ConvMeshFileReadWrite::read_mesh(const string& filename, ConvMesh& mesh){
	string str_line;
	vector<string> tmp_str;
	tmp_str = ConvMeshFileReadWrite::split(filename, '.');
	/* 最後の文字列が拡張子のはず */
	const int ss = tmp_str.size();
	string ext = tmp_str[ss-1];

	if(ext == "atls"){
		ConvMeshFileReadWrite::readATLAS(filename, mesh);
	}else if(ext == "unv" || ext == "ids"){
		ConvMeshFileReadWrite::readUNV(filename, mesh);
	}else if(ext == "neu"){
		ConvMeshFileReadWrite::readFEMAP(filename, mesh);
	}else if(ext == "vtk"){
		ConvMeshFileReadWrite::readVTK(filename, mesh);
	}else if(ext == "msh"){
		ConvMeshFileReadWrite::readGMSH(filename, mesh);
	}else if(ext == "mesh"){
		ConvMeshFileReadWrite::readMedit(filename, mesh);
	}else if(ext == "jpmmesh"){
		vector<vector<convint>> Ele_neighbour;
		ConvMeshFileReadWrite::readJPMMESH(filename, mesh, Ele_neighbour);
	}else if(ext == "jpmmesh_e"){
		vector<vector<convint>> Ele_neighbour;
		ConvMeshFileReadWrite::readJPMMESH_edge(filename, mesh, Ele_neighbour);
	}else if(ext == "unvjmag"){
		ConvMeshFileReadWrite::readUNV_jmag(filename, mesh);
	}else{
		std::cout << "No mesh format filename type.... " << std::endl;
		exit(1);
	}
}


/*
//===============================================================
// ○ 形式自動判定write
//===============================================================*/
/** 
 * @brief write file (format is auto. detected)
*/
void ConvMeshFileReadWrite::write_mesh(const string& filename, ConvMesh& mesh) {
	string str_line;
	vector<string> tmp_str;
	tmp_str = ConvMeshFileReadWrite::split(filename, '.');
	/* 最後の文字列が拡張子のはず */
	const int ss = tmp_str.size();
	string ext = tmp_str[ss - 1];

	if (ext == "atls") {
		ConvMeshFileReadWrite::writeATLAS(filename, mesh);
	}else if (ext == "unv" || ext == "ids") {
		ConvMeshFileReadWrite::writeUNV(filename, mesh);
	}else if (ext == "neu") {
		ConvMeshFileReadWrite::writeFEMAP(filename, mesh);
	}else if (ext == "vtk") {
		ConvMeshFileReadWrite::writeVTK(filename, mesh);
	}else if (ext == "jpmmesh") {
		if(!mesh.info_maked){
			mesh.makeMeshInfo();
		}
		ConvMeshFileReadWrite::writeJPMMESH(filename, mesh);
	}else if (ext == "jpmmesh_e") {
		if(!mesh.info_maked){
			mesh.makeMeshInfo();
		}
		ConvMeshFileReadWrite::writeJPMMESH_edge(filename, mesh);
	}else {
		std::cout << "No mesh format filename type.... " << std::endl;
		exit(1);
	}
}


/**/
};
};


