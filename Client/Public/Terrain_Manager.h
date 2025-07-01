#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CLayer;
class CGameInstance;
class CLandObject;
NS_END

NS_BEGIN(Client)

class CTerrain_Manager final : public CBase
{
private:
	CTerrain_Manager();
	virtual ~CTerrain_Manager() = default;

public:
	void Add_LandObject(LEVEL eLevelID, const _wstring& strLayerTag);
	void Add_Terrian(LEVEL eLevelID);

public:
	HRESULT Initialize();
	void Check_Landing();

private:
	class CGameInstance* m_pGameInstance = {};

	list<class CTerrain*> m_Terrains = {};
	list<class CLandObject*> m_LandObjects = {};

public:
	//디바이스 장치 필요 없지.
	static CTerrain_Manager* Create();
	virtual void Free() override;
};

NS_END

