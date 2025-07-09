#pragma once

#include "Client_Defines.h"
#include "CubeObject.h"

NS_BEGIN(Client)

class CMapWall final : public CCubeObject
{
private:
	CMapWall(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapWall(const CMapWall& Prototype);
	virtual ~CMapWall() = default;

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
	static CMapWall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END