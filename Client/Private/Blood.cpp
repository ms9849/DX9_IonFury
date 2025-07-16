#include "Blood.h"

#include "GameInstance.h"

CBlood::CBlood(LPDIRECT3DDEVICE9 pGraphicDev) :
	CParticleSystem { pGraphicDev }
{
}

CBlood::CBlood(const CBlood& rhs) :
	CParticleSystem { rhs }
{
}

HRESULT CBlood::Initialize_Prototype() 
{
	return S_OK;
}

HRESULT CBlood::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSize = 0.05f;
	m_vertexBufferSize = 2048;
	m_vertexBufferOffset = 0;
	m_vertexBufferBatchSize = 512;
	m_vBoundaryMin = _float3(-10.f, -5.f, -10.f);
	m_vBoundaryMax = _float3(10.f, 10.f, 10.f);

	__super::Initialize(pArg);

	srand((unsigned)time(NULL));

	_uint iNumParticles = 45;

	for (_uint i = 0; i < iNumParticles; ++i)
	{
		AddParticle();
	}

	m_pTransformCom->Set_State(STATE::POSITION, { 0.f, 0.f, 0.f });

	return S_OK;
}

void CBlood::Priority_Update(_float fTimeDelta)
{
}

void CBlood::Update(_float fTimeDelta)
{
	if (m_isDead)
		return;

	list<PARTICLE_ATTRIBUTE>::iterator iter;

	_bool bIsAllDead = true;

	for (iter = m_Particles.begin(); iter != m_Particles.end(); ++iter)
	{
		if (iter->vPosition.y < m_vBoundaryMin.y || iter->fAge >= iter->fLifeTime)
		{
			iter->bisAlive = false;

			// Blood는 눈처럼 계속 내리는게 아니니까 일단 False 세팅만 해둠
			// reset 파티클은 풀링해주는 매니저에서 사용하여 처리해줄 예정
			// ResetParticle(&(*iter));
		}
		else
		{

			iter->vPosition += iter->vVelocity * 0.016f;
			//올라갔다 떨어지게
			iter->vVelocity.y -= 3.0f * 0.016f;

			iter->fAge += fTimeDelta;
		}

		if (iter->bisAlive == true)
			bIsAllDead = false;
	}

	if (bIsAllDead == true)
	{
		m_isDead = true;
		m_bIsPlaying = false;
	}
}

void CBlood::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CBlood::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	__super::Render();

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CBlood::Set_Pos(const _float3& vPos)
{
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CBlood::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
	for (auto& attribute : m_Particles)
	{
		attribute.vPosition = { 0.f, 0.0f, 0.f };
	}

	attribute->bisAlive = true;

	attribute->vVelocity.x = m_pGameInstance->Random(-1.f, 1.f) * 0.7f;
	attribute->vVelocity.y = m_pGameInstance->Random_Normal() * 1.5f;
	attribute->vVelocity.z = m_pGameInstance->Random(-1.f, 1.f) * 0.7f;

	attribute->Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	attribute->fAge = 0.f;
	attribute->fLifeTime = 3.f;
}

HRESULT CBlood::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Blood"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CBlood* CBlood::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBlood* pInstance = new CBlood(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : Blood");
	}

	return pInstance;
}

CGameObject* CBlood::Clone(void* pArg)
{
	CBlood* pInstance = new CBlood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : Blood");
	}

	return pInstance;
}

void CBlood::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
