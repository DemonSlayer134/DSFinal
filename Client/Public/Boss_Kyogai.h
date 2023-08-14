#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBoss_Kyogai final : public CMonster
{
public:

	enum  PHASE
	{
		BEGIN, PHASE_1, PHASE_2
	};
	enum  STATE
	{
		STATE_INTERACT, STATE_BEGIN, STATE_NEXTPHASE, STATE_HEAL, STATE_AWAKE,
		STATE_GUARD,
		STATE_IDLE

	};

#pragma region AnimIndex
	enum  ANIM {
		////////////////// �⺻ MOVE///////////////////
		ANIM_IDLE = 41, // ���̵�
		ANIM_RUN = 43, // ��
		ANIM_STEP_FRONT =  46, // ���
		ANIM_STEP_FRONT2 = 47, // ����
		ANIM_STEP_BACK = 45, // �޴��
		ANIM_STEP_LEFT = 48, // �´��
		ANIM_STEP_LEFT2 = 49, // ������
		ANIM_STEP_RIGHT = 50, // ����
		ANIM_STEP_RIGHT2 = 51, // ������

		

		////////////////// ��ų ///////////////////////
		ANIM_ATKCMB1 = 1, // ��ſ� �ִ� �� 3���� -> �鳯 ���������µ�?
		ANIM_ATKCMB2 = 2, // �ι�ħ
		ANIM_ATKCMB3 = 3, // ��Ʈ

		ANIM_ATKSK_READY = 18, // ���ű� ����
		ANIM_ATKSK_LOOP = 19, // ���� ����
		ANIM_ATKSK_LB = 20, // ���� ������ ��
		ANIM_ATKSK_RB = 21, // ������ ������ ��
		ANIM_ATKSK_LF = 22, // ���� ����
		ANIM_ATKSK_RF = 23, // ������ ����

		//ANIM_ATKSK_
		////////////////// ��Ʈ��� ///////////////////

	

		////////////////// ������ ///////////////////
		ANIM_HEAL,

		////////////////// �ƽŸ�� ///////////////////
		ANIM_AWAKE = 0 // �����


	};
#pragma endregion
private:
	CBoss_Kyogai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Kyogai(const CBoss_Kyogai& rhs);
	virtual ~CBoss_Kyogai() = default;
#pragma region Tick
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();
#pragma endregion
#pragma region Trigger
#ifdef _DEBUG
	void Debug_State(_double dTimeDelta);
#endif //_DEBUG
	void Update_AnimIndex(_uint iAnimIndex);
	void Update_Trigger(_double dTimeDelta);
	void Update_TriggerTime(_double dTimeDelta);
	
	//	void Update_Begin(_double dTimeDelta);
	void Update_Phase_1(_double dTimeDelta);
	void Update_Phase_2(_double dTimeDelta);	

	void Trigger_Interact_Phase_1(_double dTimeDelta);
	void Trigger_Interact_Phase_2(_double dTimeDelta);	

	void Trigger_Interact();
	void Trigger_Begin();
	void Trigger_Guard();	
	void Trigger_PushAway();	
	void Trigger_NextPhase();	
	void Trigger_Heal();
	void Trigger_Awake();
	void Trigger_Awake_Cinematic();

#pragma endregion

#pragma region Pattern
private: //���� �Լ���
	void Update_State(_double dTimeDelta);
	void Update_Interact(_double dTimeDelta);
	void Update_Begin(_double dTimeDelta);
	void Update_PushAway(_double dTimeDelta);
	
	void Update_Guard(_double dTimeDelta);
	
	void Update_NextPhase(_double dTimeDelta);
	
	void Update_Heal(_double dTimeDelta);
	void Update_Awake(_double dTimeDelta);


#pragma endregion
private: // _bool
	
	_bool	m_bAnimFinish = { false };

	_bool	m_bHit = { false };
	_bool	m_bHeal = { false };
	
	_bool	m_bTrigger = { false };
	_bool	m_bPatternStart = { false };

	_bool	m_bAwake = { false };
	_bool	m_bFirstAwake = { false };
	_bool	m_bSecondAwake = { false };
	//_bool	m_bNextPhase = { false };

private: // time
	_double	m_dJumpStompTime = { 0.0 };
	_double m_dAwakeTime = { 0.0 };

	_double m_dTriggerTime = { 0.0 };

private:
	_uint	m_iRandomDirNum = { 0 };
	_uint	m_iRandomPatternNum = { 0 };
	_uint	m_iPunchCount = { 0 };
	_uint	m_iIdleCnt = { 0 };

	_uint	m_iTriggerCnt = { 0 };

	_uint	m_iHp = { 100 }; // �ӽ�

private:
	PHASE   m_eCurPhase = PHASE_1;
	STATE	m_eCurstate = STATE_IDLE;
	ANIM    m_eCurAnimIndex = ANIM_IDEL;
	ANIM	m_ePreAnimIndex = ANIM_IDEL;

#pragma region Render
private: // ���� �ƿ�����
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;
	/* �ӽ� �ڵ� */
	_uint	m_iNumAnim = { 0 };
	// ���� Ȯ�ο�
	_uint	m_iMeshNum = { 0 };
#pragma endregion
private:
	//CTransform* m_pPlayerTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CBoss_Kyogai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END