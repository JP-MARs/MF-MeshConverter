# MF-MeshConverter - mesh converter command-line software with mesh-modification functions
フォーマットの変更とメッシュ操作機能を有する、コマンドラインのメッシュ変換ソフトウェアです。

あるメッシュフォーマットを別のフォーマットに変換します。その際に、指定の書式で変換コマンドを記載することで、メッシュ情報を操作することができます。例えば節点の移動や材料ID変更、移動やコピーなど。複数のメッシュを結合させたり、2次元メッシュを積み上げ方式で3次元可したりすることもできます。

※本ソフトは、開発者が個人的に作成していたメッシュ操作機能を整理してOSS化したものです。整理する際に、一部のメッシュ操作機能などにバグが入った可能性もあります。もし、使っていておかしい挙動がありましたら、ぜひご指摘ください。

## 対応フォーマット
**読み込み対応形式**
* unv/ids (univ/ideas)
* neu (FEMAP neutral)
* vtk (paraview relagy)
* msh (GMSH)
* mesh (Medit)
* atls (ATLAS形式～本来こんなメッシュ用拡張子はないと思いますが、ATLAS形式拡張子として仮設定)
* 自作形式 (jpmmesh, jpmmesh_e)

**書き出し対応形式**
* unv/ids (univ/ideas)
* neu (FEMAP neutral)
* vtk (paraview relagy)
* atls (ATLAS形式～本来こんなメッシュ用拡張子はないと思いますが、ATLAS形式拡張子として仮設定)
* 自作形式 (jpmmesh, jpmmesh_e)

## 対応要素
現在、1次要素にのみ対応しています。点要素、線要素、2次元ソリッド(三角形・四角形)、3次元ソリッド(四面体・プリズム・六面体)です。

2次要素は要素タイプだけ定義済みですが、メッシュ入出力および変換操作の対応外です。


## 利用方法
### 基本利用
Windows用にはコンパイルしたexeファイルがあるのでそのまま使えます(mf_meshconverter.exe)。
起動するとファイル名を聞かれるので読み込むファイルを拡張子込みで指定してください。拡張子を読み取ってフォーマットを自動認識します。書き出し時も同様です。

ファイル名を指定後、メッシュ操作をするか聞かれます。「0」を指定すると何もせずに終わりますが、`1`か`2`を指定すると、指定した方法に従ってjsonファイルを読みに行き、jsonに記載のコマンドに従ってメッシュを操作します。

メッシュ操作のコマンドは「C++版MeshConvの使用方法とコマンドjson.pdf」に記載していますので、利用したい操作を探し、書式に従ってコマンドを入力してください。

コマンドラインで実行するときは、引数を追加すれば、上記のようなファイル名のタイプを省略可能です。

・フォーマット変更だけする場合
```bash
> ./mf_meshconverter.exe ./input.vtk ./output.vtk
```
・メッシュ操作もする場合
```bash
> ./mf_meshconverter.exe ./input.vtk ./meshmodi.json ./output.vtk
```

### 自作プログラムでの利用
buildすると、静的ライブラリも作成されます(libMFMeshConverter)。自作のC++プログラムに組み込みたい場合はライブラリ形式でも利用できます。includeフォルダとコンパイル済みのライブラリファイルをリンクして利用してください。

## その他詳細
詳しくは解説用のホームページを作成しているので参照してください。
（本ページ右上にリンク）


# MF-MeshConverter - Mesh Converter Command-Line Software with Mesh-Modification Functions

MF-MeshConverter is command-line mesh conversion software that supports format conversion and mesh-modification functions.

It converts a mesh from one mesh format to another. During conversion, mesh information can be modified by writing conversion commands in the specified format. For example, you can move nodes, change material IDs, translate or copy mesh data, and so on. It is also possible to merge multiple meshes or generate a 3D mesh from a 2D mesh using a stacking method.

Note: This software was originally developed by the author as a personal collection of mesh-modification functions, which has since been reorganized and released as OSS. During the reorganization, bugs may have been introduced into some mesh-modification functions. If you notice any unexpected behavior while using the software, please report it.

## Supported Elements

Currently, only first-order elements are supported. These include point elements, line elements, 2D solid elements, such as triangles and quadrilaterals, and 3D solid elements, such as tetrahedra, prisms, and hexahedra.

Second-order element types have been defined, but mesh input/output and conversion operations for them are not supported.

## Usage

### Basic Usage

For Windows, a precompiled executable file is available and can be used as is: `mf_meshconverter.exe`.

When launched, the program asks for a file name. Specify the input file name, including its extension. The program reads the extension and automatically detects the format. The same applies when writing the output file.

After you specify the file name, the program asks whether to perform mesh modification. If you enter `0`, the program exits without performing any mesh modification. If you enter `1` or `2`, it reads a JSON file according to the specified method and modifies the mesh according to the commands written in the JSON file.

Mesh-modification commands are described in the pdf file (sorry, Japanese only...).

When running the program from the command line, you can add arguments to omit the interactive file-name prompts described above.

For format conversion only:

```bash
./mf_meshconverter.exe ./input.vtk ./output.vtk
```

For format conversion with mesh modification:

```bash
./mf_meshconverter.exe ./input.vtk ./meshmodi.json ./output.vtk
```

### Use in Custom Programs

When the project is built, a static library is also created: `libMFMeshConverter`.

If you want to integrate MF-MeshConverter into your own C++ program, you can also use it as a library. Link the `include` folder and the precompiled library file to your program.

## Additional Details

For more details, please refer to the explanatory homepage.

A link is available in the upper-right corner of this page.
