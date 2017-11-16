#pragma once

class CIndRes;
class CGameFramework {
public:
	CGameFramework();
	~CGameFramework();

	void RegisterIndRes(shared_ptr<CIndRes> indres) { m_pIndRes = indres; };

	// �����ӿ�ũ �ʱ�ȭ (�� �����찡 �����Ǹ� ȣ��)
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd, shared_ptr<CIndRes> indres);

	void OnDestroy();

	// Msaa ���� �� Fence ����
	void SetMsaaQualityLevels();
	void CreateFence();
	
	// ViewPort and ScissorRect
	void SetViewportAndScissorRect();

	//���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ �����Լ�
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();

	// ���� Ÿ�� ��, ����-���ٽ� �� ����
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	// �������� �޽��� ���� ��ü ����, ���Ӱ�ü �Ҹ�
	void BuildObjects();
	void ReleaseObjects();

	// �����ӿ�ũ�� �ٽɱ��(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ�
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	// CPU�� GPU�� ����ȭ �ϴ� �Լ� (Fence���?)
	void WaitForGpuComplete();

	// ������ �޼���(Ű����, ���콺 �Է�) ó��
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


public:
	HWND GethWnd()									const { return m_hWnd; }
	shared_ptr<CIndRes> GetIndRes()					const { return m_pIndRes; }

private:
	HINSTANCE								m_hInstance;
	HWND									m_hWnd;

	int										m_nWndClientWidth;
	int										m_nWndClientHeight;

	// ��ġ ������ ����̽�
	shared_ptr<CIndRes>						m_pIndRes{ nullptr };

	// ���丮, ����ü��, ����̽� ������
	ComPtr<IDXGISwapChain3>					m_pdxgiSwapChain;		// ���÷��� ����
															// �ĸ� ������ ����
	static const UINT						m_nSwapChainBuffers = 2;
	// ���� ����ü���� �ĸ� ���� �ε���
	UINT									m_nSwapChainBufferIndex;

	// ���� ���ø� Ȱ��ȭ �� ���� ���ø� ���� ����
	bool									m_bMsaa4xEnable = false;
	UINT									m_nMsaa4xQualityLevels = 0;

	// ����Ÿ�� ����, ������ ��, �������̽� ������, ���� Ÿ�� ������ ������ ũ��
	// RenderTargetView�� �� �̿��Ͽ� Rtv�� ������.
	ComPtr<ID3D12Resource>					m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap>			m_pd3dRtvDescriptorHeap;
	UINT									m_nRtvDescriptorIncrementSize;

	// ����-���Ľ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ��
	// DepthStencilView �� �� ����ؼ� Dsv�� ������.
	ComPtr<ID3D12Resource>					m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap>			m_pd3dDsvDescriptorHeap;
	UINT									m_nDsvDescriptorIncrementSize;

	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� ������
	ComPtr<ID3D12CommandQueue>				m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator>			m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>		m_pd3dCommandList;

	// �潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ�
	ComPtr<ID3D12Fence>						m_pd3dFence;
	UINT64									m_nFenceValue;
	HANDLE									m_hFenceEvent;

#if defined(_DEBUG)
	ComPtr<ID3D12Debug>						m_pd3dDebugController;
#endif

	// ����Ʈ�� ������ �簢��
	// ���� :: Ư�� �������� �׸��� �׸���. (�ڸ������� ���ΰ� �� ���� ������ ������.)
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
};