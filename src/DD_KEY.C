/**************************************************************
	�L�[�C������
**************************************************************/
#include <stdio.h>
/* #include <bdosfunc.h> */
#include <glib.h>		/* ldirvm */
/* #include <msxbios.h>	 locate */
#include <math.h>			/* SLONG */
#include "dd.h"

/* dd_drv.c�ɂĒ�` */
extern char *sputsize( str, psl );

/* dd_scrn.c�ɂĒ�` */
extern VOID msgbox( x, y, stitle, stext );
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );



/**************************************************************
	�h���C�u�̑��e�� �܂��� �}�[�N�t�@�C���̕\��
	�E�}�[�N���ꂽ�t�@�C�����Ȃ��ꍇ�͑��e�ʂ̕\��
	�E�}�[�N���ꂽ�t�@�C��������ꍇ�̓t�@�C�����E�e�ʂ̕\��
	chtab()��put_capa_or_mark()��marking()
**************************************************************/
VOID put_capa_or_mark( tab, itabpage )
DD_TAB	*tab;			/* r/o �Ώۂ̃^�u�y�[�W */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	char	*swork[12];


	locate( (TINY)(LIST_X + itabpage*22), (TINY)(LIST_Y - 1) );

	if ( 0 == tab->markedct ) {

		/* �}�[�N���ꂽ�t�@�C�����Ȃ��ꍇ�͑��e�ʂ̕\�� */
		puts( tab->capa );

		/* FREE AREA BARGRAPH */
		vputsgraph( LIST_X + 7 + itabpage*22, LIST_Y - 1, tab->bar );

		
	} else {

		/* �}�[�N���ꂽ�t�@�C��������ꍇ�̓t�@�C�����E�e�ʂ̕\�� */
		sputsize( swork, (SLONG *)(tab->markedsize) );
		printf( "mark%3d:%s", tab->markedct, swork );
		
	}
}



/**************************************************************
	�t�@�C���̃}�[�N����
	keyloop()��marking()
	return�l�F	�}�[�N���ꂽ�t�@�C���̐�
**************************************************************/
int marking( tab, itabpage, index )
DD_TAB	*tab;			/* r+w �Ώۂ̃^�u�y�[�W */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
int		index;			/* r/o �ΏۂƂȂ�t�@�C�� */
{
	SLONG	slwork;
	BOOL	bonpage = FALSE;



	/* index�Ŏw�肳��Ă���t�@�C���͕\�������H */
	if( (index / LIST_H) == tab->page ) {
		bonpage = TRUE;
	}


	/* �t�@�C�����u.�v�u..�v�ł���ꍇ�̓}�[�N�̏����͂��Ȃ� */
	if(	(tab->files[ index ].attr & 0x10 ) && 
			( strcmp( ".", tab->files[ index ].name ) == 0 ) 
		||	( strcmp( "..", tab->files[ index ].name ) == 0 ) ) {

		/* �������Ȃ� */

	} else {

		/* �}�[�N�̕\������ */
		if( bonpage ) {
			locate( (TINY)(LIST_X + itabpage*2 -1), (TINY)(LIST_Y + index % LIST_H) );
		}

		if( tab->files[ index ].ismarked ) {

			/* ���łɃ}�[�N����Ă���ꍇ�̓L�����Z�� */

			tab->files[ index ].ismarked = FALSE;

			if( bonpage ) putchar(' ');	/* ���� */

			/* �}�[�N���ꂽ�t�@�C���������炷 */
			tab->markedct--;

			/* �}�[�N���ꂽ�t�@�C���T�C�Y�����炷 */
			if( 0 == tab->markedct ) {
				/* �[���N���A */
				itosl( (SLONG *)(tab->markedsize), 0 );
			} else {
				/* ���Z */
				slcpy( &slwork, (SLONG *)(tab->markedsize) );
				slsub( (SLONG *)(tab->markedsize), &slwork, (SLONG *)(tab->files[tab->cursor].size) );
			}

		} else {

			/* �}�[�N����Ă��Ȃ��ꍇ�̓}�[�N���� */
			
			tab->files[ index ].ismarked = TRUE;
			
			if( bonpage ) putchar('*');	/* ���� */
			
			/* �}�[�N���ꂽ�t�@�C�����𑝂₷ */
			tab->markedct++;
			
			/* �}�[�N���ꂽ�t�@�C���T�C�Y�𑝂₷ */
			slcpy( &slwork, (SLONG *)(tab->markedsize) );
			sladd( (SLONG *)(tab->markedsize), &slwork, (SLONG *)(tab->files[tab->cursor].size) );
		}
	}
	
	/* �}�[�N���ꂽ�t�@�C�����E�e�ʂ̕\�� */
	put_capa_or_mark( tab, itabpage );
	
	
	return tab->markedct;
}



