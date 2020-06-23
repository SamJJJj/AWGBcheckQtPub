#ifndef COLORSPACE_H
#define COLORSPACE_H

#pragma warning(disable:4731)

typedef void (*color_convert_func)( \
	unsigned char *py, unsigned char *pu, unsigned char *pv, \
	unsigned char *dst,\
	unsigned long pitch, \
	int width, \
	int height);

typedef void(*yuv2rgb10_func)(short *py, short *pu, short *pv, int *prgb, int width, int height);
typedef void(*yuv2rgb8_func)(unsigned char *py, unsigned char *pu, unsigned char *pv, unsigned char *dst, int width, int height);

inline void yuv2yv12(
	unsigned char *py,
	unsigned char *pu,
	unsigned char *pv,
	unsigned char *dst,
	unsigned long pitch,
	int width,
	int height)
{
    register unsigned char *tmpdst;
	register unsigned char *y, *u, *v;
	
    register int i;
	register int halfh,halfw,halfp;

    y = py;
    u = pu;
    v = pv;
	
    halfw = (width>>1);
    halfh = (height>>1);
    halfp = (pitch>>1);
 
    tmpdst = dst;
    for (i=0;i<height;++i) 
	{
        memcpy(tmpdst, y, width);
		y+=width;
        tmpdst+=pitch;
    }

	tmpdst = dst + height*pitch;
	for(i=0;i<halfh;++i) 
	{
        memcpy(tmpdst,v,halfw);
		v+=halfw;
        tmpdst+=halfp;
    }

	tmpdst = dst + height*pitch*5/4;
	for (i=0;i<halfh;++i) 
	{
        memcpy(tmpdst,u,halfw);
		u+=halfw;
        tmpdst+=halfp;
    }

}
extern "C" __int64 yuv2uyvy16_mmx_asm(__int64, __int64, __int64, __int64, __int64, __int64, __int64);
inline void yuv2uyvy16_mmx(
	unsigned char *py,		// [esp+4+16]
	unsigned char *pu,		// [esp+8+16]
	unsigned char *pv,		// [esp+12+16]
	unsigned char *dst,		// [esp+16+16]
	unsigned long pitch,	// [esp+20+16]
	int w,					// [esp+24+16]
	int h)					// [esp+28+16]
{	
	int width = w/2;
#ifndef _WIN64
	__asm {
		push		ebp
		push		edi
		push		esi
		push		ebx
			
		mov			edx,[width];			;load width (mult of 8)
		mov			ebx,[pu]			;load source U ptr
		mov			ecx,[pv]			;load source V ptr
		mov			eax,[py]			;load source Y ptr
		mov			edi,[dst]			;load destination ptr
		mov			esi,[pitch]			;load destination pitch
		mov			ebp,[h]				;load height
			
		lea			ebx,[ebx+edx]			;bias pointers
		lea			ecx,[ecx+edx]			;(we count from -n to 0)
		lea			eax,[eax+edx*2]
		lea			edi,[edi+edx*4]
			
		neg			edx
		mov			[esp+24+16],edx
xyloop:
		movq		mm0,[ebx+edx]			;U0-U7
			
		movq		mm7,[ecx+edx]			;V0-V7
		movq		mm2,mm0					;U0-U7
			
		movq		mm4,[eax+edx*2]
		punpcklbw	mm0,mm7					;[V3][U3][V2][U2][V1][U1][V0][U0]
			
		movq		mm5,[eax+edx*2+8]
		punpckhbw	mm2,mm7					;[V7][U7][V6][U6][V5][U5][V4][U4]
			
		movq		mm1,mm0
		punpcklbw	mm0,mm4					;[Y3][V1][Y2][U1][Y1][V0][Y0][U0]
			
		punpckhbw	mm1,mm4					;[Y7][V3][Y6][U3][Y5][V2][Y4][U2]
		movq		mm3,mm2
			
		movq		[edi+edx*4+ 0],mm0
		punpcklbw	mm2,mm5					;[YB][V5][YA][U5][Y9][V4][Y8][U4]
			
		movq		[edi+edx*4+ 8],mm1
		punpckhbw	mm3,mm5					;[YF][V7][YE][U7][YD][V6][YC][U6]
			
		movq		[edi+edx*4+16],mm2
		movq		[edi+edx*4+24],mm3
			
		add			edx,8
		jnc			xyloop
			
		mov			edx,[esp+24+16]			;reload width counter
			
		test		ebp,1					;update U/V row every other row only
		jz			oddline
			
		sub			ebx,edx					;advance U pointer
		sub			ecx,edx					;advance V pointer
			
oddline:
		sub			eax,edx					;advance Y pointer
		sub			eax,edx					;advance Y pointer
			
		add			edi,esi					;advance dest ptr
			
		dec			ebp
		jne			xyloop
		
		pop			ebx
		pop			esi
		pop			edi
		pop			ebp
		emms
	}
#else
	
	yuv2uyvy16_mmx_asm((__int64)pv,width, (__int64)py, (__int64)pu, (__int64)dst, pitch,  h);
#endif
}
extern "C" __int64 yuv2yuyv16_mmx_asm(__int64, __int64, __int64, __int64, __int64, __int64, __int64);
inline void yuv2yuyv16_mmx(
	unsigned char *py,		// [esp+4+16]
	unsigned char *pu,		// [esp+8+16]
	unsigned char *pv,		// [esp+12+16]
	unsigned char *dst,		// [esp+16+16]
	unsigned long pitch,	// [esp+20+16]
	int w,					// [esp+24+16]
	int h)					// [esp+28+16]
{
	
	int width = w/2;
#ifndef _WIN64
	__asm {
		push		ebp
		push		edi
		push		esi
		push		ebx
			
		mov			edx,[width];			;load width (mult of 8)
		mov			ebx,[pu]			;load source U ptr
		mov			ecx,[pv]			;load source V ptr
		mov			eax,[py]			;load source Y ptr
		mov			edi,[dst]			;load destination ptr
		mov			esi,[pitch]			;load destination pitch
		mov			ebp,[h]				;load height
			
		lea			ebx,[ebx+edx]			;bias pointers
		lea			ecx,[ecx+edx]			;(we count from -n to 0)
		lea			eax,[eax+edx*2]
		lea			edi,[edi+edx*4]
			
		neg			edx
		mov			[esp+24+16],edx
xyloop:
		movq		mm0,[ebx+edx]			;U0-U7
			
		movq		mm7,[ecx+edx]			;V0-V7
		movq		mm1,mm0					;U0-U7
			
		movq		mm2,[eax+edx*2]			;Y0-Y7
		punpcklbw	mm0,mm7					;[V3][U3][V2][U2][V1][U1][V0][U0]
			
		movq		mm4,[eax+edx*2+8]		;Y8-YF
		punpckhbw	mm1,mm7					;[V7][U7][V6][U6][V5][U5][V4][U4]
			
		movq		mm3,mm2
		punpcklbw	mm2,mm0					;[V1][Y3][U1][Y2][V0][Y1][U0][Y0]
			
		movq		mm5,mm4
		punpckhbw	mm3,mm0					;[V3][Y7][U3][Y6][V2][Y5][U2][Y4]
			
		movq		[edi+edx*4+ 0],mm2
		punpcklbw	mm4,mm1					;[V5][YB][U5][YA][V4][Y9][U4][Y8]
			
		movq		[edi+edx*4+ 8],mm3
		punpckhbw	mm5,mm1					;[V7][YF][U7][YE][V6][YD][U6][YC]
			
		movq		[edi+edx*4+16],mm4
			
		movq		[edi+edx*4+24],mm5
		add			edx,8
			
		jnc			xyloop
			
		mov			edx,[esp+24+16]			;reload width counter
			
		test		ebp,1					;update U/V row every other row only
		jz			oddline
			
		sub			ebx,edx					;advance U pointer
		sub			ecx,edx					;advance V pointer
			
oddline:
		sub			eax,edx					;advance Y pointer
		sub			eax,edx					;advance Y pointer
			
		add			edi,esi					;advance dest ptr
			
		dec			ebp
		jne			xyloop
			
		pop			ebx
		pop			esi
		pop			edi
		pop			ebp
		emms
	}
#else
	yuv2yuyv16_mmx_asm((__int64)pv, width, (__int64)py, (__int64)pu, (__int64)dst, pitch, h);
#endif
}


