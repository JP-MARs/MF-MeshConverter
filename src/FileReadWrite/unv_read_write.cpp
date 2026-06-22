/**
* @file unv_read_write.cpp
* @brief unv_read_write
*
*/
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>

namespace JPMRspace{
namespace MeshConvLib{



/*
//===============================================================
// ○ unv/ideasデータ読み取り
//===============================================================*/
/** 
 * @brief unv(ideas) file read
*/
void ConvMeshFileReadWrite::readUNV(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "** start reading Ideas data from file  " << filename << endl;
#endif
	mesh.info_maked = false;

	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail()  ){
		cout << ("can not open (readIdeas)") << endl;;
	}
	/* 本来の節点・要素の番号記憶 */
	map<convint, convint> nodeMap;
	map<convint, convint> nodeMap_rev;
	map<convint, convint> eleMap;

	string strHead;
	convint icntNode = 0; /*節点数カウンタ*/
	convint icntElmt = 0; /*要素数カウンタ*/
	convint icntDummy = 0;
	string strDummy;

	/*領域予約(処理速度向上の為のものであり、100000を超えても問題無い)*/
	mesh.node.clear();
	mesh.element.clear();
	mesh.node.reserve(100000);
	mesh.element.reserve(100000);

	fp_in >> strHead;
	fp_in >> strHead;
	/*読みこみ開始 -- 節点*/
	while(true){
		/**/
		fp_in >> icntDummy;
		/* マイナスなら節点終わり */
		if(icntDummy == -1){
			break;
		}

		int temp1, temp2, temp3;
		fp_in >> temp1 >> temp2 >> temp3;

		string xStr, yStr, zStr;
		fp_in >> xStr;
		auto pos = xStr.find('D');
		if (pos != std::string::npos) {
			xStr.replace(pos, 1, "E");
		}
		double x = std::stod(xStr);
		/* y座標読みこみ */
		fp_in >> yStr;
		pos = yStr.find('D');
		if (pos != std::string::npos) {
			yStr.replace(pos, 1, "E");
		}
		double y = std::stod(yStr);
		/* z座標読みこみ */
		fp_in >> zStr;
		pos = zStr.find('D');
		if (pos != std::string::npos) {
			zStr.replace(pos, 1, "E");
		}
		double z = std::stod(zStr);

		/*節点データ生成と配列格納*/
		ConvNode N_temp;
		N_temp.setID(icntNode);/* IDをゼロ始まりに修正 */
		N_temp.set(x, y, z);
		mesh.node.push_back( std::move(N_temp) );

		/* 本来の節点・要素の番号記憶 */
		nodeMap[icntNode] = icntDummy;
		nodeMap_rev[icntDummy] =icntNode;
		icntNode++;		
	}
	fp_in >> strHead;
	fp_in >> strHead;
	/*読みこみ開始 -- 要素*/
	while(true){
		/**/
		fp_in >> icntDummy;
		/* マイナスなら節点終わり */
		if(icntDummy == -1){
			break;
		}
		int type, mat;
		int temp1, temp2, temp3;//, temp4;
		fp_in >> type >> mat >>  temp1 >> temp2 >> temp3;

		ConvEType e_type;
		convint inode[30];
		for(int i = 0; i < 30; i++){
			inode[i] = 0;
		}
		/* type分け */
		/* 六面体 */
		if(type == 115){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3]  >> inode[4]  >> inode[5]  >> inode[6]  >> inode[7] ;
			e_type = ConvEType::Hex;
		/* プリズム */
		}else if(type == 112){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3]  >> inode[4]  >> inode[5];
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
				/*inode[0] = tmp_nodes[0];
				inode[1] = tmp_nodes[2];
				inode[2] = tmp_nodes[5];
				inode[3] = tmp_nodes[3];
				inode[4] = tmp_nodes[4];*/
			}else{
				e_type = ConvEType::Prizm;
			}
		/* 四面体 */
		}else if(type == 111){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3];
			e_type = ConvEType::Tetra;
		/* 四角形 */
		}else if(type == 94){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3];
			e_type = ConvEType::Quad;
		/* 三角形 */
		}else if(type == 91){
			fp_in >> inode[0]  >> inode[1]  >> inode[2];
			e_type = ConvEType::Tri;
		/* 線要素 */
		}else if(type == 21){
			fp_in >> inode[0]  >> inode[1]  >> inode[2];
			fp_in >> inode[0]  >> inode[1];
			e_type = ConvEType::Line;
		}

		for(int i = 0 ; i < 30 ; i++){
			int ccc = nodeMap_rev.count(inode[i]);
			if( ccc != 0 ){
				inode[i] = nodeMap_rev[inode[i]];
			}
		}
		eleMap[icntElmt] = icntDummy;

		ConvElement etmp;
		etmp.setMat(mat);
		etmp.setType(e_type);
		etmp.setID(icntElmt);/* IDをゼロ始まりに修正 */
		int n_size = etmp.getNodeNum();
		for(int kk = 0; kk < n_size; kk++){
			etmp.setNodes(kk, inode[kk]);
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


	/* IDの対応関係 */
	if(mesh.save_id_map){
		mesh.node_map = std::move(nodeMap);
		mesh.ele_map = std::move(eleMap);
	}

	return;
}


