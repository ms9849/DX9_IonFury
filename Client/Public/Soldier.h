#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CSoldier final : public CMonster
{
private:
	CSoldier(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSoldier(const CSoldier& Prototype);
	virtual ~CSoldier() = default;

	const _wstring m_strFrameKeys[16] = {
		TEXT("Soldier_Attack_Front"), TEXT("Soldier_Attack_SE"), TEXT("Soldier_Attack_SW"),
		TEXT("Soldier_Die_Default"), TEXT("Soldier_Die_Explosion"), TEXT("Soldier_Die_HeadShot"),
		TEXT("Soldier_Direction_NE"), TEXT("Soldier_Direction_NW"), TEXT("Soldier_Direction_SE"),
		TEXT("Soldier_Direction_SW"), TEXT("Soldier_Front"), TEXT("Soldier_Back"),
		TEXT("Soldier_Left"), TEXT("Soldier_Right"), TEXT("Soldier_Die_Idle"),
		TEXT("Soldier_Die_HeadShot_Idle")
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
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos);
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

	HRESULT Ready_Components() override;
	HRESULT Begin_RenderState() override;
	HRESULT End_RenderState() override;
	HRESULT Begin_RenderTestState();
	HRESULT End_RenderTestState();

	virtual void Attack() override;
	void Move(_float fTimeDelta);
	virtual void Move() override;
	void MoveAnimationCheck();
	bool CheckSafeArea(const _float3& vPlayerPos);

private:
	_bool m_bAnimationLock = false;
	_bool m_isRandomMove = false;
	_float3* m_vPos = {};
	_float m_fRandomMoveTime = 0.f;
	_float m_fSightFailTime = 0.f;

public:
	static CSoldier* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
