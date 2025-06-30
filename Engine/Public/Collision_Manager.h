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
	HRESULT Add_Layer_ToCollisionMgr(const _wstring& strLayerTag, class CGameObject* pGameObject);

	//GameInstance 통해서 접근 가능하게 해야 한다.
	void	Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
	void	Check_SphereCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
	void	Check_LookCollision(_float3 vPos, _float3 vLook, const _wstring& strLayerTagDst, _uint iLayerLevel, _float3* vColisionPos);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool	Sphere_Collision(class CGameObject* pSrc, class CGameObject* pDst);
	_bool	OBB_Collision(class CGameObject* pSrc, class CGameObject* pDst, _float3* vMTV);
	_bool	Look_Collision(class CGameObject* pDst, _float3* vCollisionPos, _float3 vPos, _float3 vLook);

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

NS_END