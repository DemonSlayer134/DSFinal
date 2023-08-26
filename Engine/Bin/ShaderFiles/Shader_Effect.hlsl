#include "Shader_Defines.hpp"

matrix         g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix         g_ViewMatrixInv, g_ProjMatrixInv;
texture2D      g_Texture;
vector         g_vCamPosition;

texture2D      g_NormalTexture;
texture2D      g_DiffuseTexture;

texture2D      g_BlurTexture;
texture2D      g_BlurXTexture;
texture2D      g_BlurYTexture;
texture2D      g_CombineBlurTexture;
texture2D      g_RadialBlurTexture;
texture2D      g_FinalTexture; // ���۵� �ؽ�ó
texture2D	   g_BloomTextrue; // ��� �ؽ�ó
texture2D	   g_HDRTexture; // ��� + �� �ؽ�ó

matrix         g_matProj;
matrix         g_matViewInv;
matrix         g_matProjInv;
matrix         g_matLightView;

//��
float m_TexW = 1280.f;
float m_TexH = 720.f;

//static const float Weight[13] =
//{
//	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
//	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
//	/* 0.002216,
//	0.008764,
//	0.026995,
//	0.064759,
//	0.120985,
//	0.176033,
//	0.199471,
//	0.176033,
//	0.120985,
//	0.064759,
//	0.026995,
//	0.008764,
//	0.002216,*/
//	0.1122, 0.2706, 0.556, 0.9736, 1.4522, 1.8462, 2, 1.8462, 1.4522, 0.9736, 0.556, 0.2706, 0.1122
//
//
//};
static const float Weight[25] =
{
	0.1122, 0.2706, 0.556, 0.9736, 1.4522, 1.8462,
	1.999, 1.8462, 1.4522, 0.9736, 0.556, 0.2706, 0.1122,
	0.1122, 0.2706, 0.556, 0.9736, 1.4522, 1.8462,
	1.999, 1.8462, 1.4522, 0.9736, 0.556, 0.2706
};
static const float Total = /*6.2108*/ /*1.00000012 *//*12.4216*/32.6324;
//static const float Total = 2.636;
//static const float FinalWeight[7] =
//{
//
//	0.2434, 0.36305, 0.46155,
//	0.5, 0.46155, 0.36305, 0.2434
//};
////static const float FinalTotal = 5.272;
//static const float FinalTotal = 2.636;
static const float FinalWeight[7] =
{

	0.1434, 0.26305, 0.46155,
	0.5, 0.46155, 0.26305, 0.1434
};
//static const float FinalTotal = 5.272;
static const float FinalTotal = 2.236;


sampler HDRSampler = sampler_state
{
	texture = g_FinalTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};
sampler BloomOriSampler = sampler_state
{
	texture = g_BloomTextrue;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};
sampler BloomSampler = sampler_state
{
	texture = g_HDRTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

sampler BlurSampler = sampler_state
{

	texture = g_BlurTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;

};

struct VS_IN
{
	float3      vPosition : POSITION;
	float2      vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	matrix      matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix      matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector      vColor : SV_TARGET0;
};


PS_OUT  PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}


PS_OUT PS_MAIN_DEFERRED_Test(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.f == vDiffuse.a)
		discard;

	
	Out.vColor = vDiffuse;


	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

