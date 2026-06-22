/**
* @file ConvConstants.hpp
* @brief Constants defined for Mesh Conversion Library
*
* このファイルはメッシュ変換ライブラリ内で共通して用いる定数、
* マクロ、型エイリアスを定義します。
*
*/
#ifndef DEF_CONSTANTS_CONV_INFO
#define DEF_CONSTANTS_CONV_INFO


/* オリジナル名前空間(JPMARsライブラリ) */
/**
* @namespace JPMRspace
* @brief Original namespace for JP-MARs
*/
namespace JPMRspace{

/* オリジナル名前空間(メッシュコンバータ用) */
/**
* @namespace MeshConvLib
* @brief namespace for Mesh Converter
*/
namespace MeshConvLib{


/* これがDefineされていたら、変換の経過をprintする */
/**
* @def PRINT_CONV_INFO
*/
#define PRINT_CONV_INFO

/* メッシュコンバータ用のint定義  */
/**
* @typedef convint
* @brief Aliaas for integer type used in Mesh Converter
*/
using convint = int;//size_t;


/* 円周率 */
/**
* @brief PI for mesh converter
*/
constexpr double DEF_CONV_PI = 3.14159265358979323846;		


};

};


#endif
