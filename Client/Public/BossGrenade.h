#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Bullet.h"
/*
final은 아닐 수 있음
*/

NS_BEGIN(Client)

class CBossGrenade final : public CBullet
{
public:
	typedef struct tagBulletDesc
	{
		_float3 vDir = {};
		_float3 vStartPos = {};
		_float3 vPlayerPos = {};
		_float3 vOffSet = {};
		_float3 vBulletScale = {};
		_float fBulletSpeed = {};
		_float fAngle = {};
		_bool	isPlayerBullet = false;
	} BULLET_DESC;

private:
	CBossGrenade(LPDIRECT3DDEVICE9 pGraphicDev);
	CBossGrenade(const CBossGrenade& Prototype);
	virtual ~CBossGrenade() = default;

public:
	_float Get_Damage()
	{
		return m_fDamage;
	}

	_bool Get_PlayerBullet()
	{
		return m_bPlayerBullet;
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
	HRESULT Ready_Components();

private:
	_float m_fAngle = {};
	_float m_fElapsedTime = 0.f;
	_float m_fGravity = 9.8f;
	_float m_fBulletSpeed = 10.f;
	_float3 m_vStartPos = {};
	_float3 m_vPlayerPos = {};
	_float3 m_vOffSet = {};
	_float3 m_vVelocity = {};
	_float3 m_vBulletScale = {};
	_float3 m_vScale = {};

public:
	static CBossGrenade* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
