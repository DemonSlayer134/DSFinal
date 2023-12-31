#pragma once
#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_Village_Tool final : public CLevel
{
private:
	CLevel_Village_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Village_Tool() = default;

public:
	virtual _uint	Get_NextLevel() {
		return (_uint)LEVEL_LOADING;
	};

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT	Ready_Lights();
	HRESULT	Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject(const _tchar* pLayerTag);

private:
	HRESULT	Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag);	// �� ������Ʈ �Ľ�(�ȿ�)

public:
	static			CLevel_Village_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void	Free() override;
};

END