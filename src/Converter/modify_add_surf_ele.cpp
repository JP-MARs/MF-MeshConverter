/**
* @file modify_add_surf_ele.cpp
* @brief modify_add_surf_ele 
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>

namespace JPMRspace{
namespace MeshConvLib{

/*
//=======================================================
// ● 指定した節点IDから六面体を生成
//=======================================================*/
/** 
 * @brief add hex data
*/
void MeshConverter::make_hex_ele(ConvMesh& mesh, convint* ids, int add_mat){
#ifdef PRINT_CONV_INFO
	cout << "HEX make "<<endl;
#endif
	int mat_id = add_mat;
	convint node_id[8];

	for(int i = 0 ; i < 8 ; i++){
		node_id[i] = ids[i];
	}
	ConvElement etmp;
	etmp.setType(ConvEType::Hex);
	etmp.setMat(mat_id);
	for(int i=0 ; i < 8 ; i++){
		etmp.setNodes(i, node_id[i]);
	}
	convint e_size00 = mesh.element.size();
	etmp.setID(e_size00);
	mesh.element.push_back( std::move(etmp) );
}



/*
//=======================================================
// ● 要素のうち、指定した座標面にある表面を面要素としてメッシュに加える
//=======================================================*/
/** 
 * @brief add 2D elements from 3D facets
*/
void MeshConverter::surf_add2d_from3Dsurf(ConvMesh& mesh, int surf_type, double surf_val, int target_mat, int add_sfmat){
#ifdef PRINT_CONV_INFO
	cout << "Add 2D surf from 3Dele " << endl;
#endif
	//int surf_type;
	//double surf_val;
	int mat_id;
	int smat_id;

	mat_id = target_mat;
	smat_id = add_sfmat;

	/* 四面体を構成する4つの面 *//*ieg : 面を構成する三つの局所節点番号*/
	//const int tet_ing[4][3] = {{0,1,2},{0,1,3},{1,2,3},{0,2,3}};
	/* プリズムを構成する5つの面 */
	//const int pri_ing[5][4] = {{0,1,2,-1},{3,4,5,-1},
	//{0,1,4,3}, {1,2,5,4},{0,2,5,3}};
	/* 六面体を構成する6つの面 */
	//const int hex_ing[6][4] = {{4,5,6,7},{0,1,2,3},{3,2,6,7},
	//{0,1,5,4},{1,2,6,5},{0,3,7,4}};

	/* 四面体の面の節点 */
	const int tet_ing[4][3] = {
		{0, 1, 2}, {0, 1, 3}, {1, 2, 3}, {0, 2, 3}
	};
	/* プリズムの面の節点 */
	const int pri_ing[5][4] = {
		{0,1,2, 0}, {3,4,5, 0},
		{0, 1, 4, 3}, {1, 2, 5, 4}, {0, 2, 5, 3}
	};
	/* 六面体の面の節点 */
	const int hex_ing[6][4] = {
		{4, 5, 6, 7}, {0, 1, 2, 3}, {3, 2, 6, 7}, {0, 1, 5, 4},
		{1, 2, 6, 5}, {0, 3, 7, 4}
	};
	/* ピラミッドの面の節点 */
	const int pyramid_ing[5][4] = {
		{0,1,2,3},
		{0,1,4, 0},{3,2,4, 0},{0,3,4, 0},{1,2,4, 0}
	};

	vector<ConvElement> temp_new_2D;

	const convint ele_size = mesh.element.size();
	convint ele_count = ele_size;
	for(convint e = 0 ; e < ele_size ; e++){
		/* 対象要素か判定～2Dor対象の材料ではないなら無視 */
		ConvEType type = mesh.element[e].getType();
		bool is3D = mesh.element[e].is3D();		
		int mat = mesh.element[e].getMat();
		bool is_mat = (mat == mat_id);
		if( !is3D || !is_mat ) continue;

		int num_node = mesh.element[e].getNodeNum();
		int num_edges = mesh.element[e].getEdgeNum();
		convint nodes[20];
		mesh.element[e].getNodes(nodes);
		/* 四面体 */
		if(type == ConvEType::Tetra){
			/* 各三角形面で、中心座標計算 */
			for(int s = 0 ; s < 4 ; s++){
				double xx=0,yy=0,zz=0;

				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 3 ; n++){
					convint n_id = nodes[tet_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double diff;
					if(surf_type == 0){
						double temp = (xx-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 1){
						double temp = (yy-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 2){
						double temp = (zz-surf_val);
						diff = sqrt(temp*temp);
					}
					on_surf &= (diff < eps_diff);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Tri);
					convint node2d[3];
					for(int n = 0 ; n < 3 ; n++){
						convint n_id = nodes[tet_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					ele_count++;
					temp_new_2D.push_back( std::move(tempE) );
				}
			}
		/* プリズム */
		}else if(type == ConvEType::Prizm){
			/* 各三角形面で、中心座標計算 */
			for(int s = 0 ; s < 2 ; s++){
				double xx=0,yy=0,zz=0;
				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 3 ; n++){
					convint n_id = nodes[pri_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double diff;
					if(surf_type == 0){
						double temp = (xx-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 1){
						double temp = (yy-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 2){
						double temp = (zz-surf_val);
						diff = sqrt(temp*temp);
					}
					on_surf &= (diff < eps_diff);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Tri);
					convint node2d[3];
					for(int n = 0 ; n < 3 ; n++){
						convint n_id = nodes[pri_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					ele_count++;
					temp_new_2D.push_back( std::move(tempE) );
				}
			}
			/* 各４角形面で、中心座標計算 */
			for(int s = 2 ; s < 5 ; s++){
				double xx=0,yy=0,zz=0;
				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 4 ; n++){
					convint n_id = nodes[pri_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double diff;
					if(surf_type == 0){
						double temp = (xx-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 1){
						double temp = (yy-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 2){
						double temp = (zz-surf_val);
						diff = sqrt(temp*temp);
					}
					on_surf &= (diff < eps_diff);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Quad);
					convint node2d[4];
					for(int n = 0 ; n < 4 ; n++){
						convint n_id = nodes[pri_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					ele_count++;
					temp_new_2D.push_back( std::move(tempE) );
				}
			}
		/* 六面体 */
		}else if(type == ConvEType::Hex){
			/* 各４角形面で、中心座標計算 */
			for(int s = 0 ; s < 6 ; s++){
				double xx=0,yy=0,zz=0;
				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 4 ; n++){					
					convint n_id = nodes[hex_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double diff;
					if(surf_type == 0){
						double temp = (xx-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 1){
						double temp = (yy-surf_val);
						diff = sqrt(temp*temp);
					}else if(surf_type == 2){
						double temp = (zz-surf_val);
						diff = sqrt(temp*temp);
					}
					on_surf &= (diff < eps_diff);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Quad);
					convint node2d[4];
					for(int n = 0 ; n < 4 ; n++){
						convint n_id = nodes[hex_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					ele_count++;
					temp_new_2D.push_back( std::move(tempE) );
				}
			}
		/* ピラミッド */
		}else if(type == ConvEType::Pyramid){
			std::cout << " This element type is not implimented.... in add surf"<<std::endl;
			exit(1);
		}
	}
	/* 一時保存した２Ｄ要素で、重心が完全一致しているものは削除する */
	const convint temp_2d_num = temp_new_2D.size();
	bool *temp_2d_alive = new bool[temp_2d_num];
	for(convint i = 0 ; i < temp_2d_num ; i++){
		temp_2d_alive[i] = true;
	}
	for(convint i = 0 ; i <temp_2d_num-1 ; i++ ){
		double grav1[3];
		temp_new_2D[i].getGrav(grav1, mesh.node);
		for(convint j = i+1 ; j <temp_2d_num ; j++ ){
			double grav2[3];
			temp_new_2D[j].getGrav(grav2, mesh.node);
			double dx = grav1[0]-grav2[0];
			double dy = grav1[1]-grav2[1];
			double dz = grav1[2]-grav2[2];
			double diff = sqrt( dx*dx + dy*dy + dz*dz );
			if(diff < eps_diff){
				temp_2d_alive[j] = false;
			}

		}
	}
	/* 生き残りをelemetにpush */
	convint ele_id_root = mesh.element.size();
	for(convint i = 0 ; i <temp_2d_num ; i++ ){
		if(temp_2d_alive[i]){
			temp_new_2D[i].setID(ele_id_root);
			mesh.element.push_back( std::move(temp_new_2D[i]) );
			ele_id_root++;
		}
	}
	delete[] temp_2d_alive;
#ifdef PRINT_CONV_INFO
	cout << "Add end " << ele_id_root - ele_size <<endl;
#endif
}



/*
//=======================================================
// ● 要素のうち、指定した座標面にある表面を面要素としてメッシュに加える~複数指定ver
//=======================================================*/
/** 
 * @brief add 2D elements from facets (multi)
*/
void MeshConverter::surf_add2d_from3Dsurf2(ConvMesh& mesh, int surf_type, double surf_val, const vector<int>& target_mat, int add_sfmat_start, int add_inc){
#ifdef PRINT_CONV_INFO
	cout << "Add 2D surf from 3Dele 2 " << endl;
#endif
	//int surf_type;
	//double surf_val;
	int num_mat = target_mat.size();	
	vector<int> mat_id = target_mat;
	int smat_id = add_sfmat_start;
	int inc_num = add_inc;

	/* 四面体を構成する4つの面 *//*ieg : 面を構成する三つの局所節点番号*/
	//const int tet_ing[4][3] = {{0,1,2},{0,1,3},{1,2,3},{0,2,3}};
	/* プリズムを構成する5つの面 */
	//const int pri_ing[5][4] = {{0,1,2,-1},{3,4,5,-1},
	//{0,1,4,3}, {1,2,5,4},{0,2,5,3}};
	/* 六面体を構成する6つの面 */
	//const int hex_ing[6][4] = {{4,5,6,7},{0,1,2,3},{3,2,6,7},
	//{0,1,5,4},{1,2,6,5},{0,3,7,4}};

	/* 四面体の面の節点 */
	const int tet_ing[4][3] = {
		{0, 1, 2}, {0, 1, 3}, {1, 2, 3}, {0, 2, 3}
	};
	/* プリズムの面の節点 */
	const int pri_ing[5][4] = {
		{0,1,2, 0}, {3,4,5, 0},
		{0, 1, 4, 3}, {1, 2, 5, 4}, {0, 2, 5, 3}
	};
	/* 六面体の面の節点 */
	const int hex_ing[6][4] = {
		{4, 5, 6, 7}, {0, 1, 2, 3}, {3, 2, 6, 7}, {0, 1, 5, 4},
		{1, 2, 6, 5}, {0, 3, 7, 4}
	};
	/* ピラミッドの面の節点 */
	const int pyramid_ing[5][4] = {
		{0,1,2,3},
		{0,1,4, 0},{3,2,4, 0},{0,3,4, 0},{1,2,4, 0}
	};


	const convint ele_size = mesh.element.size();
	convint ele_count = ele_size;
	for(int nnn = 0 ; nnn < num_mat ; nnn++){
		vector<ConvElement> temp_new_2D;
		int smat_id_new = smat_id + nnn*inc_num;
		for(convint e = 0 ; e < ele_size ; e++){
			/* 対象要素か判定～2Dor対象の材料ではないなら無視 */
			ConvEType type = mesh.element[e].getType();
			bool is3D = mesh.element[e].is3D();		
			int mat = mesh.element[e].getMat();
			bool is_mat = (mat == mat_id[nnn]);
			if( !is3D || !is_mat ) continue;

			int num_node = mesh.element[e].getNodeNum();
			int num_edges = mesh.element[e].getEdgeNum();
			convint nodes[20];
			mesh.element[e].getNodes(nodes);
			/* 四面体 */
			if(type == ConvEType::Tetra){
				/* 各三角形面で、中心座標計算 */
				for(int s = 0 ; s < 4 ; s++){
					double xx=0,yy=0,zz=0;

					bool on_surf = true;
					/* 全ての節点が対象の面に乗っているか判定 */
					for(int n = 0 ; n < 3 ; n++){
						convint n_id = nodes[tet_ing[s][n]];
						xx = mesh.node[n_id].getX();
						yy = mesh.node[n_id].getY();
						zz = mesh.node[n_id].getZ();
						double diff;
						if(surf_type == 0){
							double temp = (xx-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 1){
							double temp = (yy-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 2){
							double temp = (zz-surf_val);
							diff = sqrt(temp*temp);
						}
						on_surf &= (diff < eps_diff);
					}
					/* 判定OKなら、面を加える */
					if(on_surf){
						ConvElement tempE;
						tempE.setType(ConvEType::Tri);
						convint node2d[3];
						for(int n = 0 ; n < 3 ; n++){
							convint n_id = nodes[tet_ing[s][n]];
							node2d[n] = n_id;
						}
						tempE.setNodes(node2d);
						tempE.setMat(smat_id_new);
						tempE.setID(ele_count);
						double area = tempE.calcArea(mesh.node);
						if(area < 0){
							tempE.swap_nodes(1, 2);
						}
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
				/* プリズム */
			}else if(type == ConvEType::Prizm){
				/* 各三角形面で、中心座標計算 */
				for(int s = 0 ; s < 2 ; s++){
					double xx=0,yy=0,zz=0;
					bool on_surf = true;
					/* 全ての節点が対象の面に乗っているか判定 */
					for(int n = 0 ; n < 3 ; n++){
						convint n_id = nodes[pri_ing[s][n]];
						xx = mesh.node[n_id].getX();
						yy = mesh.node[n_id].getY();
						zz = mesh.node[n_id].getZ();
						double diff;
						if(surf_type == 0){
							double temp = (xx-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 1){
							double temp = (yy-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 2){
							double temp = (zz-surf_val);
							diff = sqrt(temp*temp);
						}
						on_surf &= (diff < eps_diff);
					}
					/* 判定OKなら、面を加える */
					if(on_surf){
						ConvElement tempE;
						tempE.setType(ConvEType::Tri);
						convint node2d[3];
						for(int n = 0 ; n < 3 ; n++){
							convint n_id = nodes[pri_ing[s][n]];
							node2d[n] = n_id;
						}
						tempE.setNodes(node2d);
						tempE.setMat(smat_id_new);
						tempE.setID(ele_count);
						double area = tempE.calcArea(mesh.node);
						if(area < 0){
							tempE.swap_nodes(1, 2);
						}
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
				/* 各４角形面で、中心座標計算 */
				for(int s = 2 ; s < 5 ; s++){
					double xx=0,yy=0,zz=0;
					bool on_surf = true;
					/* 全ての節点が対象の面に乗っているか判定 */
					for(int n = 0 ; n < 4 ; n++){
						convint n_id = nodes[pri_ing[s][n]];
						xx = mesh.node[n_id].getX();
						yy = mesh.node[n_id].getY();
						zz = mesh.node[n_id].getZ();
						double diff;
						if(surf_type == 0){
							double temp = (xx-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 1){
							double temp = (yy-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 2){
							double temp = (zz-surf_val);
							diff = sqrt(temp*temp);
						}
						on_surf &= (diff < eps_diff);
					}
					/* 判定OKなら、面を加える */
					if(on_surf){
						ConvElement tempE;
						tempE.setType(ConvEType::Quad);
						convint node2d[4];
						for(int n = 0 ; n < 4 ; n++){
							convint n_id = nodes[pri_ing[s][n]];
							node2d[n] = n_id;
						}
						tempE.setNodes(node2d);
						tempE.setMat(smat_id_new);
						tempE.setID(ele_count);
						double area = tempE.calcArea(mesh.node);
						if(area < 0){
							tempE.swap_nodes(0, 1);
							tempE.swap_nodes(2, 3);
						}
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
				/* 六面体 */
			}else if(type == ConvEType::Hex){
				/* 各４角形面で、中心座標計算 */
				for(int s = 0 ; s < 6 ; s++){
					double xx=0,yy=0,zz=0;
					bool on_surf = true;
					/* 全ての節点が対象の面に乗っているか判定 */
					for(int n = 0 ; n < 4 ; n++){					
						convint n_id = nodes[hex_ing[s][n]];
						xx = mesh.node[n_id].getX();
						yy = mesh.node[n_id].getY();
						zz = mesh.node[n_id].getZ();
						double diff;
						if(surf_type == 0){
							double temp = (xx-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 1){
							double temp = (yy-surf_val);
							diff = sqrt(temp*temp);
						}else if(surf_type == 2){
							double temp = (zz-surf_val);
							diff = sqrt(temp*temp);
						}
						on_surf &= (diff < eps_diff);
					}
					/* 判定OKなら、面を加える */
					if(on_surf){
						ConvElement tempE;
						tempE.setType(ConvEType::Quad);
						convint node2d[4];
						for(int n = 0 ; n < 4 ; n++){
							convint n_id = nodes[hex_ing[s][n]];
							node2d[n] = n_id;
						}
						tempE.setNodes(node2d);
						tempE.setMat(smat_id_new);
						tempE.setID(ele_count);
						double area = tempE.calcArea(mesh.node);
						if(area < 0){
							tempE.swap_nodes(0, 1);
							tempE.swap_nodes(2, 3);
						}
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
			/* ピラミッド */
			}else if(type == ConvEType::Pyramid){
				std::cout << " This element type is not implimented.... in add surf 2" << std::endl;
				exit(1);
			}
		}

		/* 一時保存した２Ｄ要素で、重心が完全一致しているものは削除する */
		const convint temp_2d_num = temp_new_2D.size();
		bool *temp_2d_alive = new bool[temp_2d_num];
		for(convint i = 0 ; i < temp_2d_num ; i++){
			temp_2d_alive[i] = true;
		}
		for(convint i = 0 ; i <temp_2d_num-1 ; i++ ){
			double grav1[3];
			temp_new_2D[i].getGrav(grav1, mesh.node);
			for(convint j = i+1 ; j <temp_2d_num ; j++ ){
				double grav2[3];
				temp_new_2D[j].getGrav(grav2, mesh.node);
				double dx = grav1[0]-grav2[0];
				double dy = grav1[1]-grav2[1];
				double dz = grav1[2]-grav2[2];
				double diff = sqrt( dx*dx + dy*dy + dz*dz );
				if(diff < eps_diff){
					temp_2d_alive[j] = false;
				}

			}
		}
		/* 生き残りをelemetにpush */
		convint ele_id_root = mesh.element.size();
		for(convint i = 0 ; i <temp_2d_num ; i++ ){
			if(temp_2d_alive[i]){
				temp_new_2D[i].setID(ele_id_root);
				mesh.element.push_back( std::move(temp_new_2D[i]) );
				ele_id_root++;
			}
		}
		delete[] temp_2d_alive;

	}
#ifdef PRINT_CONV_INFO
	cout << "Add end "<<endl;
#endif
}



/*
//=======================================================
// ● 要素のうち、指定したR座標面にある表面を面要素としてメッシュに加える
//=======================================================*/
/** 
 * @brief add 2D elements from facets at target arch
*/
void MeshConverter::surf_add2d_R(ConvMesh& mesh, double surf_val, double r_eps, int target_mat, int add_sfmat, int face_dir,
		double tmin, double tmax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "Add 2D surf from 3Dele verR " << endl;
#endif
	//double surf_val;
	//double r_eps;
	int mat_id = target_mat;	
	int smat_id = add_sfmat;
	//int face_dir;
	//double tmin,tmax,zmin,zmax;

	/* 四面体を構成する4つの面 *//*ieg : 面を構成する三つの局所節点番号*/
	//const int tet_ing[4][3] = {{0,1,2},{0,1,3},{1,2,3},{0,2,3}};
	/* プリズムを構成する5つの面 */
	//const int pri_ing[5][4] = {{0,1,2,-1},{3,4,5,-1},
	//{0,1,4,3}, {1,2,5,4},{0,2,5,3}};
	/* 六面体を構成する6つの面 */
	//const int hex_ing[6][4] = {{4,5,6,7},{0,1,2,3},{3,2,6,7},
	//{0,1,5,4},{1,2,6,5},{0,3,7,4}};

	/* 四面体の面の節点 */
	const int tet_ing[4][3] = {
		{0, 1, 2}, {0, 1, 3}, {1, 2, 3}, {0, 2, 3}
	};
	/* プリズムの面の節点 */
	const int pri_ing[5][4] = {
		{0,1,2, 0}, {3,4,5, 0},
		{0, 1, 4, 3}, {1, 2, 5, 4}, {0, 2, 5, 3}
	};
	/* 六面体の面の節点 */
	const int hex_ing[6][4] = {
		{4, 5, 6, 7}, {0, 1, 2, 3}, {3, 2, 6, 7}, {0, 1, 5, 4},
		{1, 2, 6, 5}, {0, 3, 7, 4}
	};
	/* ピラミッドの面の節点 */
	const int pyramid_ing[5][4] = {
		{0,1,2,3},
		{0,1,4, 0},{3,2,4, 0},{0,3,4, 0},{1,2,4, 0}
	};

	vector<ConvElement> temp_new_2D;

	const convint ele_size = mesh.element.size();
	convint ele_count = ele_size;
	for(convint e = 0 ; e < ele_size ; e++){
		/* 対象要素か判定～2Dor対象の材料ではないなら無視 */
		/* 対象要素か判定～2Dor対象の材料ではないなら無視 */
		ConvEType type = mesh.element[e].getType();
		bool is3D = mesh.element[e].is3D();		
		int mat = mesh.element[e].getMat();
		bool is_mat = (mat == mat_id);
		if( !is3D || !is_mat ) continue;

		int num_node = mesh.element[e].getNodeNum();
		int num_edges = mesh.element[e].getEdgeNum();
		convint nodes[20];
		mesh.element[e].getNodes(nodes);
		/* 四面体 */
		if(type == ConvEType::Tetra){
			/* 各三角形面で、中心座標計算 */
			for(int s = 0 ; s < 4 ; s++){
				double xx=0,yy=0,zz=0;

				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 3 ; n++){
					convint n_id = nodes[tet_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double rr = sqrt(xx*xx + yy*yy);

					double diff1 = fabs(rr+r_eps-surf_val);
					double diff2 = fabs(rr-r_eps-surf_val);

					on_surf &= (rr+r_eps >= surf_val && rr-r_eps <= surf_val);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Tri);
					convint node2d[3];
					for(int n = 0 ; n < 3 ; n++){
						convint n_id = nodes[tet_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					/* 法線をｒ方向に向ける */
					double grav[3];					
					tempE.getGrav(grav, mesh.node);
					double x1,y1,z1,x2,y2,z2,x3,y3,z3;
					x1 = mesh.node[nodes[tet_ing[s][0]]].getX(); y1 = mesh.node[nodes[tet_ing[s][0]]].getY(); z1 = mesh.node[nodes[tet_ing[s][0]]].getZ();
					x2 = mesh.node[nodes[tet_ing[s][1]]].getX(); y2 = mesh.node[nodes[tet_ing[s][1]]].getY(); z2 = mesh.node[nodes[tet_ing[s][1]]].getZ();
					x3 = mesh.node[nodes[tet_ing[s][2]]].getX(); y3 = mesh.node[nodes[tet_ing[s][2]]].getY(); z3 = mesh.node[nodes[tet_ing[s][2]]].getZ();
					double d1x = x2-x1; double d1y = y2-y1; double d1z = z2-z1;
					double d2x = x3-x1; double d2y = y3-y1; double d2z = z3-z1;
					double dirx = d1y*d2z - d1z*d2y;
					double diry = d1z*d2x - d1x*d2z;
					double dirz = d1x*d2y - d1y*d2x;
					double dot = grav[0]*dirx + grav[1]*diry + grav[2]*dirz;
					if(dot < 0){
						tempE.swap_nodes(1, 2);
					}
					if(face_dir < 0){
						tempE.swap_nodes(1, 2);
					}
					/* 範囲内なら追加 */
					double g_th = atan2(grav[1], grav[0]);
					g_th *= 180.0/DEF_CONV_PI;
					if(tmin <= g_th && g_th <= tmax && zmin <= grav[2] && grav[2] <= zmax){
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
			}
			/* プリズム */
		}else if(type == ConvEType::Prizm){
			/* 各三角形面で、中心座標計算 */
			for(int s = 0 ; s < 2 ; s++){
				double xx=0,yy=0,zz=0;
				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 3 ; n++){
					convint n_id = nodes[pri_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double rr = sqrt(xx*xx + yy*yy);

					double diff1 = fabs(rr+r_eps-surf_val);
					double diff2 = fabs(rr-r_eps-surf_val);

					on_surf &= (rr+r_eps >= surf_val && rr-r_eps <= surf_val);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Tri);
					convint node2d[3];
					for(int n = 0 ; n < 3 ; n++){
						convint n_id = nodes[pri_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					/* 法線をｒ方向に向ける */
					double grav[3];					
					tempE.getGrav(grav, mesh.node);
					double x1,y1,z1,x2,y2,z2,x3,y3,z3;
					x1 = mesh.node[nodes[pri_ing[s][0]]].getX(); y1 = mesh.node[nodes[pri_ing[s][0]]].getY(); z1 = mesh.node[nodes[pri_ing[s][0]]].getZ();
					x2 = mesh.node[nodes[pri_ing[s][1]]].getX(); y2 = mesh.node[nodes[pri_ing[s][1]]].getY(); z2 = mesh.node[nodes[pri_ing[s][1]]].getZ();
					x3 = mesh.node[nodes[pri_ing[s][2]]].getX(); y3 = mesh.node[nodes[pri_ing[s][2]]].getY(); z3 = mesh.node[nodes[pri_ing[s][2]]].getZ();
					double d1x = x2-x1; double d1y = y2-y1; double d1z = z2-z1;
					double d2x = x3-x1; double d2y = y3-y1; double d2z = z3-z1;
					double dirx = d1y*d2z - d1z*d2y;
					double diry = d1z*d2x - d1x*d2z;
					double dirz = d1x*d2y - d1y*d2x;
					double dot = grav[0]*dirx + grav[1]*diry + grav[2]*dirz;
					if(dot < 0){
						tempE.swap_nodes(1, 2);
					}
					if(face_dir < 0){
						tempE.swap_nodes(1, 2);
					}
					/* 範囲内なら追加 */
					double g_th = atan2(grav[1], grav[0]);
					g_th *= 180.0/DEF_CONV_PI;
					if(tmin <= g_th && g_th <= tmax && zmin <= grav[2] && grav[2] <= zmax){
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
			}
			/* 各４角形面で、中心座標計算 */
			for(int s = 2 ; s < 5 ; s++){
				double xx=0,yy=0,zz=0;
				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 4 ; n++){
					convint n_id = nodes[pri_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double rr = sqrt(xx*xx + yy*yy);

					double diff1 = fabs(rr+r_eps-surf_val);
					double diff2 = fabs(rr-r_eps-surf_val);

					on_surf &= (rr+r_eps >= surf_val && rr-r_eps <= surf_val);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Quad);
					convint node2d[4];
					for(int n = 0 ; n < 4 ; n++){
						convint n_id = nodes[pri_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					/* 法線をｒ方向に向ける */
					double grav[3];					
					tempE.getGrav(grav, mesh.node);
					double x1,y1,z1,x2,y2,z2,x3,y3,z3;
					x1 = mesh.node[nodes[pri_ing[s][0]]].getX(); y1 = mesh.node[nodes[pri_ing[s][0]]].getY(); z1 = mesh.node[nodes[pri_ing[s][0]]].getZ();
					x2 = mesh.node[nodes[pri_ing[s][1]]].getX(); y2 = mesh.node[nodes[pri_ing[s][1]]].getY(); z2 = mesh.node[nodes[pri_ing[s][1]]].getZ();
					x3 = mesh.node[nodes[pri_ing[s][3]]].getX(); y3 = mesh.node[nodes[pri_ing[s][3]]].getY(); z3 = mesh.node[nodes[pri_ing[s][3]]].getZ();
					double d1x = x2-x1; double d1y = y2-y1; double d1z = z2-z1;
					double d2x = x3-x1; double d2y = y3-y1; double d2z = z3-z1;
					double dirx = d1y*d2z - d1z*d2y;
					double diry = d1z*d2x - d1x*d2z;
					double dirz = d1x*d2y - d1y*d2x;
					double dot = grav[0]*dirx + grav[1]*diry + grav[2]*dirz;
					if(dot < 0){
						tempE.swap_nodes(0, 1);
						tempE.swap_nodes(2, 3);
					}
					if(face_dir < 0){
						tempE.swap_nodes(0, 1);
						tempE.swap_nodes(2, 3);
					}
					/* 範囲内なら追加 */
					double g_th = atan2(grav[1], grav[0]);
					g_th *= 180.0/DEF_CONV_PI;
					if(tmin <= g_th && g_th <= tmax && zmin <= grav[2] && grav[2] <= zmax){
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
			}
			/* 六面体 */
		}else if(type == ConvEType::Hex){
			/* 各４角形面で、中心座標計算 */
			for(int s = 0 ; s < 6 ; s++){
				double xx=0,yy=0,zz=0;
				bool on_surf = true;
				/* 全ての節点が対象の面に乗っているか判定 */
				for(int n = 0 ; n < 4 ; n++){					
					convint n_id = nodes[hex_ing[s][n]];
					xx = mesh.node[n_id].getX();
					yy = mesh.node[n_id].getY();
					zz = mesh.node[n_id].getZ();
					double rr = sqrt(xx*xx + yy*yy);

					double diff1 = fabs(rr+r_eps-surf_val);
					double diff2 = fabs(rr-r_eps-surf_val);

					on_surf &= (rr+r_eps >= surf_val && rr-r_eps <= surf_val);
				}
				/* 判定OKなら、面を加える */
				if(on_surf){
					ConvElement tempE;
					tempE.setType(ConvEType::Quad);
					convint node2d[4];
					for(int n = 0 ; n < 4 ; n++){
						convint n_id = nodes[hex_ing[s][n]];
						node2d[n] = n_id;
					}
					tempE.setNodes(node2d);
					tempE.setMat(smat_id);
					tempE.setID(ele_count);
					/* 法線をｒ方向に向ける */
					double grav[3];					
					tempE.getGrav(grav, mesh.node);
					double x1,y1,z1,x2,y2,z2,x3,y3,z3;
					x1 = mesh.node[nodes[hex_ing[s][0]]].getX(); y1 = mesh.node[nodes[hex_ing[s][0]]].getY(); z1 = mesh.node[nodes[hex_ing[s][0]]].getZ();
					x2 = mesh.node[nodes[hex_ing[s][1]]].getX(); y2 = mesh.node[nodes[hex_ing[s][1]]].getY(); z2 = mesh.node[nodes[hex_ing[s][1]]].getZ();
					x3 = mesh.node[nodes[hex_ing[s][3]]].getX(); y3 = mesh.node[nodes[hex_ing[s][3]]].getY(); z3 = mesh.node[nodes[hex_ing[s][3]]].getZ();
					double d1x = x2-x1; double d1y = y2-y1; double d1z = z2-z1;
					double d2x = x3-x1; double d2y = y3-y1; double d2z = z3-z1;
					double dirx = d1y*d2z - d1z*d2y;
					double diry = d1z*d2x - d1x*d2z;
					double dirz = d1x*d2y - d1y*d2x;
					double dot = grav[0]*dirx + grav[1]*diry + grav[2]*dirz;
					if(dot < 0){
						tempE.swap_nodes(0, 1);
						tempE.swap_nodes(2, 3);
					}
					if(face_dir < 0){
						tempE.swap_nodes(0, 1);
						tempE.swap_nodes(2, 3);
					}
					/* 範囲内なら追加 */
					double g_th = atan2(grav[1], grav[0]);
					g_th *= 180.0/DEF_CONV_PI;
					if(tmin <= g_th && g_th <= tmax && zmin <= grav[2] && grav[2] <= zmax){
						ele_count++;
						temp_new_2D.push_back( std::move(tempE) );
					}
				}
			}
		}
	}
	/* 一時保存した２Ｄ要素で、重心が完全一致しているものは削除する */
	const convint temp_2d_num = temp_new_2D.size();
	bool *temp_2d_alive = new bool[temp_2d_num];
	for(convint i = 0 ; i < temp_2d_num ; i++){
		temp_2d_alive[i] = true;
	}
	for(convint i = 0 ; i <temp_2d_num-1 ; i++ ){
		double grav1[3];
		temp_new_2D[i].getGrav(grav1, mesh.node);
		for(convint j = i+1 ; j <temp_2d_num ; j++ ){
			double grav2[3];
			temp_new_2D[j].getGrav(grav2, mesh.node);
			double dx = grav1[0]-grav2[0];
			double dy = grav1[1]-grav2[1];
			double dz = grav1[2]-grav2[2];
			double diff = sqrt( dx*dx + dy*dy + dz*dz );
			if(diff < eps_diff){
				temp_2d_alive[j] = false;
			}

		}
	}
	/* 生き残りをelemetにpush */
	convint ele_id_root = mesh.element.size();
	for(convint i = 0 ; i <temp_2d_num ; i++ ){
		if(temp_2d_alive[i]){
			temp_new_2D[i].setID(ele_id_root);
			mesh.element.push_back( std::move(temp_new_2D[i]) );
			ele_id_root++;
		}
	}
	delete[] temp_2d_alive;
}


/**/
};
};


