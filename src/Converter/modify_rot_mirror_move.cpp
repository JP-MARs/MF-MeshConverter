/**
* @file modify_rot_mirror_move.cpp
* @brief modify_rot_mirror_move
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>
#include <fstream>

namespace JPMRspace{
namespace MeshConvLib{



/*
//=======================================================
// ● 範囲内の節点を回転
//=======================================================*/
/** 
 * @brief rotation node positions
*/
void MeshConverter::Rotation_node(ConvMesh& mesh, int dir, double rot_deg, 
	double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, bool mode){
#ifdef PRINT_CONV_INFO
	cout << "Rotation  "<< (mode ? "XYZ " : "RTZ" ) <<endl;
#endif
	/*int dir;
	double rot_deg;
	double xmin, xmax, ymin, ymax, zmin, zmax;*/

	/* まずすべて非選択 */
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(convint i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		/* 範囲をrtzで指定 */
		if( !mode ){
			double rr, deg, zz3;
			if(dir == 0){
				rr = sqrt(zz*zz + yy*yy);
				deg = atan2(zz, yy);
				zz3 = xx;
			}else if(dir == 1){
				rr = sqrt(zz*zz + xx*xx);
				deg = atan2(xx, zz);
				zz3 = yy;
			}else{
				rr = sqrt(xx*xx + yy*yy);
				deg = atan2(yy, xx);
				zz3 = zz;
			}
			deg *= 180.0/DEF_CONV_PI;
			if( fabs(deg) > 180.01){
				cout << "Too large deg node ? " << xx << " & " << yy << " => "<<rr << ", " << deg << ", " <<zz3 << endl;
				getchar();
			}
			double diff_xmin = fabs(rr-xmin); double diff_xmax = fabs(rr-xmax);
			double diff_ymin = fabs(deg-ymin); double diff_ymax = fabs(deg-ymax);
			double diff_zmin = fabs(zz3-zmin); double diff_zmax = fabs(zz3-zmax);
			bool inX = ( (rr >= xmin || diff_xmin < eps_diff) && (rr <= xmax || diff_xmax < eps_diff) );
			bool inY = ( (deg >= ymin || diff_ymin < eps_diff) && (deg <= ymax || diff_ymax < eps_diff) );
			bool inZ = ( (zz3 >= zmin || diff_zmin < eps_diff) && (zz3 <= zmax || diff_zmax < eps_diff) );
			if(inX && inY && inZ){
				selected_node.push_back(i);
			}
		/* 範囲をxyzで指定 */
		}else{
			double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
			double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
			double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
			bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
			bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
			bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
			if(inX && inY && inZ){
				selected_node.push_back(i);
			}
		}
	}
	double rad = rot_deg * DEF_CONV_PI / 180.0;
	double rotMAT[3][3];
	if(dir == 0){
		rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
		rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
		rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
	}else if(dir == 1){
		rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
		rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
		rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
	}else{
		rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
		rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
		rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
	}
	const convint nn_size0 = selected_node.size();
	for(convint i = 0 ; i < nn_size0 ; i++){
		convint ID = selected_node[i];
		double newX,newY,newZ;
		double xx = mesh.node[ID].getX(); double yy = mesh.node[ID].getY(); double zz = mesh.node[ID].getZ();
		newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
		newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
		newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;
		mesh.node[ID].set(newX, newY, newZ);
	}
}


