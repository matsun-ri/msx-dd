/**************************************************************
	DD_SCRN.c ��ʕ\��
**************************************************************/
/*
�EPCG��������
	�EVOID setpcg()�ɂč쐬
	�Estatic TINY ptn[]���p�^�[����`
	�E�r���iMSX�̃O���t�B�b�N�L�����N�^�j�� �r��PCG
	�E0xe0..0xfa�i�u���v�`�u��v�j���啶���̃{�[���h
	�E�i����E�b��Ług�v�j���J�[�\���u���v
	�E�i����E�b��Łu*�v�j���`�F�b�N�}�[�N
�E����ȕ\������
	�Evputsgraph( x, y, str )�F�r���󎚗p
		�Estr�̕�����-40h���炵�Ē���VRAM�ɏ�������
	�Evputs_b( x, y, str )�F�p���󎚗p
		�Estr�̕�����啶������+A0h���炵�Ē���VRAM�ɏ�������
�E
*/
#include <stdio.h>
#include <glib.h>		/* vpeek, vpoke */
#include <bdosfunc.h>	/* XREG */
/* #include <msxbios.h>		locate() */
#include "dd.h"			/* math.h���܂� */

extern VOID vputsgraph( x, y, str );
extern VOID vputs_b( x, y, str );
extern VOID putbox( x, y, w, h );


/* �O���p�^�[��		���������Ώۂ̕����R�[�h, �r�b�g�p�^�[���~8 */
static TINY ptn[]={
					/* ��������c�_�O���t */
					0x11,	0x84,0x78,0x0,0x0,0xFC,0x78,0x30,0x0,	/*��Q*/
					0x12,	0x0,0x30,0x78,0xFC,0x0,0x0,0x78,0x84,	/*��R*/
					0x15,	0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,/*��U*/
					0x1c,	0xB4,0xB4,0xB4,0xB4,0xB4,0xB4,0xB4,0xB4,/*��\*/
					/* �������牡�_�O���t */
					0x0a,	0xFF,0x0,0x0,0x0,0x0,0x0,0xFF,0x0,		/*0J*/
					0x0b,	0xFF,0x0,0x80,0x80,0x80,0x0,0xFF,0x0,	/*1K*/
					0x0c,	0xFF,0x0,0xC0,0xC0,0xC0,0x0,0xFF,0x0,	/*2L*/
					0x0d,	0xFF,0x0,0xE0,0xE0,0xE0,0x0,0xFF,0x0,	/*3M*/
					0x0e,	0xFF,0x0,0xF0,0xF0,0xF0,0x0,0xFF,0x0,	/*4N*/
					0x0f,	0xFF,0x0,0xF8,0xF8,0xF8,0x0,0xFF,0x0,	/*5O*/
					0x10,	0xFF,0x0,0xFC,0xFC,0xFC,0x0,0xFF,0x0,	/*6P*/
					0x13,	0x80,0x40,0x40,0x40,0x40,0x40,0x80,0x0,	/*)S:��*/
					0x14,	0x4,0x08,0x08,0x08,0x08,0x08,0x4,0x0,	/*(T:��*/
					/* ��������r�� �z�u�̓O���t�B�b�N�L�����N�^�̌r���Ɠ���*/
					0x16, 	0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, /*��V*/
					0x17, 	0x0,0x0,0x0,0xFF,0xFF,0x0,0x0,0x0,		/*��W */
					0x18, 	0x0,0x0,0x0,0x1F,0x3F,0x30,0x30,0x30,	/*��X*/
					0x19, 	0x0,0x0,0x0,0xE0,0xF0,0x30,0x30,0x30,	/*��Y*/
					0x1a, 	0x30,0x30,0x30,0x3F,0x1F,0x0,0x0,0x0,	/*��Z*/
					0x1b, 	0x30,0x30,0x30,0xF0,0xE0,0x0,0x0,0x0,	/*��[*/
					/* ��������L�� */
/*
					'g', 	0x40,0x60,0x70,0x78,0x70,0x60,0x40,0x0,	/*��g */
*/
					'*', 	0x0,0x0,0x4,0xC,0xd8,0x70,0x20,0x0 	/*�`�F�b�N'*'*/
				};

/* ��ʃf�U�C�� ���ۂɂ͕����R�[�h��-40h���ď������� */
/*                                1         2         3         4
                         1234567890123456789012345678901234567890 */
static char scr[7][42] =
					{	"XWWWWWWWWWWWWWWWWY@@@@XWWWWWWWWWWWWWWWWY",
						"V@@@@@@@@@@@@@@@@V@@@@V@@@@@@@@@@@@@@@@V",
						"V@@@@@@@@@@@@@@@@ZWWWWWWWWWWWWWWWWWWWY@V",
						"V@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@UV@V",
						"ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[@V",
						"@@ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[",
						" [Help] [     ] [     ] [     ] [Exit]" };

/**************************************************************
	�L�����N�^�p�^�[����`
**************************************************************/
VOID setpcg()
{
	int i, j, a;
	
	/* �r���E�J�[�\���E�I�� �}�[�N */
	for ( i = 0; i < 20; i++ ) {
		for ( j = 0; j < 8; j++ ) {
			vpoke(	0x800 + (int)(ptn[ i*9 ]) * 8 + j, ptn[ i*9 + 1+j ] );
		}
	}
	/* �啶���̃{�[���h�쐬 */
	/* '@'..'Z'��0xe0..0xfa�i�u���v�`�u��v�j*/
	for ( i = 0; i < 27*8; i++ ) {
		a = vpeek( 0x800 + ((int)'@')*8 + i );
		vpoke( 0x800 + (0xe0)*8 + i, (TINY)(a | (a>>1)) );
	}
}


