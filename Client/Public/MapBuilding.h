#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
NS_END

NS_BEGIN(Client)

class CMapBuilding final : public CGameObject
{
public:
	typedef struct tagCubeDesc {
		CTransform* pTransform;
		CVIBuffer_Cube* pBuffer;
	} CUBE_DESC;

private:
	CMapBuilding(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapBuilding(const CMapBuilding& Prototype);
	virtual ~CMapBuilding() = default;

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

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CMapBuilding* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END