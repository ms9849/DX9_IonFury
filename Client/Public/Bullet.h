#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
/*
final은 아닐 수 있음
*/

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CSphereCollider;
NS_END

NS_BEGIN(Client)

class CBullet final : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		_float3 vDir = {};
		_float3 vPos = {};
	} BULLET_DESC;

private:
	CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	_float Get_Damage()
	{
		return m_fDamage;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);



private:
	_float m_fDamage = { 10.f };
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CSphereCollider* m_pSphereColliderCom = { nullptr };
	_float3 m_vDir = {};

private:
	HRESULT Ready_Components();

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
