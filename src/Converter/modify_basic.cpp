/**
* @file modify_basic.cpp
* @brief modify_basic 
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>

#include <stack>

namespace JPMRspace{
namespace MeshConvLib{



/*
//=======================================================
// ● スケーリング
//=======================================================*/
/** 
 * @brief scale nodes
*/
void MeshConverter::scaling(ConvMesh& mesh, double scale_fac){
	/* スケーリング値を読み取る */
#ifdef PRINT_CONV_INFO
	cout << "scale " << scale_fac <<endl << endl;
#endif
	const convint node_size = mesh.node.size();
	for(convint i = 0 ; i < node_size ; i++){
		/* スケール開始 */
		auto itr = mesh.node.begin();
		itr += i;
		double xx = itr->getX();
		double yy = itr->getY();
		double zz = itr->getZ();
		xx *= scale_fac;
		yy *= scale_fac;
		zz *= scale_fac;
		mesh.node[i].set(xx, yy, zz);
	}
}


/*
//=======================================================
// ● 最小の辺長を計算
//=======================================================*/
/** 
 * @brief calc. minimum edge length
*/
double MeshConverter::calc_shortest_edge_length(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << "calc min edge length... "<< endl;
#endif
	const convint ed_size = mesh.edge.size();
	if(ed_size == 0){ return 0.0;};

	double minL = 1.0e+12;
	/* 辺を探す */
	for(auto itr : mesh.edge){
		const convint s_id = itr.getSID();
		const convint e_id = itr.getEID();
		vector<ConvNode>::iterator itr_n1 = mesh.node.begin();
		itr_n1 += s_id;
		vector<ConvNode>::iterator itr_n2 = mesh.node.begin();
		itr_n2 += e_id;
		double x1 = itr_n1->getX(); double y1 = itr_n1->getY(); double z1 = itr_n1->getZ();
		double x2 = itr_n2->getX(); double y2 = itr_n2->getY(); double z2 = itr_n2->getZ();
		double dx = (x1-x2)*(x1-x2);
		double dy = (y1-y2)*(y1-y2);
		double dz = (z1-z2)*(z1-z2);
		double diff = sqrt(dx + dy + dz);
		if (minL > diff){
			minL = diff;
		}
	}
#ifdef PRINT_CONV_INFO
	cout << endl << "min. edge length => " <<minL<<endl << endl;
#endif
	return minL;
}

/*
//=======================================================
// ● 3Dと2D要素が混在しているとき、リナンバして2D要素のIDを末尾に振りなおす
//=======================================================*/
/** 
 * @brief renumbering elemnt IDs (from 3D, 2D, 1D)
*/
void MeshConverter::renumbering_3D_and_2Dele(ConvMesh& mesh){
#ifdef PRINT_CONV_INFO
	cout << " renumbering 2D ele!"<<endl;
#endif
	std::stack<ConvElement> ele_3d;
	std::stack<ConvElement> ele_2d;
	std::stack<ConvElement> ele_1d;
	const convint full_size = mesh.element.size();
	while(true){
		/* 要素の末尾から順にスタックに入れていく(前から取り出すために) */
		vector<ConvElement>::reverse_iterator itr = mesh.element.rbegin();
		//ConvEType e_type = itr->getType();
		bool is3D = itr->is3D();
		bool is1D = itr->is1D();
		if(is3D){
			ele_3d.emplace(*itr);
		}else if(is1D){
			ele_1d.emplace(*itr);
		}else{
			ele_2d.emplace(*itr);
		}
		mesh.element.pop_back();
		/* 全部取り出し終わったら抜ける */
		bool is_null = mesh.element.empty();
		if(is_null){
			break;
		}
	}
	//cout << "3d size << "<<ele_3d.size()<<endl;
	mesh.element.clear();
	mesh.element.reserve(full_size);
	convint id=0;
	/* 3Dから順に入れていく */
	if(ele_3d.size() > 0){
		while(true){
			ConvElement tempE = ele_3d.top();
			convint pre = tempE.getID();
			ele_3d.pop();
			tempE.setID(id);
			mesh.element.push_back( std::move(tempE) );
			id++;
			bool is_null = ele_3d.empty();
			if(is_null){
				break;
			}
		}
	}
	/* 次に2Dを順に入れていく */
	if(ele_2d.size() > 0){
		while(true){
			ConvElement tempE = ele_2d.top();
			ele_2d.pop();
			tempE.setID(id);
			mesh.element.push_back( std::move(tempE) );
			id++;
			bool is_null = ele_2d.empty();
			if(is_null){
				break;
			}
		}
	}
	/* 最後に1Dを順に入れていく */
	if(ele_1d.size() > 0){
		while(true){
			ConvElement tempE = ele_1d.top();
			ele_1d.pop();
			tempE.setID(id);
			mesh.element.push_back( std::move(tempE) );
			id++;
			bool is_null = ele_1d.empty();
			if(is_null){
				break;
			}
		}
	}
}




/**/
};
};


