#pragma once
#include "stdafx.h"

#include <fbxsdk.h>
#include <vector>
using namespace std;
class FbxLoad
{
	FbxManager* g_pFbxSdkManager = nullptr;
public:
	FbxLoad();
	~FbxLoad();

	HRESULT LoadFBX(vector<XMFLOAT3>* pOutVertexvector);
};

