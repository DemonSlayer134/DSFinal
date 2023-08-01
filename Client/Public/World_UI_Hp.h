#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)

class CWorld_UI_Hp final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CWorld_UI_Hp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWorld_UI_Hp(const CWorld_UI_Hp& rhs);
	virtual ~CWorld_UI_Hp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private: /* ���� ����ϰ��ϴ� ������Ʈ�� ����� �˻����� �ٷ� ����� �� �ִ�. */	
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CTransform*              m_pTransformCom = { nullptr };
public:
	void Get_Boss_Info(_double TimeDelta);
	void Set_Personal_Pos();

private:	
	_double m_Boss_Max_Hp = { 1 };
	_double m_Boss_Hp = { 1 };

	_double m_Boss_Max_D_Hp = { 1 };
	_double m_Boss_D_Hp = { 1 };

	_double m_Delay_TimeAcc = { 0 };
	_double m_Delay_Max_Time = { 3 };
	_bool m_Delay_Down = { false };

private:
	UIDESC                  m_UI_Desc = {};

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 1.f };

	_tchar					m_szX[MAX_PATH] = TEXT("");
	_tchar					m_szY[MAX_PATH] = TEXT("");

	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_float m_Size_Param = {};

	_int m_Origin_X = {};
	_int m_Origin_Y = {};

	_vector m_vTargetPos = {};

	
private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CWorld_UI_Hp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END