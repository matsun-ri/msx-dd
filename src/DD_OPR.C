/**************************************************************
	DD_OPR.c : ���[�U���͂ɑ΂���e�폈��
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
#include <math.h>			/* SLONG */
#include "dd.h"

/* dd_sub.c�ɂĒ�` */
extern char *makeshortpath( path, shortpath );

/* DD_EXEC.c�ɂĒ�` */
extern BOOL execbat( filename, image, path, bshift );
extern STATUS setenv( skey, svalue );

/* DD_COPY.c�ɂĒ�` */
extern STATUS tocopy( tabs, itabpage );
extern STATUS tomove( tabs, itabpage );
extern STATUS todelete( tabs, itabpage );

/* dd_scrn.c�ɂĒ�` */
extern VOID msgbox( x, y, stitle, stext );

/* DD_OPR2.c�ɂĒ�` */
extern STATUS chdrv( ch, tabs, itabpage );
extern STATUS tochdrv( tabs, itabpage );
extern int chuprdir( tabs, itabpage );
extern STATUS tomakedir( tabs, itabpage );
extern STATUS torename( tabs, itabpage );

/* DD_KEY.c�ɂĒ�` */
extern int marking( tab, itabpage );

extern VOID puthelp();



/**************************************************************
	���^�[���L�[�Ή�����
	return�l�F ���C�����[�v�̏�Ԃ��w��
	operation()��returnkey()
**************************************************************/
int returnkey( tabs, itabpage, keymod )
DD_TAB	tabs[];			/* r+w �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
TINY	keymod;			/* r/o �C���L�[������� */
{
	int		lsret = LS_LOOP;	/* return�l */
/*
if(keymod){
msgbox(0,0,"returnkey()","shift=ON");
}else{
msgbox(0,0,"returnkey()","shift=OFF");
}
*/
	/* �f�B���N�g���ł���΂��̒��ɓ��� */
	if( tabs[itabpage].files[ tabs[itabpage].cursor ].attr & 0x10 ){
		
		/* �f�B���N�g���� . �ł���ꍇ�͖��� */
		if( strcmp( ".", tabs[itabpage].files[ tabs[itabpage].cursor ].name ) != 0 ) {
			puts("* <CHDIR>\n");
			strcat( tabs[itabpage].path, tabs[itabpage].files[ tabs[itabpage].cursor ].name );
			strcat( tabs[itabpage].path, "\\" );
			
			/* �p�X������̒Z�k�`���쐬���� */
			makeshortpath( tabs[itabpage].path, tabs[itabpage].shortpath );
			
			/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
			tabs[itabpage].status = TS_CHDIR;
		}

	} else {

		/* �g���q�A�����s */
		if( execbat( &tabs[itabpage], &tabs[1-itabpage], keymod ) == OK ) {
/*
		if( execbat(
						tabs[itabpage].files[ tabs[itabpage].cursor ].image,
						tabs[itabpage].path,
						tabs[itabpage].files[ tabs[itabpage].cursor ].name,
						tabs[1-itabpage].path,
						tabs[1-itabpage].files[ tabs[1-itabpage].cursor ].name,
						keymod )
			 == OK ) {
*/			/* �O���R�}���h�̎��s ��̓o�b�`�t�@�C���Ɋۓ��� */
			lsret = LS_EXEC;
		}
	}
	
	return lsret;
}


/**************************************************************
	�}�[�N���ꂽ�t�@�C�����Ȃ��ꍇ�̂�
	�J�[�\���ʒu�̃t�@�C�����}�[�N����悤���݂�
	operation()��markatcursor()
	return�l�F	�}�[�N���ꂽ�t�@�C���̐�
**************************************************************/
int markatcursor( tab, itabpage )
DD_TAB	*tab;			/* r+w �Ώۂ̃^�u�y�[�W */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	/* �}�[�N���ꂽ�t�@�C�����Ȃ��ꍇ */
	if ( 0 == tab->markedct ) {
		/* �J�[�\���ʒu�̃t�@�C�����}�[�N����悤���݂� */
		marking( tab, itabpage, tab->cursor );
	}
	
	return tab->markedct;
}


