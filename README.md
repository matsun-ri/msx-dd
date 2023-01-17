# MSX-DOS2用2画面ファイラ『DD』ソースコードおきば

## MSX DDとは
- MSX-Cで作りかけたまま、4年以上寝かせている **未完成のファイラ** です。
- スマホでも使いやすいようにと思い、SCREEN0:WIDTH40の画面モードで作っています。
- 狭い画面を有効活用するため、2画面をタブ形式で表示しています。
- 解像度を要求しないので、MSX0 Stack上（DOS2が同梱されるようです）で活躍できるかもしれません。
- どなたかに、MSX-Cのトレーニングまたはリハビリがてら完成させていただければ幸いです。
- 名前は dual directoryの略みたいな感じです（Linuxのコマンドと被ってますが……）。
- **未完成です。自己責任で実行してください。**
  - テスト環境以外で実行しないでください。
  - 実行の結果、ファイルやドライブの破壊がありえます。
  - 被害に対するクレームは一切受け付けません。

## 画面サンプル
- <img src="/img/dd-1.jpg" alt="画面サンプル" width="840" height="600">  
標準画面。
- <img src="/img/dd-2.jpg" alt="画面サンプル" width="840" height="600">  
右画面に切り替えた状態。  
- <img src="/img/dd-3.jpg" alt="画面サンプル" width="840" height="600">  
ヘルプ画面（メニュー一覧）。
- <img src="/img/dd-4.jpg" alt="画面サンプル" width="840" height="600">  
複数のファイルを選択した状態。ボリューム名の下に選択数と合計ファイルサイズを表示。
- <img src="/img/dd-5.jpg" alt="画面サンプル" width="840" height="600">  
複数のファイルをコピー中の状態。コピー先はもう一方のタブで表示中のディレクトリ。
- <img src="/img/dd-6.jpg" alt="画面サンプル" width="840" height="600">  
リネーム中の画面。

## 実行環境
MSX-DOS2 または 互換OS
- 外部コマンドの実行のために環境変数を使用しているので、MSX-DOS2以降の環境が必要です。
- 開発はNextor上で行いました。

## 実行方法
同一パスにD.BAT, DD.COM, DD.CFGを配置し、D.BATを実行します。

## 実装済みの機能
- 2画面それぞれにファイル一覧の表示、パスの移動
  - 2画面間の移動は左右カーソルキー
  - SHIFT+上下カーソルキーでページ送り
- ドライブの総容量（数値）と使用量（プログレスバー）の表示
- スペースキーによる複数ファイルの選択。選択数と合計容量の表示
- もう一方の画面のパスへ向けて複数ファイルのコピー(Cキー)と移動(Mキー)
- 複数ファイルとディレクトリの削除(Dキー)
  - ディレクトリが空でない場合は中断
  - ※ 実行確認なく、いきなり削除します。
- ディレクトリの作成(Kキー)
- ドライブの切り替え
  - Lキー押下後、ドライブレターを指定
  - 1～8キーでA:～H:ドライブへ切り替え
- ファイル名の変更(Nキー)
- 終了(Qキー、あるいはESCキー)
- カレントディレクトリエントリの再読み込み(Rキー、あるいは0キー)
- ルートディレクトリへの移動(Tキー、あるいは¥キー)
- 一つ上のディレクトリへの移動(Uキー、あるいはBSキー)
- 拡張子連動実行(RETURNキーとSHIFT+RETURNキーで機能の切り替え)

## ファイルの説明
- 必須ファイル（プログラム）  
  - run\D.BAT  
DD.COMを起こすためのバッチファイル（拡張子連動実行後にDD.COMに戻るために噛ませています。）
  - run\DD.COM  
2画面ファイラの本体です。
  - run\DD.CFG  
拡張子連動実行についての設定を記述します。
- ソースファイル（タブは4文字を想定しています）
  - DD.H  
ヘッダファイル。
  - DD.C  
メイン関数、タブページの表示など
  - DD_KEY.C  
ドライブの容量表示、ファイルのマーク、キー入力ループなど
  - DD_OPR.C  
キー入力に対する動作
  - DD_OPR2.C  
ドライブ変更、ディレクトリ移動、ディレクトリ作成、ファイル名変更
  - DD_COPY.C  
ファイルのコピー、移動、削除
  - DD_INIT.C  
初期化処理
  - DD_EXEC.C  
外部プログラム実行（外部プログラムの実行のためにDDG.BATというバッチファイルを作るようになっていますが、現時点ではコメントアウトしています）
  - DD_FILE2.C  
ファイル一覧の表示
  - DD_FILE.C  
ファイル一覧の取得
  - DD_DRV.C  
ドライブ情報の取得、ドライブの変更
  - DD_SUB.C  
BDOSコール、文字列操作
  - DD_INPUT.C  
入力ボックス（ファイル名、ドライブ名）
  - DD_MSG.C  
画面表示（メッセージボックス、罫線、太字、プログレスバー）
  - DD_SCRN.C  
キャラクタパターン、棒グラフ、ヘルプ画面
- MAKEBAT.BAT  
MSX-Cでこれらのソースファイルをコンパイルするための参考（無条件にすべてのソースファイルをコンパイルするので大変な時間がかかります）  
※最後あたりでリンカL80のコマンドライン文字列制限にひっかからないようにするため、すべての.relファイルを2文字の短いファイル名にコピーしなおすという訳の分からないことをしています。
- MAKEFILE  
自作MAKEのためのMAKEFILEですが、参考までに添付します。  
（MAKEBAT.BATは、自作MAKEがこのMAKEFILEから生成しました）
- ディスクイメージ
  - disk-image/DD-src&bin.dsk  
必須ファイルとサンプルコードが入ったディスクイメージ。

## DD.CFG（拡張子連動実行）の記述
- 拡張子-タブコード-実行するコマンドライン の順に記述します。
- 同じ拡張子を、小文字と大文字で書くことができます。
  - 小文字は単にRETURNキーを押した場合に実行されます。
  - 大文字はSHIFTキーを押しながらRETURNキーを押した場合に実行されます。
- 展開されるメタ文字は以下の通りです。
  - $C カーソル位置のファイル名
  - $D 別のタブのカーソル位置のファイル名
  - $K 実行後にキー入力待ちをする場合に記述します
  - $P カレントタブのパス
  - $Q 別のタブのパス
- 詳しくは、DD.CFG, DD_EXEC.Cを参照ください。

## 制限
- 起動直後にカレントではない方のタブを表示していません。
- 各種エラーチェックを行っていません。
- ファイルをすべて消しても、1行目のファイル名が表示されたままです。（現状では、.が表示されるのは正しい動作としています）
- その他にも、数多くのバグ、未実装があります。（拡張子連動実行がとりあえず動く状態なことにびっくりするくらい）
- 20世紀以来C言語を書いていないのと、オープンソースという世界を理解していないので、ソースが汚いとかコード規約がどうのとかのクレームは困ります。

## すべきこと
- 低解像度の画面で使いやすいよう、いい感じにしていただければと思います。
- 最終的には、ジョイパッドである程度の操作ができるようにするつもりでした（スマホ上でWebMSXを使ったり、MSX0 Stackで役に立つのでは）

## あとがき
- GitHubの使い方がよくわかっていません。
- matsun-riは初回公開時以上の機能追加を行う予定はありません（主に時間とセロトニンの不足のため）。ごめんなさい。