/**************************************************************
	��ʕ\��
**************************************************************/
VOID window_l()
{
	int i;

	cls();
	
	/* �^�u */
	for ( i=0; i<3; i++ ) {
		vputsgraph( LIST_X - 2, LIST_Y - 3 + i, scr[i] );
	}
	/* ���X�g���� */
	for ( i=3; i<3 + LIST_H + 2; i++ ) {
		vputsgraph( LIST_X - 2, LIST_Y - 3 + i, scr[3] );
	}
	/* �^�u���� */
	for ( i=4; i<6; i++ ) {
		vputsgraph( LIST_X - 2, LIST_Y + LIST_H - 4 + i, scr[i] );
	}
	/* �ŉ��s */
	vputs_b( LIST_X - 2, LIST_Y + LIST_H + 2, scr[6] ); 
/*
	for ( i=0; i<3; i++ ) {
		locate( (TINY)(LIST_X - 2), (TINY)(LIST_Y - 3 + i) );
		puts( scr[i] );
	}
	for ( i=3; i<3 + LIST_H; i++ ) {
		locate( (TINY)(LIST_X - 2), (TINY)(LIST_Y - 3 + i) );
		puts( scr[3] );
	}
	for ( i=4; i<7; i++ ) {
		locate( (TINY)(LIST_X - 2), (TINY)(LIST_Y - 4 + LIST_H + i) );
		puts( scr[i] );
	}
*/
/* GRPH���� �\���e�X�g */
/*
for ( i=0; i<32; i++ ) {
	vpoke( i, (TINY)i );
}
*/
}



/**************************************************************
	�_�O���t�̕\��
**************************************************************/
/*
	�L�����N�^�R�[�h
		0x0d  0x0f..0x15  0x0e
		   [ ����0�`6�̖_ ]
		   M     O�`U     N ���w�蕶���i-0x40���Ďg����j
*/
VOID bargraph( sfreebar, ipercent )
char *sfreebar;
int	ipercent;	/* r/o �_�O���t�̒��� �p�[�Z���g */
{
	char sbar [9];
	int i, i36, ichar, imod;

	/*			   123456789 */
	strcpy( sbar, "TJJJJJJS" );

	/* 36���̂����H */
	i36 = 36 * ipercent / 100;
	/* ���������H */
	ichar = i36 / 6;
	/* �]�� */
	imod = i36 % 6;
	
	/* 6�����Ōv�Z(�S����6�����Ȃ̂�) */
	for ( i = 0; i < ichar; i++ ) {
		sbar[ 1 + i ] = 'P';
	}
	/* 100%�����ł���Η]���1�����ŕ\������ */
	if ( ichar < 6 ){
		sbar[ 1 + ichar ] = (TINY)('J' + imod );
	}
	
	strcpy( sfreebar, sbar );

	/* �����ɖ_�O���t��\�����鏈�� */
/*	vputsgraph( x, y, sbar ); */
/*
locate((TINY)1, (TINY)1);
printf("ipercent:%d iall:%d imod:%d", ipercent, ichar, imod );
*/
}


/**************************************************************
	�ʂ̃w���v�̕\��
**************************************************************/
VOID putonehelp( ix, iy, ch )
int		ix;		/* r/o �\���ʒu X */
int		iy;		/* r/o �\���ʒu Y */
char	ch;		/* r/o �\���Ώۂ̃L�[ */
{
	char stitle[16];
	char stext[16];
	
	/* ���o�� */
	strcpy( stitle, "*" );
	stitle[0] = ch;
	
	/* ���e */
	strcpy( stext, "" );
	
	/* ������ */
	switch( ch ) {
		case 'C':
			strcat( stext, " copy" );
			break;
		case 'D':
			strcat( stitle, "/DEL" );
			strcat( stext, " delete" );
			break;
		case 'H':
			strcat( stext, " this help" );
			break;
		case 'K':
			strcat( stext, " make dir." );
			break;
		case 'L':
			strcat( stext, " change drive" );
			break;
		case 'M':
			strcat( stext, " move" );
			break;
		case 'N':
			strcat( stext, " rename" );
			break;
		case 'Q':
			strcat( stitle, "/ESC" );
			strcat( stext, " exit" );
			break;
		case 'R':
			strcat( stitle, "/0" );
			strcat( stext, " reload" );
			break;
		case 'T':
			strcat( stitle, "/\\" );
			strcat( stext, " root dir." );
			break;
		case 'U':
			strcat( stitle, "/BS" );
			strcat( stext, " upper dir" );
			break;
		default:
			break;
/*
 1-8	drv
*/
	}
	
	/* ���o�� */
	vputs_b( ix, iy, stitle );
	/* ���e */
	vputs_b( ix + strlen( stitle ), iy, stext );
	
}


/**************************************************************
	�w���v��ʂ̕\��
**************************************************************/
VOID puthelp()
{
	int		i = 0;
	char	ch;
/*
| ABCDEFGH.TXT123,456 00-00-00 00:00H|H|
123456789012345678901234567890
    | A              B             |
      12345678901234 12345678901234
*/

	putbox( 5, 6, 32, 15 );			/* �g��\�� */
	
	for( i=0; i<13; i++ ) {
		
		/* ���e��\�� */
		putonehelp( 6, 7+i, (char)('A' + i) );
		putonehelp( 22, 7+i, (char)('A' + 13 + i) );
		
	}
	
	ch = getch();
}


