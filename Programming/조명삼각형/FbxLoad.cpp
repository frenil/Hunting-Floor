#include "stdafx.h"
#include "FbxLoad.h"


#include <fbxsdk.h>
#include <vector>
FbxLoad::FbxLoad()
{
}


FbxLoad::~FbxLoad()
{
}
HRESULT FbxLoad::LoadFBX(vector<XMFLOAT3> *pOutVertexVector, vector<XMFLOAT3>* pOutNormal, vector<XMFLOAT2>* pOutUV)
{

	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize("FLASH+RIG+1.fbx", -1, g_pFbxSdkManager->GetIOSettings());
	if (!bSuccess) return E_FAIL;

	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			fbxsdk::FbxLayerElementUV *pFbxLayerElementUV = pMesh->GetLayer(0)->GetUVs();
			FbxVector4* pVertices = pMesh->GetControlPoints();
			FbxVector4 normal;
			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);

				for (int k = 0; k < iNumVertices; k++) {
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					pMesh->GetPolygonVertexNormal(j, k, normal);
					
					XMFLOAT3 vertex = XMFLOAT3(
						(float)pVertices[iControlPointIndex].mData[0] * 30
						, (float)pVertices[iControlPointIndex].mData[1] * 30
						, (float)pVertices[iControlPointIndex].mData[2] * 30
					);
					XMFLOAT3 nor = XMFLOAT3((float)normal.mData[0], (float)normal.mData[1], (float)normal.mData[2]);
					fbxsdk::FbxVector2 fbxUv = pFbxLayerElementUV->GetDirectArray().GetAt(iControlPointIndex);
					XMFLOAT2 uv = XMFLOAT2((float)fbxUv.mData[0], (float)fbxUv.mData[1]);

					pOutVertexVector->push_back(vertex);
					pOutNormal->push_back(nor);
					pOutUV->push_back(uv);
				}
			}

		}

	}
	return S_OK;
}
