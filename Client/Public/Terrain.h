#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
NS_END

NS_BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	typedef struct tagTerrainDesc{
		CTransform* pTransform;
		CVIBuffer_Terrain* pBuffer;
	} TERRAIN_DESC;

private:
	CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	const TERRAIN_DESC& Get_TerrainDesc() {
		return TERRAIN_DESC{ m_pTransformCom, m_pVIBufferCom };
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };	
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END