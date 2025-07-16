#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CParticleSystem;
NS_END 

NS_BEGIN(Client)

class CParticle_Manager final : public CBase
{
	DECLARE_SINGLETON(CParticle_Manager);
private:
	CParticle_Manager();
	virtual ~CParticle_Manager() = default;

public:
	HRESULT Initialize(LEVEL eLevelID);
	HRESULT Ready_Particle_Blood();
	HRESULT Ready_Particle_YellowDust();

	void Create_Particle(const _wstring& strParticleTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const _float3& vPos, const _float3& vLocalPos = {});

	void Release_Particle_Manager();
private:
	map<const _wstring, list<CParticleSystem*>> m_Particles;
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END