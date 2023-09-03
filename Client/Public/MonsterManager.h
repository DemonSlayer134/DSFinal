#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CMonsterManager final : public CBase
{
	DECLARE_SINGLETON(CMonsterManager)

private:
	CMonsterManager();
	virtual ~CMonsterManager() = default;

public:
	//�ټ� ���� ���ð���
	void	Cnt_AttackOn_Up() { m_iCnt_AttackOn++; }
	void	Cnt_AttackOn_Down() { m_iCnt_AttackOn--; }
	_bool	Can_Attack();

	//��Ʋ ���� �� ����
	_bool	Get_BattleOn() { return m_isBattleOn; }
	void	Set_BattleOn(_bool battle) { m_isBattleOn = battle; }



private:
	//�ټ� ���� ���ð���
	_int	m_iCnt_AttackOn = { 0 };


	//��Ʋ ���� �� ����
	_bool	m_isBattleOn = { false };




public:
	virtual void Free() override;
};

END