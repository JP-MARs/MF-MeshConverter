/**
* @file modify_ele_div.cpp
* @brief modify_ele_div
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>

namespace JPMRspace{
namespace MeshConvLib{


/*
//=======================================================
// ● 四角形要素を三角形に分割
//=======================================================*/
/** 
 * @brief devide square to tri element
*/
void MeshConverter::div_sq_to_tri(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "Div sq to tri " << endl;
#endif
	/* 要素を一時vectorに */
	vector<ConvElement> temp_ele_data;
	const convint ele_size = mesh.element.size();
	convint ele_count = ele_size;
	for(convint e = 0 ; e < ele_size ; e++){
		const ConvEType e_type = mesh.element[e].getType();
		/* 四角形のとき */
		if(e_type == ConvEType::Quad){
			ConvElement tr1 = mesh.element[e];
			ConvElement tr2 = mesh.element[e];
			convint nodes[4];
			mesh.element[e].getNodes(nodes);
			tr1.setType(ConvEType::Tri);
			tr2.setType(ConvEType::Tri);
			convint nodes1[3] = {nodes[0], nodes[1], nodes[3]};
			convint nodes2[3] = {nodes[1], nodes[2], nodes[3]};
			tr1.setNodes(nodes1);
			tr2.setNodes(nodes2);
			temp_ele_data.push_back( std::move(tr1) );
			temp_ele_data.push_back( std::move(tr2) );
		}else{
			temp_ele_data.push_back( std::move(mesh.element[e]) );
		}
	}
	/*確定*/
	mesh.element.swap(temp_ele_data);
	/* ID振りなおし */
	const convint ele_count2 = mesh.element.size();
	for(convint e = 0 ; e < ele_count2 ; e++){
		mesh.element[e].setID(e);
	}
	mesh.info_maked = false;
}

