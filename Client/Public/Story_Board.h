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

class CStory_Board final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_X_Reverse = { false };
		_bool    m_Is_Y_Reverse = { false };

		_uint    m_Type = { 0 };
		_uint    m_Icon_Type = { 0 };
		_uint    m_Back_Type = { 0 };
		_float   m_Back_Layer = { 0 };


	}UIDESC;


private:
	CStory_Board(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStory_Board(const CStory_Board& rhs);
	virtual ~CStory_Board() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void Set_UI();


private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pTextureMaskCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};

	UIDESC                  m_UI_Desc = {};
	_float                  m_Alpha = { 1.f };
	_float                  m_Alpha_Dir = { 1.f };
	_bool                   m_Is_Reverse = { false };
	_bool                   m_Is_CutScene = false;
	_float                  m_Origin_X = {};
	_float                  m_Origin_Y = {};
	_float                  m_Size_Param = { 1.f };

	_float                  m_Origin_PosX = {};
	_float                  m_Origin_PosY = {};
	_float                  m_Move_Speed = { 2300.f };
    _bool                   m_bIs_Line_Move = { false };

	_float                  m_PosX_Dir = { 1.f };
	_float                  m_PosY_Dir = { 1.f };


private:
	_bool                   m_Select = { 0 };

	vector<wstring>         m_szTitle = { };
	vector<wstring>         m_szContent = { };

	_uint                   m_Story_Index = { 0 };
	_uint                   m_Pre_Story_Index = { 0 };


private:
	_float                  m_Time_X = {};
	_float                  m_UV_Speed_X = {};
	_float                  m_Time_Y = {};
	_float                  m_UV_Speed_Y = {};

	_float                  m_Time_Mask_X = {};
	_float                  m_Time_Mask_Dir = { 1.f };

	_float                  m_Tone = { 0.f };

	_bool                   m_Mask_Pass = { false };
	_bool                   m_Mask_Gray_Pass = { false };

	_uint                   m_Title_Story_Index = { 0 };
	_bool                   m_Title_Alpha_Change = { false };

	_uint                   m_Mini_Title_Story_Index = { 0 };
	_bool                   m_Mini_Title_Alpha_Change = { false };
	_bool                   m_Mini_Title_Change = { false };


private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CStory_Board* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END