inline void yuv2rgb10_c(short *py,short *pu,short *pv,int *dst,int width,int height)
{
	register unsigned int *pRGB = (unsigned int *)dst;
	register unsigned short* _py = (unsigned short*)py;
	register unsigned short* _pu = (unsigned short*)pu;
	register unsigned short* _pv = (unsigned short*)pv;

	register unsigned int tt = 0;
	register unsigned short Y = 0;
	register unsigned short U = 0;
	register unsigned short V = 0;
	register int R = 0;
	register int G = 0;
	register int B = 0;
	register unsigned int RR = 0;
	register unsigned int GG = 0;
	register unsigned int BB = 0;

	//int tick1 = GetTickCount();
	for (int i = 0; i<height; i++)
	{
		_pu = ((unsigned short*)pu) + (width / 2)*(i / 2);
		_pv = ((unsigned short*)pv) + (width / 2)*(i / 2);
		for (int j = 0; j<width; j++)
		{
			Y = *_py;
			U = *_pu;
			V = *_pv;
			_py++;
			_pu += (j & 1);
			_pv += (j & 1);

			tt = 0;
			R = ((Y << 10) + 1436 * (V - 512) + 512) >> 10;
			G = ((Y << 10) - 352 * (U - 512) - 731 * (V - 512) + 512) >> 10;
			B = ((Y << 10) + 1815 * (U - 512) + 512) >> 10;

			RR = (unsigned int)(R < 0 ? 0 : (R > 1023 ? 1023 : R));
			GG = (unsigned int)(G < 0 ? 0 : (G > 1023 ? 1023 : G));
			BB = (unsigned int)(B < 0 ? 0 : (B > 1023 ? 1023 : B));

			tt |= (BB);
			tt |= (GG << 10);
			tt |= (RR << 20);

			*pRGB = tt;
			pRGB++;
		}
	}
}


