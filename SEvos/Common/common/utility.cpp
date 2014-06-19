
#include "stdafx.h"
#include "utility.h"

#include <list>
#include <algorithm>

using namespace std;


//-----------------------------------------------------------------------------//
// szDirectory������ pFindExt Ȯ���� ����Ʈ�� ���Ե� ������ pFileList�� �����Ѵ�.
//
// szDirectory: Ž���ϰ��� �ϴ� ���丮 ���
// pFindExt: ã���� �ϴ� Ȯ����, 2���̻� �����Ҽ��ְ� �ϱ����ؼ� ����Ʈ �ڷ����°� �Ǿ���.
// pFileList: ��ġ�ϴ� Ȯ���ڸ� ���� �����̸��� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CollectFile( list<string> *pFindExt, char *szDirectory, list<string> *pFileList )
{
	if( !pFileList ) return FALSE;
	if( !pFindExt ) return FALSE;

    CFileFind finder;
    BOOL bWorking = finder.FindFile( CString(szDirectory) +"*.*" );
    while( bWorking )
    {
        bWorking = finder.FindNextFile();
        if( finder.IsDots() )
            continue;

        if( finder.IsDirectory() )
        {
			CollectFile( pFindExt, (char*)(LPCTSTR)(finder.GetFilePath()+"\\"), pFileList ); 
	    }
        else
        {
			CString strPath = finder.GetFilePath();

			list<string>::iterator it = pFindExt->begin();
			while( pFindExt->end() != it )
			{
				if (CompareExtendName((char*)(LPCTSTR)strPath , strPath.GetLength(), (char*)(*it).c_str()))
				{
					CString filename = finder.GetFileTitle() + CString(".") + it->c_str();
					pFileList->push_back( (char*)(LPCTSTR)filename );
					break;
				}
				++it;
			}
/*
			if( 3 < strPath.GetLength() )
			{
				list<string>::iterator it = pFindExt->begin();
				while( pFindExt->end() != it )
				{
					if( -1 != strPath.Find((*it).c_str(), strPath.GetLength()-3) )
					{
						CString filename = finder.GetFileTitle() + CString(".") + it->c_str();
						pFileList->push_back( (char*)(LPCTSTR)filename );
						break;
					}
					++it;
				}
			}
/**/

        }
    }
    finder.Close();

	return TRUE;
}


//-----------------------------------------------------------------------------//
// CRect::NormalizeRect()�� ���� ���� �Ѵ�.
//-----------------------------------------------------------------------------//
void NormalizeRect( RECT *prt )
{
	if( prt->left > prt->right ) 
	{
		int tmp = prt->left;
		prt->left = prt->right;
		prt->right = tmp;
	}
	if( prt->top > prt->bottom ) 
	{
		int tmp = prt->top;
		prt->top = prt->bottom;
		prt->bottom = tmp;
	}
}


float Lerp(float p1, float p2, float alpha)
{
	return p1 * (1.f - alpha) + p2 * alpha;
}


//------------------------------------------------------------------------
// srcFileName�� Ȯ���ڿ� compareExtendName �̸��� ���ٸ� true�� �����Ѵ�.
// Ȯ���ڴ� srcFileName ������ '.'�� ���� ������ �̴�.
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
BOOL CompareExtendName(char *srcFileName, int srcStringMaxLength, char *compareExtendName)
{
	const int len = strnlen_s(srcFileName, srcStringMaxLength);
	if (len <= 0)
		return FALSE;

	int count = 0;
	char temp[5];
	for (int i=0; i < len && i < 4; ++i)
	{
		const char c = srcFileName[ len-i-1];
		if ('.' == c)
		{
			break;
		}
		else
		{
			temp[ count++] = c;
		}
	}
	temp[ count] = NULL;

	char extendName[5];
	for (int i=0; i < count; ++i)
		extendName[ i] = temp[ count-i-1];
	extendName[ count] = NULL;

	if (!strcmp(extendName, compareExtendName))
	{
		return TRUE;
	}

	return FALSE;
}


//------------------------------------------------------------------------
// ���� ���ϸ� srcFileName�� Ȯ���ڰ� extendName�� ���ϸ����� �ٲ㼭 
// newFileName�� �����ؼ� �����Ѵ�.
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void ChangeExtendName(char *srcFileName, int srcStringMaxLength, char *extendName, char *newFileName, int newFileNameLength)
{
	const int len = strnlen_s(srcFileName, srcStringMaxLength);
	if (len <= 0)
	{
		strcpy_s(newFileName, 2, ".");
		strcat_s(newFileName, newFileNameLength, extendName);
		return ;
	}

	strcpy_s(newFileName, strlen(srcFileName)+1, srcFileName);

	BOOL success = FALSE;
	for (int i=0; i < len && i < 4; ++i)
	{
		const char c = newFileName[ len-i-1];
		if ('.' == c)
		{
			success = TRUE;
			strcpy_s(&newFileName[ len-i], strlen(extendName)+1, extendName);
			break;
		}
	}

	if (!success)
	{
		strcat_s(newFileName, newFileNameLength, ".");
		strcat_s(newFileName, newFileNameLength, extendName);
	}
}


//------------------------------------------------------------------------
// ��θ� ������ srcFilename���� ������ �����̸��� dest�� �����ؼ� �����Ѵ�.
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void GetFileNameNoPath(char *srcFileName, char *dest)
{
	const int len = strlen(srcFileName);

	int idx = 0;
	for (int i=0; i < len; ++i)
	{
		if (('/' == srcFileName[ len-i-1]) ||
			('\\' == srcFileName[ len-i-1]))
		{
			idx = len-i;
			break;
		}
	}

	const int size = len - idx + 1;
	if (0 >= size)
	{
		dest[ 0] = NULL;
	}
	else
	{
		strcpy_s(dest, size, &srcFileName[ idx]);
	}
}


//------------------------------------------------------------------------
// �����̸����� ��θ� �����´�.
// ��Ʈ���� �ڿ��� ���� �˻��ؼ� '/' or '\\' �� ������, ó���� ��Ʈ������ 
// �� ��ġ���� �����ؼ� �����Ѵ�.
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void GetFilePath(char *srcFileName, char *dest)
{
	const int len = strlen(srcFileName);

	int idx = 0;
	for (int i=0; i < len; ++i)
	{
		if (('/' == srcFileName[ len-i-1]) ||
			('\\' == srcFileName[ len-i-1]))
		{
			idx = len-i;
			break;
		}
	}

	const int size = idx-1;
	if (0 >= size)
	{
		dest[ 0] = NULL;
	}
	else
	{
		strncpy_s(dest, MAX_PATH, srcFileName, size);
		dest[ size] = NULL;
	}
}
