
#include "stdafx.h"
#include "control.h"
#include "lifebar.h"
#include "control.h"
#include "character.h"


CLifeBar::CLifeBar()
{

}

CLifeBar::~CLifeBar()
{

}


//-----------------------------------------------------------------------------//
// Ŭ���� ����
// pChar : �ش� ĳ������ HP,SP�� ����Ѵ�.
// MaxHP : ĳ���� �ִ� HP, �ʱⰪ���� Max���� �����Ǹ�, Max���� Barũ��� ����ȴ�.
// MaxSP : ĳ���� �ִ� SP, �ʱⰪ���� Max���� �����Ǹ�, Max���� Barũ��� ����ȴ�.
// Type  : ���ΰ�, ���� ���� ��µǴ� ����� �ٲ��.
//-----------------------------------------------------------------------------//
BOOL CLifeBar::Create( CCharacter *pChar, int MaxHp, int MaxSp, CHARACTER_TYPE Type )
{
	const int X = 160, Y = 44;
	m_Type = Type;
	m_pCharacter = pChar;
	m_MaxHp = MaxHp;
	m_MaxSp = MaxSp;

	// hero
	if( CHAR_HERO == Type )
	{
		CreateBar( m_HPBar, X, Y,    MaxHp/2, 29, D3DXCOLOR(1,0,0,0) );
		CreateBar( m_SPBar, X, Y+36, MaxSp/2, 15, D3DXCOLOR(0,0,1,0) );
	}
	// enemy
	else if( CHAR_ENEMY == Type )
	{
		CreateBar( m_HPBar, X, Y,   MaxHp/5, 5, D3DXCOLOR(0.8f,0,0,0) );
		CreateBar( m_SPBar, X, Y+5, MaxSp/5, 5, D3DXCOLOR(0,0,0.8f,0) );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void CLifeBar::Render()
{
	g_pDevice->SetTransform( D3DTS_WORLD, &g_matIdentity ); // identity matrix
	g_pDevice->SetFVF( SVtxRhwDiff::FVF );
	g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, m_HPBar, sizeof(SVtxRhwDiff) );
	g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, m_SPBar, sizeof(SVtxRhwDiff) );
}


//-----------------------------------------------------------------------------//
// ���ϸ��̼�
// ������ ��� �Ӹ����� �������ٸ� ����ؾ� �ϹǷ� �̵��ø��� ��ġ�� ������Ʈ �ؾ��Ѵ�.
//-----------------------------------------------------------------------------//
void CLifeBar::Animate( int nDelta )
{
	if( CHAR_ENEMY == m_Type )
	{
		Vector3 out;
		Vector3 pos = m_pCharacter->GetPos();
		g_Camera.GetVec3Project( &out, &pos );
		MoveBar( m_HPBar, (int)(out.x - (float)m_MaxHp/10.f), (int)out.y );
		MoveBar( m_SPBar, (int)(out.x - (float)m_MaxHp/10.f), (int)(out.y+5.f) );
	}
}


//-----------------------------------------------------------------------------//
// HP ����
//-----------------------------------------------------------------------------//
void CLifeBar::SetHp( int Hp )
{
	if( CHAR_HERO == m_Type )			Hp /= 2;
	else if( CHAR_ENEMY == m_Type )		Hp /= 5;
	AdjustBar( m_HPBar, Hp );
}


//-----------------------------------------------------------------------------//
// SP ����
//-----------------------------------------------------------------------------//
void CLifeBar::SetSp( int Sp )
{
	if( CHAR_HERO == m_Type )			Sp /= 2;
	else if( CHAR_ENEMY == m_Type )		Sp /= 5;
	AdjustBar( m_SPBar, Sp );
}


//-----------------------------------------------------------------------------//
// �������� ����
// 
//-----------------------------------------------------------------------------//
void CLifeBar::CreateBar( SVtxRhwDiff Bar[ 6], int X, int Y, int W, int H, DWORD Color )
{
	const float RHW = 1.f;

	Vector3 v[] = 
	{
		Vector3( (float)X, (float)Y, 0 ),
		Vector3( (float)(X+W), (float)Y, 0 ),
		Vector3( (float)X, (float)(Y+H), 0 ),
		Vector3( (float)(X+W), (float)(Y+H), 0 ),
	};
	int idx[] = {0,1,2, 1,3,2};

	for( int i=0; i < 6; ++i )
	{
		Bar[ i].v = v[ idx[ i]];
		Bar[ i].rhw = RHW;
		Bar[ i].c = Color;
	}
}


//-----------------------------------------------------------------------------//
// �������� �̵�
//-----------------------------------------------------------------------------//
void CLifeBar::MoveBar( SVtxRhwDiff Bar[ 6], int X, int Y )
{
	float x = Bar[ 0].v.x;
	float y = Bar[ 0].v.y;
	float W = Bar[ 1].v.x - x;
	float H = Bar[ 2].v.y - y;

	Bar[ 0].v.x = (float)X;
	Bar[ 0].v.y = (float)Y;
	Bar[ 1].v.x = (float)(X+W);
	Bar[ 1].v.y = (float)Y;
	Bar[ 2].v.x = (float)X;
	Bar[ 2].v.y = (float)(Y+H);
	Bar[ 3].v.x = (float)(X+W);
	Bar[ 3].v.y = (float)Y;
	Bar[ 4].v.x = (float)(X+W);
	Bar[ 4].v.y = (float)(Y+H);
	Bar[ 5].v.x = (float)X;
	Bar[ 5].v.y = (float)(Y+H);
}


//-----------------------------------------------------------------------------//
// �������� �� ����
//-----------------------------------------------------------------------------//
void CLifeBar::AdjustBar( SVtxRhwDiff Bar[ 6], int W )
{
	float X = Bar[ 0].v.x;
	Bar[ 1].v.x = X + (float)W;
	Bar[ 3].v.x = X + (float)W;
	Bar[ 4].v.x = X + (float)W;
}

