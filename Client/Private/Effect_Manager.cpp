#include "Effect_Manager.h"

#include "GameInstance.h"
#include "Effect_Pistol_Fire.h"

IMPLEMENT_SINGLETON(CEffect_Manager);

CEffect_Manager::CEffect_Manager() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize()
{
	/* 
	이펙트들 미리 풀링. 
	
	수업 코드의 GameInstance에 존재하던 Clone Prototype을 통해 이펙트를 받아온다. 
	*/

	list<class CEffect*> Effects = {};

	for (int i = 0; i < 20; ++i)
	{
		Effects.push_back(
			static_cast<CEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), 
			TEXT("Prototype_GameObject_Effect_Pistol_Fire"), 
			nullptr))
		);
	}

	m_Effects.emplace(TEXT("Effect_Pistol_Fire"), Effects);

	return S_OK;
}

void CEffect_Manager::Create_Effect(const _wstring& strEffectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	/*
	Object_Manager와 GameInstance에 Add_Clone_Prototype 메서드를 추가하여, 
	내가 가지고 있는 사본들을 바로 뿌려줄 수 있도록 한다.

	또한 GameObject가 죽었는지 체크하는 isDead 메서드와 
	이펙트의 현 프레임을 체크하는 Get_Frame() 메서드를 이용하여

	풀링이 가능한 상태인지 확인한다.

	풀링이 가능한 상태? -> Effect Frame이 0 일 때 (재생 중이 아닌 상태)
	*/

	/*
	아래 코드 보면 isDead가 True라면, 다시 false로 바꿔주고 프레임을 0으로 세팅하여
	풀링이 가능한 상태로 만들어주고 있다.
	*/

	/*
	레퍼런스 카운트 괜찮음? -> ㅇㅇ.
	이펙트들의 원본은 이펙트 매니저가 들고 있고,
	오브젝트 매니저에 잠시 뿌려주는 거니까 
	
	1. Initialize에서 이펙트 생성할 땐 원본 들게 되는거니까 safe_addRef 안씀
	2. 대신 오브젝트 매니저에 넘겨줄 때(참조시켜줄 때) Safe_AddRef,
	3. 이펙트의 재생이 오브젝트 매니저에서 끝나면, 이펙트의 m_isDead를 True로 만듬
	4. m_isDead가 True이기 때문에 Object_Manager Clear_DeadObj 에서 처리됨
	
	-> 오브젝트 매니저의 Clear_DeadObj를 보면 Safe_Release 후 Erase 해주기 때문에 레퍼런스 카운트가 멀쩡히 돌아간다.
	*/
	auto Effects = m_Effects.find(strEffectTag);
	
	if (Effects == m_Effects.end())
		return;

	for (auto& pEffect : Effects->second)
	{
		if (pEffect->Get_Frame() == 0.f)
		{
			Safe_AddRef(pEffect);
			m_pGameInstance->Add_Clone_ToLayer(pEffect, iLayerLevelIndex, strLayerTag);
			break;
		}
		else
		{
			pEffect->Set_Dead(false);
		}
	}
}

/*
싱글톤이니까, GameInstance의 Release_Engine처럼

레퍼런스 카운트를 감소시키는 메서드를 하나 만들어서 따로 작성해줌.

그냥 Safe_Release 한번 더해도 되긴 함
*/
void CEffect_Manager::Release_Effect_Manager()
{
	DestroyInstance();
}

void CEffect_Manager::Free()
{
	__super::Free();

	/*이펙트 리스트 비워주기*/
	for (auto& Effects : m_Effects)
	{
		for (auto& pEffect : Effects.second)
		{
			Safe_Release(pEffect);
		}
		Effects.second.clear();
	}

	Safe_Release(m_pGameInstance);
}
