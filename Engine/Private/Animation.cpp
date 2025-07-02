#include "Animation.h"

CAnimation::CAnimation(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: CComponent(Prototype)
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

void CAnimation::Set_Animation(const _wstring strFrameKey, FRAME_DESC FrameDesc)
{
	m_tFrames.emplace(strFrameKey, FrameDesc);
}

CAnimation::FRAME_DESC* CAnimation::Get_Frame_Desc(const _wstring strFrameKey)
{
	auto iter = m_tFrames.find(strFrameKey);
	return &iter->second;
}

_uint CAnimation::Get_Frame_Current_Index(const _wstring strFrameKey)
{
	auto iter = m_tFrames.find(strFrameKey);
	return iter->second.iCurrentFrame;
}

void CAnimation::Play_Animation(const _wstring strFrameKey, _float fTimeDelta)
{
	auto iter = m_tFrames.find(strFrameKey);

	iter->second.fTime += fTimeDelta;
	iter->second.bFinish = false;

	if (iter->second.iEnd != 0)
	{
		if (iter->second.fTime >= (fTimeDelta * iter->second.iFrameSpeed))
		{
			iter->second.fTime = 0.f;
			iter->second.iCurrentFrame++;

			if (iter->second.iCurrentFrame >= iter->second.iEnd)
			{
				iter->second.iCurrentFrame = 0;
				iter->second.bFinish = true;
			}
		}
	}
	else
	{
		iter->second.fTime = 0.f;
		iter->second.iCurrentFrame = 0;
		iter->second.bFinish = false;
	}
}

_bool CAnimation::Check_Animation_Finish()
{
	for (auto &iter : m_tFrames)
	{
		if (iter.second.bFinish == true)
		{
			iter.second.fTime = 0.f;
			iter.second.iCurrentFrame = 0;
			iter.second.bFinish = false;
			return true;
		}
	}
	return false;
}

_bool CAnimation::Check_Animation_Finish(const _wstring strFrameKey)
{
	auto iter = m_tFrames.find(strFrameKey);
	return iter->second.bFinish;
}

void CAnimation::Clear_Animation()
{
	for (auto &iter : m_tFrames)
	{
		iter.second.fTime = 0.f;
		iter.second.iCurrentFrame = 0;
		//iter.second.bFinish = true;
	}
}

void CAnimation::Clear_Animation(const _wstring strFrameKey)
{
	auto iter = m_tFrames.find(strFrameKey);
	iter->second.fTime = 0.f;
	iter->second.iCurrentFrame = 0;
	iter->second.bFinish = true;
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
