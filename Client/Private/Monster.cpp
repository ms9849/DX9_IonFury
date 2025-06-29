#include "Monster.h"

#include "Bullet.h"
#include "BehaviorNode.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CMonster::CMonster(const CMonster& Prototype)
	: CLandObject{ Prototype }
{

}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize()
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	//if (m_pPlayerTransform == nullptr)
	//	return E_FAIL;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	//if (FAILED(Ready_Animations()))
	//	return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	
}

void CMonster::Update(_float fTimeDelta)
{

}

void CMonster::Late_Update(_float fTimeDelta)
{

}

HRESULT CMonster::Render()
{

	return S_OK;
}

HRESULT CMonster::Ready_Animations()
{
	return S_OK;
}

//HRESULT CMonster::Ready_Components()
//{
	///* Com_Transform */
	//CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
	//	TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
	//	return E_FAIL;

	///* Com_Texture */
	//for (size_t i = 0; i < (sizeof(m_strFrameKeys) / sizeof(m_strFrameKeys[0])); ++i)
	//{
	//	_tchar strPrototypeTag[256];
	//	_tchar strComponentTag[256];

	//	CTexture* pTextureCom{ nullptr };

	//	wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_Monster_%s"), m_strFrameKeys[i].c_str());
	//	wsprintf(strComponentTag, TEXT("Com_%s_Texture"), m_strFrameKeys[i].c_str());

	//	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), strPrototypeTag,
	//		strComponentTag, reinterpret_cast<CComponent**>(&pTextureCom))))
	//		return E_FAIL;
	//	Safe_AddRef(pTextureCom);

	//	m_pTextureComs.emplace(m_strFrameKeys[i], pTextureCom);
	//}

	///* Com_Animation */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
	//	TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimationCom))))
	//	return E_FAIL;

	///* Com_VIBuffer */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;	

	///* Com_Sight */
	//CSight::SIGHT_DESC		SightDesc{ 5.f, D3DXToRadian(90.0f), m_pPlayerTransform, m_pTransformCom };
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Sight"),
	//	TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
	//	return E_FAIL;

	//return S_OK;
//}

HRESULT CMonster::Begin_RenderState()
{
	return S_OK;
}

HRESULT CMonster::End_RenderState()
{
	return S_OK;
}

void CMonster::Attack()
{

}

void CMonster::Move()
{
}

void CMonster::Free()
{
	__super::Free();

	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}
}
