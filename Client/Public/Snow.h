#pragma once  
#include "Client_Defines.h"
#include "ParticleSystem.h"

NS_BEGIN(Engine)
class CTransform;
class CTexture;
NS_END

NS_BEGIN(Client)

class CSnow final : public CParticleSystem
{
private:
	CSnow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSnow(const CSnow& Prototype);
	virtual ~CSnow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void ResetParticle(PARTICLE_ATTRIBUTE* attribute) override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CSnow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END