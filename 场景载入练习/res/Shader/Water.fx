float4x4	g_matProj;
texture		BasicTexture;
sampler2D	BasicSampler=sampler_state
{
	Texture=<BasicTexture>;		
	MagFilter=Linear;
	MinFilter=Linear;
	MipFilter=Linear;
	
	AddressU=wrap;
	AddressV=wrap;
};
//��������
struct	VS_INPUT
{
	float4	Pos:POSITION;
	
	float2	TextureUV:TEXCOORD0;
};
//�������
struct VS_OUTPUT
{
	float4	Pos:POSITION;
	
	float2	TextureUV:TEXCOORD0;
	
};
//������Ⱦ
VS_OUTPUT	VS(VS_INPUT	input)
{
	VS_OUTPUT Out=(VS_OUTPUT)0;
	Out.Pos=mul(input.Pos,g_matProj);
	Out.TextureUV=input.TextureUV;
	return Out;
};
//������Ⱦ
float4	PS(VS_OUTPUT	input):COLOR
{
	float4	Dest=tex2D(BasicSampler,input.TextureUV*20);
	return	Dest;
};
//����
technique	RenderWater
{
	pass	p0
	{
		VertexShader	=	compile	vs_2_0	VS();
		PixelShader		=	compile	ps_2_0	PS();
	}
};


