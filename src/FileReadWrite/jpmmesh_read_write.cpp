/**
* @file jpmmesh_read_write.cpp
* @brief jpmmesh_read_write (original format)
*
*/

#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>

namespace JPMRspace{
namespace MeshConvLib{



/*
//===============================================================
// ○ 独自形式データ読み取り
//===============================================================*/
/** 
 * @brief original file read (without edge data)
*/
void ConvMeshFileReadWrite::readJPMMESH(const string& filename, ConvMesh& mesh, vector<vector<convint>>& Ele_neighbour){
#ifdef PRINT_CONV_INFO
    cout << "start reading JPMMesh data from file" << endl;
#endif
    mesh.info_maked = false;

    /* ファイルオープン*/
    std::fstream fp_in(filename, std::ios::in);
    if( fp_in.fail() == true ){
        cout << ("can not open (readJPMMesh") << endl;;
    }
    mesh.node.clear();
    mesh.edge.clear();
    mesh.element.clear();

    string str;
    /* 節点数読み取り */
    fp_in >> str;
    convint n_size;
    fp_in >> n_size;
    mesh.node.resize(n_size);
    for(convint nn=0; nn < n_size; nn++){
        convint id;
        double x, y, z;
        fp_in >> id >> x >> y >> z;
        ConvNode N_temp;
        N_temp.setID(id);/* IDをゼロ始まりに修正 */
        N_temp.set(x, y, z);
        mesh.node[nn] = std::move(N_temp);
    }

    /* 要素IDmap */
    map<int, ConvEType> trans;
    trans[101] = ConvEType::Line;
    trans[201] = ConvEType::Tri;
    trans[202] = ConvEType::Quad;
    trans[203] = ConvEType::Tri2;
    trans[204] = ConvEType::Quad2;
    trans[301] = ConvEType::Tetra;
    trans[302] = ConvEType::Prizm;
    trans[303] = ConvEType::Hex;
    trans[309] = ConvEType::Pyramid;

    /* 要素読み取り */
    fp_in >> str;
    convint ele_size;
    fp_in >> ele_size;
    mesh.element.resize(ele_size);
    for(convint ee=0; ee < ele_size; ee++){
        string dummy;
        convint id;
        int type;
        int mat;
        convint inode[20];
        fp_in >> dummy >> id >> type >> mat;
        ConvEType e_type = trans[type];

        ConvElement etmp;
        etmp.setID(id);
        etmp.setType(e_type);
        int n_num = etmp.getNodeNum();
        for(int n = 0 ; n < n_num; n++){
            fp_in >> inode[n];
        }
        etmp.setNodes(inode);
        etmp.setMat(mat);
        mesh.element[ee] = std::move(etmp);
    }

    /* 要素隣接読み取り */
    fp_in >> str;
    convint e_size2;
    fp_in >> e_size2;
    Ele_neighbour.resize(e_size2);
    for(convint ee=0; ee < e_size2; ee++){
        string dummy;
        convint id;
        int type;
        int num;
        fp_in >> dummy >> id >> type >> num;

        Ele_neighbour[ee].resize(num);
        for(int n = 0; n < num; n++){
            convint nei_ele;
            fp_in >> dummy >> nei_ele;
            Ele_neighbour[ee][n] = nei_ele;
        }
    }

    /* 境界節点読み取り */
    fp_in >> str;
    convint bound_n;
    fp_in >> bound_n;
    mesh.boundNodeList.resize(bound_n);
    for(convint nn = 0; nn < bound_n; nn++){
        fp_in >> mesh.boundNodeList[nn];
    }
    fp_in.close();

    /*節点数、要素数格納*/
#ifdef PRINT_CONV_INFO
    cout << "** number of nodes    =" << n_size << endl;
    cout << "** number of Elements =" << ele_size << endl;
    cout << "read JPMMesh end!!!!!"<<endl;
#endif
    return;


}


/*
//===============================================================
// ○ 独自形式データ書き出し
//===============================================================*/
/** 
 * @brief original file write (without edge data)
*/
void ConvMeshFileReadWrite::writeJPMMESH(const string& filename, ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
    cout << "** start write JPMMesh data" << endl;
#endif
    /* ファイルオープン*/
    std::fstream fp(filename.c_str(), std::ios::out);
    if( fp.fail() ){
        cout << ("can not open (write JPMMesh)") << endl;;
    }
    const convint n_size = mesh.node.size();
    const convint e_size = mesh.element.size();

    /* 節点情報書き出し */
    fp << "Node" << endl;
    fp << n_size << endl;
    for(convint i = 0 ; i < n_size ; i++){
        convint id;
        double xx, yy, zz;
        id = mesh.node[i].getID();
        xx = mesh.node[i].getX();
        yy = mesh.node[i].getY();
        zz = mesh.node[i].getZ();
        fp <<  id << " " << std::setprecision(25) << scientific <<xx << " " << std::setprecision(25) << scientific <<yy << " " << std::setprecision(25) << scientific <<zz << endl;
    }

    /* 要素IDは次元毎にゼロ始まりでリナンバ */
    vector<convint> new_ELE_IDs;
    new_ELE_IDs.resize(e_size);
    convint id_3d=0;
    convint id_2d=0;
    convint id_1d=0;
    for(convint i = 0 ; i < e_size ; i++){
        bool is3D = mesh.element[i].is3D();
        bool is1D = mesh.element[i].is1D();
        /* 3D要素のID */
        if(is3D){
            new_ELE_IDs[i] = id_3d;
            id_3d++;;
        /* 1D=線要素のID */
        }else if(is1D){
            new_ELE_IDs[i] = id_1d;
            id_1d++;
        /* 2D要素のID */
        }else{
            new_ELE_IDs[i] = id_2d;
            id_2d++;
        }
    }

    /* 2D要素に３Dの辺情報を設定 */
    //mesh.makeEdge_2D_from3D();

#ifdef PRINT_CONV_INFO
    cout << "3D : " << id_3d << endl;
    cout << "2D : " << id_2d << endl;
    cout << "1D : " << id_1d << endl;
#endif

    /* 要素IDmap */
    map<ConvEType, int> trans;
    trans[ConvEType::Line] = 101;
    trans[ConvEType::Tri] = 201;
    trans[ConvEType::Quad] = 202;
    trans[ConvEType::Tri2] = 203;
    trans[ConvEType::Quad2] = 204;
    trans[ConvEType::Tetra] = 301;    
    trans[ConvEType::Prizm] = 302;
    trans[ConvEType::Hex] = 303;
    trans[ConvEType::Pyramid] = 309;

    /* 要素書き出し */
    fp << "Element" << endl;
    fp << e_size << endl;
    fp << "3D " << id_3d << endl;
    fp << "2D " << id_2d << endl;
    fp << "1D " << id_1d << endl;
    for(int i = 0 ; i < e_size ; i++){
        convint id = mesh.element[i].getID();
        ConvEType type = mesh.element[i].getType();
        int mat = mesh.element[i].getMat();
        convint n[50];
        mesh.element[i].getNodes(n);
        /* まず基本情報 */
        fp << "Info: " << new_ELE_IDs[i] << " " << trans[type] << " " << mat << endl;

        /* 節点書き出し(要素タイプによって数を変える) */
        /* 六面体 */
        if(type == ConvEType::Hex){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << " "<< n[4] << " "<< n[5] << " "<< n[6] << " "<< n[7] << endl;
        /* プリズム(五面体) */
        }else if(type == ConvEType::Prizm){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << " "<< n[4] << " "<< n[5] << endl;
        /* ピラミッド(五面体) */
        }else if(type == ConvEType::Pyramid){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << " "<< n[4] << endl;
        /* 四面体 */
        }else if(type == ConvEType::Tetra){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << endl;
        /* 四角形 */
        }else if(type == ConvEType::Quad){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << endl;
        /* 三角形 */
        }else if(type == ConvEType::Tri){
            fp << n[0] << " "<< n[1] << " "<< n[2] << endl;
        /* 四角形2 */
        }else if(type == ConvEType::Quad2){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << " "<< n[4] << " "<< n[5] << " "<< n[6] << " "<< n[7] << endl;
        /* 三角形2 */
        }else if(type == ConvEType::Tri2){
            fp << n[0] << " "<< n[1] << " "<< n[2] << " "<< n[3] << " "<< n[4] << " "<< n[5] << endl;
        /* 線要素 */
        }else if(type == ConvEType::Line){
            fp << n[0] << " "<< n[1] << endl;
        }
    }


    /* 全隣接要素情報書き出し（2Dは辺、3Dは面を共有する） */
    vector<vector<convint>> Ele_neighbour;

    bool is_3d_include = (id_3d > 0);
    if(is_3d_include){
        mesh.make3D_FullNeighbour(Ele_neighbour);
    }else{
        mesh.make2D_FullNeighbour(Ele_neighbour);
    }
    fp << "NeighbourElements"<<endl;
    fp << e_size << endl;
    for(convint i = 0 ; i < e_size ; i++){
        convint id = mesh.element[i].getID();
        ConvEType type = mesh.element[i].getType();
        const int the_size = Ele_neighbour[i].size();
        fp << "ELE: " <<new_ELE_IDs[i] << " " << trans[type] << " " << the_size << endl;
        if(the_size != 0){
            convint nei_ele_id;
            ConvEType nei_type;
            for(int j = 0 ; j < the_size-1 ; j++){
                nei_ele_id = Ele_neighbour[i][j];
                nei_type = mesh.element[nei_ele_id].getType();
                fp << trans[nei_type] << " " << new_ELE_IDs[nei_ele_id] << endl;
            }
            nei_ele_id = Ele_neighbour[i][the_size-1];
            nei_type = mesh.element[nei_ele_id].getType();
            fp << trans[nei_type] << " " << new_ELE_IDs[nei_ele_id] << endl;
        }
    }
    Ele_neighbour.clear();

    /* 境界上の節点書き出し */
    convint bound_n_size;
    if(is_3d_include){
        bound_n_size = mesh.boundNodeList.size();
    }else{
        bound_n_size = mesh.boundNodeList2D.size();
    }
    fp << "BoundNodeList"<<endl;
    fp << bound_n_size << endl;
    for(int i = 0 ; i < bound_n_size ; i++){
        convint n_id;
        if(is_3d_include){
            n_id = mesh.boundNodeList[i];
        }else{
            n_id = mesh.boundNodeList2D[i];
        }
        fp << n_id << endl;
    }

    fp << "MinimumEdge" << endl;
    if(is_3d_include){
        fp << mesh.minumum_edge_L_3D << endl;
    }else{
        fp << mesh.minumum_edge_L_2D << endl;
    }
    

    fp.close();
}



/**/
};
};


