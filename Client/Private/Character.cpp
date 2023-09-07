#include "pch.h"
#include "..\Public\Character.h"
#include "Camera_Free.h"
#include "GameInstance.h"

#include "AtkCollManager.h"
#include "Fade_Manager.h"
#include "SmeshStone.h"


CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CLandObject(rhs)
{
}

void CCharacter::Add_HitCollider(CGameObject* pAtkColl)
{
	if (nullptr == pAtkColl)
		return;

	if (1 > m_HitCollider.size())
		m_HitCollider.emplace_back(dynamic_cast<CAtkCollider*>(pAtkColl));
	else
	{
		_uint iCount = { 0 };

		for (auto iter = m_HitCollider.begin(); iter != m_HitCollider.end(); iter++)
		{
			if (pAtkColl == (*iter))
				break;

			iCount++;
		}

		if (m_HitCollider.size() <= iCount)
		{
			m_HitCollider.emplace_back(dynamic_cast<CAtkCollider*>(pAtkColl));
		}
	}
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_CharacterDesc, pArg, sizeof m_CharacterDesc);
	}

	m_CharacterDesc.WorldInfo.vScale = _float3(0.8f, 0.8f, 0.8f);
	m_CharacterDesc.WorldInfo.fDegree = 0.f;

	if (FAILED(__super::Initialize(&m_CharacterDesc.WorldInfo)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		//�ʱ� ��ġ ����
		m_pTransformCom->Scaling(m_WorldInfo.vScale);
		m_pTransformCom->Rotation(XMLoadFloat3(&m_WorldInfo.vAxis), m_WorldInfo.fDegree);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldInfo.vPosition));

		m_fLand_Y = m_CharacterDesc.Land_Y;
		m_eCurNavi = m_CharacterDesc.eCurNavi;
		m_eNextNavi = m_eCurNavi;
	}

	return S_OK;
}

void CCharacter::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Check_HitCollDead();
	Check_HitType();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE || pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE) {

		_bool Is_Battle = CFadeManager::GetInstance()->Get_Is_Battle();

		if (Is_Battle == false)
			m_pTransformCom->Scaling(_float3(0.67f, 0.67f, 0.67f));
		else
			m_pTransformCom->Scaling(_float3(0.8f, 0.8f, 0.8f));
	}
	

	Safe_Release(pGameInstance);


	for (_uint i = 0; i < COLL_END; i++)
		m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

}

void CCharacter::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	Status_Work(dTimeDelta);

#ifdef _DEBUG
	for (_uint i = 0; i < COLL_END; i++)
	{
		if (FAILED(m_pRendererCom->Add_DebugGroup(m_pColliderCom[i])))
			return;
	}
#endif // _DEBUG

}

HRESULT CCharacter::Render()
{
	return S_OK;
}

CTransform* CCharacter::Get_TransformCom()
{
	return m_pTransformCom;
}

HRESULT CCharacter::Read_Animation_Control_File(const char* szBinfilename)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	char szFullPath[MAX_PATH] = { "" };
	strcpy_s(szFullPath, "../Bin/Resources/AnimToolBin/");
	strcat_s(szFullPath, szBinfilename);

	ifstream fin;
	fin.open(szFullPath, ios::binary);
	if (false == fin.is_open())
	{
		return E_FAIL;
	}

	_int AnimSize;
	fin.read(reinterpret_cast<char*>(&AnimSize), sizeof(_int));

	for (_int i = 0; i < AnimSize; i++)
	{
		CAnimation::CONTROLDESC ControlDesc;

		fin.read(reinterpret_cast<char*>(&ControlDesc.m_fAnimationSpeed), sizeof(_float));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_iConnect_Anim), sizeof(_int));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_isCombo), sizeof(_bool));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_iConnect_ComboAnim), sizeof(_int));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_isRootAnimation), sizeof(_bool));

		_int isizeEvent;
		fin.read(reinterpret_cast<char*>(&isizeEvent), sizeof(_int));

		for (_int i = 0; i < isizeEvent; i++)
		{
			CAnimation::EVENTDESC EventDesc;
			fin.read(reinterpret_cast<char*>(&EventDesc.m_dTime), sizeof(_double));
			fin.read(reinterpret_cast<char*>(&EventDesc.m_isFirst), sizeof(_bool));

			ControlDesc.m_vecTime_Event.emplace_back(EventDesc);
		}
		m_pModelCom->Set_Animation_Control(i, ControlDesc);
	}
	fin.close();

	Safe_Release(pGameInstance);

	return S_OK;
}

