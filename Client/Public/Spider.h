#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CSpider final : public CMonster
{
private:
	CSpider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSpider(const CSpider& Prototype);
	virtual ~CSpider() = default;

	const _wstring m_strFrameKeys[11] = {
		TEXT("Spider_Attack"), TEXT("Spider_Die_Default"), TEXT("Spider_Direction_NE"), 
		TEXT("Spider_Direction_NW"), TEXT("Spider_Direction_SE"), TEXT("Spider_Direction_SW"),
		TEXT("Spider_Front"), TEXT("Spider_Back"), TEXT("Spider_Left"),
		TEXT("Spider_Right"), TEXT("Spider_Die_Idle")
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
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

	HRESULT Ready_Components() override;
	HRESULT Begin_RenderState() override;
	HRESULT End_RenderState() override;

	virtual void Attack() override;
	void Move(_float fTimeDelta);
	virtual void Move() override;

	void Jump(_float fTimeDelta, _float fJumpPower);

private:
	_bool m_bAnimationLock = false;
	_bool m_bFrameBlock = false;
	_float m_fJumpPower = 0.f;

public:
	static	CSpider* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END