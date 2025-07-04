#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager);
private:
	CEffect_Manager();
	CEffect_Manager(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Initialize();
	void Create_Effect(const _wstring& strEffectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	void Release_Effect_Manager();
private:
	map<const _wstring, list<class CEffect*>> m_Effects;
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END