#pragma once

class CIndRes
{
public:
	CIndRes();
	~CIndRes();
	
	bool OnCreate();
	void CreateDirect3DDevice();

	ID3D12Device*	d3dDevice()		{ return m_pd3dDevice.Get(); }
	IDXGIFactory4*	d3dFactory()	{ return m_pdxgiFactory.Get(); }
private:
	ComPtr<ID3D12Device>	m_pd3dDevice;
	ComPtr<IDXGIFactory4>	m_pdxgiFactory;
};