void CCharacter::RootAnimation(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();

	if (pAnim->Get_ControlDesc().m_isRootAnimation)
	{
		//�ִϸ��̼� ���۽� ù ��ġ
		if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0.0)
		{
			XMStoreFloat4(&m_Save_RootPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_Save_RootPos));
		}
		else
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			_float3 RootPosition = pAnim->Get_RootPosition();
			_float4x4 RootWorldConvert = m_pTransformCom->Get_WorldFloat4x4();
			_float3 FinalRootPos = { 0.0f, 0.0f, 0.0f };
			XMStoreFloat3(&FinalRootPos, XMVector3TransformCoord(XMLoadFloat3(&RootPosition), XMLoadFloat4x4(&RootWorldConvert)));

			_float4 Final = { -FinalRootPos.x * 0.005f, FinalRootPos.y * 0.005f , -FinalRootPos.z * 0.005f, 1.f };

			// �÷��̾��� ���� ��ġ�� �������� Root bone�� ��ġ�� ��ȭ��Ŵ
			_float4  SubPos = { m_Save_RootPos.x + Final.x, m_Save_RootPos.y + Final.y , m_Save_RootPos.z + Final.z , 1.f };

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&SubPos));
		}
	}
}

_bool CCharacter::EventCallProcess()
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();

	CAnimation::CONTROLDESC ControlDesc = pAnim->Get_ControlDesc();

	if (m_iPreAnimIndex_ForEvent != m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pModelCom->Set_EventReset(m_iPreAnimIndex_ForEvent);
		m_iPreAnimIndex_ForEvent = m_pModelCom->Get_iCurrentAnimIndex();

		m_iEvent_Index = 0;
	}

	if (ControlDesc.m_isEventCall)
	{
		ControlDesc.m_isEventCall = false;

		pAnim->Set_ControlDesc(ControlDesc);

		return true;
	}

	return false;
}


void CCharacter::Reset_Decleration(_float fResetSpeed)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();

	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_isReset_Atk_MoveControl = true;
	}
	if (m_isReset_Atk_MoveControl)
	{
		m_isReset_Atk_MoveControl = false;
		m_fAtk_MoveControl = fResetSpeed;
	}
}

void CCharacter::Go_Straight_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//������ �������� Transform �̵�
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Straight(dTimeDelta * m_fAtk_MoveControl , m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Backward_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//������ �������� Transform �̵�
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Backward(dTimeDelta * m_fAtk_MoveControl, m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Left_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//������ �������� Transform �̵�
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Left(dTimeDelta * m_fAtk_MoveControl, m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Right_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//������ �������� Transform �̵�
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Right(dTimeDelta * m_fAtk_MoveControl, m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Dir_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease, _float4 Dir, _bool	bIsNaviOff)
{
	//������ �������� Transform �̵�
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		if(bIsNaviOff == false)
			m_pTransformCom->Go_Dir(dTimeDelta * m_fAtk_MoveControl, XMLoadFloat4(&Dir), m_pNavigationCom[m_eCurNavi]);
		else
			m_pTransformCom->Go_Dir(dTimeDelta * m_fAtk_MoveControl, XMLoadFloat4(&Dir));

		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Dir_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed, _float4 Dir, _bool bIsJumpOn)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		if(!bIsJumpOn)
			m_pTransformCom->Go_Dir(dTimeDelta * constantSpeed, XMLoadFloat4(&Dir), m_pNavigationCom[m_eCurNavi]);
		else
			m_pTransformCom->Go_Dir(dTimeDelta * constantSpeed, XMLoadFloat4(&Dir));
	}
}

void CCharacter::Go_Straight_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed, _bool bIsJumpOn)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		if(!bIsJumpOn)
			m_pTransformCom->Go_Straight(dTimeDelta * constantSpeed , m_pNavigationCom[m_eCurNavi]);
		else
			m_pTransformCom->Go_Straight(dTimeDelta * constantSpeed);
	}
}

