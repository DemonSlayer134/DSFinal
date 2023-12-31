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

class CMini_Map final : public CUI
{
	
public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CMini_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMini_Map(const CMini_Map& rhs);
	virtual ~CMini_Map() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private: /* 내가 사용하고하는 컴포넌트의 기능을 검색없이 바로 사용할 수 있다. */	
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
	_bool    m_Is_Dialog_On = { false };


private:
	_bool m_Is_In = { false };
	_bool m_Is_Out = { false };

	_bool m_Is_In_2 = { false };
	_bool m_Is_Out_2 = { false };

	_float m_TimeAcc = { 0.f };

private:
	UIDESC                  m_UI_Desc = {};

	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 1.f };

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

	_uint m_Map_Type = { 0 };

	_float m_UV_Cut_MinX = {};
	_float m_UV_Cut_MaxX = {};
	_float m_UV_Cut_MinY = {};
	_float m_UV_Cut_MaxY = {};

	_float m_UV_Centor_X = {};
	_float m_UV_Centor_Y = {};

	_float m_UV_Player_X = {};
	_float m_UV_Player_Y = {};

	_float m_Angle = {};

	_bool m_Is_Boss_Encounter = { false };
	_bool m_Is_Boss_Battle_Done = { false };

	_bool m_Is_Mon_Encounter = { false };
	_bool m_Is_Mon_Battle_Done = { false };

	_float m_In_Out = { 1.f };

	_float m_Map_Move = { 30.f };


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CMini_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END