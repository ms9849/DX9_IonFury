#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CBossUpperBody;
class CBossLowerBody;

class CBoss final : public CGameObject
{
private:
	CBoss(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss(const CBoss& Prototype);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components();
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Resurrection();
	;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override { return; };
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override {
		COLLISION_DESC temp = {};
		temp.pCollider = nullptr;
		temp.pColliderSecond = nullptr;
		return temp;
	};

private:
	//void RotateToPlayer(CTransform* pTranform);
	void RotateWithParentTransform();

private:
	CTransform* m_pTransformCom = { nullptr };
	CBossUpperBody* m_pBossUpperBody = { nullptr };
	CBossLowerBody* m_pBossLowerBody = { nullptr };

	_bool isUpperLive = true;
	_float m_fResurrectionTime = 10.f;
	_float m_fSumResurrectionTime = 0.f;

public:
	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END