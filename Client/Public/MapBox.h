#pragma once

#include "Client_Defines.h"
#include "CubeObject.h"

NS_BEGIN(Client)

class CMapBox final : public CCubeObject
{
private:
	CMapBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapBox(const CMapBox& Prototype);
	virtual ~CMapBox() = default;

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
	virtual HRESULT Ready_Components() override;


public:
	static CMapBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END