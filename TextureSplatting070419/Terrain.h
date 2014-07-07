#ifndef CTERRAIN
#define CTERRAIN
#include "defines.h"



#pragma once

struct CUSTOMVERTEX {
	float x, y, z;		// Position
	float u1, v1;		// Alphamap coordinates
	float u2, v2;		// Texture coordinates
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEX2)

struct Line3D
{
	D3DXVECTOR3     Pos;
	D3DCOLOR        Col;
};
#define D3DFVF_LINE3D (D3DFVF_XYZ|D3DFVF_DIFFUSE)

typedef struct _Texture
{
	IDirect3DTexture9* pTexture;
	IDirect3DTexture9* pAlphamap;
	_Texture()
	{
		pTexture = NULL;
		pAlphamap = NULL;
	}

}Texture;


class CTerrain
{

	LPDIRECT3DVERTEXBUFFER9			 m_pVB;								//�������� ��ü
	LPDIRECT3DINDEXBUFFER9			 m_pIB;								//�ε������� ��ü
	LPDIRECT3DTEXTURE9				 m_pBaseTexture;					//���̽��ؽ��� ��ü
	Texture							 m_Texture[MAXTEXNUM];				//���������ؽ��� ��ü

	CUSTOMVERTEX*					 m_pVertices;

	WORD							 m_wWidth;							//���� ���λ�����
	WORD							 m_wHeight;							//���� ���λ�����
	WORD							 m_wWidthVertices;					//���� �����ؽ�
	WORD							 m_wHeightVertices;					//���� ���ι��ؽ�
	DWORD							 m_dwTotalVertices;					//���� �ѹ�Ƽ��
	
	DWORD							 m_dwTotalIndices;					//���� ���ε�
	FLOAT							 m_fLength;							//���� ��Ÿ���� ����
	DWORD							 m_dwPolygonsCount;					//�׷����� ������ ī��Ʈ					
	LPDIRECT3DDEVICE9				 m_pd3dDevice;				
	
	//-------------------------------------------------------------
	int								 m_nCurSelIndex;
	int								 m_pDrawIndex[4];


	FLOAT							 m_fBrushSize;
	FLOAT							 m_fSmoothSize;

	BOOL							 m_blPickOK;							// pick ���� ����
	D3DXVECTOR3						 m_vGetPos;								// pick ��ġ
	FLOAT							 m_fGetU;
	FLOAT							 m_fGetV;
	BOOL							 m_blClicked;
	int								 m_nTexPosX;
	int								 m_nTexPosY;


public:
	CTerrain(LPDIRECT3DDEVICE9);
	~CTerrain();

	bool							LoadTextures(); 
	BOOL							CreateAlphaMaps();
	BOOL							IntersectTriangle( const D3DXVECTOR3& orig,
														const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
														D3DXVECTOR3& v1, D3DXVECTOR3& v2,
														FLOAT* t, FLOAT* u, FLOAT* v );
	void							PickingProcess( RECT &rtRect , float fCurPosX, float fCurPosY );
	bool							DrawAlphamaps( int nIndex );
	void							DrawPickCircle( int Count, float size, D3DCOLOR Col );
	void							AlphaTextureRender();
	void							MiniAlphaTextureRender();
	HRESULT							TerrainInit(  WORD wWidth, WORD wHeight, float fLength /*, char *chFileName */);
	void							TerrainRender();

	inline	DWORD					GetTotalIndices()  const		{ return m_dwTotalIndices; }
	inline	DWORD					GetTotalVertices() const		{ return m_dwTotalVertices; }

	inline	int						GetCurSelIndex()   const		{ return m_nCurSelIndex; }
	inline	int*					GetDrawIndex()     				{ return m_pDrawIndex; }
	inline	float					GetBrushSize()	   const		{ return m_fBrushSize; }
	inline	float					GetSmoothSize()	   const		{ return m_fSmoothSize; }
	inline	BOOL					GetPickOK()		   const		{ return m_blPickOK; }
	inline	BOOL					GetClicked()       const	    { return m_blClicked; }

	inline	void					SetCurSelIndex( int nIndex )   	{ m_nCurSelIndex = nIndex;  }
	inline	void					SetBrushSize( float fSize )	   	{ m_fBrushSize   = fSize;   }
	inline	void					SetSmoothSize( float fSize )	{ m_fSmoothSize  = fSize;   }
	inline	void					SetPickOK( bool blPick ) 		{ m_blPickOK     = blPick;  }
	inline	void					SetClicked( bool blClick ) 		{ m_blClicked    = blClick; }

	inline	void					SetDrawIndex( int *pIndex )     
	{
		for( int i=0 ; i<4 ; ++i )
			m_pDrawIndex[i]  = pIndex[i];  
	}

};

#endif
