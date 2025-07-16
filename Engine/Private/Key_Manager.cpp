#include "Key_Manager.h"

CKey_Manager::CKey_Manager()
{
}

HRESULT CKey_Manager::Initialize()
{
	ZeroMemory(m_bCurKeyState, sizeof(m_bCurKeyState));

	return S_OK;
}

bool CKey_Manager::Key_Pressing(_uint _iKey)
{
	return m_bCurKeyState[_iKey] && m_bPreKeyState[_iKey];
}

// 이전 프레임에 눌린 적이 없고, 지금 막 눌렀을 때
_bool CKey_Manager::Key_Down(_uint _iKey)
{
	return m_bCurKeyState[_iKey] && !m_bPreKeyState[_iKey];
}


bool CKey_Manager::Key_Up(_uint _iKey)
{
	return !m_bCurKeyState[_iKey] && m_bPreKeyState[_iKey];;
}

void CKey_Manager::Begin_Input()
{
	ZeroMemory(m_bCurKeyState, sizeof(m_bCurKeyState));

	for (int i = 0; i < VK_MAX; ++i)
	{
		if(GetAsyncKeyState(i) & 0x8000)
			m_bCurKeyState[i] = true;
	}
}

void CKey_Manager::End_Input()
{
	memcpy(m_bPreKeyState, m_bCurKeyState, sizeof(m_bPreKeyState));

	//for (int i = 0; i < VK_MAX; ++i)
	//{
	//	if ((m_bKeyState[i]) && !(GetAsyncKeyState(i) & 0x8000))
	//		m_bKeyState[i] = !m_bKeyState[i];

	//	if ((!m_bKeyState[i]) && (GetAsyncKeyState(i) & 0x8000))
	//		m_bKeyState[i] = !m_bKeyState[i];
	//}
}

CKey_Manager* CKey_Manager::Create()
{
	CKey_Manager* pInstance = new CKey_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: Key Manager");
	}

	return pInstance;
}

void CKey_Manager::Free()
{
	__super::Free();
}
