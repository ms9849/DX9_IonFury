#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCollision_Manager : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize();

	//GameInstance 통해서 접근 가능하게 해야 한다.
	void	Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel);
	void	Check_RayCollision();
private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool	OBB_Collision(class CGameObject* pSrc, class CGameObject* pDst, _float3* vMTV);

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

NS_END