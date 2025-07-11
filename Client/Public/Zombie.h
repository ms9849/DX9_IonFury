#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CZombie final : public CMonster
{
public:
	typedef struct tagZombieDesc {
		_float3 vPos;
		_bool isAwake = false;
		_bool isTarget = false;
		_bool isLeft = false;
		vector<_float3> TargetPos;
	}ZOMBIE_DESC;

private:
	CZombie(LPDIRECT3DDEVICE9 pGraphic_Device);
	CZombie(const CZombie& Prototype);
	virtual ~CZombie() = default;

	const _wstring m_strFrameKeys[15] = {
		TEXT("Zombie_Attack"), TEXT("Zombie_Die_Default"), TEXT("Zombie_Direction_NE"),
		TEXT("Zombie_Direction_NW"), TEXT("Zombie_Direction_SE"), TEXT("Zombie_Direction_SW"),
		TEXT("Zombie_Front"), TEXT("Zombie_Back"), TEXT("Zombie_Left"),
		TEXT("Zombie_Right"), TEXT("Zombie_Die_Explosion"), TEXT("Zombie_Die_Idle"),
		TEXT("Zombie_Die_HeadShot"), TEXT("Zombie_Die_HeadShot_Idle"), TEXT("Zombie_Awake")
	};

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Ready_Animations() override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider);
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

	HRESULT Ready_Components() override;
	HRESULT Begin_RenderState() override;
	HRESULT End_RenderState() override;

	virtual void Attack() override;
	void Move(_float fTimeDelta);
	void TargetMove(_float fTimeDelta, _float3 vPos);
	virtual void Move() override;

private:
	_bool m_bAnimationLock = false;
	_bool m_bFrameBlock = false;
	_bool m_isAwake = false;
	_bool m_isTarget = false;
	_bool m_isArrive = false;
	_bool m_isLeft = false;
	_float m_fSumTime = 0.f;
	_float3 m_vPos = {};
	_uint m_uPosLen = 0;
	_uint m_uIdx = 0;
	vector<_float3> m_vTargetPos;
	ZOMBIE_DESC* m_Desc = {};

public:
	static CZombie* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END