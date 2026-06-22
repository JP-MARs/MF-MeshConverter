/**
* @file ConvFacet.cpp
* @brief define facet data structure
*
*/
#include <mfMeshConverter/ConvNodeElement.hpp>


namespace JPMRspace{
namespace MeshConvLib{

ConvFacet::ConvFacet(){
	nodes = nullptr;
	edges = nullptr;
}


/*//=======================================================
// ● 代入オペレータ
//=======================================================*/
ConvFacet& ConvFacet::operator=(const ConvFacet& ele){
	nodes.reset();
	edges.reset();
	id = ele.id;
	this->initialize(ele.e_type, ele.nodes.get(), ele.edges.get());
	return *this;
}

/*//=======================================================
// ● ムーブオペレータ
//=======================================================*/
ConvFacet& ConvFacet::operator=(ConvFacet&& ele) noexcept{
	nodes.reset();
	edges.reset();
	id = ele.id;
	e_type = ele.e_type;
	nodes = std::move(ele.nodes);
	edges = std::move(ele.edges);
	ele.nodes.reset();
	ele.nodes = nullptr;
	ele.edges.reset();
	ele.edges = nullptr;
	return *this;
}

/*//=======================================================
// ● 初期化
//=======================================================*/
void ConvFacet::initialize(ConvEType type){
	nodes.reset();
	edges.reset();
	e_type = type;
	switch(e_type){
	case ConvEType::Tri:
		nodes = std::make_unique<convint[]>(3);
		edges = std::make_unique<convint[]>(3);
		break;
	case ConvEType::Quad:
		nodes = std::make_unique<convint[]>(4);
		edges = std::make_unique<convint[]>(4);
		break;
	case ConvEType::Tri2:
		nodes = std::make_unique<convint[]>(6);
		edges = std::make_unique<convint[]>(6);
		break;
	case ConvEType::Quad2:
		nodes = std::make_unique<convint[]>(8);
		edges = std::make_unique<convint[]>(8);
		break;
	default:
		std::cout << "Facet Ele Type erro!!! "<< static_cast<int>(e_type) << std::endl;
		exit(1);
	}
	int n_num = this->getNodeNum();
	int e_num = this->getEdgeNum();
	for(int i =0 ; i < n_num ; i++){
		nodes[i] = -1;
	}
	for(int i =0 ; i < e_num ; i++){
		edges[i] = -1;
	}
}
/*//=======================================================
// ● 初期化
//=======================================================*/
void ConvFacet::initialize(ConvEType type, convint* nn, convint* ee){
	this->initialize(type);
	this->setNodes(nn);
	this->setEdges(ee);
}


/*//=======================================================
// ● 節点の数を返す
//=======================================================*/
int ConvFacet::getNodeNum() const{
	switch(e_type){
	case ConvEType::Tri:
		return 3;
	case ConvEType::Quad:
		return 4;
	case ConvEType::Tri2:
		return 6;
	case ConvEType::Quad2:
		return 8;
	default:
		std::cout << "Facet Ele Type error node !"<<std::endl;
		exit(1);
	}
}
/*//=======================================================
// ● 辺の数を返す
//=======================================================*/
int ConvFacet::getEdgeNum() const{
	switch(e_type){
	case ConvEType::Tri:
		return 3;
	case ConvEType::Quad:
		return 4;
	case ConvEType::Tri2:
		return 6;
	case ConvEType::Quad2:
		return 8;
	default:
		std::cout << "Facet Ele Type error edge!"<<std::endl;
		exit(1);
	}
}


/*//=======================================================
// ● 節点設定
//=======================================================*/
void ConvFacet::setNodes(convint *nn){
	const int n_num = this->getNodeNum();
	for(int i =0 ; i < n_num ; i++){
		nodes[i] = nn[i];
	}
}
/*//=======================================================
// ● 節点取得
//=======================================================*/
void ConvFacet::getNodes(convint *nn) const{
	const int n_num = this->getNodeNum();
	for(int i=0 ; i < n_num ; i++ ){
		nn[i] = nodes[i];
	}
}
void ConvFacet::getNodes(vector<convint>& nn) const{
	const int n_num = this->getNodeNum();
	nn.resize(n_num);
	for(int i=0 ; i < n_num ; i++ ){
		nn[i] = nodes[i];
	}
}


/*//=======================================================
// ● 辺設定
//=======================================================*/
void ConvFacet::setEdges(convint *ee){
	const int size = this->getEdgeNum();
	for(int i=0 ; i < size ; i++ ){
		edges[i] = ee[i];
	}
}
/*//=======================================================
// ● 辺取得
//=======================================================*/
void ConvFacet::getEdges(convint *ee) const{
	const int size = this->getEdgeNum();
	for(int i=0 ; i < size ; i++ ){
		ee[i] = edges[i];
	}
}
void ConvFacet::getEdges(vector<convint>& ee) const{
	const int size = this->getEdgeNum();
	ee.resize(size);
	for(int i=0 ; i < size ; i++ ){
		ee[i] = edges[i];
	}
}

/*//=======================================================
// ● 同じ面か判定
//=======================================================*/
bool ConvFacet::operator==(const ConvFacet f2) const{
	/* まず同じ要素タイプでないなら同じではない */
	if(e_type != f2.e_type) return false;
	
	/* 辺IDの重複判定 */
	/**/
	/* 三角形のとき */
	if(e_type == ConvEType::Tri){
		bool b1 = false;
		bool b2 = false;
		bool b3 = false;
		/* ３つの辺とも、相手と一致する点が合ったら、同じ面のはずである */
		for(int i = 0; i < 3; i++){
			b1 |= (edges[0] == f2.edges[i]);
			b2 |= (edges[1] == f2.edges[i]);
			b3 |= (edges[2] == f2.edges[i]);
		}
		if(b1 && b2 && b3){
			return true;
		}
	/* 四角形のとき */
	}else if(e_type == ConvEType::Quad){
		bool b1=false;
		bool b2=false;
		bool b3=false;
		bool b4=false;
		/* 4つの辺とも、相手と一致する点が合ったら、同じ面のはずである */
		for(int i = 0; i < 4; i++){
			b1 |= (edges[0] == f2.edges[i]);
			b2 |= (edges[1] == f2.edges[i]);
			b3 |= (edges[2] == f2.edges[i]);
			b4 |= (edges[3] == f2.edges[i]);
		}
		if(b1 && b2 && b3 && b4){
			return true;
		}
	}
	return false;
}



/*//=======================================================
// ● 重心計算
//=======================================================*/
void ConvFacet::getGrav(double* grav, const vector<ConvNode>& NodeData) const{
	const int size = this->getNodeNum();
	double gx=0, gy=0, gz=0;
	for(int i=0 ; i < size ; i++ ){
		auto itr = NodeData.begin();
		itr += nodes[i];
		double xx = itr->getX();
		double yy = itr->getY();
		double zz = itr->getZ();
		gx += xx; gy += yy; gz += zz;
	}
	gx /= (size); gy /= (size); gz /= (size);
	grav[0] = gx;
	grav[1] = gy;
	grav[2] = gz;
}

/*//=======================================================
// ● 面積計算(外積)
//=======================================================*/
double ConvFacet::calcArea(const vector<ConvNode>& NodeData) const{
	/* 三角形＝そのまま計算して終わる */
	if(e_type == ConvEType::Tri){
		return calc_area_tri(NodeData, nodes[0], nodes[1], nodes[2]);
	/* 四角形＝２つの三角形にして足す */
	}else if(e_type == ConvEType::Quad){
		double area1 = calc_area_tri(NodeData, nodes[0], nodes[1], nodes[2]);
		double area2 = calc_area_tri(NodeData, nodes[0], nodes[2], nodes[3]);
		return (area1 + area2);
	}else{
		std::cout << "This element type area calc is not implemented....!" << std::endl;
		exit(1);
	}
}

/*//=======================================================
// ● 三角形の面積計算
//=======================================================*/
double ConvFacet::calc_area_tri(const vector<ConvNode>& NodeData, const convint n1, const convint n2, const convint n3) const{
	double vec1[3];
	double vec2[3];
	double coef = 0.5;
	/* ベクトル計算 */
	double x1 = NodeData[n1].getX(); double y1 = NodeData[n1].getY(); double z1 = NodeData[n1].getZ(); 
	double x2 = NodeData[n2].getX(); double y2 = NodeData[n2].getY(); double z2 = NodeData[n2].getZ(); 
	double x3 = NodeData[n3].getX(); double y3 = NodeData[n3].getY(); double z3 = NodeData[n3].getZ(); 
	vec1[0] = x2 - x1; vec1[1] = y2 - y1; vec1[2] = z2 - z1;
	vec2[0] = x3 - x1; vec2[1] = y3 - y1; vec2[2] = z3 - z1;
	/* 外積計算 */
	double cross[3];
	cross[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
	cross[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
	cross[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0];
	double norm1 = cross[0]*cross[0]+cross[1]*cross[1]+cross[2]*cross[2];
	double norm2 = sqrt(norm1);
	double sum_area = coef * norm1 / norm2;
	return sum_area;
}



};
};