void CCharacter::Go_Backward_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pTransformCom->Go_Backward(dTimeDelta * constantSpeed, m_pNavigationCom[m_eCurNavi]);
	}
}

void CCharacter::Go_Left_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pTransformCom->Go_Left(dTimeDelta * constantSpeed, m_pNavigationCom[m_eCurNavi]);
	}
}

void CCharacter::Go_Right_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pTransformCom->Go_Right(dTimeDelta * constantSpeed, m_pNavigationCom[m_eCurNavi]);
	}
}

void CCharacter::Go_Straight_Deceleration_Common(_double dTimeDelta, _float ResetSpeed, _float fDecrease)
{
	//������ �������� Transform �̵�
	Reset_Decleration(ResetSpeed);

	m_pTransformCom->Go_Straight(dTimeDelta * m_fAtk_MoveControl , m_pNavigationCom[m_eCurNavi]);
	m_fAtk_MoveControl -= fDecrease;
	if (m_fAtk_MoveControl <= 0.0f)
	{
		m_fAtk_MoveControl = 0.0f;
	}
}

void CCharacter::Go_Dir_Constant(_double dTimeDelta, DIR Dir, _uint iAnimindex, _float fSpeed, _double dStartRatio, _double dEndRatio)
{
	// ���ϴ� �ִϸ��̼��� ������ŭ �̵�
	switch (Dir)
	{
	case DIR_UP:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Straight_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;
	case DIR_DOWN:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Backward_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;
	case DIR_RIGHT:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Right_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;
	case DIR_LEFT:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Left_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;

	}
	
}

void CCharacter::Navigation_To_Ground(_double dTimeDelta)
{


}

void CCharacter::Gravity(_double dTimeDelta)
{
	_float4 Pos;
	XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	

	//���� ����
	if (m_isJumpOn)
	{
		if (m_isJumpStop)
		{
			m_dTime_JumpStop += dTimeDelta;
			if (m_dTime_JumpStop >= m_dTime_JumpStop_Duration)
				m_isJumpStop = false;
		}
		else
		{
			m_pTransformCom->Go_Up(dTimeDelta * m_fJump_Acc);
			m_fJump_Acc -= m_fGravity_Fall;
		}

		m_dDelay_Fall += dTimeDelta;
		if (m_dDelay_Fall > 0.1)
		{
			if (Pos.y <= m_fLand_Y)
			{
				m_isJumpOn = false;
				Pos.y = m_fLand_Y;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

			}
		}
	}
	//�� �� ����
	else
	{
		Pos.y = m_fLand_Y;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

		m_fJump_Acc = 0.0f;
		m_isFirst_JumpAtk = true;
		m_isJumpStop = false;
	}


}

void CCharacter::Ground_Animation_Play(_int CurAnim, _int GroundAnim)
{
	if (m_pModelCom->Get_iCurrentAnimIndex() == CurAnim)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (Pos.y <= m_fLand_Y)
		{
			m_pModelCom->Set_Animation(GroundAnim);
			m_isStrictUpper = false;
		}
	}

}

void CCharacter::Jumping(_float ResetSpeed, _float fFallDecrease)
{
	m_fJump_Acc = ResetSpeed;
	m_isJumpOn = true;
	m_dDelay_Fall = 0.0;

	m_fGravity_Fall = fFallDecrease;
}

void CCharacter::JumpStop(_double dDuration)
{
	m_isJumpStop = true;
	m_dTime_JumpStop_Duration = dDuration;
	m_dTime_JumpStop = 0.0;
}

void CCharacter::Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, 
								CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg, 
								 CAtkCollider::BULLET_TYPE eBulletType)
{
	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = DurationTime;

	AtkCollDesc.pParentTransform = m_pTransformCom;

	AtkCollDesc.eAtkType = AtkType;
	AtkCollDesc.eBulletType = eBulletType;

	AtkCollDesc.fDamage = fDmg;

	AtkCollDesc.bBullet = false;
	

	XMStoreFloat4(&AtkCollDesc.AtkDir, XMVector4Normalize(vDir));

	CAtkCollManager::GetInstance()->Reuse_Collider(pLayerTag, &AtkCollDesc);
}

