
#include "StdAfx.h"

#include "terrain.h"
#include "terrainCursor.h"
#include "common/shader.h"
#include "chunk.h"


CChunk::CChunk()
{
	m_pvtxBuff = NULL;
	m_pidxBuff = NULL;
	m_pvtxBuff_Edge = NULL;
	m_layerCount = 0;

	m_mtrl.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_mtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_mtrl.Specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_mtrl.Emissive = D3DXCOLOR( .0f, .0f, .0f, 1.0f );
	m_mtrl.Power = 5.0f;

}

CChunk::~CChunk() 
{
	Clear();

}


//------------------------------------------------------------------------
// alphaTextureSize : �����ؽ��� ���簢������ �� ���� �ȼ� ����
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CChunk::Init(int colIndex, int rowIndex, int chunkPosX, int chunkPosY, 
				  int chunkWidth, int chunkHeight, float cellSize, int alphaTextureSize)
{
	Clear();

	CRegion::Init(chunkPosX, chunkPosY, chunkWidth, chunkHeight, cellSize);

	m_colIndex = colIndex;
	m_rowIndex = rowIndex;
	m_AlphaTextureSize = alphaTextureSize;

	InitLayer();

	// ù ��° ���̾ �⺻ �� �ؽ��ĸ� ������.
	char szTextureName[ MAX_PATH] = {NULL};
	char *tileName = "detail.tga";
	strcat_s(szTextureName, sizeof(szTextureName), g_szTilePath);
	strcat_s(szTextureName, sizeof(szTextureName), "/");
	strcat_s(szTextureName, sizeof(szTextureName), tileName);

	AddLayer(szTextureName);
//	SetLayerAphaTexture(&m_layer[0], 0xff);

}


//------------------------------------------------------------------------
// Init() �Լ��� ȣ��� �� LoadLayer() �Լ��� ȣ���ؾ� �Ѵ�.
// �̸� ���ؽ��� �ε��� ���۰� ������ �Ŀ� ������ �ؽ��� ������ �о���� 
// ��Ȱ�� �� �Լ��� ����ϰ� �ִ�.
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunk::LoadLayer(SChunkLoader *pLoader)
{
	ClearLayer();

	for (int i=0; i < pLoader->layerCount; ++i)
	{
		SLayerLoader *pLayerLoader = &pLoader->pLayer[ i];
		if (AddLayer(pLayerLoader->texturename, pLayerLoader->alphaname))
		{
			m_layer[ i].flag = pLayerLoader->flag;
		}
	}

	return TRUE;
}


