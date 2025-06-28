#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
/*
final은 아닐 수 있음
*/

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
NS_END

NS_BEGIN(Client)

class CBullet final : public CGameObject
{
private:
	CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

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
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CBullet* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