void CCharacter::Make_AtkBulletColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime,
	CAtkCollider::ATK_TYPE AtkType, _vector vAtkDir, _float fDmg, CTransform* pTransform,
	_double Speed, CAtkCollider::BULLET_TYPE eBulletType, const char* pEffectTag, CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc)
{
	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = DurationTime;

	AtkCollDesc.pParentTransform = pTransform;

	AtkCollDesc.eAtkType = AtkType;
	AtkCollDesc.eBulletType = eBulletType;

	AtkCollDesc.fDamage = fDmg;
	AtkCollDesc.Speed = Speed;

	AtkCollDesc.bBullet = true;
	//if (true == AtkCollDesc.bBullet)
	{
		strcpy_s(AtkCollDesc.pEffectTag, pEffectTag);
	}

	if (nullptr != pEffectWorldDesc)
		AtkCollDesc.EffectWorldDesc = *pEffectWorldDesc;
	else
	{
		AtkCollDesc.EffectWorldDesc = CEffectPlayer::EFFECTWORLDDESC();
	}

	XMStoreFloat4(&AtkCollDesc.AtkDir, XMVector3Normalize(vAtkDir));
	
	CAtkCollManager::GetInstance()->Reuse_Collider(pLayerTag, &AtkCollDesc);
}

void CCharacter::Check_HitCollDead()
{
	for (auto iter = m_HitCollider.begin(); iter != m_HitCollider.end();)
	{
		if (true == (*iter)->Get_Dead())
		{
			iter = m_HitCollider.erase(iter);
			continue;
		}

		++iter;
	}
}

void CCharacter::Check_HitType()
{
	for (auto& pHitColl : m_HitCollider)
	{
		if (false == pHitColl->Get_IsAttack(this))
		{
			if (pHitColl->Get_Collider()->Get_Hit_Small())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_ConnectSmall())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Big())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Blow())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_BigBlow())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_BigBlow(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Spin())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Upper())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Bound())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_CutScene())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_CutScene(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Hekireki())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Swamp())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Swamp(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Web())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Web(true);
			}

			pHitColl->Add_AtkObejct(this);
		}
	}
}

void CCharacter::Status_Work(_double dTimeDelta)
{
	
	//Mp
	m_dDelay_Mp_Used += dTimeDelta;
	if (m_dDelay_Mp_Used > 2.0)
		m_StatusDesc.fMp += 0.06f;

	if (100.0f <= m_StatusDesc.fMp)
	{
		m_StatusDesc.fMp = 100.0f;
	}
	if (m_StatusDesc.fMp <= 0.0f)
	{
		m_StatusDesc.fMp = 0.0f;
	}

	if (20.0f <= m_StatusDesc.fMp)
		m_isCan_Mp_Skill = true;
	else
		m_isCan_Mp_Skill = false;


	//�� ��Ʈ���� Ʈ����
	if (m_isHit_Success)
	{
		m_isHit_Success = false;

		m_dDelay_ComboReset = 0.0;
		
		m_StatusDesc.iAttackCombo++;

		if(m_StatusDesc.iSpecial_Cnt < 3 && m_StatusDesc.iAwaken == 0)
			m_StatusDesc.fSpecial += 13.3f;
	}
	
	//�޺�(Attack)
	if (m_StatusDesc.iAttackCombo > 0)
	{
		m_dDelay_ComboReset += dTimeDelta;
		if (m_dDelay_ComboReset > 3.5f)
		{
			m_dDelay_ComboReset = 0.0;
			m_StatusDesc.iAttackCombo = 0;
		}

	}

	//�޺�(Hit)
	if (m_StatusDesc.iHitCombo > 0)
	{
		m_dDelay_ComboReset_2 += dTimeDelta;
		if (m_dDelay_ComboReset_2 > 3.5f)
		{
			m_dDelay_ComboReset_2 = 0.0;
			m_StatusDesc.iHitCombo = 0;
		}

	}

	//����Ȱ�����
	if (m_StatusDesc.iSpecial_Cnt >= 3)
	{
		m_StatusDesc.fSpecial = 0.0f;
	}

	if (100.0f <= m_StatusDesc.fSpecial)
	{
		m_StatusDesc.fSpecial = 0.0f;
		m_StatusDesc.iSpecial_Cnt++;
	}

	if (m_StatusDesc.iAwaken != 0)
	{
		if (m_StatusDesc.isAwaken_First)
		{
			m_StatusDesc.isAwaken_First = false;

			m_StatusDesc.fSpecial_Save = m_StatusDesc.fSpecial;
		}
		m_StatusDesc.fSpecial = (_float)m_StatusDesc.dAwaken_TimeAcc;
		m_StatusDesc.fSpecial_Max = (_float)m_StatusDesc.dAwaken_Duration;
	}
	else
	{
		if (m_StatusDesc.isNormal_First)
		{
			m_StatusDesc.isNormal_First = false;

			m_StatusDesc.fSpecial = m_StatusDesc.fSpecial_Save;
			m_StatusDesc.fSpecial_Max = 100.0f;
		}
	}

	//����Ʈ ������
	m_StatusDesc.fSupport += 0.05f;
	if (m_StatusDesc.fSupport >= m_StatusDesc.fSupport_Max)
	{
		m_StatusDesc.fSupport = m_StatusDesc.fSupport_Max;
	}
	if (m_StatusDesc.fSupport <= 0.0f)
	{
		m_StatusDesc.fSupport = 0.0f;
	}

	//CPlayerManager::GetInstance()->Set_Status(m_StatusDesc);
}

