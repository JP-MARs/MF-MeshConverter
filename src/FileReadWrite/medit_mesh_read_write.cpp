/**
* @file medit_mesh_read_write.cpp
* @brief medit_mesh_read_write
*
*/

#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>


namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ INRIA Medit meshデータ読み取り
//===============================================================*/
/** 
 * @brief mesh(INRIA Medit) file read
*/
void ConvMeshFileReadWrite::readMedit(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "start reading GMSH msh data from file  " << filename << endl;
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
	mesh.element.reserve(100000);


	/* 最初のごみ取り */
	while(true){
		getline(fp_in, str_line);
		vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ' ');
		int ss0 = temp_str.size();
		if(ss0==0) continue;
		if(temp_str[0] == "Vertices"){
			break;
		}
	}
	/* 節点数 */
	getline(fp_in, str_line);
	const convint node_num = std::stoi(str_line);
	mesh.node.reserve(node_num);

	for(convint nn = 0; nn < node_num; nn++){
		getline(fp_in, str_line);
		vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ' ');
		double x = stod(temp_str[0]);
		double y = stod(temp_str[1]);
		double z = stod(temp_str[2]);
		ConvNode N_temp;
		N_temp.setID(icntNode);
		N_temp.set(x, y, z);
		mesh.node.push_back(std::move(N_temp));

		nodeMap[icntNode] = nn;
		nodeMap_rev[nn] = icntNode;
		icntNode++;
	}
	bool read_mode = false;
	string ele_str;
	ConvEType e_type;			
	/* 終わるまで読み取る */
	while(true){
		/* まず１行読み取る */
		getline(fp_in, str_line);
		/* 終端なら終わり */
		if( fp_in.eof() ){
			break;
		}
		/* サーチ中に"$"ぶち当たった＝設定読み取りモードへに */
		if(!read_mode){
			ele_str = "";
			vector<string> temp_str0 = ConvMeshFileReadWrite::split(str_line, ' ');
			for(auto& sss : temp_str0){
				ele_str = sss;
			}
			/* なんの要素か読み取り */
			if(ele_str == "Edges"){
				e_type = ConvEType::Line;
				read_mode = true;
			}else if(ele_str == "Triangles"){
				e_type = ConvEType::Tri;
				read_mode = true;
			}else if(ele_str == "Quadrilaterals"){
				e_type = ConvEType::Quad;
				read_mode = true;
			}else if(ele_str == "Tetrahedra"){
				e_type = ConvEType::Tetra;
				read_mode = true;
			} else if(ele_str == "Hexahedra"){
				e_type = ConvEType::Hex;
				read_mode = true;
			/* 終わり */
			}else if(ele_str == "End"){
				break;
			//}else{
			//	cout << "Medit read Etype unknown... "<< ele_str <<endl;
			//	exit(1);
			}
		}
		/* リードモードの時 */
		if(read_mode){
			/* 要素数 */
			getline(fp_in, str_line);
			const convint e_num = std::stoi(str_line);
			/* 要素read */
			for(convint ee = 0; ee < e_num; ee++){
				getline(fp_in, str_line);
				vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ' ');
				convint nodes[30];
				int num;
				int mat;
				/* 線要素を読む */
				if(e_type == ConvEType::Line){
					num = 2;
					nodes[0] = std::stoi(temp_str[0]);
					nodes[1] = std::stoi(temp_str[1]);
					mat = stoi(temp_str[2]);
				/* 三角形 */
				}else if(e_type == ConvEType::Tri){
					num = 3;
					nodes[0] = std::stoi(temp_str[0]);
					nodes[1] = std::stoi(temp_str[1]);
					nodes[2] = std::stoi(temp_str[2]);
					mat = stoi(temp_str[3]);
				/* 四角形 */
				}else if(e_type == ConvEType::Quad){
					num = 4;
					nodes[0] = std::stoi(temp_str[0]);
					nodes[1] = std::stoi(temp_str[1]);
					nodes[2] = std::stoi(temp_str[2]);
					nodes[3] = std::stoi(temp_str[3]);
					mat = stoi(temp_str[4]);
				/* 四面体 */
				}else if(e_type == ConvEType::Tetra){
					num = 4;
					nodes[0] = std::stoi(temp_str[0]);
					nodes[1] = std::stoi(temp_str[1]);
					nodes[2] = std::stoi(temp_str[2]);
					nodes[3] = std::stoi(temp_str[3]);
					mat = stoi(temp_str[4]);
				/* 六面体 */
				}else if(e_type == ConvEType::Hex){
					num = 8;
					nodes[0] = std::stoi(temp_str[0]);
					nodes[1] = std::stoi(temp_str[1]);
					nodes[2] = std::stoi(temp_str[2]);
					nodes[3] = std::stoi(temp_str[3]);
					nodes[4] = std::stoi(temp_str[4]);
					nodes[5] = std::stoi(temp_str[5]);
					nodes[6] = std::stoi(temp_str[6]);
					nodes[7] = std::stoi(temp_str[7]);
					mat = stoi(temp_str[8]);
				} else{
					std::cout << "Element read in Medit ) this element type is not supported in this program!!!" << std::endl;
					exit(1);
				}
				for(int i = 0 ; i < num ; i++){
					nodes[i]--;
				}
				/* 要素作成 */
				ConvElement etmp;
				etmp.setID(icntElmt);
				etmp.setMat(mat);
				etmp.setType(e_type);						
				for(int kk = 0; kk < num; kk++){
					etmp.setNodes(kk, nodes[kk]);
				}
				/*配列に追加*/
				mesh.element.push_back( std::move(etmp) );
				icntElmt++;
			}
			read_mode = false;
		}
	}
	fp_in.close();

	/* IDの対応関係 */
	if(mesh.save_id_map){
		mesh.node_map = std::move(nodeMap);
		mesh.ele_map = std::move(eleMap);
	}


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