inline void yuv2rgb10_c_stretch(short *py, short *pu, short *pv, int *dst, int width, int height)
{
	register unsigned int *pRGB = (unsigned int *)dst;
	register unsigned short* _py = (unsigned short*)py;
	register unsigned short* _pu = (unsigned short*)pu;
	register unsigned short* _pv = (unsigned short*)pv;

	register unsigned int tt = 0;
	register unsigned short Y = 0;
	register unsigned short U = 0;
	register unsigned short V = 0;
	register int R = 0;
	register int G = 0;
	register int B = 0;
	register unsigned int RR = 0;
	register unsigned int GG = 0;
	register unsigned int BB = 0;

	//int tick1 = GetTickCount();
	for (int i = 0; i < height; i++)
	{
		_pu = ((unsigned short*)pu) + (width / 2)*(i / 2);
		_pv = ((unsigned short*)pv) + (width / 2)*(i / 2);
		for (int j = 0; j < width; j++)
		{
			Y = *_py;
			U = *_pu;
			V = *_pv;
			_py++;
			_pu += (j & 1);
			_pv += (j & 1);

			tt = 0;

			//R = (1192 * Y + 1634 * (V - 512) )/ 1024;
			//G = (1192 * Y - 401 * (U - 512) - 832 * (V - 512)) / 1024;
			//B = (1192 * Y + 2066 * (U - 512)) / 1024;

			R = (1192 * (Y - 64) + 1634 * (V - 512)) / 1024;
			G = (1192 * (Y - 64) - 401 * (U - 512) - 832 * (V - 512))/ 1024;
			B = (1192 * (Y - 64) + 2066 * (U - 512)) / 1024;

			RR = (unsigned int)(R < 0 ? 0 : (R > 1023 ? 1023 : R));
			GG = (unsigned int)(G < 0 ? 0 : (G > 1023 ? 1023 : G));
			BB = (unsigned int)(B < 0 ? 0 : (B > 1023 ? 1023 : B));

			tt |= (BB);
			tt |= (GG << 10);
			tt |= (RR << 20);

			*pRGB = tt;
			pRGB++;
		}
	}
}

inline void yuv2rgb8_c(unsigned char *py,
							unsigned char *pu,
							unsigned char *pv,
							unsigned char *dst,
							int width,
							int height)
{
	register unsigned int *pRGB = (unsigned int*)dst;
	register unsigned char* _py = py;
	register unsigned char* _pu = pu;
	register unsigned char* _pv = pv;

	register unsigned int tt = 0;
	register unsigned char Y = 0;
	register unsigned char U = 0;
	register unsigned char V = 0;
	register int R = 0;
	register int G = 0;
	register int B = 0;
	register int RR = 0;
	register int GG = 0;
	register int BB = 0;

	for (int i = 0; i<height; i++)
	{
		_pu = pu + (width / 2)*(i / 2);
		_pv = pv + (width / 2)*(i / 2);
		for (int j = 0; j<width; j++)
		{
			RR = BB = GG =  0;
			Y = *_py;
			U = *_pu;
			V = *_pv;
			_py++;
			_pu += (j & 1);
			_pv += (j & 1);

			R = Y + 1.403*(V-128);
			G = Y - 0.715*(V-128) - 0.344*(U-128);
			B = Y + 1.773*(U-128);

			RR = (unsigned int)(R < 0 ? 0 : (R > 255 ? 255 : R));
			GG = (unsigned int)(G < 0 ? 0 : (G > 255 ? 255 : G));
			BB = (unsigned int)(B < 0 ? 0 : (B > 255 ? 255 : B));

			tt = 0;
			tt |= (BB);
			tt |= (GG << 8);
			tt |= (RR << 16);

			*pRGB = tt;
			pRGB++;
		}
	}
}



