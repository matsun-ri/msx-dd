/**************************************************************
	キーイン処理
**************************************************************/
#include <stdio.h>
/* #include <bdosfunc.h> */
#include <glib.h>		/* ldirvm */
/* #include <msxbios.h>	 locate */
#include <math.h>			/* SLONG */
#include "dd.h"

/* dd_drv.cにて定義 */
extern char *sputsize( str, psl );

/* dd_scrn.cにて定義 */
extern VOID msgbox( x, y, stitle, stext );
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );



/**************************************************************
	ドライブの総容量 または マークファイルの表示
	・マークされたファイルがない場合は総容量の表示
	・マークされたファイルがある場合はファイル数・容量の表示
	chtab()→put_capa_or_mark()←marking()
**************************************************************/
VOID put_capa_or_mark( tab, itabpage )
DD_TAB	*tab;			/* r/o 対象のタブページ */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	char	*swork[12];


	locate( (TINY)(LIST_X + itabpage*22), (TINY)(LIST_Y - 1) );

	if ( 0 == tab->markedct ) {

		/* マークされたファイルがない場合は総容量の表示 */
		puts( tab->capa );

		/* FREE AREA BARGRAPH */
		vputsgraph( LIST_X + 7 + itabpage*22, LIST_Y - 1, tab->bar );

		
	} else {

		/* マークされたファイルがある場合はファイル数・容量の表示 */
		sputsize( swork, (SLONG *)(tab->markedsize) );
		printf( "mark%3d:%s", tab->markedct, swork );
		
	}
}



/**************************************************************
	ファイルのマーク処理
	keyloop()→marking()
	return値：	マークされたファイルの数
**************************************************************/
int marking( tab, itabpage, index )
DD_TAB	*tab;			/* r+w 対象のタブページ */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
int		index;			/* r/o 対象となるファイル */
{
	SLONG	slwork;
	BOOL	bonpage = FALSE;



	/* indexで指定されているファイルは表示中か？ */
	if( (index / LIST_H) == tab->page ) {
		bonpage = TRUE;
	}


	/* ファイルが「.」「..」である場合はマークの処理はしない */
	if(	(tab->files[ index ].attr & 0x10 ) && 
			( strcmp( ".", tab->files[ index ].name ) == 0 ) 
		||	( strcmp( "..", tab->files[ index ].name ) == 0 ) ) {

		/* 何もしない */

	} else {

		/* マークの表示処理 */
		if( bonpage ) {
			locate( (TINY)(LIST_X + itabpage*2 -1), (TINY)(LIST_Y + index % LIST_H) );
		}

		if( tab->files[ index ].ismarked ) {

			/* すでにマークされている場合はキャンセル */

			tab->files[ index ].ismarked = FALSE;

			if( bonpage ) putchar(' ');	/* 消す */

			/* マークされたファイル数を減らす */
			tab->markedct--;

			/* マークされたファイルサイズを減らす */
			if( 0 == tab->markedct ) {
				/* ゼロクリア */
				itosl( (SLONG *)(tab->markedsize), 0 );
			} else {
				/* 減算 */
				slcpy( &slwork, (SLONG *)(tab->markedsize) );
				slsub( (SLONG *)(tab->markedsize), &slwork, (SLONG *)(tab->files[tab->cursor].size) );
			}

		} else {

			/* マークされていない場合はマークする */
			
			tab->files[ index ].ismarked = TRUE;
			
			if( bonpage ) putchar('*');	/* つける */
			
			/* マークされたファイル数を増やす */
			tab->markedct++;
			
			/* マークされたファイルサイズを増やす */
			slcpy( &slwork, (SLONG *)(tab->markedsize) );
			sladd( (SLONG *)(tab->markedsize), &slwork, (SLONG *)(tab->files[tab->cursor].size) );
		}
	}
	
	/* マークされたファイル数・容量の表示 */
	put_capa_or_mark( tab, itabpage );
	
	
	return tab->markedct;
}



