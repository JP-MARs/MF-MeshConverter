/**
* @file modify_add_lines.cpp
* @brief modify_add_lines 
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>

namespace JPMRspace{
namespace MeshConvLib{




/*
//=======================================================
// ● 指定した座標を始点・終点にする線要素を追加する
//=======================================================*/
/** 
 * @brief add line element
*/
void MeshConverter::line_add(ConvMesh& mesh, double* xyz1, double* xyz2, int add_mat){
#ifdef PRINT_CONV_INFO
	cout << "Add line" << endl << endl;
#endif
	double x1, y1, z1;
	double x2, y2, z2;
	int mat_id;

	x1 = xyz1[0];
	y1 = xyz1[1];
	z1 = xyz1[2];

	x2 = xyz2[0];
	y2 = xyz2[1];
	z2 = xyz2[2];

	mat_id = add_mat;
	
	/* 指定された座標のepsdiff内に節点があるかサーチ */
	convint id1 = -999, id2=-999;
	convint n_size = mesh.node.size();
	double min1 = 999999;
	double min2 = 999999;
	for(convint i = 0 ; i < n_size ; i++){
		double nx = mesh.node[i].getX();
		double ny = mesh.node[i].getY();
		double nz = mesh.node[i].getZ();
		double dx1 = (nx-x1)*(nx-x1);
		double dy1 = (ny-y1)*(ny-y1);
		double dz1 = (nz-z1)*(nz-z1);
		double diff1 = sqrt( dx1 + dy1 + dz1);
		if(min1 > diff1){
			min1 = diff1;
			id1 = i;
		}
		double dx2 = (nx-x2)*(nx-x2);
		double dy2 = (ny-y2)*(ny-y2);
		double dz2 = (nz-z2)*(nz-z2);
		double diff2 = sqrt( dx2 + dy2 + dz2);
		if(min2 > diff2){
			min2 = diff2;
			id2 = i;
		}
	}
	convint start_id = -999;
	convint end_id = -999;
	convint newID = n_size;
	/* 始点・終点があればそれを、なければ新規作成 */
	if(min1 < eps_diff){
		start_id = id1;
	}else{
		ConvNode tempN1;
		tempN1.set(x1, y1, z1);
		tempN1.setID(newID);
		mesh.node.push_back(tempN1);
		start_id = newID;//+1;
		newID++;
	}
	if(min2 < eps_diff){
		end_id = id2;
	}else{
		ConvNode tempN1;
		tempN1.set(x2, y2, z2);
		tempN1.setID(newID);
		mesh.node.push_back(tempN1);
		end_id = newID;//+1;
		newID++;
	}
	n_size = mesh.node.size();
	/*  */
	/* 線要素追加 */
	convint newELE = mesh.element.size();
	ConvElement etmp;
	convint inode[10];
	inode[0] = start_id;
	inode[1] = end_id;
	etmp.setType(ConvEType::Line);
	etmp.setMat(mat_id);
	etmp.setID(newELE);
	etmp.setNodes(inode);

	mesh.element.push_back(std::move(etmp));
}



/*
//=======================================================
// ● 指定した節点IDから線要素を追加する
//=======================================================*/
/** 
 * @brief add line element from ID
*/
void MeshConverter::line_add2(ConvMesh& mesh, convint sID0, convint eID0, int add_mat){
#ifdef PRINT_CONV_INFO
	cout << "Add line 2" << endl << endl;
#endif
	convint id1 = sID0;
	convint id2 = eID0;
	int mat_id = add_mat;

	convint start_id = id1;
	convint end_id = id2;
	/* 線要素追加 */
	convint newELE = mesh.element.size();
	ConvElement etmp;
	convint inode[10];
	inode[0] = start_id;
	inode[1] = end_id;
	etmp.setType(ConvEType::Line);
	etmp.setMat(mat_id);
	etmp.setID(newELE);
	etmp.setNodes(inode);

	mesh.element.push_back(std::move(etmp));
}