extern "C"  __int64 yuv2rgb8_sse2_asm(__int64, __int64, __int64, __int64, __int64);
inline void yuv2rgb8_sse2(unsigned char* py, unsigned char* pu, unsigned char* pv, unsigned char* prgb, int width, int height)
{
	unsigned char* _prgb = prgb;
	int i;
	int loopcnt = width >> 2;
	int const128[8] = { 128,128,128,128 };
	int const359[8] = { 359,359,359,359};
	int const88[8] = { 88,88,88,88 };
	int const183[8] = { 183,183,183,183};
	int const454[8] = { 454,454,454,454};

	for (i = 0; i < height; i++)
	{
#ifndef _WIN64
		__asm {
			push eax
			push ebx
			push ecx
			push edx
			push esi
			push edi

			mov		eax, py
			mov		ebx, pu
			mov		ecx, pv
			mov		edx, _prgb
			mov		edi, loopcnt

			pxor		xmm0, xmm0
			movdqu		xmm7, [const128]

			yuv2rgb8_sse2_loop:
			movdqu		xmm1, [eax]
				movdqu		xmm2, [ebx]
				movdqu		xmm3, [ecx]

				pxor		xmm0, xmm0
				movdqu		xmm7, [const128]

				punpcklbw	xmm1, xmm0
				punpcklbw	xmm2, xmm0
				punpcklbw	xmm3, xmm0

				punpcklbw	xmm1, xmm0
				punpcklbw	xmm2, xmm0
				punpcklbw	xmm3, xmm0

				punpckldq	xmm2, xmm2
				punpckldq	xmm3, xmm3

				pslld		xmm1, 8			//Y<<8
				psubd		xmm2, xmm7		//U-128
				psubd		xmm3, xmm7		//V-128

				movdqa		xmm4, xmm1
				movdqa		xmm5, xmm1
				movdqa		xmm6, xmm1

				//R = Y + 359 * (V - 128) / 256;
				//R = Y + 359 * (V - 128) / 256;
				movdqu		xmm0, [const359]
				movdqa		xmm7, xmm3
				pmulld		xmm7, xmm0
				paddd		xmm4, xmm7

				//G = (Y*256 - 88 * (U - 128) - 183 * (V - 128)) / 256;
				movdqu		xmm0, [const88]
				movdqa		xmm7, xmm2
				pmulld		xmm7, xmm0
				psubd		xmm5, xmm7

				movdqu		xmm0, [const183]
				movdqa		xmm7, xmm3
				pmulld		xmm7, xmm0
				psubd		xmm5, xmm7

				//B = Y + 454 * (U - 128) / 256;
				movdqu		xmm0, [const454]
				movdqa		xmm7, xmm2
				pmulld		xmm7, xmm0
				paddd		xmm6, xmm7

				pxor		xmm0, xmm0
				pmaxsd		xmm4, xmm0
				pmaxsd		xmm5, xmm0
				pmaxsd		xmm6, xmm0

				psrad		xmm4, 8
				psrad		xmm5, 8
				psrad		xmm6, 8

				pcmpeqd		xmm0, xmm0 //-1
				psrld		xmm0, 24
				pminsd		xmm4, xmm0
				pminsd		xmm5, xmm0
				pminsd		xmm6, xmm0

				//pxor		xmm0, xmm0
				//pxor		xmm1, xmm1
				//pxor		xmm2, xmm2
				//pxor		xmm3, xmm3

				//punpcklwd	xmm0,xmm4
				//punpckhwd	xmm1,xmm4

				//punpcklwd	xmm2,xmm5
				//punpckhwd	xmm3,xmm5

				//pxor		xmm4, xmm4
				//pxor		xmm5, xmm5

				//punpcklwd	xmm4,xmm6
				//punpckhwd	xmm5,xmm6

				pslld		xmm4, 16
				pslld		xmm5, 8
				por			xmm6, xmm4
				por			xmm6, xmm5
				movdqu[edx], xmm6
				add			edx, 16

				//psrad		xmm3, 8
				//psrad		xmm5, 16
				//por			xmm1, xmm3
				//por			xmm1, xmm5
				//movdqu[edx], xmm1
				//add			edx, 16

				add			eax, 4
				add			ebx, 2
				add			ecx, 2

				dec			edi
				jnz			yuv2rgb8_sse2_loop

				emms
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				pop eax
		}
#else
TCHAR sz[MAX_PATH];
//wsprintf(sz, TEXT("!!!!!!!!!!!!11this = %lx\n"), this);
//OutputDebugString(sz);

	yuv2rgb8_sse2_asm( (__int64)pv, (__int64)_prgb, (__int64)py, (__int64)pu,(__int64)loopcnt);
#endif

		loopcnt = width >> 2;
		py += width;
		_prgb += width*4;
		if ((i & 1) == 1)
		{
			pu += width / 2;
			pv += width / 2;
		}
	}

	//int tick2 = GetTickCount();
	//char temp[256];
	//sprintf_s(temp, "10bit tick = %d\n", tick2 - tick1);
	//OutputDebugString(temp);
}



