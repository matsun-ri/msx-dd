/**************************************************************
	DD_DRV.c :	�h���C�u���̎擾
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>
#include <msxbios.h>
#include <math.h>
#include "dd.h"


/* dd_sub.c�ɂĒ�` */
extern int bdoscall( c, a, b, de, hl, ix, r );
extern STATUS bdos_trap( c, a, b, de, hl, ix, r, fname );
extern VOID bargraph( sfreebar, ipercent );

/* dd_scrn.c�ɂĒ�` */
extern VOID errmsgbox( icode, fname );

/* dd_drv.c�ɂĒ�` */
extern char *sputsize( str, psl );
extern char *addbackslash( s );
extern char *makeshortpath( path, shortpath );



/**************************************************************
	�h���C�u���̎擾
	return�l�F	OK:���� ERROR:���s
	main()��getdriveinfo()
**************************************************************/
STATUS getdriveinfo( path, svollabel, scapa, sfreebar, itabpage )
char	*path;		/* r/o �p�X */
char	*svollabel;	/* w/o �{�����[�����x����Ԃ� */
char	*scapa;		/* w/o �t�@�C���e�ʂ̕\���C���[�W��Ԃ� */
char	*sfreebar;	/* freearea bargraph image  */
int 	itabpage;	/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int		id;			/* �h���C�u�ԍ� 1=a: */
	int		iret = 0;	/* �߂�l�擾�p */
	STATUS	sret = OK;	/* return�l */
	SLONG				/* MSX-C Library��long�^ */
			sldrvsize	/* �h���C�u�̑��e�� */
			,slrvfree	/* �h���C�u�̋󂫗e�� */
			,slwk1, slwk2, slwk3 /* SLONG�^�v�Z�p���[�N */
			;
	char	sfree[8];
	XREG	reg;			/* BDOS�R�[������̖߂�l�擾�p */
	FIB		fib;

	
	id = (int)( *path - 'A' ) + 1;	/* �J�����g�h���C�u�̐��l�� */

	bdoscall(_ALLOC,	/* �A���P�[�V�������̊l�� */
		0, 0,
		id,				/* E���h���C�u�ԍ� 1=a: */
		0, 0,
		&reg			/* �߂�l���p */
		);	/* �i�߂�l�ɂ�1�N���X�^������̃Z�N�^���������Ă���j */
/*
printf( "* <A:%d>\n", reg.af >> 8 );	/*1�N���X�^������̃Z�N�^��*/
printf( "* <BC:%d>\n", reg.bc );	/*�Z�N�^�T�C�Y(���512)*/
printf( "* <DE:%d>\n", reg.de );	/*�f�B�X�N��̃N���X�^�̑���*/
printf( "* <HL:%d>\n", reg.hl );	/*�f�B�X�N��̖��g�p�N���X�^��*/
*/
	itosl( &slwk1, reg.bc );			/* slwk1���Z�N�^/�N���X�^ */
	itosl( &slwk2, (reg.af >> 8) );		/* slwk2���Z�N�^�T�C�Y */
	slmul( &slwk3, &slwk1, &slwk2 );	/* slwk3��BC*A */
	
	/*�h���C�u�̑��e��*/
	itosl( &slwk1, reg.de );				/* slwk1���N���X�^���� */
	slmul( &sldrvsize, &slwk3, &slwk1 );	/* slwk3��(BC*A)*DE */
	
	/*�h���C�u�̋󂫗e��*/
	itosl( &slwk1, reg.hl );			/* slwk1�����g�p�N���X�^�� */
	slmul( &slrvfree, &slwk3, &slwk1 );	/* slwk3��(BC*A)*HL */

	/* ���e�� */
	sputsize( scapa, &sldrvsize );
	
	/* �󂫗e�� */
	sputsize( sfreebar, &slrvfree );

	/*�󂫗e�ʂ����ŋ��߂�*/
	/*���q*100�����ꂾ�ƃ}�C�i�X�l�ɂȂ�̂�*/
	/*���q��(���ꁀ100)����*/

	itosl( &slwk1, 100 );
	sldiv( &slwk2, &sldrvsize, &slwk1 );	/* ���ꁀ100 */
	sldiv( &slwk3, &slrvfree, &slwk2 );		/* �󂫁�(�e��/100) */
