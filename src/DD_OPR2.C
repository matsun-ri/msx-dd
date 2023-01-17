/**************************************************************
	DD_OPR.c : ���[�U���͂ɑ΂���e�폈��
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
/* #include <math.h>			 SLONG */
#include "dd.h"

/* dd_sub.c�ɂĒ�` */
extern int bdoscall( c, a, b, de, hl, ix, r );
extern STATUS bdos_trap( c, a, b, de, hl, ix, r, fname );
extern char *makeshortpath( path, shortpath );

/* dd_scrn.c�ɂĒ�` */
extern VOID msgbox( x, y, stitle, stext );

/* dd_input.c�ɂĒ�` */
extern VOID drvbox( stitle );
extern VOID inputbox( stitle );
extern STATUS inputline( str );

extern STATUS chdrv( ch, tabs, itabpage );



/**************************************************************
	�h���C�u�̕ύX UI����
	return�l�F	OK:���� ERROR:���s
	operation()��tochdrv()
**************************************************************/
STATUS tochdrv( tabs, itabpage )
DD_TAB	tabs[];		/* r+w �^�u�y�[�W1����\������\���� */
int		itabpage;	/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	char	ch;
	BOOL	bloop = TRUE;
	STATUS	sret = ERROR;
	
	
	/* ���͗p�{�b�N�X�`�� */
	drvbox( "CHANGE DRIVE" );
	
	/* ���݂̃h���C�u��\�� */
	locate( (TINY)17, (TINY)12 );
	putchar( tabs[itabpage].path[0] );
	locate( (TINY)17, (TINY)12 );

	do {
		ch = toupper( getch() );
		
		switch( ch ) {
			
			case 'A':	/* �h���C�u�̕ύX */
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
				sret = chdrv( (TINY)(ch - '@'), tabs, itabpage );
				bloop = FALSE;
				break;
				
			case KEY_ESC:	/* �L�����Z�� */
				bloop = FALSE;
				break;
			
		}
		
	} while ( bloop );
	
	
	return sret;
}



/**************************************************************
	��ʃf�B���N�g���ֈړ�
	return�l�F �G���[�R�[�h
	operation()��chuprdir()
**************************************************************/
int chuprdir( tabs, itabpage )
DD_TAB	tabs[];			/* r+w �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int		iret = 0;
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */
	char	pathwk[MAXPATH];	/* path�쐬�p */

	/* _PARSE���邽�߂ɍŌ��\����� */
	strncpy( pathwk, tabs[itabpage].path, strlen(tabs[itabpage].path) -1 );
	pathwk[ strlen( tabs[itabpage].path ) - 1 ] = NULL;

	/* �p�X���̉��  */
	iret = bdoscall( _PARSE,
			0, 0, 
			(unsigned)pathwk,
			0, 0,
			&reg			/* �߂�l���p */
			);
	if ( reg.hl != reg.de ) {

		/* HL=������̍Ō�̍���(�u�t�@�C�����v����)�̍ŏ��̕��� */
		/* �Ȃ̂ł�����NULL�ɂ���� \ �Ŏ~�܂� */
		*(char *)(reg.hl) = NULL;
		strcpy( tabs[itabpage].path, pathwk );
		
		/* �p�X������̒Z�k�`���쐬���� */
		makeshortpath( tabs[itabpage].path, tabs[itabpage].shortpath );
		
		/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
		tabs[itabpage].status = TS_CHDIR;
/*
	} else {
*/
		/* HL==DE�̏ꍇ */
		/* �����񂪁u\�v�����ŏI����Ă�����A���邢�̓k��(�h���C�u���͕ʂƂ���) */
		/* �������Ȃ� */
	}
	
	return iret;
}


/**************************************************************
	�T�u�f�B���N�g���̍쐬
	return�l�F	OK:���� ERROR:���s
	operation()��tomakedir()
**************************************************************/
STATUS tomakedir( tabs, itabpage )
DD_TAB	tabs[];			/* r/o �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	char	pathwk[MAXPATH];	/* path�쐬�p */
	char	sname[MAXPATH];		/* �쐬����f�B���N�g���� */
	STATUS	sret = ERROR;		/* return�l */
	
	
	strcpy( sname, "" );
	
	/* ���͗p�{�b�N�X�`�� */
	inputbox( "MAKE DIR" );
	
	/* �e�L�X�g�{�b�N�X */
	if( OK == inputline( sname ) ) {
	
		/* ���s�ׂ� */
/*
		if( isspace( sname[ strlen( sname )-1 ] )) {
			sname[ strlen( sname )-1 ] = '\0';
		}
*/		
		/* 1�����ȏ���͂���Ă���Ύ��s */
		if( 0 < strlen( sname )) {
			strcpy( pathwk, tabs[ itabpage ].path );
			strcat( pathwk, sname );
			msgbox( 0, 0, "MKDIR", pathwk );
			
			/* �T�u�f�B���N�g���̍쐬 */
			sret = mkdir( pathwk ); 
			
			if ( OK == sret ) {
				/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
				tabs[ itabpage ].status = TS_CHDIR;
			} else {
				msgbox( 0, 0, "MAKE DIR", "error cuured." );
			}
		}
	}
	
	return sret;
}


/**************************************************************
	�t�@�C���܂��̓T�u�f�B���N�g���̖��O�ύX
	return�l�F	OK:���� ERROR:���s
	operation()��torename()
**************************************************************/
STATUS torename( tabs, itabpage )
DD_TAB	tabs[];			/* r/o �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	char	pathwk[MAXPATH];	/* path�쐬�p */
	char	sname[MAXPATH];		/* �쐬����f�B���N�g���� */
	STATUS	sret = ERROR;		/* return�l */
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */
	
	
	/* �f�t�H���g�l */
	strcpy( sname, tabs[itabpage].files[ tabs[itabpage].cursor ].name );

	/* ���͗p�{�b�N�X�`�� */
	inputbox( "RENAME" );
	
	/* �e�L�X�g�{�b�N�X */
	if( OK == inputline( sname ) ) {

		/* 1�����ȏ���͂���Ă���Ύ��s */
		if( 0 < strlen( sname )) {
			strcpy( pathwk, tabs[ itabpage ].path );
			strcat( pathwk, tabs[itabpage].files[ tabs[itabpage].cursor ].name );
			
			/* ���O�̕ύX �G���[���E������DOS�R�[���őΉ� */
			if ( ( sret = bdos_trap(
					_RENAME,
					0, 0,
					(unsigned)pathwk,	/* ���t�@�C���� */
					(unsigned)sname,	/* �V�t�@�C���� */
					0,
					&reg,				/* �߂�l���p */
					"chdrv().10"
				) ) == OK ) {
				/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
				tabs[ itabpage ].status = TS_CHDIR;
			}
	/*		sret = rename( pathwk, sname );
			if ( OK == sret ) {
				/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
				tabs[ itabpage ].status = TS_CHDIR;
			} else {
				msgbox( 0, 0, "RENAME", "error cuured." );
			}
	*/
		}
	}
	
	return sret;
}

