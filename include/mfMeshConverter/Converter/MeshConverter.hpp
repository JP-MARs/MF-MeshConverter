/**
* @file MeshConverter.hpp
* @brief mesh modification functions class
*
*
*/
#ifndef DEF_MCONVMESH_CONVERTER_DEF_00
#define DEF_MCONVMESH_CONVERTER_DEF_00

#include "../ConvMeshCommon.hpp"

namespace JPMRspace{
namespace MeshConvLib{


/*
//=======================================================
// ■ Converter
//=======================================================
// FEMのメッシュの操作クラス
//=======================================================*/
/**
* @class MeshConverter
* @brief mesh modification functions class
*/
class MeshConverter{
private:
	/*--------------*/
	/*---削除系サブルーチン-----------*/
	/* 番号で要素選択 */
	void select_inits(ConvMesh& mesh, vector<bool>& active_node, vector<bool>& active_ele);
	/* 選択要素に対応した節点を選択 */
	void select_finish(ConvMesh& mesh, vector<bool>& active_node, vector<bool>& active_ele);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 積み上げサブルーチン１：積み上げデータ作成 */
	void mesh2D_3D_sub(ConvMesh& mesh, int no_layer, int no_base_mat, int no_layer_data, int no_z_data, int coor, int no_surf, bool dir_mode,
						const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate, const vector<int>& mat_id_2d,
						const vector<int>& from_layer, const vector<int>& to_layer, const vector<vector<int>>& layer_mat,
						const vector<int>& base_mat2D, const vector<int>& base_mat3D, const vector<int>& layer_pos);
	/* 積み上げサブルーチン２：zデータセット */
	void set_layer_z_val(vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_z_data, const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate);
	/* 積み上げサブルーチン２b：θデータセット */
	void set_layer_theta_val(vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_z_data, const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate);
	/* 積み上げサブルーチン３：要素の材料データセット */
	void set_layer_mat(vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_base_mat, int no_layer_data, const vector<int>& mat_id_2d, const vector<int>& from_layer, const vector<int>& to_layer, const vector<vector<int>>& layer_mat);
	/* 面要素をコピー＆セット */
	void set_surf_mat(ConvMesh& mesh, vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_layer, int no_surf, const vector<int>& base_mat2D, const vector<int>& base_mat3D, const vector<int>& layer_pos);
	/* 積み上げサブルーチン４：積み上げ確定 */
	void set_layer_fix(ConvMesh& mesh, int no_layer, vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* メッシュ結合サブルーチン１：材料更新 */
	void add_mesh_mat_mod(ConvMesh& mesh, ConvMesh& mesh2);
	/* メッシュ結合サブルーチン２：重複削除 */
	void add_mesh_connect_and_del_node(ConvMesh& mesh, ConvMesh& mesh2, int surf, double f_val, int knod);
	/*--------------*/
	/*--------------*/
	/*--------------*/
public:
	/* 同一判定の最小誤差 */
	double eps_diff;	

	MeshConverter(){
		eps_diff = 1.0e-9;
	};
	~MeshConverter() =default;
	void setEPS(double x){eps_diff=x;};
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 操作関数たち */
	/*--------------*/
	/* スケーリング */
	void scaling(ConvMesh& mesh, double scale_fac);
	/* 最小の辺長を計算 */
	double calc_shortest_edge_length(ConvMesh& mesh);
	/* 3Dと2D要素が混在しているとき、リナンバして2D要素のIDを末尾に振りなおす */
	void renumbering_3D_and_2Dele(ConvMesh& mesh);

	/*--------------*/
	/*--------------*/
	/* 選択されている要素を削除 */
	void delete_ele(ConvMesh& mesh, vector<bool>& active_node, vector<bool>& active_ele);
	/* 要素全選択 */
	void delete_ele_all(ConvMesh& mesh);
	/* 要素削除・タイプで選択 */
	void delete_ele_type(ConvMesh& mesh, const ConvEType ele_type);
	/* 削除・番号で要素選択 */
	void delete_ele_id(ConvMesh& mesh, const int ele_mat_num, const vector<convint>& selects1, const vector<convint>& selects2);
	/* 削除・要素選択-(XYZ) */
	void delete_ele_xyz(ConvMesh& mesh, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 削除・要素選択-(RθZで指定) */
	void delete_ele_RTHZ(ConvMesh& mesh, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);
	/* 削除・要素選択-材料番号で指定 */
	void delete_ele_MAT(ConvMesh& mesh, int minMatID, int maxMatID);
	/* 削除・要素選択-範囲と材料で指定(XYZ) */
	void delete_ele_MAT_xyz(ConvMesh& mesh, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int minMatID, int maxMatID);
	/* 削除・要素選択-範囲と材料で指定(RθZで指定 */
	void delete_ele_MAT_RTHZ(ConvMesh& mesh, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax, int minMatID, int maxMatID);
	/* 削除・要素選択2D要素削除 */
	void delete_ele_2D(ConvMesh& mesh);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 該当範囲の節点を移動する(0:X, 1:Y, 2:Z,  4:Xの移動位置指定,  5:Yの移動位置指定,  6:Zの移動位置指定) */
	void move_node_xyzdir(ConvMesh& mesh, int move_mode, double move_val, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 該当範囲の節点を移動する(0:T, 1:theta, 2:Z,  4:Rの移動位置指定,  5:Thetaの移動位置指定,  6:Zの移動位置指定) */
	void move_node_rtzdir(ConvMesh& mesh, int move_mode, double move_val, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);
	/* 指定したIDの節点を平行移動する（0:XYZで移動、1:RTZで移動） */
	void move_node_id_xyz(ConvMesh& mesh,int move_mode, int num_ids, convint* target_ids, double* move_1, double* move_2, double* move_3);
	/* 該当範囲の節点のx座標を指定する(XminとXmaxを指定し、あとは線形補間) */
	void move_node_xyz_type2(ConvMesh& mesh, int move_mode, double min_move, double max_move, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 該当範囲の節点のR座標を指定する(RminとRmaxを指定し、あとは線形補間) */
	void move_node_rtz_type2(ConvMesh& mesh, int move_mode, double min_move, double max_move, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);
	/* 該当範囲の節点のxyz座標を指定する(線形補間を計算する軸と座標範囲を指定して移動) */
	void move_node_xyz_type3(ConvMesh& mesh, int move_mode, int axis, double min_move, double max_move, double min_v, double max_v, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 該当範囲の節点のrtz座標を指定する(線形補間を計算する軸と座標範囲を指定して移動) */
	void move_node_rtz_type3(ConvMesh& mesh, int move_mode, int axis, double min_move, double max_move, double min_v, double max_v, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);
	/* 該当範囲の節点のxyz座標を指定する・材料範囲も指定(線形補間を計算する軸と座標範囲を指定して移動) */
	void move_node_xyz_type3_mat(ConvMesh& mesh, int move_mode, int mat1, int mat2, int axis, double min_move, double max_move, double min_v, double max_v, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 該当範囲の節点のrtz座標を指定する・材料範囲も指定(線形補間を計算する軸と座標範囲を指定して移動) */
	void move_node_rtz_type3_mat(ConvMesh& mesh, int move_mode, int mat1, int mat2, int axis, double min_move, double max_move, double min_v, double max_v, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 材料番号を変更する(range_type... 0:全範囲、1:xyzで指定、2:rtzで指定、3：xyzで指定しold材料無視、4：rtzで指定しold材料無視)  */
	void change_matID(ConvMesh& mesh, int range_type, int old_mat, int new_mat, double min1, double max1, double min2, double max2, double min3, double max3);
	/* 材料番号を変更する(全範囲) */
	void change_matID(ConvMesh& mesh, int old_mat, int new_mat);
	/* 材料番号を変更する(:xyzで指定) */
	void change_matID_xyz(ConvMesh& mesh, int old_mat, int new_mat, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 材料番号を変更する(:rtzで指定) */
	void change_matID_rtz(ConvMesh& mesh, int old_mat, int new_mat, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);

	/* 材料番号を変更する Type 2 (range_type... 0:全範囲、1:xyzで指定、2:rtzで指定、3：xyzで指定しold材料無視、4：rtzで指定しold材料無視)  */
	void change_matID2(ConvMesh& mesh, int range_type, int new_mat, int mat1, int mat2, double min1, double max1, double min2, double max2, double min3, double max3);
	/* 材料番号を変更する Type 2 (全範囲) */
	void change_matID2(ConvMesh& mesh, int new_mat, int mat1, int mat2);
	/* 材料番号を変更する Type 2 (:xyzで指定) */
	void change_matID2_xyz(ConvMesh& mesh, int new_mat, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 材料番号を変更する Type 2 (:rtzで指定) */
	void change_matID2_rtz(ConvMesh& mesh, int new_mat, int mat1, int mat2, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);

	/* 材料番号を変更する・加算ver(range_type... 0:全範囲、1:xyzで指定、2:rtzで指定) */
	void change_plus_matID(ConvMesh& mesh, int range_type, int plus_id, int mat1, int mat2, double min1, double max1, double min2, double max2, double min3, double max3);
	/* 材料番号を変更する 加算ver(全範囲) */
	void change_plus_matID(ConvMesh& mesh, int plus_id, int mat1, int mat2);
	/* 材料番号を変更する 加算ver (:xyzで指定) */
	void change_plus_matID_xyz(ConvMesh& mesh, int plus_id, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 材料番号を変更する 加算ver (:rtzで指定) */
	void change_plus_matID_rtz(ConvMesh& mesh, int plus_id, int mat1, int mat2, double rmin, double rmax, double thmin, double thmax, double zmin, double zmax);

	/* 指定した要素番号の要素の物質番号を指定した番号にする  */
	void change_matID_eid(ConvMesh& mesh, int new_mat, convint ele_id1, convint ele_id2);
	/* 材料番号を変更する(指定した材料IDのものを、要素ID+INCに) */
	void change_mat_toEID(ConvMesh& mesh, int inc_num, int target_num, int* target_matid);
	/* 隣接要素同士でつながっている要素群を抜出、それらすべてを指定した材料番号に変更させる */
	void change_neighbour_eles(ConvMesh& mesh, int num_start, convint* start_ele, int target_mat_num, int* target_matid, int after_mat, int inc);
	void change_neighbour_eles_subrutin(ConvMesh& mesh, convint ele_id, int num_mat, int *target_ids, const vector<vector<convint>>& Ele_neighbour, vector<convint>& changed_ele_IDs, bool *checked);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 指定した座標を始点・終点にする線要素を追加する */
	void line_add(ConvMesh& mesh, double* xyz1, double* xyz2, int add_mat);
	/* 指定した節点IDから線要素を追加する */
	void line_add2(ConvMesh& mesh, convint sID0, convint eID0, int add_mat);
	/* 指定した始点節点・終点節点の間にある辺を抽出し、線要素として作成 */
	void line_add_M(ConvMesh& mesh, const vector<convint>& sID0,  const vector<convint>& eID0, int add_mat);
	/* 指定した座標を始点・終点の間にある辺を抽出し、線要素として作成 */
	void line_add_M_pos(ConvMesh& mesh, double* xyz1, double* xyz2, int add_mat);
	/* 指定した円弧を描き、その円弧に乗っている辺を抽出、線要素として作成 */
	void line_add_R(ConvMesh& mesh, double targetR, double init_th, double end_th,double targetZ, double EPS_DIFF_R, int add_mat);
	/* 指定した座標の節点を生成 */
	void make_node(ConvMesh& mesh, double* xyz);
	/* 指定した節点IDから六面体を生成 */
	void make_hex_ele(ConvMesh& mesh, convint* ids, int add_mat);
	/* 要素のうち、指定した座標面にある表面を面要素としてメッシュに加える。 */
	void surf_add2d_from3Dsurf(ConvMesh& mesh, int surf_type, double surf_val, int target_mat, int add_sfmat);
	/* 要素のうち、指定した座標面にある表面を面要素としてメッシュに加える~複数指定ver。 */
	void surf_add2d_from3Dsurf2(ConvMesh& mesh, int surf_type, double surf_val, const vector<int>& target_mat, int add_sfmat_start, int add_inc);
	/* 要素のうち、指定したR座標面にある表面を面要素としてメッシュに加える。 */
	void surf_add2d_R(ConvMesh& mesh, double surf_val, double r_eps, int target_mat, int add_sfmat, int face_dir, double tmin, double tmax, double zmin, double zmax);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 指定した要素IDの要素の面積を計算する（2D要素のみ） */
	void calc_area_ele(ConvMesh& mesh, const vector<convint>& target_ele_ids, map<convint, double>& result, bool is_print);
	/* 指定した材料IDの要素の面積を計算し、その合計を計算する（2D要素のみ）  */
	void calc_area_mat(ConvMesh& mesh, const vector<int>& target_mat, map<convint, double>& result, bool is_print);
	/* 指定した要素IDの要素の体積を計算する（3D要素のみ） */
	void calc_volume_ele(ConvMesh& mesh, const vector<convint>& target_ele_id, map<convint, double>& result, bool is_print);
	/* 指定した材料IDの要素の体積を計算し、その合計を計算する（3D要素のみ）  */
	void calc_volume_mat(ConvMesh& mesh, const vector<int>& target_mat_id, map<convint, double>& result, bool is_print);
	/*--------------*/
	/* 指定した材料番号の要素を抜出して、IDをファイルに書き出す（ゼロ始まりで） */
	void output_elementID(ConvMesh& mesh, int mat_id, const string& filename, vector<convint>& result_id, bool is_print);
	/* 指定した範囲のを抜出して、IDをファイルに書き出す（ゼロ始まりで） */
	void output_elementID_POS(ConvMesh& mesh, double min1, double max1, double min2, double max2, double min3, double max3, int sort_dir,bool mode, vector<convint>& result_id, bool is_print);
	/* 指定した材料IDの要素の重心を計算し、書き出す  */
	void output_ele_grav(ConvMesh& mesh, int target_mat_id, const string& str, map<convint, vector<double>>& result, bool is_print);
	/* 指定した材料IDの要素の重心を計算し、書き出す(総和型)  */
	void output_ele_grav_SUM(ConvMesh& mesh, int target_mat_id, const string& str, map<convint, vector<double>>& result, bool is_print);
	/* 指定した範囲の節点をサーチし、指定した座標値の一覧をファイル出力する(mode true:XYZ) */
	void output_node_values(ConvMesh& mesh, double min1, double max1, double min2, double max2, double min3, double max3, int type, bool mode, vector<double>& result, bool is_print);
	/* 指定した材料の要素をサーチし、その回転対称位置にある要素とのペアを調べ、書き出す  */
	void output_rot_search(ConvMesh& mesh, int rot_num, double rot_deg, double target_deg1, double target_deg2, int target_mat, int rot_sym_mat, map<convint, vector<convint>>& result_ids, bool is_print);
	/* 指定した材料の要素をサーチし、その鏡面対称位置にある要素とのペアを調べ、書き出す  */
	void output_mirror_search(ConvMesh& mesh, int axis, int target_mat, int mirror_mat, map<convint, convint>& result_ids, bool is_print);
	/* 指定した材料の要素をサーチし、その鏡面対称・回転対称の位置にある要素とのペアを調べ、書き出す  */
	void output_mirror_rot_search(ConvMesh& mesh, int axis, double start_deg, double end_deg, int target_mat, int mirror_mat, int rot_num, double rot_deg, map<convint, vector<convint>>& result_ids, bool is_print);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 範囲内の節点を回転 */
	void Rotation_node(ConvMesh& mesh, int dir, double rot_deg, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, bool mode);
	/* 範囲内の節点を鏡面移動 */
	void MIRROR_move_node(ConvMesh& mesh, int surf, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, bool mode);
	/* 鏡面コピー・軸鏡面ver(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す) */
	void MirrorCopy(ConvMesh& mesh, int surf, int dir, int node_del, int map_make);
	/* 鏡面コピー・回転ver(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す) */
	void MirrorCopyRot(ConvMesh& mesh, int dir, double rot_deg, int node_del, int map_make);
	void MirrorCopy_SubRutin(ConvMesh& mesh, bool node_del, bool map_make, int dir, double rot_deg,
		convint node_size0, convint element_size0, convint increment_size, bool *Not_copied_Node, map<convint, convint>& AfterNode, convint *New_nodeID, map<convint, convint>& Ele_map);
	/* 鏡面コピー後、回転コピー */
	void MirrorCopyRot_MULTI(ConvMesh& mesh, int dir, double rot_deg, int node_del, int rot_times, int map_make);
	void MirrorCopyRot_MULTI_rot_sub(ConvMesh& mesh, int dir, int node_del, double rot_deg, int cp_times, int map_make, int is_inc_mat, int inc_val, convint** Ele_map);
	/* 回転コピー(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す) */
	void RotCopy(ConvMesh& mesh, int dir, double rot_deg, int cp_times, int node_del, int map_make, int is_inc_mat, int inc_val);
	void RotCopy_SubRutin(ConvMesh& mesh, bool node_del, bool map_make, int dir, double rot_deg, int cp_times,
		convint node_size0, convint element_size0, convint increment_size, bool *Not_copied_Node, map<convint, convint>& AfterNode, convint *New_nodeID, int is_inc_mat, int inc_val, convint** Ele_map);
	/* 移動コピー(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す) */
	void MoveCopy(ConvMesh& mesh, int dir, double move_val, int cp_times, int node_del, int map_make, int is_inc_mat, int inc_val);
	void MoveCopy_SubRutin(ConvMesh& mesh, bool node_del, bool map_make, int dir, double move_val, int cp_times,
		convint node_size0, convint element_size0, convint increment_size, bool *Not_copied_Node, map<convint, convint>& AfterNode, convint *New_nodeID, int is_inc_mat, int inc_val, convint** Ele_map);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 四角形要素を三角形に分割 */
	void div_sq_to_tri(ConvMesh& mesh);
	/* プリズム要素を四面体に分割。 */
	void div_pri_to_tet(ConvMesh& mesh);
	/* 六面体の局所IDを入れ替えて面を反転させる（積み上げで作った要素専用。他でやるとバグります） */
	void local_id_change_HEX(ConvMesh& mesh, int Dir, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 六面体の局所IDを好きに入れ替える（＊いまの向きがわかっているとき専用。わからずにやるとどこを向くかわからない。） */
	void local_id_mod_HEX(ConvMesh& mesh, const vector<convint>& mod_pos, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
	/* 2次元面要素の節点並びを、面積が正になるように並び替える */
	void local_id_mod_2DSurf(ConvMesh& mesh);
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/*--------------*/
	/* 2次元メッシュの積み上げ */
	void mesh2D_to_3D(ConvMesh& mesh, int no_layer, int no_base_mat, int no_layer_data, int no_z_data, int coor, int no_surf, bool dir_mode,
					const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate, const vector<int>& mat_id_2d,
					const vector<int>& from_layer, const vector<int>& to_layer, const vector<vector<int>>& layer_mat,
					const vector<int>& base_mat2D, const vector<int>& base_mat3D, const vector<int>& layer_pos);
	/* 別のメッシュを読み取って結合 */
	void add_mesh(ConvMesh& mesh, ConvMesh& mesh2, int kmat, int knod, int surf, double f_val);
};


/**/
};
};

#endif
