#include"stdafx.h"
#include "d3doffscreenrender.h"

#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <vector>

#define COMPILE_MULTIMON_STUBS
#include "multimon.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

CritSec g_init_critsec;

D3DOffscreenRender::D3DOffscreenRender()
{
	m_index = -1;
	m_hWnd = 0;
	m_width = 352;
	m_height = 288;

	m_pDirect3D9 = NULL;
	m_pDirect3DDevice = NULL;
	m_pDirect3DSurfaceRender = NULL;
	m_pBackBuffer = NULL;
	m_d3dformat = D3DFMT_UNKNOWN;
	m_b10bitSupport = false;
	m_BytePerSample = 0;
	m_p10_2_8_buffer = NULL;
	m_pRgb10_buffer = NULL;
	m_pRgb8_buffer = NULL;

	m_nAdapter = 0;

	m_yuv2rgb10 = yuv2rgb10_c;
	m_yuv2rgb8 = yuv2rgb8_c;

	m_8bit_d3dformat = D3DFMT_A8R8G8B8;
}

D3DOffscreenRender::~D3DOffscreenRender()
{

	AutoLock lock(&g_init_critsec);

	clean();

	if(m_p10_2_8_buffer)
	{
		free(m_p10_2_8_buffer);
		m_p10_2_8_buffer = NULL;
	}

	if(m_pRgb10_buffer)
	{
		free(m_pRgb10_buffer);
		m_pRgb10_buffer = NULL;
	}

	if(m_pRgb8_buffer)
	{
		free(m_pRgb8_buffer);
		m_pRgb8_buffer = NULL;
	}

}

int D3DOffscreenRender::init(int index, HWND hWnd, int width, int height)
{
	m_index		= index;
	m_hWnd		= hWnd;
	m_width		= width;
	m_height	= height;
	m_screenWidth = m_screenHeight = 0;

	int iret = 0;

	__try
	{
		m_d3dformat = D3DFMT_A2R10G10B10;
		OutputDebugString("D3DOffscreenRender::init，first initDirect3D D3DFMT_A2R10G10B10!\n");
		iret = initDirect3D(m_d3dformat);
		if (iret < 0)
		{
			OutputDebugString("D3DOffscreenRender::init,first initDirect3D D3DFMT_A2R10G10B10 failed!\n");
			destroyDirect3D();
			return -1;
		}
		else
		{
			m_b10bitSupport = true;
		}
	}
	__except(0,1)
	{
		iret = -1;
	}

	//destroyDirect3D();
	return iret;
}

int D3DOffscreenRender::clean()
{
	
	int iret = 0;
	__try
	{
		iret = destroyDirect3D();
	}
	__except(0,1)
	{
		iret = -1;
	}

	return iret;
}
int D3DOffscreenRender::reinitDirect3D()
{
	if(m_BytePerSample <= 1)
	{
		m_d3dformat = m_8bit_d3dformat;
		OutputDebugString("D3DOffscreenRender::render BytePerSample = 1 ， initDirect3D D3DFMT_A8R8G8B8!\n");
		int iret = initDirect3D(m_d3dformat);
		if (iret<0)
		{
			OutputDebugString("D3DOffscreenRender::render initDirect3D D3DFMT_A8R8G8B8 failed!\n");
			return -1;
		}
	}
	else
	{
		//----------------逻辑修改为10bit转成8bit渲染，这里需要创建8bit渲染-----------------
		m_d3dformat = m_8bit_d3dformat;
		//m_d3dformat = D3DFMT_A2R10G10B10;
		OutputDebugString("D3DOffscreenRender::render initDirect3D D3DFMT_A2R10G10B10!\n");
		int iret = initDirect3D(m_d3dformat);
		if (iret < 0)
		{
			m_b10bitSupport = false;
			OutputDebugString("D3DOffscreenRender::render initDirect3D D3DFMT_A2R10G10B10 failed!\n");
			m_d3dformat = m_8bit_d3dformat;
			OutputDebugString("D3DOffscreenRender::render initDirect3D D3DFMT_A8R8G8B8!\n");
			iret = initDirect3D(m_d3dformat);
			if (iret<0)
			{
				OutputDebugString("D3DOffscreenRender::render initDirect3D D3DFMT_A8R8G8B8 failed!\n");
				return -1;
			}
		}
	}

	return 0;
}

