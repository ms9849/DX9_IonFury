#pragma once

#include "Client_Defines.h"
#include "ParticleSystem.h"

NS_BEGIN(Engine)
class CTransform;
class CTexture;
NS_END

NS_BEGIN(Client)

class CYellowDust final : public CParticleSystem
{
private:
	CYellowDust(LPDIRECT3DDEVICE9 pGraphicDev);
	CYellowDust(const CYellowDust& rhs);
	virtual ~CYellowDust() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(const _float3& vPos);
	void Set_LocalPos(const _float3& vPos);
public:
	virtual void ResetParticle(PARTICLE_ATTRIBUTE* attribute);

private:
	CTransform* m_pTransformCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CYellowDust* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

NS_END

