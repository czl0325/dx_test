//==============================================================
//					产生动态云
//==============================================================

//--------------------------------------------------------------
// 全局变量
//--------------------------------------------------------------
matrix	g_matWorldViewProj;		//组合变换矩阵
float2	g_cloudTexOffset;		//云彩的移动量
float	g_fFogStart = 0.0f;		//控制云的起始位置和结束位置
float	g_fFogEnd	= 15.0f;
vector	g_fFogColor = {0.85f,0.85f,0.85f,0.0f};

//--------------------------------------------------------------
// 顶点渲染器输入结构-----其中的语义很关键,具体的见龙书的P239页
//--------------------------------------------------------------
struct VS_INPUT
{
	vector Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------
// 顶点渲染器输出结构
//--------------------------------------------------------------
struct VS_OUTPUT
{
	vector Pos : POSITION;
	float2 Tex : TEXCOORD0;		//天空纹理

	float2 CloudTex : TEXCOORD1;//云彩纹理
	float  Fade : COLOR;		//用于平滑过渡两种颜色
};

//--------------------------------------------------------------
// 顶点渲染器入口函数
//--------------------------------------------------------------
VS_OUTPUT VS_SkyMain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(input.Pos,g_matWorldViewProj);	//计算出各顶点对应的世界坐标
	output.Tex = input.Tex;							//天空球的纹理坐标不去改变它
	
	float2 CloudPos = input.Pos.xz*0.1f;			//根据顶点坐标获取云彩的初始纹理坐标
	output.CloudTex = g_cloudTexOffset.xy + CloudPos;//结合偏移量,得到云彩的当前纹理坐标
	float Distance  = input.Pos.y;

	output.Fade = smoothstep(g_fFogStart,g_fFogEnd,Distance);//smoothstep()可以用来平滑过渡两种颜色
	return output;
};

//--------------------------------------------------------------
// 纹理采样器
//--------------------------------------------------------------
//天空纹理
texture SkyTex;
sampler2D skySampler = sampler_state//使用sampler_state,可以初始化一个采样器对象
{
	//设置采样纹理---关联纹理skyTex
	texture = (SkyTex);
	//设置采样状态
	AddressU = clamp;//设置纹理寻址模式-------clamp为夹取,可以增加边界的吻合,具体见精通155页
	AddressV = clamp;
	Magfilter = linear;//设置纹理过滤方式-------linear为线性纹理过滤,具体见精通155页
	Minfilter = linear;
	Mipfilter = linear;
};

//云彩纹理
texture CloudMap;
sampler2D CloudSampler = sampler_state
{
	texture = (CloudMap);
	AddressU = wrap;//设置纹理寻址模式--------wrap为重叠
	AddressV = wrap;
	Magfilter = linear;
	Minfilter = linear;
	Mipfilter = linear;
};

//--------------------------------------------------------------
// 像素渲染器入口函数
//--------------------------------------------------------------
float4 PS_SkyMain(VS_OUTPUT input) : COLOR
{
	//在像素着色器中使用HLSL的内置函数tex*-相关来给纹理采样
	//它从每张纹理中取得要被映射到像素上的图素
	vector color = tex2D(CloudSampler,input.CloudTex);//参数为采样状态和纹理坐标---对云彩纹理进行采样
	color = lerp(tex2D(skySampler,input.Tex),color,color.a*input.Fade*2.5f);//通过参数三([0,1]),在参数一和参数二之间线性插值
	return lerp(g_fFogColor,color,input.Fade);
}

//--------------------------------------------------------------
// 技术
//--------------------------------------------------------------
technique Sky
{
	pass p0
	{
		//这里的顶点渲染器只是转化世界坐标,所以没必要,
		//加上反而会影响速度,特别是显卡差的,尤其明显
		vertexshader = compile vs_2_0 VS_SkyMain();//编译入口函数
		pixelshader	 = compile ps_2_0 PS_SkyMain();

		ZEnable			= true;
		ZWriteEnable	= true;
	}
}