int D3DOffscreenRender::initDirect3D(D3DFORMAT _format)
{
	AutoLock lock(&g_init_critsec);

	HRESULT lRet;
	int err = 0;
	
	destroyDirect3D();
	m_d3dformat = _format;

	if (m_d3dformat == D3DFMT_A8R8G8B8)OutputDebugString("initDirect3D D3DFMT_A8R8G8B8!\n");
	else if(m_d3dformat == D3DFMT_A2R10G10B10)OutputDebugString("initDirect3D D3DFMT_A2R10G10B10!\n");
	else OutputDebugString("initDirect3D D3DFMT_YV12!\n");

	m_pDirect3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( m_pDirect3D9 == NULL )
		return -1;

	int nCount = m_pDirect3D9->GetAdapterCount();
	m_nAdapter = D3DADAPTER_DEFAULT;
	HMONITOR hMonitorCur = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	for (int i=0; i<nCount; i++)
	{
		HMONITOR hMonitor = m_pDirect3D9->GetAdapterMonitor(i);
		if (hMonitor == hMonitorCur)
		{
			m_nAdapter = i;
		}
	}

	D3DCAPS9 caps;
	m_pDirect3D9->GetDeviceCaps(m_nAdapter, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// 检查是否支持后台渲染和窗口的显示 
	D3DDISPLAYMODE d3dDisplayMode;
	lRet = m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode);
	if (FAILED(lRet))
	{
		return -1;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.BackBufferFormat = d3dDisplayMode.Format;
	d3dpp.hDeviceWindow = m_hWnd;
	d3dpp.BackBufferCount = 1;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	BOOL bret = GetClientRect(m_hWnd,&m_rtViewport);
	
	lRet=m_pDirect3D9->CreateDevice( m_nAdapter, D3DDEVTYPE_HAL,m_hWnd,
		vp,
		&d3dpp, &m_pDirect3DDevice );	
	
	if(FAILED(lRet))
	{
		char temp[256];
		sprintf(temp,"initDirect3D CreateDevice failed!left = %d,right = %d,top = %d,bottom = %d\n",
			m_rtViewport.left,m_rtViewport.right,m_rtViewport.top,m_rtViewport.bottom);
		OutputDebugStringA(temp);
		return -1;
	}

	D3DCAPS9 cap;
	lRet = m_pDirect3DDevice->GetDeviceCaps(&cap);
	if(FAILED(lRet))
	{
		OutputDebugString("initDirect3D GetDeviceCaps failed!\n");
		return -1;
	}

	int _width = m_width;
	int _height = m_height;

	lRet=m_pDirect3DDevice->CreateOffscreenPlainSurface(
		_width,_height,
		_format,
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);

	if(FAILED(lRet))
	{
		OutputDebugString("initDirect3D CreateOffscreenPlainSurface failed!\n");
		return -1;
	}

	if (m_d3dformat == D3DFMT_A8R8G8B8)OutputDebugString("initDirect3D D3DFMT_A8R8G8B8 success!\n");
	else if (m_d3dformat == D3DFMT_A2R10G10B10)OutputDebugString("initDirect3D D3DFMT_A2R10G10B10 success!\n");
	else OutputDebugString("initDirect3D D3DFMT_YV12! success\n");

	return err;
}

HRESULT D3DOffscreenRender::destroyDirect3D()
{
	OutputDebugString("destroyDirect3D\n");
	
	//AutoLock lock(&m_CritSec);

	int i = 0;
	
	if(m_pDirect3DSurfaceRender)
	{
		m_pDirect3DSurfaceRender->Release();
		m_pDirect3DSurfaceRender = NULL;
		i++;
	}
	if(m_pDirect3DDevice)
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = NULL;
		i++;
	}
	if(m_pDirect3D9)
	{
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
		i++;
	}

	if(m_pRgb10_buffer)
	{
		free(m_pRgb10_buffer);
		m_pRgb10_buffer = NULL;
	}
	if(m_pRgb8_buffer)
	{
		free(m_pRgb8_buffer);
		m_pRgb8_buffer = NULL;
	}
	OutputDebugString("destroyDirect3D over\n");

	return S_OK;
}


