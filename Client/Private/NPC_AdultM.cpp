#include "pch.h"
#include "..\Public\NPC_AdultM.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"
#include "Fade_Manager.h"

CNPC_AdultM::CNPC_AdultM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNPC(pDevice, pContext)
{
}

CNPC_AdultM::CNPC_AdultM(const CNPC_AdultM& rhs)
	: CNPC(rhs)
{
}

HRESULT CNPC_AdultM::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_AdultM::Initialize(void* pArg)
{

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModelCom->Set_Animation(ANIM_STNAD_NORMAL);

	if (FAILED(Read_Animation_Control_File("NPC_AdultM.bin")))
	{
		MSG_BOX("Failed to AnimData Read : NPC_AdultM");
		return E_FAIL;
	}
	
	m_pTransformCom->Set_Look(m_CharacterDesc.NPCDesc.DirNPC);


	if (NPC_QUEST == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_STNAD_NORMAL);
	else if (NPC_STAND == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_STAND_GUNBANG);
	else if (NPC_TALK == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_SPEAK_YUPJARI);
	else if (NPC_LISTEN == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_SIT_LISTEN_PALZZANG);
	else if (NPC_SIT == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_SIT_NORMAL);
	else if (NPC_SITTALK == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_SIT_SPEAK_YUPJARI);
	else if (NPC_DOWN == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_WALLSTAY);
	else if (NPC_DOWNTALK == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_SLEEP);
	else if (NPC_WALK == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_WALK_NORMAL);
	else if (NPC_WALKTALK == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_WALK_PALZZANG);
	else if (NPC_WORK == m_CharacterDesc.NPCDesc.eNPC)
		m_pModelCom->Set_Animation(ANIM_WORK);



	return S_OK;
}

void CNPC_AdultM::Tick(_double dTimeDelta)
{
	if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
	{
		Find_Section();

		if (m_iPlayer_Section == m_CharacterDesc.NPCDesc.iSection || m_iPlayer_Section_Sub == m_CharacterDesc.NPCDesc.iSection)
		{
			__super::Tick(dTimeDelta);

			if (true == m_isDead)
				return;

			if (m_isTalking)
			{

				m_CharacterDesc.NPCDesc.eNPC = NPC_TALK;
			}

			Animation_Control(dTimeDelta);

			//�ִϸ��̼� ó��
			m_pModelCom->Play_Animation(dTimeDelta);
			RootAnimation(dTimeDelta);

			//�̺�Ʈ ��
			EventCall_Control(dTimeDelta);

		}
	}
}

void CNPC_AdultM::LateTick(_double dTimeDelta)
{
	if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
	{
		if (m_iPlayer_Section == m_CharacterDesc.NPCDesc.iSection || m_iPlayer_Section_Sub == m_CharacterDesc.NPCDesc.iSection)
		{
			__super::LateTick(dTimeDelta);

			Gravity(dTimeDelta);
		}
	}
#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CNPC_AdultM::Render()
{
	if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
	{
		if (m_iPlayer_Section == m_CharacterDesc.NPCDesc.iSection || m_iPlayer_Section_Sub == m_CharacterDesc.NPCDesc.iSection)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;

			if (FAILED(SetUp_ShaderResources()))
				return E_FAIL;



			_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
			//Outline Render
			for (m_iMeshNum = 0; m_iMeshNum < iNumMeshes; m_iMeshNum++)
			{
				if (FAILED(m_pModelCom->Bind_ShaderResource(m_iMeshNum, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(m_iMeshNum, m_pShaderCom, "g_BoneMatrices")))
					return E_FAIL;

				if (m_iMeshNum == 2)
					m_pShaderCom->Begin(2);
				else
					m_pShaderCom->Begin(1);

				m_pModelCom->Render(m_iMeshNum);
			}
			// Default Render
			for (_uint i = 0; i < iNumMeshes; i++)
			{
				if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
					return E_FAIL;

				m_pShaderCom->Begin(0);

				m_pModelCom->Render(i);
			}

		}
	}
	return S_OK;
}

HRESULT CNPC_AdultM::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector	vLightEye = XMVectorSet(130.f, 10.f, 140.f, 1.f);
	_vector	vLightAt = XMVectorSet(60.f, 0.f, 60.f, 1.f);
	_vector	vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);


	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix",
		&FloatLightViewMatrix)))
		return E_FAIL;

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), _float(1280) / _float(720), 0.2f, 300.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix",
		&FloatLightProjMatrix)))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;


		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	return S_OK;
}


void CNPC_AdultM::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
#pragma region Animation_0
		/*if (ANIM_ATK_COMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}

			CEffectPlayer::Get_Instance()->Play("hjd", m_pTransformCom);
		}*/
		
#pragma endregion
		m_iEvent_Index++;
	}
}

