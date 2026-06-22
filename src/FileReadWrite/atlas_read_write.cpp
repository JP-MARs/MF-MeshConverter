/**
* @file atlas_read_write.cpp
* @brief atlas_read_write
*
*/
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>

namespace JPMRspace{
namespace MeshConvLib{



/*
//===============================================================
// ○ ATLASデータ読み取り
//===============================================================*/
/** 
 * @brief ATLAS read
*/
void ConvMeshFileReadWrite::readATLAS(const string& filename, ConvMesh& mesh){
	mesh.info_maked = false;
#ifdef PRINT_CONV_INFO
	cout << "start reading ATLAS data from file" << endl;
#endif
	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail() ){
		cout << ("can not open (readATLAS)") << endl;;
	}

	map<convint, convint> nodeMap;
	map<convint, convint> nodeMap_rev;
	map<convint, convint> eleMap;

	string strHead;
	convint icntNode = 0; /*節点数カウンタ*/
	convint icntElmt = 0; /*要素数カウンタ*/
	convint icntDummy= 0;
	string yStr, strDummy, strDummy2;
	double x, y, z;

	/*領域予約(処理速度向上の為のものであり、100000を超えても問題無い)*/
	mesh.node.clear();
	mesh.element.clear();
	mesh.node.reserve(100000);
	mesh.element.reserve(100000);

	/* 先頭読み取り */
	getline(fp_in, strDummy);

	convint id;
	int type, mat;

	while(true){

		/*節点読みこみ開始*/
		while (fp_in >> id){
			/*終了チェック*/
			if (id < 0){
				cout << "node read Fin!"<<endl;
				break;
			}

			fp_in >> x;
			fp_in >> y;
			fp_in >> z;
			ConvNode N_temp;
			N_temp.setID(icntNode);/* IDをゼロ始まりに修正 */
			N_temp.set(x, y, z);
			mesh.node.push_back( std::move(N_temp) );

			/* 本来の節点・要素の番号記憶 */
			nodeMap[icntNode] = id;
			nodeMap_rev[id] = icntNode;
			icntNode++;		
		}
		fp_in >> strDummy;
		fp_in >> strDummy2;
		/*要素読みこみ開始*/
		while (fp_in >> id){
			/*終了チェック*/
			if (id < 0){
				cout << "Ele read Fin!"<<endl;
				break;
			}
			fp_in >> type;
			ConvElement etmp;
			convint inode[30];
			for(int kk=0 ; kk < 30 ; kk++){
				inode[kk]=0;
			}
			/* 六面体 */
			if(type == 308){
				fp_in >> mat;
				fp_in >> inode[0] >> inode[1] >> inode[2] >> inode[3] >> inode[4];
				fp_in >> inode[5] >> inode[6] >> inode[7];
				etmp.setType(ConvEType::Hex);
			/* プリズム(五面体) */
			}else if(type == 306){
				fp_in >> mat;
				fp_in >> inode[0] >> inode[1] >> inode[2] >> inode[3] >> inode[4];
				fp_in >> inode[5];
				/* おなじ節点があったら、これはピラミッド要素である */
				bool same = false;
				for(int ii = 0 ; ii < 6 ; ii++){
					for(int jj = ii+1 ; jj < 6 ; jj++){
						same |= (inode[ii] == inode[jj]);
					}
				}
				if(same){
					etmp.setType(ConvEType::Pyramid);
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
					etmp.setType(ConvEType::Prizm);
				}
			/* 四面体 */
			}else if(type == 304){
				fp_in >> mat;
				fp_in >> inode[0] >> inode[1] >> inode[2] >> inode[3];
				etmp.setType(ConvEType::Tetra);
			/* 四角形 */
			}else if(type == 204){
				fp_in >> mat;
				fp_in >> inode[0] >> inode[1] >> inode[2] >> inode[3];
				etmp.setType(ConvEType::Quad);
			/* 三角形 */
			}else if(type == 203){
				fp_in >> mat;
				fp_in >> inode[0] >> inode[1] >> inode[2];
				etmp.setType(ConvEType::Tri);
			/* 線要素 */
			}else if(type == 102){
				fp_in >> mat;
				fp_in >> inode[0] >> inode[1];
				etmp.setType(ConvEType::Line);
			}else{
				cout << "error in read element!!!!!!!!!!! " << type << ", " << id <<endl;
			}
			/* IDをゼロ始まりに修正 */
			for(int kk=0 ; kk < 30 ; kk++){
				convint inode_tmp = inode[kk];
				int ccc = nodeMap_rev.count(inode_tmp);
				if(ccc != 0){
					inode[kk] = nodeMap_rev[inode_tmp];
				}
			}
			etmp.setMat(mat);
			etmp.setID(icntElmt);/* IDをゼロ始まりに修正 */
			int n_size = etmp.getNodeNum();
			for(int kk = 0; kk < n_size; kk++){
				etmp.setNodes(kk, inode[kk]);
			}
			/*配列に追加*/
			mesh.element.push_back( std::move(etmp) );
			
			eleMap[icntElmt] = id;
			icntElmt++;
		}
		/* 次を読む */
		if( fp_in.eof() ){
			break;
		}
		/* まだあるならごみ取り */
		string st1,st2;
		fp_in >> st1 >> st2;
	}
	fp_in.close();

	/* IDの対応関係 */
	if(mesh.save_id_map){
		mesh.node_map = std::move(nodeMap);
		mesh.ele_map = std::move(eleMap);
	}


