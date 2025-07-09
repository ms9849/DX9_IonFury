#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CLayer;
class CGameInstance;
class CLandObject;
class CGameObject;
NS_END

NS_BEGIN(Client)

class CTerrain_Manager final : public CBase
{
private:
	CTerrain_Manager();
	virtual ~CTerrain_Manager() = default;

public:
	void Add_LandObject(LEVEL eLevelID, const _wstring& strLayerTag);
	void Add_LandObject_One(class CGameObject* pClone);
	void Add_Terrian(LEVEL eLevelID);
	//큐브 형태 오브젝트들을 담는 메서드. 추후 로직의 분리를 위해 리스트도 따로 관리한다.
	void Add_Cube(LEVEL eLevelID);

public:
	HRESULT Initialize();
	void Check_Landing();

private:
	class CGameInstance* m_pGameInstance = {};

	list<class CTerrain*> m_Terrains = {};
	list<class CMapSlope*> m_CubeObjects = {};
	
	/* LandObject는 아직 수정 X */
	list<class CLandObject*> m_LandObjects = {};

public:
	//디바이스 장치 필요 없지.
	static CTerrain_Manager* Create();
	virtual void Free() override;
};

NS_END

