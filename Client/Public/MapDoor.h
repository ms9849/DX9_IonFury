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

class CMapDoor final : public CGameObject
{
private:
	CMapDoor(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapDoor(const CMapDoor& Prototype);
	virtual ~CMapDoor() = default;

public:
	void Set_Door_Open(_bool bOpen);
	_bool Get_Door_Open();
	_bool Get_Door_End();
	void Door_Animation(_float fTimeDelta);

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
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CBoxCollider* m_pBoxColliderCom = { nullptr };

	_bool m_bOpen{ false };
	_bool m_bEnd{ false };
	_float m_fTimeStack{ 0.f };

private:
	virtual HRESULT Ready_Components();

public:
	static CMapDoor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END