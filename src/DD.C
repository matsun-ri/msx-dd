/**************************************************************
	DD.com : Dual-screen filer for MSX-DOS2
**************************************************************/
#include <stdio.h>
/*#include <bdosfunc.h>*/		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
#include <math.h>			/* SLONG */
#include "dd.h"

/* DD_INIT.cにて定義 */
extern VOID init( argc, argv, tab );

/* dd_sub.cにて定義 */
extern int getfiles( path, files );

/* dd_drv.cにて定義 */
extern char *sputsize( str, psl );
extern STATUS getdriveinfo( path, svollabel, scapa, sfreebar, itabpage );

/* dd_file.cにて定義 */
extern VOID filelist( files, ifiles, ilistcur, ilistpage, itabpage );
extern BOOL keyloop( files, ifiles, ilistcur, ilistpage, keyin, b_shift, itabpage );

/* dd_scrn.cにて定義 */
extern VOID msgbox( x, y, stitle, stext );
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );

/* DD_EXEC.cにて定義 */
extern STATUS setenv( skey, svalue );

/* DD_OPR.cにて定義 */
extern int chuprdir( tabs, itabpage );
extern int operation( ch, b_shift, tabs, itabpage );

/* DD_KEY.cにて定義 */
extern BOOL keyloop( tab, itabpage, keyin, b_shift );
extern VOID put_capa_or_mark( tab, itabpage );



/**************************************************************
	指定したタブページ枠描画
	chtab()→tabframe()
**************************************************************/
VOID tabframe( itabpage )
int		itabpage;	/* r/o 操作中のタブページ 0or1 */
{
	int i;
	
	vputsgraph( LIST_X - 2 + itabpage*22, LIST_Y - 3, "XWWWWWWWWWWWWWWWWY" );
	vputsgraph( LIST_X - 2 + itabpage*22, LIST_Y - 2, "V@@@@@@@@@@@@@@@@V" );
	
	switch( itabpage ) {
		case 0:	/* 左ページ */
			vputsgraph( LIST_X - 2, LIST_Y - 1,
									"V@@@@@@@@@@@@@@@@ZWWWWWWWWWWWWWWWWWWWY@V" );
			break;

		case 1:	/* 右ページ */
			vputsgraph( LIST_X - 2, LIST_Y - 1,
									"V@XWWWWWWWWWWWWWWWWWWW[@@@@@@@@@@@@@@@@V" );
			break;

		default:
			break;
	}
	
	/* リスト部分 */
	for ( i=3; i<3 + LIST_H + 2; i++ ) {
		vputsgraph( LIST_X - 2 + itabpage*2, LIST_Y - 3 + i, 
									"V@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@V" );
	}

	switch( itabpage ) {
		case 0:	/* 左ページ */
			vputsgraph( LIST_X - 2, LIST_Y + LIST_H + 0,
									"ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[@V" );
			vputsgraph( LIST_X - 2, LIST_Y + LIST_H + 1,
									"@@ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[" );
			vputsgraph( LIST_X - 2 + 36, LIST_Y,  "R" );
			vputsgraph( LIST_X - 2 + 36, LIST_Y + LIST_H -1,  "Q" );
			break;

		case 1:	/* 右ページ */
			vputsgraph( LIST_X - 2, LIST_Y + LIST_H + 0,
									"V@ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[" );
			vputsgraph( LIST_X - 2, LIST_Y + LIST_H + 1,
									"ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[@@" );
			vputsgraph( LIST_X - 2 + 38, LIST_Y,  "R" );
			vputsgraph( LIST_X - 2 + 38, LIST_Y + LIST_H -1,  "Q" );
			break;

		default:
			break;
	}
}