#ifndef _WIN64
inline void yuv2rgb8_sse2_2(unsigned char* py, unsigned char* pu, unsigned char* pv, unsigned char* prgb, int width, int height)
{
	unsigned char* _prgb = prgb;
	int i;
	int loopcnt = width >> 3;
	short const135[8] = { 135,135,135,135,135,135,135,135 };
	short const179[8] = { 179,179,179,179,179,179,179,179 };
	short const11[8] = { 11,11,11,11,11,11,11,11 };
	short const183[8] = { 183,183,183,183,183,183,183,183};
	short const227[8] = { 227,227,227,227,227,227,227,227 };

	for (i = 0; i < height; i++)
	{

		__asm {
			push eax
			push ebx
			push ecx
			push edx
			push esi
			push edi

			mov		eax, py
			mov		ebx, pu
			mov		ecx, pv
			mov		edx, _prgb
			mov		edi, loopcnt

			yuv2rgb8_sse2_loop2:
			movdqu		xmm1, [eax]
				movdqu		xmm2, [ebx]
				movdqu		xmm3, [ecx]

				pxor		xmm0, xmm0

				punpcklbw	xmm1, xmm0
				punpcklbw	xmm2, xmm0
				punpcklbw	xmm3, xmm0

				punpckldq	xmm2, xmm2
				punpckldq	xmm3, xmm3

				movdqa		xmm4, xmm1
				movdqa		xmm5, xmm1
				movdqa		xmm6, xmm1

				//R = Y + 179 * V / 128 - 179;
				movdqu		xmm0, [const179]
				movdqa		xmm7, xmm3
				pmullw		xmm7, xmm0
				psrlw		xmm7, 7
				paddw		xmm4, xmm7
				psubw		xmm4, xmm0

				//G = Y + 135 - 11 * U / 32 - 183 * V / 256;
				movdqu		xmm0, [const135]
				paddw		xmm5, xmm0

				movdqu		xmm0, [const11]
				movdqa		xmm7, xmm2
				pmullw		xmm7, xmm0
				psrlw		xmm7, 5
				psubw		xmm5, xmm7

				movdqu		xmm0, [const183]
				movdqa		xmm7, xmm3
				pmullw		xmm7, xmm0
				psrlw		xmm7, 8
				psubw		xmm5, xmm7

				//B = Y + 227 * U / 128 - 227;
				movdqu		xmm0, [const227]
				movdqa		xmm7, xmm2
				pmullw		xmm7, xmm0
				psrlw		xmm7, 7
				paddw		xmm6, xmm7
				psubw		xmm6, xmm0

				pxor		xmm0, xmm0
				pmaxsw		xmm4, xmm0
				pmaxsw		xmm5, xmm0
				pmaxsw		xmm6, xmm0

				//psraw		xmm4, 8
				//psraw		xmm5, 8
				//psraw		xmm6, 8

				pcmpeqw		xmm0, xmm0 //-1
				psrlw		xmm0, 8
				pminsw		xmm4, xmm0
				pminsw		xmm5, xmm0
				pminsw		xmm6, xmm0

				pxor		xmm1, xmm1
				pxor		xmm2, xmm2
				punpcklbw	xmm1, xmm4
				punpckhbw	xmm2, xmm4

				pxor		xmm3, xmm3
				pxor		xmm4, xmm4
				punpcklbw	xmm3, xmm5
				punpckhbw	xmm4, xmm5

				pxor		xmm5, xmm5
				pxor		xmm7, xmm7
				punpcklbw	xmm5, xmm6
				punpckhbw	xmm7, xmm6

				pslld		xmm1, 8
				pslld		xmm2, 8
				psrld		xmm5, 8
				psrld		xmm7, 8
				por			xmm1, xmm3
				por			xmm1, xmm5
				por			xmm2, xmm4
				por			xmm2, xmm7
				movdqu[edx], xmm1
				add			edx, 16
				movdqu[edx], xmm2
				add			edx, 16

				add			eax, 8
				add			ebx, 4
				add			ecx, 4

				dec			edi
				jnz			yuv2rgb8_sse2_loop2

				emms
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				pop eax
		}

		loopcnt = width >> 3;
		py += width;
		_prgb += width * 4;
		if ((i & 1) == 1)
		{
			pu += width / 2;
			pv += width / 2;
		}
	}
}
#else
inline void yuv2rgb8_sse2_2(unsigned char* py, unsigned char* pu, unsigned char* pv, unsigned char* prgb, int width, int height)
{
	unsigned char* _prgb = prgb;
	int i;
	int loopcnt = width >> 2;

	for (i = 0; i < height; i++)
	{
		yuv2rgb8_sse2_asm((__int64)pv, (__int64)_prgb, (__int64)py, (__int64)pu, (__int64)loopcnt);

		loopcnt = width >> 2;
		py += width;
		_prgb += width * 4;
		if ((i & 1) == 1)
		{
			pu += width / 2;
			pv += width / 2;
		}
	}
}
#endif


