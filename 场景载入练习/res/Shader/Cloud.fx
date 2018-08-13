//==============================================================
//					������̬��
//==============================================================

//--------------------------------------------------------------
// ȫ�ֱ���
//--------------------------------------------------------------
matrix	g_matWorldViewProj;		//��ϱ任����
float2	g_cloudTexOffset;		//�Ʋʵ��ƶ���
float	g_fFogStart = 0.0f;		//�����Ƶ���ʼλ�úͽ���λ��
float	g_fFogEnd	= 15.0f;
vector	g_fFogColor = {0.85f,0.85f,0.85f,0.0f};

//--------------------------------------------------------------
// ������Ⱦ������ṹ-----���е�����ܹؼ�,����ļ������P239ҳ
//--------------------------------------------------------------
struct VS_INPUT
{
	vector Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------
// ������Ⱦ������ṹ
//--------------------------------------------------------------
struct VS_OUTPUT
{
	vector Pos : POSITION;
	float2 Tex : TEXCOORD0;		//�������

	float2 CloudTex : TEXCOORD1;//�Ʋ�����
	float  Fade : COLOR;		//����ƽ������������ɫ
};

//--------------------------------------------------------------
// ������Ⱦ����ں���
//--------------------------------------------------------------
VS_OUTPUT VS_SkyMain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(input.Pos,g_matWorldViewProj);	//������������Ӧ����������
	output.Tex = input.Tex;							//�������������겻ȥ�ı���
	
	float2 CloudPos = input.Pos.xz*0.1f;			//���ݶ��������ȡ�Ʋʵĳ�ʼ��������
	output.CloudTex = g_cloudTexOffset.xy + CloudPos;//���ƫ����,�õ��Ʋʵĵ�ǰ��������
	float Distance  = input.Pos.y;

	output.Fade = smoothstep(g_fFogStart,g_fFogEnd,Distance);//smoothstep()��������ƽ������������ɫ
	return output;
};

//--------------------------------------------------------------
// ���������
//--------------------------------------------------------------
//�������
texture SkyTex;
sampler2D skySampler = sampler_state//ʹ��sampler_state,���Գ�ʼ��һ������������
{
	//���ò�������---��������skyTex
	texture = (SkyTex);
	//���ò���״̬
	AddressU = clamp;//��������Ѱַģʽ-------clampΪ��ȡ,�������ӱ߽���Ǻ�,�������ͨ155ҳ
	AddressV = clamp;
	Magfilter = linear;//����������˷�ʽ-------linearΪ�����������,�������ͨ155ҳ
	Minfilter = linear;
	Mipfilter = linear;
};

//�Ʋ�����
texture CloudMap;
sampler2D CloudSampler = sampler_state
{
	texture = (CloudMap);
	AddressU = wrap;//��������Ѱַģʽ--------wrapΪ�ص�
	AddressV = wrap;
	Magfilter = linear;
	Minfilter = linear;
	Mipfilter = linear;
};

//--------------------------------------------------------------
// ������Ⱦ����ں���
//--------------------------------------------------------------
float4 PS_SkyMain(VS_OUTPUT input) : COLOR
{
	//��������ɫ����ʹ��HLSL�����ú���tex*-��������������
	//����ÿ��������ȡ��Ҫ��ӳ�䵽�����ϵ�ͼ��
	vector color = tex2D(CloudSampler,input.CloudTex);//����Ϊ����״̬����������---���Ʋ�������в���
	color = lerp(tex2D(skySampler,input.Tex),color,color.a*input.Fade*2.5f);//ͨ��������([0,1]),�ڲ���һ�Ͳ�����֮�����Բ�ֵ
	return lerp(g_fFogColor,color,input.Fade);
}

//--------------------------------------------------------------
// ����
//--------------------------------------------------------------
technique Sky
{
	pass p0
	{
		//����Ķ�����Ⱦ��ֻ��ת����������,����û��Ҫ,
		//���Ϸ�����Ӱ���ٶ�,�ر����Կ����,��������
		vertexshader = compile vs_2_0 VS_SkyMain();//������ں���
		pixelshader	 = compile ps_2_0 PS_SkyMain();

		ZEnable			= true;
		ZWriteEnable	= true;
	}
}