void CNPC_AdultM::Animation_Control(_double dTimeDelta)
{
	Set_Height();
	if (NPC_QUEST == m_CharacterDesc.NPCDesc.eNPC)
	{
		Animation_Control_Quest(dTimeDelta);
	}
	else
	{
		if(NPC_SIT != m_CharacterDesc.NPCDesc.eNPC && NPC_SITTALK != m_CharacterDesc.NPCDesc.eNPC
			&& NPC_DOWN != m_CharacterDesc.NPCDesc.eNPC && NPC_DOWNTALK != m_CharacterDesc.NPCDesc.eNPC)
			//Sway(dTimeDelta);

		if (NPC_STAND == m_CharacterDesc.NPCDesc.eNPC || NPC_TALK == m_CharacterDesc.NPCDesc.eNPC
			|| NPC_SIT == m_CharacterDesc.NPCDesc.eNPC || NPC_SITTALK == m_CharacterDesc.NPCDesc.eNPC
			|| NPC_DOWN == m_CharacterDesc.NPCDesc.eNPC || NPC_DOWNTALK == m_CharacterDesc.NPCDesc.eNPC)
		{
			Animation_Control_Stand(dTimeDelta);
		}
		else if (NPC_WALK == m_CharacterDesc.NPCDesc.eNPC || NPC_WALKTALK == m_CharacterDesc.NPCDesc.eNPC)
		{
			Animation_Control_Walk(dTimeDelta);
		}
	}
}

void CNPC_AdultM::Animation_Control_Quest(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//����ũ
	if (pGameInstance->Get_DIKeyDown(DIK_H))
	{
		m_pModelCom->Set_Animation(ANIM_SPEAK_GUNBANG);
	}
	if (pGameInstance->Get_DIKeyUp(DIK_H))
	{
		m_pModelCom->Set_Animation(20);
	}
	Safe_Release(pGameInstance);
}

void CNPC_AdultM::Animation_Control_Stand(_double dTimeDelta)
{
	if (NPC_STAND == m_CharacterDesc.NPCDesc.eNPC)
	{
		
	}
	else if ( NPC_TALK == m_CharacterDesc.NPCDesc.eNPC)
	{
		if (m_isFirst_Talk)
		{
			m_isFirst_Talk = false;

			if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_WORK)
			{
				m_pModelCom->Set_LinearDuration(ANIM_SPEAK_PALZZANG, 0.5f);
				m_pModelCom->Set_Animation(ANIM_SPEAK_PALZZANG);
			}
		}

		if (m_isTalking)
		{
			if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_SPEAK_PALZZANG)
			{
				m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);
			}

			if (Calculate_Distance() > 2.0f)
			{
				m_isTalking = false;
				if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_SPEAK_PALZZANG)
				{
					m_pModelCom->Set_LinearDuration(ANIM_STNAD_NORMAL, 0.5f);
					m_pModelCom->Set_Animation(ANIM_STNAD_NORMAL);
				}
			}
		}
	}
	else if (NPC_SIT == m_CharacterDesc.NPCDesc.eNPC)
	{
		
	}
	else if (NPC_SITTALK == m_CharacterDesc.NPCDesc.eNPC)
	{
		
	}
	else if (NPC_DOWN == m_CharacterDesc.NPCDesc.eNPC )
	{
		//m_fLand_Y = 4.85f;
	}
	else if (NPC_DOWNTALK == m_CharacterDesc.NPCDesc.eNPC)
	{
		//m_fLand_Y = 4.85f;
	}
}

void CNPC_AdultM::Animation_Control_Walk(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	//Sway(dTimeDelta);

	//�ȱ� ����
	if (iCurAnim == 31 || iCurAnim == 34)
	{
		Calculate_To_Spot();
	
		m_pTransformCom->Go_Straight(dTimeDelta * 0.2f);
	}
}

void CNPC_AdultM::Sway(_double dTimeDelta)
{
	if (Calculate_Distance() < 0.8f)
	{
		if (m_isFirst_Sway)
		{
			m_isFirst_Sway = false;

			m_pModelCom->Set_Animation(25); //stagger
			m_dCoolTime_Walk = 0.0;
		}
	}
	if (m_pModelCom->Get_iCurrentAnimIndex() == 25) //stagger
	{

		if (0.0f < m_dCoolTime_Walk && m_dCoolTime_Walk < 1.0f)
		{
			_float4 Dir;
			XMStoreFloat4(&Dir, -Calculate_Dir_FixY());
			m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);
			//Go_Dir_Constant(dTimeDelta, 25, 0.15f, Dir); // stagger
		}
	}

	if (m_isFirst_Sway == false)
	{
		m_dCoolTime_Walk += dTimeDelta;

		if (m_CharacterDesc.NPCDesc.eNPC == NPC_WALK || m_CharacterDesc.NPCDesc.eNPC == NPC_WALKTALK)
		{
			if (m_dCoolTime_Walk > 5.0f)
			{
				m_dCoolTime_Walk = 0.0;
				m_isFirst_Sway = true;

				m_pModelCom->Set_Animation(34);
				
			}
		}
		else
		{
			if (m_dCoolTime_Walk > 1.85f)
			{
				m_dCoolTime_Walk = 0.0;
				m_isFirst_Sway = true;

				m_pModelCom->Set_Animation(ANIM_STNAD_NORMAL);
			}
		}
	}
}

HRESULT CNPC_AdultM::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC_AdultM"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CNPC_AdultM");
		return E_FAIL;
	}

	

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CNPC_AdultM");
		return E_FAIL;
	}

	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CNPC_AdultM");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CNPC_AdultM");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CNPC_AdultM");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(0.7f, 0.7f, 0.7f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CNPC_AdultM");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_AdultM::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	// OutlineThickness
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

CNPC_AdultM* CNPC_AdultM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_AdultM* pInstance = new CNPC_AdultM(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC_AdultM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_AdultM::Clone(void* pArg)
{
	CNPC_AdultM* pInstance = new CNPC_AdultM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC_AdultM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_AdultM::Free()
{
	
	__super::Free();
}