/*
//=======================================================
// ● 指定した始点節点・終点節点の間にある辺を抽出し、線要素として作成 
//=======================================================*/
/** 
 * @brief add line elements between two points
*/
void MeshConverter::line_add_M(ConvMesh& mesh, const vector<convint>& sID0,  const vector<convint>& eID0, int add_mat){
#ifdef PRINT_CONV_INFO
	cout << "Add line M" << endl << endl;
#endif
	vector<convint> start_id_all;
	vector<convint> end_id_all;

	const int mat_id = add_mat;

	/* 指定データ数 */
	int line_num = sID0.size();
	for(int iii = 0 ; iii < line_num ; iii++){

		convint start_id = sID0[iii];
		convint end_id = eID0[iii];
		if( mesh.edge.empty() ){
			mesh.makeMeshInfo();
		}
		mesh.info_maked = false;

		/* 指定した始点・終点の座標をゲット */
		double x1,x2,y1,y2,z1,z2;
		x1 = mesh.node[start_id].getX(); y1 = mesh.node[start_id].getY(); z1 = mesh.node[start_id].getZ();
		x2 = mesh.node[end_id].getX(); y2 = mesh.node[end_id].getY(); z2 = mesh.node[end_id].getZ();
		double vecW[3] = {x2-x1, y2-y1, z2-z1};
		double normW=0;
		for(int k=0 ; k < 3 ; k++){
			normW += vecW[k]*vecW[k];
		}
		double vecS[3] = {x1, y1, z1};
		/* 全ての辺をサーチ */
		vector<convint> on_search_edge;
		const convint ed_size00 = mesh.edge.size();
		for(convint i=0 ; i < ed_size00 ; i++){
			double xx1,yy1,zz1;
			double xx2,yy2,zz2;
			convint sID = mesh.edge[i].getSID();
			convint eID = mesh.edge[i].getEID();
			xx1 = mesh.node[sID].getX(); yy1 = mesh.node[sID].getY(); zz1 = mesh.node[sID].getZ(); 
			xx2 = mesh.node[eID].getX(); yy2 = mesh.node[eID].getY(); zz2 = mesh.node[eID].getZ(); 
			double vec1[3] = {xx1, yy1, zz1};
			double vec2[3] = {xx2, yy2, zz2};
			double cross1=0, cross2=0;
			for(int k=0 ; k < 3 ; k++){
				double temp = vec1[k] - vecS[k];
				cross1 += vecW[k]*temp;
				temp = vec2[k] - vecS[k];
				cross2 += vecW[k]*temp;
			}
			double tt1 = cross1 / normW;
			double tt2 = cross2 / normW;
			//cout << tt1 << ", " << tt2<<endl;
			/* 始点と一致するか */
			double ansA1 = vecW[0]*tt1 + x1;
			double ansA2 = vecW[1]*tt1 + y1;
			double ansA3 = vecW[2]*tt1 + z1;
			double dx1 = ansA1-xx1;
			double dy1 = ansA2-yy1;
			double dz1 = ansA3-zz1;
			double diff1 = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
			/* 終点と一致するか */
			double ansB1 = vecW[0]*tt2 + x1;
			double ansB2 = vecW[1]*tt2 + y1;
			double ansB3 = vecW[2]*tt2 + z1;
			dx1 = ansB1-xx2;
			dy1 = ansB2-yy2;
			dz1 = ansB3-zz2;
			double diff2 = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
			/* 一致したらOK */
			if(diff1 < eps_diff && diff2 < eps_diff && tt1 >= -1.0*eps_diff && tt1 <= 1.0+eps_diff && tt2 > -1.0*eps_diff &&  tt2 <= 1.0+eps_diff ){
				on_search_edge.push_back(i);
				//cout << xx1 << ", " << yy1 << " : " << xx2 << ", " << yy2<<endl;
			}
		}
		/* 発見した辺を線要素として追加 */
		for(convint i=0 ; i < on_search_edge.size() ; i++){
			convint sID = mesh.edge[on_search_edge[i]].getSID();
			convint eID = mesh.edge[on_search_edge[i]].getEID();
			double xx1,yy1,zz1;
			double xx2,yy2,zz2;
			xx1 = mesh.node[sID].getX(); yy1 = mesh.node[sID].getY(); zz1 = mesh.node[sID].getZ(); 
			xx2 = mesh.node[eID].getX(); yy2 = mesh.node[eID].getY(); zz2 = mesh.node[eID].getZ(); 
			double vecE[3] = {xx2-xx1, yy2-yy1, zz2-zz1};
			/* 向きが逆な入力した線要素向きとそろえる */
			double cross = vecW[0]*vecE[0] + vecW[1]*vecE[1] + vecW[2]*vecE[2];
			if(cross < 0.0){
				convint tmp = sID;
				sID = eID;
				eID = tmp;
			}
			/* 線要素追加 */
			convint newELE = mesh.element.size();
			ConvElement etmp;
			convint inode[10];
			inode[0] = sID;
			inode[1] = eID;
			etmp.setType(ConvEType::Line);
			etmp.setMat(mat_id);
			etmp.setID(newELE);
			etmp.setNodes(inode);
			mesh.element.push_back(std::move(etmp));
		}
	}
}


