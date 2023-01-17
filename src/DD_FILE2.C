/**************************************************************
	ファイル一覧の取得・表示
**************************************************************/
#include <stdio.h>
/* #include <bdosfunc.h> */
#include <glib.h>		/* ldirvm */
/* #include <msxbios.h>	 locate */
/* #include <math.h>	 slcpy */
#include "dd.h"


/* dd_scrn.cにて定義 */
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );




/**************************************************************
	ファイル一覧を表示
	main()→filelist()
**************************************************************/
VOID filelist( files, ifiles, ilistcur, ilistpage, itabpage )
FILES	*files;		/* r/o FILES型構造体へのポインタ */
int		ifiles;		/* r/o ファイル数 */
int		ilistcur;	/* r/o カーソルが何個目のファイルを指しているか */
int		ilistpage;	/* r/o ページ 最初=0 */
int		itabpage;	/* r/o 操作中のタブページ 0or1 */
{
	int		i, ipos,  iup, idown;


	/* 縦棒グラフ表示 */

	iup = 0;

	if( 0 == ifiles ) { 

		/* ファイルがない場合は□で埋める */
		iup = idown = LIST_H - 2;

	} else {

		/* まず空白で埋める */
		for ( i=0; i < LIST_H - 2; i++ ) {
			vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "U" );
		}

		/* 表示中のページの最上と最下のファイルに当たる部分を求める */
		/* ページ内で一番上のファイル 分母=LIST_H-2 */
		iup = ilistpage * (LIST_H-2) * (LIST_H-2) / ifiles;
		/* ページ内で一番下のファイル 分母=LIST_H-2 */
		ipos = (ilistpage + 1) * (LIST_H-2) - 1;
		/* 最終ページは剰余分しか表示されていないので修正 */
		if ( ifiles <= ipos ) ipos = ifiles - 1;
		idown = (LIST_H-2) * ipos / (ifiles - 1);
		if ( idown <= iup ) idown = iup + 1; /* 最低でも1ループさせる */
		
	}

	/* 上を□で埋める */
	for( i=0; i<iup; i++ ) {
		vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "U" );
	}
	/* 間を■で埋める */
	for( i=iup; i<idown; i++ ) {
		vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "\\" );
	}
	/* 下を□で埋める */
	for( i=idown; i<LIST_H-2; i++ ) {
		vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "U" );
	}

	/* ファイル名の表示 */
	for ( i = 0; i < LIST_H; i++ ) {
		
		ipos = LIST_H * ilistpage + i;

		/* まだ最後のファイルではない？ */
		if( ipos < ifiles ) {
			
			/* ファイルを表示 */
/*
			puts( files[ ipos ].image );
*/
			ldirvm(  (LIST_Y + i            - 1) * 40 
					+ LIST_X + itabpage * 2 - 1,
					files[ ipos ].image,	
					34 );
					
			/* マークの表示/非表示 */
			locate( (TINY)(LIST_X + itabpage*2 - 1), (TINY)(LIST_Y + i) );
			if( files[ ipos ].ismarked ) {
				putchar( '*' ); 
			} else {
				putchar( ' ' ); 
/*				putchar( '-' ); */
			}

		} else {
			
			/* ファイルではない場所 */
			filvrm(  (LIST_Y + i            - 1 ) *40 
					+ LIST_X + itabpage * 2 - 2,
					35, ' ' );
/*					35, '/' );
*/
/*
			/* マークの消去 */
			putchar( ' ' ); 
			/*  スペースを表示*/
			/*     1234567890123456789012345678901234567890*/
			puts( "----------------------------------" );
*/
		}
	}
/*		
	/* カーソル表示 */
	putcursor( ilistcur + itabpage*2 , 'g', ' ' );
*/
	
	/* カーソル位置のファイル名を太字表示する */
	vputs_b( LIST_X + itabpage*2, LIST_Y + ilistcur % LIST_H, files[ ilistcur ].image );

}




