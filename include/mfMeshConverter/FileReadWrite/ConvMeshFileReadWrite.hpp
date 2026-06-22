/**
* @file ConvMeshFileReadWrite.hpp
* @brief define read/write mesh file data
*
*/
#ifndef DEF_MCONVMESH_DEAR_WRITE_DEF_2
#define DEF_MCONVMESH_DEAR_WRITE_DEF_2

#include "../ConvMeshCommon.hpp"

#include <sstream>
#include <fstream>
#include <iomanip>

namespace JPMRspace{
namespace MeshConvLib{

using std::setw;
using std::right;
using std::scientific;



/*
//=======================================================
// ■ ConvMeshFileReadWrite
//=======================================================
// FEMのメッシュ読み書きクラス
//=======================================================*/
/**
* @class ConvMeshFileReadWrite
* @brief Static class for mesh read/write
* read: .unv, .neu, .vtk, .msh, .mesh, original(jpmmesh), and private mode
* output: .unv, .neu, .vtk, original(jpmmesh)
*/
class ConvMeshFileReadWrite{
public:
	static std::vector<std::string> split(const std::string& input, char delimiter);

	/* 形式自動判定read */
	static void read_mesh(const string& filename, ConvMesh& mesh);
	/* 形式自動判定write */
	static void write_mesh(const string& filename, ConvMesh& mesh);

	/* 独自形式JPMMESH */
	static void readJPMMESH(const string& filename, ConvMesh& mesh, vector<vector<convint>>& Ele_neighbour);
	static void writeJPMMESH(const string& filename, ConvMesh& mesh);
	static void readJPMMESH_edge(const string& filename, ConvMesh& mesh, vector<vector<convint>>& Ele_neighbour);
	static void writeJPMMESH_edge(const string& filename, ConvMesh& mesh, bool e2d_out=false);

	/* ALTAS */
	static void readATLAS(const string& filename, ConvMesh& mesh);
	static void writeATLAS(const string& filename, ConvMesh& mesh);

	/* unv/ideas */
	static void readUNV(const string& filename, ConvMesh& mesh);
	static void writeUNV(const string& filename, ConvMesh& mesh);

	/* FEMAP neu */
	static void readFEMAP(const string& filename, ConvMesh& mesh);
	static void writeFEMAP(const string& filename, ConvMesh& mesh);

	/* VTK neu */
	static void readVTK(const string& filename, ConvMesh& mesh);
	static void writeVTK(const string& filename, ConvMesh& mesh);

	/* GMSH msh */
	static void readGMSH(const string& filename, ConvMesh& mesh);

	/* INRIA Medit mesh */
	static void readMedit(const string& filename, ConvMesh& mesh);


	/* 個別の特殊形式たち */
	static void readUNV_jmag(const string& filename, ConvMesh& mesh);/* JMAGの多目的ファイル出力で出てくるunv */
	static void read_WAMITdgf(const string filename, ConvMesh& mesh);/* WATMIの境界要素モデル */

};


/**/
};
};

#endif