int D3DOffscreenRender::render(unsigned char *py, unsigned char *pu, unsigned char *pv, 
							   int width, int height,int BytePerSample)
{
	if (py==0||pu==0||pv==0||width<=0||height<=0||m_hWnd == NULL)
	{
		return -1;
	}

	check_stretch();
	
	if( NULL == MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL))
		return 0;
	
	RECT window_rect;
	memset(&window_rect, 0, sizeof(window_rect));
	GetWindowRect(m_hWnd, &window_rect);

	if (window_rect.left == window_rect.right || 
		window_rect.top == window_rect.bottom)
	{
		return 0;
	}	

	RECT client_rect;
	BOOL bret = GetClientRect(m_hWnd, &client_rect);

	if(m_pDirect3D9 == NULL || m_pDirect3DDevice == NULL|| m_pDirect3DSurfaceRender == NULL)
	{
		m_width		= width;
		m_height	= height;
		m_screenWidth = m_screenHeight = 0;

		m_BytePerSample = BytePerSample;

		int iret = reinitDirect3D();
		if(iret<0) return iret;

	} 

	if (m_rtViewport.right != client_rect.right ||
		m_rtViewport.bottom != client_rect.bottom ||
		((width * height) != (m_width*m_height)) ||
		m_BytePerSample != BytePerSample)
	{
		m_rtViewport.left = client_rect.left;
		m_rtViewport.right = client_rect.right;
		m_rtViewport.top = client_rect.top;
		m_rtViewport.bottom = client_rect.bottom;
		m_width = width;
		m_height = height;
		m_BytePerSample = BytePerSample;

		int iret = reinitDirect3D();
		if(iret<0) return iret;
	}

	if(m_BytePerSample <= 1 && m_d3dformat != m_8bit_d3dformat)
	{
		m_d3dformat = m_8bit_d3dformat;

		if(m_d3dformat == D3DFMT_A8R8G8B8)
			OutputDebugString("m_BytePerSample <= 1 ，initDirect3D D3DFMT_A8R8G8B8!\n");
		else
			OutputDebugString("m_BytePerSample <= 1 ，initDirect3D D3DFMT_YV12!\n");
		int iret = initDirect3D(m_d3dformat);
		if (iret<0)
		{
			if (m_d3dformat == D3DFMT_A8R8G8B8)
				OutputDebugString("D3DOffscreenRender::render，initDirect3D D3DFMT_A8R8G8B8 failed!\n");
			else
				OutputDebugString("D3DOffscreenRender::render，initDirect3D D3DFMT_YV12! failed\n");
			return -1;
		}
	}

	HRESULT lRet;

	if(m_pDirect3DSurfaceRender == NULL)
		return -1;
	D3DLOCKED_RECT d3d_rect;

	lRet=m_pDirect3DSurfaceRender->LockRect(&d3d_rect,NULL,D3DLOCK_DONOTWAIT);
	if(FAILED(lRet))
	{
		return -1;
	}

	if(m_BytePerSample <= 1)
	{
		if(m_8bit_d3dformat == D3DFMT_A8R8G8B8)
		{
			BOOL iret = process_8bit_rgbRender(py, pu, pv,(unsigned char*)(d3d_rect.pBits), d3d_rect.Pitch,width, height);
			if(!iret)
			{
				m_pDirect3DSurfaceRender->UnlockRect();
				OutputDebugString("D3DOffscreenRender::process_8bit_rgbRender failed!\n");
				return -1;
			}
		}
		else
		{
			yuv2yv12(py, pu, pv, (unsigned char*)(d3d_rect.pBits), d3d_rect.Pitch, width, height);
		}
	}
	else
	{
		if(m_d3dformat == D3DFMT_A2R10G10B10)
		{
			BOOL iret = process_10bit_rgbRender(py, pu, pv,(unsigned char*)(d3d_rect.pBits), d3d_rect.Pitch,width, height);
			if(!iret)
			{
				m_pDirect3DSurfaceRender->UnlockRect();
				OutputDebugString("D3DOffscreenRender::process_10bit_rgbRender failed!\n");
				return -1;
			}
		}
		else
		{
			//-------------------这里是将10bit YUV转成8bit YUV进行渲染--------------------------
			int iret = precess_10bit_2_8bit(py, pu, pv,width, height);
			if(iret)
			{
				unsigned char* _py = (unsigned char*)m_p10_2_8_buffer;
				unsigned char* _pu = (unsigned char*)m_p10_2_8_buffer +width*height;
				unsigned char* _pv = (unsigned char*)m_p10_2_8_buffer +width*height*5/4;
				
				if(m_8bit_d3dformat == D3DFMT_A8R8G8B8)
				{
					BOOL iret = process_8bit_rgbRender(_py, _pu, _pv,(unsigned char*)(d3d_rect.pBits), d3d_rect.Pitch,width, height);
					if(!iret)
					{
						m_pDirect3DSurfaceRender->UnlockRect();
						OutputDebugString("D3DOffscreenRender::process_8bit_rgbRender failed!\n");
						return -1;
					}  
				}
				else
				{
					yuv2yv12(_py, _pu, _pv, (unsigned char*)(d3d_rect.pBits), d3d_rect.Pitch, width, height);
				}

			}
		}
	}

	lRet=m_pDirect3DSurfaceRender->UnlockRect();
	if(FAILED(lRet))
	{
		OutputDebugString("D3DOffscreenRender::UnlockRect failed!\n");
		return -1;
	}

	if (m_pDirect3DDevice == NULL)
		return -1;

	m_pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	m_pDirect3DDevice->BeginScene();

	IDirect3DSurface9 * pBackBuffer = NULL;
	if(SUCCEEDED(m_pDirect3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer)))
	{
		m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender,NULL,pBackBuffer,&m_rtViewport,D3DTEXF_LINEAR);
		pBackBuffer->Release();
	}	

	m_pDirect3DDevice->EndScene();
	HRESULT hr = m_pDirect3DDevice->Present( NULL, NULL, NULL, NULL );

	int iret = 0;

	if (hr  ==  D3DERR_DEVICELOST)
	{
		if (m_pDirect3DDevice -> TestCooperativeLevel()  == D3DERR_DEVICENOTRESET)
		{
			D3DPRESENT_PARAMETERS d3dpp; 
			m_pDirect3DDevice -> Reset( &d3dpp);
			iret = initDirect3D(m_d3dformat);
		}
	}

	return iret;
}