void CCharacter::Use_Mp_Skill()
{
	if (m_isCan_Mp_Skill)
	{
		if(m_StatusDesc.iAwaken < 2)
			m_StatusDesc.fMp -= 20.0f;
		m_dDelay_Mp_Used = 0.0;
	}
}

_float4 CCharacter::Calculate_Dir_From_Pos(_float4 Pos)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 MyPos;
	XMStoreFloat4(&MyPos, vMyPos);

	Pos.y = MyPos.y;
	_vector vTarget = XMLoadFloat4(&Pos);
	
	_float4		Dir;
	XMStoreFloat4(&Dir, XMVector3Normalize(vTarget - vMyPos));

	return Dir;
}

_float CCharacter::Calculate_Distance_From_Pos(_float4 Pos)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 MyPos;
	XMStoreFloat4(&MyPos, vMyPos);

	Pos.y = MyPos.y;
	_vector vTarget = XMLoadFloat4(&Pos);
	
	_vector vDir = XMVector3Normalize(vTarget - vMyPos);
	_float fDistance = Convert::GetLength(vTarget - vMyPos);

	return fDistance;
}

void CCharacter::Set_Height()
{
	m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (LEVEL_TRAIN == pGameInstance->Get_CurLevelIdx())
	{
		_float vPosX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_fLand_Y -= fabsf(vPosX - 205.15f) * 0.15f;
	}

	Safe_Release(pGameInstance);
}

void CCharacter::Camera_Shake(_double dShakeTime, _uint iShakePower)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera")));
	pCamera->Shake(dShakeTime, iShakePower);

	Safe_Release(pGameInstance);
}

