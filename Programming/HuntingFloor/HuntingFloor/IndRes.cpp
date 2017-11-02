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

	// DXGI Factory ����
	hResult = ::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	// Adapter (�����ġ)
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
		// Direct 11���� ����
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	// Ư�� ���� 11�� �����ϴ� ����̽��� ������ �� ������ WARP ����̽��� ����
	if (!pd3dAdapter) {
		ComPtr<IDXGIAdapter> pWarpAdapter;
		m_pdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
		D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_9_3, IID_PPV_ARGS(&m_pd3dDevice));
	}

	// �׷��� !Device
	if (!m_pd3dDevice)
	{
		// ���� �޼���
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}
}
