#pragma once

#include "Client_Defines.h"
#include "ParticleSystem.h"

NS_BEGIN(Engine)
class CTransform;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBlood final : public CParticleSystem
{
private:
	CBlood(LPDIRECT3DDEVICE9 pGraphicDev);
	CBlood(const CBlood& rhs);
	virtual ~CBlood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(const _float3& vPos);

public:
	virtual void ResetParticle(PARTICLE_ATTRIBUTE* attribute) override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CBlood* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

NS_END

