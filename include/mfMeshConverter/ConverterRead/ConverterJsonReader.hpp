/**
* @file ConverterJsonReader.hpp
* @brief functions for converter .exe mode
*
*/
#ifndef DEF_READ_JSON_CONVERTER_PROCESS_DEF
#define DEF_READ_JSON_CONVERTER_PROCESS_DEF

#include "../thirdparty/json.hpp"
#include "../ConvMeshCommon.hpp"
#include "../Converter/MeshConverter.hpp"


namespace JPMRspace{
namespace MeshConvLib{

/* 節点などの記述モード。falseでゼロ始まり。デフォルトは１始まり */
static bool READ_ID_MODE = true;

using convjson = nlohmann::json;

/* 節点読み取りモード変更 */
void renumber_mode(convjson& modify);

/* スケーリング変更 */
void scaling_read(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 2Dと3Dで分けてリナンバ */
void renumber_2D(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 重複判定距離の変更 */
void set_tolera(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 最小の辺の長さを表示 */
void min_edge_length(ConvMesh& mesh, MeshConverter& converter, convjson& modify);

/*------------------------------------------------------*/
/*------------------------------------------------------*/

/* 要素削除（要素タイプ指定） */
void del_ele_type(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（1D） */
void del_ele_1D(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（2D） */
void del_ele_2D(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（ID指定） */
void del_ele_ID(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（xyzで範囲指定） */
void del_ele_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（rtzで範囲指定） */
void del_ele_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（材料で範囲指定） */
void del_ele_mat(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（材料とxyz範囲で範囲指定） */
void del_ele_mat_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素削除（材料とrtz範囲で範囲指定） */
void del_ele_mat_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);

/*------------------------------------------------------*/
/*------------------------------------------------------*/

/* 節点移動（xyzで範囲指定） */
void node_move_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（rtzで範囲指定） */
void node_move_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（指定したIDの節点を平行移動） */
void node_move_id(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（範囲を指定して線形補間、xyzで範囲指定） */
void node_move2_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（範囲を指定して線形補間、rtzで範囲指定） */
void node_move2_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
void node_move3_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、rtzで範囲指定） */
void node_move3_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
void node_move3_mat_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);
/* 節点移動（指定した材料範囲に含まれている節点に対して、AXISで指定した値[IMIN, IMAX]の範囲で、範囲を指定して線形補間、xyzで範囲指定） */
void node_move3_mat_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify, int dir);


/*------------------------------------------------------*/
/*------------------------------------------------------*/

/* 材料ID変更・材料IDの単独指定 */
void mat_change_singleID(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・材料IDの範囲指定 */
void mat_change_interval(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・xyzで範囲指定 */
void mat_change_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・rtzで範囲指定 */
void mat_change_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・材料ID範囲指定・xyzで範囲指定 */
void mat_change_xyz2(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・材料ID範囲指定・rtzで範囲指定 */
void mat_change_rtz2(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・材料ID加算・xyzで範囲指定 */
void mat_change_plus_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・材料ID加算・rtzで範囲指定 */
void mat_change_plus_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・要素IDで指定 */
void mat_change_ele(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・指定した材料IDの要素について、材料IDを自身の要素ID+INCに変更する */
void mat_change_ele_id_to_mat(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料ID変更・隣接一括 */
void mat_change_neighbour_eles(ConvMesh& mesh, MeshConverter& converter, convjson& modify);


/*------------------------------------------------------*/
/*------------------------------------------------------*/

/* 線要素追加・座標指定 */
void line_add(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 線要素追加・座標指定・途中全部作成・複数指定 */
void line_add_multi(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 線要素追加・元になる節点指定 */
void line_add_node(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 線要素追加・元になる節点指定・途中全部作成 */
void line_add_node2(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 線要素追加・元になる節点指定・途中全部作成・複数指定 */
void line_add_node2_multi(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 線要素追加・円弧 */
void line_add_R(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 節点作成 */
void node_make(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 六面体作成 */
void make_hex_ele(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 面要素作成 */
void surf_add(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 面要素作成・複数ver */
void surf_add_multi(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 面要素作成・半径指定 */
void surf_add_R(ConvMesh& mesh, MeshConverter& converter, convjson& modify);


/*------------------------------------------------------*/
/*------------------------------------------------------*/

/* 回転移動 */
void mesh_rot_move_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 回転移動・rtz指定 */
void mesh_rot_move_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 鏡面移動 */
void mesh_mirror_move_xyz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 鏡面移動・rtz指定 */
void mesh_mirror_move_rtz(ConvMesh& mesh, MeshConverter& converter, convjson& modify);

/* 回転コピー */
void mesh_rot_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 鏡面コピー */
void mesh_mirror_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 軸指定鏡面コピー */
void mesh_rotmirror_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 軸指定鏡面コピー+回転コピー */
void mesh_mirror_rot_multi_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 平行移動コピー */
void mesh_move_copy(ConvMesh& mesh, MeshConverter& converter, convjson& modify);


/*------------------------------------------------------*/
/*------------------------------------------------------*/

/* 要素面積書き出し */
void mesh_ele_area_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素面積書き出し・材料IDごと */
void mesh_ele_area_mat_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 材料番号の要素を抜出して、IDをファイルに書き出す */
void mesh_eleids_in_mat_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した範囲の要素IDを抜出して、IDをファイルに書き出す /xyz */
void mesh_ele_output_range_xyz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した範囲の要素IDを抜出して、IDをファイルに書き出す/ rtz */
void mesh_ele_output_range_rtz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した範囲の節点をサーチし、指定した座標値をDをファイルに書き出す / xyz */
void mesh_node_output_range_xyz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した範囲の節点をサーチし、指定した座標値をDをファイルに書き出す / rtz */
void mesh_node_output_range_rtz_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/*　要素重心をファイルに書き出す  */
void mesh_ele_grav_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/*　材料IDごとの要素たちの重心を書き出す  */
void mesh_ele_grav_sum_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素体積書き出し */
void mesh_ele_volume_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 要素体積書き出し・材料IDごと */
void mesh_ele_volume_mat_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した材料の要素をサーチし、その回転対称位置にある要素とのペアを調べる */
void mesh_rot_search_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べる */
void mesh_mirror_search_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べる */
void mesh_rot_and_mirror_search_print(ConvMesh& mesh, MeshConverter& converter, convjson& modify);


/*------------------------------------------------------*/
/*------------------------------------------------------*/


/* 2D to 3D */
void mesh_2d_to_3d(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* メッシュ結合 */
void mesh_connect(ConvMesh& mesh, MeshConverter& converter, convjson& modify);

/* 六面体の節点ローカルID変更 */
void mesh_hex_local_change(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 六面体の節点ローカルIDを自分で指定して変更 */
void mesh_hex_local_change_self(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 2次元面要素の節点並びを、面積が正になるように並び替える */
void mesh_2d_local_changer(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 2次元面要素の四角形を全て三角形に */
void mesh_sq_to_tri(ConvMesh& mesh, MeshConverter& converter, convjson& modify);
/* 3次元要素のプリズムを全て四面体に */
void mesh_pri_to_tet(ConvMesh& mesh, MeshConverter& converter, convjson& modify);


/* end of namespace*/
};
};



#endif

