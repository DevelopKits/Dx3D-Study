//-----------------------------------------------------------------------------//
// 2008-03-13  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__DRAWTEXT_H__)
#define __DRAWTEXT_H__


class CDrawText
{
public:
	CDrawText();
	virtual ~CDrawText();

protected:
	typedef struct _tagSText
	{
		int x, y;
		IDirect3DTexture9 *ptex;
		SVtxDiff v[ 6];

//		SVector3
	} SText;
	typedef std::map< int, SText* > TextMap;	// key = hiword(x)+loword(y)

	LPD3DXFONT m_pDxFont;
	TextMap m_Textmap;				// ��µ� �ؽ�Ʈ�� �����Ѵ�. x,y��ġ���� Ű��.
	std::list< SText* > m_Garbage;	// ����� ������ ������ �ؽ�Ʈ�� �����ؼ� �޸𸮸� ��������.

public:
	BOOL Create();

	BOOL CreateFont( int FontId, char *pFontName, int Size );
	void DrawText( int X, int Y, char *pString, int FontId=0 );
	void Render();
	void Clear();

protected:
	void SetText( int FontId, SText *pTextData, char *pString );

};

#endif // __DRAWFONT_H__
