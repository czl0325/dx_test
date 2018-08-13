//==============================================================
//					���ɷ���ĺ���
//==============================================================

//--------------------------------------------------------------
// ȫ�ֱ���
//--------------------------------------------------------------
matrix	g_matWorldViewProj;		//��ϱ任����

//--------------------------------------------------------------
// ���������
//--------------------------------------------------------------
texture ButterflyMap : TEXCOORD;
sampler2D S_Butterfly = sampler_state
{
	Texture = (ButterflyMap);
    MipFilter =LINEAR;
    MinFilter =LINEAR;
    MagFilter =LINEAR;
    AddressU  =WRAP;
    AddressV  =WRAP;
};

//--------------------------------------------------------------
// ����ṹ
//--------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos	: POSITION;		//����λ��
	float2 Tex	: TEXCOORD0;	//��������
};

//--------------------------------------------------------------
// ����ṹ
//--------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos	: POSITION;
	float2 Tex	: TEXCOORD0;	//��������
	float  Fade : COLOR;
};

//--------------------------------------------------------------
// ������Ⱦ������
//--------------------------------------------------------------
VS_OUTPUT VS_ButterflyMain(VS_INPUT Input)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    Out.Pos = mul(Input.Pos,g_matWorldViewProj);
    
    Out.Tex = Input.Tex;
    float Dist = Out.Pos.z;
    if (Dist >1000) Dist = 1000;
    Out.Fade = smoothstep(0,3000,Dist);
    return Out;
}

//--------------------------------------------------------------
// ������Ⱦ������
//--------------------------------------------------------------
float4 PS_ButterflyMain(float2 Tex :TEXCOORD0, float Fade:COLOR):COLOR
{
  float4 sceneColor = tex2D(S_Butterfly,Tex);
  sceneColor	= lerp(sceneColor,float4(0.85f,0.85f,0.85f,0.0f),Fade);
  return sceneColor;
}

//--------------------------------------------------------------
// ����
//--------------------------------------------------------------
technique Butterfly
{
	pass  P0
	{
		ZEnable			= TRUE;		//��Ȳ��Կ���
		ZFunc			= LESSEQUAL;//С�ڵ�ǰ��Ȼ�������ʱ����Ⱦ
		ZWriteEnable	= FALSE;	//���ֵ�ǰ�����ֵ
		CullMode		= None;
		AlphaBlendEnable = TRUE;
		SrcBlend		= SRCALPHA;
		DestBlend		= InvSrcAlpha;
	    
		BlendOp = ADD;
		BlendOpAlpha = ADD;
	    
		MultiSampleAntialias =false;//ͼ�ο����
		Lighting    =FALSE;
		FogEnable   =FALSE;
		VertexShader  =compile vs_2_0 VS_ButterflyMain();
		PixelShader   =compile ps_2_0 PS_ButterflyMain();
	}
}
