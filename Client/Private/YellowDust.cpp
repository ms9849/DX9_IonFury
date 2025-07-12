#include "YellowDust.h"

#include "GameInstance.h"

CYellowDust::CYellowDust(LPDIRECT3DDEVICE9 pGraphicDev) :
    CParticleSystem { pGraphicDev }
{
}

CYellowDust::CYellowDust(const CYellowDust& rhs) :
    CParticleSystem { rhs }
{
}

HRESULT CYellowDust::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CYellowDust::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSize = 0.01f;
	m_vertexBufferSize = 2048;
	m_vertexBufferOffset = 0;
	m_vertexBufferBatchSize = 512;
	m_vBoundaryMin = _float3(-10.f, -5.f, -10.f);
	m_vBoundaryMax = _float3(10.f, 10.f, 10.f);

	__super::Initialize(pArg);

	srand((unsigned)time(NULL));

	_uint iNumParticles = 30;

	for (_uint i = 0; i < iNumParticles; ++i)
	{
		AddParticle();
	}

	m_pTransformCom->Set_State(STATE::POSITION, { 0.f, 0.f, 0.f });

	return S_OK;
}

void CYellowDust::Priority_Update(_float fTimeDelta)
{
}

void CYellowDust::Update(_float fTimeDelta)
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
			iter->vVelocity.y -= 5.0f * 0.016f;

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

void CYellowDust::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CYellowDust::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	__super::Render();

	return S_OK;
}

void CYellowDust::Set_Pos(const _float3& vPos)
{
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CYellowDust::Set_LocalPos(const _float3& vLocalPos)
{
	for (auto& attribute : m_Particles)
	{
		attribute.vPosition = vLocalPos;
	}
}

void CYellowDust::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
	for (auto& attribute : m_Particles)
	{
		attribute.vPosition = { 0.f, 0.f, 0.f };
	}

	attribute->bisAlive = true;

	attribute->vVelocity.x = m_pGameInstance->Random(-1.f, 1.f) * 0.5f;
	attribute->vVelocity.y = m_pGameInstance->Random_Normal() * 1.2f;
	attribute->vVelocity.z = m_pGameInstance->Random(-1.f, 1.f) * 0.5f;

	attribute->Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	attribute->fAge = 0.f;
	attribute->fLifeTime = 3.f;
}

HRESULT CYellowDust::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_YellowDust"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

    return S_OK;
}

CYellowDust* CYellowDust::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CYellowDust* pInstance = new CYellowDust(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : YellowDust");
	}

	return pInstance;
}

CGameObject* CYellowDust::Clone(void* pArg)
{
	CYellowDust* pInstance = new CYellowDust(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : YellowDust");
	}

	return pInstance;
}

void CYellowDust::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
