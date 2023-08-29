#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPaper final : public CUI
{
	
public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
		_uint    m_Paper_Type = { 0 };

	}UIDESC;


private:
	CPaper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPaper(const CPaper& rhs);
	virtual ~CPaper() = default;

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

public:
	void Set_UI();
	void Get_Player_Info(_double TimeDelta);
	void Fade_In(_double TimeDelta);
	void Fade_Out(_double TimeDelta);


private:
	vector<wstring>			m_szText_0 = {};

	vector<wstring>			m_szText_1 = {};

	vector<wstring>			m_szText_2 = {};

	vector<wstring>			m_szText_3 = {};

	_int                   m_Cur_Num = { 0 };

	_bool                   m_Is_Selected = { false };

private:
	_bool m_Is_In = { false };
	_bool m_Is_Out = { false };
	_bool m_Is_Font_Render = { false };

	_float m_TimeAcc = { 0.f };

private:
	UIDESC                  m_UI_Desc = {};

	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 0.f };

	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_bool m_Pt_In = { false };

	_float m_Size_Param = {};

	_float m_Origin_Size_Param = {};


	_float m_Origin_X = {};
	_float m_Origin_Y = {};

	_float m_Origin_PosX = {};
	_float m_Origin_PosY = {};
	_float m_Start_PosX = {};
	_float m_Start_PosY = {};

	_bool m_Is_Fix = { true };


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CPaper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END