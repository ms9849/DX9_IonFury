#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	list<class CGameObject*> Get_GameObjects();
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject();

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Clear();
	void Clear_DeadObj();

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END