BOOL D3DOffscreenRender::precess_10bit_2_8bit(unsigned char *py, unsigned char *pu,unsigned char *pv,
											  int width, int height)
{
	if(!py|| !pu ||!pv || width <= 0 || height <= 0)
	{
		return false;
	}

	if(m_width != width || m_height != height)
	{
		m_width = width;
		m_height = height;

		if(m_p10_2_8_buffer)
		{
			free(m_p10_2_8_buffer);
			m_p10_2_8_buffer = NULL;
		}
	}

	if(!m_p10_2_8_buffer)
	{
		m_p10_2_8_buffer = (unsigned char*)malloc(width*height*3/2);
		if(!m_p10_2_8_buffer)
		{
			return false;
		}
	}
	unsigned short* p10bitpySrc = (unsigned short*)py;
	unsigned char* p8bitpyDst = (unsigned char*)m_p10_2_8_buffer;

	for(int i = 0;i<width*height;i++)
	{
		//unsigned short src = (*p10bitpySrc)>1023?1023:(*p10bitpySrc);
		//*p8bitpyDst = m_gamma_table[src];
		*p8bitpyDst = (UINT8)((*p10bitpySrc) >> 2);
		p10bitpySrc++;
		p8bitpyDst++;
	}

	unsigned short* p10bitpuSrc = (unsigned short*)pu;
	unsigned char* p8bitpuDst = (unsigned char*)m_p10_2_8_buffer+width*height;

	for(int i = 0;i<width*height/4;i++)
	{
		*p8bitpuDst = (UINT8)((*p10bitpuSrc)>>2);
		p10bitpuSrc++;
		p8bitpuDst++;
	}

	unsigned short* p10bitpvSrc = (unsigned short*)pv;
	unsigned char* p8bitpvDst = (unsigned char*)m_p10_2_8_buffer+width*height*5/4;

	for(int i = 0;i<width*height/4;i++)
	{
		*p8bitpvDst = (UINT8)((*p10bitpvSrc)>>2);
		p10bitpvSrc++;
		p8bitpvDst++;
	}
	return true;
}

