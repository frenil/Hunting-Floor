#pragma once

class CIndRes
{
public:
	CIndRes();
	~CIndRes();
	
	bool OnCreate();

	void CreateDirect3DDevice();

private:
	ComPtr<ID3D12Device>	m_pd3dDevice;
	ComPtr<IDXGIFactory4>	m_pdxgiFactory;
};