/**************************************************************
	タブページの切り替え対応
	ボリュームラベル・総容量・カレントディレクトリの表示
	main()→chtab()
**************************************************************/
VOID chtab( tabs, itabpage )
DD_TAB	tabs[];		/* r/o タブページ1枚を表現する構造体 */
int		itabpage;	/* r/o 操作中のタブページ 0or1 */
{
	/* タブページ枠描画 */
	tabframe( itabpage );

	/* アクティブなタブページのボリュームラベルの表示 */
	locate( (TINY)(3 + itabpage * 22), (TINY)2 );
	puts( tabs[itabpage].label );

	/* ドライブの総容量 または マークファイルの表示 */
	put_capa_or_mark( &tabs[itabpage], itabpage );

	if ( 0 == itabpage ){

		/* 左タブページがアクティブな場合*/

		/* 左タブ：カレントディレクトリの表示 */
		vputsgraph(	LIST_X - 1, LIST_Y + LIST_H, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" );
		vputs_b(	LIST_X - 1, LIST_Y + LIST_H, tabs[itabpage].shortpath );

		/* 右タブ：カレントディレクトリ表示 */
		locate( (TINY)(LIST_X + 36 - strlen(tabs[1-itabpage].shortpath) ), (TINY)(LIST_Y + LIST_H + 1) );
		puts( tabs[1-itabpage].shortpath );
		
		/* アクティブでないタブは移動しないのでここで―――で消す必要はないはず */

	} else {

		/* 右タブページがアクティブな場合*/

		/* 右タブ：カレントディレクトリ表示 */
		vputsgraph(	LIST_X + 1, LIST_Y + LIST_H, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" );
		vputs_b(	LIST_X + 36 - strlen(tabs[itabpage].shortpath), LIST_Y + LIST_H, tabs[itabpage].shortpath );

		/* 左タブ：カレントディレクトリの表示 */
		locate( (TINY)(LIST_X - 1), (TINY)(LIST_Y + LIST_H + 1) );
		puts( tabs[1-itabpage].shortpath );
	}
	
}


/**************************************************************
	main procedure
**************************************************************/
int main( argc, argv )
int		argc;
char	*argv[];
{
	int		iret = 0;
	int		itabpage = 0;			/* 操作中のタブページ 0or1 */
	int		loopstatus = LS_CHGTAB;	/* メインループの状態 */
	char	ch;						/* 推されたキー */
	TINY	keymod = 0;				/* 修飾キー押下状態 */
	DD_TAB	tabs[TAB_MAX];			/* タブページ1枚を表現する構造体 */


	/* 初期化処理 */
	init( argc, argv, tabs );
	
	
	/* メインループ */
	while( loopstatus < LS_BORDER ) {

		/* ドライブ情報の取得が必要なら */
		if ( TS_CHDRV == tabs[itabpage].status ) {
			
			/* ドライブ情報の取得 */
			if( OK == getdriveinfo( tabs[itabpage].path, tabs[itabpage].label, tabs[itabpage].capa, tabs[itabpage].bar, itabpage ) ) {

				tabs[itabpage].status = TS_CHDIR; /* 後でファイル一覧の取得 */

			}
		}
		
		/* ファイル一覧の取得が必要なら */
		if ( TS_CHDIR == tabs[itabpage].status ) {

			/* 指定されたパスのファイル一覧を得る */
			tabs[itabpage].count = getfiles( tabs[itabpage].path, tabs[itabpage].files );
			tabs[itabpage].cursor = 0;	/* カーソルが何個目のファイルを指しているか */
			tabs[itabpage].page = 0;	/* ページ */
			tabs[itabpage].markedct = 0;					/* マークされたファイル数 */
			itosl( (SLONG *)(tabs[itabpage].markedsize), 0 ); /* ↑のファイルサイズ */
			tabs[itabpage].status = TS_NOP;
			loopstatus = LS_CHGTAB;	/* 後でボリュームラベルと総容量を表示 */

		}

		/* タブページの切り替え対応 */
		/* 	ボリュームラベル・総容量・カレントディレクトリの表示 */
		if ( LS_CHGTAB == loopstatus ) {
			chtab( tabs, itabpage );
			loopstatus = LS_LOOP;
		}
		
		/* カーソル位置からページ番号を得る */
		tabs[itabpage].page = tabs[itabpage].cursor / LIST_H;

		/* ファイル一覧の表示 冗長だけど全部R/Oなので許して */
		filelist( tabs[itabpage].files, tabs[itabpage].count, tabs[itabpage].cursor, tabs[itabpage].page, itabpage );

		/* キー入力処理 */
/*		keyloop( tabs[itabpage].files, tabs[itabpage].count, &tabs[itabpage].cursor, &tabs[itabpage].page, &tabs[itabpage].marked, &ch, &bshift, itabpage ); */
		keyloop( &tabs[ itabpage ], itabpage, &ch, &keymod );
		
/*
locate((TINY)1,(TINY)2);
printf("* <main():%d>", (int)keymod );
*/

/*		
locate( (TINY)LIST_X, (TINY)(LIST_Y + LIST_H + 4) );
printf( "* <targer:%d=%s>\n", ilistcur, files[ilistcur].name );
*/		
		/* ユーザの指示に応じた処理を行う */
		loopstatus = operation( ch, keymod, tabs, &itabpage );

	}
	
	/* 単なる終了であれば */
	if( LS_END == loopstatus ) {
		/* 外部プログラム実行用バッチファイル実行用環境変数をクリア */
		setenv( "DD_EXEC", "" );
		
		/* 環境変数初期化 */
/*		setenv( "DD_EXEC1", "" );
		setenv( "DD_EXEC2", "" );
		setenv( "DD_EXEC3", "" );
		setenv( "DD_EXEC4", "" );
		setenv( "DD_EXEC5", "" );
		setenv( "DD_EXEC6", "" );
		setenv( "DD_EXEC7", "" );
*/
	}

printf("loopstatus:[%d]\n", loopstatus );

}
