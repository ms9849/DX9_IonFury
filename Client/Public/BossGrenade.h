#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Bullet.h"
/*
final은 아닐 수 있음
*/
NS_BEGIN(Engine)
class CAnimation;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CBossGrenade final : public CBullet
{
public:
	/*typedef struct tagBulletDesc
	{
		_float3 vDir = {};
		_float3 vStartPos = {};
		_float3 vPlayerPos = {};
		_float3 vOffSet = {};
		_float3 vBulletScale = {};
		_float fBulletSpeed = {};
		_float fAngle = {};
		_bool	isPlayerBullet = false;
		CTransform* pPlayerTransform;
	} BULLET_DESC;*/

	typedef struct tagBulletDesc : public CBullet::tagBulletDesc
	{
		_float3 vStartPos = {};
		_float3 vPlayerPos = {};
		_float3 vOffSet = {};
		_float fAngle = {};
		CTransform* pPlayerTransform;
	} BULLET_DESC;

private:
	CBossGrenade(LPDIRECT3DDEVICE9 pGraphicDev);
	CBossGrenade(const CBossGrenade& Prototype);
	virtual ~CBossGrenade() = default;

	const _wstring m_strFrameKeys[1] = {
		TEXT("Grenade_Explosion"),
	};

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

	void RotateToPlayer(CTransform* pTranform);

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);
	virtual HRESULT Ready_Animations();

	virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();

private:
	HRESULT Ready_Components();

private:
	CAnimation* m_pAnimationCom = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };;
	CVIBuffer_Rect* m_pVIBufferCom_Rect = { nullptr };
	map<const _wstring, CTexture*> m_pTextureComs;
	_wstring m_strFrameKey = {};

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

	_bool m_bAnimateionOn = false;
	_bool m_bExplosion = false;

public:
	static CBossGrenade* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
