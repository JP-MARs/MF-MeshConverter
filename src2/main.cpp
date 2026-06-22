/* これがdefineされていたら、手入力形式になる */
#define FOR_OPEN_MODE

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <exception>

using namespace std;

#include <mfMeshConverter/ConvMeshCommon.hpp>
#include <mfMeshConverter/Converter/MeshConverter.hpp>
#include <mfMeshConverter/FileReadWrite/ConvMeshFileReadWrite.hpp>
#include <mfMeshConverter/ConverterRead/ConverterJsonReader.hpp>

using namespace JPMRspace::MeshConvLib;

#define IS_WINDOWS_SISTEM
#ifdef IS_WINDOWS_SISTEM
#ifdef _DEBUG
#pragma comment(lib, "libMFMeshConverter_Deb.lib")
#else
#pragma comment(lib, "libMFMeshConverter.lib")
#endif
#endif


/* プロトタイプ宣言 */
int mesh_modify(int mode3, const string& filename, ConvMesh& mesh);

/* メイン */
int main(int argc, char* argv[]){

	/* メッシュデータ定義 */
	ConvMesh mesh;

	/*-------------------------------------*/
	/*-------------------------------------*/
	/*-------------------------------------*/
	/*-------------------------------------*/
	/*-------------------------------------*/
	/* 自動モード・変換コマンドあり */
	if(argc == 4){
		const string input_file = argv[1];
		const string command_file = argv[2];
		const string output_file = argv[3];

		cout << input_file << endl;
		cout << command_file << endl;
		cout << output_file << endl;

		try{
			ConvMeshFileReadWrite::read_mesh(input_file, mesh);
			int fin = mesh_modify(2, command_file, mesh);
			if(fin == 1){
				return 1;
			}
			ConvMeshFileReadWrite::write_mesh(output_file, mesh);
		}catch(...){
			cout << "error...."<<endl;
			return 1;
		}
	/*-------------------------------------*/
	/*-------------------------------------*/
	/*-------------------------------------*/
	/* 自動モード・変換コマンドなし */
	}else if(argc == 3){
		const string input_file = argv[1];
		const string output_file = argv[2];

		cout << input_file << endl;
		cout << output_file << endl;

		try{
			ConvMeshFileReadWrite::read_mesh(input_file, mesh);
			ConvMeshFileReadWrite::write_mesh(output_file, mesh);
		}catch(...){
			cout << "error...."<<endl;
			return 1;
		}
	}else{
	/*-------------------------------------*/
	/*-------------------------------------*/
	/*-------------------------------------*/
	/* 手動打ち込みモード */
		/* メッシュ読み取り */
		cout << "type file name (with file extension)" << endl;
		string filename1;
		cin >> filename1;

		/* 書き出しモード */
		cout << "type output file name (with file extension)" << endl;
		string filename2;
		cin >> filename2;

		/* メッシュ修正をするか */
		cout << "Modify Mesh ? : yes=1 or 2, no=0" << endl;
		cout << " for [yes(1)], prepare [meshmodi.json] file " << endl;
		cout << " for [yes(2)], control filename (json format) will be asked " << endl;
		cout << endl;
		int mode3;
		string str;
		cin >> str;
		mode3 = stoi(str);
		string filename3;
		if(mode3 == 2) {
			cout << "Input control file name " << endl;
			cin >> filename3;
		}

		/* 実際に読み取り */
		ConvMeshFileReadWrite::read_mesh(filename1, mesh);
		cout << endl;

		/*---------------------------------------------------*/
		/*---------------------------------------------------*/
		/* メッシュ修正の場合、作業開始 */
		if(mode3 != 0) {
			int fin = mesh_modify(mode3, filename3, mesh);
			if(fin == 1){
				return 1;
			}
			cout << endl;
		}
		/*---------------------------------------------------*/
		/*---------------------------------------------------*/

		/* 実際に書き出し */
		ConvMeshFileReadWrite::write_mesh(filename2, mesh);
	}

	return 0;
}


