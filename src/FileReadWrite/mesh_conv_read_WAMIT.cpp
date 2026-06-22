/**
* @file mesh_conv_read_WAMITe.cpp
* @brief mesh_conv_read_WAMIT dgf
*
*/

#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>


namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ read_WAMITdgfデータ読み取り
//===============================================================*/
/** 
 * @brief WAMIT surface elements  file read
*/
void ConvMeshFileReadWrite:: read_WAMITdgf(const string filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "start reading dgf sruf data from file  " << filename << endl;
#endif
	mesh.info_maked = false;

	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail() ){
		cout << ("can not open (readMeditFile)") << endl;;
	}
	/* 本来の節点・要素の番号記憶 */
	map<convint, convint> nodeMap;
	map<convint, convint> nodeMap_rev;
	map<convint, convint> eleMap;


	string str_line;
	convint icntNode = 0; /*節点数カウンタ*/
	convint icntElmt = 0; /*要素数カウンタ*/
	string strDummy;

	/*領域予約(処理速度向上の為のものであり、100000を超えても問題無い)*/
	mesh.node.clear();
	mesh.element.clear();


	/* 最初のごみ取り */
	getline(fp_in, str_line);
	getline(fp_in, str_line);
	getline(fp_in, str_line);

	/* パネル数 */
	getline(fp_in, str_line);
	const convint panell_num = std::stoi(str_line);
	mesh.node.reserve(4*panell_num);
	mesh.element.reserve(panell_num);

	for(convint nn = 0; nn < panell_num; nn++){
		/* 節点を取得 */
		convint node_ids[4];
		for(int i = 0 ; i < 4 ; i++){
			getline(fp_in, str_line);
			vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ' ');
			double x = stod(temp_str[0]);
			double y = stod(temp_str[1]);
			double z = stod(temp_str[2]);
			ConvNode N_temp;
			N_temp.setID(icntNode);
			N_temp.set(x, y, z);
			mesh.node.push_back(std::move(N_temp));

			node_ids[i] = icntNode;
			icntNode++;
		}
		/* 要素作成 */
		ConvElement etmp;
		etmp.setID(icntElmt);
		etmp.setMat(1);
		ConvEType e_type = ConvEType::Quad;
		etmp.setType(e_type);						
		for(int kk = 0; kk < 4; kk++){
			etmp.setNodes(kk, node_ids[kk]);
		}
		/*配列に追加*/
		mesh.element.push_back( std::move(etmp) );
		icntElmt++;
	}

	fp_in.close();

	
#ifdef PRINT_CONV_INFO
	/*節点数、要素数格納*/
	cout << "** number of nodes    =" << icntNode << endl;
	cout << "** number of Elements =" << icntElmt << endl;
#endif
	return;

}



/**/
};
};


