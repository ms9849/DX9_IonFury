#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "UIHp.h"
#include "UIBullets.h"
#include "UIInteraction.h"
#include "UIAim.h"
#include "UIArmor.h"

NS_BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Cube(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Items(const _wstring& strLayerTag);

private:
	CUIHp* m_pUIHp{ nullptr };
	CUIArmor* m_pUIArmor{ nullptr };
	CUIBullets* m_pUIBullets{ nullptr };
	CUIInteraction* m_pUIInteraction{ nullptr };
	CUIAim* m_pUIAim{ nullptr };
	CCamera::CAMERA_CONFIG m_CameraSettings;

public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END