/*
//=======================================================
// ● 範囲内の節点を鏡面移動
//=======================================================*/
/** 
 * @brief mirror move node positions
*/
void MeshConverter::MIRROR_move_node(ConvMesh& mesh, int surf, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, bool mode){
#ifdef PRINT_CONV_INFO
	cout << "Mirror Move " << endl;
#endif
	//int surf;
	//double xmin, xmax, ymin, ymax, zmin, zmax;

	/* まずすべて非選択 */
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 対象節点選択開始 */
	vector<convint> selected_node;
	for(convint i = 0 ; i < n_size ; i++){
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		if( !mode ){
			double rr, deg, zz3;
			rr = sqrt(xx*xx + yy*yy);
			deg = atan2(yy, xx);
			zz3 = zz;
			deg *= 180.0/DEF_CONV_PI;
			if( fabs(deg) > 180.01){
				cout << xx << " & " << yy << " => "<<rr << ", " << deg << ", " <<zz3 << endl;
				getchar();
			}
			double diff_xmin = fabs(rr-xmin); double diff_xmax = fabs(rr-xmax);
			double diff_ymin = fabs(deg-ymin); double diff_ymax = fabs(deg-ymax);
			double diff_zmin = fabs(zz3-zmin); double diff_zmax = fabs(zz3-zmax);
			bool inX = ( (rr >= xmin || diff_xmin < eps_diff) && (rr <= xmax || diff_xmax < eps_diff) );
			bool inY = ( (deg >= ymin || diff_ymin < eps_diff) && (deg <= ymax || diff_ymax < eps_diff) );
			bool inZ = ( (zz3 >= zmin || diff_zmin < eps_diff) && (zz3 <= zmax || diff_zmax < eps_diff) );
			if(inX && inY && inZ){
				selected_node.push_back(i);
			}
		}else{
			double diff_xmin = fabs(xx-xmin); double diff_xmax = fabs(xx-xmax);
			double diff_ymin = fabs(yy-ymin); double diff_ymax = fabs(yy-ymax);
			double diff_zmin = fabs(zz-zmin); double diff_zmax = fabs(zz-zmax);
			bool inX = ( (xx >= xmin || diff_xmin < eps_diff) && (xx <= xmax || diff_xmax < eps_diff) );
			bool inY = ( (yy >= ymin || diff_ymin < eps_diff) && (yy <= ymax || diff_ymax < eps_diff) );
			bool inZ = ( (zz >= zmin || diff_zmin < eps_diff) && (zz <= zmax || diff_zmax < eps_diff) );
			if(inX && inY && inZ){
				selected_node.push_back(i);
			}
		}
	}
	const convint nn_size0 = selected_node.size();
	for(int i = 0 ; i < nn_size0 ; i++){
		convint ID = selected_node[i];
		double xx = mesh.node[ID].getX(); double yy = mesh.node[ID].getY(); double zz = mesh.node[ID].getZ();
		double newX,newY,newZ;
		newX=xx; newY=yy; newZ=zz;
		if(surf == 0){
			newY *= -1.0;
		}else if(surf == 1){
			newX *= -1.0;
		}else{
			newZ *= -1.0;
		}
		mesh.node[ID].set(newX, newY, newZ);
	}
	const convint element_size0 = mesh.element.size();
	for(convint ee = 0 ; ee < element_size0 ; ee++){
		/* 反転した分、局所IDも反転 *////* 要素タイプ 0:三角形、1:四角形、2:四面体、3:六面体、4:プリズム,5:ピラミッド   101:点要素,  201:線要素*/
		ConvEType type = mesh.element[ee].getType();
		if(type == ConvEType::Tri || type == ConvEType::Tetra){
			convint swp0 = mesh.element[ee].getNode(0);
			convint swp1 = mesh.element[ee].getNode(1);
			mesh.element[ee].setNodes(0, swp1);
			mesh.element[ee].setNodes(1, swp0);
		}else if(type == ConvEType::Quad){
			convint swp0 = mesh.element[ee].getNode(0);
			convint swp1 = mesh.element[ee].getNode(1);
			mesh.element[ee].setNodes(0, swp1);
			mesh.element[ee].setNodes(1, swp0);
			swp0 = mesh.element[ee].getNode(2);
			swp1 = mesh.element[ee].getNode(3);
			mesh.element[ee].setNodes(2, swp1);
			mesh.element[ee].setNodes(3, swp0);
		}else if(type == ConvEType::Hex){
			if(surf == 2){
				convint swp0 = mesh.element[ee].getNode(0);
				convint swp1 = mesh.element[ee].getNode(4);
				mesh.element[ee].setNodes(0, swp1);
				mesh.element[ee].setNodes(4, swp0);
				swp0 = mesh.element[ee].getNode(1);
				swp1 = mesh.element[ee].getNode(5);
				mesh.element[ee].setNodes(1, swp1);
				mesh.element[ee].setNodes(5, swp0);
				swp0 = mesh.element[ee].getNode(2);
				swp1 = mesh.element[ee].getNode(6);
				mesh.element[ee].setNodes(2, swp1);
				mesh.element[ee].setNodes(6, swp0);
				swp0 = mesh.element[ee].getNode(3);
				swp1 = mesh.element[ee].getNode(7);
				mesh.element[ee].setNodes(3, swp1);
				mesh.element[ee].setNodes(7, swp0);
			}else if(surf == 0){
				convint swp0 = mesh.element[ee].getNode(0);
				convint swp1 = mesh.element[ee].getNode(3);
				mesh.element[ee].setNodes(0, swp1);
				mesh.element[ee].setNodes(3, swp0);
				swp0 = mesh.element[ee].getNode(1);
				swp1 = mesh.element[ee].getNode(2);
				mesh.element[ee].setNodes(1, swp1);
				mesh.element[ee].setNodes(2, swp0);
				swp0 = mesh.element[ee].getNode(4);
				swp1 = mesh.element[ee].getNode(7);
				mesh.element[ee].setNodes(4, swp1);
				mesh.element[ee].setNodes(7, swp0);
				swp0 = mesh.element[ee].getNode(5);
				swp1 = mesh.element[ee].getNode(6);
				mesh.element[ee].setNodes(5, swp1);
				mesh.element[ee].setNodes(6, swp0);
			}else{
				convint swp0 = mesh.element[ee].getNode(0);
				convint swp1 = mesh.element[ee].getNode(1);
				mesh.element[ee].setNodes(0, swp1);
				mesh.element[ee].setNodes(1, swp0);
				swp0 = mesh.element[ee].getNode(2);
				swp1 = mesh.element[ee].getNode(3);
				mesh.element[ee].setNodes(2, swp1);
				mesh.element[ee].setNodes(3, swp0);
				swp0 = mesh.element[ee].getNode(4);
				swp1 = mesh.element[ee].getNode(5);
				mesh.element[ee].setNodes(4, swp1);
				mesh.element[ee].setNodes(5, swp0);
				swp0 = mesh.element[ee].getNode(6);
				swp1 = mesh.element[ee].getNode(7);
				mesh.element[ee].setNodes(6, swp1);
				mesh.element[ee].setNodes(7, swp0);
			}
		}else if(type == ConvEType::Prizm){
			if(surf == 2){
				convint swp0 = mesh.element[ee].getNode(0);
				convint swp1 = mesh.element[ee].getNode(3);
				mesh.element[ee].setNodes(0, swp1);
				mesh.element[ee].setNodes(3, swp0);
				swp0 = mesh.element[ee].getNode(1);
				swp1 = mesh.element[ee].getNode(4);
				mesh.element[ee].setNodes(1, swp1);
				mesh.element[ee].setNodes(4, swp0);
				swp0 = mesh.element[ee].getNode(2);
				swp1 = mesh.element[ee].getNode(5);
				mesh.element[ee].setNodes(2, swp1);
				mesh.element[ee].setNodes(5, swp0);
			}else{
				convint swp0 = mesh.element[ee].getNode(0);
				convint swp1 = mesh.element[ee].getNode(1);
				mesh.element[ee].setNodes(0, swp1);
				mesh.element[ee].setNodes(1, swp0);
				swp0 = mesh.element[ee].getNode(3);
				swp1 = mesh.element[ee].getNode(4);
				mesh.element[ee].setNodes(3, swp1);
				mesh.element[ee].setNodes(4, swp0);
			}
		} else if(type == ConvEType::Pyramid){
			std::cout <<  "This element type is not implimented... mirror..." << endl;
		}
	}
}