/*
//===============================================================
// ○ JMAG unvデータ読み取り（多目的ファイル出力の結果）
//===============================================================*/
/** 
 * @brief unv(ideas) file made by JMAG read
*/
void ConvMeshFileReadWrite::readUNV_jmag(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "** start reading Ideas data from file  " << filename << endl;
#endif
	mesh.info_maked = false;

	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail()  ){
		cout << ("can not open (readIdeas)") << endl;;
	}
	/* 本来の節点・要素の番号記憶 */
	map<convint, convint> nodeMap;
	map<convint, convint> nodeMap_rev;
	map<convint, convint> eleMap;

	string strHead;
	convint icntNode = 0; /*節点数カウンタ*/
	convint icntElmt = 0; /*要素数カウンタ*/
	convint icntDummy = 0;
	string strDummy;

	/*領域予約(処理速度向上の為のものであり、100000を超えても問題無い)*/
	mesh.node.clear();
	mesh.element.clear();
	mesh.node.reserve(100000);
	mesh.element.reserve(100000);

	/*読みこみ開始 -- ダミーを飛ばして節点までループ*/	
	while(true){
		fp_in >> strHead;
		cout << strHead << endl;
		try{
			icntDummy = std::stoi(strHead);
			if(icntDummy == -1){
				fp_in >> icntDummy;
				if(icntDummy == 2411){
					break;
				}
			}else{				
				while(true){
					fp_in >> strHead;
					icntDummy = std::stoi(strHead);
					if(icntDummy == -1){
						break;
					}
				}
			}
		}catch(...){
		}
	}
	while(true){
		/**/
		fp_in >> icntDummy;
		/* マイナスなら節点終わり */
		if(icntDummy == -1){
			break;
		}

		int temp1, temp2, temp3;
		fp_in >> temp1 >> temp2 >> temp3;

		string xStr, yStr, zStr;
		fp_in >> xStr;
		auto pos = xStr.find('D');
		if (pos != std::string::npos) {
			xStr.replace(pos, 1, "E");
		}
		double x = std::stod(xStr);
		/* y座標読みこみ */
		fp_in >> yStr;
		pos = yStr.find('D');
		if (pos != std::string::npos) {
			yStr.replace(pos, 1, "E");
		}
		double y = std::stod(yStr);
		/* z座標読みこみ */
		fp_in >> zStr;
		pos = zStr.find('D');
		if (pos != std::string::npos) {
			zStr.replace(pos, 1, "E");
		}
		double z = std::stod(zStr);

		/*節点データ生成と配列格納*/
		ConvNode N_temp;
		N_temp.setID(icntNode);/* IDをゼロ始まりに修正 */
		N_temp.set(x, y, z);
		mesh.node.push_back( std::move(N_temp) );

		/* 本来の節点・要素の番号記憶 */
		nodeMap[icntNode] = icntDummy;
		nodeMap_rev[icntDummy] =icntNode;
		icntNode++;
	}
	fp_in >> strHead;
	fp_in >> strHead;
	/*読みこみ開始 -- 要素*/
	while(true){
		/**/
		fp_in >> icntDummy;
		/* マイナスなら節点終わり */
		if(icntDummy == -1){
			break;
		}
		ConvEType e_type;
		int type, mat;
		int temp1, temp2, temp3, temp4;
		fp_in >> type >> mat >>  temp1 >> temp2 >> temp3;
		//JMAGの場合、材料こっち
		mat = temp1;

		convint inode[30];
		/* type分け */
		/* 六面体 */
		if(type == 115){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3]  >> inode[4]  >> inode[5]  >> inode[6]  >> inode[7] ;
			e_type = ConvEType::Hex;
			/* プリズム */
		}else if(type == 112){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3]  >> inode[4]  >> inode[5];
			/* おなじ節点があったら、これはピラミッド要素である */
			bool same = false;
			for(int ii = 0 ; ii < 6 ; ii++){
				for(int jj = ii+1 ; jj < 6 ; jj++){
					same |= (inode[ii] == inode[jj]);
				}
			}
			if(same){
				e_type = ConvEType::Pyramid;
				int tmp_nodes[30];
				for(int ii = 0 ; ii < 6 ; ii++){
					tmp_nodes[ii] = inode[ii];
				}
				inode[0] = tmp_nodes[2];
				inode[1] = tmp_nodes[1];
				inode[2] = tmp_nodes[4];
				inode[3] = tmp_nodes[5];
				inode[4] = tmp_nodes[0];
				/*inode[0] = tmp_nodes[0];
				inode[1] = tmp_nodes[2];
				inode[2] = tmp_nodes[5];
				inode[3] = tmp_nodes[3];
				inode[4] = tmp_nodes[4];*/
			}else{
				e_type = ConvEType::Prizm;
			}
			/* 四面体 */
		}else if(type == 111){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3];
			e_type = ConvEType::Tetra;
			/* 四角形 */
		}else if(type == 94){
			fp_in >> inode[0]  >> inode[1]  >> inode[2]  >> inode[3];
			e_type = ConvEType::Quad;
			/* 三角形 */
		}else if(type == 91){
			fp_in >> inode[0]  >> inode[1]  >> inode[2];
			e_type = ConvEType::Tri;
			/* 線要素 */
		}else if(type == 21){
			fp_in >> inode[0]  >> inode[1]  >> inode[2];
			fp_in >> inode[0]  >> inode[1];
			e_type = ConvEType::Line;
		}

		for(int i = 0 ; i < 30 ; i++){
			int ccc = nodeMap_rev.count(inode[i]);
			if( ccc != 0 ){
				inode[i] = nodeMap_rev[inode[i]];
			}
		}
		eleMap[icntElmt] = icntDummy;

		ConvElement etmp;
		etmp.setMat(mat);
		etmp.setType(e_type);
		etmp.setID(icntElmt);/* IDをゼロ始まりに修正 */
		int n_size = etmp.getNodeNum();
		for(int kk = 0; kk < n_size; kk++){
			etmp.setNodes(kk, inode[kk]);
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


	/* IDの対応関係 */
	if(mesh.save_id_map){
		mesh.node_map = std::move(nodeMap);
		mesh.ele_map = std::move(eleMap);
	}

	return;

}


