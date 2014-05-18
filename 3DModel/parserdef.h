
#if !defined(__PARSERDEF_H__)
#define __PARSERDEF_H__

/*
	��¥: 2002-11-28 programer: ������
	��¥: 2003-09-19 programer: ������
		- refactoring
*/


#include <string.h>
#define SCRIPT_VERSION		"1.04"		// 2003-09-20


/////////////////////////////////////////////////////////////////////////
// �Ǻ� ����

// ��ɾ� ����Ʈ
static char *g_szComlist = "{}";

// �ּ� �Ǻ�
static char *g_szComment = "/";

// ���� ���� �Ǻ�, ����, tab, carriage return, line feed�� �Ǻ�
static char *g_szWhite = " \t\n\r";

// ��ġ �ڷ� �Ǻ�
static char *g_szDigit = "0123456789.";

// ��ɾ ������ �Ǻ�����
static char *g_szAlpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#_";

static char *g_szOperator = "[]#*";

// �޸� �Ǻ�
static char *g_szComma = ",";

// ���ڿ� �Ǻ�����
static char *g_szString = "'";

// ������ �Ǻ�����
static char *g_szAssign = "=";

/////////////////////////////////////////////////////////////////////////
// �Ǻ��Լ�

// ��ɾ� ����Ʈ "{}"
static char* is_comlist( char c )
{
	return strchr( g_szComlist, c );
}

// �ּ� �Ǻ� '//'
static char* is_comment( char c )
{
	return strchr( g_szComment, c );
}

// ���� ���� �Ǻ�, ����, tab, carriage return, line feed�� �Ǻ�
static char* is_white( char c )
{
	return strchr( g_szWhite, c );
}

// ��ġ �ڷ� �Ǻ�
static char* is_digit( char c )
{
	return strchr( g_szDigit, c );
}

// ��ɾ�� �������� �Ǻ��� ���� ���� �Ǻ� �Լ�
static char* is_alpha( char c )
{
	return strchr( g_szAlpha, c );
}

// �޸� �Ǻ�
static char* is_comma( char c )
{
	return strchr( g_szComma, c );
}

// ���ڿ� �Ǻ��Լ�
static char* is_string( char c )
{
	return strchr( g_szString, c );
}

// ������ �Ǻ� '='
static char* is_assign( char c )
{
	return strchr( g_szAssign, c );
}

// ���۷����� �Ǻ�
static char* is_operator( char c )
{
	return strchr( g_szOperator, c );
}

#endif // __PARSERDEF_H__
