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

//HRESULT CFile_Manager::Save_Layers(LEVEL iLevel)
//{
//    JSON jLayers = JSON::array();
//
//    for (const auto& pair : m_pGameInstance->Get_Layers_InLevel(ENUM_CLASS(iLevel))) {
//        const _wstring& layerTag = pair.first;
//
//        auto pLayer = pair.second;
//        if (!pLayer
//            && layerTag != TEXT("Prototype_GameObject_Player_RightHand")
//            && layerTag != TEXT("Prototype_GameObject_Player_LeftHand")) continue;
//
//        JSON jLayer;
//        jLayer["Level"] = static_cast<_uint>(iLevel); // <-- Level 历厘
//        jLayer["Layer"] = WStringToUTF8(layerTag);
//        jLayer["Objects"] = JSON::array();
//
//        for (const auto& pObject : pLayer->Get_GameObjects()) {
//            CTransform* pTransform = dynamic_cast<CTransform*>(
//                m_pGameInstance->Get_Component(ENUM_CLASS(iLevel), layerTag, TEXT("Com_Transform"), pObject->Get_ID())
//                );
//            if (!pTransform) continue;
//
//            _float4x4 matWorld;
//            memcpy(&matWorld, pTransform->Get_WorldMatrixPtr(), sizeof(_float4x4));
//
//            JSON jObj;
//            jObj["ID"] = pObject->Get_ID();
//            jObj["LayerLevel"] = pObject->Get_Desc().iLayerLevel;
//            jObj["Proto"] = WStringToUTF8(pObject->Get_Desc().strProto);
//            jObj["ProtoLevel"] = pObject->Get_Desc().iProtoLevel;
//            if (layerTag == TEXT("Layer_Map_Objects") || layerTag == TEXT("Layer_BackGround") || layerTag == TEXT("Layer_Map_Slope"))
//                jObj["TextureIndex"] = pObject->Get_Desc().iTextureIndex;
//            if (pObject->Get_Desc().strProto == TEXT("Prototype_GameObject_Terrain"))
//                jObj["TerrainSize"] = WStringToUTF8(pObject->Get_Desc().strTerrainSize);
//            jObj["Matrix"] = {
//                { matWorld._11, matWorld._12, matWorld._13, matWorld._14 },
//                { matWorld._21, matWorld._22, matWorld._23, matWorld._24 },
//                { matWorld._31, matWorld._32, matWorld._33, matWorld._34 },
//                { matWorld._41, matWorld._42, matWorld._43, matWorld._44 },
//            };
//            jLayer["Objects"].push_back(jObj);
//        }
//
//        jLayers.push_back(jLayer);
//    }
//
//    /*jObj["Right"] = { matWorld._11, matWorld._12, matWorld._13, matWorld._14 },
//                jObj["Up"] = { matWorld._21, matWorld._22, matWorld._23, matWorld._24 },
//                jObj["Look"] = { matWorld._31, matWorld._32, matWorld._33, matWorld._34 },
//                jObj["Position"] = { matWorld._41, matWorld._42, matWorld._43, matWorld._44 },*/
//
//    // 历厘
//    string jsonStr = jLayers.dump(4);
//
//    HANDLE hFile = CreateFileW(TEXT("../../Data/LevelGamePlay.json"),
//        GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
//
//    if (hFile == INVALID_HANDLE_VALUE)
//        return E_FAIL;
//
//    DWORD written = 0;
//    WriteFile(hFile, jsonStr.c_str(), (DWORD)jsonStr.size(), &written, nullptr);
//    CloseHandle(hFile);
//
//    return S_OK;
//}

HRESULT CFile_Manager::Load_Layers(LEVEL iLevel)
{
    HANDLE hFile = CreateFileW(TEXT("../../Data/LevelGamePlay.json"),
        GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

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
            int iLevelNum = jLayer["Level"].get<int>();
            LEVEL level = static_cast<LEVEL>(iLevelNum);

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
                if (UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Map_Objects")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_BackGround")
                    || UTF8ToWString(jLayer["Layer"].get<string>()) == TEXT("Layer_Slope"))
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
    std::string result(size_needed - 1, 0); // null 力寇
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
    return result;
}

_wstring CFile_Manager::UTF8ToWString(const string& str)
{
    if (str.empty()) return {};

    _uint size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    _wstring result(size_needed - 1, 0); // null 力寇
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
  /*  if (m_ObjectDescs)
    {
        delete m_ObjectDescs;
        m_ObjectDescs = nullptr;
    }*/
}
