/**************************************************************
	DD.com : Dual-screen filer for MSX-DOS2
**************************************************************/
#include <stdio.h>
/*#include <bdosfunc.h>*/		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
#include <math.h>			/* SLONG */
#include "dd.h"

/* DD_INIT.c�ɂĒ�` */
extern VOID init( argc, argv, tab );

/* dd_sub.c�ɂĒ�` */
extern int getfiles( path, files );

/* dd_drv.c�ɂĒ�` */
extern char *sputsize( str, psl );
extern STATUS getdriveinfo( path, svollabel, scapa, sfreebar, itabpage );

/* dd_file.c�ɂĒ�` */
extern VOID filelist( files, ifiles, ilistcur, ilistpage, itabpage );
extern BOOL keyloop( files, ifiles, ilistcur, ilistpage, keyin, b_shift, itabpage );

/* dd_scrn.c�ɂĒ�` */
extern VOID msgbox( x, y, stitle, stext );
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );

/* DD_EXEC.c�ɂĒ�` */
extern STATUS setenv( skey, svalue );

/* DD_OPR.c�ɂĒ�` */
extern int chuprdir( tabs, itabpage );
extern int operation( ch, b_shift, tabs, itabpage );

/* DD_KEY.c�ɂĒ�` */
extern BOOL keyloop( tab, itabpage, keyin, b_shift );
extern VOID put_capa_or_mark( tab, itabpage );