/*
//===============================================================
// ○ unv/ideasデータ書き出し
//===============================================================*/
/** 
 * @brief unv(ideas) file write
*/
void ConvMeshFileReadWrite::writeUNV(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "** start write Ideas data" << endl;
#endif
	/* ファイルオープン*/
	std::fstream fp(filename, std::ios::out);
	if( fp.fail() ){
		cout << ("can not open (write NAS)") << endl;;
	}

	/* 適当にヘッダ */
	fp << "    -1" << endl;
	fp << "  2411" << endl;

	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();

	/* まず節点を書き出す */
	for(convint i = 0 ; i < n_size ; i++){
		convint id = mesh.node[i].getID();
		id++;/* 1はじまりに直す */
		double xx = mesh.node[i].getX();
		double yy = mesh.node[i].getY();
		double zz = mesh.node[i].getZ();
		//char xchar[999], ychar[999], zchar[999];

		/* 指数表示をEからDにして、指数部が3ケタなら2ケタにする */
		std::stringstream ss;
		ss << std::setprecision(16) << std::scientific << xx;
		//sprintf(xchar, "%18.16e", xx);
		string xStr1;// = string(xchar);		
		ss >> xStr1;
		size_t pos = xStr1.find_last_of('e');
		xStr1[pos] ='D';
		string temp = xStr1.substr(0, pos+1);
		size_t xl = xStr1.length(); size_t xl2 = temp.size();
		size_t diff = xl-xl2;
		if(diff==4){
			xStr1[xl-3] = xStr1[xl-2];
			xStr1[xl-2] = xStr1[xl-1];
			xStr1[xl-1] = 'X';
			string temp0 = xStr1.substr(0, xStr1.find_last_of('X'));
			xStr1 = temp0;
		}

		//sprintf(xchar, "%18.16e", yy);
		ss.clear();
		ss << std::setprecision(16) << std::scientific << yy;
		string yStr1;// = string(xchar);		
		ss >> yStr1;
		pos = yStr1.find_last_of('e');
		yStr1[pos] ='D';
		temp = yStr1.substr(0, pos+1);
		xl = yStr1.length(); xl2 = temp.size();
		diff = xl-xl2;
		if(diff==4){
			yStr1[xl-3] = yStr1[xl-2];
			yStr1[xl-2] = yStr1[xl-1];
			yStr1[xl-1] = 'X';
			string temp0 = yStr1.substr(0, yStr1.find_last_of('X'));
			yStr1 = temp0;
		}
		//sprintf(xchar, "%18.16e", zz);
		ss.clear();
		ss << std::setprecision(16) << scientific << zz;
		string zStr1;// = string(xchar);		
		ss >> zStr1;
		pos = zStr1.find_last_of('e');
		zStr1[pos] ='D';
		temp = zStr1.substr(0, pos+1);
		xl = zStr1.length(); xl2 = temp.size();
		diff = xl-xl2;
		if(diff==4){
			zStr1[xl-3] = zStr1[xl-2];
			zStr1[xl-2] = zStr1[xl-1];
			zStr1[xl-1] = 'X';
			string temp0 = zStr1.substr(0, zStr1.find_last_of('X'));
			zStr1 = temp0;
		}

		fp << setw(10) << right << id << "         0         0        11" << endl;
		fp << setw(25) << right << xStr1.c_str() << setw(25) << right << yStr1.c_str() << setw(25) << right << zStr1.c_str() << endl;
		//fprintf(fp, "% 10d         0         0        11\n", id);
		//fprintf(fp, "% 25s% 25s% 25s\n", xStr1.c_str(), yStr1.c_str(), zStr1.c_str());
	}
	fp << "    -1" << endl;
	fp << "    -1" << endl;
	fp << "  2412" << endl;

	map<ConvEType, int> trans;// = {91, 94, 111, 115, 112, 999};;
	trans[ConvEType::Line] = 21;
	trans[ConvEType::Tri] = 91;
	trans[ConvEType::Quad] = 94;
	trans[ConvEType::Tetra] = 111;
	trans[ConvEType::Pyramid] = 999;
	trans[ConvEType::Prizm] = 112;
	trans[ConvEType::Hex] = 115;

	/* つぎ、要素を書き出す */
	for(convint i = 0 ; i < e_size ; i++){
		/* まず要素タイプゲット */
		ConvEType type0 = mesh.element[i].getType();
		/*int trans[] = {91, 94, 111, 115, 112, 999};
		int type;
		if(type0 <= 99){
			type = trans[type0];
		}else if(type0 == 201){
			type = 21;
		}*/
		int type = trans[type0];

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
		/* type分け */
		/* 六面体 */
		if(type == 115){
			fp << setw(10) << right << id <<"       115" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         8" << endl;
			fp << setw(10) << right <<inode[0] << setw(10) << right <<inode[1] << setw(10) << right <<inode[2] << setw(10) << right <<inode[3];
			fp << setw(10) << right <<inode[4] << setw(10) << right <<inode[5] << setw(10) << right <<inode[6] << setw(10) << right <<inode[7] << endl;
		/* プリズム */
		}else if(type == 112){
			fp << setw(10) << right << id <<"       112" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         6" << endl;
			fp << setw(10) << right <<inode[0] << setw(10) << right <<inode[1] << setw(10) << right <<inode[2] << setw(10) << right <<inode[3];
			fp << setw(10) << right <<inode[4] << setw(10) << right <<inode[5] << endl;
		/* ピラミッド */
		}else if(type == 999){
			int type00X = 306;
			convint tmp_inode[6];/* 振りなおしてプリズム化する */
			tmp_inode[0] = inode[4];
			tmp_inode[1] = inode[1];
			tmp_inode[2] = inode[0];
			tmp_inode[3] = inode[4];
			tmp_inode[4] = inode[2];
			tmp_inode[5] = inode[3];
			fp << setw(10) << right << id <<"       112" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         6" << endl;
			fp << setw(10) << right <<tmp_inode[0] << setw(10) << right <<tmp_inode[1] << setw(10) << right <<tmp_inode[2] << setw(10) << right <<tmp_inode[3];
			fp << setw(10) << right <<tmp_inode[4] << setw(10) << right <<tmp_inode[5] << endl;
		/* 四面体 */
		}else if(type == 111){
			fp << setw(10) << right << id <<"       111" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         4" << endl;
			fp << setw(10) << right <<inode[0] << setw(10) << right <<inode[1] << setw(10) << right <<inode[2] << setw(10) << right <<inode[3] << endl;
		/* 四角形 */
		}else if(type == 94){
			fp << setw(10) << right << id <<"        94" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         4" << endl;
			fp << setw(10) << right <<inode[0] << setw(10) << right <<inode[1] << setw(10) << right <<inode[2] << setw(10) << right <<inode[3] << endl;
		/* 三角形 */
		}else if(type == 91){
			fp << setw(10) << right << id <<"        91" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         3" << endl;
			fp << setw(10) << right <<inode[0] << setw(10) << right <<inode[1] << setw(10) << right <<inode[2] << endl;
		/* 線要素 */
		}else if(type == 21){
			fp << setw(10) << right << id <<"        21" << setw(10) << right << mat <<  setw(10) << right << mat << "         7         2" << endl;
			fp << "         0         1         1" << endl;
			fp << setw(10) << right <<inode[0] << setw(10) << right <<inode[1] << endl;
		}
	}
	fp << "    -1" << endl;
	fp.close();
#ifdef PRINT_CONV_INFO
	cout << "** end write Ideas data" << endl;
#endif
}



/**/
};
};


