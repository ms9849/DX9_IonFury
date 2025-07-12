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
	void	Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
	void	Check_AABBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
	void	Check_SphereCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
	void	Check_RayCollision(const _float3& vPos, const _float3& vLook, const _wstring& strLayerTagDst, _uint iLayerLevel, _float3* vColisionPos);
	//충돌한 녀석의 법선벡터까지 구해준다.
	void	Check_RayToAABBCollision(const _wstring& strLayerTagRay, const _wstring& strLayerTagAABB, _uint iLayerLevel, _float fTimeDelta, _float3* vColisionPos);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool	Sphere_Collision(class CGameObject* pSrc, class CGameObject* pDst);
	_bool	OBB_Collision(class CGameObject* pSrc, class CGameObject* pDst, _float3* vMTV);
	_bool	AABB_Collision(class CGameObject* pSrc, class CGameObject* pDst, _float3* vMTV);
	_bool	Look_Collision(class CGameObject* pDst, _float3* vCollisionPos, _float3 vPos, _float3 vLook);
	_bool	RayToAABB_Collision(class CGameObject* pRay, class CGameObject* pAABB, _float3* vPos, CComponent** pCollider, _float fTimeDelta, _float3* vPlaneNormal);

	_bool	Check_inAABB(const _float3& vPos, const _float3& vMin, const _float3 vMax);

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

NS_END