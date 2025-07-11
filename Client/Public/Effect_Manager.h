#pragma once

#include "Client_Defines.h"
#include "Effect.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager);
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Initialize(LEVEL eLevelID);
	HRESULT Ready_Pistol_Fire(LEVEL eLevelID);
	HRESULT Ready_Boss_Die(LEVEL eLevelID);
	HRESULT Ready_Grenade_Explosion(LEVEL eLevelID);

	void Create_Effect(const _wstring& strEffectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const _float3& vPos);

	void Release_Effect_Manager();

private:
	map<const _wstring, list<class CEffect*>> m_Effects = {};
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END