/**************************************************************
	�w�肵���^�u�y�[�W�g�`��
	chtab()��tabframe()
**************************************************************/
VOID tabframe( itabpage )
int		itabpage;	/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int i;
	
	vputsgraph( LIST_X - 2 + itabpage*22, LIST_Y - 3, "XWWWWWWWWWWWWWWWWY" );
	vputsgraph( LIST_X - 2 + itabpage*22, LIST_Y - 2, "V@@@@@@@@@@@@@@@@V" );
	
	switch( itabpage ) {
		case 0:	/* ���y�[�W */
			vputsgraph( LIST_X - 2, LIST_Y - 1,
									"V@@@@@@@@@@@@@@@@ZWWWWWWWWWWWWWWWWWWWY@V" );
			break;

		case 1:	/* �E�y�[�W */
			vputsgraph( LIST_X - 2, LIST_Y - 1,
									"V@XWWWWWWWWWWWWWWWWWWW[@@@@@@@@@@@@@@@@V" );
			break;

		default:
			break;
	}
	
	/* ���X�g���� */
	for ( i=3; i<3 + LIST_H + 2; i++ ) {
		vputsgraph( LIST_X - 2 + itabpage*2, LIST_Y - 3 + i, 
									"V@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@V" );
	}

	switch( itabpage ) {
		case 0:	/* ���y�[�W */
			vputsgraph( LIST_X - 2, LIST_Y + LIST_H + 0,
									"ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[@V" );
			vputsgraph( LIST_X - 2, LIST_Y + LIST_H + 1,
									"@@ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[" );
			vputsgraph( LIST_X - 2 + 36, LIST_Y,  "R" );
			vputsgraph( LIST_X - 2 + 36, LIST_Y + LIST_H -1,  "Q" );
			break;

		case 1:	/* �E�y�[�W */
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
	�^�u�y�[�W�̐؂�ւ��Ή�
	�{�����[�����x���E���e�ʁE�J�����g�f�B���N�g���̕\��
	main()��chtab()
**************************************************************/
VOID chtab( tabs, itabpage )
DD_TAB	tabs[];		/* r/o �^�u�y�[�W1����\������\���� */
int		itabpage;	/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	/* �^�u�y�[�W�g�`�� */
	tabframe( itabpage );

	/* �A�N�e�B�u�ȃ^�u�y�[�W�̃{�����[�����x���̕\�� */
	locate( (TINY)(3 + itabpage * 22), (TINY)2 );
	puts( tabs[itabpage].label );

	/* �h���C�u�̑��e�� �܂��� �}�[�N�t�@�C���̕\�� */
	put_capa_or_mark( &tabs[itabpage], itabpage );

	if ( 0 == itabpage ){

		/* ���^�u�y�[�W���A�N�e�B�u�ȏꍇ*/

		/* ���^�u�F�J�����g�f�B���N�g���̕\�� */
		vputsgraph(	LIST_X - 1, LIST_Y + LIST_H, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" );
		vputs_b(	LIST_X - 1, LIST_Y + LIST_H, tabs[itabpage].shortpath );

		/* �E�^�u�F�J�����g�f�B���N�g���\�� */
		locate( (TINY)(LIST_X + 36 - strlen(tabs[1-itabpage].shortpath) ), (TINY)(LIST_Y + LIST_H + 1) );
		puts( tabs[1-itabpage].shortpath );
		
		/* �A�N�e�B�u�łȂ��^�u�͈ړ����Ȃ��̂ł����Ł\�\�\�ŏ����K�v�͂Ȃ��͂� */

	} else {

		/* �E�^�u�y�[�W���A�N�e�B�u�ȏꍇ*/

		/* �E�^�u�F�J�����g�f�B���N�g���\�� */
		vputsgraph(	LIST_X + 1, LIST_Y + LIST_H, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" );
		vputs_b(	LIST_X + 36 - strlen(tabs[itabpage].shortpath), LIST_Y + LIST_H, tabs[itabpage].shortpath );

		/* ���^�u�F�J�����g�f�B���N�g���̕\�� */
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
	int		itabpage = 0;			/* ���쒆�̃^�u�y�[�W 0or1 */
	int		loopstatus = LS_CHGTAB;	/* ���C�����[�v�̏�� */
	char	ch;						/* �����ꂽ�L�[ */
	TINY	keymod = 0;				/* �C���L�[������� */
	DD_TAB	tabs[TAB_MAX];			/* �^�u�y�[�W1����\������\���� */


	/* ���������� */
	init( argc, argv, tabs );
	
	
	/* ���C�����[�v */
	while( loopstatus < LS_BORDER ) {

		/* �h���C�u���̎擾���K�v�Ȃ� */
		if ( TS_CHDRV == tabs[itabpage].status ) {
			
			/* �h���C�u���̎擾 */
			if( OK == getdriveinfo( tabs[itabpage].path, tabs[itabpage].label, tabs[itabpage].capa, tabs[itabpage].bar, itabpage ) ) {

				tabs[itabpage].status = TS_CHDIR; /* ��Ńt�@�C���ꗗ�̎擾 */

			}
		}
		
		/* �t�@�C���ꗗ�̎擾���K�v�Ȃ� */
		if ( TS_CHDIR == tabs[itabpage].status ) {

			/* �w�肳�ꂽ�p�X�̃t�@�C���ꗗ�𓾂� */
			tabs[itabpage].count = getfiles( tabs[itabpage].path, tabs[itabpage].files );
			tabs[itabpage].cursor = 0;	/* �J�[�\�������ڂ̃t�@�C�����w���Ă��邩 */
			tabs[itabpage].page = 0;	/* �y�[�W */
			tabs[itabpage].markedct = 0;					/* �}�[�N���ꂽ�t�@�C���� */
			itosl( (SLONG *)(tabs[itabpage].markedsize), 0 ); /* ���̃t�@�C���T�C�Y */
			tabs[itabpage].status = TS_NOP;
			loopstatus = LS_CHGTAB;	/* ��Ń{�����[�����x���Ƒ��e�ʂ�\�� */

		}

		/* �^�u�y�[�W�̐؂�ւ��Ή� */
		/* 	�{�����[�����x���E���e�ʁE�J�����g�f�B���N�g���̕\�� */
		if ( LS_CHGTAB == loopstatus ) {
			chtab( tabs, itabpage );
			loopstatus = LS_LOOP;
		}
		
		/* �J�[�\���ʒu����y�[�W�ԍ��𓾂� */
		tabs[itabpage].page = tabs[itabpage].cursor / LIST_H;

		/* �t�@�C���ꗗ�̕\�� �璷�����ǑS��R/O�Ȃ̂ŋ����� */
		filelist( tabs[itabpage].files, tabs[itabpage].count, tabs[itabpage].cursor, tabs[itabpage].page, itabpage );

		/* �L�[���͏��� */
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
		/* ���[�U�̎w���ɉ������������s�� */
		loopstatus = operation( ch, keymod, tabs, &itabpage );

	}
	
	/* �P�Ȃ�I���ł���� */
	if( LS_END == loopstatus ) {
		/* �O���v���O�������s�p�o�b�`�t�@�C�����s�p���ϐ����N���A */
		setenv( "DD_EXEC", "" );
		
		/* ���ϐ������� */
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