/**
* @file gmsh_read_write.cpp
* @brief gmsh_read_write
*
*/

#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>


namespace JPMRspace{
namespace MeshConvLib{


/*
//===============================================================
// ○ GMSH mshデータ読み取り
//===============================================================*/
/** 
 * @brief msh(GMSH) file read
*/
void ConvMeshFileReadWrite::readGMSH(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "start reading GMSH msh data from file  " << filename << endl;
#endif
	mesh.info_maked = false;

	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail() ){
		cout << ("can not open (readGmshFile)") << endl;;
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
	//mesh.node.reserve(100000);
	//mesh.element.reserve(100000);

	bool read_mode = false;
	//int read_type = 0;

	/* 最初のごみ取り（フォーマットについて） */
	getline(fp_in, str_line);
	getline(fp_in, str_line);
	getline(fp_in, str_line);

	while(true){
		/* まず１行読み取る */
		getline(fp_in, str_line);
		/* 終端なら終わり */
		if( fp_in.eof() ){
			break;
		}
		/* サーチ中に"$"ぶち当たった＝設定読み取りモードへに */
		if(str_line[0] == '$' && !read_mode){
			read_mode = true;
			//getline(fp_in, str_line);
			//read_type = stoi(str_line);
		}
		/* リードモードの時 */
		if(read_mode){
			/* 節点read開始 */
			if(str_line == "$Nodes"){
				/* 節点情報を読む */
				getline(fp_in, str_line);
				vector<string> n_tmp_str = ConvMeshFileReadWrite::split(str_line, ' ');
				const convint num_tag = std::stoi(n_tmp_str[0]);
				const convint num_nodes = std::stoi(n_tmp_str[1]);
				const convint min_node_id = std::stoi(n_tmp_str[2]);
				const convint max_node_id = std::stoi(n_tmp_str[3]);
				mesh.node.reserve(num_nodes);
				/* タグの数だけループ */
				for(convint tt = 0; tt < num_tag; tt++){
					/* このノードタグについて読む */
					getline(fp_in, str_line);
					vector<string> ntag_tmp_str = ConvMeshFileReadWrite::split(str_line, ' ');
					const int entityDim = std::stoi(ntag_tmp_str[0]);
					const int entityTag = std::stoi(ntag_tmp_str[1]);
					const int parametric = std::stoi(ntag_tmp_str[2]);
					const convint numNodesInBlock = std::stoi(ntag_tmp_str[3]);
					/* まずID一覧を読む */
					vector<convint> node_ids_temp;
					node_ids_temp.resize(numNodesInBlock);
					for(convint nn = 0; nn < numNodesInBlock; nn++){
						getline(fp_in, str_line);
						convint temp_nid = std::stoi(str_line);
						node_ids_temp[nn] = temp_nid;
					}
					/* 座標を読んで確定させる */
					for(convint nn = 0; nn < numNodesInBlock; nn++){
						getline(fp_in, str_line);
						vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ' ');
						double x = stod(temp_str[0]);
						double y = stod(temp_str[1]);
						double z = stod(temp_str[2]);
						ConvNode N_temp;
						N_temp.setID(icntNode);
						N_temp.set(x, y, z);
						mesh.node.push_back(std::move(N_temp));

						nodeMap[icntNode] = node_ids_temp[nn];
						nodeMap_rev[node_ids_temp[nn]] = icntNode;
						icntNode++;
					}
				}
				/* 最後の一行を読んで節点モード終了 */
				getline(fp_in, str_line);
				if(str_line == "$EndNodes"){
					read_mode = false;
				}else{
					std::cout << "Node read end Error!!! in GMSH "<<endl;
					exit(1);
				}
			/**/
			/* 要素read開始 */
			}else if(str_line == "$Elements"){
				/* 要素情報を読む */
				getline(fp_in, str_line);
				vector<string> e_tmp_str = ConvMeshFileReadWrite::split(str_line, ' ');
				const convint num_tag = std::stoi(e_tmp_str[0]);
				const convint num_elements = std::stoi(e_tmp_str[1]);
				const convint min_ele_id = std::stoi(e_tmp_str[2]);
				const convint max_ele_id = std::stoi(e_tmp_str[3]);
				mesh.element.reserve(num_elements);

				/* タグの数だけループ */
				for(convint tt = 0; tt < num_tag; tt++){
					/* この要素タグについて読む */
					getline(fp_in, str_line);
					vector<string> etag_tmp_str = ConvMeshFileReadWrite::split(str_line, ' ');
					const int entityDim = std::stoi(etag_tmp_str[0]);
					const int entityTag = std::stoi(etag_tmp_str[1]);
					const int elementType = std::stoi(etag_tmp_str[2]);	/* 要素タイプ */
					const convint numElementsInBlock = std::stoi(etag_tmp_str[3]);/* 要素数 */
					/* 読んで確定させる */
					for(convint nn = 0; nn < numElementsInBlock; nn++){
						getline(fp_in, str_line);
						vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ' ');
						convint eid= stoi(temp_str[0]);
						ConvEType e_type;
						convint nodes[30];
						int num;
						/* 線要素を読む */
						if(elementType == 1){
							num = 2;
							e_type = ConvEType::Line;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
						/* 三角形 */
						}else if(elementType == 2){
							num = 3;
							e_type = ConvEType::Tri;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
						/* 四角形 */
						}else if(elementType == 3){
							num = 4;
							e_type = ConvEType::Quad;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
						/* 四面体 */
						}else if(elementType == 4){
							num = 4;
							e_type = ConvEType::Tetra;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
						/* 六面体 */
						}else if(elementType == 5){
							num = 8;
							e_type = ConvEType::Hex;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
							nodes[4] = std::stoi(temp_str[5]);
							nodes[5] = std::stoi(temp_str[6]);
							nodes[6] = std::stoi(temp_str[7]);
							nodes[7] = std::stoi(temp_str[8]);
						/* プリズム */
						}else if(elementType == 6){
							num = 6;
							e_type = ConvEType::Prizm;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
							nodes[4] = std::stoi(temp_str[5]);
							nodes[5] = std::stoi(temp_str[6]);
						/* ピラミッド */
						}else if(elementType == 7){
							num = 5;
							e_type = ConvEType::Pyramid;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
							nodes[4] = std::stoi(temp_str[5]);
						/* 三角形2次 */
						}else if(elementType == 9){
							num = 6;
							e_type = ConvEType::Tri2;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
							nodes[4] = std::stoi(temp_str[5]);
							nodes[5] = std::stoi(temp_str[6]);
						/* 四角形2次 */
						} else if(elementType == 16){
							num = 8;
							e_type = ConvEType::Quad2;
							nodes[0] = std::stoi(temp_str[1]);
							nodes[1] = std::stoi(temp_str[2]);
							nodes[2] = std::stoi(temp_str[3]);
							nodes[3] = std::stoi(temp_str[4]);
							nodes[4] = std::stoi(temp_str[5]);
							nodes[5] = std::stoi(temp_str[6]);
							nodes[6] = std::stoi(temp_str[7]);
							nodes[7] = std::stoi(temp_str[8]);
						} else{
							std::cout << "Element read ) this element type is not supported in this program!!!" << std::endl;
							exit(1);
						}

						for(int i = 0 ; i < num ; i++){
							convint inode_tmp = nodes[i];
							size_t ccc = nodeMap_rev.count(inode_tmp);
							if(ccc != 0){
								nodes[i] = nodeMap_rev[inode_tmp];
							}
						}
						/* 要素作成 */
						ConvElement etmp;
						etmp.setID(icntElmt);
						etmp.setMat(entityTag);
						etmp.setType(e_type);						
						for(int kk = 0; kk < num; kk++){
							etmp.setNodes(kk, nodes[kk]);
						}
						/*配列に追加*/
						mesh.element.push_back( std::move(etmp) );

						eleMap[icntElmt] = eid;
						icntElmt++;
					}
				}
				/* 最後の一行を読んで節点モード終了 */
				getline(fp_in, str_line);
				if(str_line == "$EndElements"){
					read_mode = false;
				}else{
					std::cout << "Element read end Error!!! in GMSH "<<endl;
					exit(1);
				}
			/**/
			/* それ以外＝無視 */
			}else{
				while(true){
					/* 一行読み取り */
					getline(fp_in, str_line);
					/* －１に当たったらこのモード終了 */
					if(str_line[0] == '$'){
						read_mode = false;
						break;
					}
				}
			}
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


