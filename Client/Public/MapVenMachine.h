#pragma once

#include "Client_Defines.h"
#include "CubeObject.h"

NS_BEGIN(Client)

class CMapVenMachine final : public CCubeObject
{
private:
	CMapVenMachine(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapVenMachine(const CMapVenMachine& Prototype);
	virtual ~CMapVenMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos);

	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

private:
	_int m_iHp = { 100 };
	_int m_iTextureNum = { 0 };
	_bool m_bBroken = { false };
	_float m_fTimeAcc = {};
	_float m_fItemCoolDown = {};

private:
	virtual HRESULT Ready_Components() override;
	virtual void Item_Dispense(_float fTimeDelta); 

public:
	static CMapVenMachine* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END