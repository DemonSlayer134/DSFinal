
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4		g_vCamPosition;

texture2D	g_DiffuseTexture;
texture2D   g_NormalTexture;
texture2D	g_RampTexture;
texture2D	g_EmissiveTexture;


float		g_fFar = 400.f;

float			g_fTimeAcc;
float2			g_vPanningSpeed;
float			g_fAlpha;
float			g_fUVRatio;
float			g_fDiffuseRatio;
float2			g_vCustomUV;

bool			g_bLiarColor;

struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV	: TEXCOORD0;
	float3		vTangent	: TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

VS_OUT VS_Main(VS_IN _In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(_In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(_In.vNormal, 0.f), g_WorldMatrix));
	// ���Ƽ���
	Out.vTangent = normalize(mul(vector(_In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
	// ���ӱ���
	Out.vTexUV = _In.vTexUV;
	Out.vWorldPos = mul(vector(_In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vEmissive : SV_TARGET3;
	vector		vDiffuse_Cha : SV_TARGET4;
};

struct PS_NONDEFERRED
{
	vector		vColor : SV_TARGET0;
};

PS_OUT  PS_Main(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, _In.vTexUV);
	vector  vEmissive = g_EmissiveTexture.Sample(LinearSampler, _In.vTexUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse_Cha = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(_In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(_In.vProjPos.w / g_fFar, _In.vProjPos.z / _In.vProjPos.w, _In.vProjPos.w / g_fFar, 0.f);
	Out.vEmissive = vEmissive;

	return Out;
};

PS_OUT  PS_Main_NormalTexture(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, _In.vTexUV);

	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, _In.vTexUV);

	vector  vEmissive = g_EmissiveTexture.Sample(LinearSampler, _In.vTexUV);

	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(_In.vTangent.xyz, _In.vBinormal.xyz, _In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;

	// In.vNormal xyz������ -1 ~ 1
	// Out.vNormal ������� �� �ִ� xyz���� 0 ~ 1
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(_In.vProjPos.w / g_fFar, _In.vProjPos.z / _In.vProjPos.w, 0.f, 0.f);
	Out.vEmissive = vEmissive;
	Out.vDiffuse_Cha = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
};

PS_NONDEFERRED  PS_Blend(PS_IN _In)
{
	PS_NONDEFERRED	Out = (PS_NONDEFERRED)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, _In.vTexUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vColor = vMtrlDiffuse;

	return Out;
};

PS_OUT  PS_Ramp(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, _In.vTexUV);
	float fRamp = vMtrlDiffuse.r;
	float2 vGradientUV = float2(fRamp, _In.vTexUV.y);
	vector vMtrlRamp = g_RampTexture.Sample(LinearSampler, vGradientUV);

	if (vMtrlDiffuse.a < 0.9f)
		discard;

	Out.vDiffuse = vMtrlRamp;
	//Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(_In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(_In.vProjPos.w / g_fFar, _In.vProjPos.z / _In.vProjPos.w, 0.f, 0.f);

	return Out;
};

PS_OUT  PS_BLOOD(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	/* �� ��־Ƹ� �����ϱ����� ���ý����̽�(x:Tangent, y:biNormal, z:Normal)�� ���ǵǾ��ִ� �����̴�. */
	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	if (vMtrlDiffuse.r < 0.3f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	// In.vNormal xyz������ -1 ~ 1
	// Out.vNormal ������� �� �ִ� xyz���� 0 ~ 1
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT  PS_JUMPEFFECT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a = vMtrlDiffuse.r;

	Out.vDiffuse.b += 0.5f;


	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_SMELL(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a = vMtrlDiffuse.r * g_fAlpha;
	Out.vDiffuse.r *= 5.5f;
	Out.vDiffuse.g = 0.f;
	Out.vDiffuse.b = 0.f;



	if (Out.vDiffuse.a < 0.3f)
		discard;

	return Out;
}



PS_OUT  PS_ALPHA(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.rgb *= 0.4f;
	Out.vDiffuse.a *= g_fAlpha;

	return Out;
}

PS_OUT  PS_ALPHA_REAL(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (g_bLiarColor == false)
		Out.vDiffuse = vMtrlDiffuse * g_fDiffuseRatio;

	else
	{
		Out.vDiffuse = vMtrlDiffuse * g_fDiffuseRatio;
		Out.vDiffuse.b = Out.vDiffuse.r;
		Out.vDiffuse.r = 0.f;
	}

		Out.vDiffuse.a *= g_fAlpha;
	
	return Out;
}


PS_OUT  PS_WIND(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	if (vMtrlDiffuse.r < 0.3f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = vMtrlDiffuse.r * 0.07f;


	return Out;
}

PS_OUT  PS_FOG(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * g_fUVRatio);
	vDiffuse.a = vDiffuse.r * 0.04f;
	vDiffuse.b *= 3.f;
		
	Out.vDiffuse = vDiffuse;

	return Out;
}

PS_OUT  PS_REDRECT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse * g_fDiffuseRatio;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	if (Out.vDiffuse.g > 0.5f && Out.vDiffuse.b > 0.5f)
		discard;

	Out.vDiffuse.a *= g_fAlpha * 0.7f;

	return Out;
}

PS_NONDEFERRED  PS_ALERTMESH(PS_IN _In)
{
	PS_NONDEFERRED	Out = (PS_NONDEFERRED)0;

	_In.vTexUV.x += g_vCustomUV.x;
	_In.vTexUV.y += g_vCustomUV.y;

	float UVX = _In.vTexUV.x;
	float UVY = _In.vTexUV.y;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));
	float fRamp = vMtrlDiffuse.r;
	if (0.98f < fRamp)
		fRamp = 0.98f;
	float2 vGradientUV = float2(fRamp, _In.vTexUV.y);
	vector vMtrlRamp = g_RampTexture.Sample(LinearSampler, vGradientUV);

	if (0.9f > vMtrlDiffuse.a)
		discard;

	Out.vColor = vMtrlRamp;
	Out.vColor.a = vMtrlDiffuse.r * g_fAlpha;
	
	if (0.1f > Out.vColor.a)
		discard;

	return Out;
};

PS_NONDEFERRED  PS_HANDAURA(PS_IN _In)
{
	PS_NONDEFERRED	Out = (PS_NONDEFERRED)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, _In.vTexUV);

	Out.vColor = vMtrlDiffuse * 2.f;
	Out.vColor.a = vMtrlDiffuse.b * g_fAlpha;

	if (0.1f > Out.vColor.a)
		discard;

	return Out;
};

technique11 DefaultTechnique
{
	pass General // 0
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main();
	}

	pass General2 // 1
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_NormalTexture();
	}

	pass Blend // 2
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Blend();
	}

	pass Ramp // 3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Ramp();
	}

	pass Blood // 4
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOD();
	}

	pass JumpEffect // 5
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlendingOne, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_JUMPEFFECT();
	}

	pass Smell // 6
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SMELL();
	}

	pass Alpha // 7
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA();
	}

	pass Wind // 8
	{
		SetRasterizerState(RS_CULL_CW);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIND();
	}

	pass Fog // 9
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FOG();
	}

	pass Alpha_Real // 10
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlendingOne, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA_REAL();
	}

	pass RedRect // 11
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_REDRECT();
	}

	pass AlertMesh // 12
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALERTMESH();
	}

	pass HandAura // 13
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HANDAURA();
	}
};



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
