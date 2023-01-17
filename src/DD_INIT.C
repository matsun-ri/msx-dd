/**************************************************************
	��������
**************************************************************/
#include <stdio.h>
#include <msxbios.h>
#include <math.h>		/* itosl */
#include "dd.h"

/* dd_scrn.c�ɂĒ�` */
extern VOID setpcg();
extern VOID window_l();

/* dd_sub.c�ɂĒ�` */
extern char *addbackslash( s );
extern char *makeshortpath( path, shortpath );

/**************************************************************
	����������
	�E�\���֘A������
	�E�z��ϐ�������
	�E�R�}���h���C�������̏���
	main()��init()
**************************************************************/
VOID init( argc, argv, tabs )
int		argc;		/* r/o �R�}���h���C������ */
char	*argv[];	/* r/o �R�}���h���C������ */
DD_TAB	tabs[];		/* w/o �������Ώ� */
{
	int		i = 0;
	int 	j = 0;
	char	swork[MAXPATH];


	/* �R���\�[���o�b�t�@�̖����� */
	setbuf( stdout, NULL );

	/* �\���֌W�̏����� */
	*(TINY *)(0xf3ae) = 40;			/* LINL40��WIDTH */
	screen( (TINY)0 );				/* screen 0:width 40�ɐݒ� */
	setpcg();						/* �L�����N�^�p�^�[����` */
/*
	window_l();						/* ��ʘg������ */
*/

	/* �^�u�y�[�W�\���̏����� */
	for ( i=0; i<TAB_MAX; i++ ){
		tabs[i].status = TS_CHDRV;	/* ����\�����Ɏ擾������ */
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

	/* ����������΃p�X�Ƃ��Ĉ��� */
	/* �i�O��̊O���R�}���h���s���ɁA���̎��̃p�X�� */
	/*   �����Ƃ��ēn���悤�ɂ��Ă���j */
	if ( 2 < argc ){
		/* ������2�����^�u�E�E�^�u�ɐU�蕪�� */
		strcpy( tabs[ 0 ].path, argv[1] );
		strcpy( tabs[ 1 ].path, argv[2] );
	} else if ( 1 < argc ){
		/* ������1�����^�u�͈����E�E�^�u�̓J�����g�f�B���N�g�� */
		strcpy( tabs[ 0 ].path, argv[1] );
		getcwd( tabs[ 1 ].path, sizeof( tabs[ 1 ].path ));
	} else {
		/* �����Ȃ������E�Ƃ��J�����g�f�B���N�g�� */
		getcwd( tabs[ 0 ].path, sizeof( tabs[ 0 ].path ) );
		strcpy( tabs[ 1 ].path, tabs[ 0 ].path );
	}
	

	/* �Ō��\�𑫂� */
	addbackslash( tabs[ 0 ].path );
	addbackslash( tabs[ 1 ].path );

	/* �p�X������̒Z�k�`���쐬���� */
	makeshortpath( tabs[ 0 ].path, tabs[ 0 ].shortpath );
	makeshortpath( tabs[ 1 ].path, tabs[ 1 ].shortpath );
}