//------------------------------------------------------------------------
// Layer �ʱ�ȭ
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
void CChunk::InitLayer()
{
	for( int m=0; m < MAX_LAYER_CNT; ++m )
	{
		m_layer[ m].flag = 0;
		m_layer[ m].palpha = NULL;
//		m_layer[ m].ptexture = NULL;
//		m_layer[ m].ptexturename = NULL;
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CChunk::Render(CShader *pShader, int pass)
{
	if( !m_Visible ) return;

	const int vtxsize = m_columnVtxCount * m_rowVtxCount;
	const int trisize = m_columnCellCount * m_rowCellCount * 2;
//		if (!(CChunk::VISIBLE & m_layer[ i].flag)) 
//			continue;

	pShader->BeginPass(pass);

	pShader->SetTexture("TerrainMap", m_layer[ 0].ptexture );
	pShader->SetTexture("AlphaMap", m_layer[ 0].palpha );
	pShader->SetTexture("TileMap1", (m_layerCount>1)? m_layer[ 1].ptexture : NULL );
	pShader->SetTexture("TileMap2", (m_layerCount>2)? m_layer[ 2].ptexture : NULL );
	pShader->SetTexture("TileMap3", (m_layerCount>3)? m_layer[ 3].ptexture : NULL );
	pShader->SetTexture("TileMap4", (m_layerCount>4)? m_layer[ 4].ptexture : NULL );

	g_pDevice->SetStreamSource( 0, m_pvtxBuff, 0, sizeof(SVtxNormTex) );
	g_pDevice->SetFVF( SVtxNormTex::FVF );
	g_pDevice->SetIndices( m_pidxBuff );

	pShader->CommitChanges();
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vtxsize, 0, trisize );

	pShader->EndPass();
}

/*
//------------------------------------------------------------------------
// 
// [2011/3/25 jjuiddong]
//------------------------------------------------------------------------
void CChunk::RenderFixedPipeline()
{
	g_pDevice->SetMaterial( &m_mtrl );

	const int vtxsize = m_columnVtxCount * m_rowVtxCount;
	const int trisize = m_columnCellCount * m_rowCellCount * 2;
	for (int i=0; i < m_layerCount; ++i)
	{
		if (!(CChunk::VISIBLE & m_layer[ i].flag)) 
			continue;

		g_pDevice->SetTexture( 0, m_layer[ i].ptexture );
		g_pDevice->SetTexture( 1, m_layer[ i].palpha );

		g_pDevice->SetStreamSource( 0, m_pvtxBuff, 0, sizeof(SVtxNormTex) );
		g_pDevice->SetFVF( SVtxNormTex::FVF );
		g_pDevice->SetIndices( m_pidxBuff );
		g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vtxsize, 0, trisize );
	}

	g_pDevice->SetTexture( 0, NULL );
	g_pDevice->SetTexture( 1, NULL );

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
}
/**/

//------------------------------------------------------------------------
// ���� ���� ȭ�鿡 ����Ѵ�. ù��° ���̾ ȭ�鿡 ����ϰ� �ؽ��Ĵ�
// �������� �ʴ´�.
// [2011/3/14 jjuiddong]
//------------------------------------------------------------------------
void CChunk::RenderDepth()
{
	if (!m_Visible) return;
	const int vtxsize = m_columnVtxCount * m_rowVtxCount;
	const int trisize = m_columnCellCount * m_rowCellCount * 2;
	for (int i=0; i < 1; ++i)
	{
		if (!(CChunk::VISIBLE & m_layer[ i].flag)) 
			continue;
		g_pDevice->SetStreamSource( 0, m_pvtxBuff, 0, sizeof(SVtxNormTex) );
		g_pDevice->SetFVF( SVtxNormTex::FVF );
		g_pDevice->SetIndices( m_pidxBuff );
		g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vtxsize, 0, trisize );
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CChunk::Clear()
{
	CRegion::Clear();
	ClearLayer();
}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CChunk::ModifyHeight( CTerrainCursor *pcursor, int nElapsTick )
{
	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	const int vtxSize = GetVertexCount();
	Vector2 cursorPos(pcursor->GetCursorPos().x, pcursor->GetCursorPos().z);

	for (int i=0; i < vtxSize; ++i)
	{
		Vector2 v1(pv[i].v.x, pv[i].v.z);
		Vector2 p = cursorPos - v1;
		const float len = p.Length();
		if (pcursor->GetOutterRadius() < len) continue;
		float offsetH = pcursor->GetHeightIncrementOffset() * (nElapsTick * 0.001f);
		pv[ i].v.y += offsetH * (pcursor->GetOutterRadius()-len) / pcursor->GetOutterRadius();
	}

	m_pvtxBuff->Unlock();

	UpdateEdge();
}


//------------------------------------------------------------------------
// Layer ����
// [2011/2/11 jjuiddong]
//------------------------------------------------------------------------
CChunk::SLayer* CChunk::GetLayer(int idx)
{
	if (m_layerCount <= idx) return NULL;
	return &m_layer[idx];
}


//------------------------------------------------------------------------
// ���̾� �߰�
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunk::AddLayer()
{
	if( MAX_LAYER_CNT <= m_layerCount ) return FALSE;

	m_layer[ m_layerCount].layer = m_layerCount;
	m_layer[ m_layerCount].flag = CChunk::VISIBLE | CChunk::EDIT;

	// 0�� �ε��� ���̾�� �����ؽ��ĸ� �����Ѵ�.
	if (m_layerCount == 0)
	{
		// Create Alpha Texture
		g_pDevice->CreateTexture( m_AlphaTextureSize, m_AlphaTextureSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
	//	g_pDevice->CreateTexture( m_AlphaTextureSize, m_AlphaTextureSize, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, 
			&m_layer[ m_layerCount].palpha, NULL );

		D3DLOCKED_RECT lockrect;
		m_layer[ m_layerCount].palpha->LockRect( 0, &lockrect, NULL, 0 );
		memset( lockrect.pBits, 0x0, lockrect.Pitch*m_AlphaTextureSize );
		m_layer[ m_layerCount].palpha->UnlockRect( 0 );
	}
	else
	{
		m_layer[ m_layerCount].palpha = NULL;
	}

	++m_layerCount;
	return TRUE;
}


//------------------------------------------------------------------------
// szTextureName: Ÿ�� �ؽ��� ���� �̸�
// szAlphaname : ���� �ؽ��� ���� �̸�
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunk::AddLayer( char *szTextureName, char *szAlphaname ) // szAlphaname = NULL
{
	if (!AddLayer()) return FALSE;

	SLayer *pLayer = &m_layer[ m_layerCount-1];
	strcpy_s( pLayer->ptexturename, sizeof(pLayer->ptexturename), szTextureName );
	pLayer->ptexture = CFileLoader::LoadTexture( szTextureName );
	pLayer->palphaname[ 0] = NULL;

	if ((pLayer->layer == 0) && szAlphaname)
	{
		strcpy_s(pLayer->palphaname, sizeof(pLayer->palphaname), szAlphaname);
		if (!LoadAlphaTexture(szAlphaname, pLayer->palpha))
		{
			SetLayerAphaTexture(pLayer, 0xff);
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���̾ insert_layeridx ��ġ�� �����ϰ�, ������ ���̾���� �ڷ� ��ĭ�� 
// �и���.
// pchunk: ������ ���̾ ���� ûũ 
// insert_layeridx: ������ ���̾� �ε���
// player: ������ ���̾� ����
//-----------------------------------------------------------------------------//
BOOL CChunk::InsertLayer( int insert_layeridx, CChunk::SLayer *player )
{
	if( (m_layerCount < insert_layeridx) || (0 > insert_layeridx) ) return FALSE;
	if( MAX_LAYER_CNT <= insert_layeridx ) return FALSE;
	if( MAX_LAYER_CNT <= m_layerCount ) return FALSE;

	// layer�� ��ĭ�� ������ �̵�
	for( int i=insert_layeridx; i < m_layerCount+1; ++i )
		m_layer[ i+1] = m_layer[ i];

	// ����
	m_layer[ insert_layeridx] = *player;
	++m_layerCount;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// �ش� ûũ�� ���̾� layeridx�� ���� �Ѵ�. 
// ���̾ ���ŵ��� �������� �ٽ� ���ĵȴ�.
// pchunk: ���̾ ���� ûũ
// layeridx: ������ ���̾�
//-----------------------------------------------------------------------------//
BOOL CChunk::DeleteLayer( int layeridx )
{
	if( (m_layerCount <= layeridx) || (0 > layeridx) ) return FALSE;

	SAFE_RELEASE( m_layer[ layeridx].palpha );
//	SAFE_ADELETE( m_layer[ layeridx].palphaname );
//	SAFE_ADELETE( m_layer[ layeridx].ptexturename );

	// layer�� ��ĭ�� ������ �̵�
	for( int i=layeridx+1; i < m_layerCount; ++i )
		m_layer[ i-1] = m_layer[ i];

	DeleteAlphaTexture(layeridx);

	--m_layerCount;
	if( 0 > m_layerCount ) 
		m_layerCount = 0;
	return TRUE;
}


//------------------------------------------------------------------------
// layerNum ��ġ�� �����ؽ��ĸ� �ʱ�ȭ �ϰ�, �� �ڿ� �ִ� �����ؽ��ĸ�
// layerNum ��ġ�� �̵���Ų��.
// [2011/3/28 jjuiddong]
//------------------------------------------------------------------------
void CChunk::DeleteAlphaTexture(int layerNum)
{
	IDirect3DTexture9 *pAlpha = m_layer[ 0].palpha;

	DWORD delMask = GetAlphaMask(layerNum);
	DWORD moveMask = 0;
	for (int i=layerNum+1; i < m_layerCount; ++i)
		moveMask |= GetAlphaMask(i);

	D3DLOCKED_RECT lockrect;
	pAlpha->LockRect( 0, &lockrect, NULL, 0 );

	const int size = lockrect.Pitch*m_AlphaTextureSize;
	BYTE *pbits = (BYTE*)lockrect.pBits;
	for (int x=0; x < m_AlphaTextureSize; x++)
	{
		for (int y=0; y < m_AlphaTextureSize; ++y)
		{
			DWORD *ppixel = (DWORD*)(pbits + (x*4) + (lockrect.Pitch * y));
			DWORD moveVal = *ppixel & moveMask;
			*ppixel = *ppixel & ~(delMask | moveMask); // �̵��� AlphaTexture �ʱ�ȭ
			*ppixel = *ppixel | (moveVal << 8);
		}
	}
	pAlpha->UnlockRect( 0 );
}


//-----------------------------------------------------------------------------//
// �ΰ��� ���̾ �ϳ��� ��ģ��.
// ���յ� ���̾ �ٸ� �ؽ��ĸ� �����ٸ�, �Լ��� �����ϰ� ���ϵȴ�.
// �ΰ��� ���̾��� ���İ��� ADD���꿡 ���� ���յȴ�.
// pchunk: ���յ� ���̾ ���� ûũ
// layeridx_from: ���յ� ���̾�, ���յǰ� ��������.
// layeridx_to: ���յ� ���̾�, �״�� �����ִ´�.
//-----------------------------------------------------------------------------//
BOOL CChunk::MergeLayer( int layeridx_from, int layeridx_to )
{
	if( (m_layerCount <= layeridx_from) || (0 > layeridx_from) ) return FALSE;
	if( (m_layerCount <= layeridx_to) || (0 > layeridx_to) ) return FALSE;

	// ���� �ؽ������� �˻�
	if( strcmp(m_layer[ layeridx_from].ptexturename, m_layer[ layeridx_to].ptexturename) )
		return FALSE;

	// ���İ� �ռ�
	//-------------------------------------------------------------
	// ���� �������� ����
	//-------------------------------------------------------------

	// ���յǰ� ���� ���̾�� ���ŵȴ�.
	DeleteLayer( layeridx_from );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// layeridx_to�� layeridx_from�� ���̾ �̵���Ű�� ������ ���̾�� ��ĭ��
// �и���.
// pchunk: �̵��� ���̾ ���� ûũ
// layeridx_from: �̵��� ���̾� �ε���
// layeridx_to: ��ġ�� ���̾� �ε���
//-----------------------------------------------------------------------------//
BOOL CChunk::MoveLayer( int layeridx_from, int layeridx_to )
{
	if( (m_layerCount <= layeridx_from) || (0 > layeridx_from) ) return FALSE;
	if( (m_layerCount <= layeridx_to) || (0 > layeridx_to) ) return FALSE;
	if( layeridx_from == layeridx_to ) return TRUE;

	CChunk::SLayer layer = m_layer[ layeridx_from];
	DeleteLayer(layeridx_from );
	InsertLayer(layeridx_to, &layer );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ������ �� �ִ� ���̾ �����Ѵ�.
// hide, no edit ������ ���̾�� ������ �� �����Ƿ� �ٸ� ���̾ ã�´�.
// �귯���� MATCH���¶�� ���̾�� �귯���� ���� �ؽ��� ���̾ ã�� �����Ѵ�.
// pchunk: ���̾ �����ϰ� �ִ� ûũ
//-----------------------------------------------------------------------------//
CChunk::SLayer* CChunk::GetTopEditLayer( CTerrainCursor *pcursor )
{
	int cnt = m_layerCount;
	for( int i=cnt-1; i >= 0; --i )
	{
		if( !(CChunk::VISIBLE & m_layer[ i].flag) ||
			!(CChunk::EDIT & m_layer[ i].flag) )
			continue;
		if( 
			// cond1
			// MATCH �϶�
			// �귯���� ���� �ؽ��ĸ� ���� ���̾ ã�� �����Ѵ�.
			( (pcursor->IsMatchMode()) && 
			(!strcmp(pcursor->GetTextureName(), m_layer[ i].ptexturename))) ||

			// cond2
			// ERASE �϶�
			// ž ���̾ �����Ѵ�.
			(!(pcursor->IsMatchMode()) && 
			(pcursor->IsEraseMode())) ||

			// cond3
			// MATCH, ERASE ��� �ƴҶ�
			// ž ���̾ �귯���� �ؽ��Ŀ� ���ٸ� �����ϰ�, 
			// �׷��� �ʴٸ� NULL�� �����Ѵ�.
			(!pcursor->IsMatchMode() && 
			!pcursor->IsEraseMode() &&
			(i == (cnt-1)) &&
			(!strcmp(pcursor->GetTextureName(), m_layer[ i].ptexturename)))
			)
			return &m_layer[ i];
	}

	return NULL;
}


//------------------------------------------------------------------------
// �ش� ���̾��� Alpha �ؽ��ĸ� val ������ ä���.
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
void CChunk::SetLayerAphaTexture(SLayer *player, BYTE val)
{
	if (!player) return;

	D3DLOCKED_RECT lockrect;
	player->palpha->LockRect( 0, &lockrect, NULL, 0 );
	memset( lockrect.pBits, val, lockrect.Pitch*m_AlphaTextureSize );
	player->palpha->UnlockRect( 0 );
}


//------------------------------------------------------------------------
// pos : Ŭ���� �� ��ǥ
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
void CChunk::DrawBrush( CTerrainCursor *pcursor )
{
	Vector3 pos = pcursor->GetCursorPos();

	if (!IsInRegion(pcursor, pos)) return;

	float u, v;
	GetBrushUV(pos, &u, &v);

	BOOL addlayer = FALSE;
	SLayer *ptop = NULL;
	ptop = GetTopEditLayer( pcursor );
	if (!ptop && !pcursor->IsEraseMode())
	{
		if (!AddLayer(pcursor->GetTextureName())) 
			return;
		ptop = &m_layer[ m_layerCount-1];
	}
	if (!ptop)
		return;

//	Brush(u, v, pcursor, ptop->layer, ptop->palpha);
	Brush(u, v, pcursor, ptop->layer, m_layer[ 0].palpha);
}


//-----------------------------------------------------------------------------//
// �����ؽ����� x,y��ǥ��(2����) �귯���Ѵ�.
// u,v: �귯���� �����ؽ��� u,v��ǥ
// pbrush: �귯������ 
// palpha: �귯���ϰ��� �ϴ� �ؽ�������
//-----------------------------------------------------------------------------//
void CChunk::Brush( float u, float v, CTerrainCursor *pcursor, int layer, IDirect3DTexture9 *palpha )
{
	// Create Alph Texture
	D3DLOCKED_RECT lockrect;
	palpha->LockRect( 0, &lockrect, NULL, 0 );

	// 1�� ���̾�� �׻� ���İ��� 1�̴�.
	// 2�� ���̾�� Alpha.a �� ���İ��� �����Ѵ�.
	// 3�� ���̾�� Alpha.r �� ���İ��� �����Ѵ�.
	const int shiftSize = GetAlphaShiftSize(layer);
	const int mask = GetAlphaMask(layer);

	BYTE *pbits = (BYTE*)lockrect.pBits;
	for( int ay=0; ay < m_AlphaTextureSize; ++ay )
	{
		for( int ax=0; ax < m_AlphaTextureSize; ++ax )
		{
			float au = (float)ax / (float)m_AlphaTextureSize;
			float av = (float)ay / (float)m_AlphaTextureSize;

			float len = sqrt( ((au-u)*(au-u)) + ((av-v)*(av-v)) );
			len *= (m_columnCellCount * m_cellSize);

			// A8R8G8B8 Format
			DWORD *ppixel = (DWORD*)(pbits + (ax*4) + (lockrect.Pitch * ay));
			int alpha = (*ppixel & mask) >> shiftSize;

			// A8 Format
//			BYTE *ppixel = (BYTE*)(pbits + (ax) + (lockrect.Pitch * ay));
//			int alpha = *ppixel;

			if( len <= pcursor->GetInnerRadius() )
			{
				alpha = (int)(pcursor->GetInnerAlpha() * 255.f);
				if (pcursor->IsEraseMode())
					alpha = 0;
			}
			else if (len <= pcursor->GetOutterRadius())
			{
				// ����
				float width = pcursor->GetOutterRadius() - pcursor->GetInnerRadius(); //pbrush->radius - pbrush->center_radius);
				float delta = 1.f - ((len - pcursor->GetInnerRadius()) / width);
				float value = ((pcursor->GetInnerAlpha() - pcursor->GetOutterAlpha()) * delta) * 255;

				if (pcursor->IsEraseMode())
				{
					if( (255-value) < alpha )
						alpha = (int)(255.f-value);
				}
				else
				{
					if( value > alpha )	
						alpha = (int)value;
				}
			}

			alpha = max(alpha, 0);
			alpha = min(alpha, 255);

			// A8R8G8B8 Format
			*ppixel = ((DWORD)alpha<<shiftSize) | (*ppixel&~mask);

			// A8 Format
//			*ppixel = (BYTE)alpha;
		}
	}

	palpha->UnlockRect( 0 );
}


//------------------------------------------------------------------------
// ���� ûũ���� �귯�� ��ġ pos ������ ��� u,v ��ǥ�� �����Ѵ�.
// pu,pv ��� uv��ǥ���� �����Ǿ� ���ϵȴ�.
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
void CChunk::GetBrushUV(Vector3 pos, float *pu, float *pv )
{
	const int width = (int)(m_columnCellCount * m_cellSize);
	const int height = (int)(m_rowCellCount * m_cellSize);
	Vector2 centerPos((float)m_posX, (float)-m_posY);
	Vector2 brushPos(pos.x, -pos.z);

	brushPos -= centerPos;

	*pu = brushPos.x / (float)width;
	*pv = brushPos.y / (float)height;
}


//------------------------------------------------------------------------
// ûũ�� ������ ���� �����ʿ� �����Ѵ�.
// ����� Height���� �Ҵ� �Ѵ�.
// [2011/2/11 jjuiddong]
//------------------------------------------------------------------------
void CChunk::UpdateChunkToMainTerrain(CTerrain *pmainTerrain)
{
	if (!pmainTerrain) return;
	IDirect3DVertexBuffer9 *pvtxBuf = pmainTerrain->GetVertexBuffer();
	if (!pvtxBuf) return;

	const int mapPitch = (int)((float)pmainTerrain->GetRegionWidth() / m_cellSize) + 1;	
	const int startIdx = m_columnCellCount*m_colIndex + mapPitch*m_rowCellCount*m_rowIndex;
	const int chunkPitch = m_rowVtxCount;

	SVtxNormTex *pmapVtx;
	SVtxNormTex *pchunkVtx;
	pvtxBuf->Lock(0, 0, (void**)&pmapVtx, 0);
	m_pvtxBuff->Lock(0, 0, (void**)&pchunkVtx, 0);

	for (int y=0; y < m_rowVtxCount; ++y)
	{
		for (int x=0; x < m_columnVtxCount; ++x)
		{
			const int mapIdx = startIdx + x + (y * mapPitch);
			const int chunkIdx = x + (y *chunkPitch);
			pmapVtx[ mapIdx].v = pchunkVtx[ chunkIdx].v;
			pmapVtx[ mapIdx].n = pchunkVtx[ chunkIdx].n;
		}
	}

	pvtxBuf->Unlock();
	m_pvtxBuff->Unlock();
}


//------------------------------------------------------------------------
// ���� ���� ������ ûũ�� ������Ʈ �Ѵ�. (vertex, normal ������ ������Ʈ �Ѵ�.)
// [2011/2/18 jjuiddong]
//------------------------------------------------------------------------
void CChunk::UpdateMainTerrainToChunk(CTerrain *pmainTerrain)
{
	if (!pmainTerrain) return;
	IDirect3DVertexBuffer9 *pvtxBuf = pmainTerrain->GetVertexBuffer();
	if (!pvtxBuf) return;

	const int mapPitch = (int)((float)pmainTerrain->GetRegionWidth() / m_cellSize) + 1;	
	const int startIdx = m_columnCellCount*m_colIndex + mapPitch*m_rowCellCount*m_rowIndex;
	const int chunkPitch = m_rowVtxCount;

	SVtxNormTex *pmapVtx;
	SVtxNormTex *pchunkVtx;
	pvtxBuf->Lock(0, 0, (void**)&pmapVtx, 0);
	m_pvtxBuff->Lock(0, 0, (void**)&pchunkVtx, 0);

	for (int y=0; y < m_rowVtxCount; ++y)
	{
		for (int x=0; x < m_columnVtxCount; ++x)
		{
			const int mapIdx = startIdx + x + (y * mapPitch);
			const int chunkIdx = x + (y *chunkPitch);
			pchunkVtx[ chunkIdx].v = pmapVtx[ mapIdx].v;
			pchunkVtx[ chunkIdx].n = pmapVtx[ mapIdx].n;
		}
	}

	pvtxBuf->Unlock();
	m_pvtxBuff->Unlock();
}


//------------------------------------------------------------------------
// szAlphaTextureWriteLocatePath : �����ؽ��İ� ����� ���ϰ��
// ChunkLoader�� �����ϸ鼭 �����ؽ��ĸ� �����ϱ� ������ �ʿ��ϴ�.
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunk::GetChunkLoader( char *szAlphaTextureWriteLocatePath, SChunkLoader *ploader)
{
	ploader->layerCount = m_layerCount;
	for (int i=0; i < m_layerCount; ++i)
	{
		ploader->pLayer[ i].flag = m_layer[ i].flag;
		strcpy_s(ploader->pLayer[ i].texturename, MAX_PATH, m_layer[ i].ptexturename);
		if (i == 0)
		{
			GenerateAlphaTextureFile(szAlphaTextureWriteLocatePath, i, &m_layer[ i], ploader->pLayer[ i].alphaname);
		}
	}

	return TRUE;
}


//------------------------------------------------------------------------
// pLayer : �����ؽ��� ������ ������ ���̾�
// szAlphaTextFileName : ����� �����ؽ��� �����̸��� �����ؼ� �����Ѵ�.
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunk::GenerateAlphaTextureFile( char *szAlphaTextureWriteLocatePath, int layerIndex, SLayer *pLayer, 
									  char *szAlphaTextFileName)
{
	if (!pLayer->palpha) return FALSE;

	char alphaFileName[ MAX_PATH];
	GetAlphaTextureFileName(m_colIndex, m_rowIndex, layerIndex, alphaFileName, sizeof(alphaFileName));
	char fullFileName[ MAX_PATH] = {NULL};
	strcpy_s(fullFileName, sizeof(fullFileName), szAlphaTextureWriteLocatePath);
	strcat_s(fullFileName, sizeof(fullFileName), "/");
	strcat_s(fullFileName, sizeof(fullFileName), alphaFileName);

	FILE *fp;
	fopen_s(&fp, fullFileName, "wb");
	if (fp == NULL)
		return FALSE;

	D3DLOCKED_RECT lockrect;
	pLayer->palpha->LockRect( 0, &lockrect, NULL, 0 );
	fwrite(lockrect.pBits, lockrect.Pitch*m_AlphaTextureSize, sizeof(BYTE), fp);
	pLayer->palpha->UnlockRect( 0 );

	fclose(fp);

	strcpy_s(szAlphaTextFileName, MAX_PATH, fullFileName);

	return TRUE;
}


//------------------------------------------------------------------------
// ���� �ؽ��� �����̸��� �����Ѵ�. 
// newAlphaTextureFileName : ������ ���� �̸��� ����Ǿ� �����Ѵ�.
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
void CChunk::GetAlphaTextureFileName( int chunkColIndex, int chunkRowIndex, int layerIndex, 
							 char *newAlphaTextureFileName, int stringSize)
{
	sprintf_s(newAlphaTextureFileName, stringSize, "chunkAlpha%d%d_%d.alp", 
		chunkColIndex, chunkRowIndex, layerIndex);
}


//------------------------------------------------------------------------
// pAlphaTexture : �̹� �޸𸮰� �����Ǿ� �ִ� ���¿��� �Ѵ�.
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunk::LoadAlphaTexture( char *szAlphaTextureFileName, IDirect3DTexture9 *pAlphaTexture)
{
	FILE *fp;
	fopen_s(&fp, szAlphaTextureFileName, "rb");
	if (fp == NULL)
		return FALSE;

	D3DLOCKED_RECT lockrect;
	pAlphaTexture->LockRect( 0, &lockrect, NULL, 0 );
	fread(lockrect.pBits, lockrect.Pitch*m_AlphaTextureSize, sizeof(BYTE), fp);
	pAlphaTexture->UnlockRect( 0 );

	fclose(fp);
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunk::ClearLayer()
{
	for( int i=0; i < m_layerCount; ++i )
	{
		SAFE_RELEASE( m_layer[ i].palpha );
	}
	m_layerCount = 0;
}


//------------------------------------------------------------------------
// 
// [2011/3/25 jjuiddong]
//------------------------------------------------------------------------
DWORD CChunk::GetAlphaMask(int layerNum)
{
	const int MASK_A = 0xff000000;
	const int MASK_R = 0x00ff0000;
	const int MASK_G = 0x0000ff00;
	const int MASK_B = 0x000000ff;

	// 1�� ���̾�� �׻� ���İ��� 1�̴�.
	// 2�� ���̾�� Alpha.a �� ���İ��� �����Ѵ�.
	// 3�� ���̾�� Alpha.r �� ���İ��� �����Ѵ�.
	int shiftSize = 0;
	int mask = 0;
	switch(layerNum)
	{
	case 1:
		shiftSize = 24;
		mask = 0xff000000;
		break;
	case 2:
		shiftSize = 16;
		mask = 0x00ff0000;
		break;
	case 3:
		shiftSize = 8;
		mask = 0x0000ff00;
		break;
	case 4:
		shiftSize = 0;
		mask = 0x000000ff;
		break;
	}

	return mask;
}


//------------------------------------------------------------------------
// 
// [2011/3/28 jjuiddong]
//------------------------------------------------------------------------
int CChunk::GetAlphaShiftSize(int layerNum)
{
	int shiftSize = 0;
	switch(layerNum)
	{
	case 1: shiftSize = 24; break;
	case 2: shiftSize = 16; break;
	case 3: shiftSize = 8; break;
	case 4: shiftSize = 0; break;
	}
	return shiftSize;
}
