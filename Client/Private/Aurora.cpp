#include "pch.h"
#include "..\Public\Aurora.h"

#include "GameInstance.h"
#include "Character.h"

CAurora::CAurora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CAurora::CAurora(const CAurora& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CAurora::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAurora::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Reset_Data();

	m_iFrame = Random::Generate_Int(0, 35);

	return S_OK;
}

void CAurora::Tick(_double TimeDelta) 
{
	switch (m_EffectDesc.eGroup)
	{
	case GROUP_0:
		if (!m_EffectDesc.pGameObject->Get_IsAuroraOn(GROUP_0))
		{
			m_fAlpha -= 0.4f * (_float)TimeDelta;
			if (m_fAlpha < 0.f)
				m_fAlpha = 0.f;
			return;
		}
		break;
	case GROUP_1:
		if (!m_EffectDesc.pGameObject->Get_IsAuroraOn(GROUP_1))
		{
			m_fAlpha = 0.f;
			return;
		}
	}

	if (m_isSpreadDead)
		m_fAlpha -= 0.1f * (_float)TimeDelta;
	else
		m_fAlpha += 0.5f * (_float)TimeDelta;

	if (m_fAlpha < 0.f)
	{
		m_fAlpha = 0.f;
		m_isDead = true;
	}

	
	if (m_fAlpha > m_fMaxAlpha)
		m_fAlpha = m_fMaxAlpha;

	__super::Tick(TimeDelta);

	m_fAccX += (_float)m_dSpeedX * (_float)TimeDelta;
	m_fAccY += (_float)m_dSpeedY * (_float)TimeDelta;
	m_fAccZ += (_float)m_dSpeedZ * (_float)TimeDelta;

	m_vSize.x += m_fSizeSpeedX * (_float)TimeDelta;
	m_vSize.y += m_fSizeSpeedY * (_float)TimeDelta;

	m_pTransformCom->Scaling(m_vSize);

	Update_Frame(TimeDelta);

	if (TYPE_LOCAL == m_EffectDesc.eType)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTransform->Get_State(CTransform::STATE_POSITION)
			+ XMVectorSet(m_fPlusX, m_fPlusY + m_fAccY, m_fPlusZ, 0.f));
	}
	else if (TYPE_WORLD == m_EffectDesc.eType)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Convert::ToVector(m_vInitialPos) + XMVectorSet(m_fAccX, m_fAccY, m_fAccZ, 0.f));
	}
}

void CAurora::LateTick(_double TimeDelta)
{
	if (m_fAlpha == 0.f)
		return;
	
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	_vector m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());

	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
		return;

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

HRESULT CAurora::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(29);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CAurora::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_e_cmn_Smoke023"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_RampTexture */
	switch (m_EffectDesc.eColor)
	{
	case COLOR_BLUE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ramp07"),
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
			return E_FAIL;
		break;
	case COLOR_RED:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ramp_ef_en300900110_red #3417236"),
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
			return E_FAIL;
		break;
	case COLOR_PURPLE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ramp_ef_fire_torch_purple01 #1497231"),
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
			return E_FAIL;
		break;
	case COLOR_CHACRA:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_EFF_GMS_ChakraClaw_B_01"),
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
			return E_FAIL;
		break;
	case COLOR_SKY:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_EFF_GMS_AmeVillage_Sea_02_BC"),
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
			return E_FAIL;
		break;
	case COLOR_YELLOW:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ramp_ef_yellow #2844303"),
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	
	return S_OK;
}

HRESULT CAurora::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pRampTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_RampTexture", 0)))
		return E_FAIL;

	_float fUVRatioX = (1.f / 6.f) * (m_iFrame % 6);
	_float fUVRatioY = (1.f / 6.f) * (m_iFrame / 6);

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioX", &fUVRatioX, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioY", &fUVRatioY, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vColor", &m_vColor, sizeof _float3)))
		return E_FAIL;
	
	Safe_Release(pGameInstance);

	return S_OK;
}

void CAurora::Update_Frame(_double TimeDelta)
{
	m_FrameAccTime += TimeDelta;

	if (m_FrameAccTime >= m_dFrameSpeed)
	{
		++m_iFrame;
		m_FrameAccTime = 0.0;
		if (m_iFrame >= m_iNumX * m_iNumY - 1)
		{
			if (m_EffectDesc.eGroup != 2)
				Reset_Data();
			else
			{
				m_iFrame = m_iNumX * m_iNumY - 1;
				m_isSpreadDead = true;
			}
		}
	}
	
	
}