//inline void yuv2rgb8_sse2_3(unsigned char* py, unsigned char* pu, unsigned char* pv, unsigned char* prgb, int width, int height)
//{
//	unsigned char* _prgb = prgb;
//	int i;
//	int loopcnt = width >> 3;
//	short const135[16] = { 135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135 };
//	short const179[16] = { 179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179 };
//	short const11[16] = { 11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11 };
//	short const183[16] = { 183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183 };
//	short const227[16] = { 227,227,227,227,227,227,227,227,227,227,227,227,227,227,227,227 };
//
//	for (i = 0; i < height; i++)
//	{
//#ifndef _WIN64
//		__asm {
//			push eax
//			push ebx
//			push ecx
//			push edx
//			push esi
//			push edi
//
//			mov		eax, py
//			mov		ebx, pu
//			mov		ecx, pv
//			mov		edx, _prgb
//			mov		edi, loopcnt
//
//			yuv2rgb8_sse2_loop3 :
//			vmovdqu		ymm1, [eax]
//				vmovdqu		ymm2, [ebx]
//				vmovdqu		ymm3, [ecx]
//
//				vpxor		ymm0, ymm0, ymm0
//
//				vpunpcklbw	ymm1,ymm1, ymm0
//				vpunpcklbw	ymm2,ymm2, ymm0
//				vpunpcklbw	ymm3,ymm3, ymm0
//
//				vpunpckldq	ymm2,ymm2, ymm2
//				vpunpckldq	ymm3,ymm3, ymm3
//
//				vmovdqa		ymm4, ymm1
//				vmovdqa		ymm5, ymm1
//				vmovdqa		ymm6, ymm1
//
//				//R = Y + 179 * V / 128 - 179;
//				vmovdqu		ymm0, [const179]
//				vmovdqa		ymm7, ymm3
//				vpmullw		ymm7,ymm7, ymm0
//				vpsrlw		ymm7,ymm7, 7
//				vpaddw		ymm4,ymm4, ymm7
//				vpsubw		ymm4,ymm4, ymm0
//
//				//G = Y + 135 - 11 * U / 32 - 183 * V / 256;
//				vmovdqu		ymm0, [const135]
//				vpaddw		ymm5,ymm5, ymm0
//
//				movdqu		xmm0, [const11]
//				movdqa		xmm7, xmm2
//				pmullw		xmm7, xmm0
//				psrlw		xmm7, 5
//				psubw		xmm5, xmm7
//
//				movdqu		xmm0, [const183]
//				movdqa		xmm7, xmm3
//				pmullw		xmm7, xmm0
//				psrlw		xmm7, 8
//				psubw		xmm5, xmm7
//
//				//B = Y + 227 * U / 128 - 227;
//				movdqu		xmm0, [const227]
//				movdqa		xmm7, xmm2
//				pmullw		xmm7, xmm0
//				psrlw		xmm7, 7
//				paddw		xmm6, xmm7
//				psubw		xmm6, xmm0
//
//				pxor		xmm0, xmm0
//				pmaxsw		xmm4, xmm0
//				pmaxsw		xmm5, xmm0
//				pmaxsw		xmm6, xmm0
//
//				//psraw		xmm4, 8
//				//psraw		xmm5, 8
//				//psraw		xmm6, 8
//
//				pcmpeqw		xmm0, xmm0 //-1
//				psrlw		xmm0, 8
//				pminsw		xmm4, xmm0
//				pminsw		xmm5, xmm0
//				pminsw		xmm6, xmm0
//
//				pxor		xmm1, xmm1
//				pxor		xmm2, xmm2
//				punpcklbw	xmm1, xmm4
//				punpckhbw	xmm2, xmm4
//
//				pxor		xmm3, xmm3
//				pxor		xmm4, xmm4
//				punpcklbw	xmm3, xmm5
//				punpckhbw	xmm4, xmm5
//
//				pxor		xmm5, xmm5
//				pxor		xmm7, xmm7
//				punpcklbw	xmm5, xmm6
//				punpckhbw	xmm7, xmm6
//
//				pslld		xmm1, 8
//				pslld		xmm2, 8
//				psrld		xmm5, 8
//				psrld		xmm7, 8
//				por			xmm1, xmm3
//				por			xmm1, xmm5
//				por			xmm2, xmm4
//				por			xmm2, xmm7
//				movdqu[edx], xmm1
//				add			edx, 16
//				movdqu[edx], xmm2
//				add			edx, 16
//
//				add			eax, 8
//				add			ebx, 4
//				add			ecx, 4
//
//				dec			edi
//				jnz			yuv2rgb8_sse2_loop3
//
//				emms
//				pop edi
//				pop esi
//				pop edx
//				pop ecx
//				pop ebx
//				pop eax
//		}
//#else
//		TCHAR sz[MAX_PATH];
//		//wsprintf(sz, TEXT("!!!!!!!!!!!!11this = %lx\n"), this);
//		//OutputDebugString(sz);
//
//		yuv2rgb8_sse2_asm((__int64)pv, (__int64)_prgb, (__int64)py, (__int64)pu, (__int64)loopcnt);
//#endif
//
//		loopcnt = width >> 3;
//		py += width;
//		_prgb += width * 4;
//		if ((i & 1) == 1)
//		{
//			pu += width / 2;
//			pv += width / 2;
//		}
//	}
//}


