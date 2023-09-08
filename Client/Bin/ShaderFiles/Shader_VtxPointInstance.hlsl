
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;

texture2D g_Texture;
texture2D g_RampTexture;

float		g_fAlpha;
float2		g_vTexCoord;
int2		g_vSpriteCount;
float		g_fRampMax;

struct VS_IN
{
	float3		vPosition : POSITION;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float4		vColor : COLOR0;
	float2		vPSize : PSIZE;
	float		fAngle : COLOR1;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float4		vColor : COLOR0;
	float2		vPSize : PSIZE;
	float		fAngle : COLOR1;
};

VS_OUT VS_Main(VS_IN _In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix		TransformMatrix = float4x4(_In.vRight, _In.vUp, _In.vLook, _In.vTranslation);
	float4		vLocalPosition = mul(vector(_In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vLocalPosition, g_WorldMatrix);
	Out.vPSize = _In.vPSize;
	Out.vColor = _In.vColor;
	Out.fAngle = _In.fAngle;

	return Out;
};

struct GS_IN
{
	float4		vPosition : POSITION;
	float4		vColor : COLOR0;
	float2		vPSize : PSIZE;
	float		fAngle : COLOR1;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
	float2		vTexCoord : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_Main(point GS_IN _In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT	Out[4];

	float3		vLook = g_vCamPosition.xyz - _In[0].vPosition.xyz;
	float3		vRight = normalize(cross(float3(cos(_In[0].fAngle), sin(_In[0].fAngle), 0.f), vLook)) * _In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * _In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);


	Out[0].vPosition = float4(_In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vColor = _In[0].vColor;
	Out[0].vTexCoord = float2(0.f, 0.f);

	Out[1].vPosition = float4(_In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vColor = _In[0].vColor;
	Out[1].vTexCoord = float2(1.f, 0.f);

	Out[2].vPosition = float4(_In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vColor = _In[0].vColor;
	Out[2].vTexCoord = float2(1.f, 1.f);

	Out[3].vPosition = float4(_In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vColor = _In[0].vColor;
	Out[3].vTexCoord = float2(0.f, 1.f);

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
	float2		vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT  PS_Main(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector vMtrlEffect = g_Texture.Sample(LinearSampler, _In.vTexCoord);
	
	Out.vColor.xyz = saturate(vMtrlEffect.xyz + 0.3f);
	Out.vColor.w = vMtrlEffect.r * g_fAlpha;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
};

PS_OUT  PS_Ramp(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;
	
	vector vMtrlEffect = g_Texture.Sample(LinearSampler, _In.vTexCoord);
	float fRamp = vMtrlEffect.r;
	fRamp *= g_fRampMax;
	float2 vGradientUV = float2(fRamp, _In.vTexCoord.y);
	vector vMtrlRamp = g_RampTexture.Sample(LinearSampler, vGradientUV);

	Out.vColor.xyz = saturate(vMtrlRamp.xyz + 0.3f);
	Out.vColor.w = vMtrlEffect.r * g_fAlpha;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
};

PS_OUT  PS_Sprite(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2 vTexUV = float2(g_vTexCoord.x + (_In.vTexCoord.x * (1.f / g_vSpriteCount.x)), g_vTexCoord.y + (_In.vTexCoord.y * (1.f / g_vSpriteCount.y)));

	vector vMtrlEffect = g_Texture.Sample(LinearSampler, vTexUV);

	Out.vColor.xyz = saturate(vMtrlEffect.xyz + 0.3f);
	Out.vColor.w = vMtrlEffect.r * g_fAlpha;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
};

PS_OUT  PS_Sprite_Ramp(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2 vTexUV = float2(g_vTexCoord.x + (_In.vTexCoord.x * (1.f / g_vSpriteCount.x)), g_vTexCoord.y + (_In.vTexCoord.y * (1.f / g_vSpriteCount.y)));

	vector vMtrlEffect = g_Texture.Sample(LinearSampler, vTexUV);
	float fRamp = vMtrlEffect.r;
	fRamp *= g_fRampMax;
	float2 vGradientUV = float2(fRamp, _In.vTexCoord.y);
	vector vMtrlRamp = g_RampTexture.Sample(LinearSampler, vGradientUV);

	Out.vColor.xyz = saturate(vMtrlRamp.xyz + 0.3f);
	Out.vColor.w = vMtrlEffect.r * g_fAlpha;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
};

technique11 DefaultTechnique
{
	pass General
	{//0
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = compile gs_5_0 GS_Main();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main();
	}

	pass Ramp
	{//1
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = compile gs_5_0 GS_Main();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Ramp();
	}

	pass Sprite
	{//2
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = compile gs_5_0 GS_Main();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Sprite();
	}

	pass Sprite_Ramp
	{//3
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = compile gs_5_0 GS_Main();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Sprite_Ramp();
	}
}



////======================================���̴� �����=============================================
//// ���̴� = ����� ���� ������ ������ ����
//// DX9 = ������� ������ ������ ����
//
//// ��ġ�� �˾Ƽ� ���ִ� ������ ������������ ������� �ʰ�
//// ���� ������ ������������ ����� �����ϸ鼭
//// ��� �߰� �� ����ȭ�� �ϴ°� = ���̴��� ����Ѵ�
//
//// ���̴��� �����Ǵ� ����
//// ���̴� �ڵ带 ��üȭ���Ѿ� �Ѵ� - ���̴� Ŭ���� ������Ʈ�� ���� ó���� ��
//// ID3DX11Effect* pEffect ��ü - ���̴� Ŭ������ ����������
//// pEffect���� Apply() - ���̴��� �׸����̴�.
//
//// m_pContext->DrawIndexed();
//// �ε��� ������ ù��° �ε����� ������ ��������
//// �� ������ ���ڷ� ���ؽ� ���̴� �Լ��� ȣ���Ѵ� (VS_Main())
//// �Լ� ȣ���� ������, ����Ʈ ��ȯ�� ����, ����Ʈ ��ȯ���� ������
//// ���� �ε����� ������ �����ͼ� �ٽ� ������ �����Ѵ�.
//// 3��° �������� ������ ������ (�ﰢ���� �׸� �� �ְ� �Ǹ�)
//// �����Ͷ���� �����Ѵ�. - ������ ������ ������� �ȼ��� �����س����� ����
//
////���̴� ������ /fx (ȿ��) ���̴��̱� ������ technique11�� �����Ѵ�
////� �Լ��� ȣ��� ������ ���̴� ���� �ȿ��� �˷��ִ°�
////�ٸ� ������ ��쿡�� ���̴� ���Ͽ��� �Լ��� �����, Ŭ���̾�Ʈ���� � �Լ��� ȣ������ �������ش�.
////������ �ϵ��� �´� ���̴� �������� ���̴��� ������ �� �� �ټ��� technique11�� �����Ѵ�
//
//struct VS_IN
//{
//	// Ŭ�󿡼� �����ִ� ������ ���� ����ü - ������ ���� ���� �ٸ� ���̴� ������ ���� ��
//	// ex)VTXTEX
//	// : POSITION - �ø�ƽ - D3DDECLUSAGE ����ü�� _POSITION, _NORMAL ������
//	// ��������� �ø�ƽ�� �������� ���� ����Ұ�
//	float3	vPosition : POSITION;
//	float2	vTexCoord : TEXCOORD0;
//};
//
//struct VS_OUT
//{
//	// ���� �ڵ忡���� VS_Main()���� ����, ��, ���� ��ȯ�� ��� �����Ѵ�
//	// ���� ����� ���� �ڿ� w��������� �ؾ� ������ȯ�� �Ϸ�ȴ�.
//	// VS_IN ������ Position�� float3�̶� w�� ����
//	// ������ȯ�� �Ϸ��� w���� �־�� w�����⸦ �� �� �ֱ⶧���� float4�� �������� �Ѵ�
//	// IN�� OUT�� �ڷ����� �޶����� ������ IN, OUT�� ���� �����ߴ�.
//
//
//	float4 vPosition : SV_POSITION;
//	float2 vTexCoord : TEXCOORD0;
//};
//
//VS_OUT VS_Main(VS_IN _In)
//{
//	//���� �ϳ��� �޾Ƽ� ������ ����� ������ ���� ���̴�
//
//	//_In.vPosition * ���� * �� * ����
//	// mul() - ����� ���� �� ���
//	// DX9���� ����ߴ� TransformCoord�� �ٸ��� �ڵ����� z�����⸦ �������� �ʴ´�.
//	// �ܼ��� ���ϱ��� ���
//}
//
////������ ����, ��, ���� ����� ��� ���ϰ� �Լ��� ����Ǹ�
//// �ڵ����� z �����⸦ �����ؼ� ���� ��ȯ�� �Ϸ��Ѵ�.
//// ���� ����Ʈ ��ȯ�� �Ѵ�.
//
////�����Ͷ�����
//struct PS_IN
//{
//	// �ȼ� ���̴����� ����ϴ� ����ü
//	// �ȼ��� ������ ������� ��������� ������ VS_OUT���� ����ص� ����������
//	// �������� ���� PS_IN���� �����
//
//	//�ȼ� ���̵� �ܰ迡���� �̹� �ȼ��� ��ġ�� �������ֱ� ������ Position�� ��� �ʿ����� �ʴ� - �����ص� ����
//	float4 vPosition : SV_POSITION;
//	float2 vTexCoord : TEXCOORD0;
//};
//
//struct PS_OUT
//{
//	//�ȼ� ���̴��� OUT����ü
//	//�ȼ� ���̴��� �ȼ��� ���� �����ϱ� ������ ���� ���� float4(vector)�� ������ �ȴ�.
//	//TARGET�� 0~7���� ����, �� ��ȣ�� ����Ÿ�ٿ� ��������� ������ �׸���
//	vector vColor : SV_TARGET0;
//	vector vShade : SV_TARGET1;
//};
//
//
//// vector PS_Main(PS_IN _In) : SV_TARGET0 ���� ����ص� ������ OUT ����ü�� ���� ����� ����Ұ�(������ ���)
//PS_OUT PS_Main(PS_IN _In)
//{
//	// �ȼ��� �������� ���� �����Ѵ�.
//	// �ȼ��� �� ��ŭ ����Ǳ� ������ �ִ��� ������ ����°� ����
//
//	PS_OUT vOut = (PS_OUT)0;	//vOut�� 0���� �ʱ�ȭ
//
//
//	return vOut;
//}
//
//technique11 DefaultTechnique
//{
//	//������� ���� ���̴��� ������� ������ �ٸ� ���̴� ���� ��
//	//������� ���� ���̴��� �����ȴ�.
//	//���� ������� �ʵ��� ����� �� ���̴�. (������ ���)
//
//	//VertexShader = compile vs_5_0 VS_Main();
//	// ���ؽ� ���̴� = vs_5_0�������� �����ϰ�, VS_Main() �Լ��� ȣ���Ѵ�
//	// VS_Main()�Լ��� �������� ������ ������ �߻��Ѵ�
//	//GeometryShader = NULL;
//	//HullShader = NULL;
//	//DomainShader = NULL;
//	//PixelShader = compile vs_5_0 PS_Main();
//	// �ȼ� ���̴� = vs_5_0�������� �����ϰ�, PS_Main() �Լ��� ȣ���Ѵ�
//
//
//
//	//pass - ����� ĸ��ȭ
//
//	//0�� ���� - �Ϲ� - �׸���, �븻��, �𵨸� ���� ����
//	//1�� ���� - ���� - ����������� ��ȿ������ ����
//	//���� ���·� ���� ������ ���� ���¸� �ٸ��� �� ��쿡 ����ϱ� ����
//	//[���� ����] -- �߿�
//	pass General
//	{
//		VertexShader = compile vs_5_0 VS_Main();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile vs_5_0 PS_Main();
//	};
//
//	pass Cloaking
//	{
//		VertexShader = compile vs_5_0 VS_Main();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile vs_5_0 PS_Main();
//	};
//};
