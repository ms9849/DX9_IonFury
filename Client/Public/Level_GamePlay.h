#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Camera.h"

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
	HRESULT Ready_Objects_By_JSON();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Particle(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Cube(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Items(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map_Objects(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map_Slope(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Interaction_Objects(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Bullet(const _wstring& strLayerTag);

private:
	class CBullet_Manager* m_pBullet_Manager = {};
	class CParticle_Manager* m_pParticle_Manager = {};
	class CEffect_Manager* m_pEffect_Manager = {};
	class CTerrain_Manager* m_pTerrain_Manager = {};
	class CUIHp* m_pUIHp { nullptr };
	class CUIArmor* m_pUIArmor { nullptr };
	class CUIBullets* m_pUIBullets { nullptr };
	class CUIInteraction* m_pUIInteraction { nullptr };
	class CUIAim* m_pUIAim { nullptr };
	class CUICardKey* m_pUICardKey { nullptr };

	CCamera::CAMERA_CONFIG m_CameraSettings;
	vector<class CUIItemQueue*> m_pUIItemQueues{};

	// FPS 계산 변수
	_float m_fFPSTimer{ 0.f }, m_fTimeDelta{ 0.f };
	_uint m_iFPSCount{ 0 }, m_iCurrentFPS{ 0 };

	// json
	map<_wstring, vector<CGameObject::GAMEOBJECT_DESC>>* m_ObjectDescs{};

public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END