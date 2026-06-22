/**
* @file ConvNodeElement.cpp
* @brief define node, edge, element, facet data structure for mesh conversion
*
*/

#include <mfMeshConverter/ConvNodeElement.hpp>


namespace JPMRspace{
namespace MeshConvLib{


ConvEdge::ConvEdge(){
	id = 0;
	s_ID = -1;
	e_ID = -1;
	edge2D = false;
}

double ConvEdge::calc_length(const vector<ConvNode>& NodeData) const{
	auto itr = NodeData.begin();
	itr += s_ID;
	double x1 = itr->getX();
	double y1 = itr->getY();
	double z1 = itr->getZ();
	itr = NodeData.begin();
	itr += e_ID;
	double x2 = itr->getX();
	double y2 = itr->getY();
	double z2 = itr->getZ();
	double dx = (x1-x2)*(x1-x2);
	double dy = (y1-y2)*(y1-y2);
	double dz = (z1-z2)*(z1-z2);
	double length = sqrt(dx+dy+dz);
	return length;
}



ConvElement::ConvElement(){
	nodes = nullptr;
	edges = nullptr;
}
/*//=======================================================
// ● この要素が3Dか
//=======================================================*/
bool ConvElement::is3D() const{
	if(e_type == ConvEType::Tetra || e_type == ConvEType::Prizm || e_type == ConvEType::Hex || e_type == ConvEType::Pyramid){
		return true;
	}
	return false;
}
/*//=======================================================
// ● この要素が1Dか
//=======================================================*/
bool ConvElement::is1D() const{
	if(e_type == ConvEType::Point || e_type == ConvEType::Line){
		return true;
	}
	return false;
}


/*//=======================================================
// ● 代入オペレータ
//=======================================================*/
ConvElement& ConvElement::operator=(const ConvElement& ele){
	nodes.reset();
	edges.reset();
	id = ele.id;
	this->initialize(ele.e_type, ele.mat, ele.nodes.get(), ele.edges.get());
	return *this;
}

/*//=======================================================
// ● ムーブオペレータ
//=======================================================*/
ConvElement& ConvElement::operator=(ConvElement&& ele) noexcept{
	nodes.reset();
	edges.reset();
	id = ele.id;
	e_type = ele.e_type;
	mat = ele.mat;
	nodes = std::move(ele.nodes);
	edges = std::move(ele.edges);
	ele.nodes.reset();
	ele.edges.reset();
	ele.nodes = nullptr;
	ele.edges = nullptr;
	return *this;
}

/*//=======================================================
// ● 初期化
//=======================================================*/
void ConvElement::initialize(ConvEType type){
	e_type = type;
	switch(type){
	case ConvEType::Point:
		nodes = std::make_unique<convint[]>(1);
		edges.reset();
		break;
	case ConvEType::Line:
		nodes = std::make_unique<convint[]>(2);
		edges = std::make_unique<convint[]>(1);
		break;
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
	case ConvEType::Tetra:
		nodes = std::make_unique<convint[]>(4);
		edges = std::make_unique<convint[]>(6);
		break;
	case ConvEType::Prizm:
		nodes = std::make_unique<convint[]>(6);
		edges = std::make_unique<convint[]>(9);
		break;
	case ConvEType::Hex:
		nodes = std::make_unique<convint[]>(8);
		edges = std::make_unique<convint[]>(12);
		break;
	case ConvEType::Pyramid:
		nodes = std::make_unique<convint[]>(5);
		edges = std::make_unique<convint[]>(8);
		break;
	}
	const int edge_num = this->getEdgeNum();
	for(int i =0 ; i < edge_num ; i++){
		edges[i] = -1;
	}
}
/*//=======================================================
// ● 初期化
//=======================================================*/
void ConvElement::initialize(ConvEType type, int mat_, convint* nn, convint* ee){
	mat = mat_;
	this->initialize(type);
	this->setNodes(nn);
	this->setEdges(ee);
}


/*//=======================================================
// ● 節点の数を返す
//=======================================================*/
int ConvElement::getNodeNum() const{
	switch(e_type){
	case ConvEType::Point:
		return 1;
	case ConvEType::Line:
		return 2;
	case ConvEType::Tri:
		return 3;
	case ConvEType::Quad:
		return 4;
	case ConvEType::Tri2:
		return 6;
	case ConvEType::Quad2:
		return 8;
	case ConvEType::Tetra:
		return 4;
	case ConvEType::Prizm:
		return 6;
	case ConvEType::Hex:
		return 8;
	case ConvEType::Pyramid:
		return 5;		
	}
	cout << "error... ! This element type node num is not implemented....!" << endl;
	std::abort();
}
/*//=======================================================
// ● 辺の数を返す
//=======================================================*/
int ConvElement::getEdgeNum() const{
	switch(e_type){
	case ConvEType::Point:
		return 0;
	case ConvEType::Line:
		return 1;
	case ConvEType::Tri:
		return 3;
	case ConvEType::Quad:
		return 4;
	case ConvEType::Tri2:
		return 6;
	case ConvEType::Quad2:
		return 8;
	case ConvEType::Tetra:
		return 6;
	case ConvEType::Prizm:
		return 9;
	case ConvEType::Hex:
		return 12;
	case ConvEType::Pyramid:
		return 8;
	}
	cout << "error... ! This element type node num is not implemented....!" << endl;
	std::abort();
}
/*//=======================================================
// ● 面の数を返す
//=======================================================*/
int ConvElement::getFacetNum() const{
	switch(e_type){
	case ConvEType::Tetra:
		return 4;
	case ConvEType::Prizm:
		return 5;
	case ConvEType::Hex:
		return 6;
	case ConvEType::Pyramid:
		return 5;
	default:
		return 0;
	}
}


/*//=======================================================
// ● 節点設定
//=======================================================*/
void ConvElement::setNodes(convint *nn){
	const int size = this->getNodeNum();
	for(int i=0 ; i < size ; i++ ){
		nodes[i] = nn[i];
	}
}
void ConvElement::setNodes(const vector<convint>& nn){
	const int size = this->getNodeNum();
	for(int i=0 ; i < size ; i++ ){
		nodes[i] = nn[i];
	}
}

/*//=======================================================
// ● 節点取得
//=======================================================*/
void ConvElement::getNodes(convint *nn) const{
	const int size = this->getNodeNum();
	for(int i=0 ; i < size ; i++ ){
		nn[i] = nodes[i];
	}
}
void ConvElement::getNodes(vector<convint>& nn) const{
	const int size = this->getNodeNum();
	nn.resize(size);
	for(int i=0 ; i < size ; i++ ){
		nn[i] = nodes[i];
	}
}

/*//=======================================================
// ● 節点入れ替え
//=======================================================*/
void ConvElement::swap_nodes(int i, int j){
	convint temp = nodes[i];
	nodes[i] = nodes[j];
	nodes[j] = temp;
}


/*//=======================================================
// ● 辺設定
//=======================================================*/
void ConvElement::setEdges(convint *ee){
	const int size = this->getEdgeNum();
	for(int i=0 ; i < size ; i++ ){
		edges[i] = ee[i];
	}
}
void ConvElement::setEdges(const vector<convint>& ee){
	const int size = this->getEdgeNum();
	for(int i=0 ; i < size ; i++ ){
		edges[i] = ee[i];
	}
}
/*//=======================================================
// ● 辺取得
//=======================================================*/
void ConvElement::getEdges(convint *ee) const{
	const int size = this->getEdgeNum();
	for(int i=0 ; i < size ; i++ ){
		ee[i] = edges[i];
	}
}
void ConvElement::getEdges(vector<convint>& ee) const{
	const int size = this->getEdgeNum();
	ee.resize(size);
	for(int i=0 ; i < size ; i++ ){
		ee[i] = edges[i];
	}
}


/*//=======================================================
// ● 重心計算
//=======================================================*/
void ConvElement::getGrav(double *grav, const vector<ConvNode>& NodeData) const{
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
	gx /= size; gy /= size; gz /= size;
	grav[0] = gx;
	grav[1] = gy;
	grav[2] = gz;
}


/*//=======================================================
// ● 面積計算
//=======================================================*/
double ConvElement::calcArea(const vector<ConvNode>& NodeData) const{
	double area;
	switch(e_type){
	/* 三角形の面積 */
	case ConvEType::Tri:
	case ConvEType::Tri2:
		area = calc_area_tri(NodeData);
		return area;
	/* 四角形の面積 */
	case ConvEType::Quad:
	case ConvEType::Quad2:
		area = calc_area_quad(NodeData);
		return area;
	/* それ以外は無視 */
	default:
		cout << "This element type area calc is not implemented....!"<<endl;
		exit(1);
		return -1.0;
	}
}
/*//=======================================================
// ● 面積計算（三角形）
//=======================================================*/
double ConvElement::calc_area_tri(const vector<ConvNode>& NodeData) const{
	double x1 = NodeData[nodes[0]].getX(); double y1 = NodeData[nodes[0]].getY();
	double x2 = NodeData[nodes[1]].getX(); double y2 = NodeData[nodes[1]].getY();
	double x3 = NodeData[nodes[2]].getX(); double y3 = NodeData[nodes[2]].getY();
	double area = x2*y3-x3*y2 + x1*(y2-y3) + y1*(x3-x2);
	area *= 0.5;
	return area;
}
/*//=======================================================
// ● 面積計算（四角形）
//=======================================================*/
double ConvElement::calc_area_quad(const vector<ConvNode>& NodeData) const{
	constexpr int num_gauss = 2;
	double gauss_xp[2] = {0.577350269189626, -0.577350269189626};
	double gauss_yp[2] = {0.577350269189626, -0.577350269189626};
	double gauss_w[2] = {1, 1};
	double pos_rst[2];
	double area = 0;
	/* ガウス積分！ */
	for(int xg = 0 ; xg < num_gauss ; xg++){
		for(int yg = 0 ; yg < num_gauss ; yg++){
			pos_rst[0] = gauss_xp[xg]; pos_rst[1] = gauss_yp[yg];
			double Data_rk[] = {-1,  1,  1, -1};
			double Data_sk[] = {-1, -1,  1,  1};
			/* ヤコビアン */
			double dx_dr=0, dx_ds=0, dy_dr=0, dy_ds=0;
			for(int i = 0 ; i < 4 ; i++){
				double xx = NodeData[nodes[i]].getX();
				double yy = NodeData[nodes[i]].getY();
				dx_dr += Data_rk[i] * (1.0 + Data_sk[i]*pos_rst[1]) * xx / 4.0;
				dx_ds += Data_sk[i] * (1.0 + Data_rk[i]*pos_rst[0]) * xx / 4.0;
				dy_dr += Data_rk[i] * (1.0 + Data_sk[i]*pos_rst[1]) * yy / 4.0;
				dy_ds += Data_sk[i] * (1.0 + Data_rk[i]*pos_rst[0]) * yy / 4.0;
			}
			/* ヤコビアン */
			double detJ = dx_dr*dy_ds - dy_dr*dx_ds;
			area += detJ*gauss_w[xg]*gauss_w[yg];
		}
	}
	return area;
}


/*//=======================================================
// ● 体積計算
//=======================================================*/
double ConvElement::calcVolume(const vector<ConvNode>& NodeData) const{
	switch(e_type){
	/* 四面体の面積 */
	case ConvEType::Tetra:
		return calcVolume_Tet(NodeData);
	/* プリズムの面積 */
	case ConvEType::Prizm:
		return calcVolume_Pri(NodeData);
	/* 六面体の面積 */
	case ConvEType::Hex:
		return calcVolume_Hex(NodeData);
	default:
		cout << "This element type Volume calc is not implemented....!"<<endl;
		exit(1);
		return -1.0;
	}
}

/*//=======================================================
// ● 要素の体積を計算(四面体)
//=======================================================*/
double ConvElement::calcVolume_Tet(const vector<ConvNode>& NodeData) const {
	double coefB[4]; double coefC[4]; double coefD[4]; double coefE[4];
	double volume;
	calcTriCoef(volume, coefB, coefC, coefD, coefE, NodeData);
	return volume;
}
/*//=======================================================
// ● 要素の体積を計算(プリズム)
//=======================================================*/
double ConvElement::calcVolume_Pri(const vector<ConvNode>& NodeData) const {
	const int num_gauss1 = 3;
	const int num_gauss2 = 2;
	double gauss_xp[3] = {0.5, 0.0, 0.5};
	double gauss_yp[3] = {0.5, 0.5, 0.0};
	double gauss_zp[3] = {-1.0*1.0/sqrt(3.0), 1.0/sqrt(3.0)};
	double gauss_w1[3] = {1.0/3.0, 1.0/3.0, 1.0/3.0};
	double gauss_w2[3] = {1.0, 1.0};
	double pos_rst[3];
	double volume = 0;
	/* ガウス積分！ */
	for(int xyg = 0 ; xyg < num_gauss1 ; xyg++){
		for(int zg = 0 ; zg < num_gauss2 ; zg++){
			pos_rst[0] = gauss_xp[xyg]; pos_rst[1] = gauss_yp[xyg]; pos_rst[2] = gauss_zp[zg];
			/* ヤコビアン */
			double Jacob = this->calc_Jacob_Pri(pos_rst, NodeData);
			volume += Jacob*gauss_w1[xyg]*gauss_w2[zg];
		}
	}
	return volume;
}
/*//=======================================================
// ● 要素の体積を計算(六面体)
//=======================================================*/
double ConvElement::calcVolume_Hex(const vector<ConvNode>& NodeData) const {
	const int num_gauss = 4;
	double gauss_xp[4]; double gauss_yp[4]; double gauss_zp[4]; 
	double gauss_w[4];

	gauss_xp[0] = 0.0;					gauss_yp[0] = sqrt(2.0/3.0);		gauss_zp[0] = -1.0/sqrt(3.0);
	gauss_xp[1] = 0.0;					gauss_yp[1] = -1.0*sqrt(2.0/3.0);	gauss_zp[1] = -1.0/sqrt(3.0);
	gauss_xp[2] = sqrt(2.0/3.0);		gauss_yp[2] = 0.0;					gauss_zp[2] = 1.0/sqrt(3.0);
	gauss_xp[3] = -1.0*sqrt(2.0/3.0);	gauss_yp[3] = 0.0;					gauss_zp[3] = 1.0/sqrt(3.0);

	gauss_w[0] = 2.0; gauss_w[1] = 2.0;
	gauss_w[2] = 2.0; gauss_w[3] = 2.0;
	double pos_rst[3];
	double volume = 0;
	/* ガウス積分！ */
	for(int xg = 0 ; xg < num_gauss ; xg++){
		pos_rst[0] = gauss_xp[xg]; pos_rst[1] = gauss_yp[xg]; pos_rst[2] = gauss_zp[xg];
		/* ヤコビアン */
		double Jacob = this->calc_Jacob_Hex(pos_rst, NodeData);
		volume += Jacob*gauss_w[xg];
	}
	return volume;
}

/* 四面体補間関数用係数計算 */
void ConvElement::calcTriCoef(double& volume, double* coefB, double* coefC, double* coefD, double* coefE, const vector<ConvNode>& NodeData) const {
	double xx[4];  double yy[4];  double zz[4];
	for(int i = 0 ; i < 4 ; i++){
		ConvNode n_ptr = NodeData[nodes[i]];
		xx[i] = n_ptr.getX();
		yy[i] = n_ptr.getY();
		zz[i] = n_ptr.getZ();
	}
	/* 循環添え字 */
	const int pos_j[] = {1, 2, 3, 0};
	const int pos_m[] = {2, 3, 0, 1};
	const int pos_n[] = {3, 0, 1, 2};
	/* 係数セット */
	for(int i = 0 ; i < 4 ; i++){
		const int j = pos_j[i];
		const int m = pos_m[i];
		const int n = pos_n[i];

		double pow_i = -1.0;
		for(int aa = 0 ; aa < i ; aa++){
			pow_i *= -1.0;
		}
		coefB[i] = pow_i*( xx[j]*(yy[n]*zz[m]-yy[m]*zz[n]) + xx[m]*(yy[j]*zz[n]-yy[n]*zz[j]) + xx[n]*(yy[m]*zz[j]-yy[j]*zz[m]) );
		coefC[i] = pow_i*( yy[j]*(zz[m]-zz[n]) + yy[m]*(zz[n]-zz[j]) + yy[n]*(zz[j]-zz[m]) );
		coefD[i] = pow_i*( zz[j]*(xx[m]-xx[n]) + zz[m]*(xx[n]-xx[j]) + zz[n]*(xx[j]-xx[m]) );
		coefE[i] = pow_i*( xx[j]*(yy[m]-yy[n]) + xx[m]*(yy[n]-yy[j]) + xx[n]*(yy[j]-yy[m]) );
	}
	volume = xx[0]*coefC[0] + xx[1]*coefC[1] + xx[2]*coefC[2] + xx[3]*coefC[3];
	volume /= 6.0;
}
/* ヤコビアン計算(プリズム) */
double ConvElement::calc_Jacob_Pri(double *pos_rst, const vector<ConvNode>& NodeData) const {
	const double Data_lam1[6] = { 1,  0,  0,  1,  0,  0};
	const double Data_lam2[6] = {-1,  1,  0, -1,  1,  0};
	const double Data_lam3[6] = {-1,  0,  1, -1,  0,  1};
	const double Data_tk[6] =   {-1, -1, -1,  1,  1,  1};
	/* まず　全体座標/局所　を計算 */
	double dx_dr=0, dx_ds=0, dx_dt=0;
	double dy_dr=0, dy_ds=0, dy_dt=0;
	double dz_dr=0, dz_ds=0, dz_dt=0;
	for(int i = 0 ; i < 6 ; i++){
		ConvNode n_ptr = NodeData[nodes[i]];
		const double xx = n_ptr.getX();
		const double yy = n_ptr.getY();
		const double zz = n_ptr.getZ();
		dx_dr += xx * 0.5*(Data_lam2[i]) * (1.0 + Data_tk[i] * pos_rst[2]);
		dx_ds += xx * 0.5*(Data_lam3[i]) * (1.0 + Data_tk[i] * pos_rst[2]);
		dx_dt += xx * 0.5*(Data_lam1[i] + Data_lam2[i]*pos_rst[0] + Data_lam3[i]*pos_rst[1]) * (Data_tk[i]);
		dy_dr += yy * 0.5*(Data_lam2[i]) * (1.0 + Data_tk[i] * pos_rst[2]);
		dy_ds += yy * 0.5*(Data_lam3[i]) * (1.0 + Data_tk[i] * pos_rst[2]);
		dy_dt += yy * 0.5*(Data_lam1[i] + Data_lam2[i]*pos_rst[0] + Data_lam3[i]*pos_rst[1]) * (Data_tk[i]);
		dz_dr += zz * 0.5*(Data_lam2[i]) * (1.0 + Data_tk[i] * pos_rst[2]);
		dz_ds += zz * 0.5*(Data_lam3[i]) * (1.0 + Data_tk[i] * pos_rst[2]);
		dz_dt += zz * 0.5*(Data_lam1[i] + Data_lam2[i]*pos_rst[0] + Data_lam3[i]*pos_rst[1]) * (Data_tk[i]);
	}
	/* ヤコビアン～三角形なので面積分を1/2する！！ */
	double detJ1 = (dx_dr*dy_ds*dz_dt + dx_ds*dy_dt*dz_dr + dx_dt*dy_dr*dz_ds);
	double detJ2 = (dx_dt*dy_ds*dz_dr + dy_dt*dz_ds*dx_dr + dz_dt*dx_ds*dy_dr);
	return( 0.5*(detJ1-detJ2) );
}


/*//=======================================================
// ● ヤコビアン計算(六面体)
//=======================================================*/
double ConvElement::calc_Jacob_Hex(double *pos_rst, const vector<ConvNode>& NodeData) const {
	const double Data_rk[] = {-1,  1,  1, -1, -1,  1,  1, -1};
	const double Data_sk[] = {-1, -1,  1,  1, -1, -1,  1,  1};
	const double Data_tk[] = {-1, -1, -1, -1,  1,  1,  1,  1};
	/* まず　全体座標/局所　を計算 */
	double dx_dr=0, dx_ds=0, dx_dt=0;
	double dy_dr=0, dy_ds=0, dy_dt=0;
	double dz_dr=0, dz_ds=0, dz_dt=0;
	for(int i = 0 ; i < 8 ; i++){
		ConvNode n_ptr = NodeData[nodes[i]];
		const double xx = n_ptr.getX();
		const double yy = n_ptr.getY();
		const double zz = n_ptr.getZ();
		dx_dr += xx * (Data_rk[i]) * (1.0 + Data_sk[i]*pos_rst[1]) * (1.0 + Data_tk[i]*pos_rst[2]) * 0.125;
		dx_ds += xx * (1.0 + Data_rk[i]*pos_rst[0]) * (Data_sk[i]) * (1.0 + Data_tk[i]*pos_rst[2]) * 0.125;
		dx_dt += xx * (1.0 + Data_rk[i]*pos_rst[0]) * (1.0 + Data_sk[i]*pos_rst[1]) * (Data_tk[i]) * 0.125;
		dy_dr += yy * (Data_rk[i]) * (1.0 + Data_sk[i]*pos_rst[1]) * (1.0 + Data_tk[i]*pos_rst[2]) * 0.125;
		dy_ds += yy * (1.0 + Data_rk[i]*pos_rst[0]) * (Data_sk[i]) * (1.0 + Data_tk[i]*pos_rst[2]) * 0.125;
		dy_dt += yy * (1.0 + Data_rk[i]*pos_rst[0]) * (1.0 + Data_sk[i]*pos_rst[1]) * (Data_tk[i]) * 0.125;
		dz_dr += zz * (Data_rk[i]) * (1.0 + Data_sk[i]*pos_rst[1]) * (1.0 + Data_tk[i]*pos_rst[2]) * 0.125;
		dz_ds += zz * (1.0 + Data_rk[i]*pos_rst[0]) * (Data_sk[i]) * (1.0 + Data_tk[i]*pos_rst[2]) * 0.125;
		dz_dt += zz * (1.0 + Data_rk[i]*pos_rst[0]) * (1.0 + Data_sk[i]*pos_rst[1]) * (Data_tk[i]) * 0.125;
	}

	/* ヤコビアン */
	double detJ1 = (dx_dr*dy_ds*dz_dt + dx_ds*dy_dt*dz_dr + dx_dt*dy_dr*dz_ds);
	double detJ2 = (dx_dt*dy_ds*dz_dr + dy_dt*dz_ds*dx_dr + dz_dt*dx_ds*dy_dr);
	return (detJ1-detJ2);
}


};
};