/*
//=======================================================
// ● 指定した座標を始点・終点の間にある辺を抽出し、線要素として作成
//=======================================================*/
/** 
 * @brief add line elements between two positions
*/
void MeshConverter::line_add_M_pos(ConvMesh& mesh, double* xyz1, double* xyz2, int add_mat){
#ifdef PRINT_CONV_INFO
	cout << "Add line M from Pos" << endl << endl;
#endif
	convint id1, id2;
	convint start_id;
	convint end_id;
	double s_x, s_y, s_z;
	double e_x, e_y, e_z;

	s_x = xyz1[0];
	s_y = xyz1[1];
	s_z = xyz1[2];

	e_x = xyz2[0];
	e_y = xyz2[1];
	e_z = xyz2[2];

	int mat_id = add_mat;

	/* 始点と終点をサーチ */
	convint n_size = mesh.node.size();
	double min1 = 999999;
	double min2 = 999999;
	for (convint i = 0; i < n_size; i++) {
		double nx = mesh.node[i].getX();
		double ny = mesh.node[i].getY();
		double nz = mesh.node[i].getZ();
		double dx1 = (nx - s_x)*(nx - s_x);
		double dy1 = (ny - s_y)*(ny - s_y);
		double dz1 = (nz - s_z)*(nz - s_z);
		double diff1 = sqrt(dx1 + dy1 + dz1);
		if (min1 > diff1) {
			min1 = diff1;
			id1 = i;
		}
		double dx2 = (nx - e_x)*(nx - e_x);
		double dy2 = (ny - e_y)*(ny - e_y);
		double dz2 = (nz - e_z)*(nz - e_z);
		double diff2 = sqrt(dx2 + dy2 + dz2);
		if (min2 > diff2) {
			min2 = diff2;
			id2 = i;
		}
	}
	start_id = id1;
	end_id = id2;
#ifdef PRINT_CONV_INFO
	cout << "find nodes => " << start_id << ", " << end_id << endl;
#endif
	if (mesh.edge.empty()) {
		mesh.makeMeshInfo();
	}
	mesh.info_maked = false;

	/* 指定した始点・終点の座標をゲット */
	double x1, x2, y1, y2, z1, z2;
	x1 = mesh.node[start_id].getX(); y1 = mesh.node[start_id].getY(); z1 = mesh.node[start_id].getZ();
	x2 = mesh.node[end_id].getX(); y2 = mesh.node[end_id].getY(); z2 = mesh.node[end_id].getZ();
	double vecW[3] = { x2 - x1, y2 - y1, z2 - z1 };
	double normW = 0;
	for (int k = 0; k < 3; k++) {
		normW += vecW[k] * vecW[k];
	}
	double vecS[3] = { x1, y1, z1 };
	/* 全ての辺をサーチ */
	vector<convint> on_search_edge;
	const convint ed_size00 = mesh.edge.size();
	for (convint i = 0; i < ed_size00; i++) {
		double xx1, yy1, zz1;
		double xx2, yy2, zz2;
		convint sID = mesh.edge[i].getSID();
		convint eID = mesh.edge[i].getEID();
		xx1 = mesh.node[sID].getX(); yy1 = mesh.node[sID].getY(); zz1 = mesh.node[sID].getZ();
		xx2 = mesh.node[eID].getX(); yy2 = mesh.node[eID].getY(); zz2 = mesh.node[eID].getZ();
		double vec1[3] = { xx1, yy1, zz1 };
		double vec2[3] = { xx2, yy2, zz2 };
		double cross1 = 0, cross2 = 0;
		for (int k = 0; k < 3; k++) {
			double temp = vec1[k] - vecS[k];
			cross1 += vecW[k] * temp;
			temp = vec2[k] - vecS[k];
			cross2 += vecW[k] * temp;
		}
		double tt1 = cross1 / normW;
		double tt2 = cross2 / normW;
		//cout << tt1 << ", " << tt2<<endl;
		/* 始点と一致するか */
		double ansA1 = vecW[0] * tt1 + x1;
		double ansA2 = vecW[1] * tt1 + y1;
		double ansA3 = vecW[2] * tt1 + z1;
		double dx1 = ansA1 - xx1;
		double dy1 = ansA2 - yy1;
		double dz1 = ansA3 - zz1;
		double diff1 = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
		/* 終点と一致するか */
		double ansB1 = vecW[0] * tt2 + x1;
		double ansB2 = vecW[1] * tt2 + y1;
		double ansB3 = vecW[2] * tt2 + z1;
		dx1 = ansB1 - xx2;
		dy1 = ansB2 - yy2;
		dz1 = ansB3 - zz2;
		double diff2 = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
		/* 一致したらOK */
		if (diff1 < eps_diff && diff2 < eps_diff && tt1 >= -1.0*eps_diff && tt1 <= 1.0 + eps_diff && tt2 > -1.0*eps_diff &&  tt2 <= 1.0 + eps_diff) {
			on_search_edge.push_back(i);
			//cout << xx1 << ", " << yy1 << " : " << xx2 << ", " << yy2<<endl;
		}
	}
	/* 発見した辺を線要素として追加 */
	for (convint i = 0; i < on_search_edge.size(); i++) {
		convint sID = mesh.edge[on_search_edge[i]].getSID();
		convint eID = mesh.edge[on_search_edge[i]].getEID();
		double xx1, yy1, zz1;
		double xx2, yy2, zz2;
		xx1 = mesh.node[sID].getX(); yy1 = mesh.node[sID].getY(); zz1 = mesh.node[sID].getZ();
		xx2 = mesh.node[eID].getX(); yy2 = mesh.node[eID].getY(); zz2 = mesh.node[eID].getZ();
		double vecE[3] = { xx2 - xx1, yy2 - yy1, zz2 - zz1 };
		/* 向きが逆なら入力した線要素向きとそろえる */
		double cross = vecW[0] * vecE[0] + vecW[1] * vecE[1] + vecW[2] * vecE[2];
		if (cross < 0.0) {
			convint tmp = sID;
			sID = eID;
			eID = tmp;
		}
		/* 線要素追加 */
		convint newELE = mesh.element.size();
		ConvElement etmp;
		convint inode[10];
		inode[0] = sID;
		inode[1] = eID;
		etmp.setType(ConvEType::Line);
		etmp.setMat(mat_id);
		etmp.setID(newELE);
		etmp.setNodes(inode);
		mesh.element.push_back( std::move(etmp) );
	}
}