extern "C" __int64 yuv2rgb10_sse2_asm(__int64, __int64, __int64, __int64, __int64);
inline void yuv2rgb10_sse2(short *py, short *pu, short *pv, int *prgb, int width, int height)
{
	int i;
	int loopcnt = width >> 2;
	int const1436[4] = {1436,1436,1436,1436};
	int const352[4] = {352,352,352,352};
	int const731[4] = {731,731,731,731};
	int const1815[4] = {1815,1815,1815,1815};

	for(i=0; i<height; i++)
	{
#ifndef _WIN64
		__asm{
			push eax
				push ebx
				push ecx
				push edx
				push esi
				push edi

				mov		eax, py
				mov		ebx, pu
				mov		ecx, pv
				mov		edx, prgb
				mov		edi, loopcnt

				pxor		xmm0, xmm0
				pcmpeqd		xmm7, xmm7 //-1
				psrld		xmm7, 31
				pslld		xmm7, 9   //512

yuv2rgb10_loop:
			movdqu		xmm1, [eax]
			movdqu		xmm2, [ebx]
			movdqu		xmm3, [ecx]

			punpcklwd	xmm1, xmm0
				punpcklwd	xmm2, xmm0
				punpcklwd	xmm3, xmm0
				punpcklqdq	xmm2, xmm2
				punpcklqdq	xmm3, xmm3

				pslld		xmm1, 10
				psubd		xmm2, xmm7
				psubd		xmm3, xmm7

				movdqa		xmm4, xmm1
				movdqa		xmm5, xmm2
				movdqa		xmm6, xmm3

				movdqu		xmm0, [const1436]
			pmulld		xmm3, xmm0
				paddd		xmm1, xmm3
				paddd		xmm1, xmm7
				psrad		xmm1, 10		//R

				movdqa		xmm3, xmm6
				movdqu		xmm0, [const352]
			pmulld		xmm2, xmm0
				movdqu		xmm0, [const731]
			pmulld		xmm3, xmm0

				paddd		xmm3, xmm2
				movdqa		xmm2, xmm4
				paddd		xmm2, xmm7
				psubd		xmm2, xmm3
				psrad		xmm2, 10		//G

				movdqu		xmm0, [const1815]
			pmulld		xmm5, xmm0
				paddd		xmm4, xmm7
				paddd		xmm4, xmm5
				psrad		xmm4, 10		//B

				pcmpeqd		xmm0, xmm0 //-1
				psrld		xmm0, 22
				pminsd		xmm1, xmm0
				pminsd		xmm2, xmm0
				pminsd		xmm4, xmm0
				pxor		xmm0, xmm0
				pmaxsd		xmm1, xmm0
				pmaxsd		xmm2, xmm0
				pmaxsd		xmm4, xmm0

				pslld		xmm1, 20
				pslld		xmm2, 10
				por			xmm1, xmm2
				por			xmm1, xmm4
				movdqu		[edx], xmm1

				add			eax, 8
				add			ebx, 4
				add			ecx, 4
				add			edx, 16

				dec			edi
				jnz			yuv2rgb10_loop

				emms
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				pop eax
		}
#else
		yuv2rgb10_sse2_asm((__int64)py, (__int64)pu, (__int64)pv, (__int64)prgb, loopcnt);
#endif

		loopcnt = width >> 2;
		py += width;
		prgb += width;
		if((i&1)==1)
		{
			pu += width/2;
			pv += width/2;
		}
	}

	//int tick2 = GetTickCount();
	//char temp[256];
	//sprintf_s(temp, "10bit tick = %d\n", tick2 - tick1);
	//OutputDebugString(temp);
}



//R = 1192 * Y / 1024 + 1634 * (V - 512) / 1024;
//G = 1192 * Y / 1024 - 401 * (U - 512) / 1024 - 832 * (V - 512) / 1024;
//B = 1192 * Y / 1024 + 2066 * (U - 512) / 1024;

