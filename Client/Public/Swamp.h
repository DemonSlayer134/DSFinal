#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSwamp final : public CGameObject
{
public:
	enum PATTERN {PATTERN_NONE, PATTERN_STEPIN, PATTERN_STEPOUT, PATTERN_DECREASE, PATTERN_SHORYU , PATTERN_THROWAWAY , PATTERN_BIG , PATTERN_TOSMALL};
	enum STATE { STATE_WAIT, STATE_INCREASING, STATE_DECREASING };

public:
	typedef struct tagSwampDesc
	{
		CGameObject* pOwner;
		CTransform*	pTransform;
		
	}EFFECTDESC;

	
private:
	CSwamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwamp(const CSwamp& rhs);
	virtual ~CSwamp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Pattern(PATTERN ePattern) { m_eCurPattern = ePattern; }

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	void	Position_Setting();
	void	Pattern_Setting(_double TimeDelta);
	void	State_Setting(_double TimeDelta);
	void	Create_WatterEffect();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	EFFECTDESC				m_EffectDesc;

	_uint					m_iNum = { 0 };

	STATE					m_eCurState = { STATE_WAIT };
	PATTERN					m_eCurPattern = { PATTERN_NONE };

	_float					m_fSize = { 0.01f };

	_float					m_fScaleSpeed = { 0.f };

	_bool					m_bWaterEffect = { false };

	_bool					m_bDown = { false };

	_double					m_dBigAccTime = { 0.0 };
public:
	static CSwamp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END