/**/
/**/
/**/
/* メッシュ修正 */
/**/
int mesh_modify(int mode3, const string& filename, ConvMesh& mesh){
	/* まずファイルを読み取る */
	string filename_f;
	if(mode3 == 1){
		filename_f = "meshmodi.json";
	}else{
		filename_f = filename;
	}
	cout << "read controll file" << endl;
	/* ファイルオープン*/
	fstream fp_in(filename_f, ios::in);
	if( fp_in.fail() == true ){
		cout << ("can not open control file") << endl;;
		exit(1);
	}
	/* JSON化 */
	nlohmann::json modify;
	fp_in >> modify;
	fp_in.close();

	MeshConverter converter;

	const size_t num = modify.size();
	cout << "Number of Converter Commands : " << num << endl;
	/* 変換ループ開始 */
	/* JSONを１つずつ読んでいく */
	for(size_t aaa = 0 ; aaa < num ; aaa++){
		try{
			//string id = std::to_string(aaa);
			string command = modify[aaa]["name"].get<string>();
			cout << "Converter Command " << aaa << " : " << command << endl;	

			/* 節点モードの変更 */
			if(command == "NUMBER_ID"){
				int mode = modify[aaa]["mode"].get<int>();
				if(mode == 0){
					READ_ID_MODE = false;
				}else{
					READ_ID_MODE = true;
				}

			/* メッシュ情報作成 */
			}else if(command == "MAKE_ELEMINFO"){
				mesh.makeMeshInfo();

			/* スケーリング変更 */
			}else if(command == "SCALE"){
				scaling_read(mesh, converter, modify[aaa]);

			/* 2Dと3Dで分けてリナンバ */
			}else if(command == "RENUMBER2D"){
				renumber_2D(mesh, converter, modify[aaa]);

			/*重複節点判定のときの判定距離の変更 */
			}else if(command == "TOLERA"){
				set_tolera(mesh, converter, modify[aaa]);

			/*最少の辺長を計算して画面出力 */
			}else if(command == "EDGELENGTH"){
				min_edge_length(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*指定した要素タイプの要素を削除 */
			}else if(command == "DELELETYPE"){
				del_ele_type(mesh, converter, modify[aaa]);

			/*1Dの要素を削除する */
			}else if(command == "DELELE1D"){
				del_ele_1D(mesh, converter, modify[aaa]);

			/*2Dの要素を削除する */
			}else if(command == "DELELE2D"){
				del_ele_2D(mesh, converter, modify[aaa]);

			/*指定されたID範囲の要素を削除 */
			}else if(command == "DELID"){
				del_ele_ID(mesh, converter, modify[aaa]);

			/*指定されたX,Y,Z範囲の要素を削除 */
			}else if(command == "DELXYZ"){
				del_ele_xyz(mesh, converter, modify[aaa]);

			/*指定されたR,Th,Z範囲の要素を削除 */
			}else if(command == "DELRTZ"){
				del_ele_rtz(mesh, converter, modify[aaa]);

			/*指定する範囲の物質番号をもつ要素を削除 */
			}else if(command == "DELELEMAT"){
				del_ele_mat(mesh, converter, modify[aaa]);

			/*指定する範囲の中で指定する範囲の物質番号をもつ要素を削除 */
			}else if(command == "DELELEMATXYZ"){
				del_ele_mat_xyz(mesh, converter, modify[aaa]);

			/*指定する範囲の中で指定する範囲の物質番号をもつ要素を削除 */
			}else if(command == "DELELEMATRTZ"){
				del_ele_mat_rtz(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*該当範囲の節点をＸ方向に移動 */
			}else if(command == "NODMVX"){
				node_move_xyz(mesh, converter, modify[aaa], 0);

			/*該当範囲の節点をY方向に移動 */
			}else if(command == "NODMVY"){
				node_move_xyz(mesh, converter, modify[aaa], 1);

			/*該当範囲の節点をZ方向に移動 */
			}else if(command == "NODMVZ"){
				node_move_xyz(mesh, converter, modify[aaa], 2);

			/*該当範囲の節点をR方向に移動 */
			}else if(command == "NODMVR"){
				node_move_rtz(mesh, converter, modify[aaa], 0);

			/*該当範囲の節点をT方向に移動 */
			}else if(command == "NODMVT"){
				node_move_rtz(mesh, converter, modify[aaa], 1);

			/*該当範囲の節点をZ方向に移動 */
			}else if(command == "NODMVTZ"){
				node_move_rtz(mesh, converter, modify[aaa], 2);

			/*該当範囲の節点をＸ方向に移動 */
			}else if(command == "NODMVFIXX"){
				node_move_xyz(mesh, converter, modify[aaa], 4);

			/*該当範囲の節点をY方向に移動 */
			}else if(command == "NODMVFIXY"){
				node_move_xyz(mesh, converter, modify[aaa], 5);

			/*該当範囲の節点をZ方向に移動 */
			}else if(command == "NODMVFIXZ"){
				node_move_xyz(mesh, converter, modify[aaa], 6);

			/*該当範囲の節点をR方向に移動 */
			}else if(command == "NODMVFIXR"){
				node_move_rtz(mesh, converter, modify[aaa], 4);

			/*該当範囲の節点をT方向に移動 */
			}else if(command == "NODMVFIXT"){
				node_move_rtz(mesh, converter, modify[aaa], 5);

			/*該当範囲の節点をZ方向に移動 */
			}else if(command == "NODMVFIXTZ"){
				node_move_rtz(mesh, converter, modify[aaa], 6);

			/*指定したIDの節点を平行移動 */
			}else if(command == "NDMVNXYZ"){
				node_move_id(mesh, converter, modify[aaa], 0);

			/*指定したIDの節点を平行移動 */
			}else if(command == "NDMVNRTZ"){
				node_move_id(mesh, converter, modify[aaa], 1);

			/*節点移動（範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVX2"){
				node_move2_xyz(mesh, converter, modify[aaa], 0);

			/*節点移動（範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVY2"){
				node_move2_xyz(mesh, converter, modify[aaa], 1);

			/*節点移動（範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVZ2"){
				node_move2_xyz(mesh, converter, modify[aaa], 2);

			/*節点移動（範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVR2"){
				node_move2_rtz(mesh, converter, modify[aaa], 0);

			/*節点移動（範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVT2"){
				node_move2_rtz(mesh, converter, modify[aaa], 1);

			/*節点移動（範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVTZ2"){
				node_move2_rtz(mesh, converter, modify[aaa], 2);

			/*節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVX3"){
				 node_move3_xyz(mesh, converter, modify[aaa], 0);

			/*節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVY3"){
				 node_move3_xyz(mesh, converter, modify[aaa], 1);

			/*節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVZ3"){
				 node_move3_xyz(mesh, converter, modify[aaa], 2);

			/*節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVR3"){
				 node_move3_rtz(mesh, converter, modify[aaa], 0);

			/*節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVT3"){
				 node_move3_rtz(mesh, converter, modify[aaa], 1);

			/*節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
			}else if(command == "NDMVTZ3"){
				 node_move3_rtz(mesh, converter, modify[aaa], 2);

			/*節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）  */
			}else if(command == "NDMVX3MAT"){
				 node_move3_mat_xyz(mesh, converter, modify[aaa], 0);

			/*節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）  */
			}else if(command == "NDMVY3MAT"){
				 node_move3_mat_xyz(mesh, converter, modify[aaa], 1);

			/*節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）  */
			}else if(command == "NDMVZ3MAT"){
				 node_move3_mat_xyz(mesh, converter, modify[aaa], 2);

			/*節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）  */
			}else if(command == "NDMVR3MAT"){
				 node_move3_mat_rtz(mesh, converter, modify[aaa], 0);

			/*節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）  */
			}else if(command == "NDMVT3MAT"){
				 node_move3_mat_rtz(mesh, converter, modify[aaa], 1);

			/*節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定）  */
			}else if(command == "NDMVTZ3MAT"){
				 node_move3_mat_rtz(mesh, converter, modify[aaa], 2);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*材料ID変更・材料IDの単独指 */
			}else if(command == "MATC"){
				 mat_change_singleID(mesh, converter, modify[aaa]);

			/*材料ID変更・材料IDの範囲指定 */
			}else if(command == "MATC2"){
				 mat_change_interval(mesh, converter, modify[aaa]);

			/*材料ID変更・xyzで範囲指定 */
			}else if(command == "MATCNX"){
				 mat_change_xyz(mesh, converter, modify[aaa]);

			/*材料ID変更・rtzで範囲指定 */
			}else if(command == "MATCNR"){
				 mat_change_rtz(mesh, converter, modify[aaa]);

			/*材料ID変更・xyzで範囲指定 */
			}else if(command == "MATCNX2"){
				 mat_change_xyz2(mesh, converter, modify[aaa]);

			/*材料ID変更・rtzで範囲指定 */
			}else if(command == "MATCNR2"){
				 mat_change_rtz2(mesh, converter, modify[aaa]);

			/*材料ID変更・xyzで範囲指定 */
			}else if(command == "MATPLUSNX"){
				 mat_change_plus_xyz(mesh, converter, modify[aaa]);

			/*材料ID変更・rtzで範囲指定 */
			}else if(command == "MATPLUSNR"){
				 mat_change_plus_rtz(mesh, converter, modify[aaa]);

			/*材料ID変更・要素IDで指定 */
			}else if(command == "MATCEL"){
				 mat_change_ele(mesh, converter, modify[aaa]);

			/*材料ID変更・指定した材料IDの要素について、材料IDを自身の要素ID+INCに変更する */
			}else if(command == "MATTOID"){
				 mat_change_ele_id_to_mat(mesh, converter, modify[aaa]);

			/*材料ID変更・隣接一括 */
			}else if(command == "MATCNEGHB"){
				 mat_change_neighbour_eles(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*線要素追加・座標指定 */
			}else if(command == "LINEADD"){
				 line_add(mesh, converter, modify[aaa]);

			/*指定した節点IDを始点・終点にする線要素 */
			}else if(command == "LINEADD2"){
				 line_add_node(mesh, converter, modify[aaa]);

			/*指定した始点節点・終点節点の間にある辺を抽出し、線要素として作成 */
			}else if(command == "LINEADDM"){
				 line_add_node2(mesh, converter, modify[aaa]);

			/*指定した始点節点・終点節点の間にある辺を抽出し、線要素として作成*/
			}else if(command == "LINEADDMC"){
				 line_add_node2_multi(mesh, converter, modify[aaa]);

			/*線要素追加・座標指定・途中全部作成・複数指定*/
			}else if(command == "LINEADDMP"){
				 line_add_multi(mesh, converter, modify[aaa]);

			/*線要素追加・円弧*/
			}else if(command == "LINEADDR"){
				 line_add_R(mesh, converter, modify[aaa]);

			/*節点作成*/
			}else if(command == "NODMAKE"){
				 node_make(mesh, converter, modify[aaa]);

			/*六面体作成*/
			}else if(command == "HEXMK"){
				 make_hex_ele(mesh, converter, modify[aaa]);

			/*面作成*/
			}else if(command == "SURFADD"){
				 surf_add(mesh, converter, modify[aaa]);

			/*面作成複数*/
			}else if(command == "SURFADD2"){
				 surf_add_multi(mesh, converter, modify[aaa]);

			/*面要素作成・半径指定*/
			}else if(command == "SURFADDR"){
				 surf_add_R(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*指定した範囲内にある節点を回転移動*/
			}else if(command == "ROTXYZ"){
				 mesh_rot_move_xyz(mesh, converter, modify[aaa]);

			/*指定した範囲内にある節点を回転移動*/
			}else if(command == "ROTRTZ"){
				 mesh_rot_move_rtz(mesh, converter, modify[aaa]);

			/*鏡面移動*/
			}else if(command == "MIRRORMVXYZ"){
				 mesh_mirror_move_xyz(mesh, converter, modify[aaa]);

			/*鏡面移動*/
			}else if(command == "MIRRORMVRTZ"){
				 mesh_mirror_move_rtz(mesh, converter, modify[aaa]);

			/*回転コピー*/
			}else if(command == "ROTCOPY"){
				 mesh_rot_copy(mesh, converter, modify[aaa]);

			/*鏡映コピー*/
			}else if(command == "MIRRORCOPY"){
				 mesh_mirror_copy(mesh, converter, modify[aaa]);

			/*軸指定鏡面コピー*/
			}else if(command == "MIRROR_ROT"){
				 mesh_rotmirror_copy(mesh, converter, modify[aaa]);

			/*鏡映コピー(＜DEG＞のAXIS軸に鏡面コピー）したあと、指定した回数、回転コピーする*/
			}else if(command == "MIRROR_ROT_MULTI"){
				 mesh_mirror_rot_multi_copy(mesh, converter, modify[aaa]);

			/*平行移動コピー*/
			}else if(command == "MVCOPY"){
				 mesh_move_copy(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*要素面積書き出し*/
			}else if(command == "CALCAREA"){
				 mesh_ele_area_print(mesh, converter, modify[aaa]);

			/*指定した材料IDの要素の面積を計算し、その合計を計算する（2D要素のみ）*/
			}else if(command == "CALCAREAMAT"){
				 mesh_ele_area_mat_print(mesh, converter, modify[aaa]);

			/*指定した材料番号の要素を抜出して、IDをファイルに書き出す*/
			}else if(command == "ELEOUTPUT"){
				 mesh_eleids_in_mat_print(mesh, converter, modify[aaa]);

			/*指定した範囲の要素IDを抜出して、IDをファイルに書き出す*/
			}else if(command == "ELEOUTPUTPXYZ"){
				 mesh_ele_output_range_xyz_print(mesh, converter, modify[aaa]);

			/*指定した範囲の要素IDを抜出して、IDをファイルに書き出す*/
			}else if(command == "ELEOUTPUTPRTZ"){
				 mesh_ele_output_range_rtz_print(mesh, converter, modify[aaa]);

			/*指定した範囲の節点をサーチし、指定した座標値をNode_Coordinates.csvファイルに書き出す*/
			}else if(command == "NODEOUTPUT"){
				 mesh_node_output_range_xyz_print(mesh, converter, modify[aaa]);

			/*指定した範囲の節点をサーチし、指定した座標値をNode_Coordinates.csvファイルに書き出す*/
			}else if(command == "NODEOUTPUTRTZ"){
				 mesh_node_output_range_rtz_print(mesh, converter, modify[aaa]);

			/*指定した材料IDの要素たちの重心を書き出す*/
			}else if(command == "GRAVOUTPUT"){
				 mesh_ele_grav_print(mesh, converter, modify[aaa]);

			/*材料IDごとの要素たちの重心を書き出す。*/
			}else if(command == "GRAVOUTPUTSUM"){
				 mesh_ele_grav_sum_print(mesh, converter, modify[aaa]);

			/*指定した要素IDの要素の体積を計算*/
			}else if(command == "CALCVOLUME"){
				 mesh_ele_volume_print(mesh, converter, modify[aaa]);

			/*指定した材料IDの要素の体積を計算し、その合計を計算*/
			}else if(command == "CALCVOLUMEMAT"){
				 mesh_ele_volume_mat_print(mesh, converter, modify[aaa]);

			/*指定した材料の要素をサーチし、その回転対称位置にある要素とのペアを調べる*/
			}else if(command == "ROTSEARCH"){
				 mesh_rot_search_print(mesh, converter, modify[aaa]);

			/*指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べる*/
			}else if(command == "MIRRORSEARCH"){
				 mesh_mirror_search_print(mesh, converter, modify[aaa]);

			/*指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べる。さらに、回転対称の位置にある要素もサーチし、ペアを調る*/
			}else if(command == "MIRROR_ROT_SEARCH"){
				 mesh_rot_and_mirror_search_print(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/

			/*2Dメッシュを押し出して3次元化する*/
			}else if(command == "2DTO3D"){
				 mesh_2d_to_3d(mesh, converter, modify[aaa]);

			/*他のメッシュを読み込んで、いまのメッシュに結合する*/
			}else if(command == "ADDMSH"){
				 mesh_connect(mesh, converter, modify[aaa]);

			/* ローカルID順序変更系 */
			}else if(command == "HEXLOCALCN"){
				 mesh_hex_local_change(mesh, converter, modify[aaa]);

			}else if(command == "HEXLOCMOD"){
				 mesh_hex_local_change_self(mesh, converter, modify[aaa]);

			}else if(command == "MODLOCAL2D"){
				 mesh_2d_local_changer(mesh, converter, modify[aaa]);

			/* 分割・結合・変換系 */
			}else if(command == "DIVSQTOTRI"){
				 mesh_sq_to_tri(mesh, converter, modify[aaa]);

			}else if(command == "DIVPRITOTET"){
				 mesh_pri_to_tet(mesh, converter, modify[aaa]);

			/*=========================================================*/
			/*=========================================================*/
			/*=========================================================*/
			/* 不明なコマンドは警告出力 */
			}else{
				cout << "Unknown command: " << command << endl;
			}
		/* エラーキャッチ */
		}catch (...){
			cout << "Error in control file at No." << aaa << endl;
			return 1;
		}
	}
	return 0;
}

