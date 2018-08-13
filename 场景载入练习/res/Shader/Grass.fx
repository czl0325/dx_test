//==============================================================
//					���ɰڶ��Ĳ�
//==============================================================

//--------------------------------------------------------------
// ȫ�ֱ���
//--------------------------------------------------------------
matrix	g_matWorldViewProj;	//��ϱ任����
float	g_XOffset;			//����X���ƫ����
float	g_ZOffset;			//����Z���ƫ����
float4	FogColor = float4(0.83f,0.83f,0.83f,0.0f);

//--------------------------------------------------------------
// ���������
//--------------------------------------------------------------
texture GrassMap : TEXCOORD;
sampler2D S_Grass = sampler_state
{
	Texture = (GrassMap);
    MipFilter =LINEAR;
    MinFilter =LINEAR;
    MagFilter =LINEAR;
    AddressU  =WRAP;
    AddressV  =WRAP;
};

//--------------------------------------------------------------
// ��������ṹ
//--------------------------------------------------------------
struct VS_INPUT
{
  float4 Pos	:POSITION;
  float2 Tex	:TEXCOORD0;
  float2 Dir	:TEXCOORD1;
};

//--------------------------------------------------------------
// ��������ṹ
//--------------------------------------------------------------
struct VS_OUTPUT
{
  float4 Pos	:POSITION;
  float2 Tex	:TEXCOORD0;
  float	 Fade	:COLOR;
};

//--------------------------------------------------------------
// ������Ⱦ������
//--------------------------------------------------------------
VS_OUTPUT VS_GrassMain(VS_INPUT Input)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    float4 OutPos = float4(0,0,0,0);
	float4 NewPos = Input.Pos;
	float	DirX = Input.Dir.x;
	float	DirZ = Input.Dir.y;
	NewPos.x += g_XOffset*DirX;//����X��Z�����ϰڶ����λ��
	NewPos.z += g_ZOffset*DirZ;
	OutPos = mul(NewPos,g_matWorldViewProj);
    Out.Pos = OutPos;

    Out.Fade =smoothstep(0.0f,2000.0f,OutPos.z);
    Out.Tex =Input.Tex;
    return Out;
}

//--------------------------------------------------------------
// ������Ⱦ������
//--------------------------------------------------------------
float4 PS_GrassMain(float2 Tex :TEXCOORD0, float Fade :COLOR):COLOR
{
  float4 sceneColor = tex2D(S_Grass,Tex);
  sceneColor = lerp(sceneColor,FogColor,Fade);//ͨ����ɫ�Ļ�϶�Զ���Ĳݽ�����
  return sceneColor;
}

//--------------------------------------------------------------
// Ч������
//--------------------------------------------------------------
technique Grass
{
	pass P0
	{
		ZEnable =true;		//��Ȳ��Կ���
		ZFunc =less;		//С�ڵ�ǰ��Ȼ�������ʱ����Ⱦ
		ZWriteEnable =true;//���ֵ�ǰ�����ֵ
		cullmode   =none;
		AlphaTestEnable =true;//����alpha����
		AlphaRef = 120;
		AlphaFunc=Greater;
		Lighting =false;
		FogEnable =false;
		VertexShader =compile vs_2_0 VS_GrassMain();
		PixelShader  =compile ps_2_0 PS_GrassMain();
	}
}
