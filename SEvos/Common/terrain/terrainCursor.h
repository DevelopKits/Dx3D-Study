
#ifndef __TERRAINCURSOR_H__
#define __TERRAINCURSOR_H__


class CTerrain;
class CTerrainCursor
{
public:
	CTerrainCursor();
	CTerrainCursor(CTerrain *pterrain);
	virtual ~CTerrainCursor();

protected:
	enum 
	{ 
		ERASE=0x01,		// ���찳 ����
		MATCH=0x20,		// ���� �ؽ��ĸ� ã�Ƽ� �׸��� ����
	};

	BOOL m_Load;					// Ÿ�� �ؽ��İ� �����Ǿ��ٸ� TRUE
	BOOL m_IsTerrainUp;				// TRUE�̸� ���� ���̸� ���̰�, �ƴϸ� �����.
	DWORD m_Flag;					// ERASE, MATCH etc..
	float m_OutterRadius;			// �귯�� ���� ������ (alpha�� ����)	(0 ~ 1) �ִ� ûũ ũ�����
	float m_InnerRadius;			// �귯�� �߽� ������ (alpha=255 ����)	(0 ~ 1)
	float m_OutterAlpha;			// ū ���� ���İ� (0 ~ 1)
	float m_InnerAlpha;				// ������(�߽ɿ�)�� ���İ� (0 ~ 1)
	float m_Offset;					// ���� ���� ������ (offset�� ��ŭ ���� �ٲ۴�) (�ʴ� ���̰� �ٲ�� �ӵ�)
	char m_TextureName[ MAX_PATH];	// �귯���� ���Ǵ� �ؽ���(Ÿ��) �����̸�

	POINT m_ScreenPos;				// ��ũ������ ��ġ
	Vector3 m_CursorPos;			// Terrain ������ ��ġ

	CTerrain *m_pTerrain;
	CMesh m_InnerCursor;
	CMesh m_OutterCursor;

public:
	BOOL Init();
	void Render();
	void Update( int nElapseTick );
	void UpdateCursor(POINT mousePos);
	void UpdateCursor();
	void Clear();

	BOOL IsLoad() const { return m_Load; }
	void SetTerrain(CTerrain *pterrain) { m_pTerrain = pterrain; }
	void SetEraseMode(BOOL isErase);
	void SetMatchMode(BOOL isMatch);
	void SetOutterRadius( float outterradius );
	void SetInnerRadius( float innerradius );
	void SetOutterAlpha( float outteralpha);
	void SetInnerAlpha( float innerralpha);
	void SetHeightIncrementOffset( float offset );
	void SetTextureName( char *texturename );
	void SetTerrainUp(BOOL isUp) { m_IsTerrainUp = isUp; }

	float GetOutterRadius() { return m_OutterRadius; }
	float GetInnerRadius() { return m_InnerRadius; }
	float GetOutterAlpha() { return m_OutterAlpha; }
	float GetInnerAlpha() { return m_InnerAlpha; }
	BOOL IsEraseMode() { return m_Flag & ERASE; }
	BOOL IsMatchMode() { return m_Flag & MATCH; }
	float GetHeightIncrementOffset() { return (m_IsTerrainUp)? m_Offset : -m_Offset; }
	char* GetTextureName() { return m_TextureName; }

	POINT GetScreenPos() { return m_ScreenPos; }	// screen ��ǥ�� �����Ѵ�. (2����)
	Vector3 GetCursorPos() { return m_CursorPos; }	// map �󿡼��� ��ġ�� �����Ѵ�. (3����)

protected:
	void _UpdateCursor(Vector3 cursorPos);

};

#endif
