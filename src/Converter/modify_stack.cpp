/**
* @file modify_stack.cpp
* @brief modify_stack 
*
*/
#include <mfMeshConverter/Converter/MeshConverter.hpp>
#include <fstream>

namespace JPMRspace{
namespace MeshConvLib{



/*
//===============================================================
// ○ 2次元メッシュの積み上げ
//===============================================================*/
/** 
 * @brief stack 2D ele to 3D
*/
void MeshConverter::mesh2D_to_3D(ConvMesh& mesh, int no_layer, int no_base_mat, int no_layer_data, int no_z_data, int coor, int no_surf, bool dir_mode,
	const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate, const vector<int>& mat_id_2d,
	const vector<int>& from_layer, const vector<int>& to_layer, const vector<vector<int>>& layer_mat,
	const vector<int>& base_mat2D, const vector<int>& base_mat3D, const vector<int>& layer_pos){
#ifdef PRINT_CONV_INFO
	cout << "make 2D to 3D "<<endl<<endl;
#endif
	/* 積み上げデータ読み取り：基本部 */
	//int no_layer, no_base_mat, no_layer_data, no_z_data, coor, no_surf, dir_mode;

#ifdef READREADREAD__READ
	/* データ読み取り：z座標値 */
	int pos;
	double z_val, z_rate;
	int *layer_data;
	double *layer_z_data;
	double *layer_z_rate;
	/* 指定Zがゼロのとき、全指定と判断 */
	if(no_z_data == 0){
		no_z_data = no_layer;
		layer_data = new int[no_z_data+1];
		layer_z_data = new double[no_z_data+1];
		layer_z_rate = new double[no_z_data+1];
		for(int i = 0 ; i < no_z_data ; i++){
			getline(fp_in, str_line);
			tmp_line = MeshConvLib::MeshFileReadWrite::split(str_line, ' ');
			//fp_in >> z_val;
			layer_data[i] = i;
			layer_z_data[i] = std::stod(tmp_line[0]);
			layer_z_rate[i] = 1.0;
		}
	}else{
		layer_data = new int[no_z_data+1];
		layer_z_data = new double[no_z_data+1];
		layer_z_rate = new double[no_z_data+1];
		for(int i = 0 ; i < no_z_data ; i++){
			getline(fp_in, str_line);
			//cout << "stack " << i << " => " <<  str_line << endl;
			tmp_line = MeshConvLib::MeshFileReadWrite::split(str_line, ' ');
			//fp_in >> pos >> z_val >> z_rate;
			layer_data[i] = std::stoi(tmp_line[0]) - 1;
			layer_z_data[i] = std::stod(tmp_line[1]);
			layer_z_rate[i] = std::stod(tmp_line[2]);
			//cout << layer_data[i] << ", " << layer_z_data[i] << ", " << layer_z_rate[i] << endl;
		}
		getline(fp_in, str_line);
		tmp_line = MeshConvLib::MeshFileReadWrite::split(str_line, ' ');
		//fp_in >> pos >> z_val;/* 最後の位置 */
		layer_data[no_z_data] = no_layer;
		layer_z_data[no_z_data] = std::stod(tmp_line[1]);;
	}
	/* データ読み取り：2Dでの材料 */
	int *mat_id_2d = new int[no_base_mat];
	getline(fp_in, str_line);
	tmp_line = MeshConvLib::MeshFileReadWrite::split(str_line, ' ');
	for(int i = 0 ; i < no_base_mat ; i++){
		//fp_in >> mat_id_2d[i];
		mat_id_2d[i] = std::stoi(tmp_line[i]);
	}
#endif

#ifdef READREADREAD__READ
	/* データ読み取り：3D化で変更する材料ID　*/
	vector<int> from_layer;// = new int[no_layer_data];
	vector<int> to_layer;// = new int[no_layer_data];
	vector<vector<int>> layer_mat;// = new int*[no_layer_data];
	from_layer.resize(no_layer_data);
	to_layer.resize(no_layer_data);
	layer_mat.resize(no_layer_data);

	for(int i = 0 ; i < no_layer_data ; i++){
		layer_mat[i] = new int[no_base_mat];
	}
	for(int i = 0 ; i < no_layer_data ; i++){
		getline(fp_in, str_line);
		tmp_line = MeshConvLib::MeshFileReadWrite::split(str_line, ' ');

		int from, to;
		from = std::stoi(tmp_line[0]);
		to = std::stoi(tmp_line[1]);
		//fp_in >> from >> to;
		from_layer[i] = from - 1; 
		to_layer[i] = to - 1; 
		for(int j = 0 ; j < no_base_mat ; j++){
			//fp_in >> layer_mat[i][j];
			layer_mat[i][j] = std::stoi(tmp_line[2+j]);
		}
	}
#endif

#ifdef READREADREAD__READ
	/* 面番号読み取り */
	int *base_mat2D = new int[no_surf];
	int *base_mat3D = new int[no_surf];
	int *layer_pos = new int[no_surf];
	for(int i = 0 ; i < no_surf ; i++){
		int mat2, mat3, pos;
		getline(fp_in, str_line);
		tmp_line = MeshConvLib::MeshFileReadWrite::split(str_line, ' ');
		//fp_in >> mat2 >> mat3 >> pos;
		base_mat2D[i] = std::stoi(tmp_line[0]);//mat2;
		base_mat3D[i] = std::stoi(tmp_line[1]);//mat3;
		layer_pos[i] = std::stoi(tmp_line[2]);//pos;
	}
#endif

	/* 積み上げ開始 */
	mesh2D_3D_sub(mesh, no_layer, no_base_mat, no_layer_data, no_z_data, coor, no_surf, dir_mode,
		layer_data, layer_z_data, layer_z_rate, mat_id_2d, from_layer, to_layer, layer_mat, base_mat2D, base_mat3D, layer_pos);
#ifdef PRINT_CONV_INFO
	cout << "layering success 1"<<endl;
#endif

}

/*
//===============================================================
// ○ 2次元メッシュの積み上げ　-　サブルーチンその1
//===============================================================*/
void MeshConverter::mesh2D_3D_sub(ConvMesh& mesh, int no_layer, int no_base_mat, int no_layer_data, int no_z_data, int coor, int no_surf, bool dir_mode,
	const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate, const vector<int>& mat_id_2d,
	const vector<int>& from_layer, const vector<int>& to_layer, const vector<vector<int>>& layer_mat,
	const vector<int>& base_mat2D, const vector<int>& base_mat3D, const vector<int>& layer_pos){
	/*-------------------------------------------------------------*/

	/* 各層での節点＆要素を保持する */
	vector<ConvNode> *layer_node = new vector<ConvNode>[no_layer+1];
	vector<ConvElement> *layer_ele = new vector<ConvElement>[no_layer];
	/* まず気にせず各層へデータコピー(座標値は後で設定する) */
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	for(int kk=0 ; kk < no_layer+1 ; kk++){
		for(convint i=0 ; i < n_size ; i++){
			layer_node[kk].push_back(mesh.node[i]);
			/* 新しい節点IDをセット */
			convint new_id = kk*n_size + i;
			layer_node[kk][i].setID(new_id);
		}
	}
	/* 気にせず積み上げ3D要素を作る(材料IDは後で決める) */
	for(int kk=0 ; kk < no_layer ; kk++){
		for(convint i=0 ; i < e_size ; i++){
			ConvElement tempE = mesh.element[i];
			/* 新しい要素IDをセット */
			convint new_id = kk*e_size + i;
			tempE.setID(new_id);
			/* 要素タイプを3D化 */
			ConvEType type2D = mesh.element[i].getType();
			ConvEType type3D;
			if(type2D == ConvEType::Tri){
				type3D = ConvEType::Prizm;
			}else if(type2D == ConvEType::Quad){
				type3D = ConvEType::Hex;
			}else if(type2D == ConvEType::Point){
				type3D = ConvEType::Line;
			}else if(type2D == ConvEType::Line){
				type3D = ConvEType::Quad;
			}else{
				std::cout << "Stack element is not implimented...!!"<<std::endl;
				exit(1);
			}
			tempE.setType(type3D);
			/* 2Dでの元の節点IDを持ってくる */
			convint node_2d[8];
			mesh.element[i].getNodes(node_2d);
			convint node_3d_down[8];
			convint node_3d_up[8];
			/* 3Dでの節点IDを決める */
			if(type2D == ConvEType::Tri){
				for(int j = 0 ; j < 3 ; j++){
					convint id0 = layer_node[kk][node_2d[j]].getID();
					node_3d_down[j]  = id0;
					convint id1 = layer_node[kk+1][node_2d[j]].getID();
					node_3d_up[j]  = id1;
				}
			}else if(type2D == ConvEType::Quad){
				for(int j = 0 ; j < 4 ; j++){
					convint id0 = layer_node[kk][node_2d[j]].getID();
					node_3d_down[j]  = id0;
					convint id1 = layer_node[kk+1][node_2d[j]].getID();
					node_3d_up[j]  = id1;
				}
			}else if(type2D == ConvEType::Point){
				for(int j = 0 ; j < 1 ; j++){
					convint id0 = layer_node[kk][node_2d[j]].getID();
					node_3d_down[j]  = id0;
					convint id1 = layer_node[kk+1][node_2d[j]].getID();
					node_3d_up[j]  = id1;
				}
			}else if(type2D == ConvEType::Line){
				for(int j = 0 ; j < 2 ; j++){
					convint id0 = layer_node[kk][node_2d[j]].getID();
					node_3d_down[j]  = id0;
					convint id1 = layer_node[kk+1][node_2d[j]].getID();
					node_3d_up[j]  = id1;
				}
			}

			/* 3D層に決めた節点IDを渡す */
			if(type2D == ConvEType::Tri){
				for(int j = 0 ; j < 3 ; j++){
					tempE.setNodes(j, node_3d_down[j]);
					tempE.setNodes(j+3, node_3d_up[j]);
				}
			}else if(type2D == ConvEType::Quad){
				for(int j = 0 ; j < 4 ; j++){
					tempE.setNodes(j, node_3d_down[j]);
					tempE.setNodes(j+4, node_3d_up[j]);
				}
			}else if(type2D == ConvEType::Point){
				tempE.setNodes(0, node_3d_down[0]);
				tempE.setNodes(1, node_3d_up[0]);
			}else if(type2D == ConvEType::Line){
				tempE.setNodes(0, node_3d_down[0]);
				tempE.setNodes(1, node_3d_down[1]);
				tempE.setNodes(2, node_3d_up[1]);
				tempE.setNodes(3, node_3d_up[0]);
			}
			/* 押し出しがθなら、軸が変わるので右ねじを逆回りにする */
			if(dir_mode == 1){
				if(type2D == ConvEType::Tri){
					convint temp = node_3d_down[0];
					node_3d_down[0] =  node_3d_down[1];
					node_3d_down[1] =  temp;
					temp = node_3d_up[0];
					node_3d_up[0] =  node_3d_up[1];
					node_3d_up[1] =  temp;
					for(int j = 0 ; j < 3 ; j++){
						tempE.setNodes(j, node_3d_down[j]);
						tempE.setNodes(j+3, node_3d_up[j]);
					}
				}else if(type2D == ConvEType::Quad){
					convint temp1 = node_3d_down[0];
					convint temp2 = node_3d_down[1];
					node_3d_down[0] =  node_3d_down[3];
					node_3d_down[1] =  node_3d_down[2];
					node_3d_down[3] =  temp1;
					node_3d_down[2] =  temp2;
					temp1 = node_3d_up[0];
					temp2 = node_3d_up[1];
					node_3d_up[0] =  node_3d_up[3];
					node_3d_up[1] =  node_3d_up[2];
					node_3d_up[3] =  temp1;
					node_3d_up[2] =  temp2;
					for(int j = 0 ; j < 4 ; j++){
						tempE.setNodes(j, node_3d_down[j]);
						tempE.setNodes(j+4, node_3d_up[j]);
					}
				}else if(type2D == ConvEType::Line){
					tempE.setNodes(0, node_3d_down[1]);
					tempE.setNodes(1, node_3d_down[0]);
					tempE.setNodes(2, node_3d_up[0]);
					tempE.setNodes(3, node_3d_up[1]);
				}
			}
			/* 確定 */
			layer_ele[kk].push_back(tempE);
		}
	}
#ifdef PRINT_CONV_INFO
	cout << "step1"<<endl;
#endif
	/* 節点のz座標を決める */
	if(dir_mode == 0){
		set_layer_z_val(layer_node, layer_ele, no_z_data, layer_data, layer_z_data, layer_z_rate);
	}else{
		set_layer_theta_val(layer_node, layer_ele, no_z_data, layer_data, layer_z_data, layer_z_rate);
	}
#ifdef PRINT_CONV_INFO
	cout << "step2"<<endl;
#endif
	/* 各層の要素の材料を決める */
	set_layer_mat(layer_node, layer_ele, no_base_mat, no_layer_data,  mat_id_2d, from_layer, to_layer, layer_mat);
#ifdef PRINT_CONV_INFO
	cout << "step3"<<endl;
#endif
	/* 面要素をコピー＆セット */
	if(no_surf > 0){
		set_surf_mat(mesh, layer_node, layer_ele, no_layer, no_surf, base_mat2D, base_mat3D, layer_pos);
	}
#ifdef PRINT_CONV_INFO
	cout << "step4"<<endl;
#endif
	/* 積み上げ時に材料-1になった要素を消しつつ、積み上げ結果をセット */
	set_layer_fix(mesh, no_layer, layer_node, layer_ele);

	delete[] layer_node;
	delete[] layer_ele;
}

/*
//===============================================================
// ○ 積み上げサブルーチン２：zデータセット
//===============================================================*/
void MeshConverter::set_layer_z_val(vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_z_data, const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate){
	/* 開始位置をまず決定 */
	const convint n_size0 = layer_node[0].size();
	for(convint nn = 0; nn < n_size0 ; nn++){
		layer_node[0][nn].setZ(layer_z_data[0]);
	}
	/* Zデータの数だけループ */
	for(int kk = 0 ; kk < no_z_data ; kk++ ){
		const int start_lay = layer_data[kk];
		const int end_lay = layer_data[kk+1];
		/* まず決まっている次の層の節点をセット */
		const convint n_size = layer_node[end_lay].size();
		double newZ = layer_z_data[kk+1];
		for(convint nn = 0; nn < n_size ; nn++){
			layer_node[end_lay][nn].setZ(newZ);
		}
		int diff = end_lay - start_lay;
		double mode = fabs(1.0-layer_z_rate[kk]);
		/* ｚを求める */
		/* 等分割 */
		if( mode < 1.0e-6){
			double z1 = layer_node[start_lay][0].getZ();
			double z2 = layer_node[end_lay][0].getZ();
			double dZ = (z2 - z1) / diff;
			for(int i = 0; i < diff-1 ; i++){
				double val = dZ * (i+1.0);		
				/* この層のすべての要素に決めたZをセット */
				for(int nn = 0; nn < n_size ; nn++){
					layer_node[i+1+start_lay][nn].setZ(val+z1);
				}
			}
		/* 等比分割 */
		}else{
			double z_rate = layer_z_rate[kk];
			if(layer_z_rate[kk] < 0){
				z_rate = -1.0/ layer_z_rate[kk];
			}
			double z1 = layer_node[start_lay][0].getZ();
			double z2 = layer_node[end_lay][0].getZ();
			double DZ = z2 - z1;
			double sum_rate = 0;
			for(int aa=0 ; aa < diff ; aa++){
				sum_rate += pow(z_rate, 1.0*aa);
			}
			double baseZ = layer_node[start_lay][0].getZ();
			for(int i = 0; i < diff-1 ; i++){
				double val = DZ / sum_rate * pow(z_rate, 1.0*i);
				for(int nn = 0; nn < n_size ; nn++){
					layer_node[i+1+start_lay][nn].setZ(val+baseZ);
				}
				baseZ += val;
			}
		}
	}
}

/*
//===============================================================
// ○ 積み上げサブルーチン２b：θデータセット
//===============================================================*/
void MeshConverter::set_layer_theta_val(vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_z_data, const vector<int>& layer_data, const vector<double>& layer_z_data, const vector<double>& layer_z_rate){
	/* 各層のθ情報を保持させる */
	int max_lay = 1+layer_data[no_z_data];
	double *theta_data = new double[max_lay];
	theta_data[0] = 0.0;
	const convint n_size = layer_node[0].size();
	for(convint nn = 0; nn < n_size ; nn++){				
		double new_th = 0;
		double rr = layer_node[0][nn].getX();
		double xx = rr * cos(new_th);
		double yy = rr * sin(new_th);
		double zz = layer_node[0][nn].getY();
		layer_node[0][nn].setX(xx);
		layer_node[0][nn].setY(yy);
		layer_node[0][nn].setZ(zz);
	}
	/* Zデータの数だけループ */
	for(int kk = 0 ; kk < no_z_data ; kk++ ){
		const int start_lay = layer_data[kk];
		const int end_lay = layer_data[kk+1];
		/* まず決まっている次の層の節点をセット */
		double newTh = layer_z_data[kk+1];
		theta_data[end_lay] = newTh;
		/* 終わりの層のすべての要素に決めたthetaに基づく３D値をセット */
		for(convint nn = 0; nn < n_size ; nn++){
			double new_th = newTh;
			new_th *= DEF_CONV_PI/180.0;
			double rr = layer_node[end_lay][nn].getX();
			double xx = rr * cos(new_th);
			double yy = rr * sin(new_th);
			double zz = layer_node[end_lay][nn].getY();
			layer_node[end_lay][nn].setX(xx);
			layer_node[end_lay][nn].setY(yy);
			layer_node[end_lay][nn].setZ(zz);
		}
		int diff = end_lay - start_lay;
		/* ｚを求める */
		for(int i = 0; i < diff-1 ; i++){
			double th1 = theta_data[i+start_lay];//layer_node[i+start_lay][0].getZ();
			double th2 = theta_data[end_lay];//layer_node[end_lay][0].getZ();
			double val;
			double mode = fabs(1.0-layer_z_rate[kk]);
			/* 等分割 */
			if( mode < 1.0e-6){
				double div = (diff-i);
				val = (th2 - th1) / div;
			}else{
				if(layer_z_rate[kk] > 0){
					val = (th2 - th1)*(layer_z_rate[kk]);
				}else{
					val = (th2 - th1)*(1.0-layer_z_rate[kk]);
				}
			}
			theta_data[i+1+start_lay] = val+th1;
			/* この層のすべての要素に決めたthetaに基づく３D値をセット */
			for(convint nn = 0; nn < n_size ; nn++){				
				double new_th = val+th1;
				new_th *= DEF_CONV_PI/180.0;
				double rr = layer_node[i+1+start_lay][nn].getX();
				double xx = rr * cos(new_th);
				double yy = rr * sin(new_th);
				double zz = layer_node[i+1+start_lay][nn].getY();
				layer_node[i+1+start_lay][nn].setX(xx);
				layer_node[i+1+start_lay][nn].setY(yy);
				layer_node[i+1+start_lay][nn].setZ(zz);
			}
		}
	}
	delete[] theta_data;
}

/*
//===============================================================
// ○ 積み上げサブルーチン３：要素の材料データセット
//===============================================================*/
void MeshConverter::set_layer_mat(vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_base_mat, int no_layer_data, const vector<int>& mat_id_2d, const vector<int>& from_layer, const vector<int>& to_layer, const vector<vector<int>>& layer_mat){
	/* 設定数だけループ */
	const convint e_size = layer_ele[0].size();
	for(int kk = 0 ; kk < no_layer_data ; kk++ ){
		const int start_lay = from_layer[kk];
		const int end_lay = to_layer[kk];
		/* from から to までの要素層でループ */
		for(int pp=start_lay ; pp <= end_lay ; pp++){
			for(convint e=0 ; e < e_size ; e++){
				/* 元要素の材料ゲット */
				int mat = layer_ele[pp][e].getMat();
				int pos_mat=0;
				/* 元材料の2Dでの番号をサーチ */
				for(int mm = 0 ; mm < no_base_mat ; mm++){
					if(mat_id_2d[mm] == mat){
						pos_mat = mm;
					}
				}
				/* 同じ位置での材料を設定 */
				int new_mat = layer_mat[kk][pos_mat];
				layer_ele[pp][e].setMat(new_mat);
			}
		}
	}
}

/*
//===============================================================
// ○ 面要素をコピー＆セット
//===============================================================*/
void MeshConverter::set_surf_mat(ConvMesh& mesh, vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele, int no_layer, int no_surf, const vector<int>& base_mat2D, const vector<int>& base_mat3D, const vector<int>& layer_pos){
	const convint n_size = mesh.node.size();
	const convint e_size = mesh.element.size();
	/* 3Dへコピーする２Dの要素をサーチ */
	vector<convint> *ele2D_to3D_surf = new vector<convint>[no_surf];
	for(convint i = 0 ; i < e_size ; i++){
		convint id = mesh.element[i].getID();
		int mat = mesh.element[i].getMat();
		ConvEType type = mesh.element[i].getType();
		bool is3D = mesh.element[i].is3D();
		/* 元が2D要素の時or線要素 */
		if(!is3D){
			for(int j = 0 ; j < no_surf ; j++){
				if(base_mat2D[j] == mat){
					ele2D_to3D_surf[j].push_back(id);
				}
			}
		}
	}
	/* 各層へコピー */
	for(int i = 0 ; i < no_surf ; i++){
		int l_pos0 = layer_pos[i];
		int l_pos = abs(l_pos0) - 1;
		int new_mat = base_mat3D[i];
		const convint the_size = ele2D_to3D_surf[i].size();
		for(int j = 0 ; j < the_size ; j++){
			const convint id = ele2D_to3D_surf[i][j];
			ConvEType type = mesh.element[id].getType();
			/* 元が2D要素 */
			if(type == ConvEType::Tri || type == ConvEType::Quad){
				ConvElement tempE = mesh.element[id];
				/* 節点変更 */
				convint nodes[30];
				tempE.getNodes(nodes);
				convint new_node_id[4];
				ConvEType type = tempE.getType();
				if(type == ConvEType::Tri){
					new_node_id[0] = nodes[0] + l_pos*n_size;
					new_node_id[1] = nodes[1] + l_pos*n_size;
					new_node_id[2] = nodes[2] + l_pos*n_size;					
					if(l_pos0 < 0){
						convint tmp = new_node_id[0];
						new_node_id[0] = new_node_id[1];
						new_node_id[1] = tmp;
					}
				}else if(type == ConvEType::Quad){
					new_node_id[0] = nodes[0] + l_pos*n_size;
					new_node_id[1] = nodes[1] + l_pos*n_size;
					new_node_id[2] = nodes[2] + l_pos*n_size;
					new_node_id[3] = nodes[3] + l_pos*n_size;
					if(l_pos0 < 0){
						convint tmp = new_node_id[0];
						new_node_id[0] = new_node_id[1];
						new_node_id[1] = tmp;
						tmp = new_node_id[2];
						new_node_id[2] = new_node_id[3];
						new_node_id[3] = tmp;
					}
				}
				tempE.setNodes(new_node_id);
				/* ID変更 */
				tempE.setMat(new_mat);
				/* 層へ追加 */
				layer_ele[no_layer-1].push_back(tempE);
			/* 元が線要素 */
			}else{
				/* 積み上げ先で面になっている要素を参照, 材料変更 */
				layer_ele[l_pos][id].setMat(new_mat);
				/* 面を反転 */
				if(l_pos0 < 0){
					convint nodes[8];
					layer_ele[l_pos][id].getNodes(nodes);
					convint tmp = nodes[0];
					nodes[0] = nodes[1];
					nodes[1] = tmp;
					tmp = nodes[2];
					nodes[2] = nodes[3];
					nodes[3] = tmp;
					layer_ele[l_pos][id].setNodes(nodes);
				}
			}
		}
	}
	delete[] ele2D_to3D_surf;
}

/*
//===============================================================
// ○ 積み上げサブルーチン４：積み上げ確定
//===============================================================*/
void MeshConverter::set_layer_fix(ConvMesh& mesh, int no_layer, vector<ConvNode>* layer_node, vector<ConvElement> *layer_ele){

	const convint nodesize_2d = mesh.node.size();
	mesh.node.clear();
	mesh.element.clear();
	vector<ConvNode> empt_n;
	vector<ConvElement> empt_e;
	empt_n.resize(0);
	empt_e.resize(0);

	/* 現時点での全節点数は */
	convint n_size_all=0;
	convint e_size_all=0;
	for(int kk = 0 ; kk < no_layer+1 ; kk++ ){
		n_size_all += layer_node[kk].size();
	}
	for(int kk = 0 ; kk < no_layer ; kk++ ){
		e_size_all += layer_ele[kk].size();
	}
	if( n_size_all != nodesize_2d*(no_layer+1) ){
		cout << "node is is not matched in 2D size "<< n_size_all << ", " <<  nodesize_2d*(no_layer+1) <<endl;
		getchar();
	}
#ifdef PRINT_CONV_INFO
	cout << "total N size " << n_size_all << endl;
	cout << "total E size " << e_size_all << endl;
#endif
	try{
		mesh.node.reserve(n_size_all);
		mesh.element.reserve(e_size_all);
	}catch(...){
		cout << "MEM OVER!"<<endl;
		getchar();
	}

	/* 節点追加フラグを作っておく */
	bool *active_l_node = new bool[n_size_all];
	for(convint kk = 0 ; kk < n_size_all ; kk++ ){
		active_l_node[kk] = false;
	}

	/* まず要素を一層ずつ追加していく */
	for(int kk = 0 ; kk < no_layer ; kk++ ){
		const convint e_size_lay = layer_ele[kk].size();
		//cout << kk << ", " << e_size_lay << endl;
		for(convint i = 0 ; i < e_size_lay ; i++){
			/* 材料IDが負なら追加しない */
			int mat = layer_ele[kk][i].getMat();
			if(mat < 0){
				continue;
			}
			/* 追加 */
			mesh.element.push_back( (layer_ele[kk][i]) );
			/* 追加された要素が使っている節点は、追加フラグをonに */
			int n_num = layer_ele[kk][i].getNodeNum();
			for(int j = 0 ; j < n_num ; j++){
				convint n_id = layer_ele[kk][i].getNode(j);
				if(n_id >= n_size_all){
					cout << "error in lay node fix !!"<<endl;
					exit(1);
				}
				active_l_node[n_id] = true;
			}
		}
		/* 終わったらその層のデータを空にしてメモリ節約 */
		layer_ele[kk].swap(empt_e);
	}
	convint count_n=0;
	convint *new_node_id = new convint[n_size_all];
	for(convint i = 0 ; i < n_size_all ; i++){			
		if(active_l_node[i]){
			new_node_id[i] = count_n;
			count_n++;
		}else{
			new_node_id[i] = -99;
		}
	}

	/* 生存フラグがonの節点を追加 */
	convint current_num=0;
	for(int kk = 0 ; kk < no_layer+1 ; kk++ ){
		const convint num = layer_node[kk].size();
		for(convint i = 0 ; i < num ; i++){
			if(active_l_node[current_num+i]){
				/* 追加 */
				mesh.node.push_back(layer_node[kk][i]);
			}
		}
		current_num += num;
		/* 終わったらその層のデータを空にしてメモリ節約 */
		layer_node[kk].swap(empt_n);
	}
	/* 上から番号を付け直して終わる */
	const convint n_size2 = mesh.node.size();
	const convint e_size2 = mesh.element.size();

	for(convint i = 0 ; i < n_size2 ; i++){
		mesh.node[i].setID(i);
	}
	for(convint i = 0 ; i < e_size2 ; i++){
		mesh.element[i].setID(i);
		int n_num = mesh.element[i].getNodeNum();
		/* 要素節点IDもなおす */
		for(int j = 0 ; j < n_num ; j++){
			convint n_id = mesh.element[i].getNode(j);
			convint new_id = new_node_id[n_id];
			if(new_id < 0){
				cout << "fixed new node id is error!"<<endl;
				exit(1);
			}
			mesh.element[i].setNodes(j, new_id);
		}
	}
	delete[] new_node_id;
	delete[] active_l_node;
}

/**/
};
};


