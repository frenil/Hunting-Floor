#include "stdafx.h"
#include "IndRes.h"

CIndRes::CIndRes()
{
}

CIndRes::~CIndRes()
{
}

bool CIndRes::OnCreate()
{
	CreateDirect3DDevice();
	return true;
}

void CIndRes::CreateDirect3DDevice()
{
	HRESULT hResult;

	// DXGI Factory 생성
	hResult = ::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	// Adapter (출력장치)
	ComPtr<IDXGIAdapter1> pd3dAdapter = nullptr;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0, 
			_uuidof(ID3D12Device), (void **)&m_pd3dDevice))) 
			break;
	}

	// !Device
	if (!m_pd3dDevice)
	{
		// Direct 11으로 생성
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	// 특성 레벨 11을 지원하는 디바이스를 생성할 수 없을때 WARP 디바이스를 생성
	if (!pd3dAdapter) {
		ComPtr<IDXGIAdapter> pWarpAdapter;
		m_pdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
		D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_9_3, IID_PPV_ARGS(&m_pd3dDevice));
	}

	// 그래도 !Device
	if (!m_pd3dDevice)
	{
		// 종료 메세지
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}
}
