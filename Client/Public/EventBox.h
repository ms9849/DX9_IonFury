#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CBoxCollider;
NS_END

NS_BEGIN(Client)

class CEventBox : public CGameObject
{
private:
	CEventBox(LPDIRECT3DDEVICE9 pGraphicDev);
	CEventBox(const CGameObject& Prototype);
	virtual ~CEventBox() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();

private:
    CTransform* m_pTransformCom = { nullptr };
    CBoxCollider* m_pBoxColliderCom = { nullptr };

public:
    virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
    virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override;

public:
	static CEventBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
