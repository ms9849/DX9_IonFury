#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CLayer;
NS_END

NS_BEGIN(Client)

class CFile_Manager : public CBase
{
private:
    CFile_Manager();
    virtual ~CFile_Manager() = default;

public:
    HRESULT Initialize();
    //HRESULT Save_Layers(LEVEL iLevel);
    HRESULT Load_Layers(LEVEL iLevel);
    map<_wstring, vector<CGameObject::GAMEOBJECT_DESC>>* Get_ObjectDescs();

private:
    class CGameInstance* m_pGameInstance = { nullptr };

private:
    string WStringToUTF8(const _wstring& wstr);
    _wstring UTF8ToWString(const string& str);
    map<_wstring, vector<CGameObject::GAMEOBJECT_DESC>>* m_ObjectDescs{};

public:
    static CFile_Manager* Create();
    virtual void Free() override;
};

NS_END