/*
	itosl( &slwk1, 100 );
	slmul( &slwk2, &slrvfree, &slwk1 );		/* slwk2��slrvfree*100 */
	sldiv( &slwk3, &slwk2, &sldrvsize );	/* slwk3��slrvfree*100/sldrvsize */
*/

/*
	locate( (TINY)(LIST_X + itabpage*22), (TINY)(LIST_Y - 1) );
	printf( "%s%3ld%%free", scapawk, &slwk3 );
*/

	/*�g�p�ʖ_�O���t �g�p�ʁ��S�e��-�󂫗e��*/
/* 	bargraph( LIST_X + 7 + itabpage*22, LIST_Y - 1, 100 - *(int *)&slwk3 ); */
	bargraph( sfreebar, 100 - *(int *)&slwk3 );

	/* �{�����[�����x���̎擾 */
	/*                  12345678901n*/
	strcpy( svollabel, "( no name )" );	/* �{�����[�����x������ */
	iret = bdoscall( _FFIRST,
					0, 
					0x08,			/* �{�����[�����x���̎擾 */
					(unsigned)path,	/* �p�X */
					0,
					(unsigned)&fib,	/* �߂�l���p */
					&reg			/* �߂�l���p */
					);
	if ( 0 == iret ) {
		/* �{�����[�����x������ */
		strcpy( svollabel, fib.name );
	} else if ( 0xd7 == iret ) {	
		/* 0xd7=.NOFIL(File not found) */
		/* �{�����[�����x����������Ȃ����� */
		iret = 0; /* �G���[���� */
	} else {
		/* File not found �ȊO�̃G���[�̏ꍇ�̓��b�Z�[�W��\�� */
		errmsgbox( iret, "getdriveinfo()" );
		sret = ERROR;
	}

	/*�h���C�u�e�ʁE�󂫗e�� 2�s�\��*/
/*
		locate( (TINY)LIST_X + 2, (TINY)(LIST_Y - 2) );
		printf( "%sBYTES", scapa );
		locate( (TINY)LIST_X + 2, (TINY)(LIST_Y - 1) );
		printf( "%sFREE", sfree );
*/
/*
		printf( "* <Total:[%10ld]->[%s]>\n", &sldrvsize, sarea );
		printf( "* <Free:[%10ld]->[%s]>\n", &slrvfree, sarea );
*/
	
	return sret;
}



/**************************************************************
	�h���C�u�̕ύX
	return�l�F	OK:���� ERROR:���s
	operation()��chdrv()��tochdrv()
**************************************************************/
STATUS chdrv( idrv, tabs, itabpage )
TINY	idrv;			/* r/o �J�����g�h���C�u 1=a: */
DD_TAB	tabs[];		/* r+w �^�u�y�[�W1����\������\���� */
int		itabpage;	/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	STATUS	sret = ERROR;		/* return�l */
	char	pathwk[MAXPATH];	/* path�쐬�p */
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */


	/* �w��h���C�u�̃J�����g�f�B���N�g���̊l��  */
	/* �i�h���C�u���ƑO��� \ �͊܂܂�Ȃ��j */
	if ( ( sret = bdos_trap(
			_GETCD,
			0, 
			(int)idrv,				/* �h���C�u 1=a: */
			(unsigned)pathwk,	/* �߂�l���p */
			0, 0,
			&reg,				/* �߂�l���p */
			"chdrv().10"
		) ) == OK ) {
		
		/* �G���[�łȂ���� */
		
		/* �h���C�u���ƑO��� \ ��t�� */
		strcpy( tabs[itabpage].path, "*:\\" );
		tabs[itabpage].path[0] = (char)('@'+idrv);		/* �h���C�u�� */
		strcat( tabs[itabpage].path, pathwk );
		/* �Ō��\�𑫂��i�������ɑ����ƃ��[�g�f�B���N�g����\���d������j */
		addbackslash( tabs[itabpage].path );

		/* �p�X������̒Z�k�`���쐬���� */
		makeshortpath( tabs[itabpage].path, tabs[itabpage].shortpath );
		
		/* ����\�����Ƀh���C�u���̎擾���K�v */
		tabs[itabpage].status = TS_CHDRV;
		
	}
	
	return sret;
}



