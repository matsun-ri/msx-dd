/**************************************************************
	DD_INPUT.c ��ʕ\���i���̓{�b�N�X�֘A�j
**************************************************************/
#include <stdio.h>
#include <msxbios.h>		/* locate() */
#include "dd.h"			/* math.h���܂� */


/* dd_msg.c�ɂĒ�` */
extern VOID vputsgraph( x, y, str );
extern VOID vputs_b( x, y, str );
extern VOID putbox( x, y, w, h );


/**************************************************************
	�t�@�C�������͗p�{�b�N�X
	X13, Y10����W14, H5�̕\���̈�����
	1�s�ځFX14,   Y11 �^�C�g��	vputs_b( 14, 11, "NEW NAME" );
	2�s�ځFX14+1, Y12 ���͍s
	3�`5�s�ځFX14+1,Y13�`15 �K�C�h
**************************************************************/
VOID inputbox( stitle )
char *stitle;	/* r/o �^�C�g�� �����\������� */
{
	putbox( 13, 10, 16, 7 );			/* �g��\�� */
	vputs_b( 14, 11, 	stitle );
	locate( (TINY)15, (TINY)13 );
	puts( 				"^^^^^^^^ ^^^" );
	vputs_b( 15, 14, 	"[RET] ok" );
	vputs_b( 15, 15, 	"[ESC] cancel" );
}