	/*節点数、要素数格納*/
#ifdef PRINT_CONV_INFO
	cout << "** number of nodes    =" << icntNode << endl;
	cout << "** number of Elements =" << icntElmt << endl;
	cout << "read ATLAS end!!!!!"<<endl;
#endif
	return;
}


/*
//===============================================================
// ○ ATLASデータ書き出し
//===============================================================*/
/** 
 * @brief ATLAS write
*/
void ConvMeshFileReadWrite::writeATLAS(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "write ATLAS file "<< endl;
#endif

	/* ファイルオープン*/
	std::fstream fp(filename, std::ios::out);
	if( fp.fail() ){
		cout << ("can not open (write ATLAS)") << endl;;
	}
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();

	/* まず節点を書き出す */
	fp << "GRID    (I8,3E19.10)" << endl;
	for(convint i = 0 ; i < n_size ; i++){
		convint id = mesh.node[i].getID();
		id++;/* 1はじまりに直す */
		double xx = mesh.node[i].getX();
		double yy = mesh.node[i].getY();
		double zz = mesh.node[i].getZ();
		fp << setw(8) << right << id << setw(20) << scientific << right << xx << setw(20) << scientific << right << yy << setw(20) << scientific << right << zz << endl;
		//fprintf(fp, "% 8d %19.10e %19.10e %19.10e\n", id, xx, yy, zz);
	}
	fp << "   -1" << endl;

	map<ConvEType, int> trans;// = {203, 204, 304, 308, 306, 305};
	trans[ConvEType::Line] = 102;
	trans[ConvEType::Tri] = 203;
	trans[ConvEType::Quad] = 204;
	trans[ConvEType::Tetra] = 304;
	trans[ConvEType::Pyramid] = 305;
	trans[ConvEType::Prizm] = 306;
	trans[ConvEType::Hex] = 308;

	/* つぎ、要素を書き出す */
	fp <<  "CONC    (8I8)" << endl;
	for(convint i = 0 ; i < e_size ; i++){
		/* まず要素タイプゲット */
		ConvEType type0 = mesh.element[i].getType();
		
		int type;
		try{
			type = trans[type0];
		}catch(...){
			cout << "No acceptable element ! " << (int)(type0) << endl;
			exit(1);
		}
		/*if(type0 <= 99){
			type = trans[type0];
		}else if(type0 == 201){
			type = 102;
		}*/
		/* その他の情報 */
		convint id = mesh.element[i].getID();
		id++;/* 1始まりに直す */
		vector<convint> inode;
		mesh.element[i].getNodes(inode);
		int n_size = mesh.element[i].getNodeNum();
		/* 1始まりに直す */
		for(int kk = 0 ; kk < n_size ; kk++){
			inode[kk]++;
		}
		int mat = mesh.element[i].getMat();
		/* 六面体 */
		if(type == 308){
			fp << setw(8) << right << id << setw(8) << right << type <<  setw(8) << right << mat;
			fp << setw(8) << right <<inode[0] << setw(8) << right <<inode[1] << setw(8) << right <<inode[2] << setw(8) << right <<inode[3] << setw(8) << right <<inode[4];
			fp << setw(8) << right <<inode[5] << setw(8) << right <<inode[6] << setw(8) << right <<inode[7] << endl;
		/* プリズム(五面体) */
		}else if(type == 306){
			fp << setw(8) << right << id << setw(8) << right << type <<  setw(8) << right << mat;
			fp << setw(8) << right <<inode[0] << setw(8) << right <<inode[1] << setw(8) << right <<inode[2] << setw(8) << right <<inode[3] << setw(8) << right <<inode[4];
			fp << setw(8) << right <<inode[5] << endl;
		/* ピラミッド(五面体) */
		}else if(type == 305){
			int type00X = 306;
			convint tmp_inode[6];/* 振りなおしてプリズム化する */
			tmp_inode[0] = inode[4];
			tmp_inode[1] = inode[1];
			tmp_inode[2] = inode[0];
			tmp_inode[3] = inode[4];
			tmp_inode[4] = inode[2];
			tmp_inode[5] = inode[3];
			fp << setw(8) << right << id << setw(8) << right << type00X <<  setw(8) << right << mat;
			fp << setw(8) << right <<tmp_inode[0] << setw(8) << right <<tmp_inode[1] << setw(8) << right <<tmp_inode[2] << setw(8) << right <<tmp_inode[3] << setw(8) << right <<tmp_inode[4];
			fp << setw(8) << right <<tmp_inode[5] << endl;
		/* 四面体 */
		}else if(type == 304){
			fp << setw(8) << right << id << setw(8) << right << type <<  setw(8) << right << mat;
			fp << setw(8) << right <<inode[0] << setw(8) << right <<inode[1] << setw(8) << right <<inode[2] << setw(8) << right <<inode[3] << endl;
		/* 四角形 */
		}else if(type == 204){
			fp << setw(8) << right << id << setw(8) << right << type <<  setw(8) << right << mat;
			fp << setw(8) << right <<inode[0] << setw(8) << right <<inode[1] << setw(8) << right <<inode[2] << setw(8) << right <<inode[3] << endl;
		/* 三角形 */
		}else if(type == 203){
			fp << setw(8) << right << id << setw(8) << right << type <<  setw(8) << right << mat;
			fp << setw(8) << right <<inode[0] << setw(8) << right <<inode[1] << setw(8) << right <<inode[2] <<  endl;
		/* 線要素 */
		}else if(type == 102){
			fp << setw(8) << right << id << setw(8) << right << type <<  setw(8) << right << mat;
			fp << setw(8) << right <<inode[0] << setw(8) << right <<inode[1] << endl;
		}else{
			cout << "error in read elemt!!!!!!!!!!!"<<endl;
		}
	}
	fp << "   -1" << endl;
	fp.close();
}



/**/
};
};


