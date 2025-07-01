#include "GameInstance.h"

#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Renderer.h"
#include "Key_Manager.h"
#include "Sound_Manager.h"
#include "Collision_Manager.h"
#include "Picking.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, LPDIRECT3DDEVICE9* ppOut)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWindowMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppOut);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	
	m_pRenderer = CRenderer::Create(*ppOut);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create();
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppOut, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pObject_Manager->Clear_DeadObj();

	m_pLevel_Manager->Update(fTimeDelta);

	/*
	키 매니저의 업데이트는 가장 마지막에 돌아야 합니다
	절대 바꾸시면 안돼요
	절대로
	진짜 절대로
	*/
	m_pKey_Manager->Update();
}

HRESULT CGameInstance::Draw()
{
	m_pRenderer->Render();

	m_pLevel_Manager->Render();

	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pPrototype_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
}

_float CGameInstance::Random_Normal()
{
	return static_cast<_float>(rand()) / RAND_MAX;	
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + Random_Normal() * (fMax - fMin);	
}

_bool CGameInstance::CCW(_float3 vSrc, _float3 vDst)
{
	_float fSum = vSrc.x * vDst.y - vSrc.y * vDst.x;

	if (fSum < 0) 
		return true;

	return false;
}

#pragma region GRAPHIC_DEVICE

void CGameInstance::Render_Begin(D3DXCOLOR Color)
{
	m_pGraphic_Device->Render_Begin(Color);
}

void CGameInstance::Render_End(HWND hWnd)
{
	m_pGraphic_Device->Render_End(hWnd);
}

#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(pNewLevel);
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iLevelIndex, strPrototypeTag, pArg);;
}

#pragma endregion

#pragma region OBJECT_MANAGER

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);	
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

CGameObject* CGameInstance::Find_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Find_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, pArg);
}

CLayer* CGameInstance::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLayerLevelIndex, strLayerTag);
}
map<const _wstring, class CLayer*> CGameInstance::Get_Layers_InLevel(_uint iLayerLevelIndex)
{
	return m_pObject_Manager->Get_Layers_InLevel(iLayerLevelIndex);
}
#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

#pragma endregion

#pragma region KEY_MANAGER

bool CGameInstance::Key_Pressing(_uint _iKey)
{
	return m_pKey_Manager->Key_Pressing(_iKey);
}

bool CGameInstance::Key_Up(_uint _iKey)
{
	return m_pKey_Manager->Key_Up(_iKey);
}

bool CGameInstance::Key_Down(_uint _iKey)
{
	return m_pKey_Manager->Key_Down(_iKey);
}
#pragma endregion

#pragma region SOUND_MANAGER
void CGameInstance::PlaySoundOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	m_pSound_Manager->PlaySoundOnce(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(CHANNELID eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

#pragma endregion

#pragma region COLLISION_MANAGER
void CGameInstance::Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta)
{
	m_pCollision_Manager->Check_OBBCollision(strLayerTagSrc, strLayerTagDst, iLayerLevel, fTimeDelta);
}

void CGameInstance::Check_RayCollision(_float3 vPos, _float3 vLook, const _wstring& strLayerTagDst, _uint iLayerLevel, _float3* vColisionPos)
{
	m_pCollision_Manager->Check_RayCollision(vPos, vLook, strLayerTagDst, iLayerLevel, vColisionPos);
}
void CGameInstance::Check_SphereCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta)
{
	m_pCollision_Manager->Check_SphereCollision(strLayerTagSrc, strLayerTagDst, iLayerLevel, fTimeDelta);
}
#pragma endregion 

#pragma region PICKING

_bool CGameInstance::Picking_InWorldSpace(const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	return m_pPicking->Picking_InWorldSpace(vRayPos, vRayDir, vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::Picking_InLocalSpace(const _float4x4* pWorldMatrixInverse, const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	return m_pPicking->Picking_InLocalSpace(pWorldMatrixInverse, vRayPos, vRayDir, vPointA, vPointB, vPointC, pOut);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
	DestroyInstance();

	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPicking);
}

void CGameInstance::Free()
{
	__super::Free();
}