/*
//=======================================================
// ● 指定した円弧を描き、その円弧に乗っている辺を抽出、線要素として作成
//=======================================================*/
/** 
 * @brief add line element from arch
*/
void MeshConverter::line_add_R(ConvMesh& mesh, double targetR, double init_th, double end_th,double targetZ, double EPS_DIFF_R, int add_mat){
#ifdef PRINT_CONV_INFO
	cout << "Add line R" << endl << endl;
#endif
	/*double targetR;
	double init_th, end_th;
	double init_rad, end_rad;
	double targetZ;
	double EPS_DIFF_R;*/
	int mat_id = add_mat;

	/* 変換 */
	double init_rad = init_th * DEF_CONV_PI / 180.0;
	double end_rad = end_th * DEF_CONV_PI / 180.0;

	/* 辺データがなかったら作る */
	if (mesh.edge.empty()) {
		mesh.makeMeshInfo();
	}
	mesh.info_maked = false;

	/* 全ての辺をサーチ */
	vector<convint> on_search_edge;
	const convint ori_ele_num = mesh.element.size();
	const convint ed_size00 = mesh.edge.size();
	for (convint i = 0; i < ed_size00; i++) {
		double xx1, yy1, zz1;
		double xx2, yy2, zz2;
		convint sID = mesh.edge[i].getSID();
		convint eID = mesh.edge[i].getEID();
		xx1 = mesh.node[sID].getX(); yy1 = mesh.node[sID].getY(); zz1 = mesh.node[sID].getZ();
		xx2 = mesh.node[eID].getX(); yy2 = mesh.node[eID].getY(); zz2 = mesh.node[eID].getZ();
		double th1 = atan2(yy1, xx1);
		double th2 = atan2(yy2, xx2);
		double radA,radB;
		convint init_id,end_id;
		if(th1 > th2){
			radA = th2;
			radB = th1;
			init_id = eID;
			end_id = sID;
		}else{
			radA = th1;
			radB = th2;
			init_id = sID;
			end_id = eID;
		}
		const double gx = 0.5*(xx1 + xx2);
		const double gy = 0.5*(yy1 + yy2);
		const double gz = 0.5*(zz1 + zz2);
		const double gr = sqrt(gx*gx + gy*gy);
		const double gr1 = sqrt(xx1*xx1 + yy1*yy1);
		const double gr2 = sqrt(xx2*xx2 + yy2*yy2);
		double diff_gr = fabs(gr - targetR);
		double diff_r1 = fabs(gr1 - targetR);
		double diff_r2 = fabs(gr2 - targetR);
		bool diff_r = ( (diff_r1 < EPS_DIFF_R) && (diff_r2 < EPS_DIFF_R) );
		double diff_z = fabs(gz - targetZ);
		double diff_rad = fabs(radA-radB);
		/* 円弧に乗っているなら、線要素に追加 */
		if(diff_r && diff_z < EPS_DIFF_R && init_rad-1.0e-6 <= radA && radB <= end_rad+1.0e-6){
			/* 線要素追加 */
			convint newELE = mesh.element.size();
			ConvElement etmp;
			convint inode[10];
			inode[0] = init_id;
			inode[1] = end_id;
			etmp.setType(ConvEType::Line);
			etmp.setMat(mat_id);
			etmp.setID(newELE);
			etmp.setNodes(inode);
			mesh.element.push_back( std::move(etmp) );
		}
	}
	const convint after_ele_num = mesh.element.size();
#ifdef PRINT_CONV_INFO
	cout << "New edge elements ... >> " << after_ele_num-ori_ele_num << endl;
#endif
	mesh.edge.clear();
}


/*
//=======================================================
// ● 指定した座標の節点を生成
//=======================================================*/
/** 
 * @brief add node data
*/
void MeshConverter::make_node(ConvMesh& mesh, double* xyz){
	double xx, yy, zz;

	xx = xyz[0];
	yy = xyz[1];
	zz = xyz[2];

	ConvNode tempN;
	tempN.set(xx, yy, zz);
	convint id0 = mesh.node.size();
	tempN.setID(id0);
	mesh.node.push_back(tempN);
#ifdef PRINT_CONV_INFO
	cout << "node add ! "<<xx << ", " << yy << ", " << zz << ", " << id0 <<endl;
#endif
	mesh.info_maked = false;
}




/**/
};
};


