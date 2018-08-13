//==============================================================
//					生成飞舞的蝴蝶
//==============================================================

//--------------------------------------------------------------
// 全局变量
//--------------------------------------------------------------
matrix	g_matWorldViewProj;		//组合变换矩阵

//--------------------------------------------------------------
// 纹理采样器
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
// 输入结构
//--------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos	: POSITION;		//顶点位置
	float2 Tex	: TEXCOORD0;	//纹理坐标
};

//--------------------------------------------------------------
// 输出结构
//--------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos	: POSITION;
	float2 Tex	: TEXCOORD0;	//纹理坐标
	float  Fade : COLOR;
};

//--------------------------------------------------------------
// 顶点渲染主函数
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
// 象素渲染主函数
//--------------------------------------------------------------
float4 PS_ButterflyMain(float2 Tex :TEXCOORD0, float Fade:COLOR):COLOR
{
  float4 sceneColor = tex2D(S_Butterfly,Tex);
  sceneColor	= lerp(sceneColor,float4(0.85f,0.85f,0.85f,0.0f),Fade);
  return sceneColor;
}

//--------------------------------------------------------------
// 技术
//--------------------------------------------------------------
technique Butterfly
{
	pass  P0
	{
		ZEnable			= TRUE;		//深度测试开启
		ZFunc			= LESSEQUAL;//小于当前深度缓冲区的时候渲染
		ZWriteEnable	= FALSE;	//保持当前的深度值
		CullMode		= None;
		AlphaBlendEnable = TRUE;
		SrcBlend		= SRCALPHA;
		DestBlend		= InvSrcAlpha;
	    
		BlendOp = ADD;
		BlendOpAlpha = ADD;
	    
		MultiSampleAntialias =false;//图形抗锯齿
		Lighting    =FALSE;
		FogEnable   =FALSE;
		VertexShader  =compile vs_2_0 VS_ButterflyMain();
		PixelShader   =compile ps_2_0 PS_ButterflyMain();
	}
}