/**************************************************************
	�t�@�C��������
	return�l�F	OK:���͊m�� ERROR:�L�����Z��
**************************************************************/
STATUS inputline( str )
char *str;	/* r+w	�f�t�H���g�l�E�m��l 16�o�C�g�v���� */
			/* 		�m�莞�̂ݓ��e���ύX����� */
{
	int		ix = 0;			/* �J�[�\���ʒu */
	int		ilen = 0;
	STATUS	sret = ERROR;	/* return�l */
	char	ch;				/* ���͂��ꂽ�L�[ */
	char	sinput[16];
	char	wk1[16], wk2[16], wk3[16];
	char	*dot = NULL;
	BOOL	bloop = TRUE;	/* ���[�v�t���O */
	BOOL	bshaping = FALSE;	/* �e�L�X�g���`�t���O */
	
	strcpy( sinput, str );
	
	ix = strlen( sinput );
/*
60 '
70 F$="hoehoe.c"
80 X=LEN(F$)
100 'loop
110 '
120 LOCATE 15,12
130 PRINT F$;"----+----+----+"
*/
	do {
		/* �\���X�V */
		locate( (TINY)15, (TINY)12 );
		puts( "-------- ---" );
		locate( (TINY)15, (TINY)12 );
		puts( sinput ); 
/*
140 LOCATE 15+X
150 A=ASC(INPUT$(1))
*/
		/* �L�[���� */
		locate( (TINY)(15+ix), (TINY)12 );
		ch = toupper( getch() );
		
		
		/* ���͂ɑ΂��鏈��*/
		switch( ch ) {
			
/* 170 IF A=27 THEN END 'ESC */
			case	KEY_ESC:	/* ESC���I�� */
				bloop = FALSE;
				break;

/* 180 IF A=8 AND 0<X THEN X=X-1:F$=LEFT$(F$,X-1)+MID$(F$,X+1) */
			case	KEY_BS:		/* BS */
				if ( 0 < ix ) {
					strcpy( wk2, (char *)(sinput+ix) );
					ix--;
					strcpy( wk1, sinput );
					wk1[ix] = '\0';
					strcpy( sinput, wk1 );
					strcat( sinput, wk2 );
				}
				break;

/* 190 IF A=127 THEN F$=LEFT$(F$,X)+MID$(F$,X+2) */
			case	KEY_DEL:		/* DEL */
				strcpy( wk1, sinput );
				wk1[ix] = '\0';
				strcpy( wk2, (char *)(sinput+ix+1) );
				strcpy( sinput, wk1 );
				strcat( sinput, wk2 );
				break;
				
/* 195 IF A=13  THEN 400 'seikei */
			case	KEY_RET:		/* RET�����` */
			
/*
400 'seikei
410 L=LEN(F$)
*/
				bshaping = FALSE;	/* �e�L�X�g���`�t���O */
				ilen = strlen( sinput );
				
/*
419 ' .ga nakattara . wo tuika
420 IF 8<L AND 0=INSTR(F$,".") THEN F$=LEFT$(F$,8)+"."+MID$(F$,9)
*/
				/* 9�����ȏ゠��̂�.��������Ȃ��ꍇ��.��ǉ� */
				if( 8 < ilen ) {
					if ( strchr( sinput, '.' ) == NULL ) {
						strcpy( wk1, sinput );
						strcpy( wk2, sinput );
						wk1[8] = '.';
						wk1[9] = '\0';
						strcpy( sinput, wk1 );
						strcat( sinput, (char *)(wk2 + 8) );
						
						bshaping = TRUE;
					}
				}
/*
430 M=INSTR(F$,".")
*/
				dot = strchr( sinput, '.' );
/*
439 ' .ga attara . noato3moji ikouwo kirisute
440 IF 0<M THEN F$=LEFT$(F$,M+3)
*/
				if ( NULL != dot ) {
/*
         1111
1234567890123

0123456789012

abcdefghijkl
abc.def
*/
					/* .������ꍇ�́A.�̌��4�����ڈȍ~��؂�̂� */
					ilen = (int)(dot - sinput) + 4;
					if( ilen < strlen(sinput) ) {

						if ( 12 < ilen ) ilen = 12;
						*(char *)(sinput + ilen) = '\0';

						bshaping = TRUE;
					}

/*
450 IF 9<M THEN F$=LEFT$(F$,8)+MID$(F$,M)
*/
					/* .��9�����ڈȍ~�ɂ���ꍇ�́A.�̑O9�����ڈȍ~��؂�̂� */
					if ( 8 < (int)(dot - sinput) ) {
						strcpy( wk1, sinput );
						wk1[8] = '\0';
						strcpy( sinput, wk1 );
						strcat( sinput, dot );
						
						bshaping = TRUE;
					}
				}
				
				/* ���`���s���Ă��Ȃ���Ίm��Ƃ��� */
				if ( FALSE == bshaping ){
					bloop = FALSE;
					sret = OK;
					strcpy( str, sinput );
				}
				
				break;
/*
230 '�-�� ��޳
240 IF A=&H1D THEN X=X-1
250 IF A=&H1C THEN X=X+1
*/
			case KEY_RIGHT:			/* ���L�[ �C�� */
				ix++;
				break;
				
			case KEY_LEFT:			/* ���L�[ �C�� */
				ix--;
				break;

			case ':':				/* �t�@�C�����Ƃ��Ė����ȕ��� */
			case ';':
			case ',':
			case '=':
			case '+':
			case '\\':
			case '<':
			case '>':
			case '|':
			case '/':
			case '"':
			case '[':
			case ']':
			case 0xff:
				/* �������Ȃ� */
				break;
				
/*dd
210 '̧��Ҳ� ¶�Ų Ӽ�
220 ':;.,=+\<>|/"[]
200 '0~20H,7fH,FFh
280 '�ݹ�ŲӼަ ʼ޸
290 IF (A<=&H20) OR (&H5F<A) THEN 110
*/
			default:
				/* �R���g���[���R�[�h�Ƌ󔒂͖��� */
				if ( ch <= ' ' ) break;
/*
300 ' �� . �� ̸���Ų �ޱ� �� . � ���
310 IF (A=&H2E) AND (0<INSTR(F$,".")) THEN 110
*/
				/* �t�@�C�����Ɋ���.������ꍇ��.�𖳎����� */
				if ( ( '.' == ch ) && ( strchr( sinput, '.' ) != NULL ) ) {
					 break;
				}
/*
320 '��ƭ�
330 F$=LEFT$(F$,X)+CHR$(A)+MID$(F$,X+1)
340 F$=LEFT$(F$,12)
350 X=X+1
*/
				/* ���͂���������}�� */
				strcpy( wk1, sinput );
				wk1[ix] = '\0';
				wk2[0]  = ch;
				wk2[1]  = '\0';
				strcpy( wk3, (char *)(sinput+ix) );
				strcpy( sinput, wk1 );
				strcat( sinput, wk2 );
				strcat( sinput, wk3 );
				sinput[12] = '\0';
				ix++;
		}
		
/*
260 IF X<0 THEN X=0
270 IF LEN(F$)<X THEN X=LEN(F$)

360 IF LEN(F$)<X THEN X=LEN(F$)
*/
		ilen = strlen( sinput );
		if( ilen < ix ) ix = ilen;
		if( ix < 0 ) ix = 0;
/*
370 GOTO 110
*/

	} while ( TRUE == bloop );
	
	
	return sret;
}


/**************************************************************
	�h���C�u�����͗p�{�b�N�X
**************************************************************/
VOID drvbox( stitle )
char *stitle;	/* r/o �^�C�g�� �����\������� */
{
	putbox( 12, 10, 18, 6 );			/* �g��\�� */
	vputs_b( 13, 11, 	stitle );
	locate( (TINY)14, (TINY)12 );
	puts( 				"to:_" );
	vputs_b( 14, 13, 	"[A]-[H] drive" );
	vputs_b( 14, 14, 	"[ESC]   cancel" );
}

