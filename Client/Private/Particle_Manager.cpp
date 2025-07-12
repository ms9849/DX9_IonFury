#include "Particle_Manager.h"

#include "GameInstance.h"
#include "Blood.h"
#include "YellowDust.h"

IMPLEMENT_SINGLETON(CParticle_Manager);

CParticle_Manager::CParticle_Manager() :
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CParticle_Manager::Initialize(LEVEL eLevelID)
{
	if (FAILED(Ready_Particle_Blood()))
		return E_FAIL;

	if (FAILED(Ready_Particle_YellowDust()))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticle_Manager::Ready_Particle_Blood()
{
	/*
	파티클들 미리 풀링.

	수업 코드의 GameInstance에 존재하던 Clone Prototype을 통해 파티클 오브젝트를 받아온다.
	*/
	auto iter = m_Particles.find(TEXT("Particle_Blood"));
	if (iter != m_Particles.end())
		return S_OK;

	list<CParticleSystem*> Particles = {};

	for (_int i = 0; i < 50; ++i)
	{
		Particles.push_back(
			static_cast<CParticleSystem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Blood"), nullptr))
		);
	}

	m_Particles.emplace(TEXT("Particle_Blood"), Particles);
	return S_OK;
}

HRESULT CParticle_Manager::Ready_Particle_YellowDust()
{
	auto iter = m_Particles.find(TEXT("Particle_YellowDust"));
	if (iter != m_Particles.end())
		return S_OK;

	list<CParticleSystem*> Particles = {};

	for (_int i = 0; i < 50; ++i)
	{
		Particles.push_back(
			static_cast<CParticleSystem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_YellowDust"), nullptr))
		);
	}

	m_Particles.emplace(TEXT("Particle_YellowDust"), Particles);

	return S_OK;
}

void CParticle_Manager::Create_Particle(const _wstring& strParticleTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const _float3& vPos, const _float3& vLocalPos)
{
	auto Particles = m_Particles.find(strParticleTag);

	if (Particles == m_Particles.end())
		return;

	for (auto& pParticle : Particles->second)
	{
		// 파티클은 dead 처리 어떻게 할건지 잘 생각해봐야 한다.
		
		if (pParticle->isDead() == false && pParticle->isPlaying() == false)
		{
			if (strParticleTag == TEXT("Particle_Blood"))
			{
				static_cast<CBlood*>(pParticle)->Set_Pos(vPos);
				pParticle->Reset();
			}
			else if (strParticleTag == TEXT("Particle_YellowDust"))
			{
				static_cast<CYellowDust*>(pParticle)->Set_Pos(vPos);
				pParticle->Reset();
			}

			Safe_AddRef(pParticle);
			pParticle->Set_Playing(true);
			m_pGameInstance->Add_Clone_ToLayer(pParticle, iLayerLevelIndex, strLayerTag);

			break;
		}
		else if (pParticle->isDead() == true && pParticle->isPlaying() == false)
		{
			pParticle->Set_Dead(false);
		}
	}
}

void CParticle_Manager::Release_Particle_Manager()
{
	DestroyInstance();
}

void CParticle_Manager::Free()
{
	__super::Free();

	/*파티클 리스트 비워주기*/
	for (auto& Particles : m_Particles)
	{
		for (auto& pParticle : Particles.second)
		{
			Safe_Release(pParticle);
		}
		Particles.second.clear();

	}
	Safe_Release(m_pGameInstance);
}
