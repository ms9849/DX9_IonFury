#pragma once

#include "Client_Defines.h"
#include "CubeObject.h"

NS_BEGIN(Client)

class CMapElevator final : public CCubeObject
{
private:
	CMapElevator(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapElevator(const CMapElevator& Prototype);
	virtual ~CMapElevator() = default;

public:
	void Set_Elevator_Active(_bool bActive);
	_bool Get_Elevator_End();
	void Elevator_Animation(_float fTimeDelta);

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
	_bool m_bUp{ true };
	_bool m_bActive{ false };
	_bool m_bEnd{ true };
	_float m_fTimeStack{ 0.f };

private:
	virtual HRESULT Ready_Components() override;


public:
	static CMapElevator* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END