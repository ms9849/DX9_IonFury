#include "CFile_Manager.h"

#include "GameInstance.h"
#include "Layer.h"

CFile_Manager::CFile_Manager() :
    m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFile_Manager::Initialize()
{
    return S_OK;
}

HRESULT CFile_Manager::Load_Layers(LEVEL iLevel)
{
    HANDLE hFile{ nullptr };

    if (iLevel == LEVEL::GAMEPLAY)
    {
        hFile = CreateFileW(TEXT("../../Data/LevelGamePlay.json"),
            GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    }
    else if (iLevel == LEVEL::BOSSFIGHT)
    {
        hFile = CreateFileW(TEXT("../../Data/LevelBossFight.json"),
            GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    }

    if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;

    DWORD fileSize = GetFileSize(hFile, nullptr);
    string buffer(fileSize, 0);
    DWORD bytesRead = 0;
    ReadFile(hFile, &buffer[0], fileSize, &bytesRead, nullptr);
    CloseHandle(hFile);

    try {
        JSON jData = JSON::parse(buffer);

        m_ObjectDescs = new map<_wstring, vector<CGameObject::GAMEOBJECT_DESC>>;

        m_ObjectDescs->clear();

        for (const auto& jLayer : jData) {
            _wstring layerName = UTF8ToWString(jLayer["Layer"].get<string>());
            const auto& jObjects = jLayer["Objects"];

            vector<CGameObject::GAMEOBJECT_DESC> objectList;

            for (const auto& jObj : jObjects) {
                CGameObject::GAMEOBJECT_DESC desc = {};
                desc.iObjectID = jObj["ID"].get<_uint>();
                desc.strProto = UTF8ToWString(jObj["Proto"].get<string>());
                desc.iProtoLevel = jObj["ProtoLevel"].get<_uint>();
                desc.strLayer = layerName;
                desc.iLayerLevel = jObj["LayerLevel"].get<_uint>();
                if (UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_Deco")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_AABB")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_AABB_Ride")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_OBB")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_OBB_Ride")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_Ride")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_Gate")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects_Deco")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_BackGround"))
                    desc.iTextureIndex = jObj["TextureIndex"].get<_uint>();

                if (UTF8ToWString(jObj["Proto"].get<string>()) == TEXT("Prototype_GameObject_Terrain"))
                    desc.strTerrainSize = UTF8ToWString(jObj["TerrainSize"].get<string>());

                const auto& mat = jObj["Matrix"];
                desc.matWorld = {
                    mat[0][0], mat[0][1], mat[0][2], mat[0][3],
                    mat[1][0], mat[1][1], mat[1][2], mat[1][3],
                    mat[2][0], mat[2][1], mat[2][2], mat[2][3],
                    mat[3][0], mat[3][1], mat[3][2], mat[3][3]
                };

                objectList.push_back(desc);
            }

            m_ObjectDescs->emplace(layerName, move(objectList));
        }

        return S_OK;
    }
    catch (...) {
        return E_FAIL;
    }
}

map<_wstring, vector<CGameObject::GAMEOBJECT_DESC>>* CFile_Manager::Get_ObjectDescs()
{
    return m_ObjectDescs;
}

string CFile_Manager::WStringToUTF8(const _wstring& wstr)
{
    if (wstr.empty()) return {};

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size_needed - 1, 0); // null 제외
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
    return result;
}

_wstring CFile_Manager::UTF8ToWString(const string& str)
{
    if (str.empty()) return {};

    _uint size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    _wstring result(size_needed - 1, 0); // null 제외
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);
    return result;
}

CFile_Manager* CFile_Manager::Create()
{
    CFile_Manager* pInstance = new CFile_Manager();

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CFile_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    if (m_ObjectDescs)
    {
        for (auto& pair : *m_ObjectDescs)
            pair.second.clear();

        m_ObjectDescs->clear();

        delete m_ObjectDescs;
        m_ObjectDescs = nullptr;
    }
}