/**************************************************************
	�L�[�C��
	�y�[�W���ŃJ�[�\�����ړ����邾���̊Ԃ͂�������[�v����
	return�l�F	TRUE : ���X�g�\���𑱂���i�y�[�W�ؑֈȉ��̃C�x���g�j
				FALSE: ���̏������K�v�ƂȂ�C�x���g������
	main()��keyloop()
**************************************************************/
/*
keyloop( tabs[itabpage].files, tabs[itabpage].count,
		 &tabs[itabpage].cursor, &tabs[itabpage].page, &tabs[itabpage].marked,
		  &ch, &b_shift, itabpage );
BOOL keyloop( files, ifiles, ilistcur, ilistpage, imarked, keyin, b_shift, itabpage )
FILES	*files;			/* r/o FILES�^�\���̂ւ̃|�C���^ */
int		ifiles;			/* r/o �t�@�C���� */
int		*ilistcur;		/* r+w �J�[�\�������ڂ̃t�@�C�����w���Ă��邩 */
int		*ilistpage;		/* r+w �y�[�W �ŏ�=0 */
int		*imarked;		/* w/o �}�[�N���ꂽ�t�@�C���� */
int		*keyin;			/* w/o ���͂��ꂽ�L�[ �p�������̏ꍇ�͑啶��������� */
BOOL	*b_shift;		/* w/o SHIFT�L�[�������ꂽ�� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
*/
BOOL keyloop( tab, itabpage, keyin, keymod )
DD_TAB	*tab;			/* r+w �Ώۂ̃^�u�y�[�W */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
char	*keyin;			/* w/o ���͂��ꂽ�L�[ */
TINY	*keymod;		/* w/o �C���L�[�̃}�g���N�X */
{
	int		ioldpage;	/* �y�[�W�ړ��v�Z�p */
	char	ch;			/* ���͂��ꂽ�L�[ */
	BOOL	b_csronly = TRUE;	/* �y�[�W���ŃJ�[�\�����ړ����邾�� */
	BOOL	b_listloop = TRUE;	/* ���X�g�\���𑱂��� */
	BOOL	b_shift = FALSE;	/* SHIFT�L�[�������ꂽ�� */


	/* �J�[�\�����y�[�W�����ړ����邾���́i�y�[�W���ω����Ȃ��j�Ԃ̓��[�v */
	while ( b_csronly ) {
/*
locate( (TINY)20, (TINY)(LIST_Y + LIST_H + 2) );
printf("* <files:%d ilistpage:%d ilistcur:%d>", ifiles, *ilistpage, *ilistcur );
*/

		/* �L�[���͑҂� */
		locate( (TINY)(LIST_X + itabpage*2 - 1), (TINY)(LIST_Y + tab->cursor % LIST_H) );
		ch = toupper( getch() );

		/* SHIFT�L�[������Ԃ��擾 */
		/* �i������Ă���L�[�̃r�b�g���u0�v�ɂȂ�j */
		*keymod = *(TINY *)(0xfbeb);
/* bit7       6       5      4        3       2       1       0   */
/* F8/F3	F7/F2	F6/F1	����	CAPS	GRAPH	CTRL	SHIFT */
		if( (*keymod & KM_SHIFT)==0 ) b_shift = TRUE;
		
		
		/* SPACE�L�[�ɂ��}�[�N�i=�}�[�N�{���ړ��j */
		if ( ' ' == ch ) {

			marking( tab, itabpage, tab->cursor );

			ch = KEY_DOWN;	/* �J�[�\�������Ɉړ����� */

			/* �}�[�N���̃t�@�C�����E���v�e�ʂ�\�������邽�߂Ƀu���[�N������ */
/* ���������t�@�C���̍ĕ\���Ƃ�����Ƃ�����
			b_csronly = FALSE;
*/
		}

		/* �J�[�\���̏㉺���� */
		if ( (KEY_UP == ch) || (KEY_DOWN == ch) ) {

			/* �O�̃J�[�\�������� */
/*				putcursor( (*ilistcur), ' ', ' ' );
*/

			/* �J�[�\���ʒu�̃t�@�C������ʏ�\���ɖ߂� */
			ldirvm( (LIST_Y + tab->cursor % LIST_H - 1)*40 + (LIST_X + itabpage*2-1),
					tab->files[ tab->cursor ].image,	
					34 );					/* ���� */
/*
			locate( (TINY)(LIST_X + itabpage*2), (TINY)(LIST_Y + tab->cursor % LIST_H) );
			puts( tab->files[ tab->cursor ].image );
*/
			if( KEY_UP == ch ){

				/* ���L�[ �C��*/
				if( b_shift ) {

					/* Shift+���FPage Up*/
					if ( 0 < tab->page ) {
						/* 2�y�[�W�ڈȍ~��������O�y�[�W�̐擪 */
						tab->cursor = (tab->page - 1) * LIST_H;
					} else {
						/* 1�y�[�W�ڂ������狭���I�ɐ擪 */
						tab->cursor = 0;
					}
					
				}else if (0 < tab->cursor ) {
					/* �J�[�\���ʒu�� */
					tab->cursor--;
				}

			} else if( KEY_DOWN == ch ) {
				
				/* ���L�[ �C��*/
				if( b_shift ) {
/*
locate((TINY)1,(TINY)1);
printf("* <files:%d ilistpage:%d ilistcur:%d>", ifiles, *ilistpage, *ilistcur );
*/
					/* Shift+���FPage Down*/
					if ( (tab->cursor / LIST_H) < ((tab->count - 1) / LIST_H) ) {
						/* �ŏI�y�[�W�ȊO�������玟�y�[�W�̐擪 */
						tab->cursor = (tab->page + 1) * LIST_H;
					} else {
						/* �Ō�� */
						tab->cursor = tab->count - 1;
					}

				}else if ( tab->cursor < tab->count - 1 ) {
					/* �J�[�\���ʒu�� */
					tab->cursor++;
				}
			}

			/* �V�����J�[�\����`�� */
/*				putcursor( (*ilistcur), 'g', ' ' );
*/
			/* �J�[�\���ʒu����y�[�W�ԍ��𓾂� */
			ioldpage = tab->page;
			tab->page = tab->cursor / LIST_H;
			
			if ( ioldpage == tab->page ) {
				/* �y�[�W���ς���ĂȂ���� */
				/* �J�[�\���ʒu�̃t�@�C�����𑾎��\������ */
				vputs_b( LIST_X + itabpage*2, LIST_Y + tab->cursor % LIST_H, tab->files[ tab->cursor ].image );
			} else {
				/* �y�[�W���ς������u���[�N */
				b_csronly = FALSE;
			}

		} else {
/*
locate((TINY)1,(TINY)1);
printf("* <ch:%d>", (int)ch );
*/
			
			switch( ch ) {
				
				case KEY_TAB:	/* TAB�L�[�ɂ��}�[�N */
					marking( tab, itabpage, tab->cursor );
					break;
					
				/* �����e�ɖ߂��L�[�̓X���[ */
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