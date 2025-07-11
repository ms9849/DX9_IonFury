#include "Snow.h"

#include "GameInstance.h"

CSnow::CSnow(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CParticleSystem{ pGraphic_Device }
{
}

CSnow::CSnow(const CSnow& Prototype) :
    CParticleSystem{ Prototype }
{
}

HRESULT CSnow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSnow::Initialize(void* pArg)
{
	srand((unsigned)time(NULL));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PARTICLESYSTEM_DESC* pDesc = static_cast<PARTICLESYSTEM_DESC*>(pArg);

	m_fSize = 0.5f;
	m_vertexBufferSize = 2048;
	m_vertexBufferOffset = 0;
	m_vertexBufferBatchSize = 512;

	int iNumParticles = pDesc->iNumParticles;

	m_vBoundaryMin = pDesc->vBoundaryMin;
	m_vBoundaryMax = pDesc->vBoundaryMax;


	for (int i = 0; i < iNumParticles; ++i)
	{
		AddParticle();
	}

	__super::Initialize(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, _float3(
		0.f,
		1.f,
		0.f
	));

	return S_OK;
}

void CSnow::Priority_Update(_float fTimeDelta)
{
}

void CSnow::Update(_float fTimeDelta)
{
	list<PARTICLE_ATTRIBUTE>::iterator iter;

	for (iter = m_Particles.begin(); iter != m_Particles.end(); ++iter)
	{
		iter->vPosition += iter->vVelocity * fTimeDelta;

		iter->fAge += fTimeDelta;

		if (iter->vPosition.y < m_vBoundaryMin.y)
		{
			iter->bisAlive = false;
			ResetParticle(&(*iter));
		}

	}
}

void CSnow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CSnow::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

 	__super::Render();

	return S_OK;
}

void CSnow::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
	attribute->bisAlive = true;

	m_pGameInstance->GetRandomVector(
		&attribute->vPosition,
		&m_vBoundaryMin,
		&m_vBoundaryMax
	);

	attribute->vPosition.y = m_vBoundaryMax.y;

	attribute->vVelocity.x = m_pGameInstance->Random_Normal()* -1.5f;
	attribute->vVelocity.y = m_pGameInstance->Random_Normal() * -1.5f;
	attribute->vVelocity.z = 0.f;

	attribute->Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	attribute->fAge = 0.f;
	attribute->fLifeTime = 2.f;
}

HRESULT CSnow::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

    return S_OK;
}

CSnow* CSnow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSnow* pInstance = new CSnow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : Snow");
	}
	return pInstance;
}

CGameObject* CSnow::Clone(void* pArg)
{
	CSnow* pInstance = new CSnow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : Snow");
	}
    return pInstance;
}

void CSnow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
}
