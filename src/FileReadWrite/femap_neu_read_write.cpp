/**
* @file femap_neu_read_write.cpp
* @brief femap_neu_read_write
*
*/
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>


namespace JPMRspace{
namespace MeshConvLib{


/*//=======================================================
// ● 文字列split自作ver
//=======================================================*/
/** 
 * @brief split strings
 * @param input string data
 * @param delimiter a character for split
*/
std::vector<std::string> ConvMeshFileReadWrite::split(const std::string& input, char delimiter){
	std::string field;
	std::vector<std::string> result;
	std::string input2 = input;
	/* タブをただの空白に変換しておく */
	for(auto& cc : input2){
		if(cc == '\t'){
			cc = ' ';
		}
	}
	std::istringstream stream(input2);
	while ( getline(stream, field, delimiter) ) {
		if(field.size() == 0 || field == " "){
			continue;
		}
		size_t pos;
		while((pos = field.find_first_of(" 　\t")) != std::string::npos){
			field.erase(pos, 1);
		}
		result.push_back(field);
	}
	return result;
}


/*
//===============================================================
// ○ FEMAP neuデータ読み取り
//===============================================================*/
/** 
 * @brief neu(FEMAP neutral) file read
*/
void ConvMeshFileReadWrite::readFEMAP(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "start reading femap neu data from file  " << filename << endl;
#endif
	mesh.info_maked = false;

	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail() ){
		cout << ("can not open (readNeuFile)") << endl;;
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
	mesh.node.reserve(100000);
	mesh.element.reserve(100000);

	bool read_mode = false;
	int read_type = 0;

	while(true){
		/* まず１行読み取る */
		getline(fp_in, str_line);
		/* 終端なら終わり */
		if( fp_in.eof() ){
			break;
		}
		/* サーチ中に-１ぶち当たった＝設定読み取りモードへに */
		if(str_line == "   -1" && !read_mode){
			read_mode = true;
			getline(fp_in, str_line);
			read_type = stoi(str_line);
		}
		/* リードモードの時 */
		if(read_mode){
			/* 節点read開始 */
			if(read_type == 403){
				while(true){
					/* 一行読み取り */
					getline(fp_in, str_line);
					int judge_int = std::stoi(str_line);
					/* －１に当たったらこのモード終了 */
					if(judge_int == -1){
						read_mode = false;
						read_type = 0;
						break;
					}
					/* 節点を読む */
					vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ',');
					/* IDセット */
					convint id = stoi(temp_str[0]);
					/* 座標値 */
					double x = stod(temp_str[11]);
					double y = stod(temp_str[12]);
					double z = stod(temp_str[13]);
					ConvNode N_temp;
					N_temp.setID(icntNode);
					N_temp.set(x, y, z);
					mesh.node.push_back( std::move(N_temp) );
					
					nodeMap[icntNode] = id;
					nodeMap_rev[id] = icntNode; 
					icntNode++;		
				}
			/**/
			/* 要素read開始 */
			}else if(read_type == 404){
				while(true){
					/* 一行読み取り */
					getline(fp_in, str_line);
					int judge_int = std::stoi(str_line);
					/* －１に当たったらこのモード終了 */
					if(judge_int == -1){
						read_mode = false;
						read_type = 0;
						break;
					}
					/* 要素情報1行目を読む */
					vector<string> temp_str = ConvMeshFileReadWrite::split(str_line, ',');
					/* IDセット */
					convint id = stoi(temp_str[0]);
					//id--;/* IDをゼロ始まりに修正 */
					/* 材料ID(FEMAP上はプロパティID) */
					int material_prop = stoi(temp_str[2]);
					/* 要素タイプ(FEMAP上はtopology) */
					int type = stoi(temp_str[4]);

					/* ２行目読み取り＝使わない */
					getline(fp_in, str_line);

					/* 3&4行目読み取り＝要素節点 */
					getline(fp_in, str_line);
					vector<string> temp_str1 = ConvMeshFileReadWrite::split(str_line, ',');
					getline(fp_in, str_line);
					vector<string> temp_str2 = ConvMeshFileReadWrite::split(str_line, ',');
					convint inode[30];
					for(int kk = 0 ; kk < 30 ; kk++){
						inode[kk]=0;
					}
					/* type分け */
					ConvEType e_type;
					/* 六面体 */
					if(type == 8){
						inode[0] = stoi(temp_str1[0]);
						inode[1] = stoi(temp_str1[1]);
						inode[2] = stoi(temp_str1[2]);
						inode[3] = stoi(temp_str1[3]);
						inode[4] = stoi(temp_str1[4]);
						inode[5] = stoi(temp_str1[5]);
						inode[6] = stoi(temp_str1[6]);
						inode[7] = stoi(temp_str1[7]);
						e_type = ConvEType::Hex;
					/* プリズム */
					}else if(type == 7){
						inode[0] = stoi(temp_str1[0]);
						inode[1] = stoi(temp_str1[1]);
						inode[2] = stoi(temp_str1[2]);
						inode[3] = stoi(temp_str1[4]);
						inode[4] = stoi(temp_str1[5]);
						inode[5] = stoi(temp_str1[6]);
						/* おなじ節点があったら、これはピラミッド要素である */
						bool same = false;
						for(int ii = 0 ; ii < 6 ; ii++){
							for(int jj = ii+1 ; jj < 6 ; jj++){
								same |= (inode[ii] == inode[jj]);
							}
						}
						if(same){
							e_type = ConvEType::Pyramid;
							convint tmp_nodes[30];
							for(int ii = 0 ; ii < 6 ; ii++){
								tmp_nodes[ii] = inode[ii];
							}
							inode[0] = tmp_nodes[2];
							inode[1] = tmp_nodes[1];
							inode[2] = tmp_nodes[4];
							inode[3] = tmp_nodes[5];
							inode[4] = tmp_nodes[0];
						}else{
							e_type = ConvEType::Prizm;
						}
					/* 四面体 */
					}else if(type == 6){
						inode[0] = stoi(temp_str1[0]);
						inode[1] = stoi(temp_str1[1]);
						inode[2] = stoi(temp_str1[2]);
						inode[3] = stoi(temp_str1[3]);
						e_type = ConvEType::Tetra;
					/* 四角形 */
					}else if(type == 4){
						inode[0] = stoi(temp_str1[0]);
						inode[1] = stoi(temp_str1[1]);
						inode[2] = stoi(temp_str1[2]);
						inode[3] = stoi(temp_str1[3]);
						e_type = ConvEType::Quad;
					/* 三角形 */
					}else if(type == 2){
						inode[0] = stoi(temp_str1[0]);
						inode[1] = stoi(temp_str1[1]);
						inode[2] = stoi(temp_str1[2]);
						e_type = ConvEType::Tri;
					/* 線要素 */
					}else if(type == 0){
						inode[0] = stoi(temp_str1[0]);
						inode[1] = stoi(temp_str1[1]);
						e_type = ConvEType::Line;
					}

					for(int i = 0 ; i < 30 ; i++){
						convint inode_tmp = inode[i];
						size_t ccc = nodeMap_rev.count(inode_tmp);
						if(ccc != 0){
							inode[i] = nodeMap_rev[inode_tmp];
						}
					}
					/* 要素作成 */
					ConvElement etmp;
					etmp.setID(icntElmt);
					etmp.setMat(material_prop);
					etmp.setType(e_type);
					etmp.setID(icntElmt);
					int n_size = etmp.getNodeNum();
					for(int kk = 0; kk < n_size; kk++){
						etmp.setNodes(kk, inode[kk]);
					}
					/*配列に追加*/
					mesh.element.push_back( std::move(etmp) );

					eleMap[icntElmt] = id;
					icntElmt++;

					/* 5~8行目読み取り＝使わない */
					getline(fp_in, str_line);
					getline(fp_in, str_line);
					getline(fp_in, str_line);
					getline(fp_in, str_line);
				}
			/**/
			/**/
			/* それ以外＝無視 */
			}else{
				while(true){
					/* 一行読み取り */
					getline(fp_in, str_line);
					int judge_int = std::stoi(str_line);
					/* －１に当たったらこのモード終了 */
					if(judge_int == -1){
						read_mode = false;
						read_type = 0;
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


/*
//===============================================================
// ○ neuデータ書き出し
//===============================================================*/
/** 
 * @brief neu(FEMAP neutral) file write
*/
void ConvMeshFileReadWrite::writeFEMAP(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "** start write FEMAP data" << endl;
#endif
	/* ファイルオープン*/
	//FILE *fp = fopen(filename.c_str(), "w");
	std::fstream fp(filename.c_str(), std::ios::out);
	if( fp.fail() ){
		cout << ("can not open (write Neu)") << endl;;
	}

	/* 最初に色々書き込む */
	fp << "   -1" << endl;
	fp << "   100" << endl;
	fp << "<NULL>" << endl;
	fp << "19.1," << endl;
	fp << "   -1" << endl;
	fp << "   -1" << endl;
	fp << "   413" << endl;
	fp << "1,124," << endl;
	fp << "Default" << endl;
	fp << "9999,124," << endl;
	fp << "Construction Layer" << endl;
	fp << "   -1" << endl;
	/* マテリアルを書き込む＝使わないのでダミーで1種類書き出しておく */
	fp << "   -1" << endl;
	fp << "   601" << endl;
	/*2D用*/
	fp << "1,-601,55,0,0,1,0," << endl;
	fp << "dammy2D" << endl;
	fp << "10," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "25," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0," << endl;
	fp << "200," << endl;
	fp << "1.,1.,1.,1.,1.,1.,0.1,0.1,0.1,1.022727272727273," << endl;
	fp << "0.113636363636364,0.113636363636364,0.,0.,0.,1.022727272727273,0.113636363636364,0.,0.,0.," << endl;
	fp << "1.022727272727273,0.,0.,0.,1.,0.,0.,1.,0.,1.," << endl;
	fp << "1.01010101010101,0.101010101010101,0.,1.01010101010101,0.,1.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "50," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "70," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	/*3D用*/
	fp << "1,-601,55,0,0,2,0," << endl;
	fp << "dammy3D" << endl;
	fp << "10," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "25," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,	" << endl;
	fp << "200," << endl;
	fp << "1.,1.,1.,1.,1.,1.,0.1,0.1,0.1,1.022727272727273," << endl;
	fp << "0.113636363636364,0.113636363636364,0.,0.,0.,1.022727272727273,0.113636363636364,0.,0.,0.," << endl;
	fp << "1.022727272727273,0.,0.,0.,1.,0.,0.,1.,0.,1.," << endl;
	fp << "1.01010101010101,0.101010101010101,0.,1.01010101010101,0.,1.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "50," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "70," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	/*1D用*/
	fp << "3,-601,55,0,0,0,0," << endl;
	fp << "dammy3D" << endl;
	fp << "10," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "25," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,	" << endl;
	fp << "200," << endl;
	fp << "1.,1.,1.,1.,1.,1.,0.1,0.1,0.1,1.022727272727273," << endl;
	fp << "0.113636363636364,0.113636363636364,0.,0.,0.,1.022727272727273,0.113636363636364,0.,0.,0.," << endl;
	fp << "1.022727272727273,0.,0.,0.,1.,0.,0.,1.,0.,1.," << endl;
	fp << "1.01010101010101,0.101010101010101,0.,1.01010101010101,0.,1.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "0.,0.,0.,0.,0.,0.,0.,0.,0.,0.," << endl;
	fp << "50," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "70," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "0,0,0,0,0,0,0,0,0,0," << endl;
	fp << "   -1" << endl;
	/**/
	/**/
	/* 材料（プロパティ）を書き出す＝まずID数を列挙する */
	map<int, int> material1D;
	map<int, int> material2D;
	map<int, int> material3D;
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	for(convint i = 0 ; i < e_size ; i++){
		int mat = mesh.element[i].getMat();
		bool is3D = mesh.element[i].is3D();
		bool is1D = mesh.element[i].is1D();
		if(is3D){
			material3D[mat] = 1;
		}else if(is1D){
			material1D[mat] = 1;
		}else{
			material2D[mat] = 1;
		}
	}
	/* 書き出し開始 */
	fp << "   -1" << endl;
	fp << "   402" << endl;
	int counter1 = 1;
	int counter2 = 1;
	/* 1次元用プロパティを書き出す */
	for(auto itr : material1D) {
		int prop_id = itr.first;
		string name = "mat1D_" + std::to_string(prop_id);
		fp << prop_id << "," << counter2 << ",3,5,1,0,0," << endl;
		fp << name << endl;
		fp << "0,0,0,0," << endl;
		fp << "10," << endl;
		fp << "0,0,0,0,0,0,0,0," << endl;
		fp << "0,0," << endl;
		fp << "78," << endl;
		fp << "1.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.," << endl;
		fp << "0," << endl;
		fp << "0," << endl;
		counter1++;
		counter2++;
		if(counter2 > 170){
			counter2=1;
		}
	}
	counter2 = 1;
	/* 2次元用プロパティを書き出す */
	for(auto itr : material2D) {
		int prop_id = itr.first;
		string name = "mat2D_" + std::to_string(prop_id);
		fp << prop_id << "," << counter2 << ",1,17,1,0,0," << endl;
		fp << name << endl;
		fp << "0,0,0,0," << endl;
		fp << "10," << endl;
		fp << "0,0,0,0,0,0,0,0," << endl;
		fp << "0,0," << endl;
		fp << "78," << endl;
		fp << "1.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.," << endl;
		fp << "0," << endl;
		fp << "0," << endl;
		counter1++;
		counter2++;
		if(counter2 > 170){
			counter2=1;
		}
	}
	counter2 = 1;
	/* 3次元用プロパティを書き出す */
	for(auto itr : material3D) {
		int prop_id = itr.first;
		string name = "mat3D_" + std::to_string(prop_id);
		fp << prop_id << "," << counter2 << ",2,39,1,0,0," << endl;
		fp << name << endl;
		fp << "0,0,0,0," << endl;
		fp << "10," << endl;
		fp << "0,0,0,0,0,0,0,0," << endl;
		fp << "0,0," << endl;
		fp << "78," << endl;
		fp << "1.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.,0.,0.," << endl;
		fp << "0.,0.,0.," << endl;
		fp << "0," << endl;
		fp << "0," << endl;
		counter1++;
		counter2++;
		if(counter2 > 170){
			counter2=1;
		}
	}
	fp << "   -1" << endl;

	/**/
	/**/
	/**/
	/* 節点書き出し開始 */
	fp << "   -1" << endl;
	fp << "   403" << endl;
	/* まず節点を書き出す */
	for(convint i = 0 ; i < n_size ; i++){
		convint id = mesh.node[i].getID();
		id++;/* 1はじまりに直す */
		double xx = mesh.node[i].getX();
		double yy = mesh.node[i].getY();
		double zz = mesh.node[i].getZ();
		fp<< id << ",0,0,1,46,0,0,0,0,0,0," <<  std::setprecision(25) << scientific<< xx << "," <<  std::setprecision(25) << scientific<< yy <<"," <<  std::setprecision(25) << scientific<< zz << ",0,0," << endl;
	}
	fp << "   -1" << endl;
	/**/
	/**/
	/*********************************** */
	/*********************************** */
	/*********************************** */
	/*********************************** */
	//ここまだ途中！！！！！！！！！！！！！！！！！！！！！
	/* 要素タイプ 0:三角形、1:四角形、2:四面体、3:六面体、4:プリズム,5:ピラミッド   101:点要素,  201:線要素*/
	//	int trans[] = {2, 4, 6, 8, 7, 999};
	//int trans_femap_type[] = {17, 17, 39, 39, 39, 39};
	map<ConvEType, int> trans;
	map<ConvEType, int> trans_femap;
	trans[ConvEType::Line] = 21;
	trans_femap[ConvEType::Line] = 39;/*????????????????あってる？？？*/

	trans[ConvEType::Tri] = 2;
	trans_femap[ConvEType::Tri] = 17;
	trans[ConvEType::Quad] = 2;
	trans_femap[ConvEType::Quad] = 17;

	trans[ConvEType::Tetra] = 6;
	trans_femap[ConvEType::Tetra] = 39;
	trans[ConvEType::Prizm] = 7;
	trans_femap[ConvEType::Prizm] = 39;
	trans[ConvEType::Hex] = 8;
	trans_femap[ConvEType::Hex] = 39;
	/*********************************** */

	/* つぎ、要素を書き出す */
	fp << "   -1" << endl;
	fp << "   404" << endl;
	for(convint i = 0 ; i < e_size ; i++){
		/* まず要素タイプゲット */
		ConvEType type0 = mesh.element[i].getType();
		int type;
		int type_fe;
		try{
			type = trans[type0];
			type_fe = trans_femap[type0];
		}catch(...){
			cout << "No acceptable element ! " << (int)(type0) << endl;
			exit(1);
		}
		/* その他の情報 */
		convint id = mesh.element[i].getID();
		id++;/* 1始まりに直す */
		convint inode[50];
		for(int kk = 0 ; kk < 50 ; kk++){
			inode[kk]=0;
		}
		mesh.element[i].getNodes(inode);
		/* 1始まりに直す */
		for(int kk = 0 ; kk < 50 ; kk++){
			inode[kk]++;
		}
		int mat = mesh.element[i].getMat();
		fp<< id << ",124," << mat << "," << type_fe << "," << type << ",1,0,0,0,0,0,0,0,0,0," << endl;
		fp << "0,0,0,0,0,0,0,0,0,0,0,0,"<< endl;
		/* 節点 */
		/* 六面体 */
		if(type0 == ConvEType::Hex){
			fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ","<<inode[3] << ","<<inode[4] << ","<<inode[5] << ","<<inode[6] << ","<<inode[7] << ","<< 0 << "," << 0 << "," << endl;
			fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		/* プリズム */
		}else if(type0 == ConvEType::Prizm){
			//fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ","<<inode[3] << ","<<inode[4] << ","<<inode[5] << ","<<0 << ","<<0 << ","<< 0 << "," << 0 << "," << endl;
			fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ",0," <<inode[3] << ","<<inode[4] << ","<<inode[5] << ","<<0 << ","<<0 << ","<< 0 << "," <<endl;
			fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		/* ピラミッド */
		//}else if(type0 == 5){
		//	fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ","<<inode[3] << ","<<inode[4] << ","<<inode[5] << ","<<0 << ","<<0 << ","<< 0 << "," << 0 << "," << endl;
		//	fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		/* 四面体 */
		}else if(type0 == ConvEType::Tetra){
			//fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ","<<inode[3] << ","<<inode[4] << ","<<inode[5] << ","<<0 << ","<<0 << ","<< 0 << "," << 0 << "," << endl;
			fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ",0," <<inode[3] << ","<<inode[4] << ","<<inode[5] << ","<<0 << ","<<0 << ","<< 0 << "," <<endl;
			fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		/* 四角形 */
		}else if(type0 == ConvEType::Quad){
			fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ","<<inode[3] << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<< 0 << "," << 0 << "," << endl;
			fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		/* 三角形 */
		}else if(type0 == ConvEType::Tri){
			fp << inode[0] << ","<<inode[1] << ","<<inode[2] << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<< 0 << "," << 0 << "," << endl;
			fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		/* 線要素 */
		}else if(type0 == ConvEType::Line){
			fp << inode[0] << ","<<inode[1] << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<< 0 << "," << 0 << "," << endl;
			fp << 0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << ","<<0 << "," << 0 << "," << endl;
		}
		fp << "0.,0.,0.,0,0,0,0,0,0,"<< endl;
		fp << "0.,0.,0.,"<< endl;
		fp << "0.,0.,0.,"<< endl;
		fp << "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"<< endl;
	}
	fp <<"   -1" << endl;

	/* その他、色々書き出す */
	fp <<"   -1" << endl;
	fp <<"   943" << endl;
	fp <<"0,0,1," << endl;
	fp <<"0,0,0,0,0,0," << endl;
	fp <<"2,2,2,2,2,2," << endl;
	fp <<"   -1" << endl;
	fp <<"   -1" << endl;
	fp <<"   942" << endl;
	fp <<"0,0,0,0," << endl;
	fp <<"1,65,0,0," << endl;
	fp <<"2,128,0,0," << endl;
	fp <<"3,192,0,0," << endl;
	fp <<"4,255,0,0," << endl;
	fp <<"5,0,65,0," << endl;
	fp <<"6,65,65,0," << endl;
	fp <<"7,128,65,0," << endl;
	fp <<"8,192,65,0," << endl;
	fp <<"9,255,65,0," << endl;
	fp <<"10,0,128,0," << endl;
	fp <<"11,65,128,0," << endl;
	fp <<"12,128,128,0," << endl;
	fp <<"13,192,128,0," << endl;
	fp <<"14,255,128,0," << endl;
	fp <<"15,0,192,0," << endl;
	fp <<"16,65,192,0," << endl;
	fp <<"17,128,192,0," << endl;
	fp <<"18,192,192,0," << endl;
	fp <<"19,255,192,0," << endl;
	fp <<"20,0,255,0," << endl;
	fp <<"21,65,255,0," << endl;
	fp <<"22,128,255,0," << endl;
	fp <<"23,192,255,0," << endl;
	fp <<"24,255,255,0," << endl;
	fp <<"25,0,0,65," << endl;
	fp <<"26,65,0,65," << endl;
	fp <<"27,128,0,65," << endl;
	fp <<"28,192,0,65," << endl;
	fp <<"29,255,0,65," << endl;
	fp <<"30,0,65,65," << endl;
	fp <<"31,65,65,65," << endl;
	fp <<"32,128,65,65," << endl;
	fp <<"33,192,65,65," << endl;
	fp <<"34,255,65,65," << endl;
	fp <<"35,0,128,65," << endl;
	fp <<"36,65,128,65," << endl;
	fp <<"37,128,128,65," << endl;
	fp <<"38,192,128,65," << endl;
	fp <<"39,255,128,65," << endl;
	fp <<"40,0,192,65," << endl;
	fp <<"41,65,192,65," << endl;
	fp <<"42,128,192,65," << endl;
	fp <<"43,192,192,65," << endl;
	fp <<"44,255,192,65," << endl;
	fp <<"45,0,255,65," << endl;
	fp <<"46,65,255,65," << endl;
	fp <<"47,128,255,65," << endl;
	fp <<"48,192,255,65," << endl;
	fp <<"49,255,255,65," << endl;
	fp <<"50,0,0,128," << endl;
	fp <<"51,65,0,128," << endl;
	fp <<"52,128,0,128," << endl;
	fp <<"53,192,0,128," << endl;
	fp <<"54,255,0,128," << endl;
	fp <<"55,0,65,128," << endl;
	fp <<"56,65,65,128," << endl;
	fp <<"57,128,65,128," << endl;
	fp <<"58,192,65,128," << endl;
	fp <<"59,255,65,128," << endl;
	fp <<"60,0,128,128," << endl;
	fp <<"61,65,128,128," << endl;
	fp <<"62,128,128,128," << endl;
	fp <<"63,192,128,128," << endl;
	fp <<"64,255,128,128," << endl;
	fp <<"65,0,192,128," << endl;
	fp <<"66,65,192,128," << endl;
	fp <<"67,128,192,128," << endl;
	fp <<"68,192,192,128," << endl;
	fp <<"69,255,192,128," << endl;
	fp <<"70,0,255,128," << endl;
	fp <<"71,65,255,128," << endl;
	fp <<"72,128,255,128," << endl;
	fp <<"73,192,255,128," << endl;
	fp <<"74,255,255,128," << endl;
	fp <<"75,0,0,192," << endl;
	fp <<"76,65,0,192," << endl;
	fp <<"77,128,0,192," << endl;
	fp <<"78,192,0,192," << endl;
	fp <<"79,255,0,192," << endl;
	fp <<"80,0,65,192," << endl;
	fp <<"81,65,65,192," << endl;
	fp <<"82,128,65,192," << endl;
	fp <<"83,192,65,192," << endl;
	fp <<"84,255,65,192," << endl;
	fp <<"85,0,128,192," << endl;
	fp <<"86,65,128,192," << endl;
	fp <<"87,128,128,192," << endl;
	fp <<"88,192,128,192," << endl;
	fp <<"89,255,128,192," << endl;
	fp <<"90,0,192,192," << endl;
	fp <<"91,65,192,192," << endl;
	fp <<"92,128,192,192," << endl;
	fp <<"93,192,192,192," << endl;
	fp <<"94,255,192,192," << endl;
	fp <<"95,0,255,192," << endl;
	fp <<"96,65,255,192," << endl;
	fp <<"97,128,255,192," << endl;
	fp <<"98,192,255,192," << endl;
	fp <<"99,255,255,192," << endl;
	fp <<"100,0,0,255," << endl;
	fp <<"101,65,0,255," << endl;
	fp <<"102,128,0,255," << endl;
	fp <<"103,192,0,255," << endl;
	fp <<"104,255,0,255," << endl;
	fp <<"105,0,65,255," << endl;
	fp <<"106,65,65,255," << endl;
	fp <<"107,128,65,255," << endl;
	fp <<"108,192,65,255," << endl;
	fp <<"109,255,65,255," << endl;
	fp <<"110,0,128,255," << endl;
	fp <<"111,65,128,255," << endl;
	fp <<"112,128,128,255," << endl;
	fp <<"113,192,128,255," << endl;
	fp <<"114,255,128,255," << endl;
	fp <<"115,0,192,255," << endl;
	fp <<"116,65,192,255," << endl;
	fp <<"117,128,192,255," << endl;
	fp <<"118,192,192,255," << endl;
	fp <<"119,255,192,255," << endl;
	fp <<"120,0,255,255," << endl;
	fp <<"121,65,255,255," << endl;
	fp <<"122,128,255,255," << endl;
	fp <<"123,192,255,255," << endl;
	fp <<"124,255,255,255," << endl;
	fp <<"125,10,10,10," << endl;
	fp <<"126,20,20,20," << endl;
	fp <<"127,30,30,30," << endl;
	fp <<"128,40,40,40," << endl;
	fp <<"129,50,50,50," << endl;
	fp <<"130,60,60,60," << endl;
	fp <<"131,70,70,70," << endl;
	fp <<"132,80,80,80," << endl;
	fp <<"133,90,90,90," << endl;
	fp <<"134,100,100,100," << endl;
	fp <<"135,110,110,110," << endl;
	fp <<"136,120,120,120," << endl;
	fp <<"137,130,130,130," << endl;
	fp <<"138,140,140,140," << endl;
	fp <<"139,150,150,150," << endl;
	fp <<"140,160,160,160," << endl;
	fp <<"141,170,170,170," << endl;
	fp <<"142,180,180,180," << endl;
	fp <<"143,190,190,190," << endl;
	fp <<"144,200,200,200," << endl;
	fp <<"145,210,210,210," << endl;
	fp <<"146,220,220,220," << endl;
	fp <<"147,230,230,230," << endl;
	fp <<"148,240,240,240," << endl;
	fp <<"149,250,250,250," << endl;
	fp <<"150,91,111,145," << endl;
	fp <<"151,132,149,177," << endl;
	fp <<"152,160,174,198," << endl;
	fp <<"153,188,199,216," << endl;
	fp <<"154,215,225,240," << endl;
	fp <<"155,130,147,135," << endl;
	fp <<"156,150,167,155," << endl;
	fp <<"157,170,187,175," << endl;
	fp <<"158,190,207,195," << endl;
	fp <<"159,210,227,215," << endl;
	fp <<"160,136,130,108," << endl;
	fp <<"161,156,150,128," << endl;
	fp <<"162,176,170,148," << endl;
	fp <<"163,196,190,168," << endl;
	fp <<"164,216,210,188," << endl;
	fp <<"165,255,255,255," << endl;
	fp <<"166,255,255,255," << endl;
	fp <<"167,255,255,255," << endl;
	fp <<"168,255,255,255," << endl;
	fp <<"169,255,255,255," << endl;
	fp <<"170,255,255,255," << endl;
	fp <<"171,255,255,255," << endl;
	fp <<"172,255,255,255," << endl;
	fp <<"173,255,255,255," << endl;
	fp <<"174,255,255,255," << endl;
	fp <<"   -1" << endl;


	fp.close();
#ifdef PRINT_CONV_INFO
	cout << "** end write Ideas data" << endl;
#endif

}



/**/
};
};


