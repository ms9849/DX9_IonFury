#include "ParticleSystem.h"

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CGameObject { pGraphic_Device }
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& Prototype) :
    CGameObject{ Prototype }
    , m_pVertexBuffer{ Prototype.m_pVertexBuffer }
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticleSystem::Initialize(void* pArg)
{
    if (FAILED(m_pGraphic_Device->CreateVertexBuffer(
        m_vertexBufferSize * sizeof(PARTICLE),
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
        PARTICLE::FVF,
        D3DPOOL_DEFAULT,
        &m_pVertexBuffer,
        0)))
        return E_FAIL;

    return S_OK;
}

void CParticleSystem::Priority_Update(_float fTimeDelta)
{
}

void CParticleSystem::Update(_float fTimeDelta)
{
}

void CParticleSystem::Late_Update(_float fTimeDelta)
{
}

HRESULT CParticleSystem::Render()
{
    if (!m_Particles.empty())
    {
        // 먼저 렌더 상태를 지정하자
        if (FAILED(Begin_RenderState()))
            return E_FAIL;

        m_pGraphic_Device->SetFVF(PARTICLE::FVF);
        m_pGraphic_Device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PARTICLE));

        // 할당할 버텍스 버퍼의 오프셋과 크기를 지정해주자
        // 버텍스 버퍼의 크기를 벗어날 경우 오프셋을 0으로 초기화해준다
        if (m_vertexBufferOffset >= m_vertexBufferSize)
            m_vertexBufferOffset = 0;

        // 그래픽카드가 그릴 버텍스 버퍼 세그먼트
        PARTICLE* pParticles = {};

        m_pVertexBuffer->Lock(
            m_vertexBufferOffset * sizeof(PARTICLE),
            m_vertexBufferBatchSize * sizeof(PARTICLE),
            (void**)&pParticles,
            m_vertexBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

        DWORD iNumParticlesInBatch = 0;

        // 버텍스 버퍼의 오프셋과 크기를 알아냈으니 복사를 시작하자
        list<PARTICLE_ATTRIBUTE>::iterator iter;
        for (iter = m_Particles.begin(); iter != m_Particles.end(); ++iter)
        {
            if (iter->bisAlive)
            {
                // 일단 전 프레임에서 생존한 파티클을
                // 다음 버텍스 버퍼 세그먼트로 복사하자
                pParticles->vPosition = iter->vPosition;
                pParticles->Color = iter->Color;
                ++pParticles;
                ++iNumParticlesInBatch;

                // 이번 프레임의 복사작업을 마쳤다면 파티클을 그리자
                if (iNumParticlesInBatch == m_vertexBufferBatchSize)
                {
                    m_pVertexBuffer->Unlock();

                    m_pGraphic_Device->DrawPrimitive(
                        D3DPT_POINTLIST,
                        m_vertexBufferOffset,
                        m_vertexBufferBatchSize);

                    // 그래픽카드와 동시에 작업할 것이기에
                    // 다음 프레임의 파티클을 채우자
                    m_vertexBufferOffset += m_vertexBufferBatchSize;

                    // 할당할 버텍스 버퍼의 오프셋과 크기를 지정해주자
                  // 버텍스 버퍼의 크기를 벗어날 경우 오프셋을 0으로 초기화해준다
                    if (m_vertexBufferOffset >= m_vertexBufferSize)
                        m_vertexBufferOffset = 0;


                    m_pVertexBuffer->Lock(
                        m_vertexBufferOffset * sizeof(PARTICLE),
                        m_vertexBufferBatchSize * sizeof(PARTICLE),
                        (void**)&pParticles,
                        m_vertexBufferOffset ? D3DLOCK_NOOVERWRITE :
                        D3DLOCK_DISCARD);

                    iNumParticlesInBatch = 0;
                }
            }
        }

        m_pVertexBuffer->Unlock();

        if (FAILED(End_RenderState()))
            return E_FAIL;
    }

    return S_OK;
}

/*
파티클들을 리셋해주는 함수
*/
void CParticleSystem::Reset()
{
    list<PARTICLE_ATTRIBUTE>::iterator iter;

    for (iter = m_Particles.begin(); iter != m_Particles.end(); ++iter)
    {
        // ResetParticle 함수는 추상 메소드이기문에
        // 무조건 자식 클래스에서 재정의 해줘야한다.
        ResetParticle(&(*iter));
    }

}

void CParticleSystem::AddParticle()
{
    PARTICLE_ATTRIBUTE Attribute = {};
    ResetParticle(&Attribute);

    m_Particles.push_back(Attribute);
}

_bool CParticleSystem::isEmpty()
{
    return m_Particles.empty();
}

void CParticleSystem::removeDeadParticles()
{
    list<PARTICLE_ATTRIBUTE>::iterator iter;

    iter = m_Particles.begin();

    while (iter != m_Particles.end())
    {
        if (iter->bisAlive == false)
            iter = m_Particles.erase(iter);
        else
            iter++;

    }
}

HRESULT CParticleSystem::Begin_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE, TypetoDW(m_fSize));
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE_MIN, TypetoDW(0.f));

    // POINTSCALE_A,B,C를 통해 거리에 따른 파티클 크기를 제어하자
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_A, TypetoDW(0.f));
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_B, TypetoDW(0.f));
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_C, TypetoDW(1.f));

    // 알파 블렌딩을 적용시키자
    // 텍스쳐의 알파채널을 받아와 블렌딩을 해준다
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    return S_OK;
}

HRESULT CParticleSystem::End_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);


    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    return S_OK;
}

void CParticleSystem::Free()
{
    __super::Free();
}