/*
//=======================================================
// ● リズム要素を四面体に分割
//=======================================================*/
/** 
 * @brief devide prizm to tetra
*/
void MeshConverter::div_pri_to_tet(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "Div sq to tri " << endl;
#endif
	convint node_num = mesh.node.size();
	vector<vector<convint>> node_lines;
	node_lines.resize(node_num);
	for(convint i=0 ; i < node_num ; i++){
		node_lines[i].clear();
	}

	/* 要素を一時vectorに */
	vector<ConvElement> temp_ele_data;
	const convint ele_size = mesh.element.size();
	convint ele_count = ele_size;
	for(convint e = 0 ; e < ele_size ; e++){
		const ConvEType e_type = mesh.element[e].getType();
		/* プリズムのとき */
		if(e_type == ConvEType::Prizm){
			double grav[3];
			mesh.element[e].getGrav(grav, mesh.node);
			/* 重心に新たな節点を追加する */
			ConvNode tempN;
			convint n_id_new = mesh.node.size();
			tempN.setID(n_id_new);
			tempN.set(grav[0], grav[1], grav[2]);
			mesh.node.push_back(tempN);

			ConvElement tets[8];
			for(int i=0 ; i < 8 ; i++){
				tets[i] = mesh.element[e];
				tets[i].setType(ConvEType::Tetra);
			}
			convint nodes[6];
			mesh.element[e].getNodes(nodes);
			/* 上下面は分割してしまう */
			convint nodes0[4] = {nodes[0], nodes[1], nodes[2], n_id_new};
			convint nodes1[4] = {nodes[3], nodes[5], nodes[4], n_id_new};
			tets[0].setNodes(nodes0);
			tets[1].setNodes(nodes1);
			/* 左右の面の既存分割サーチ */
			convint faces[3][4] = {
				{nodes[0], nodes[1], nodes[4], nodes[3]},
				{nodes[1], nodes[2], nodes[5], nodes[4]},
				{nodes[2], nodes[0], nodes[3], nodes[5]}
			};
			for(int s=0; s < 3 ; s++){
				int line1 = node_lines[faces[s][0]].size();
				int line2 = node_lines[faces[s][1]].size();
				bool is_line_02 = false;
				bool is_line_13 = false;
				bool no_line = true;
				/* 面0-2ラインがあるか */
				if(line1 != 0){
					for(int j = 0 ; j < line1 ; j++){
						if(node_lines[faces[s][0]][j] == faces[s][2]){
							is_line_02 = true;
							no_line = false;
						}
					}
				}
				/* 面1-3ラインがあるか */
				if(line2 != 0){
					for(int j = 0 ; j < line2 ; j++){
						if(node_lines[faces[s][1]][j] == faces[s][3]){
							is_line_13 = true;
							no_line = false;
						}
					}
				}
				if(is_line_02 && is_line_13){
					cout << "Error in DIV search ! " << endl;
					getchar();
				}
				/* 0-2分割にする場合 */
				if(is_line_02 || no_line){
					convint nodesA[4] = {faces[s][0], faces[s][2], faces[s][1], n_id_new};
					convint nodesB[4] = {faces[s][0], faces[s][3], faces[s][2], n_id_new};
					tets[2*s+2].setNodes(nodesA);
					tets[2*s+3].setNodes(nodesB);
					node_lines[faces[s][0]].push_back(faces[s][2]);
					/* 1-3分割にする場合 */
				}else if(is_line_13){
					convint nodesA[4] = {faces[s][0], faces[s][3], faces[s][1], n_id_new};
					convint nodesB[4] = {faces[s][1], faces[s][3], faces[s][2], n_id_new};
					tets[2*s+2].setNodes(nodesA);
					tets[2*s+3].setNodes(nodesB);
					node_lines[faces[s][1]].push_back(faces[s][3]);
					/* 両方ラインがある＝これはエラーだ！ */
				}else if(is_line_02 && is_line_13){
					cout << "Error in DIV ! " << endl;
					getchar();
				}
			}
			/* 保存確定 */
			for(int i=0 ; i < 8 ; i++){
				temp_ele_data.push_back( std::move(tets[i]) );
			}
		}else{
			temp_ele_data.push_back( std::move(mesh.element[e]) );
		}
	}
	/*確定*/
	mesh.element.swap(temp_ele_data);
	/* ID振りなおし */
	const convint ele_count2 = mesh.element.size();
	for(convint e = 0 ; e < ele_count2 ; e++){
		mesh.element[e].setID(e);
	}
	mesh.info_maked = false;
}


/*
//===============================================================
// ○ 六面体の局所IDを入れ替えて面を反転させる（積み上げで作った要素専用。他でやるとバグります）
//===============================================================*/
/** 
 * @brief change local node id order
*/
void MeshConverter::local_id_change_HEX(ConvMesh& mesh, int Dir, int mat1, int mat2, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "HEX local change !"<<endl;
#endif
	/*int Dir;
	int mat1,mat2;
	double xmin, xmax, ymin, ymax, zmin, zmax;*/

	/**/

	const convint element_size0 = mesh.element.size();
	for(convint i = 0 ; i < element_size0 ;i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double xx = grav[0]; double yy = grav[1]; double zz = grav[2];

		double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
		double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
		bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		bool in_range = (inX && inY && inZ);
		if( !in_range ){
			continue;
		}
		int mat = mesh.element[i].getMat();
		bool in_mat = (mat1 <= mat && mat <= mat2);
		if( !in_mat ) continue;

		ConvEType type = mesh.element[i].getType();
		if(type == ConvEType::Hex){
			convint before[4];
			convint after[4];
			int Dir_0 = Dir;
			for(int kkk=0; kkk < 2 ; kkk++){
				if(Dir_0==0){
					/*0123-4567*/
					before[0] = 0; before[1] = 1; before[2] = 2; before[3] = 3;
					after[0] = 4;  after[1] = 5;  after[2] = 6;  after[3] = 7;
				}else if(Dir_0==1){
					/*1265-0374*/
					before[0] = 1; before[1] = 2; before[2] = 6; before[3] = 5;
					after[0] = 0;  after[1] = 3;  after[2] = 7;  after[3] = 4;
				}else{
					/*0154-3267*/
					before[0] = 0; before[1] = 1; before[2] = 5; before[3] = 4;
					after[0] = 3;  after[1] = 2;  after[2] = 6;  after[3] = 7;
				}
				for(int j = 0; j < 4 ; j++){
					convint swp0 = mesh.element[i].getNode(before[j]);
					convint swp1 = mesh.element[i].getNode(after[j]);
					mesh.element[i].setNodes(before[j], swp1);
					mesh.element[i].setNodes(after[j], swp0);
				}
				/* 一回反転しただけだと、左手系になるので、指定していない面でもう1回反転させる */
				if(kkk==0){
					int change[3] = {1, 2, 0};
					Dir_0 = change[Dir];
				}
			}
		}
	}
}


