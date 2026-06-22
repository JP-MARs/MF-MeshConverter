/**
* @file vtk_read_write.cpp
* @brief vtk_read_write
*
*/
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>

namespace JPMRspace{
namespace MeshConvLib{



/*
//===============================================================
// ○ VTKデータ読み取り
//===============================================================*/
/** 
 * @brief vtk(legacy) file read
*/
void ConvMeshFileReadWrite::readVTK(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "start reading VTK data from file" << endl;
#endif
    mesh.info_maked = false;

	/* ファイルオープン*/
	std::fstream fp_in(filename, std::ios::in);
	if( fp_in.fail() == true ){
		cout << ("can not open (readVTK") << endl;;
        exit(1);
	}

	string strHead;
	convint icntNode = 0; /*節点数カウンタ*/
	convint icntElmt = 0; /*要素数カウンタ*/
	string yStr, strDummy, strDummy2;
	double x, y, z;

	/*領域予約(処理速度向上の為のものであり、100000を超えても問題無い)*/
    mesh.node.clear();
    mesh.element.clear();
	mesh.node.reserve(100000);
    mesh.element.reserve(100000);

	/* 先頭読み取り */
	getline(fp_in, strDummy);
	getline(fp_in, strDummy);
	getline(fp_in, strDummy);
	getline(fp_in, strDummy);


	/* ノード数読み取り */
	getline(fp_in, strDummy);
	cout << strDummy << endl;
	vector<string> tmp_line;
	tmp_line = ConvMeshFileReadWrite::split(strDummy, ' ');
	icntNode = stoi(tmp_line[1]);

	/* ノード読み取り */
	for(convint i = 0; i < icntNode; i++){
		getline(fp_in, strDummy);
		vector<string> tmp_line0;
		tmp_line0 = ConvMeshFileReadWrite::split(strDummy, ' ');
		x = stod(tmp_line0[0]);
		y = stod(tmp_line0[1]);
		z = stod(tmp_line0[2]);
		ConvNode N_temp;
		N_temp.setID(i);/* IDをゼロ始まりに修正 */
		N_temp.set(x, y, z);
		mesh.node.push_back( std::move(N_temp) );
	}

	/* 要素数read */
	getline(fp_in, strDummy);
	if(strDummy.empty()){
		cout << "null..." << endl;
		getline(fp_in, strDummy);
	}
	tmp_line = ConvMeshFileReadWrite::split(strDummy, ' ');
	icntElmt = stoi(tmp_line[1]);

	/* 要素読み取り */
	vector<vector<convint>> temp_node_data;
	for(convint i = 0; i < icntElmt; i++){
		getline(fp_in, strDummy);
		vector<string> tmp_line0;
		tmp_line0 = ConvMeshFileReadWrite::split(strDummy, ' ');
		/* 要素タイプ取得 */
		int n_size = stoi(tmp_line0[0]);
		/* 節点read */
		vector<convint> tempVec;
		for(int nn=0 ; nn < n_size ; nn++){
			int temp = stoi(tmp_line0[nn+1]);
			tempVec.push_back(temp);
		}
		temp_node_data.push_back(tempVec);
	}

	getline(fp_in, strDummy);
	if(strDummy.empty()){
		cout << "null..." << endl;
		getline(fp_in, strDummy);
	}
	/* 要素タイプを読み取る */
	for(convint i = 0; i < icntElmt; i++){
		getline(fp_in, strDummy);
		vector<string> tmp_line0;
		tmp_line0 = ConvMeshFileReadWrite::split(strDummy, ' ');
		int e_type = stoi(tmp_line0[0]);
		ConvEType type_e;
		if(e_type == 5){
			type_e = ConvEType::Tri;
        }else if(e_type == 22){
            type_e = ConvEType::Tri2;
        }else if(e_type == 9){
            type_e = ConvEType::Quad;
        }else if(e_type == 23){
            type_e = ConvEType::Quad2;
        }else if(e_type == 10){
            type_e = ConvEType::Tetra;
        }else if(e_type == 12){
            type_e = ConvEType::Hex;
        }else if(e_type == 13){
			type_e = type_e = ConvEType::Prizm;
		}else if(e_type == 14){
			type_e = type_e = ConvEType::Pyramid;
		}else if(e_type == 3){
			type_e = type_e = ConvEType::Line;
		}else{
			std::cout << "Element read ) this element type is not supported in this program!!!" << std::endl;
            exit(1);
        }
		ConvElement etmp;
		etmp.setID(i);
		etmp.setType(type_e);
		convint inode[30];
		for(int n = 0 ; n < temp_node_data[i].size(); n++){
			inode[n] = temp_node_data[i][n];
		}
		etmp.setNodes(inode);
		etmp.setMat(1);/* 材料IDは仮セット */
		/*配列に追加*/
		mesh.element.push_back( std::move(etmp) );
	}
	temp_node_data.clear();

	/* 要素の材料ID取得*/
	getline(fp_in, strDummy);
	if(strDummy.empty()){
		cout << "null..." << endl;
		getline(fp_in, strDummy);
	}
	getline(fp_in, strDummy);
	getline(fp_in, strDummy);
	cout << strDummy << endl;
	for(convint i = 0; i < icntElmt; i++){
		getline(fp_in, strDummy);
		vector<string> tmp_line0;
		tmp_line0 = ConvMeshFileReadWrite::split(strDummy, ' ');
		/* 要素タイプ取得 */
		int mat = stoi(tmp_line0[0]);
		mesh.element[i].setMat(mat);
	}
	fp_in.close();

    /* IDの対応関係 */
    if(mesh.save_id_map){
        mesh.node_map.clear();
        mesh.ele_map.clear();
    }

	/*節点数、要素数格納*/
#ifdef PRINT_CONV_INFO
	cout << "** number of nodes    =" << icntNode << endl;
	cout << "** number of Elements =" << icntElmt << endl;
	cout << "read VTK end!!!!!"<<endl;
#endif
	return;


}


/*
//===============================================================
// ○ VTKデータ書き出し
//===============================================================*/
/** 
 * @brief vtk(legacy) file write
*/
void ConvMeshFileReadWrite::writeVTK(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
    cout << "** start write VTK data" << endl;
#endif
    /* ファイルオープン*/
    //FILE *fp = fopen(filename.c_str(), "w");
    std::fstream fp(filename.c_str(), std::ios::out);
    if( fp.fail() ){
        cout << ("can not open (write VTK)") << endl;;
    }
    /* ヘッダ書き出し */
    fp << "# vtk DataFile Version 2.0" << endl;
    fp << "hexahedron" << endl;
    fp << "ASCII" << endl;
    fp << "DATASET UNSTRUCTURED_GRID" << endl;

    const convint n_size = mesh.node.size();
    const convint e_size = mesh.element.size();

    /* 節点情報書き出し */
    fp << "POINTS " << n_size << " float" << endl;
    for(convint i = 0 ; i < n_size ; i++){
        double xx, yy, zz;
        xx = mesh.node[i].getX();
        yy = mesh.node[i].getY();
        zz = mesh.node[i].getZ();
        fp <<  std::setprecision(25) << scientific <<xx << " " << std::setprecision(25) << scientific <<yy << " " << std::setprecision(25) << scientific <<zz << endl;
    }

    /* 要素タイプ数え */
    convint sq_num = 0;
    convint tri_num = 0;
    convint sq_num2 = 0;
    convint tri_num2 = 0;
    convint tet_num = 0;
    convint pri_num = 0;
    convint hex_num = 0;
    convint pyra_num = 0;
    convint line_enum = 0;
    for(convint i = 0 ; i < e_size ; i++){
        const ConvEType type = mesh.element[i].getType();
        if(type == ConvEType::Tri){
            tri_num++;
        }else if(type == ConvEType::Tri2){
            tri_num2++;
        }else if(type == ConvEType::Quad){
            sq_num++;
        }else if(type == ConvEType::Quad2){
            sq_num2++;
        }else if(type == ConvEType::Tetra){
            tet_num++;
        }else if(type == ConvEType::Hex){
            hex_num++;
        }else if(type == ConvEType::Prizm){
            pri_num++;
        }else if(type == ConvEType::Pyramid){
            pyra_num++;
        }else if(type == ConvEType::Line){
            line_enum++;
        }
    }
    /* 要素データの書き出し(要素数＋書き出す情報の数（三角形節点数+番号と、四角形節点数+番号）) */
    fp << "CELLS " << e_size << " " << (tri_num*4)+(tri_num2*7)+(sq_num*5)+(sq_num2*9)+(tet_num*5)+(pri_num*7)+(hex_num*9)+(pyra_num*6)+(line_enum*3) << endl;
    for(convint i = 0 ; i < e_size ; i++){
        const ConvEType type = mesh.element[i].getType();
        convint inode[8];
        if(type == ConvEType::Tri){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            fp << "3 " << inode[0] << " " << inode[1] << " " << inode[2] << endl;
        }else if(type == ConvEType::Tri2){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            inode[4] = mesh.element[i].getNode(4);
            inode[5] = mesh.element[i].getNode(5);
            fp << "6 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << " " << inode[4] << " " << inode[5] << endl;
        }else if(type == ConvEType::Quad){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            fp << "4 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << endl;
        }else if(type == ConvEType::Quad2){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            inode[4] = mesh.element[i].getNode(4);
            inode[5] = mesh.element[i].getNode(5);
            inode[6] = mesh.element[i].getNode(6);
            inode[7] = mesh.element[i].getNode(7);
            fp << "8 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << " " << inode[4] << " " << inode[5]  << " " << inode[6]  << " " << inode[7] << endl;
        }else if(type == ConvEType::Tetra){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            fp << "4 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << endl;
        }else if(type == ConvEType::Hex){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            inode[4] = mesh.element[i].getNode(4);
            inode[5] = mesh.element[i].getNode(5);
            inode[6] = mesh.element[i].getNode(6);
            inode[7] = mesh.element[i].getNode(7);
            fp << "8 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << " " << inode[4] << " " << inode[5]  << " " << inode[6]  << " " << inode[7] << endl;
        }else if(type == ConvEType::Prizm){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            inode[4] = mesh.element[i].getNode(4);
            inode[5] = mesh.element[i].getNode(5);
            fp << "6 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << " " << inode[4] << " " << inode[5] << endl;
        }else if(type == ConvEType::Pyramid){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            inode[2] = mesh.element[i].getNode(2);
            inode[3] = mesh.element[i].getNode(3);
            inode[4] = mesh.element[i].getNode(4);
            fp << "5 " << inode[0] << " " << inode[1] << " " << inode[2]  << " " << inode[3] << " " << inode[4] << endl;
        }else if(type == ConvEType::Line){
            inode[0] = mesh.element[i].getNode(0);
            inode[1] = mesh.element[i].getNode(1);
            fp << "2 " << inode[0] << " " << inode[1] << endl;
        }
    }

    /* 要素タイプの指定 */
    fp << "CELL_TYPES " << e_size << endl;
    for(convint i = 0 ; i < e_size ; i++){
        const ConvEType type = mesh.element[i].getType();
        if(type == ConvEType::Tri){
            fp << 5 << endl;
        }else if(type == ConvEType::Tri2){
            fp << 22 << endl;
        }else if(type == ConvEType::Quad){
            fp << 9 << endl;
        }else if(type == ConvEType::Quad2){
            fp << 23 << endl;
        }else if(type == ConvEType::Tetra){
            fp << 10 << endl;
        }else if(type == ConvEType::Hex){
            fp << 12 << endl;
        }else if(type == ConvEType::Prizm){
            fp << 13 << endl;
        }else if(type == ConvEType::Pyramid){
            fp << 14 << endl;
        }else if(type == ConvEType::Line){
            fp << 3 << endl;
        }
    }
    /* 材料番号の書き出し */
    fp << "CELL_DATA " << e_size << endl;
    fp << "SCALARS Material float" << endl;
    fp << "LOOKUP_TABLE default" << endl;
    for(convint i = 0 ; i < e_size ; i++){
        const int mat = mesh.element[i].getMat();
        fp << mat << endl;
    }
    fp.close();
}



/**/
};
};


