
#ifndef __REGION_H__
#define __REGION_H__

/*
��ǥ ü��

<VertexBuffer �� ����� ����>

0 1	2 3	4 5 6 7	| 8 9 10 ~~~ nVtxPerCol-1
nVtxPerCol..	|
				|
				|
				|Pos(0,0)
------------------------------------->x
				|
				|
				|
				|
				| ~~~~~~~~~~~~~~~~(nVtxPerCol * nVtxPerRow)-1



<Cell>
���� ���ؽ��� �װ� �� ������� ���簢���̴�. �ﰢ�� �ΰ��� �𿩼� �����ȴ�.
�� ũ��� m_CellSize�� ����ȴ�.
������ �� ũ��� ���� ũ�� Width, Height �� nVtxPerCol, nVtxPerRow �� ���ؼ�
�����ȴ�.

Row �� �� ������ Row�� ���� ���ؽ� �������� -1 �� �Ͱ� ����.
���������� Col �� �� ������ Col�� ���� ���ؽ� �������� -1 �� �Ͱ� ����.

0 1	2 3	4 5 6 7	| 8 9 10 ~~~ nVtxPerRow-1
nVtxPerRow...	|
				|
				|
				|Pos(0,0)
-------------------------------------> x
				|
				|
				|
				|
				| ~~~~~~~~~~~~~~~~(nVtxPerCol * nVtxPerRow)-1

/**/

class CTerrainCursor;
class CRegion
{
public:
	CRegion();
	virtual ~CRegion();
	enum { VISIBLE=0x01, EDIT=0x02, FOCUS1=0x04, FOCUS2=0x08 };

protected:
	BOOL m_Visible;
	DWORD m_Flag;				// visible=0x01, focus=0x40, 
	int m_posX;					// ûũ ��ġ (x,y ûũ������)
	int m_posY;
	int m_columnCellCount;		// ������ �����ϴ� ���� ���� ����
	int m_rowCellCount;			// ������ �����ϴ� ���� ���� ����
	int m_columnVtxCount;
	int m_rowVtxCount;
	float m_cellSize;			// ���� �Ѻ��� ũ�� (���ؽ� ���� ����)
	// ��ü ���ؽ� ���� = (cell_width_cnt+1) * (cell_height_cnt+1)
	// ��ü �� ���� = cell_width_cnt * cell_height_cnt 
	// ������ ���� =  ��ü �� ���� / 2
	// ���� �Ѻ��� ũ�� = cell_width_cnt * cellsize (���簢�� �϶�)

	D3DMATERIAL9 m_mtrl;
	IDirect3DVertexBuffer9 *m_pvtxBuff;
	IDirect3DIndexBuffer9 *m_pidxBuff;
	IDirect3DVertexBuffer9 *m_pvtxBuff_Edge; // �ܰ��� ���ؽ� ����

public:
	virtual void Init(int posX, int posY, int width, int height, float cellsize);
	virtual void Render();
	virtual void RenderEdge();
	virtual void RenderNormal();
	virtual void UpdateEdge();
	virtual void Clear();

	void SetFocus(BOOL bFocus);
	BOOL IsInRegion( CTerrainCursor *pcursor, Vector3 pos );
	BOOL Pick( Vector2 *pPos, Vector3 *pvTarget );
	void UpdateNormals();

	BOOL IsFocus();
	float GetRegionWidth() { return (float)m_columnCellCount * m_cellSize; }
	float GetRegionHeight() { return (float)m_rowCellCount * m_cellSize; }
	float GetHeight( float x, float y );
	float GetHeightMapEntry( int row, int col );
	Vector3 GetNormalVector(int vtxRow, int vtxCol );
	BOOL GetVertex(int row, int col, Vector3 *poutV);
	BOOL GetVertex(int vtxIndex, Vector3 *poutV);
	Vector2 GetPos() { return Vector2((float)m_posX, (float)m_posY); }
	int GetVertexCount() { return m_columnVtxCount * m_rowVtxCount; }
	int GetTriangleCount() { return m_columnCellCount * m_rowCellCount * 2; }
	int GetEdgeVertexCount();
	float GetCellSize() { return m_cellSize; }
	int GetColumnVertexCount() { return m_columnVtxCount; }
	int GetColumnCellCount() { return m_columnCellCount; }
	int GetRowVertexCount() { return m_rowVtxCount; }
	int GetRowCellCount() { return m_rowCellCount; }

	IDirect3DVertexBuffer9* GetVertexBuffer() { return m_pvtxBuff; }
	IDirect3DIndexBuffer9* GetIndexBuffer() { return m_pidxBuff; }
	D3DMATERIAL9* GetMaterial() { return &m_mtrl; }

protected:
	virtual void CreateEdge();
	virtual void CreateVertexBuffer(int posX, int posY, int width, int height, float cellSize);
	virtual void CreateIndexBuffer();
	BOOL isInDrawBrush(float u, float v, CTerrainCursor *pcursor);
	Vector3 _GetNormalVector(int vtxIdx1, int vtxIdx2, int vtxIdx3);

};

#endif