/*
//===============================================================
// ○ 六面体の局所IDを好きに入れ替える（＊いまの向きがわかっているとき専用。わからずにやるとどこを向くかわからない。）
//===============================================================*/
/** 
 * @brief change local node id order for hex
*/
void MeshConverter::local_id_mod_HEX(ConvMesh& mesh, const vector<convint>& mod_pos, int mat1, int mat2, 
	double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
#ifdef PRINT_CONV_INFO
	cout << "HEX local mod !"<<endl;
#endif
	/*int mod_pos[8];
	int mat1,mat2;
	double xmin, xmax, ymin, ymax, zmin, zmax;*/


	const convint element_size0 = mesh.element.size();
	for(convint i = 0 ; i < element_size0 ;i++){
		double grav[3];
		mesh.element[i].getGrav(grav, mesh.node);
		double xx = grav[0]; double yy = grav[1]; double zz = grav[2];

		double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
		double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
		double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
		bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
		bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
		bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
		bool in_range = (inX && inY && inZ);
		if( !in_range ){
			continue;
		}
		int mat = mesh.element[i].getMat();
		bool in_mat = (mat1 <= mat && mat <= mat2);
		if( !in_mat ) continue;

		ConvEType type = mesh.element[i].getType();
		if(type == ConvEType::Hex){
			int pre_nodes[8];
			for(int kkk=0; kkk < 8 ; kkk++){
				pre_nodes[kkk] = mesh.element[i].getNode(kkk);
			}
			for(int kkk=0; kkk < 8 ; kkk++){
				mesh.element[i].setNodes(kkk, pre_nodes[mod_pos[kkk]]);
			}
		}
	}
}


/*
//=======================================================
// ● 2次元面要素の節点並びを、面積が正になるように並び替える 
//=======================================================*/
/** 
 * @brief change local node ids order of 2D edge for Jacob is positive
*/
void MeshConverter::local_id_mod_2DSurf(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "2D surf index mod!" << endl;
#endif
	const convint ele_size = mesh.element.size();
	for(convint e = 0 ; e < ele_size ; e++){
		const ConvEType e_type = mesh.element[e].getType();
		/* 三角形 */
		if(e_type == ConvEType::Tri){
			double area = mesh.element[e].calcArea(mesh.node);
			if(area < 0){
				convint nodes[20];
				mesh.element[e].getNodes(nodes);
				convint temp1 = nodes[0];
				nodes[0] = nodes[1];
				nodes[1] = temp1;
				mesh.element[e].setNodes(nodes);
			}
		/* 四角形 */
		}else if(e_type == ConvEType::Quad){
			double area = mesh.element[e].calcArea(mesh.node);
			if(area < 0){
				convint nodes[20];
				mesh.element[e].getNodes(nodes);
				convint temp1 = nodes[0];
				convint temp2 = nodes[3];
				nodes[0] = nodes[1];
				nodes[3] = nodes[2];
				nodes[1] = temp1;
				nodes[2] = temp2;
				mesh.element[e].setNodes(nodes);
			}
		}
	}
	/*確定*/
	mesh.info_maked = false;
}



/**/
};
};


