#pragma once

#include "Component.h"

/* �׺���̼��� �����ϴ� �ﰢ������ ��Ƽ� �����Ѵ�. */
/* �׺���̼��� �̿��ϴ� ��ü�� � �쿡 �����ϴ����� ���� �ε����� �����Ѵ�.  */
/* ��ȿ����� �����ӿ����� �Ǵ��� �����Ѵ�. */
BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_int	iCurrentIndex = { -1 };
	}NAVIDESC;

protected:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	vector<class CCell*>		Get_Cells() { return m_Cells; }
	CCell* Get_Cell(_uint iIndex) {
		return m_Cells[iIndex];
	}

	const _int		Get_CurCellIndex() {
		return m_NaviDesc.iCurrentIndex;
	}

	void	Set_CurIndex(const _int iCurIndex) { m_NaviDesc.iCurrentIndex = iCurIndex; }
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

public: /* �׺���̼��� �����ϴ� ������ ������ �̿�(�ִ�3��)�� �����Ѵ�.*/
	HRESULT	SetUp_Neighbors();
	_bool	is_MoveOnNavigation(_fvector & vPositio);
	_vector	MoveOnNavigation(_fvector& vPosition, _fvector vLook);
	_fvector Compute_Height(class CTransform* pOwnerTransform);
	_vector  SlidingVector(_fvector vPosition, _fvector vLook, _int* pNeighborIndex);

private:
	vector<class CCell*>			m_Cells;
	NAVIDESC						m_NaviDesc;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext , const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END