void CAurora::Reset_Data()
{
	m_dFrameSpeed = Random::Generate_Float(0.04f, 0.06f);

	m_dSpeedY = (_double)Random::Generate_Float(0.1f, 0.3f);

	m_fAccY = 0.f;

	m_iFrame = Random::Generate_Int(8, 10);

	m_fAlpha = 0.f;

	if (m_EffectDesc.eCharacter == CHARACTER_KYOGAI)
	{
		m_fPlusX = Random::Generate_Float(-1.f, 1.0f);
		m_fPlusY = Random::Generate_Float(0.f, 2.5f);
		m_fPlusZ = Random::Generate_Float(-1.0f, 1.0f);

		m_fMaxAlpha = 0.2f;

		m_vSize = { Random::Generate_Float(1.0f, 1.4f),Random::Generate_Float(1.0f, 1.4f),1.f };
		m_fSizeSpeedX = Random::Generate_Float(1.5f, 1.8f);
		m_fSizeSpeedY = Random::Generate_Float(1.5f, 1.7f);

		
	}
	else if (m_EffectDesc.eCharacter == CHARACTER_AKAZA)
	{
		m_fPlusX = Random::Generate_Float(-0.3f, 0.3f);
		m_fPlusY = Random::Generate_Float(0.f, 1.3f);
		m_fPlusZ = Random::Generate_Float(-0.3f, 0.3f);

		m_fMaxAlpha = 0.2f;

		m_vSize = { Random::Generate_Float(0.9f, 1.4f),Random::Generate_Float(0.9f, 1.4f),1.f };
		m_fSizeSpeedX = Random::Generate_Float(0.7f, 0.9f);
		m_fSizeSpeedY = Random::Generate_Float(0.7f, 0.9f);
	}
	else if (m_EffectDesc.eCharacter == CHARACTER_TANJIRO)
	{
		m_fPlusX = Random::Generate_Float(-0.3f, 0.3f);
		m_fPlusY = Random::Generate_Float(0.f, 1.f);
		m_fPlusZ = Random::Generate_Float(-0.3f, 0.3f);

		m_fMaxAlpha = 0.1f;

		m_vSize = { Random::Generate_Float(0.9f, 1.4f),Random::Generate_Float(0.9f, 1.4f),1.f };
		m_fSizeSpeedX = Random::Generate_Float(0.7f, 0.9f);
		m_fSizeSpeedY = Random::Generate_Float(0.5f, 0.7f);

	}
	else if (m_EffectDesc.eCharacter == CHARACTER_ZENITSU)
	{
		m_dSpeedY = (_double)Random::Generate_Float(0.1f, 0.2f);

		m_fPlusX = Random::Generate_Float(-0.2f, 0.2f);
		m_fPlusY = Random::Generate_Float(0.3f, 0.8f);
		m_fPlusZ = Random::Generate_Float(-0.2f, 0.2f);

		m_fMaxAlpha = 0.1f;

		m_vSize = { Random::Generate_Float(0.9f, 1.4f),Random::Generate_Float(0.9f, 1.4f),1.f };
		m_fSizeSpeedX = Random::Generate_Float(0.6f, 0.8f);
		m_fSizeSpeedY = Random::Generate_Float(0.5f, 0.7f);

	}

	if (m_EffectDesc.eType == TYPE_WORLD)
	{

		m_fPlusX = Random::Generate_Float(-0.3f, 0.3f);
		m_fPlusY = Random::Generate_Float(0.f, 1.3f);
		m_fPlusZ = Random::Generate_Float(-0.3f, 0.3f);

		m_fMaxAlpha = 0.2f;
		m_fAlpha = m_fMaxAlpha;

		m_vSize = { Random::Generate_Float(0.9f, 1.4f),Random::Generate_Float(0.9f, 1.4f),1.f };
		m_fSizeSpeedX = Random::Generate_Float(2.7f, 3.0f);
		m_fSizeSpeedY = Random::Generate_Float(2.7f, 3.0f);

		m_dFrameSpeed = Random::Generate_Float(0.018f, 0.022f);

		XMStoreFloat4(&m_vInitialPos, m_EffectDesc.pTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(m_fPlusX, m_fPlusY, m_fPlusZ, 0.f));

		m_dSpeedX = Random::Generate_Float(-3.f, 3.f);
		m_dSpeedY = Random::Generate_Float(-1.f, 3.f);
		m_dSpeedZ = Random::Generate_Float(-3.f, 3.f);
	}
}

CAurora* CAurora::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAurora* pInstance = new CAurora(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAurora");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CAurora::Clone(void* pArg)
{
	CAurora* pInstance = new CAurora(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAurora");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAurora::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRampTextureCom);
}
