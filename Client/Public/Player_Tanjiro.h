#pragma once

#include "Client_Defines.h"
#include "Player.h"

#include "Sword.h"
#include "SwordHome.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer_Tanjiro final : public CPlayer
{
public:
	enum ANIM {
		ANIM_ADV_IDLE = 0,
		ANIM_ADV_JUMP = 1, //1~4
		ANIM_ADV_SWORD_PUTOUT = 5, ANIM_ADV_SWORD_PUTIN = 6, 
		ANIM_ADV_RUN = 9, ANIM_ADV_RUN_END = 10,
		ANIM_ADV_RUN_SAPUN = 11, ANIM_ADV_RUN_SAPUN_END = 12, 
		ANIM_ADV_SLIDE = 15,
		ANIM_ADV_TALK = 16,
		ANIM_ADV_TOUCH = 17, //17~19
		ANIM_ATK_COMBO = 21, // 21~23, 24 25 26 은 분기 27컷씬레디, 28컷씬
		ANIM_ATK_AIRCOMBO = 29,
		ANIM_ATK_CHARGE = 31, // 33이 어택임
		ANIM_ATK_SKILL_GUARD = 34, ANIM_ATK_SKILL_MOVE = 38, ANIM_ATK_SKILL_NORMAL = 42,
		ANIM_ATK_THROW = 47, ANIM_ATK_THROW_CUTSCENE = 48,
		ANIM_ATK_AIRTRACK = 49, // 49~51
		ANIM_BATTLE_AWAKEN = 54, ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE = 55,
		ANIM_BATTLE_GUARD = 63, ANIM_BATTLE_GUARD_HIT_BIG = 66, ANIM_BATTLE_GUARD_HIT_SMALL = 67, ANIM_BATTLE_GUARD_PUSH = 68,
		ANIM_DEATH = 75,
		ANIM_BATTLE_DASH = 79, // 79~80, 81end
		ANIM_BATTLE_IDLE = 82,
		ANIM_BATTLE_JUMP = 83,
		ANIM_BATTLE_RUN = 87, ANIM_BATTLE_RUN_END = 89,
		ANIM_BATTLE_STEP_AB = 91, ANIM_BATTLE_STEP_AF = 92, ANIM_BATTLE_STEP_AL = 93, ANIM_BATTLE_STEP_AR = 94,
		ANIM_BATTLE_STEP_B = 95, ANIM_BATTLE_STEP_F = 96, ANIM_BATTLE_STEP_L = 97, ANIM_BATTLE_STEP_R = 99,
		ANIM_ATK_SPECIAL_CUTSCENE = 101, ANIM_ATK_SPECIAL_READY = 107,
		ANIM_DMG_SMALL = 74, ANIM_DMG_SMALL_RETURN = 62, ANIM_DMG_BIG_RETURN = 61, ANIM_DMG_BIG = 129,
		ANIM_DMG_SMALL_UPPER = 130, 
		ANIM_BATTLESTART = 115,
		ANIM_DMG_AIR_SMALL_CONNECT_0 = 116, ANIM_DMG_AIR_SMALL_CONNECT_1 = 117, ANIM_DMG_AIR_SMALL_CONNECT_2 = 130,
		ANIM_DMG_SWAMPBIND = 118,
		ANIM_DMG_BLOW = 119, // 119~121
		ANIM_FALL = 124, //124,125idle, 126착지
		ANIM_GBLOW = 128,
		ANIM_DMG_SPIN = 131, // 131~133
		ANIM_DOWN = 136,
		ANIM_DOWN_GETUP_MOVE = 137, // 137~138, 82
		ANIM_DOWN_GETUP = 134, //134~135, 61, 82
		ANIM_ADV_STEALTH_IDLE = 143,
		ANIM_ADV_STEALTH_WALK = 144, //144~146
		ANIM_ADV_STEALTH_WALK_L180 = 147, ANIM_ADV_STEALTH_WALK_L90 = 148,
		ANIM_ADV_STEALTH_WALK_R180 = 149, ANIM_ADV_STEALTH_WALK_R90 = 150,

		ANIM_END = 151
	};

private:
	CPlayer_Tanjiro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Tanjiro(const CPlayer_Tanjiro& rhs);
	virtual ~CPlayer_Tanjiro() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //애니메이션 제어용 함수들
	void	EventCall_Control(_double dTimeDelta);

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Battle_Move(_double dTimeDelta);
	void	Animation_Control_Battle_Jump(_double dTimeDelta);
	void	Animation_Control_Battle_Attack(_double dTimeDelta);
	void	Animation_Control_Battle_Charge(_double dTimeDelta);
	void	Animation_Control_Battle_Skill(_double dTimeDelta);
	void	Animation_Control_Battle_Guard(_double dTimeDelta);
	void	Animation_Control_Battle_Dash(_double dTimeDelta);
	void	Animation_Control_Battle_Awaken(_double dTimeDelta);
	void	Animation_Control_Battle_Special(_double dTimeDelta);

	void	Animation_Control_Battle_Dmg(_double dTimeDelta);

	void	Animation_Control_Adventure_Move(_double dTimeDelta);
	void	Animation_Control_Adventure_Act(_double dTimeDelta);

	void	Player_Change(_double dTimeDelta);

	void	Moving_Restrict();

	void	Event_Tanjiro_Death(_double dTimeDelta);

private:
	void	Smell_Detection(_double dTimeDelta);

private:
	void	Create_SwampWaterParticleEffect(_double dTimeDelta);	// 안원 ( 물에 빠졌을떄)

	//사운드용
	void	Play_Sound_Atk(_int iType, _float vol); // 0:small, 1:medium, 2:big
	void	Play_Sound_Dmg(_int iType, _float vol);

private:
	CSword* m_pSword = { nullptr };
	CSwordHome* m_pSwordHome = { nullptr };

	//스케일 조정
	_float m_fScaleChange = { 0.8f };
	

	//잠입모드 변경
	_bool	m_isStealthMode = { false };
	_bool	m_isPlayerBack_Tanjiro = { false };
	_double m_dDelay_PlayerBack_Tanjiro = { 0.0 };

	

	//자코 전투 직후,
	_double m_dDelay_ZakoBattle_After = { 0.0 };
	_bool	m_isFirst_ZakoBattle_After = { true };
	_bool	m_isFirst_ZakoBattle_After_0 = { true };
	_bool	m_isFirst_ZakoBattle_After_1 = { true };

	_bool	m_isZakoBeforeBattle = { false };
	_double m_dDelay_ZakoBattle_Before = { 0.0 };

	_bool	m_isFirst_ZakoBattle_Before_0 = { true };
	_bool	m_isFirst_ZakoBattle_Before_1 = { true };


private: //사운드
	_bool	m_isSound_Awaken = { false };
	_double m_dSound_Awaken_Complete = { 0.0 };


private: //애니메이션 제어용 변수들
	//Move
	_float	m_fMove_Speed = { 20.0f  }; 


	//오의 용
	_double		m_dTime_Special_Ready = { 0.0 };
	_double		m_dTime_Special_CutScene = { 0.0 };

	//오의 컷씬용
	vector<_double>		m_vecTime;
	_bool		m_isSpecialOn = { false };

	_bool		m_isFirst_Special_Jump = { true };
	_bool		m_isSecond_Special_Jump = { true };


	//어드벤처 점프
	_bool		m_isBoxJumping = { false };
	_double		m_dDelay_BoxJump = { 0.0 };
	
	_float4		m_vTanjrioToBoxDir = { 0.0f, 0.0f, 0.0f, 0.0f };


	// 위치 지정
	_float4 m_ResetPos[4];
	_int	m_iResetIndex = { 0 }; // 3이 자코방 앞

	//스왐프 관련
	_bool	m_isFirst_SwampUi = { true };
	_bool	m_isFirst_SwampHit = { true };
	_double m_dDelay_SwampHit_Again = { 0.0 };

	_bool	m_isSwampUpper = { false };


	//사망 이벤트
	_bool	m_isPlayerTanjiroDead = { false };
	
	_bool	m_isFirst_Event_Death = { true };
	
	_double		m_dDelay_TanjiroDead = { 0.0 };

	_bool	m_isFirst_Dead_0 = { true };
	_bool	m_isFirst_Dead_1 = { true };
	_bool	m_isFirst_Dead_2 = { true };
	_bool	m_isFirst_Dead_3 = { true };
	_bool	m_isFirst_Dead_4 = { true };
	_bool	m_isFirst_Dead_5 = { true };
	_bool	m_isFirst_Dead_6 = { true };
	_bool	m_isFirst_Dead_7 = { true };
	_bool	m_isFirst_Dead_8 = { true };
	_bool	m_isFirst_Dead_9 = { true };
	_bool	m_isFirst_Dead_10 = { true }; 
	

private:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };

private:
	_double m_dWaterEffectAccTime = { 0.0 };	// 물에 빠졌을때 이펙트 용(안원)

	_double m_dAwakenParticleAccTime = { 0.0 };


private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	

public:
	static CPlayer_Tanjiro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END