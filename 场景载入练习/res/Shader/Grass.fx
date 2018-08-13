//==============================================================
//					生成摆动的草
//==============================================================

//--------------------------------------------------------------
// 全局变量
//--------------------------------------------------------------
matrix	g_matWorldViewProj;	//组合变换矩阵
float	g_XOffset;			//草在X轴的偏移量
float	g_ZOffset;			//草在Z轴的偏移量
float4	FogColor = float4(0.83f,0.83f,0.83f,0.0f);

//--------------------------------------------------------------
// 纹理采样器
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
// 顶点输入结构
//--------------------------------------------------------------
struct VS_INPUT
{
  float4 Pos	:POSITION;
  float2 Tex	:TEXCOORD0;
  float2 Dir	:TEXCOORD1;
};

//--------------------------------------------------------------
// 顶点输出结构
//--------------------------------------------------------------
struct VS_OUTPUT
{
  float4 Pos	:POSITION;
  float2 Tex	:TEXCOORD0;
  float	 Fade	:COLOR;
};

//--------------------------------------------------------------
// 顶点渲染主函数
//--------------------------------------------------------------
VS_OUTPUT VS_GrassMain(VS_INPUT Input)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    float4 OutPos = float4(0,0,0,0);
	float4 NewPos = Input.Pos;
	float	DirX = Input.Dir.x;
	float	DirZ = Input.Dir.y;
	NewPos.x += g_XOffset*DirX;//草在X，Z方向上摆动后的位置
	NewPos.z += g_ZOffset*DirZ;
	OutPos = mul(NewPos,g_matWorldViewProj);
    Out.Pos = OutPos;

    Out.Fade =smoothstep(0.0f,2000.0f,OutPos.z);
    Out.Tex =Input.Tex;
    return Out;
}

//--------------------------------------------------------------
// 象素渲染主函数
//--------------------------------------------------------------
float4 PS_GrassMain(float2 Tex :TEXCOORD0, float Fade :COLOR):COLOR
{
  float4 sceneColor = tex2D(S_Grass,Tex);
  sceneColor = lerp(sceneColor,FogColor,Fade);//通过颜色的混合对远处的草进行雾化
  return sceneColor;
}

//--------------------------------------------------------------
// 效果函数
//--------------------------------------------------------------
technique Grass
{
	pass P0
	{
		ZEnable =true;		//深度测试开启
		ZFunc =less;		//小于当前深度缓冲区的时候渲染
		ZWriteEnable =true;//保持当前的深度值
		cullmode   =none;
		AlphaTestEnable =true;//开启alpha测试
		AlphaRef = 120;
		AlphaFunc=Greater;
		Lighting =false;
		FogEnable =false;
		VertexShader =compile vs_2_0 VS_GrassMain();
		PixelShader  =compile ps_2_0 PS_GrassMain();
	}
}
