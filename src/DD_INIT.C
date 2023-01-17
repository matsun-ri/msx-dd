/**************************************************************
	初期処理
**************************************************************/
#include <stdio.h>
#include <msxbios.h>
#include <math.h>		/* itosl */
#include "dd.h"

/* dd_scrn.cにて定義 */
extern VOID setpcg();
extern VOID window_l();

/* dd_sub.cにて定義 */
extern char *addbackslash( s );
extern char *makeshortpath( path, shortpath );

/**************************************************************
	初期化処理
	・表示関連初期化
	・配列変数初期化
	・コマンドライン引数の処理
	main()→init()
**************************************************************/
VOID init( argc, argv, tabs )
int		argc;		/* r/o コマンドライン引数 */
char	*argv[];	/* r/o コマンドライン引数 */
DD_TAB	tabs[];		/* w/o 初期化対象 */
{
	int		i = 0;
	int 	j = 0;
	char	swork[MAXPATH];


	/* コンソールバッファの無効化 */
	setbuf( stdout, NULL );

	/* 表示関係の初期化 */
	*(TINY *)(0xf3ae) = 40;			/* LINL40←WIDTH */
	screen( (TINY)0 );				/* screen 0:width 40に設定 */
	setpcg();						/* キャラクタパターン定義 */
/*
	window_l();						/* 画面枠初期化 */
*/

	/* タブページ構造体初期化 */
	for ( i=0; i<TAB_MAX; i++ ){
		tabs[i].status = TS_CHDRV;	/* 初回表示時に取得させる */
		tabs[i].count = 0;
		tabs[i].cursor = 0;
		tabs[i].page = 0;
		tabs[i].markedct = 0;
		itosl( (SLONG *)(tabs[i].markedsize), 0 );
		strcpy( tabs[i].path, "" );
		strcpy( tabs[i].shortpath, "" );
		strcpy( tabs[i].label, "" );
		strcpy( tabs[i].capa, "" );
		strcpy( tabs[i].bar,"" );
		for ( j=0; j<MAXFILES; j++ ){
			strcpy( tabs[i].files[j].name, "" );
			strcpy( tabs[i].files[j].image, "" );
			tabs[i].files[j].attr = (TINY)10;
	/*		tabs[i].files[j].time = 0;
			tabs[i].files[j].date = 0;	*/
			itosl( (SLONG *)(tabs[i].files[j].size), 0 );
			tabs[i].files[j].ismarked = FALSE;
			tabs[i].files[j].isalive = FALSE;
		}
	}

	/* 引数があればパスとして扱う */
	/* （前回の外部コマンド実行時に、その時のパスを */
	/*   引数として渡すようにしている） */
	if ( 2 < argc ){
		/* 引数が2つ→左タブ・右タブに振り分け */
		strcpy( tabs[ 0 ].path, argv[1] );
		strcpy( tabs[ 1 ].path, argv[2] );
	} else if ( 1 < argc ){
		/* 引数が1つ→左タブは引数・右タブはカレントディレクトリ */
		strcpy( tabs[ 0 ].path, argv[1] );
		getcwd( tabs[ 1 ].path, sizeof( tabs[ 1 ].path ));
	} else {
		/* 引数なし→左右ともカレントディレクトリ */
		getcwd( tabs[ 0 ].path, sizeof( tabs[ 0 ].path ) );
		strcpy( tabs[ 1 ].path, tabs[ 0 ].path );
	}
	

	/* 最後に\を足す */
	addbackslash( tabs[ 0 ].path );
	addbackslash( tabs[ 1 ].path );

	/* パス文字列の短縮形を作成する */
	makeshortpath( tabs[ 0 ].path, tabs[ 0 ].shortpath );
	makeshortpath( tabs[ 1 ].path, tabs[ 1 ].shortpath );
}