/**************************************************************
	���[�U�̎w���ɉ������������s��
	return�l�F ���C�����[�v�̏�Ԃ��w��
	main()��operation()
**************************************************************/
int operation( ch, keymod, tabs, itabpage )
char	ch;				/* r/o ���[�U����̓��� �p�����͑啶���œn����� */
TINY	keymod;			/* r/o �C���L�[������� */
DD_TAB	tabs[];			/* r+w �^�u�y�[�W1����\������\���� */
int		*itabpage;		/* r+w ���쒆�̃^�u�y�[�W 0or1 */
{
	int		iret = 0;
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */
	int		lsret = LS_LOOP;	/* return�l */
	char	sdrv[4];			/* ���[�g�f�B���N�g�� */


	/* �t�@�C���u..�v�̑I����BS�L�[�ɓǂݑւ��� */
	if(	( KEY_RET == ch ) &&
		( tabs[*itabpage].files[ tabs[*itabpage].cursor ].attr & 0x10 ) && 
		( strcmp( "..", tabs[*itabpage].files[ tabs[*itabpage].cursor ].name ) == 0 ) ) {
		ch =(char)KEY_BS;
	}

	switch( ch ) {
			
		/* �I�� */
		case KEY_ESC:
		case 'Q':
			lsret = LS_END;		/* �P�Ȃ�I�� */
			break;
		
		/* �^�u�y�[�W�̐؂�ւ� */
		case KEY_RIGHT:			/* ���L�[ �C�� */
		case KEY_LEFT:			/* ���L�[ �C�� */
			*itabpage = 1 - *itabpage;
			lsret = LS_CHGTAB;
			break;

		/* 1-8���h���C�u�ύX */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			chdrv( (TINY)(ch-'0'), tabs, *itabpage ); /* UI�Ȃ� */
			break;
		
		/* �t�@�C���̃R�s�[ */
		case 'C':
			if( 0 < markatcursor( &tabs[ *itabpage ], *itabpage ) ) {
				tocopy( tabs, *itabpage );
			}
			break;
			
		/* �t�@�C���܂��̓T�u�f�B���N�g���̍폜 */
		case 'D':
		case KEY_DEL:
			if( 0 < markatcursor( &tabs[ *itabpage ], *itabpage ) ) {
				todelete( tabs, *itabpage );
			}
			break;
			
		/* �w���v�̕\�� */
		case 'H':
			puthelp();
			break;

		/* �T�u�f�B���N�g���̍쐬 */
		case 'K':
			tomakedir( tabs, *itabpage );
			break;
			
		/* �h���C�u�ύX */
		case 'L':
			tochdrv( tabs, *itabpage ); /* UI���� */
			break;
			
		/* �t�@�C���܂��̓T�u�f�B���N�g���̈ړ� */
		case 'M':
			if( 0 < markatcursor( &tabs[ *itabpage ], *itabpage ) ) {
				tomove( tabs, *itabpage );
			}
			break;
			
		/* �t�@�C���܂��̓T�u�f�B���N�g���̖��O�ύX */
		case 'N':
			torename( tabs, *itabpage );
			break;
		
		/* 0�����݂̃h���C�u�E�p�X�̍ēǍ� */
		case 'R':
		case '0':
			/* ����\�����Ƀh���C�u�̎擾���K�v */
			tabs[*itabpage].status = TS_CHDRV;
			break;

		/* ���[�g�f�B���N�g���ֈړ� */
		case 'T':
		case KEY_ROOT:
			/* d:\ �̎���NULL�ɂ��� */
			*(tabs[*itabpage].path + 3) = NULL;
			
			/* �p�X������̒Z�k�`���쐬���� */
			makeshortpath( tabs[*itabpage].path, tabs[*itabpage].shortpath );
			
			/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
			tabs[*itabpage].status = TS_CHDIR;
			
			break;
		
		/* ��̃f�B���N�g���ֈړ� */
		case 'U':
		case KEY_BS:
			chuprdir( tabs, *itabpage );
			break;
			
		/* ���^�[���L�[�Ή����� */
		case KEY_RET:
/*
if(b_shift){
msgbox(0,0,"operation()","shift=ON");
}else{
msgbox(0,0,"operation()","shift=OFF");
}
*/
			lsret = returnkey( tabs, *itabpage, keymod );
			break;
		
		default:
			break;
	}
	
	return lsret;
}
