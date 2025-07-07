#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CBoxCollider;
NS_END

NS_BEGIN(Client)

class CMapSlope final : public CGameObject
{
public:
	typedef struct tagCubeDesc {
		CTransform* pTransform;
		CVIBuffer_Cube* pBuffer;
	} CUBE_DESC;

private:
	CMapSlope(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapSlope(const CMapSlope& Prototype);
	virtual ~CMapSlope() = default;

public:
	const CUBE_DESC& Get_CubeDesc() {
		return CUBE_DESC{ m_pTransformCom, m_pVIBufferCom };
	}

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
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CBoxCollider* m_BoxColliderCom = { nullptr };

private:
	HRESULT Ready_Components();


public:
	static CMapSlope* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END