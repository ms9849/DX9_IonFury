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

class CBullet : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		_float fBulletSpeed = {};
		_float fDuration = {};
		_float3 vDir = {};
		_float3 vPos = {};
		_float3 vBulletScale = {};
		_bool	isPlayerBullet = {};
	} BULLET_DESC;

protected:
	CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	_float Get_Damage() { return m_fDamage; }
	_bool Get_PlayerBullet() { return m_bPlayerBullet; }
	const RAY_DESC& Get_RayDesc();
	void Set_Desc(const BULLET_DESC& Desc);
	void Set_Fired(_bool bFlag) { m_bisFire = bFlag; }
	_bool Get_Fired() { return m_bisFire; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider) override;

	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

protected:
	_bool m_bisFire = { false };
	_bool m_bPlayerBullet = { false };
	_float m_fDamage = { 10.f };
	_float m_fBulletSpeed = { 15.f };
	_float m_fDuration = 5.f;
	_float m_fSumTime = 0.f;
	_float3 m_vDir = {};
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CSphereCollider* m_pSphereColliderCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
