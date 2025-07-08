#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	/* 충돌한 상대 객체와, 충돌 타입을 리턴 */
	//virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta);

	/* 충돌한 상대 객체, 충돌이 발생한 내 자신의 콜라이더, 충돌 타입을 리턴 */
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType,_float fTimeDelta);
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider);

	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);
	virtual const RAY_DESC& Get_RayDesc();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void  Set_Dead(_bool bDead) {
		m_isDead = bDead;
	}

	_bool isDead() const {
		return m_isDead;
	}

	_uint Get_ID() const {
		return m_iObjectID;
	}

	_float Get_CamDistance() const {
		return m_fCamDistance;
	}
	class CComponent* Find_Component(const _wstring& strComponentTag);

protected:
	_uint						m_iObjectID = {};
	
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_bool						m_isDead = { false };
	_float						m_fCamDistance = {};
	map<const _wstring, class CComponent*>		m_Components;

protected:
	void	Compute_CamDistance(const _float3& vWorldPos);
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END