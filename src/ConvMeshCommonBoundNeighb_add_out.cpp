/**
* @file ConvMeshCommonBoundNeighb_add_out.cpp
* @brief define neibhouring add
*
*/
#include <mfMeshConverter/ConvMeshCommon.hpp>
#include <fstream>
#include <iomanip>

namespace JPMRspace{
namespace MeshConvLib{


/*
//=======================================================
// ■ MeshCommon
//=======================================================
// Mesh共通作業staticクラス
//=======================================================*/

/*
//===============================================================
// ○ 辺を線要素として加える
//===============================================================*/
/** 
 * @brief add line elements fron edge data
*/
void ConvMesh::addLineElemets_fromEdge(){
	const convint e_size = edge.size();
	for(convint i = 0 ; i < e_size ; i++){
		ConvElement ele;
		ele.setType(ConvEType::Line);
		ele.setMat(9999);
		convint sID = edge[i].getSID();
		convint eID = edge[i].getEID();
		convint nn[2] = {sID, eID};
		ele.setNodes(nn);
		convint ee[1] = {i};
		ele.setEdges(ee);
		element.push_back( std::move(ele) );
	}
}
/*
//===============================================================
// ○ 境界の辺を線要素として加える
//===============================================================*/
/** 
 * @brief add line elements from boundary edges
*/
void ConvMesh::addSurfLineElemets_fromEdge(){
	const convint the_size = boundEdgeList.size();
	for(convint i = 0 ; i < the_size ; i++){
		ConvElement ele;
		ele.setType(ConvEType::Line);
		ele.setMat(9999);
		convint sID = edge[boundEdgeList[i]].getSID();
		convint eID = edge[boundEdgeList[i]].getEID();
		convint nn[2] = {sID, eID};
		ele.setNodes(nn);
		convint ee[1] = {i};
		ele.setEdges(ee);
		element.push_back( std::move(ele) );
	}
}

/*
//===============================================================
// ○ 面を2D要素として加える
//===============================================================*/
/** 
 * @brief add elements from 2D eleents
*/
void ConvMesh::addSurfaceElemets_fromFacet(){
	const convint f_size = facets.size();
	for(convint i = 0 ; i < f_size ; i++){
		ConvEType f_type = facets[i].getType();

		const int num_n = facets[i].getNodeNum();
		const int num_e = facets[i].getEdgeNum();
		vector<convint> nodes;
		facets[i].getNodes(nodes);
		vector<convint> edges;
		facets[i].getEdges(edges);

		ConvElement ele;
		ele.setType(f_type);
		ele.setNodes(nodes);
		ele.setEdges(edges);
		ele.setMat(9998);
		element.push_back( std::move(ele) );
	}
}

/*
//===============================================================
// ○ 境界の面を2D要素として加える
//===============================================================*/
/** 
 * @brief add 2D elements from boundary facets
*/
void ConvMesh::addBoundSurfaceElemets_fromFacet(){
	const convint bond_f_size = boundFacetList.size();	
	for(convint i = 0 ; i < bond_f_size ; i++){
		const convint id = boundFacetList[i];
		ConvEType f_type = facets[id].getType();

		const int num_n = facets[id].getNodeNum();
		const int num_e = facets[id].getEdgeNum();
		vector<convint> nodes;
		facets[id].getNodes(nodes);
		vector<convint> edges;
		facets[id].getEdges(edges);

		ConvElement ele;
		ele.setType(f_type);
		ele.setNodes(nodes);
		ele.setEdges(edges);
		ele.setMat(9998);
		element.push_back( std::move(ele) );
	}
}

/*
//===============================================================
// ○ 境界の面を、外側が法線になるように書き出す(デバック用だが、WAMITに使える)
//===============================================================*/
/** 
 * @brief export boundary facets (norm vector is outer dir.)
 * @param filename filename
 * @param range x,y,z range for output
*/
void ConvMesh::exportBoundFacet_outward(const string& filename, const vector<vector<double>>& range) const{
	/* 範囲設定 */
	vector<vector<double>> range_true;
	if(range.empty()){
		range_true.resize(3);
		for(int i = 0 ; i < 3 ; i++){
			range_true[i].resize(2);
			range_true[i][0] = -1.0e+9;
			range_true[i][1] = 1.0e+9;
		}
	}else{
		range_true = range;
	}
	convint target_surf_num = 0;

	const convint bond_f_size = boundFacetList.size();	

	/* 範囲内にあるfacetを探す */
	vector<convint> target_facet_ids;
	for(convint i = 0 ; i < bond_f_size ; i++){
		const convint id = boundFacetList[i];
		double grav[3];
		facets[id].getGrav(grav, node);
		bool bl = true;
		for(int p = 0 ; p < 3 ; p++){
			bl &= (range_true[p][0] < grav[p] && grav[p] < range_true[p][1]);
		}
		if(bl){
			target_facet_ids.push_back(id);
		}
	}
	const convint target_facet_id_num = target_facet_ids.size();

	std::fstream fp(filename, std::ios::out);
	fp << target_facet_id_num << endl;
	
	for(convint i = 0 ; i < target_facet_id_num ; i++){
		const convint id = target_facet_ids[i];
		/* こいつを使っている要素を探す */
		convint target_ele=-1;
		const convint ele_size = element.size();
		bool bl = false;
		for(convint e = 0; e < ele_size; e++){
			const convint num_f = element[e].getFacetNum();
			for(int ff = 0 ; ff < num_f; ff++){
				if(ele_facets_id[e][ff] == id){
					target_ele = e;
					bl = true;
					break;
				}
			}
			if(bl){
				break;
			}
		}
		if(target_ele < 0){
			cout << "error...???"<<endl;
			exit(1);
		}
		/* 対象となる元要素の重心を取得 */
		double grav1[3];
		element[target_ele].getGrav(grav1, node);

		ConvEType f_type = facets[id].getType();
		const int num_n = facets[id].getNodeNum();
		//const int num_e = facets[id].getEdgeNum();
		vector<convint> nodes;
		facets[id].getNodes(nodes);
		/* 面の重心を求める */
		double grav2[3] = { 0,0,0 };
		for(auto nid : nodes){
			double x = node[nid].getX();
			double y = node[nid].getY();
			double z = node[nid].getZ();
			grav2[0] += x;
			grav2[1] += y;
			grav2[2] += z;
		}
		const int ss = nodes.size();
		grav2[0] /= ss;
		grav2[1] /= ss;
		grav2[2] /= ss;

		/* 法線を求める */
		double vec_n[3] = {
			grav2[0]-grav1[0],
			grav2[1]-grav1[1],
			grav2[2]-grav1[2]
		};

		/* 面の外積を求める */
		convint n1;
		convint n2;
		convint n3;
		if(f_type == ConvEType::Tri){
			n1 = nodes[0];
			n2 = nodes[1];
			n3 = nodes[2];
		}else if(f_type == ConvEType::Quad){
			n1 = nodes[0];
			n2 = nodes[1];
			n3 = nodes[3];
		}else{
			cout << "Other face ele type is not maked..."<<endl;
			exit(1);
		}
		double vec12[3];
		vec12[0] = node[n2].getX();
		vec12[1] = node[n2].getY();
		vec12[2] = node[n2].getZ();
		vec12[0] -= node[n1].getX();
		vec12[1] -= node[n1].getY();
		vec12[2] -= node[n1].getZ();

		double vec13[3];
		vec13[0] = node[n3].getX();
		vec13[1] = node[n3].getY();
		vec13[2] = node[n3].getZ();
		vec13[0] -= node[n1].getX();
		vec13[1] -= node[n1].getY();
		vec13[2] -= node[n1].getZ();

		double vec_cross[3];
		vec_cross[0] = vec12[1]*vec13[2] - vec12[2]*vec13[1];
		vec_cross[1] = -1.0*vec12[0]*vec13[2] + vec12[2]*vec13[0];
		vec_cross[2] = vec12[0]*vec13[1] - vec12[1]*vec13[0];
		/*double norm = vec_cross[0]*vec_cross[0] + vec_cross[1]*vec_cross[1] + vec_cross[2]*vec_cross[2];
		norm = sqrt(norm);
		if(norm < 1.0e-8){
			cout << grav2[0] << ", " << grav2[1] << ", " << grav2[2] << endl;
		}*/

		/* ２つの法線の内積をとる */
		const double dot = vec_n[0]*vec_cross[0] + vec_n[1]*vec_cross[1] + vec_n[2]*vec_cross[2];
		/* 正ならそのまま書き出す */
		if(dot >= 0){
			for(auto nid : nodes){
				double x = node[nid].getX();
				double y = node[nid].getY();
				double z = node[nid].getZ();
				fp <<  " " << std::setw(15) << std::scientific <<std::setprecision(8)  << x << " " << std::setw(15) << y << " " << std::setw(15) <<  z << endl;
			}
			//cout << grav2[0] << ", " << grav2[1] << ", " << grav2[2] << ", " << vec_cross[0] <<", " << vec_cross[1] << ", " << vec_cross[2] << endl;
		/* 負なら反転して書き出す */
		} else{
			//cout << "inv..."<<endl;
			if(f_type == ConvEType::Tri){
				const convint tmp = nodes[1];
				nodes[1] = nodes[2];
				nodes[2] = tmp;
			} else if(f_type == ConvEType::Quad){
				const convint tmp = nodes[3];
				nodes[3] = nodes[1];
				nodes[1] = tmp;
			}
			for(auto nid : nodes){
				double x = node[nid].getX();
				double y = node[nid].getY();
				double z = node[nid].getZ();
				fp <<  " " << std::setw(15) << std::scientific <<std::setprecision(8)  << x << " " << std::setw(15) << y << " " << std::setw(15) <<  z << endl;
			}
			//cout << grav2[0] << ", " << grav2[1] << ", " << grav2[2] << ", " << -1.0*vec_cross[0] <<", " << -1.0*vec_cross[1] << ", " << -1.0*vec_cross[2] << endl;
		}
	}
	fp.close();
}

/* end of namespace*/
};
};