/*
//=======================================================
// ● 鏡面コピー・軸鏡面ver(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す)
//=======================================================*/
/** 
 * @brief copy elements (mirror)
*/
void MeshConverter::MirrorCopy(ConvMesh& mesh, int surf, int dir, int node_del, int map_make){
#ifdef PRINT_CONV_INFO
	cout << "Mirror Copy " << endl;
#endif
	/*int surf, dir;
	//double rot_deg;
	int node_del;
	int map_make;*/

	/* 初期サイズ */
	const convint node_size0 = mesh.node.size();
	/* y=0を探す */
	vector<convint> Not_copied_ID;	
	bool *Not_copied_Node = new bool[node_size0];
	convint *New_nodeID = new convint[node_size0];
	convint counter=0;
	for(convint i = 0 ; i < node_size0 ;i++){
		Not_copied_Node[i] = false;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		bool copy_surf=false;
		if(surf == 0){
			if(dir == 0){
				copy_surf = (yy < eps_diff);
			}else{
				copy_surf = (yy > -1.0*eps_diff);
			}
		}else if(surf == 1){
			if(dir == 0){
				copy_surf = (xx < eps_diff);
			}else{
				copy_surf = (xx > -1.0*eps_diff);
			}
		}else{
			if(dir == 0){
				copy_surf = (zz < eps_diff);
			}else{
				copy_surf = (zz > -1.0*eps_diff);
			}
		}
		New_nodeID[i] = i - counter;

		if(copy_surf && node_del == 1){
			Not_copied_ID.push_back(i);
			Not_copied_Node[i] = true;
			counter++;
		}
	}
	const convint not_copy_size = Not_copied_ID.size();
	map<convint, convint> AfterNode;
#ifdef PRINT_CONV_INFO
	cout << "on Surf Node size => " <<not_copy_size << endl;
#endif
	for(int i = 0 ; i < not_copy_size ;i++){
		convint id0 =  Not_copied_ID[i];
		if(Not_copied_Node[id0]){
			AfterNode.insert( pair<convint, convint>(id0, id0) );
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "Map size => " << AfterNode.size() << endl;
#endif
	/* 1回転で増える数 */
	const convint increment_size = node_size0 - not_copy_size;
#ifdef PRINT_CONV_INFO
	cout << "Inc size => " << increment_size <<endl;
#endif
	/* 初期要素数 */
	const convint element_size0 = mesh.element.size();

	convint aft_size00 = AfterNode.size();
	if(aft_size00 != not_copy_size){
		cout << "Mapping node data is not correct ! please adjust eps_diff value !!! " << endl;
		exit(1);
	}

	map<convint, convint> Ele_map;
	int count_n=0;
	/* 節点追加 */
	for(convint i = 0 ; i < node_size0 ;i++){
		/* 無視節点は無視 */
		if(Not_copied_Node[i]) continue;
		/* 反転して2倍回転 */
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		if(surf == 0){
			yy *= -1.0;
		}else if(surf == 1){
			xx *= -1.0;
		}else{
			zz *= -1.0;
		}
		double newX = xx;
		double newY = yy;
		double newZ = zz;
		/* 節点作成 */
		ConvNode N_temp;
		convint id_new = count_n + node_size0;
		N_temp.setID(id_new);
		N_temp.set(newX, newY, newZ);
		mesh.node.push_back(N_temp);
		count_n++;
	}
	/* 要素追加 */
	for(convint i = 0 ; i < element_size0 ;i++){
		ConvElement etmp;
		int ori_m = mesh.element[i].getMat();
		etmp.setMat(ori_m);
		etmp = mesh.element[i];
		int node_num = mesh.element[i].getNodeNum();
		/* 節点設定！ */
		for(int kk = 0 ; kk < node_num ; kk++){
			convint preN = mesh.element[i].getNode(kk);
			if(Not_copied_Node[preN]){
				//cout << AfterNode[preN] << endl;
				convint aft_node = AfterNode[preN];
				etmp.setNodes(kk, aft_node);
			}else{
				//cout << preN << " => " << New_nodeID[preN] << " ===>>> "<< New_nodeID[preN] + node_size0 + nnn*increment_size<<endl;
				convint aft_rot = New_nodeID[preN] + node_size0;
				etmp.setNodes(kk, aft_rot);
			}
		}
		/* 反転した分、局所IDも反転 */
		ConvEType type = etmp.getType();
		if(type == ConvEType::Tri || type == ConvEType::Tetra){
			convint swp0 = etmp.getNode(0);
			convint swp1 = etmp.getNode(1);
			etmp.setNodes(0, swp1);
			etmp.setNodes(1, swp0);
		}else if(type == ConvEType::Quad){
			convint swp0 = etmp.getNode(0);
			convint swp1 = etmp.getNode(1);
			etmp.setNodes(0, swp1);
			etmp.setNodes(1, swp0);
			swp0 = etmp.getNode(2);
			swp1 = etmp.getNode(3);
			etmp.setNodes(2, swp1);
			etmp.setNodes(3, swp0);
		}else if(type == ConvEType::Hex){
			if(surf == 2){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(4);
				etmp.setNodes(0, swp1);
				etmp.setNodes(4, swp0);
				swp0 = etmp.getNode(1);
				swp1 = etmp.getNode(5);
				etmp.setNodes(1, swp1);
				etmp.setNodes(5, swp0);
				swp0 = etmp.getNode(2);
				swp1 = etmp.getNode(6);
				etmp.setNodes(2, swp1);
				etmp.setNodes(6, swp0);
				swp0 = etmp.getNode(3);
				swp1 = etmp.getNode(7);
				etmp.setNodes(3, swp1);
				etmp.setNodes(7, swp0);
			}else if(surf == 0){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(3);
				etmp.setNodes(0, swp1);
				etmp.setNodes(3, swp0);
				swp0 = etmp.getNode(1);
				swp1 = etmp.getNode(2);
				etmp.setNodes(1, swp1);
				etmp.setNodes(2, swp0);
				swp0 = etmp.getNode(4);
				swp1 = etmp.getNode(7);
				etmp.setNodes(4, swp1);
				etmp.setNodes(7, swp0);
				swp0 = etmp.getNode(5);
				swp1 = etmp.getNode(6);
				etmp.setNodes(5, swp1);
				etmp.setNodes(6, swp0);
			}else{
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(1);
				etmp.setNodes(0, swp1);
				etmp.setNodes(1, swp0);
				swp0 = etmp.getNode(2);
				swp1 = etmp.getNode(3);
				etmp.setNodes(2, swp1);
				etmp.setNodes(3, swp0);
				swp0 = etmp.getNode(4);
				swp1 = etmp.getNode(5);
				etmp.setNodes(4, swp1);
				etmp.setNodes(5, swp0);
				swp0 = etmp.getNode(6);
				swp1 = etmp.getNode(7);
				etmp.setNodes(6, swp1);
				etmp.setNodes(7, swp0);
			}
		}else if(type == ConvEType::Prizm){
			if(surf == 2){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(3);
				etmp.setNodes(0, swp1);
				etmp.setNodes(3, swp0);
				swp0 = etmp.getNode(1);
				swp1 = etmp.getNode(4);
				etmp.setNodes(1, swp1);
				etmp.setNodes(4, swp0);
				swp0 = etmp.getNode(2);
				swp1 = etmp.getNode(5);
				etmp.setNodes(2, swp1);
				etmp.setNodes(5, swp0);
			}else{
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(1);
				etmp.setNodes(0, swp1);
				etmp.setNodes(1, swp0);
				swp0 = etmp.getNode(3);
				swp1 = etmp.getNode(4);
				etmp.setNodes(3, swp1);
				etmp.setNodes(4, swp0);
			}
		}
		convint ee_id = mesh.element.size();
		etmp.setID(ee_id);
		/* 確定 */
		mesh.element.push_back( std::move(etmp) );

		Ele_map.insert( pair<convint, convint>(i, ee_id) );
	}
	if(map_make == 1){
		std::fstream fp("MappingMirror.csv", std::ios::out);
		fp << element_size0 << endl;
		//fprintf(fp, "%d\n", element_size0);
		for(convint i = 0 ; i < element_size0 ;i++){
			fp << i << ", " << Ele_map[i] << endl;
		}
		fp.close();
	}
	delete[] Not_copied_Node;
	delete[] New_nodeID;
}


/*
//=======================================================
// ● 鏡面コピー・回転ver(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す)
//=======================================================*/
/** 
 * @brief copy elements (mirror and rot.)
*/
void MeshConverter::MirrorCopyRot(ConvMesh& mesh, int dir, double rot_deg, int node_del, int map_make){
#ifdef PRINT_CONV_INFO
	cout << "Mirror Copy " << endl;
#endif
	/*int dir;
	double rot_deg;
	int node_del;
	int map_make;*/

	/* 初期サイズ */
	const convint node_size0 = mesh.node.size();
	/* y=0を探す */
	vector<convint> Not_copied_ID;	
	bool *Not_copied_Node = new bool[node_size0];
	convint *New_nodeID = new convint[node_size0];
	convint counter=0;
	for(convint i = 0 ; i < node_size0 ;i++){
		Not_copied_Node[i] = false;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		/* まず鏡像面まで回転 */
		double rad = -1.0*rot_deg *DEF_CONV_PI/ 180.0;
		double rotMAT[3][3];
		if(dir == 0){
			rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
			rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
		}else if(dir == 1){
			rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
			rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
		}else{
			rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
		}
		double newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
		double newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
		double newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;
		New_nodeID[i] = i - counter;

		/* 節点が鏡像面ならコピーしない */
		bool coppy_surf=false;
		if(dir == 0){
			coppy_surf = (newZ > -1.0*eps_diff);
		}else if(dir == 1){
			coppy_surf = (newX > -1.0*eps_diff);
		}else{
			coppy_surf = (newY > -1.0*eps_diff);
		}
		if(coppy_surf && node_del == 1){
			Not_copied_ID.push_back(i);
			Not_copied_Node[i] = true;
			counter++;
		}
	}
	const convint not_copy_size = Not_copied_ID.size();
	map<convint, convint> AfterNode;
#ifdef PRINT_CONV_INFO
	cout << "on Surf Node size => " <<not_copy_size << endl;
#endif
	for(convint i = 0 ; i < not_copy_size ;i++){
		convint id0 =  Not_copied_ID[i];
		if(Not_copied_Node[id0]){
			AfterNode.insert( pair<convint, convint>(id0, id0) );
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "Map size => " << AfterNode.size() << endl;
#endif
	/* 1回転で増える数 */
	const convint increment_size = node_size0 - not_copy_size;
#ifdef PRINT_CONV_INFO
	cout << "Inc size => " << increment_size <<endl;
#endif
	/* 初期要素数 */
	const convint element_size0 = mesh.element.size();

	convint aft_size00 = AfterNode.size();
	if(aft_size00 != not_copy_size){
		cout << "Mapping node data is not correct ! please adjust eps_diff value !!! " << endl;
		exit(1);
	}

	map<convint, convint> Ele_map;

	MirrorCopy_SubRutin(mesh, node_del, map_make, dir, rot_deg,
		node_size0, element_size0, increment_size, Not_copied_Node, AfterNode, New_nodeID, Ele_map);

	if(map_make == 1){
		std::fstream fp("MappingMirrorRot.csv", std::ios::out);
		fp << element_size0 << endl;
		for(int i = 0 ; i < element_size0 ;i++){
			fp << i << ", " << Ele_map[i] << endl;
		}
		fp.close();
	}

	delete[] Not_copied_Node;
	delete[] New_nodeID;
}

/*
//=======================================================
// ● 鏡面コピー・回転ver(サブルーチンnode_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す)
//=======================================================*/
void MeshConverter::MirrorCopy_SubRutin(ConvMesh& mesh, bool node_del, bool map_make, int dir, double rot_deg,
	convint node_size0, convint element_size0, convint increment_size, bool *Not_copied_Node, map<convint, convint>& AfterNode, convint *New_nodeID, map<convint, convint>& Ele_map){

	convint count_n=0;
	for(int nnn = 0  ; nnn < 1 ; nnn++){
#ifdef PRINT_CONV_INFO
		cout << "ROT => " << nnn << endl;
#endif
		/* 節点追加 */
		for(convint i = 0 ; i < node_size0 ;i++){
			/* 無視節点は無視 */
			if(Not_copied_Node[i]) continue;
			/* 反転して2倍回転 */
			double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
			double rad = 2.0*rot_deg * DEF_CONV_PI / 180.0;
			double rotMAT[3][3];
			if(dir == 0){
				zz *= -1.0;
				rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
				rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
				rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
			}else if(dir == 1){
				xx *= -1.0;
				rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
				rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
				rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
			}else{
				yy *= -1.0;
				rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
				rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
				rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
			}
			double newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
			double newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
			double newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;
			//cout << xx << ", " << yy << " ==>> " << newX << ", " << newY<<endl;
			/* 節点作成 */
			ConvNode N_temp;
			convint id_new = count_n + node_size0 + nnn*increment_size;
			N_temp.setID(id_new);
			N_temp.set(newX, newY, newZ);
			mesh.node.push_back(N_temp);
			count_n++;
		}
		/* 要素追加 */
		for(convint i = 0 ; i < element_size0 ;i++){
			ConvElement etmp;
			int ori_m = mesh.element[i].getMat();
			etmp.setMat(ori_m);
			etmp = mesh.element[i];
			convint node_num = mesh.element[i].getNodeNum();
			/* 節点設定！ */
			for(int kk = 0 ; kk < node_num ; kk++){
				convint preN = mesh.element[i].getNode(kk);
				if(Not_copied_Node[preN]){
					//cout << AfterNode[preN] << endl;
					convint aft_node = AfterNode[preN];
					etmp.setNodes(kk, aft_node);
				}else{
					//cout << preN << " => " << New_nodeID[preN] << " ===>>> "<< New_nodeID[preN] + node_size0 + nnn*increment_size<<endl;
					convint aft_rot = New_nodeID[preN] + node_size0 + nnn*increment_size;
					etmp.setNodes(kk, aft_rot);
				}
			}
			/* 反転した分、局所IDも反転 */
			ConvEType type = etmp.getType();
			if(type == ConvEType::Tri || type == ConvEType::Tetra){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(1);
				etmp.setNodes(0, swp1);
				etmp.setNodes(1, swp0);
			}else if(type == ConvEType::Quad){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(1);
				etmp.setNodes(0, swp1);
				etmp.setNodes(1, swp0);
				swp0 = etmp.getNode(2);
				swp1 = etmp.getNode(3);
				etmp.setNodes(2, swp1);
				etmp.setNodes(3, swp0);
			}else if(type == ConvEType::Hex){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(1);
				etmp.setNodes(0, swp1);
				etmp.setNodes(1, swp0);
				swp0 = etmp.getNode(2);
				swp1 = etmp.getNode(3);
				etmp.setNodes(2, swp1);
				etmp.setNodes(3, swp0);
				swp0 = etmp.getNode(4);
				swp1 = etmp.getNode(5);
				etmp.setNodes(4, swp1);
				etmp.setNodes(5, swp0);
				swp0 = etmp.getNode(6);
				swp1 = etmp.getNode(7);
				etmp.setNodes(6, swp1);
				etmp.setNodes(7, swp0);
			}else if(type == ConvEType::Prizm){
				convint swp0 = etmp.getNode(0);
				convint swp1 = etmp.getNode(1);
				etmp.setNodes(0, swp1);
				etmp.setNodes(1, swp0);
				swp0 = etmp.getNode(3);
				swp1 = etmp.getNode(4);
				etmp.setNodes(3, swp1);
				etmp.setNodes(4, swp0);
			}
			convint ee_id = mesh.element.size();
			etmp.setID(ee_id);
			/* 確定 */
			mesh.element.push_back( std::move(etmp) );

			Ele_map.insert( pair<convint, convint>(i, ee_id) );
		}
	}
}

/*
//=======================================================
// ● 鏡面コピー後、回転コピー
//=======================================================*/
/** 
 * @brief copy elements (mirror & rotation)
*/
void MeshConverter::MirrorCopyRot_MULTI(ConvMesh& mesh, int dir, double rot_deg, int node_del, int rot_times, int map_make){
#ifdef PRINT_CONV_INFO
	cout << "Mirror Rot Multi" << endl;
#endif
	/*int dir;
	double rot_deg;
	int node_del;
	int rot_times;
	int map_make;*/

	/* 初期サイズ */
	const convint node_size0 = mesh.node.size();
	/* y=0を探す */
	vector<convint> Not_copied_ID;	
	bool *Not_copied_Node = new bool[node_size0];
	convint *New_nodeID = new convint[node_size0];
	convint counter=0;
	for(convint i = 0 ; i < node_size0 ;i++){
		Not_copied_Node[i] = false;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		/* まず鏡像面まで回転 */
		double rad = -1.0*rot_deg * DEF_CONV_PI / 180.0;
		double rotMAT[3][3];
		if(dir == 0){
			rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
			rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
		}else if(dir == 1){
			rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
			rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
		}else{
			rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
		}
		double newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
		double newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
		double newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;
		New_nodeID[i] = i - counter;

		/* 節点が鏡像面ならコピーしない */
		bool coppy_surf=false;
		if(dir == 0){
			coppy_surf = (newZ > -1.0*eps_diff);
		}else if(dir == 1){
			coppy_surf = (newX > -1.0*eps_diff);
		}else{
			coppy_surf = (newY > -1.0*eps_diff);
		}
		if(coppy_surf && node_del == 1){
			Not_copied_ID.push_back(i);
			Not_copied_Node[i] = true;
			counter++;
		}
	}
	const convint not_copy_size = Not_copied_ID.size();
	map<convint, convint> AfterNode;
	//cout << "on Surf Node size => " <<not_copy_size << endl;
	for(convint i = 0 ; i < not_copy_size ;i++){
		convint id0 =  Not_copied_ID[i];
		if(Not_copied_Node[id0]){
			AfterNode.insert( pair<convint, convint>(id0, id0) );
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "Map size => " << AfterNode.size() << endl;
#endif
	/* 1回転で増える数 */
	const convint increment_size = node_size0 - not_copy_size;
#ifdef PRINT_CONV_INFO
	cout << "Inc size => " << increment_size <<endl;
#endif
	/* 初期要素数 */
	const convint element_size0 = mesh.element.size();

	convint aft_size00 = AfterNode.size();
	if(aft_size00 != not_copy_size){
		cout << "Mapping node data is not correct ! please adjust eps_diff value !!! " << endl;
		exit(1);
	}

	map<convint, convint> Ele_map;
	MirrorCopy_SubRutin(mesh, node_del, map_make, dir, rot_deg,
		node_size0, element_size0, increment_size, Not_copied_Node, AfterNode, New_nodeID, Ele_map);

	delete[] Not_copied_Node;
	delete[] New_nodeID;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*つぎ、回転*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	convint after_mir_ele = mesh.element.size();
	convint** Ele_map_rot = new convint*[after_mir_ele];
	for(convint i = 0 ; i < after_mir_ele ;i++){
		Ele_map_rot[i] = new convint[rot_times];
	}
#ifdef PRINT_CONV_INFO
	cout << "step1 end, " << endl;
#endif
	MirrorCopyRot_MULTI_rot_sub(mesh, dir, node_del, 2.0*rot_deg, rot_times, map_make, 0, 0, Ele_map_rot) ;

	if(map_make == 1){
		convint** Ele_map_all = new convint*[element_size0];
		for(convint i = 0 ; i < element_size0 ;i++){
			Ele_map_all[i] = new convint[2*rot_times+1];
			Ele_map_all[i][0] = Ele_map[i];
			for(int j = 0 ; j < rot_times ; j++){
				Ele_map_all[i][2*j+1] = Ele_map_rot[i][j];
				convint mirror_E = Ele_map[i];
				Ele_map_all[i][2*j+2] = Ele_map_rot[mirror_E][j];
			}
		}

		std::fstream fp("MappingRotationMulti.csv", std::ios::out);
		fp <<  element_size0 << ", " << (2*rot_times+1) << endl;
		for(int i = 0 ; i < element_size0 ;i++){
			fp << i;
			for(int j = 0 ; j < 2*rot_times+1 ;j++){
				fp << ", " << Ele_map_all[i][j];
			}
			fp << endl;
		}
		fp.close();

		for(convint i = 0 ; i < element_size0 ;i++){
			delete[] Ele_map_all[i];
		}
		delete[] Ele_map_all;
	}


	for(convint i = 0 ; i < after_mir_ele ;i++){
		delete[] Ele_map_rot[i];
	}
	delete[] Ele_map_rot;
}

/*
//=======================================================
// ● 鏡面コピー後、回転コピー(サブルーチン)
//=======================================================*/
void MeshConverter::MirrorCopyRot_MULTI_rot_sub(ConvMesh& mesh, int dir, int node_del, double rot_deg, int cp_times, int map_make, int is_inc_mat, int inc_val, convint** Ele_map){
	/* 初期サイズ */
	const convint node_size0 = mesh.node.size();

	vector<convint> Not_copied_ID;	
	bool *Not_copied_Node = new bool[node_size0];
	convint *New_nodeID = new convint[node_size0];
	convint counter=0;
	/* コピーしない節点サーチ */
	for(convint i = 0 ; i < node_size0 ;i++){
		Not_copied_Node[i] = false;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		New_nodeID[i] = i - counter;

		if(node_del == 1){
			bool bl1 = (dir == 0 && zz < eps_diff);
			bool bl2 = (dir == 1 && xx < eps_diff);
			bool bl3 = (dir == 2 && yy < eps_diff);
			if(bl1 || bl2 || bl3){
				Not_copied_ID.push_back(i);
				Not_copied_Node[i] = true;
				counter++;
			}
		}
	}

	const convint not_copy_size = Not_copied_ID.size();
	map<convint, convint> AfterNode;
#ifdef PRINT_CONV_INFO
	cout << "on Surf Node size => " <<not_copy_size << endl;
#endif
	for(convint i = 0 ; i < not_copy_size ;i++){
		convint id0 =  Not_copied_ID[i];
		double xx = mesh.node[id0].getX(); double yy = mesh.node[id0].getY(); double zz = mesh.node[id0].getZ();
		/* まず鏡像面まで回転 */
		double rad = rot_deg * DEF_CONV_PI / 180.0;
		double rotMAT[3][3];
		if(dir == 0){
			rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
			rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
		}else if(dir == 1){
			rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
			rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
		}else{
			rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
		}
		double newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
		double newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
		double newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;

		/* 対応する節点を探す */
		for(convint j = 0 ; j < node_size0 ; j++){
			double xx2 = mesh.node[j].getX();
			double yy2 = mesh.node[j].getY();
			double zz2 = mesh.node[j].getZ();
			double diff = sqrt( (newX-xx2)*(newX-xx2) + (newY-yy2)*(newY-yy2) + (newZ-zz2)*(newZ-zz2) );
			if( diff < eps_diff){
				AfterNode.insert( pair<convint, convint>(id0, j) );
			}
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "Map size => " << AfterNode.size() << endl;
#endif
	/* 1回転で増える数 */
	const convint increment_size = node_size0 - not_copy_size;
#ifdef PRINT_CONV_INFO
	cout << "Inc size => " << increment_size <<endl;
#endif
	/* 初期要素数 */
	const convint element_size0 = mesh.element.size();

	convint aft_size = AfterNode.size();
	if(aft_size != not_copy_size){
		cout << "Mapping node data is not correct ! please adjust eps_diff value !!! " << endl;

		while(true){
		}

		exit(1);
	}


	RotCopy_SubRutin(mesh, node_del, map_make, dir, rot_deg, cp_times,
		node_size0, element_size0, increment_size, Not_copied_Node, AfterNode, New_nodeID, is_inc_mat, inc_val, Ele_map);


	delete[] Not_copied_Node;
	delete[] New_nodeID;
}


/*
//=======================================================
// ● 回転コピー(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す) 
//=======================================================*/
/** 
 * @brief copy elements (rotation)
*/
void MeshConverter::RotCopy(ConvMesh& mesh, int dir, double rot_deg, int cp_times, int node_del, int map_make, int is_inc_mat, int inc_val){
#ifdef PRINT_CONV_INFO
	cout << "Rotation Copy " << endl;
#endif
	/*int dir;
	double rot_deg;
	int cp_times;
	int node_del;
	int map_make;
	int is_inc_mat;
	int inc_val;*/

	/* 初期サイズ */
	const convint node_size0 = mesh.node.size();

	vector<convint> Not_copied_ID;	
	bool *Not_copied_Node = new bool[node_size0];
	convint *New_nodeID = new convint[node_size0];
	convint counter=0;
	/* コピーしない節点サーチ */
	for(convint i = 0 ; i < node_size0 ;i++){
		Not_copied_Node[i] = false;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		New_nodeID[i] = i - counter;

		if(node_del == 1){
			bool bl1 = (dir == 0 && zz < eps_diff);
			bool bl2 = (dir == 1 && xx < eps_diff);
			bool bl3 = (dir == 2 && yy < eps_diff);
			if(bl1 || bl2 || bl3){
				Not_copied_ID.push_back(i);
				Not_copied_Node[i] = true;
				counter++;
			}
		}
	}

	const convint not_copy_size = Not_copied_ID.size();
	map<convint, convint> AfterNode;
	//cout << "on Surf Node size => " <<not_copy_size << endl;
	for(convint i = 0 ; i < not_copy_size ;i++){
		convint id0 =  Not_copied_ID[i];
		double xx = mesh.node[id0].getX(); double yy = mesh.node[id0].getY(); double zz = mesh.node[id0].getZ();
		/* まず鏡像面まで回転 */
		double rad = rot_deg * DEF_CONV_PI / 180.0;
		double rotMAT[3][3];
		if(dir == 0){
			rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
			rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
		}else if(dir == 1){
			rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
			rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
		}else{
			rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
			rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
			rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
		}
		double newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
		double newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
		double newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;

		/* 対応する節点を探す */
		for(int j = 0 ; j < node_size0 ; j++){
			double xx2 = mesh.node[j].getX();
			double yy2 = mesh.node[j].getY();
			double zz2 = mesh.node[j].getZ();
			double diff = sqrt( (newX-xx2)*(newX-xx2) + (newY-yy2)*(newY-yy2) + (newZ-zz2)*(newZ-zz2) );
			if( diff < eps_diff){
				AfterNode.insert( pair<convint, convint>(id0, j) );
			}
		}
	}

	/* 1回転で増える数 */
	const convint increment_size = node_size0 - not_copy_size;
	/* 初期要素数 */
	const convint element_size0 = mesh.element.size();

	convint aft_size = AfterNode.size();
	if(aft_size != not_copy_size){
		cout << "Mapping node data is not correct ! please adjust eps_diff value !!! " << endl;
		exit(1);
	}

	convint** Ele_map = new convint*[element_size0];
	for(int i = 0 ; i < element_size0 ;i++){
		Ele_map[i] = new convint[cp_times];
	}

	RotCopy_SubRutin(mesh, node_del, map_make, dir, rot_deg, cp_times,
		node_size0, element_size0, increment_size, Not_copied_Node, AfterNode, New_nodeID, is_inc_mat, inc_val, Ele_map);

	if(map_make == 1){
		std::fstream fp("MappingRotation.csv", std::ios::out);
		fp << element_size0 << endl;
		for(int i = 0 ; i < element_size0 ;i++){
			fp << i ;
			for(int j = 0 ; j < cp_times ;j++){
				fp << ", " << Ele_map[i][j] << endl;
			}
			fp << endl;
		}
		fp.close();
	}
	for(convint i = 0 ; i < element_size0 ;i++){
		delete[] Ele_map[i];
	}
	delete[] Ele_map;

	delete[] Not_copied_Node;
	delete[] New_nodeID;
}


/*
//=======================================================
// ● 回転コピーサブルーチン(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す) 
//=======================================================*/
void MeshConverter::RotCopy_SubRutin(ConvMesh& mesh, bool node_del, bool map_make, int dir, double rot_deg, int cp_times,
	convint node_size0, convint element_size0, convint increment_size, bool *Not_copied_Node, map<convint, convint>& AfterNode, convint *New_nodeID, int is_inc_mat, int inc_val, convint** Ele_map){

	convint count_n=0;
	for(int nnn = 0  ; nnn < cp_times ; nnn++){
		//cout << "ROT => " << nnn << endl;
		count_n=0;
		/* 節点追加 */
		for(convint i = 0 ; i < node_size0 ;i++){
			/* 無視節点は無視 */
			if(Not_copied_Node[i]) continue;
			/* 回転 */
			double xx = mesh.node[i].getX();
			double yy = mesh.node[i].getY();
			double zz = mesh.node[i].getZ();
			double rad = (nnn+1)*rot_deg * DEF_CONV_PI / 180.0;
			double rotMAT[3][3];
			if(dir == 0){
				rotMAT[0][0] = 1.0;        rotMAT[0][1] = 0.0;        rotMAT[0][2] = 0.0; 
				rotMAT[1][0] = 0.0;        rotMAT[1][1] = cos(rad);   rotMAT[1][2] = -1.0*sin(rad); 
				rotMAT[2][0] = 0.0;        rotMAT[2][1] = sin(rad);   rotMAT[2][2] = cos(rad); 
			}else if(dir == 1){
				rotMAT[0][0] = cos(rad);     rotMAT[0][1] = 0.0;        rotMAT[0][2] = sin(rad); 
				rotMAT[1][0] = 0.0;          rotMAT[1][1] = 1.0;        rotMAT[1][2] = 0.0; 
				rotMAT[2][0] = -1.0*sin(rad);rotMAT[2][1] = 0.0;        rotMAT[2][2] = cos(rad); 
			}else{
				rotMAT[0][0] = cos(rad);        rotMAT[0][1] = -1.0*sin(rad);rotMAT[0][2] = 0.0; 
				rotMAT[1][0] = sin(rad);        rotMAT[1][1] = cos(rad);     rotMAT[1][2] = 0.0; 
				rotMAT[2][0] = 0.0;             rotMAT[2][1] = 0.0;          rotMAT[2][2] = 1.0; 
			}
			double newX = rotMAT[0][0]*xx + rotMAT[0][1]*yy + rotMAT[0][2]*zz;
			double newY = rotMAT[1][0]*xx + rotMAT[1][1]*yy + rotMAT[1][2]*zz;
			double newZ = rotMAT[2][0]*xx + rotMAT[2][1]*yy + rotMAT[2][2]*zz;

			/* 節点作成 */
			ConvNode N_temp;
			convint id_new = count_n + node_size0 + nnn*increment_size;
			N_temp.setID(id_new);
			N_temp.set(newX, newY, newZ);
			mesh.node.push_back(N_temp);
			count_n++;
		}

		/* 要素追加 */
		for(convint i = 0 ; i < element_size0 ;i++){
			ConvElement etmp;
			int ori_m = mesh.element[i].getMat();
			etmp = mesh.element[i];
			if(is_inc_mat == 1){
				const int new_mat = ori_m + (nnn+1)*inc_val;
				etmp.setMat(new_mat);
			}
			convint node_num = mesh.element[i].getNodeNum();
			/* 節点設定！ */
			for(int kk = 0 ; kk < node_num ; kk++){
				convint preN = mesh.element[i].getNode(kk);
				if(Not_copied_Node[preN]){
					//cout << AfterNode[preN] << endl;
					convint aft_node = AfterNode[preN];
					if(nnn==0){
						etmp.setNodes(kk, aft_node);
					}else{
						const convint n_id_new = New_nodeID[aft_node] + node_size0 + (nnn-1)*increment_size;
						etmp.setNodes(kk, n_id_new);
					}
				}else{
					//cout << preN << " => " << New_nodeID[preN] << " ===>>> "<< New_nodeID[preN] + node_size0 + nnn*increment_size<<endl;
					const convint n_id_new = New_nodeID[preN] + node_size0 + nnn*increment_size;
					etmp.setNodes(kk, n_id_new);
				}
			}
			convint ee_id = mesh.element.size();
			etmp.setID(ee_id);
			/* 確定 */
			mesh.element.push_back( std::move(etmp) );

			Ele_map[i][nnn] = ee_id;
		}
	}
}


/*
//=======================================================
// ● 移動コピー(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す
//=======================================================*/
/** 
 * @brief copy elements (linear move)
*/
void MeshConverter::MoveCopy(ConvMesh& mesh, int dir, double move_val, int cp_times, int node_del, int map_make, int is_inc_mat, int inc_val){
#ifdef PRINT_CONV_INFO
	cout << "MoveCopy " << endl;
#endif
	/*int dir;
	double move_val;
	int cp_times;
	int node_del;
	int map_make;
	int is_inc_mat;
	int inc_val;*/

	/* 初期サイズ */
	const convint node_size0 = mesh.node.size();

	vector<convint> Not_copied_ID;	
	bool *Not_copied_Node = new bool[node_size0];
	convint *New_nodeID = new convint[node_size0];
	convint counter=0;
	/* コピーしない節点サーチ */
	for(convint i = 0 ; i < node_size0 ;i++){
		Not_copied_Node[i] = false;
		double xx = mesh.node[i].getX(); double yy = mesh.node[i].getY(); double zz = mesh.node[i].getZ();
		New_nodeID[i] = i - counter;

		if(node_del == 1){
			bool bl1 = (dir == 0 && fabs(xx-move_val) < eps_diff);
			bool bl2 = (dir == 1 && fabs(yy-move_val) < eps_diff);
			bool bl3 = (dir == 2 && fabs(zz-move_val) < eps_diff);
			if(bl1 || bl2 || bl3){
				Not_copied_ID.push_back(i);
				Not_copied_Node[i] = true;
				counter++;
			}
		}
	}

	const convint not_copy_size = Not_copied_ID.size();
	map<convint, convint> AfterNode;
	//cout << "on Surf Node size => " <<not_copy_size << endl;
	for(convint i = 0 ; i < not_copy_size ;i++){
		convint id0 =  Not_copied_ID[i];
		double xx = mesh.node[id0].getX(); double yy = mesh.node[id0].getY(); double zz = mesh.node[id0].getZ();
		/* 逆移動へまで回転 */
		double newX = xx;
		double newY = yy;
		double newZ = zz;
		if(dir == 0){
			newX -= move_val;
		}
		if(dir == 1){
			newY -= move_val;
		}
		if(dir == 2){
			newZ -= move_val;
		}

		/* 対応する節点を探す */
		for(convint j = 0 ; j < node_size0 ; j++){
			double xx2 = mesh.node[j].getX();
			double yy2 = mesh.node[j].getY();
			double zz2 = mesh.node[j].getZ();
			double diff = sqrt( (newX-xx2)*(newX-xx2) + (newY-yy2)*(newY-yy2) + (newZ-zz2)*(newZ-zz2) );
			if( diff < eps_diff){
				AfterNode.insert( pair<convint, convint>(id0, j) );
			}
		}
	}
	/* 1回転で増える数 */
	const convint increment_size = node_size0 - not_copy_size;
	/* 初期要素数 */
	const convint element_size0 = mesh.element.size();

	cout << "inc size " <<  increment_size << endl;

	convint aft_size = AfterNode.size();
	if(aft_size != not_copy_size){
		cout << "Mapping node data is not correct ! please adjust eps_diff value !!! " << endl;
		exit(1);
	}

	convint** Ele_map = new convint*[element_size0];
	for(convint i = 0 ; i < element_size0 ;i++){
		Ele_map[i] = new convint[cp_times];
	}

	MoveCopy_SubRutin(mesh, node_del, map_make, dir, move_val, cp_times,
		node_size0, element_size0, increment_size, Not_copied_Node, AfterNode, New_nodeID, is_inc_mat, inc_val, Ele_map);

	if(map_make == 1){
		std::fstream fp("MappingMove.csv", std::ios::out);
		fp << element_size0 << endl;
		//fprintf(fp, "%d\n", element_size0);
		for(convint i = 0 ; i < element_size0 ;i++){
			fp << i;
			for(int j = 0 ; j < cp_times ;j++){
				fp << ", " << Ele_map[i][j];
			}
			fp << endl;
		}
		fp.close();
	}
	for(convint i = 0 ; i < element_size0 ;i++){
		delete[] Ele_map[i];
	}
	delete[] Ele_map;

	delete[] Not_copied_Node;
	delete[] New_nodeID;
}

/*
//=======================================================
// ● 移動コピーサブルーチン(node_del:コピー時に重複削除, map_make:コピー時に、コピー元とコピー先の要素のマッピングを書き出す
//=======================================================*/
void MeshConverter::MoveCopy_SubRutin(ConvMesh& mesh, bool node_del, bool map_make, int dir, double move_val, int cp_times,
	convint node_size0, convint element_size0, convint increment_size, bool *Not_copied_Node, map<convint, convint>& AfterNode, convint *New_nodeID, int is_inc_mat, int inc_val, convint** Ele_map){

	convint count_n=0;
	for(int nnn = 0  ; nnn < cp_times ; nnn++){
		cout << "MV => " << nnn << endl;
		count_n=0;
		/* 節点追加 */
		for(convint i = 0 ; i < node_size0 ;i++){
			/* 無視節点は無視 */
			if(Not_copied_Node[i]) continue;
			/* 回転 */
			double xx = mesh.node[i].getX();
			double yy = mesh.node[i].getY();
			double zz = mesh.node[i].getZ();
			double newX = xx;
			double newY = yy;
			double newZ = zz;
			if(dir == 0){
				newX += (nnn+1)*move_val;
			}
			if(dir == 1){
				newY += (nnn+1)*move_val;
			}
			if(dir == 2){
				newZ += (nnn+1)*move_val;
			}
			/* 節点作成 */
			ConvNode N_temp;
			convint id_new = count_n + node_size0 + nnn*increment_size;
			N_temp.setID(id_new);
			N_temp.set(newX, newY, newZ);
			mesh.node.push_back(N_temp);
			count_n++;
		}

		/* 要素追加 */
		for(convint i = 0 ; i < element_size0 ;i++){
			ConvElement etmp;
			int ori_m = mesh.element[i].getMat();
			etmp = mesh.element[i];
			if(is_inc_mat == 1){
				const int new_mat = ori_m + (nnn+1)*inc_val;
				etmp.setMat(new_mat);
			}
			convint node_num = mesh.element[i].getNodeNum();
			/* 節点設定！ */
			for(int kk = 0 ; kk < node_num ; kk++){
				convint preN = mesh.element[i].getNode(kk);
				if(Not_copied_Node[preN]){
					//cout << AfterNode[preN] << endl;
					convint aft_node = AfterNode[preN];
					if(nnn==0){
						etmp.setNodes(kk, aft_node);
					}else{
						const convint n_id_new = New_nodeID[aft_node] + node_size0 + (nnn-1)*increment_size;
						etmp.setNodes(kk, n_id_new);
					}
				}else{
					//cout << preN << " => " << New_nodeID[preN] << " ===>>> "<< New_nodeID[preN] + node_size0 + nnn*increment_size<<endl;
					const convint n_id_new = New_nodeID[preN] + node_size0 + nnn*increment_size;
					etmp.setNodes(kk, n_id_new);
				}
			}
			convint ee_id = mesh.element.size();
			etmp.setID(ee_id);
			/* 確定 */
			mesh.element.push_back( std::move(etmp) );

			Ele_map[i][nnn] = ee_id;
		}
	}
}




/**/
};
};