void CCharacter::Create_GroundSmoke(CGroundSmoke::SMOKE_TYPE eSmokeType , _fvector vOffsetPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	CEffectW::EFFECTWDESC EffectWDesc;
	EffectWDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSetY(vOffsetPos , 0.f);
 	EffectWDesc.eEffectWType = CEffectW_Manager::EFFECT_GROUNDSMOKE;
	EffectWDesc.iNumX = 6; EffectWDesc.iNumY = 6;

	switch (eSmokeType)
	{
	case CGroundSmoke::SMOKE_FALLDOWN:
		EffectWDesc.vStartPosX = { -0.5f,0.5f }; EffectWDesc.vStartPosY = { -0.02f,0.15f }; EffectWDesc.vStartPosZ = { -0.5f,0.5f };
		EffectWDesc.vFrameSpeed = { 0.03f , 0.04f };
		EffectWDesc.vStartSizeX = { 0.9f , 1.4f }; EffectWDesc.vStartSizeY = { 0.8f , 1.1f };
		EffectWDesc.vSpeedX = { -2.0f , 2.0f }; EffectWDesc.vSpeedY = { 0.05f , 0.1f };EffectWDesc.vSpeedZ = { -3.f , 3.f };
		EffectWDesc.vSizeSpeedX = { 0.8f , 1.3f }; EffectWDesc.vSizeSpeedY = { 0.8f , 1.3f };
		
		for (_uint i = 0; i < 10; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);

		break;
	case CGroundSmoke::SMOKE_SMESHSPREAD:
		EffectWDesc.vStartPosX = { -0.7f,0.7f }; EffectWDesc.vStartPosY = { -0.1f,0.15f }; EffectWDesc.vStartPosZ = { -0.7f,0.7f };
		EffectWDesc.vFrameSpeed = { 0.04f , 0.06f };
		EffectWDesc.vStartSizeX = { 0.7f , 1.0f }; EffectWDesc.vStartSizeY = { 0.7f , 1.0f };
		EffectWDesc.vSpeedX = { -2.6f , 2.6f }; EffectWDesc.vSpeedY = { 0.04f , 0.06f }; EffectWDesc.vSpeedZ = { -2.6f , 2.6f };
		EffectWDesc.vSizeSpeedX = { 2.1f , 2.6f }; EffectWDesc.vSizeSpeedY = { 2.1f , 2.6f };

		for (_uint i = 0; i < 30; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	case CGroundSmoke::SMOKE_SIDESTEP:
		EffectWDesc.vStartPosX = { -0.3f,0.3f };EffectWDesc.vStartPosY = { -0.01f,0.2f };EffectWDesc.vStartPosZ = { -0.3f,0.3f };
		EffectWDesc.vFrameSpeed = { 0.01f , 0.015f };
		EffectWDesc.vStartSizeX = { 1.2f , 1.4f };	EffectWDesc.vStartSizeY = { 1.1f , 1.5f };
		EffectWDesc.vSpeedX = { -2.0f , 2.0f };EffectWDesc.vSpeedY = { 0.12f , 0.19f };EffectWDesc.vSpeedZ = { -2.0f , 2.0f };
		EffectWDesc.vSizeSpeedX = { 0.7f , 1.2f }; EffectWDesc.vSizeSpeedY = { 0.7f , 1.2f };

		for (_uint i = 0; i < 8; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	case CGroundSmoke::SMOKE_UPDOWN:
		EffectWDesc.vStartPosX = { -0.15f,0.15f }; EffectWDesc.vStartPosY = { -0.05f,-0.03f }; EffectWDesc.vStartPosZ = { -0.15f,0.15f };
		EffectWDesc.vFrameSpeed = { 0.04f , 0.06f }; 
		EffectWDesc.vStartSizeX = { 1.2f , 1.4f }; EffectWDesc.vStartSizeY = { 1.2f , 1.8f };
		EffectWDesc.vSpeedX = { -0.0f , 0.0f }; EffectWDesc.vSpeedY = { 2.f , 5.5f }; EffectWDesc.vSpeedZ = { -0.0f , 0.f };
		EffectWDesc.vSizeSpeedX = { 1.2f , 1.4f }; EffectWDesc.vSizeSpeedY = { 1.6f , 1.8f };
		EffectWDesc.fGravity = 4.5f;

		for (_uint i = 0; i < 12; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	case CGroundSmoke::SMOKE_DASHLAND:
	
		EffectWDesc.vStartPosX = { -0.5f,0.5f };EffectWDesc.vStartPosY = { -0.05f,0.15f };EffectWDesc.vStartPosZ = { -0.5f,0.5f };
		EffectWDesc.vFrameSpeed = { 0.015f , 0.018f };
		EffectWDesc.vStartSizeX = { 0.9f , 1.4f };EffectWDesc.vStartSizeY = { 0.8f , 1.1f };
		EffectWDesc.vSpeedX = { -3.0f , 3.0f };	EffectWDesc.vSpeedY = { 0.05f , 0.07f }; 	EffectWDesc.vSpeedZ = { -3.0f , 3.f };
		EffectWDesc.vSizeSpeedX = { 1.2f , 1.5f };EffectWDesc.vSizeSpeedY = { 1.2f , 1.5f };

		for (_uint i = 0; i < 10; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	case CGroundSmoke::SMOKE_RUN:

		EffectWDesc.vStartPosX = { -0.2f,0.2f };EffectWDesc.vStartPosY = { -0.05f,0.1f };EffectWDesc.vStartPosZ = { -0.2f,0.2f };
		EffectWDesc.vFrameSpeed = { 0.005f , 0.01f };
		EffectWDesc.vStartSizeX = { 0.7f , 1.2f };EffectWDesc.vStartSizeY = { 0.6f , 1.0f };
		EffectWDesc.vSpeedX = { -0.0f , 0.0f };EffectWDesc.vSpeedY = { 0.05f , 0.1f };EffectWDesc.vSpeedZ = { 0.0f , 3.f };
		EffectWDesc.vSizeSpeedX = { 0.4f , 0.7f };EffectWDesc.vSizeSpeedY = { 0.4f , 0.7f };

		for (_uint i = 0; i < 2; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	 case CGroundSmoke::SMOKE_JUMP:
	
		EffectWDesc.vStartPosX = { -0.5f,0.5f };EffectWDesc.vStartPosY = { -0.05f,0.15f };EffectWDesc.vStartPosZ = { 0.f,0.5f };
		EffectWDesc.vFrameSpeed = { 0.01f , 0.02f };
		EffectWDesc.vStartSizeX = { 0.9f , 1.4f };EffectWDesc.vStartSizeY = { 0.8f , 1.1f };
		EffectWDesc.vSpeedX = { -0.0f , 0.0f };EffectWDesc.vSpeedY = { 0.05f , 0.1f };EffectWDesc.vSpeedZ = { 0.0f , 3.f };
		EffectWDesc.vSizeSpeedX = { 0.3f , 0.5f };EffectWDesc.vSizeSpeedY = { 0.3f , 0.5f };

		for (_uint i = 0; i < 5; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	 case CGroundSmoke::SMOKE_BLADECREATE:
 		 EffectWDesc.vPos = vOffsetPos;
		 EffectWDesc.vStartPosX = { -0.15f,0.15f }; EffectWDesc.vStartPosY = { -1.0f,-0.5f }; EffectWDesc.vStartPosZ = { -0.15f,0.15f };
		 EffectWDesc.vFrameSpeed = { 0.03f , 0.05f };
		 EffectWDesc.vStartSizeX = { 2.2f ,	2.5f }; EffectWDesc.vStartSizeY = { 3.8f , 4.2f };
		 EffectWDesc.vSpeedX = { 0.0f , 0.0f }; EffectWDesc.vSpeedY = { 7.f , 7.5f }; EffectWDesc.vSpeedZ = { 0.0f , 0.f };
		 EffectWDesc.vSizeSpeedX = { 0.5f , 1.0f }; EffectWDesc.vSizeSpeedY = {	2.0f , 2.5f };
		 EffectWDesc.vStartFrame = { 0.f , 10.f };
		 EffectWDesc.fGravity = { 15.f};

 		 for (_uint i = 0; i < 1; ++i)
 			 CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		 break;
	 case CGroundSmoke::SMOKE_KYOGAI_KICKDOWN:
		
		 EffectWDesc.vStartPosX = { -7.0f,7.0f }; EffectWDesc.vStartPosY = { -0.0f,0.5f }; EffectWDesc.vStartPosZ = { -7.0f,7.0f };
		 EffectWDesc.vFrameSpeed = { 0.03f , 0.05f };
		 EffectWDesc.vStartSizeX = { 3.0f ,	3.5f }; EffectWDesc.vStartSizeY = { 3.0f , 3.5f };
		 EffectWDesc.vSpeedX = { 0.0f , 0.0f }; EffectWDesc.vSpeedY = { 0.5f , 0.8f }; EffectWDesc.vSpeedZ = { 0.0f , 0.f };
		 EffectWDesc.vSizeSpeedX = { 1.5f , 2.0f }; EffectWDesc.vSizeSpeedY = { 2.0f , 2.5f };
		 EffectWDesc.vStartFrame = { 0.f , 10.f };

		 for (_uint i = 0; i < 20; ++i)
			 CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		 break;
	default:
		break;
	}

	Safe_Release(pGameInstance);
}

void CCharacter::Create_StoneParticle(_fvector vOffsetPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	CEffectW::EFFECTWDESC EffectWDesc;
	EffectWDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffsetPos;
	EffectWDesc.eEffectWType = CEffectW_Manager::EFFECT_STONEPARTICLE;
	EffectWDesc.iNumX = 2; EffectWDesc.iNumY = 2;

	EffectWDesc.vStartPosX = { -0.5f,0.5f }; EffectWDesc.vStartPosY = { -0.00f,0.7f }; EffectWDesc.vStartPosZ = { -0.1f,0.1f };
	EffectWDesc.vStartSizeX = { 0.05f , 0.20f }; EffectWDesc.vStartSizeY = { 0.05f , 0.20f };
	EffectWDesc.vSpeedX = { -2.5f , 2.5f }; EffectWDesc.vSpeedY = { 1.f , 7.f }; EffectWDesc.vSpeedZ = { -2.5f , 2.5f };
	EffectWDesc.vSizeSpeedX = { 0.f , 0.f }; EffectWDesc.vSizeSpeedY = { 0.f , 0.f };
	EffectWDesc.fGravity = 6.0f;

	for (_uint i = 0; i < 30; ++i)
		CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECTW_TYPE(EffectWDesc.eEffectWType), &EffectWDesc);

	Safe_Release(pGameInstance);
}

void CCharacter::Create_SmeshStone(_fvector vOffsetPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	CSmeshStone::EFFECTDESC EffectDesc;
	EffectDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffsetPos;

	pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SmeshStone"), &EffectDesc);

	Safe_Release(pGameInstance);
}

void CCharacter::Play_FallDownEffect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	if (iCurIdx == LEVEL_VILLAGE || iCurIdx == LEVEL_FINALBOSS)
	{
		CEffectPlayer::Get_Instance()->Play("FallDown_Particle", m_pTransformCom);	// �� ����Ʈ
		CEffectPlayer::Get_Instance()->Play("FallDown_Effect", m_pTransformCom);	// ���׶� ����Ʈ ���� Ŀ����
	}

	Safe_Release(pGameInstance);
}

void CCharacter::Play_HitEffect()
{
	_uint iRanNum = Random::Generate_Int(0, 5);

	CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

	switch (iRanNum)
	{
	case 0:
		CEffectPlayer::Get_Instance()->Play("Hit_Effect0", m_pTransformCom);
		break;
	case 1:
	{
		EffectWorldDesc.vPosition.y += 0.8f;
		EffectWorldDesc.fScale = 1.4f;
		CEffectPlayer::Get_Instance()->Play("Hit_Effect3", m_pTransformCom, &EffectWorldDesc);
		break; 
	}
	case 2:
	{
		EffectWorldDesc.vPosition.y += 0.8f;
		EffectWorldDesc.fScale = 1.4f;
		CEffectPlayer::Get_Instance()->Play("Hit_Effect4", m_pTransformCom, &EffectWorldDesc);
		break;
	}
	case 3:
		EffectWorldDesc.fScale = 1.4f;
		CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom);
		break;
	case 4:
		EffectWorldDesc.fScale = 1.4f;
		CEffectPlayer::Get_Instance()->Play("Hit_Effect6", m_pTransformCom);
		break;
	case 5:
		EffectWorldDesc.fScale = 1.4f;
		CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom);
		break;
	}
}

HRESULT CCharacter::Add_Components()
{
	/* for.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed to Add_Com_Renderer : CCharacter");
		return E_FAIL;
	}



	return S_OK;
}

void CCharacter::SetUp_Height()
{
	_fvector vPos = Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CCharacter::Tick_Collider(_double dTimeDelta)
{
	for (_uint i = 0; i < COLL_END; i++)
		m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);
}

void CCharacter::Free()
{
	__super::Free();

	m_HitCollider.clear();

	for (_uint i = 0; i < COLL_END; i++)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);


}