BOOL D3DOffscreenRender::process_10bit_rgbRender(unsigned char* py, 
												 unsigned char* pu, 
												 unsigned char* pv,
												 unsigned char* pDest, 
												 int Pitch,
												 int width, 
												 int height)
{
	if(m_width != width || m_height != height)
	{
		m_width = width;
		m_height = height;

		if(m_pRgb10_buffer)
		{
			free(m_pRgb10_buffer);
			m_pRgb10_buffer = NULL;
		}
	}

	if(!m_pRgb10_buffer)
	{
		m_pRgb10_buffer = (unsigned char*)malloc(width*height*4);
		if(!m_pRgb10_buffer)
		{
			return false;
		}
	}

	m_yuv2rgb10((short*)py,(short*)pu,(short*)pv,(int*)m_pRgb10_buffer,m_width,m_height);

	unsigned char* tmprgb = m_pRgb10_buffer;
	unsigned char* tmpdst = pDest;

	if (width * 4 == Pitch)
	{
		memcpy(tmpdst, tmprgb, m_width *m_height * 4);
	}
	else
	{ 
		for (int i = 0; i<m_height; i++)
		{
			memcpy(tmpdst, tmprgb, m_width * 4);
			tmprgb += width * 4;
			tmpdst += Pitch;
		}
	}
	return true;
}


BOOL D3DOffscreenRender::process_8bit_rgbRender(unsigned char* py, 
												 unsigned char* pu, 
												 unsigned char* pv,
												 unsigned char* pDest, 
												 int Pitch,
												 int width, 
												 int height)
{
	if(m_width != width || m_height != height)
	{
		m_width = width;
		m_height = height;

		if(m_pRgb8_buffer)
		{
			free(m_pRgb8_buffer);
			m_pRgb8_buffer = NULL;
		}
	}

	if(!m_pRgb8_buffer)
	{
		m_pRgb8_buffer = (unsigned char*)malloc(width*height*4);
		if(!m_pRgb8_buffer)
		{
			return false;
		}
	}

	m_yuv2rgb8(py, pu, pv, m_pRgb8_buffer, m_width, m_height);

	unsigned char* tmprgb = m_pRgb8_buffer;
	unsigned char* tmpdst = pDest;
	if (width * 4 == Pitch)
	{
		memcpy(tmpdst, tmprgb, width *height * 4);
	}
	else
	{
		for (int i = 0; i < height; i++)
		{
			memcpy(tmpdst, tmprgb, width * 4);
			tmprgb += width * 4;
			tmpdst += Pitch;
		}
	}
	return true;
}

void D3DOffscreenRender::check_stretch()
{
	m_8bit_d3dformat = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
	
	m_yuv2rgb10 = yuv2rgb10_c;
	m_yuv2rgb8 = yuv2rgb8_c;

}