//==============================Bloom======================================
float fGamma = 0.5f;
PS_OUT PS_Bloom(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	float fBrightColor = 0.f;
	vector vDiffuseColor = g_FinalTexture.Sample(LinearSampler, In.vTexUV);

	vector vFragColor = vDiffuseColor;
	if (vFragColor.a == 0.f)
		discard;

	float fBrightness = dot(vFragColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

	if (fBrightness > 0.99f)
		fBrightColor = vector(vFragColor.rgb, 1.f);

	Out.vColor = fBrightColor;

	if (Out.vColor.a == 0.f)
		discard;


	return Out;
}

PS_OUT PS_Apply_Bloom(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector vHDRColor = g_FinalTexture.Sample(HDRSampler, In.vTexUV); // ���� �ؽ�ó
	vector vBloomOriTex = g_BloomTextrue.Sample(BloomOriSampler, In.vTexUV); // ��� ������ �ؽ�ó
	vector vBloomColor = g_HDRTexture.Sample(BloomSampler, In.vTexUV); // ��� + ������ �ؽ�ó

	if (vHDRColor.a == 0.f)
		discard;

	vector vBloom = pow(pow(abs(vBloomColor), 0.5f) + pow(abs(vBloomOriTex), 0.5f), 1.f / 0.5f);

	vector vOut = (vHDRColor);

	vOut = pow(abs(vOut), 0.5f);
	vBloom = pow(abs(vBloom), 0.5f);

	vOut += vBloom * 1.f;
	Out.vColor = pow(abs(vOut), 1 / 0.5f);

	if (Out.vColor.a == 0.f)
		discard;

	//// ����� ��
	//float2 Direction = _In.vTexUV - float2(0.5f, 0.5f);
	//float3 c = float3(0.0, 0.0, 0.0);
	//float f = 1.0 / 24;

	//for (int i = 0; i < 24; i++)
	//{
	//	c += g_BlurTexture.Sample(BlurSampler, _In.vTexUV - 0.01 * Direction * float(i)) * f;
	//	Out.vColor.rgb = c;
	//}


	return Out;
}

//==============================Blur======================================


PS_OUT PS_BlurX(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2	t = _In.vTexUV;
	float2	uv = 0;

	float	tu = 1.f / m_TexW;

	for (int i = -12; i < 12; ++i)
	{
		uv = t + float2(tu * i, 0);
		Out.vColor += Weight[12 + i] * g_BlurTexture.Sample(BlurSampler, uv);
	}

	Out.vColor /= Total * 0.5f;


	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_BlurY(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2 t = _In.vTexUV;
	float2 uv = 0;

	float tv = 1.f / (m_TexH / 2.f);

	for (int i = -12; i < 12; ++i)
	{
		uv = t + float2(0, tv * i);
		Out.vColor += Weight[12 + i] * g_BlurTexture.Sample(BlurSampler, uv);
	}

	Out.vColor /= Total * 0.5f;


	if (Out.vColor.a == 0.f)
		discard;


	return Out;
}
PS_OUT PS_BlurX_3(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2	t = _In.vTexUV;
	float2	uv = 0;

	float	tu = 1.f / m_TexW;


	for (int i = -3; i < 3; ++i)
	{
		uv = t + float2(tu * i, 0);
		Out.vColor += FinalWeight[3 + i] * g_BlurTexture.Sample(BlurSampler, uv);
	}


	Out.vColor /= FinalTotal;
	/*if (Out.vColor.a == 0.f)
		discard;*/

	return Out;
}
PS_OUT PS_BlurY_3(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2 t = _In.vTexUV;
	float2 uv = 0;

	float tv = 1.f / (m_TexH / 2.f);


	for (int i = -3; i < 3; ++i)
	{
		uv = t + float2(0, tv * i);
		Out.vColor += FinalWeight[3 + i] * g_BlurTexture.Sample(BlurSampler, uv);
	}

	Out.vColor /= FinalTotal;

	/*if (Out.vColor.a == 0.f)
		discard;*/

	return Out;
}
PS_OUT PS_Combine_Blur(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector      vFinal = g_Texture.Sample(LinearSampler, In.vTexUV);
	vector      vBlurX = g_BlurXTexture.Sample(LinearSampler, In.vTexUV);
	vector      vBlurY = g_BlurYTexture.Sample(LinearSampler, In.vTexUV);

	/*if (vFinal.a < 0.2f)
		discard;*/


	Out.vColor = ((vFinal + vBlurX + vBlurY) / 3.f);

	if (Out.vColor.a == 0.0f)
		discard;


	return Out;
}

PS_OUT PS_RadialBlur(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector vFinalColor = g_RadialBlurTexture.Sample(LinearSampler, In.vTexUV); // ���� �ؽ�ó

	if (vFinalColor.a == 0.f)
		discard;

	//if (true == g_bRadialBlur)
	//{
	//	float2 Direction = In.vTexUV - float2(0.5f, 0.5f);
	//	float3 c = float3(0.0, 0.0, 0.0);
	//	float f = 1.0 / 12;

	//	for (int i = 0; i < 12; i++)
	//	{
	//		c += g_RadialBlurTexture.Sample(LinearClampSampler, In.vTexUV - 0.01 * Direction * float(i)) * f;
	//		Out.vColor.rgb = c;
	//	}
	//	
	//	Out.vColor.a = vFinalColor.a;
	//	/*if (Out.vColor.a == 0.f)
	//		discard;*/
	//}
	//else
		Out.vColor = vFinalColor;
		Out.vColor.a = 1.f;
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Debug
	{//0
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Deferred_Test
	{//1
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_Test();
	}


	pass BlurX
	{//2
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurX();
	}

	pass BlurY
	{//3
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurY();
	}

	pass CombineBlur
	{//4
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Combine_Blur();
	}

	pass BlurX_3
	{//5
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurX_3();
	}
	pass BlurY_3
	{//6
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurY_3();
	}
	pass ExportBloom
	{//7
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Bloom();
	}
	pass Bloom
	{//8
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Apply_Bloom();
	}
	pass RadialBlur
	{//8
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RadialBlur();
	}
}