//R = (1192 * Y  + 1634 * V - 836608) / 1024;
//G = (1192 * Y  + 631296 - 401 * U - 832 * V )/ 1024;
//B = (1192 * Y  + 2066 * U - 1057792) / 1024;
extern "C" __int64 yuv2rgb10_sse2_stretch_asm(__int64, __int64, __int64, __int64, __int64);
inline void yuv2rgb10_sse2_stretch(short *py, short *pu, short *pv, int *prgb, int width, int height)
{
	//int tick1 = GetTickCount();
	int i;
	int loopcnt = width >> 2;
	int const64[4] = { 64,64,64,64 };
	int const1192[4] = { 1192,1192,1192,1192 };
	int const1634[4] = { 1634,1634,1634,1634 };
	int const401[4] = { 401,401,401,401 };
	int const832[4] = { 832,832,832,832 };
	int const2066[4] = { 2066,2066,2066,2066 };
	int const836608[4] = { 836608,836608,836608,836608 };
	int const631296[4] = { 631296,631296,631296,631296 };
	int const1057792[4] = { 1057792,1057792,1057792,1057792 };

	for (i = 0; i < height; i++)
	{
#ifndef _WIN64
		__asm {
			push eax
			push ebx
			push ecx
			push edx
			push esi
			push edi

			mov		eax, py
			mov		ebx, pu
			mov		ecx, pv
			mov		edx, prgb
			mov		edi, loopcnt

			pxor		xmm0, xmm0
			pcmpeqd		xmm7, xmm7 //-1
			psrld		xmm7, 31
			pslld		xmm7, 9   //512

			yuv2rgb10_loop:
			
			movdqu		xmm1, [eax]  //Y
			movdqu		xmm2, [ebx]  //U
			movdqu		xmm3, [ecx]  //V

				pxor		xmm0, xmm0

			punpcklwd	xmm1, xmm0
			punpcklwd	xmm2, xmm0
			punpcklwd	xmm3, xmm0
			punpcklqdq	xmm2, xmm2
			punpcklqdq	xmm3, xmm3

				movdqu		xmm0, [const64]
				psubd		xmm1, xmm0		// (Y - 64)

			movdqu		xmm0, [const1192]
			pmulld		xmm1, xmm0		//1192 * (Y - 64)

				movdqa		xmm4, xmm1
				movdqa		xmm5, xmm1
				movdqa		xmm6, xmm1
				
				//R = (1192 * (Y - 64)  + 1634 * V - 836608) / 1024;
				movdqa		xmm7, xmm3
				movdqu		xmm0, [const1634]
				pmulld		xmm7, xmm0		//1634 * V
				paddd		xmm4, xmm7		//1192 * (Y - 64)  + 1634 * V

				movdqu		xmm0, [const836608]
				psubd		xmm4, xmm0		//1192 * (Y - 64)  + 1634 * V - 836608

				//G = (1192 * (Y - 64)  + 631296 - 401 * U - 832 * V )/ 1024;
				movdqu		xmm0, [const631296]
				paddd		xmm5, xmm0		//1192 * (Y - 64)  + 631296

				movdqa		xmm7, xmm2
				movdqu		xmm0, [const401]
				pmulld		xmm7, xmm0
				psubd		xmm5, xmm7		//1192 * (Y - 64)  + 631296 - 401 * U 

				movdqa		xmm7, xmm3
				movdqu		xmm0, [const832]
				pmulld		xmm7, xmm0
				psubd		xmm5, xmm7		//1192 * (Y - 64)  + 631296 - 401 * U - 832 * V 

				//B = (1192 * (Y - 64)  + 2066 * U - 1057792) / 1024;
				movdqa		xmm7, xmm2
				movdqu		xmm0, [const2066]
				pmulld		xmm7, xmm0
				paddd		xmm6, xmm7		//1192 * (Y - 64)  + 2066 * U

				movdqu		xmm0, [const1057792]
				psubd		xmm6, xmm0		//1192 * (Y - 64)  + 2066 * U - 1057792

				pxor		xmm0, xmm0
				pmaxsd		xmm4, xmm0		//if R<0  R = 0
				pmaxsd		xmm5, xmm0		//if G<0  G = 0
				pmaxsd		xmm6, xmm0		//if B<0  B = 0
				psrld		xmm4, 10		//R= R/1024
				psrld		xmm5, 10		//G= G/1024
				psrld		xmm6, 10		//B= B/1024

				pcmpeqd		xmm0, xmm0 //-1
				psrld		xmm0, 22
				pminsd		xmm4, xmm0
				pminsd		xmm5, xmm0
				pminsd		xmm6, xmm0

				pslld		xmm4, 20
				pslld		xmm5, 10
				por			xmm4, xmm5
				por			xmm4, xmm6
				movdqu[edx], xmm4
				add			eax, 8
				add			ebx, 4
				add			ecx, 4
				add			edx, 16

				dec			edi
				jnz			yuv2rgb10_loop

				emms
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				pop eax
		}
#else
yuv2rgb10_sse2_stretch_asm((__int64)pv, (__int64)prgb, (__int64)py, (__int64)pu, loopcnt);
#endif
		loopcnt = width >> 2;
		py += width;
		prgb += width;
		if ((i & 1) == 1)
		{
			pu += width / 2;
			pv += width / 2;
		}
	}

			//pslld		xmm1, 10
			//psubd		xmm2, xmm7
			//psubd		xmm3, xmm7

			//movdqa		xmm4, xmm1
			//movdqa		xmm5, xmm2
			//movdqa		xmm6, xmm3

			//movdqu		xmm0, [const1634]
			//pmulld		xmm3, xmm0
			//paddd		xmm1, xmm3
			//paddd		xmm1, xmm7
			//psrad		xmm1, 10		//R

			//movdqa		xmm3, xmm6
			//movdqu		xmm0, [const401]
			//pmulld		xmm2, xmm0
			//movdqu		xmm0, [const832]
			//pmulld		xmm3, xmm0

			//paddd		xmm3, xmm2
			//movdqa		xmm2, xmm4
			//paddd		xmm2, xmm7
			//psubd		xmm2, xmm3
			//psrad		xmm2, 10		//G

			//movdqu		xmm0, [const2066]
			//pmulld		xmm5, xmm0
			//paddd		xmm4, xmm7
			//paddd		xmm4, xmm5
			//psrad		xmm4, 10		//B

			//pcmpeqd		xmm0, xmm0 //-1
			//psrld		xmm0, 22
			//pminsd		xmm1, xmm0
			//pminsd		xmm2, xmm0
			//pminsd		xmm4, xmm0
			//pxor		xmm0, xmm0
			//pmaxsd		xmm1, xmm0
			//pmaxsd		xmm2, xmm0
			//pmaxsd		xmm4, xmm0

			//pslld		xmm1, 20
			//pslld		xmm2, 10
			//por			xmm1, xmm2
			//por			xmm1, xmm4
			//movdqu[edx], xmm1


}

#endif // COLORSPACE_H





















