#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CWebManager final : public CBase
{
	DECLARE_SINGLETON(CWebManager)

private:
	CWebManager();
	virtual ~CWebManager() = default;

	
public: // �Լ�
	void	Tick(_double dTimeDelta);

	void	Make_WebBullet(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime,
		CAtkCollider::ATK_TYPE AtkType, _vector vAtkDir, _float fDmg, CTransform* pTransform,
		_double Speed, CAtkCollider::BULLET_TYPE eBulletType, const char* pEffectTag, CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc);

	
public: // �ټ�

private: // ����
	

public:
	virtual void Free() override;
};

END