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
		STATE_GUARD, STATE_LINKERCMB,
		STATE_ATKCMB, STATE_ATKCMB2, STATE_AWAKE_ROOMCHANGE, STATE_ATKSTEP,
		STATE_STOMPKICK, STATE_ATKPUNCH, 

		STATE_HIT_SMALL, STATE_HIT_CONNECTSMALL, STATE_HIT_BIG, STATE_HIT_BLOW, STATE_HIT_BIGBLOW,
		STATE_HIT_BOUND, STATE_SPIN, STATE_GETUP, STATE_BIGGETUP, STATE_ROLLGETUP,
		STATE_HIT_UPPER, STATE_HIT_CUTSCENE

	};

#pragma region AnimIndex
	enum  ANIM {
		////////////////// �⺻ MOVE///////////////////
		ANIM_IDLE = 41, // ���̵�
		ANIM_RUN = 43, // ��
		ANIM_STEP_FRONT = 46, // ���
		ANIM_STEP_FRONT2 = 47, // ����
		ANIM_STEP_BACK = 45, // �޴��
		ANIM_STEP_LEFT = 48, // �´��
		ANIM_STEP_LEFT2 = 49, // ������
		ANIM_STEP_RIGHT = 50, // ����
		ANIM_STEP_RIGHT2 = 51, // ������

		ANIM_WALK_LEFT_START = 60, // �� �ȱ�
		ANIM_WALK_LEFT_LOOP = 61, // �� �ɾ�
		ANIM_WALK_LEFT_END = 62, // �� �ɾ� ��
		ANIM_WALK_RIGHT_START = 63, // �� �ȱ�
		ANIM_WALK_RIGHT_LOOP = 61, // �� �ɾ�
		ANIM_WALK_RIGHT_END = 62, // �� �ɾ� ��

		////////////////// ��ų ///////////////////////
		ANIM_ATKCMB1 = 1, // ��ſ� �ִ� �� 3���� -> �鳯 ���������µ�?
		ANIM_ATKCMB2 = 2, // �ι�ħ
		ANIM_ATKCMB3 = 3, // ��Ʈ -> �� �ѹ��� �� ������� �ؾ��� ��?

		//������ 2 ��� �� ����ġ��
		ANIM_ATKCMB_01READY = 4, // ���� ���ġ�� �غ��ڼ�
		ANIM_ATKCMB_02LOOP = 5, // ��� �������� ġ�� // 8��ġ�� 2���� 4��
		ANIM_ATKCMB_03END = 6, // ��� ����ġ�� ������ ������ ���̵��ڼ��� ���ƿ�

		ANIM_ATKSKCMB = 8, // ���� ������� ���ġ�� �ʹٴٴٴ�
		ANIM_ATKPUNCH = 9, // ����ġ~
		ANIM_ATKPUNCH2 = 10, // ������ġ~ 10 -11 / 10 - 12 / 0.54
		
		ANIM_KICKDOWN = 11, // �޹� �������

		ANIM_ATKCMBW05 = 12, // �� ���а� ��� ����~

		ANIM_STOMPKICK = 13, // ������� �غ��ڼ��ϰ� �� �̰ž���

		ANIM_ATKSK_READY = 18, // ���ű� ����
		ANIM_ATKSK_LOOP = 19, // ���� ����
		ANIM_ATKSK_LB = 20, // ���� ������ ��
		ANIM_ATKSK_RB = 21, // ������ ������ ��
		ANIM_ATKSK_LF = 22, // ���� ����
		ANIM_ATKSK_RF = 23, // ������ ����
		ANIM_ATKSK_END = 24, // ������ 42 IDLE�� ������

		ANIM_ROOMCHANGE_READY = 25, // ���� ���� �Ͽʹٴٴٴٴ٤� �غ��ڼ�
		ANIM_ROOMCHANGE_START = 26, // �� �ʹٴٴٴٴ٤� ��ٲٰ�ų �������� ��
		ANIM_ROOMCHANGE_END = 27, // �� �ʹٴٴٴٴ� ����

		ANIM_ATKSTEP_BACK = 28, // �ڵ��鼭 ��ġ��
		ANIM_ATKSTEP_FRONT = 29, // �յ��鼭 ��ġ��


		////////////////// ��Ʈ��� ///////////////////
		ANIM_GUARD_READY = 32, // ���� �غ��ڼ�
		ANIM_GUARD_LOOP = 33, // ���� ����
		ANIM_GUARD_END = 34, // ���� Ǯ����
		ANIM_GUARD_BREAK = 36, // ���� Ǯ��

		ANIM_BASETURN_L = 52, // ���� ��� ��¦ ��Ʈ��� �ƴѵ� �����ҵ�?
		ANIM_BASETURN_R = 53, // ������ ��� ��¦

		ANIM_HIT = 67, // ��Ʈ�� ��Ʈ~

		////////////////// ������ ///////////////////
		ANIM_HEAL,
		ANIM_DEATH = 66, // ����
		ANIM_DEATH_GETUP = 69, // �������� ��Ƴ���
		
		ANIM_GETUP = 68, // �Ͼ��
		ANIM_HITDOWN = 70, // ���������鼭 ����
		ANIM_HITDOWN_LOOP = 71, // �����ݴ� �ڼ� ����
		ANIM_HITDOWN_GETUP = 72, // �Ӹ��а� �Ͼ
		ANIM_HITDOWN_RETURN = 31, // ����

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
#endif //_DEBUG
	void Debug_State(_double dTimeDelta);

	void EventCall_Control(_double dTimeDelta);

	void Update_Hit_Messenger(_double dTimeDelta);
	void Update_AnimIndex(_uint iAnimIndex);
	void Update_Trigger(_double dTimeDelta);

	void Update_Begin(_double dTimeDelta);
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
	void Trigger_JumpStep();// �������� 0820
	void Trigger_AtkCmb();
	void Trigger_AtkCmb2();
	void Trigger_AtkStep(); 
	void Trigger_StompKick();
	void Trigger_AtkPunch();
	void Trigger_LinkerCmb();

	void Trigger_Hit_Small();
	void Trigger_Hit_ConnectSmall();
	void Trigger_Hit_Upper();
	void Trigger_Hit_Big();
	void Trigger_Hit_Blow();
	void Trigger_Hit_BigBlow();
	void Trigger_Hit_Bound();
	void Trigger_Hit_Spin();
	void Trigger_Hit_CutScene();
	void Trigger_Hit_GetUp();
	void Trigger_Hit_RollGetUp();
	void Trigger_Hit_BigGetUp();

	void Trigger_Awake_RoomChange(_double dTimeDelta); 

#pragma endregion

#pragma region Pattern
private: //���� �Լ���
	void Update_State(_double dTimeDelta);
	void Update_Interact(_double dTimeDelta);
	void Update_PushAway(_double dTimeDelta);
	void Update_Guard(_double dTimeDelta);
	void Update_NextPhase(_double dTimeDelta);
	void Update_Heal(_double dTimeDelta);
	void Update_Awake(_double dTimeDelta);
	void Update_JumpStep(_double dTimeDelta);// �������� 0820
	void Update_AtkCmb(_double dTimeDelta);
	void Update_AtkCmb2(_double dTimeDelta);
	void Update_AtkStep(_double dTimeDelta); 
	void Update_StompKick(_double dTimeDelta);
	void Update_AtkPunch(_double dTimeDelta);
	void Update_LinkerCmb(_double dTimeDelta);

	void Update_Hit_Small(_double dTimeDelta);
	void Update_Hit_Upper(_double dTimeDelta);
	void Update_Hit_Big(_double dTimeDelta);
	void Update_Hit_Blow(_double dTimeDelta);
	void Update_Hit_BigBlow(_double dTimeDelta);
	void Update_Hit_Bound(_double dTimeDelta);
	void Update_Hit_Spin(_double dTimeDelta);
	void Update_Hit_CutScene(_double dTimeDelta);
	void Update_Hit_GetUp(_double dTimeDelta);
	void Update_Hit_RollGetUp(_double dTimeDelta);
	void Update_Hit_BigGetUp(_double dTimeDelta);

	void Update_Awake_RoomChange(_double dTimeDelta);

private:
	enum BLADETYPE { BLADE_THREE_RANDOM, BLADE_THREE_FRONT, BLADE_FIVE_RANDOM, BLADE_FIVE_FRONT};
	void	Create_AlertRect(BLADETYPE eBladeType, _fvector vDir = { 0.f });
	void	Create_BladeEffect(BLADETYPE eBladeType, _fvector vDir, _double dLongLifeTime, _double dSpeed);


#pragma endregion
private: // _bool
	_bool	m_bAtkStepType = { false };

private: // time

	_double m_dAwakeTime = { 0.0 };
	_double m_dTriggerTime = { 0.0 };

private:
	_uint	m_iRandomDirNum = { 0 };
	_uint	m_iLinkerNum = { 0 };
	_uint	m_iRandomPatternNum = { 0 };
	_uint	m_iAtkStepTypeNum = { 0 };
	_uint	m_iIdleCnt = { 0 };
	_uint	m_iTriggerCnt = { 0 };
	
private:
	PHASE   m_eCurPhase = PHASE_1;
	STATE	m_eCurstate = STATE_INTERACT;
	ANIM    m_eCurAnimIndex = ANIM_IDLE;
	ANIM	m_ePreAnimIndex = ANIM_IDLE;

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CBoss_Kyogai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END