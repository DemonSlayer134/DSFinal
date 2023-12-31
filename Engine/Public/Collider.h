#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	enum DATA { DATA_ORIGIN, DATA_CURRENT, DATA_END};

public:
	typedef struct tagColliderDesc
	{
		_float3 vPosition;
		_float3 vSize;
		_float3 vRotation;
	}COLLIDERDESC;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	_float Get_Collider() {
		return m_pSphere[DATA_ORIGIN]->Radius;
	}
	_bool	Get_Coll() {
		return m_isColl;
	}

	COLLIDERDESC Get_Desc() {
		return m_ColliderDesc;
	}

	_bool	Get_SphereOff() { return m_isSphereOff; }
	_bool	Get_Hit_Small() { return m_isHit_Small; }
	_bool	Get_Hit_ConnectSmall() { return m_isHit_ConnectSmall; }
	_bool	Get_Hit_Big() { return m_isHit_Big; }
	_bool	Get_Hit_Blow() { return m_isHit_Blow; }
	_bool	Get_Hit_BigBlow() { return m_isHit_BigBlow; }
	_bool	Get_Hit_Spin() { return m_isHit_Spin; }
	_bool	Get_Hit_Upper() { return m_isHit_Upper; }
	_bool	Get_Hit_Bound() { return m_isHit_Bound; }
	_bool	Get_Hit_CutScene() { return m_isHit_CutScene; }
	_bool	Get_Hit_Hekireki() { return m_isHit_Hekireki; }
	_bool	Get_Hit_Swamp() { return m_isHit_Swamp; }
	_bool	Get_Hit_Web() { return m_isHit_Web; }

	_float4		Get_AtkDir() { return m_AtkDir; }
	_float	Get_fDamage() { return m_fDamage; }
	_bool	Get_Death() { return m_isDeath; }

	_bool	Get_CanHit() { return m_isCanHit; }
public:
	void	Set_Coll(_bool isColl) {
		m_isColl = isColl;
	}


	void	Set_SphereOff(_bool SphereOff) { m_isSphereOff = SphereOff; }
	void	Set_Hit_Small(_bool Hit) { m_isHit_Small = Hit; }
	void	Set_Hit_ConnectSmall(_bool Hit) { m_isHit_ConnectSmall = Hit; }
	void	Set_Hit_Big(_bool Hit) { m_isHit_Big = Hit; }
	void	Set_Hit_Blow(_bool	Hit) { m_isHit_Blow = Hit; }
	void	Set_Hit_BigBlow(_bool Hit) { m_isHit_BigBlow = Hit; }
	void	Set_Hit_Spin(_bool Hit) { m_isHit_Spin = Hit; }
	void	Set_Hit_Upper(_bool Hit) { m_isHit_Upper = Hit; }
	void	Set_Hit_Bound(_bool Hit) { m_isHit_Bound = Hit; }
	void	Set_Hit_CutScene(_bool Hit) { m_isHit_CutScene = Hit; }
	void	Set_Hit_Hekireki(_bool Hit) { m_isHit_Hekireki = Hit; }
	void	Set_Hit_Swamp(_bool Hit) { m_isHit_Swamp = Hit; }
	void	Set_Hit_Web(_bool Hit) { m_isHit_Web = Hit; }


	void	Set_AtkDir(_float4 Dir) { m_AtkDir = Dir; }
	void	Set_fDamage(_float fDmg) { m_fDamage = fDmg; }
	void	Set_Death(_bool Dead) { m_isDeath = Dead; }

	void	Set_Color(_float4 vColor) { m_vColor = vColor; }

	void	Set_CanHit(_bool CanHit) { m_isCanHit = CanHit; }
public:
	void ReMake_Collider(TYPE eColliderType, _fmatrix TransformMatrix);
	void ReMake_Collider(_float fRadius);
	void ReMake_Collider(_float3 vCenter, _float fRadius, _fmatrix TransformMatrix);

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;
	void			Tick(_fmatrix TransformMatrix, _double dTimeDelta);

public:
	_bool			Intersect(CCollider* pTargetCollider);
	_vector			ComputePushVec(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	virtual HRESULT			Render() override;
#endif // _DEBUG

private:
	BoundingBox*			m_pAABB[DATA_END] = { nullptr };
	BoundingOrientedBox*	m_pOBB[DATA_END] = { nullptr };
	BoundingSphere*			m_pSphere[DATA_END] = { nullptr };

	TYPE					m_eColliderType = { TYPE_END };
	COLLIDERDESC			m_ColliderDesc;
	_bool					m_isColl = { false };
	_double					m_dCoolTime = { 3.0 };

	//For. Attack Collider
	_bool		m_isSphereOff = { false };
	_bool		m_isHit_Small = { false };
	_bool		m_isHit_ConnectSmall = { false };
	_bool		m_isHit_Big = { false };
	_bool		m_isHit_Blow = { false };
	_bool		m_isHit_Spin = { false };
	_bool		m_isHit_Upper = { false };
	_bool		m_isHit_BigBlow = { false };
	_bool		m_isHit_Bound = { false };
	_bool		m_isHit_CutScene = { false };
	_bool		m_isHit_Hekireki = { false };
	_bool		m_isHit_Swamp = { false };
	_bool		m_isHit_Web = { false };

	_bool		m_isCanHit = { true };

	_float4		m_AtkDir = { 0.f, 0.f, 0.f, 0.f };
	_float		m_fDamage = { 0.0f };
	
	_bool		m_isDeath = { false };

	// For. Color Collider
	_float4		m_vColor = { 0.f , 1.f , 0.f  , 1.f };

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pPrimitiveBatch = { nullptr };
	BasicEffect*									m_pEffects = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END