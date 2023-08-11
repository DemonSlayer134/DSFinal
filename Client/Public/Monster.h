#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster : public CCharacter
{
public:
	

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();

public:
	void Get_PlayerComponent();
		
public: // Calculate
	void  Calculate_To_Player();

	_bool Check_Distance(_float fDistance);
	_float Calculate_Distance();
	_vector Calculate_Dir();
	_vector Calculate_Dir_FixY();

protected:
	//�÷��̾� ���� calculate
	_float			m_fDistance_To_Player;
	_float4		m_PlayerPos;
	_float4		m_Dir_To_Player;
	


protected: //�ִϸ��̼� ����� �Լ���
	void	Dir_Setting(_bool Reverse);


protected: // �ִϸ��̼� ����� ������
	_bool m_isDeath_Motion = { false };
	

protected:
	/* �ӽ� �ڵ� */
	_uint	m_iNumAnim = { 0 };
	// ���� Ȯ�ο�
	_uint	m_iMeshNum = { 0 };

	
protected:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;
protected:
	CTransform* m_pPlayerTransformCom = { nullptr };
	
protected:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END