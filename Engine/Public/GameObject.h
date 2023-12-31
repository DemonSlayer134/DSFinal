#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagWorldInfo
	{
		XMFLOAT3	vScale;
		XMFLOAT3	vAxis;
		float		fDegree;
		XMFLOAT4	vPosition;
	}WORLDINFO;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
	
public:
	_bool	Get_Dead() const {	//삭제처리용
		return m_isDead;
	}
	_bool	Get_Collect() const {	//오브젝트 풀용
		return m_isCollect;
	}

	void Set_Dead(_bool isDead = true) {			//삭제처리용
		m_isDead = isDead;
	}
	void Set_Collect(_bool isCollect = true) {			//오브젝트 풀용
		m_isCollect = isCollect;
	}

	_bool Get_TimeFree() const {    // 시간제약용
		return m_Is_TimeFree;
	}

	_float					Get_ViewZ(void) {
		return m_fViewZ;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_double dTimeDelta);
	virtual void	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth(); // Shadow
	virtual HRESULT Render_SSAO(); // Shadow

public:
	class CComponent*	Get_Component(const _tchar * pComponentTag);

public:
	HRESULT				Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ class CComponent** ppOut, void* pArg = nullptr);

public:
	class CComponent*	Find_Component(const _tchar* pComponentTag);

protected:
	void					Compute_ViewZ(const _fvector vPos);

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	WORLDINFO				m_WorldInfo;
	_bool					m_isDead = { false };
	_bool					m_isCollect = { false };

	// 시간제약 체크용
	_bool                   m_Is_TimeFree = { false };

	// Sorting용
	_float					m_fViewZ = 0.f;

protected:
	unordered_map<const _tchar*, class CComponent*>		m_Components;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END