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

	void	Create_WebShot(_int TexIdx, _float4 CreatePos, _float3 Scale, _float4 ShotDir);
	void	Shoot_ArrowWeb(_float4 CreatePos, _float4 ShotDir);
	void	Shoot_WebBall(_float4 CreatePos, _float4 ShotDir);
	void	Shoot_WideWeb(_float4 CreatePos, _float4 ShotDir);
	void	Shoot_JikWeb(_float4 CreatePos, _float4 ShotDir);

	void	Make_WebBullet(_float3 Size, _float3 Pos, _vector vAtkDir, _double Speed, 
		CAtkCollider::BULLET_TYPE eBulletType, const char* pEffectTag);

public: // �ټ�
	void	Set_TransformCom(CTransform* pTransform) { 
		m_pTransformCom = pTransform;
	}

private: // ����
	CTransform* m_pTransformCom = { nullptr }; // �÷��̾��� Ʈ��������.

	//�����
	_double m_dDelay_WebGimmick_0 = { 0.0 };
	
	_double m_dDelay_WebGimmick_Full = { 0.0 };

	_int	m_iWebEffect_Type = { 0 };

public:
	virtual void Free() override;
};

END