/**************************************************************
	キーイン
	ページ内でカーソルが移動するだけの間はこれをループする
	return値：	TRUE : リスト表示を続ける（ページ切替以下のイベント）
				FALSE: 他の処理が必要となるイベントが発生
	main()→keyloop()
**************************************************************/
/*
keyloop( tabs[itabpage].files, tabs[itabpage].count,
		 &tabs[itabpage].cursor, &tabs[itabpage].page, &tabs[itabpage].marked,
		  &ch, &b_shift, itabpage );
BOOL keyloop( files, ifiles, ilistcur, ilistpage, imarked, keyin, b_shift, itabpage )
FILES	*files;			/* r/o FILES型構造体へのポインタ */
int		ifiles;			/* r/o ファイル数 */
int		*ilistcur;		/* r+w カーソルが何個目のファイルを指しているか */
int		*ilistpage;		/* r+w ページ 最初=0 */
int		*imarked;		/* w/o マークされたファイル数 */
int		*keyin;			/* w/o 入力されたキー 英小文字の場合は大文字化される */
BOOL	*b_shift;		/* w/o SHIFTキーが押されたか */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
*/
BOOL keyloop( tab, itabpage, keyin, keymod )
DD_TAB	*tab;			/* r+w 対象のタブページ */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
char	*keyin;			/* w/o 入力されたキー */
TINY	*keymod;		/* w/o 修飾キーのマトリクス */
{
	int		ioldpage;	/* ページ移動計算用 */
	char	ch;			/* 入力されたキー */
	BOOL	b_csronly = TRUE;	/* ページ内でカーソルが移動するだけ */
	BOOL	b_listloop = TRUE;	/* リスト表示を続ける */
	BOOL	b_shift = FALSE;	/* SHIFTキーが押されたか */


	/* カーソルがページ内を移動するだけの（ページが変化しない）間はループ */
	while ( b_csronly ) {
/*
locate( (TINY)20, (TINY)(LIST_Y + LIST_H + 2) );
printf("* <files:%d ilistpage:%d ilistcur:%d>", ifiles, *ilistpage, *ilistcur );
*/

		/* キー入力待ち */
		locate( (TINY)(LIST_X + itabpage*2 - 1), (TINY)(LIST_Y + tab->cursor % LIST_H) );
		ch = toupper( getch() );

		/* SHIFTキー押下状態を取得 */
		/* （押されているキーのビットが「0」になる） */
		*keymod = *(TINY *)(0xfbeb);
/* bit7       6       5      4        3       2       1       0   */
/* F8/F3	F7/F2	F6/F1	かな	CAPS	GRAPH	CTRL	SHIFT */
		if( (*keymod & KM_SHIFT)==0 ) b_shift = TRUE;
		
		
		/* SPACEキーによるマーク（=マーク＋↓移動） */
		if ( ' ' == ch ) {

			marking( tab, itabpage, tab->cursor );

			ch = KEY_DOWN;	/* カーソルを下に移動する */

			/* マーク中のファイル数・合計容量を表示させるためにブレークさせる */
/* いちいちファイルの再表示とかやっとられっか
			b_csronly = FALSE;
*/
		}

		/* カーソルの上下処理 */
		if ( (KEY_UP == ch) || (KEY_DOWN == ch) ) {

			/* 前のカーソルを消す */
/*				putcursor( (*ilistcur), ' ', ' ' );
*/

			/* カーソル位置のファイル名を通常表示に戻す */
			ldirvm( (LIST_Y + tab->cursor % LIST_H - 1)*40 + (LIST_X + itabpage*2-1),
					tab->files[ tab->cursor ].image,	
					34 );					/* 長さ */
/*
			locate( (TINY)(LIST_X + itabpage*2), (TINY)(LIST_Y + tab->cursor % LIST_H) );
			puts( tab->files[ tab->cursor ].image );
*/
			if( KEY_UP == ch ){

				/* ↑キー イン*/
				if( b_shift ) {

					/* Shift+↑：Page Up*/
					if ( 0 < tab->page ) {
						/* 2ページ目以降だったら前ページの先頭 */
						tab->cursor = (tab->page - 1) * LIST_H;
					} else {
						/* 1ページ目だったら強制的に先頭 */
						tab->cursor = 0;
					}
					
				}else if (0 < tab->cursor ) {
					/* カーソル位置↑ */
					tab->cursor--;
				}

			} else if( KEY_DOWN == ch ) {
				
				/* ↓キー イン*/
				if( b_shift ) {
/*
locate((TINY)1,(TINY)1);
printf("* <files:%d ilistpage:%d ilistcur:%d>", ifiles, *ilistpage, *ilistcur );
*/
					/* Shift+↓：Page Down*/
					if ( (tab->cursor / LIST_H) < ((tab->count - 1) / LIST_H) ) {
						/* 最終ページ以外だったら次ページの先頭 */
						tab->cursor = (tab->page + 1) * LIST_H;
					} else {
						/* 最後尾 */
						tab->cursor = tab->count - 1;
					}

				}else if ( tab->cursor < tab->count - 1 ) {
					/* カーソル位置↓ */
					tab->cursor++;
				}
			}

			/* 新しいカーソルを描く */
/*				putcursor( (*ilistcur), 'g', ' ' );
*/
			/* カーソル位置からページ番号を得る */
			ioldpage = tab->page;
			tab->page = tab->cursor / LIST_H;
			
			if ( ioldpage == tab->page ) {
				/* ページが変わってなければ */
				/* カーソル位置のファイル名を太字表示する */
				vputs_b( LIST_X + itabpage*2, LIST_Y + tab->cursor % LIST_H, tab->files[ tab->cursor ].image );
			} else {
				/* ページが変わったらブレーク */
				b_csronly = FALSE;
			}

		} else {
/*
locate((TINY)1,(TINY)1);
printf("* <ch:%d>", (int)ch );
*/
			
			switch( ch ) {
				
				case KEY_TAB:	/* TABキーによるマーク */
					marking( tab, itabpage, tab->cursor );
					break;
					
				/* 制御を親に戻すキーはスルー */
				case KEY_BS:
				case KEY_RET:
				case KEY_ESC:
				case KEY_ROOT:
				case KEY_RIGHT:
				case KEY_LEFT:
					b_csronly = FALSE;
					b_listloop = FALSE;
					break;
				default:
					b_csronly = FALSE;
					b_listloop = FALSE;
					break;
					
			}
		}
	}
/*
locate((TINY)1,(TINY)1);
printf("* <keyloop():%d>", (int)(*keymod) );
*/
	*keyin = ch;
	
	return b_listloop;
}


