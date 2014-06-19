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
float4x4 mWLP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWLPB;		// �ؽ�ó��ǥ����� ����
float4   vCol;		// �޽û�
float4	 vLightDir;	// ��������
float    fId = 0;

float    fNear=1.0f;
float    fFar =7.0f;


// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture DecaleMap;
sampler DecaleMapSamp = sampler_state
{
    Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};


// ------------------------------------------------------------
// �ؽ�ó
// ------------------------------------------------------------
texture SrcTex;
sampler SrcSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = POINT;
    MagFilter = POINT;
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
	float4 Diffuse		: COLOR0;
	float4 Ambient		: COLOR1;
	float4 ShadowMapUV	: TEXCOORD0;
	float4 Depth		: TEXCOORD1;
	float4 DecaleTex	: TEXCOORD2;
};

// -------------------------------------------------------------
// 1�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL,	         // �𵨹���
      float2 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ǥ��ȯ
	float4 pos = mul( Pos, mWLP );
	pos.z += 0.001f; // ���� �𵨰� ���̰��� ��ġ�� ������ �ڷ� �̵���Ų��.
	
    // ��ġ��ǥ
    Out.Pos = pos;
    
    // ����
    Out.Diffuse.w = (pos.z-fNear)/(fFar-fNear);  

    // ī�޶���ǥ�迡���� ���̸� �ؽ�ó�� �ִ´�
    Out.ShadowMapUV = pos;

    return Out;
}


// -------------------------------------------------------------
// 1�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
    float4 Out;
	Out = float4(0, 0, 0, 1);
	Out.a = (In.Diffuse.w > 0.5f)? 1.f-In.Diffuse.w : In.Diffuse.w;	

    return Out;
}


// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL,	         // �𵨹���
      float2 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
    Out.Pos = mul(Pos, mWVP);
    
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// Ȯ�걤
	Out.Ambient = 0.3f;							 // ȯ�汤
	
	// �׸��� ��
	Out.ShadowMapUV = mul(Pos, mWLPB);

	// �񱳸� ���� ���̰�
	Out.Depth       = mul(Pos, mWLP);

	// ���� �ؽ�ó
	Out.DecaleTex   = mul(Pos, mWVPB);
	
    return Out;
}



// 2�н�:�ȼ����̴�(�ؽ�ó����)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
    float4 Color;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
	float4 decale = tex2Dproj( DecaleMapSamp, In.DecaleTex );

//  Color = In.Ambient
//		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);
//	Color = 0.3f
//		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);		 

//	Color = 0.3f + ((shadow < In.Depth.z -0.33f) ? 0 : 0.7f);
//	Color = 1;

	Color = 0.3f + ((shadow < 1) ? 0 : 0.7f);
	
	return Color * decale;
}



// -------------------------------------------------------------
// 2�н�:�ȼ����̴�(�ؽ�ó����)
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color = 0;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
    
    Color = In.Ambient
	 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);

    return Color;
}  

PixelShader PS_ZEdge = asm
{
    ps_1_1
    
	tex t0	// ����������(�������)
	tex t1	// ����������(�����ϴ�)
	tex t2	// ����������(�����ϴ�)
	tex t3	// ����������(�������)
	
	// �μ��� t#�������͸� 2�� ����Ҽ� �����Ƿ� �ϴ� ����
	mov r0, t0
	mov r1, t2
	
	add_x2     r0,   r0,  -t1	;       r0                  r1
	add_x2     r1,   r1,  -t3	; 4*(t0.a-t1.a)        4*(t2.a-t3.a)
	mul_x2		r0,   r0,   r0	;
	mul_x2		r1,   r1,   r1	; 16*(t0-t1����)^2,  16*(t2-t3����)^2)
	add_x2     r0,   r0,   r1	; r0.a = 64((t0-t0����)^2+(t3-t1����)^2)
	mov        r0,   1-r0.a		; r0.a = (1-64((t0-t0����)^2+(t3-t1����)^2))
};

	
// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0// �׸��ڸ� ����
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    
    pass P1// ���ĺ���
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }    
    
    pass P2 // ����
    {
        // ���̴�
        PixelShader  = <PS_ZEdge>;
		// �ؽ�ó
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
    
}
