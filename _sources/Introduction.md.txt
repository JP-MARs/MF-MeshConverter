<div style="text-align: justify;">

# Introduction

## About this OSS
&ensp;このOSSは、メッシュ修正操作機能を備える、メッシュコンバータライブラリです。
実行ファイル形式の提供と、自作ソフトに組み込むためにライブラリ形式でも提供しています。
前者はjsonを使ったコマンドでメッシュを操作できます。後者は、jsonで呼び出していた関数を各自で呼び出せばメッシュを操作できます。

### 実行形式での利用方法（フォーマット変換）
&ensp;基本機能は、あるメッシュフォーマットを別のフォーマットに変換する機能です。
実行すると、ファイル名を聞かれるので、読み込みたいファイルを指定します。
その後、書き出したいファイル名を打ち込むと、フォーマットを変えて書き出します。
基本的には拡張子を自動で認識します。

```
type file name (with file extension)
test.unv
type output file name (with file extension)
test.vtk
Modify Mesh ? : yes=1 or 2, no=0
 for [yes(1)], prepare [meshmodi.json] file
 for [yes(2)], control filename (json format) will be asked

0
```

**読み込み対応形式**
* unv (univ/ideal)
* neu (FEMAP neutral)
* vtk (paraview relagy)
* msh (GMSH)
* mesh (Medit)
* 自作形式 (jpmmesh, jpmmesh_e)

**書き出し対応形式**
* unv (univ/ideal)
* neu (FEMAP neutral)
* vtk (paraview relagy)
* 自作形式 (jpmmesh, jpmmesh_e)

&ensp;なお、普段は利用しないであろうメッシュフォーマットもいくつか対応しています。
そのようなフォーマットの自動認識はさせていないので、ソースを見て、各自で対応する読み込み関数を呼び出せば使えます。
FileReadWrite内のhppとcppをご覧ください。

### 実行形式での利用方法（メッシュ操作機能）
&ensp;メッシュを読み込んだ後、JSONで記述したコマンドに従ってメッシュを操作してから書き出すことができます。
利用可能なコマンドはリポジトリのトップにあるpdfファイル(日本語)にまとまっているので参照してください。
実行例は下記のとおりです。

```
type file name (with file extension)
test.unv
type output file name (with file extension)
test.vtk
Modify Mesh ? : yes=1 or 2, no=0
 for [yes(1)], prepare [meshmodi.json] file
 for [yes(2)], control filename (json format) will be asked

1
```

変換コマンドで「１」を指定すると「meshmodi.json」を読み込んで、その中の記述に従ってメッシュを操作します。
変換コマンドで「２」を指定すると、好きなjsonファイル名を指定します。あとは同じです。
例えば、meshmodi.jsonを、下記のように記述します。

```
[
   {
      "name": "DELELEMAT",
      "range": [-1, 0]
   },
   {
      "name": "DELELEMAT",
      "range": [4, 8]
   }
]
```

この例では、指定した材料IDの範囲になっている要素を削除します。

&ensp;また、コマンドライン実行時に、ファイル名などを引数に追加すれば、コマンド手打ちせずに自動で変換処理してくれます。

・入力メッシュファイル名と出力メッシュファイルを指定。フォーマット変換だけしてくれる。
```
./mf_meshconverter.out ./input.vtk ./output.vtk
```

・入力メッシュファイル名・変換コマンドjsonファイル名・出力メッシュファイルを指定。メッシュ操作をしたうえでフォーマット変換する。
```
./mf_meshconverter.out ./input.vtk ./meshmodi.json ./output.vtk
```


## About modification functions
&ensp;メッシュを操作する処理を多数用意しています。
利用可能なコマンドはリポジトリのトップにあるpdfファイル(日本語)にまとまっているので参照してください。

大別して、下記に関する操作を実装しています。
* 要素の削除
* 節点の移動
* 材料IDの変更
* 要素の追加
* 回転・鏡面・並進移動・コピー
* 複数メッシュの結合
* 2次元メッシュの積み上げ3D化


### Json format
&ensp;メッシュ操作コマンドはJSONで記述します。
JSONでまず配列を定義し、順々に辞書型でコマンドを入力してください。配列の上から順に操作を実行していきます。


</div>
