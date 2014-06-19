// -------------------------------------------------------------
// �׸��� ��
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWVPB;		// �ؽ�ó��ǥ����� ����
//float4x4 mWLP;		// ���ÿ��� �������������� ��ǥ��ȯ
//float4x4 mWLPB;		// �ؽ�ó��ǥ����� ����
//float4   vCol;
float distNearFar = 1000.f;	// ������������ near - far ���� �Ÿ�

float4 vLightDir;	// ��������
float3 vEyePos;		// ī�޶���ġ(������ǥ��)

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture AlphaMap;
sampler AlphaMapSamp = sampler_state
{
    Texture = <AlphaMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

// -------------------------------------------------------------
texture TerrainMap;
sampler TerrainMapSamp = sampler_state
{
    Texture = <TerrainMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap1;
sampler TileMapSamp1 = sampler_state
{
    Texture = <TileMap1>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap2;
sampler TileMapSamp2 = sampler_state
{
    Texture = <TileMap2>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap3;
sampler TileMapSamp3 = sampler_state
{
    Texture = <TileMap3>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap4;
sampler TileMapSamp4 = sampler_state
{
    Texture = <TileMap4>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

// ------------------------------------------------------------
// �ؽ�ó
// ------------------------------------------------------------
texture BackGroundMap;
sampler BackGroundMapSamp = sampler_state
{
    Texture = <BackGroundMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};


// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos			: POSITION;
	float4 Color		: COLOR0;
	float2 Tex			: TEXCOORD0;
    float3 N			: TEXCOORD1;
    float3 Eye			: TEXCOORD2;	
    float3 Loc			: TEXCOORD3;
};

// -------------------------------------------------------------
// 0�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL,	         // �𵨹���
      float2 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ      
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����

    // ��ǥ��ȯ
	float4 pos = mul( Pos, mWVP );

    // ��ġ��ǥ
    Out.Pos = pos;
    Out.Loc = Pos.xyz;

    // �ؽ�ó ��ǥ
    Out.Tex = Tex;

	float amb = -vLightDir.w;	// ȯ�汤�� ����
	float3 L = -vLightDir.xyz; // ������ǥ�迡���� ��������
//	Out.Color = vColor * max(amb, dot(Normal, -vLightDir));
	Out.Color.a = max(amb, dot(Normal, -vLightDir.xyz));
	
	// �ݿ��ݻ�� ����
	Out.N   = Normal.xyz;
	Out.Eye = vEyePos - Pos.xyz;
    
    return Out;
}


// -------------------------------------------------------------
// 0�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -vLightDir.xyz;
	float3 H = normalize(L + normalize(In.Eye));	// ��������
	float3 N = normalize(In.N);

	float4 Out;
	float4 base = tex2D( TerrainMapSamp, In.Tex );
	float4 alpha = tex2D( AlphaMapSamp, In.Tex );
	float4 tile1 = tex2D( TileMapSamp1, In.Tex );
	float4 tile2 = tex2D( TileMapSamp2, In.Tex );
	float4 tile3 = tex2D( TileMapSamp3, In.Tex );
	float4 tile4 = tex2D( TileMapSamp4, In.Tex );
	
	Out = base;
	Out = (Out * (1.f - alpha.a)) + (tile1 * alpha.a);
	Out = (Out * (1.f - alpha.r)) + (tile2 * alpha.r);
	Out = (Out * (1.f - alpha.g)) + (tile3 * alpha.g);
	Out = (Out * (1.f - alpha.b)) + (tile4 * alpha.b);
	
	Out = In.Color.a * Out;	// Ȯ�걤+ȯ�汤
	//	 + pow(max(0,dot(N, H)), 10);	// �ݿ��ݻ籤
	
// 	float depthAlpha = 1f;
// 	float len = max(0, -10 - In.Pos.y);
// 	len = saturate(len * depthAlpha);
// 	len = rsqrt(len);
// 	len = saturate( Rcp(len) );
// 
// 	float4 absorbLay(1, 1, 1, 1);	// ���� ����� ���� �� (��, ��, û)
// 	float4 absorbLay * len;
	Out.a = 1;	

    return Out;
}



// -------------------------------------------------------------
// 1�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL,	         // �𵨹���
      float2 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ      
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����

    // ��ǥ��ȯ
	float4 pos = mul( Pos, mWVP );

    // ��ġ��ǥ
    Out.Pos = pos;
    Out.Loc.z = pos.z / distNearFar;

    // �ؽ�ó ��ǥ
    Out.Tex = Tex;

//	float amb = -vLightDir.w;	// ȯ�汤�� ����
//	float3 L = -vLightDir.xyz; // ������ǥ��	������ ��������
//	Out.Color = vColor * max(amb, dot(Normal, -vLightDir));
//	Out.Color.a = max(amb, dot(Normal, -vLightDir.xyz));
	
	// �ݿ��ݻ�� ����
//	Out.N   = Normal.xyz;
//	Out.Eye = vEyePos - Pos.xyz;

    return Out;
}


// -------------------------------------------------------------
// 1�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
	float4 Out;
//	Out = In.Loc.z;
//	Out = float4(1,0,1,1);
//	float4 base = tex2D( TerrainMapSamp, In.Tex );
//	Out = base;	

	Out = In.Loc.z;
	Out.a = 1;

    return Out;
}


// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0// ���� ���
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
//	    Sampler[0] = (TerrainMapSamp);
    }

    pass P1// ���̰� ����
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    
}

