#ifndef D3DRENDER_H
#define D3DRENDER_H

#include "utils.h"
#include "savc/colorspace.h"
#include <windows.h>
#include <d3d9.h>
#include <stdio.h>

class D3DOffscreenRender
{
public:
	// ���캯��
	D3DOffscreenRender();
	// ��������
	~D3DOffscreenRender();
	// �ӿں���
	int init(int index, HWND hWnd, int width, int height);	

	int render(unsigned char *py, unsigned char *pu, unsigned char *pv, 
		int width, int height,
		int BytePerSample);	

	int clean();

	void ChangehWnd(HWND hWnd);

private:
	int initDirect3D(D3DFORMAT _format);

	// �ڲ�����
	HRESULT destroyDirect3D();
private:
	// �����Ĳ���
	int	m_index;
	int	m_width;	// Ҫ�����Ļ�ͼ����Ŀ��
	int	m_height;	// Ҫ�����Ļ�ͼ����ĸ߶�
	// ���ھ��
	HWND	m_hWnd;

	D3DFORMAT m_d3dformat;
	bool m_b10bitSupport;

	RECT m_rtViewport;

	int	m_screenWidth;
	int	m_screenHeight;

	IDirect3D9 *m_pDirect3D9;
	IDirect3DDevice9* m_pDirect3DDevice;
	IDirect3DSurface9* m_pDirect3DSurfaceRender;

	IDirect3DSurface9* m_pBackBuffer;

	int m_nAdapter;

	int m_BytePerSample;

	unsigned char m_gamma_table[1024];
	unsigned char m_gamma_table_flag;

	unsigned char* m_p10_2_8_buffer;

	unsigned char* m_pRgb10_buffer;
	unsigned char* m_pRgb8_buffer;

	D3DFORMAT m_8bit_d3dformat;

	yuv2rgb10_func m_yuv2rgb10;
	yuv2rgb8_func m_yuv2rgb8;

private:

	BOOL precess_10bit_2_8bit(unsigned char *py, unsigned char *pu,unsigned char *pv,
		int width, int height);

	BOOL process_10bit_rgbRender(unsigned char* py, 
		unsigned char* pu, 
		unsigned char* pv,
		unsigned char* pDest, 
		int Pitch,
		int width, 
		int height);

	BOOL process_8bit_rgbRender(unsigned char* py, 
		unsigned char* pu, 
		unsigned char* pv,
		unsigned char* pDest, 
		int Pitch,
		int width, 
		int height);

	int reinitDirect3D();

	void check_stretch();
};

#endif /* D3DRENDER_H */
