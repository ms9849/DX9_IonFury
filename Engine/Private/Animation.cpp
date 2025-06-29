#include "Animation.h"

CAnimation::CAnimation(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: CComponent{ Prototype }
	//, m_tFrame{ Prototype.m_tFrame }
{
}

HRESULT CAnimation::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimation::Initialize(void* pArg)
{
	return S_OK;
}

void CAnimation::Set_Animation(FRAME_DESC* pFrameDesc)
{
	m_tFrame = pFrameDesc;
}

CAnimation::FRAME_DESC* CAnimation::Get_Animation()
{
	return m_tFrame;
}

_uint CAnimation::Get_Frame_Index()
{
	return static_cast<_uint>(m_tFrame->fTime);
}

void CAnimation::Play_Animation(_float fTimeDelta)
{
	m_tFrame->fTime += m_tFrame->iEnd * (fTimeDelta / (((1 / 60.f) * m_tFrame->iFrameSpeed) * m_tFrame->iEnd));
	m_bFinished = false;

	if (m_tFrame->fTime >= m_tFrame->iEnd)
	{
		m_tFrame->fTime = 0.f;
		m_bFinished = true;
	}
}

_bool CAnimation::Check_Animation_Finish()
{
	return m_bFinished;
}

void CAnimation::Clear_Animation()
{
	m_tFrame->fTime = 0.f;
	m_bFinished = true;
}

CAnimation* CAnimation::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAnimation* pInstance = new CAnimation(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CAnimation::Clone(void* pArg)
{
	CAnimation* pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();
}
