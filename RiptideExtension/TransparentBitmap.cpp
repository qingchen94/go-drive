// TransparentBitmap.cpp: implementation of the CTransparentBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TransparentBitmap.h"

/////////////////////////////////////////////////
// CTransparentBitmap::CColorTheme
// 

HLSCOLOR CTransparentBitmap::CColorTheme::RGB2HLS(COLORREF rgb)
{
	unsigned char minval = __min(GetRValue(rgb), __min(GetGValue(rgb), GetBValue(rgb)));
	unsigned char maxval = __max(GetRValue(rgb), __max(GetGValue(rgb), GetBValue(rgb)));
	float mdiff = float(maxval) - float(minval);
	float msum = float(maxval) + float(minval);

	float luminance = msum / 510.0f;
	float saturation = 0.0f;
	float hue = 0.0f;

	if (maxval != minval)
	{
		float rnorm = (maxval - GetRValue(rgb)) / mdiff;
		float gnorm = (maxval - GetGValue(rgb)) / mdiff;
		float bnorm = (maxval - GetBValue(rgb)) / mdiff;

		saturation = (luminance <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

		if (GetRValue(rgb) == maxval) hue = 60.0f * (6.0f + bnorm - gnorm);
		if (GetGValue(rgb) == maxval) hue = 60.0f * (2.0f + rnorm - bnorm);
		if (GetBValue(rgb) == maxval) hue = 60.0f * (4.0f + gnorm - rnorm);
		if (hue > 360.0f) hue = hue - 360.0f;
	}
	return HLS((hue * 255) / 360, luminance * 255, saturation * 255);
}

BYTE CTransparentBitmap::CColorTheme::_ToRGB(float rm1, float rm2, float rh)
{
	if (rh > 360.0f) rh -= 360.0f;
	else if (rh <   0.0f) rh += 360.0f;

	if (rh <  60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
	else if (rh < 180.0f) rm1 = rm2;
	else if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

	return (BYTE)(rm1 * 255);
}

COLORREF CTransparentBitmap::CColorTheme::HLS2RGB(HLSCOLOR hls)
{
	float hue = ((int)HLS_H(hls) * 360) / 255.0f;
	float luminance = HLS_L(hls) / 255.0f;
	float saturation = HLS_S(hls) / 255.0f;

	if (saturation == 0.0f)
	{
		return RGB(HLS_L(hls), HLS_L(hls), HLS_L(hls));
	}
	float rm1, rm2;

	if (luminance <= 0.5f) rm2 = luminance + luminance * saturation;
	else                     rm2 = luminance + saturation - luminance * saturation;
	rm1 = 2.0f * luminance - rm2;
	BYTE red = _ToRGB(rm1, rm2, hue + 120.0f);
	BYTE green = _ToRGB(rm1, rm2, hue);
	BYTE blue = _ToRGB(rm1, rm2, hue - 120.0f);

	return RGB(red, green, blue);
}

COLORREF CTransparentBitmap::CColorTheme::HLS_TRANSFORM(COLORREF rgb, int percent_L, int percent_S)
{
	HLSCOLOR hls = RGB2HLS(rgb);
	BYTE h = HLS_H(hls);
	BYTE l = HLS_L(hls);
	BYTE s = HLS_S(hls);

	if (percent_L > 0)
	{
		l = BYTE(l + ((255 - l) * percent_L) / 100);
	}
	else if (percent_L < 0)
	{
		l = BYTE((l * (100 + percent_L)) / 100);
	}
	if (percent_S > 0)
	{
		s = BYTE(s + ((255 - s) * percent_S) / 100);
	}
	else if (percent_S < 0)
	{
		s = BYTE((s * (100 + percent_S)) / 100);
	}
	return HLS2RGB(HLS(h, l, s));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransparentBitmap::CTransparentBitmap(EImageAlignment nAlignment)
{
	m_bDeleteObj = false;
	m_hbmpImage = NULL;
	m_hbmpMask = NULL;
	m_szImage.cx = m_szImage.cy = 0;
	m_bClipImage = false;
	m_nAlignment = nAlignment;
	m_bIsTrue32Bit = false;
}

CTransparentBitmap::~CTransparentBitmap()
{
	if ( m_bDeleteObj && m_hbmpImage != NULL )
		DeleteObject(m_hbmpImage);

	if ( m_hbmpMask != NULL )
		DeleteObject(m_hbmpMask);
}

CTransparentBitmap::CTransparentBitmap(const CTransparentBitmap& other)
{
	*this = other;
}

CTransparentBitmap& CTransparentBitmap::operator = (const CTransparentBitmap& other)
{
	m_bDeleteObj = false;
	m_hbmpImage = NULL;
	m_hbmpMask = NULL;
	SetImage(other.m_hbmpImage, other.m_bDeleteObj ? false : true);
	m_bClipImage = other.m_bClipImage;
	m_nAlignment = other.m_nAlignment;

	return *this;
}

void CTransparentBitmap::SetImage(HBITMAP hbmpImage, bool bShared)
{
	if ((m_bIsTrue32Bit = IsImage32Bit(hbmpImage)) == true)
	{
		SetImage32(hbmpImage, bShared);
		return;
	}

	if ( m_hbmpImage != NULL )
	{
		if ( m_bDeleteObj )
			DeleteObject(m_hbmpImage);
		DeleteObject(m_hbmpMask);

		m_hbmpImage = NULL;
		m_hbmpMask = NULL;
	}

	if ( (m_hbmpMask = BuildImageMask(hbmpImage, &m_szImage)) != NULL )
	{
		if ( bShared )
		{
			m_hbmpImage = hbmpImage;
			m_bDeleteObj = false;
		}
		else // copy image
		{
			BITMAP bmImage;
			GetBitmap(hbmpImage, bmImage);
			m_hbmpImage = CreateBitmapIndirect(&bmImage);
			m_bDeleteObj = true;
			if ( bmImage.bmBits != NULL )
				delete[] bmImage.bmBits;
		}
	}
}

HBITMAP CTransparentBitmap::BuildImageMask(HBITMAP hbmpImage, SIZE* pszImage)
{
	BITMAP bmImage;
	if ( !GetBitmap(hbmpImage, bmImage) ) // bad image, do nothing
		return NULL;

	POINT ptTransSpot;
	ptTransSpot.x = ptTransSpot.y = 0;

	DWORD dwTransPixel = GetPixelAt(bmImage, ptTransSpot);

	// build monochrome mask
	BITMAP bmMask;
	bmMask.bmType = 0;
	bmMask.bmPlanes = 1;
	bmMask.bmBitsPixel = 1;
	bmMask.bmHeight = bmImage.bmHeight;
	bmMask.bmWidth = bmImage.bmWidth;

	bmMask.bmWidthBytes = (bmImage.bmWidth + 7) / 8;
	if ( bmMask.bmWidthBytes % 2 )
		++(bmMask.bmWidthBytes);

	// alloc bits buffer
	int nMaskBufSize = bmMask.bmHeight * bmMask.bmWidthBytes;
	bmMask.bmBits = (LPVOID)new BYTE[nMaskBufSize];
	memset(bmMask.bmBits, 0, nMaskBufSize);

	// set each mask bit based on transparent pixel
	LPBYTE pbImageLine = (LPBYTE)bmImage.bmBits;
	LPBYTE pbMaskLine = (LPBYTE)bmMask.bmBits;

	int nImageLineOffset;
	int nMaskLineOffset;

	for (int i = 0; i < bmImage.bmHeight; i++ ) // for each scan line
	{
		nImageLineOffset = 0;
		nMaskLineOffset = 0;

		for ( int j = 0; j < bmImage.bmWidth; j++ ) // for each pixel in the line
		{
			DWORD dwPixel = ReadPixelInLine(pbImageLine, nImageLineOffset, bmImage.bmBitsPixel);
			WriteBitInLine(pbMaskLine, nMaskLineOffset, dwPixel == dwTransPixel ? true : false);
		}

		// move to next line
		pbImageLine += bmImage.bmWidthBytes;
		pbMaskLine += bmMask.bmWidthBytes;
	}

	// create mask bitmap
	HBITMAP hbmpMask = CreateBitmapIndirect(&bmMask);

	if ( pszImage != NULL )
	{
		pszImage->cx = bmImage.bmWidth;
		pszImage->cy = bmImage.bmHeight;
	}

	// release bits buffer
	if ( bmImage.bmBits != NULL )
		delete[] bmImage.bmBits;

	if ( bmMask.bmBits != NULL )
		delete[] bmMask.bmBits;

	return hbmpMask;
}

bool CTransparentBitmap::GetBitmap(HBITMAP hbmp, BITMAP &bm)
{
	if ( GetObject(hbmp, sizeof(BITMAP), (LPVOID)&bm) == 0 )
		return false;

	int nBufSize = bm.bmHeight * bm.bmWidthBytes;
	bm.bmBits = (LPVOID)new BYTE[nBufSize];

	if ( GetBitmapBits(hbmp, nBufSize, bm.bmBits) != nBufSize )
	{
		delete[] bm.bmBits;
		return false;
	}

	return true;
}

DWORD CTransparentBitmap::GetPixelAt(BITMAP &bm, POINT &ptPixelPos)
{
	ATLASSERT(ptPixelPos.x >= 0 && ptPixelPos.x < bm.bmWidth &&
		ptPixelPos.y >= 0 && ptPixelPos.y < bm.bmHeight);

	// move to line
	LPBYTE pbLine = (LPBYTE)bm.bmBits + bm.bmWidthBytes * ptPixelPos.y;

	// move to start bit
	int nBitOffset = ptPixelPos.x * bm.bmBitsPixel;

	// read
	return ReadPixelInLine(pbLine, nBitOffset, bm.bmBitsPixel);
}

DWORD CTransparentBitmap::ReadPixelInLine(LPBYTE pbBits, int &nStartBitOffset, int nBitsPixel)
{
	ATLASSERT(nBitsPixel <= 32);

	DWORD dwRet = 0;

	// move to start byte
	pbBits += nStartBitOffset / 8;
	int nBitOffsetInByte = nStartBitOffset % 8;

	// first byte
	BYTE b = *pbBits++;

	int nBitsShift = 24 + nBitOffsetInByte;
	dwRet = ((DWORD)b) << nBitsShift;

	nBitsShift -= 8;

	// read the rest bytes (optional)
	int nBitsLeft = nBitsPixel - 8 + nBitOffsetInByte;
	for ( int i = 0; nBitsLeft > 0 && i < 4; i++, nBitsLeft -= 8 ) // 2nd to 5th byte
	{
		b = *pbBits++;

		if ( nBitsLeft >= 8 ) // read the whole byte
		{
			dwRet |= ((DWORD)b) << nBitsShift;
			nBitsShift -= 8;
		}
		else // only high-order bits
		{
			dwRet |= ((DWORD)b) >> (8 - nBitsLeft);
		}
	}

	// move to next pixel
	nStartBitOffset += nBitsPixel;

	return dwRet;
}

void CTransparentBitmap::WriteBitInLine(LPBYTE pbBits, int &nStartBitOffset, bool bIsZero)
{
	// move to start byte
	pbBits += nStartBitOffset / 8;
	int nBitOffsetInByte = nStartBitOffset % 8;

	// set the bit
	BYTE b = bIsZero ? 0 : 0x80;
	*pbBits |= (b >> nBitOffsetInByte);

	// move to next pixel
	nStartBitOffset++;
}

void CTransparentBitmap::WritePixelInLine(LPBYTE pbBits, int &nStartBitOffset, int nBitsPixel, DWORD dwPixel)
{
	ATLASSERT(nBitsPixel <= 32);

	// move to start byte
	pbBits += nStartBitOffset / 8;
	int nBitOffsetInByte = nStartBitOffset % 8;

	DWORD dwByteMask = 0x000000ff;

	// first byte
	int nBitsShift = 24 + nBitOffsetInByte;
	BYTE b = (BYTE)((dwPixel >> nBitsShift) & dwByteMask);
	*pbBits++ = b;

	nBitsShift -= 8;

	// read the rest bytes (optional)
	int nBitsLeft = nBitsPixel - 8 + nBitOffsetInByte;
	for (int i = 0; nBitsLeft > 0 && i < 4; i++, nBitsLeft -= 8) // 2nd to 5th byte
	{
		if (nBitsLeft >= 8) // read the whole byte
		{
			b = (BYTE)((dwPixel >> nBitsShift) & dwByteMask);
			nBitsShift -= 8;
		}
		else // only high-order bits
		{
			b = (BYTE)((dwPixel << (8 - nBitsLeft)) & dwByteMask);
		}

		*pbBits++ = b;
	}

	// move to next pixel
	nStartBitOffset += nBitsPixel;
}

void CTransparentBitmap::OnDraw(HDC hdcTarget, RECT &rcTarget)
{
	if (m_bIsTrue32Bit)
	{
		OnDraw32(hdcTarget, rcTarget);
		return;
	}

	if ( m_hbmpImage == NULL ) // not ready, do nothing
		return;

	int wDraw = rcTarget.right - rcTarget.left;
	int hDraw = rcTarget.bottom - rcTarget.top;
	if ( wDraw <= 0 || hDraw <= 0 ) // no room to draw, do nothing
		return;

	if ( m_nAlignment == ALIGN_STRETCHTOFIT )
	{
		OnDrawStretchedImage(hdcTarget, rcTarget);
		return;
	}

	HRGN hrgnClip = NULL;
	HRGN hrgnClipPrev = NULL;
	if ( m_bClipImage )
	{
		GetClipRgn(hdcTarget, hrgnClipPrev);

		hrgnClip = CreateRectRgn(rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
		SelectClipRgn(hdcTarget, hrgnClip);
	}

	// prepare to draw image
	HDC hdcImage = CreateCompatibleDC(hdcTarget);
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, m_hbmpImage);

	// paint image
	// Note: TransparentBlt() no longer available in ver 5.1 or later

	// calc. coord. of top-left corner for painting
	int x, y;
	switch ( m_nAlignment )
	{
	case ALIGN_TOPLEFT:
		x = rcTarget.left;
		y = rcTarget.top;
		break;
	case ALIGN_BOTTOMLEFT:
		x = rcTarget.left;
		y = rcTarget.bottom - m_szImage.cy;
		break;
	case ALIGN_TOPRIGHT:
		x = rcTarget.right - m_szImage.cx;
		y = rcTarget.top;
		break;
	case ALIGN_BOTTOMRIGHT:
		x = rcTarget.right - m_szImage.cx;
		y = rcTarget.bottom - m_szImage.cy;
		break;
	default: // ALIGN_CENTER
		x = rcTarget.left + (wDraw - m_szImage.cx) / 2;
		y = rcTarget.top + (hDraw - m_szImage.cy) / 2;
	}

	RECT rc;
	rc.left = x;
	rc.top = y;
	rc.right = x + m_szImage.cx;
	rc.bottom = y + m_szImage.cy;
	InvertRect(hdcTarget, &rc);

	MaskBlt(hdcTarget, x, y, m_szImage.cx, m_szImage.cy, 
		hdcImage, 0, 0, m_hbmpMask, 0, 0, MAKEROP4(SRCCOPY, DSTINVERT));

	// release resource
	if ( m_bClipImage )
	{
		SelectClipRgn(hdcTarget, hrgnClipPrev);
		DeleteObject(hrgnClip);
	}
	SelectObject(hdcImage, hbmpPrev);
	DeleteDC(hdcImage);
}

void CTransparentBitmap::OnDrawStretchedImage(HDC hdcTarget, RECT& rcTarget)
{
	// prepare to draw image
	int wDraw = rcTarget.right - rcTarget.left;
	int hDraw = rcTarget.bottom - rcTarget.top;

	HDC hdcImage = CreateCompatibleDC(hdcTarget);
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, m_hbmpImage);

	// get stretched image first
	HDC hdcStretched = CreateCompatibleDC(hdcTarget);
	HBITMAP hbmpStretched = CreateCompatibleBitmap(hdcTarget, wDraw, hDraw);
	HGDIOBJ hbmpStretchedPrev = SelectObject(hdcStretched, hbmpStretched);

	StretchBlt(hdcStretched, 0, 0, wDraw, hDraw,
			hdcImage, 0, 0, m_szImage.cx, m_szImage.cy, SRCCOPY);

	// build stretched image mask
	HBITMAP hbmpMask = BuildImageMask(hbmpStretched);

	// now paint strecthed bitmap transparently
	InvertRect(hdcTarget, &rcTarget);

	MaskBlt(hdcTarget, rcTarget.left, rcTarget.top, wDraw, hDraw, 
		hdcStretched, 0, 0, hbmpMask, 0, 0, MAKEROP4(SRCCOPY, DSTINVERT));

	// release resource
	SelectObject(hdcImage, hbmpPrev);
	DeleteDC(hdcImage);

	SelectObject(hdcStretched, hbmpStretchedPrev);
	DeleteObject(hbmpStretched);
	DeleteDC(hdcStretched);

	DeleteObject(hbmpMask);
}

void CTransparentBitmap::SetAlignment(EImageAlignment nAlignment)
{
	m_nAlignment = nAlignment;
}

HBITMAP CTransparentBitmap::CreateBitmap32(const SIZE& size, void** ppBits)
{
	if (ppBits != NULL)
	{
		*ppBits = NULL;
	}

	if (size.cx <= 0 || size.cy == 0)
	{
		return NULL;
	}

	BITMAPINFO bi = { 0 };

	// Fill in the BITMAPINFOHEADER
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = size.cx;
	bi.bmiHeader.biHeight = size.cy;
	bi.bmiHeader.biSizeImage = size.cx * abs(size.cy);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	LPVOID pData = NULL;
	HBITMAP hbmp = ::CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pData, NULL, 0);

	if (pData != NULL && hbmp != NULL && ppBits != NULL)
	{
		*ppBits = pData;
	}

	return hbmp;
}

HBITMAP CTransparentBitmap::CreateBitmap32(HBITMAP hbmpImage)
{
	BITMAP bmImage;
	if (!GetBitmap(hbmpImage, bmImage)) // bad image, do nothing
		return NULL;

	if (bmImage.bmBitsPixel < 24) // support only 24 or 32-bit
	{
		if (bmImage.bmBits != NULL)
			delete[] bmImage.bmBits;

		return NULL;
	}

	SIZE szBmp;
	szBmp.cx = bmImage.bmWidth;
	szBmp.cy = bmImage.bmHeight;

	LPBYTE pbBits32 = NULL;
	HBITMAP hbmp32 = CreateBitmap32(szBmp, (void**)&pbBits32);
	if (hbmp32 == NULL || pbBits32 == NULL)
		return NULL;

	POINT ptTransSpot;
	ptTransSpot.x = 0;
	ptTransSpot.y = 0;

	DWORD dwTransPixel = GetPixelAt(bmImage, ptTransSpot);

	// replace "transparent" pixels with transparent color
	DWORD r, g, b;// , a;

	LPBYTE pbImageLine = (LPBYTE)bmImage.bmBits;

	int nImageLineOffset;
	LPBYTE pbWrite;

	for (int i = 0; i < bmImage.bmHeight; i++) // for each scan line
	{
		nImageLineOffset = 0;

		pbWrite = pbBits32 + (bmImage.bmHeight - i - 1) * bmImage.bmWidth * 4;

		for (int j = 0; j < bmImage.bmWidth; j++) // for each pixel in the line
		{
			DWORD dwPixel = ReadPixelInLine(pbImageLine, nImageLineOffset, bmImage.bmBitsPixel);
			if (dwPixel == dwTransPixel)
			{
				*(DWORD*)pbWrite = 0;
			}
			else
			{
				r = (dwPixel >> 8) & 0x000000ff;
				g = (dwPixel >> 16) & 0x000000ff;
				b = (dwPixel >> 24) & 0x000000ff;
				*(DWORD*)pbWrite = (r << 16) | (g << 8) | b | 0xff000000;
			}
			pbWrite += 4;
		}

		// move to next line
		pbImageLine += bmImage.bmWidthBytes;
	}

	// release bits buffer
	if (bmImage.bmBits != NULL)
		delete[] bmImage.bmBits;

	return hbmp32;
}

HBITMAP CTransparentBitmap::CreateBitmap32(HICON hicon)
{
	ICONINFO ii;
	if (!GetIconInfo(hicon, &ii))
		return NULL;

	BITMAP bmImage;
	int n = GetObject(ii.hbmColor, sizeof(BITMAP), (LPVOID)&bmImage);

	DeleteObject(ii.hbmColor);
	DeleteObject(ii.hbmMask);
	if (n == 0)
		return NULL;

	SIZE szImage;
	szImage.cx = bmImage.bmWidth;
	szImage.cy = bmImage.bmHeight;

	HBITMAP hbmp32 = CreateBitmap32(szImage, NULL);
	if (hbmp32 == NULL)
		return NULL;

	HDC hdc = GetDC(NULL);
	HDC hdcImage = CreateCompatibleDC(hdc);
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, hbmp32);

	DrawIconEx(hdcImage, 0, 0, hicon, szImage.cx, szImage.cy, 0, NULL, DI_NORMAL);

	SelectObject(hdcImage, hbmpPrev);
	DeleteDC(hdcImage);
	ReleaseDC(NULL, hdc);

	return hbmp32;
}

HICON CTransparentBitmap::CreateIcon(HBITMAP hbmpImage)
{
	BITMAP bmImage;
	GetObject(hbmpImage, sizeof(BITMAP), (LPVOID)&bmImage);

	HBITMAP hbmpMask = ::CreateCompatibleBitmap(::GetDC(NULL), bmImage.bmWidth, bmImage.bmHeight);

	HBITMAP hbmp32 = CreateBitmap32(hbmpImage);

	ICONINFO ii = { 0 };
	ii.fIcon = TRUE;
	ii.hbmColor = hbmpImage;
	ii.hbmMask = hbmpMask;

	HICON hIcon = ::CreateIconIndirect(&ii);
	::DeleteObject(hbmpMask);
	::DeleteObject(hbmp32);

	return hIcon;
}

HBITMAP CTransparentBitmap::CreateDisabledBitmap32(HICON hIcon)
{
	ICONINFO ii;
	if (!GetIconInfo(hIcon, &ii))
		return NULL;

	BITMAP bmImage;
	int n = GetObject(ii.hbmColor, sizeof(BITMAP), (LPVOID)&bmImage);

	DeleteObject(ii.hbmColor);
	DeleteObject(ii.hbmMask);
	if (n == 0)
		return NULL;

	SIZE szImage;
	szImage.cx = bmImage.bmWidth;
	szImage.cy = bmImage.bmHeight;

	LPBYTE pbBits32 = NULL;
	HBITMAP hbmp32 = CreateBitmap32(szImage, (void**)&pbBits32);
	if (hbmp32 == NULL || pbBits32 == NULL)
		return NULL;

	HDC hdc = GetDC(NULL);
	HDC hdcImage = CreateCompatibleDC(hdc);
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, hbmp32);

	DrawIconEx(hdcImage, 0, 0, hIcon, szImage.cx, szImage.cy, 0, NULL, DI_NORMAL);

	SelectObject(hdcImage, hbmp32);
	DeleteDC(hdcImage);
	ReleaseDC(NULL, hdc);

	LPDWORD lpdwPixels = (LPDWORD)pbBits32;
	int nImageSize = bmImage.bmWidth * bmImage.bmHeight;

	BYTE r, g, b, a;

	for (int i = 0; i < nImageSize; i++) // for each scan line
	{
		DWORD clr = *lpdwPixels;

		a = (clr & 0xff000000) >> 24;
		if (a == 0) // transparent pixel
			*lpdwPixels++ = 0;
		else
		{
			r = GetRValue(clr);
			g = GetGValue(clr);
			b = GetBValue(clr);

			// pre-multiple alpha
			r = r * a / 255 + 255 - a;
			g = g * a / 255 + 255 - a;
			b = b * a / 255 + 255 - a;

			int l = 0.2126 * r + 0.7152 * g + 0.0722 * b;
			l = 255 - (255 - l) * 0.25; // make it lighter

			clr = RGB(l, l, l);

			*lpdwPixels++ = clr | 0xff000000;
		}
	}

	return hbmp32;
}

void CTransparentBitmap::SetImage32(HBITMAP hbmpImage, bool bShared)
{
	// image is true 32-bit, no longer need bitmap mask

	if (m_hbmpImage != NULL)
	{
		if (m_bDeleteObj)
			DeleteObject(m_hbmpImage);
		DeleteObject(m_hbmpMask);

		m_hbmpImage = NULL;
		m_hbmpMask = NULL;
	}

	if (bShared)
	{
		m_hbmpImage = hbmpImage;
		m_bDeleteObj = false;
	}
	else // copy image
	{
		m_hbmpImage = CopyBitmap32(hbmpImage);
		m_bDeleteObj = true;
	}

	BITMAP bmImage;
	GetObject(hbmpImage, sizeof(BITMAP), (LPVOID)&bmImage);
	m_szImage.cx = bmImage.bmWidth;
	m_szImage.cy = bmImage.bmHeight;
}

void CTransparentBitmap::OnDraw32(HDC hdcTarget, RECT &rcTarget)
{
	if (m_hbmpImage == NULL) // not ready, do nothing
		return;

	int wDraw = rcTarget.right - rcTarget.left;
	int hDraw = rcTarget.bottom - rcTarget.top;
	if (wDraw <= 0 || hDraw <= 0) // no room to draw, do nothing
		return;

	if (m_nAlignment == ALIGN_STRETCHTOFIT)
	{
		OnDrawStretchedImage32(hdcTarget, rcTarget);
		return;
	}

	HRGN hrgnClip = NULL;
	HRGN hrgnClipPrev = NULL;
	if (m_bClipImage)
	{
		GetClipRgn(hdcTarget, hrgnClipPrev);

		hrgnClip = CreateRectRgn(rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
		SelectClipRgn(hdcTarget, hrgnClip);
	}

	// prepare to draw image
	HDC hdcImage = CreateCompatibleDC(hdcTarget);
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, m_hbmpImage);

	// paint image
	// calc. coord. of top-left corner for painting
	int x, y;
	switch (m_nAlignment)
	{
	case ALIGN_TOPLEFT:
		x = rcTarget.left;
		y = rcTarget.top;
		break;
	case ALIGN_BOTTOMLEFT:
		x = rcTarget.left;
		y = rcTarget.bottom - m_szImage.cy;
		break;
	case ALIGN_TOPRIGHT:
		x = rcTarget.right - m_szImage.cx;
		y = rcTarget.top;
		break;
	case ALIGN_BOTTOMRIGHT:
		x = rcTarget.right - m_szImage.cx;
		y = rcTarget.bottom - m_szImage.cy;
		break;
	default: // ALIGN_CENTER
		x = rcTarget.left + (wDraw - m_szImage.cx) / 2;
		y = rcTarget.top + (hDraw - m_szImage.cy) / 2;
	}

	// bitblt does not work for 32-bit bitmap
	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.SourceConstantAlpha = 255;

	AlphaBlend(hdcTarget, x, y, m_szImage.cx, m_szImage.cy, hdcImage, 0, 0, m_szImage.cx, m_szImage.cy, bf);

	// release resource
	if (m_bClipImage)
	{
		SelectClipRgn(hdcTarget, hrgnClipPrev);
		DeleteObject(hrgnClip);
	}
	SelectObject(hdcImage, hbmpPrev);
	DeleteDC(hdcImage);
}

HBITMAP CTransparentBitmap::CopyBitmap32(HBITMAP hbmpImage)
{
	BITMAP bmImage;
	int n = GetObject(hbmpImage, sizeof(BITMAP), (LPVOID)&bmImage);
	if (n == 0)
		return NULL;

	SIZE szImage;
	szImage.cx = bmImage.bmWidth;
	szImage.cy = bmImage.bmHeight;

	HBITMAP hbmp32 = CreateBitmap32(szImage, NULL);
	if (hbmp32 == NULL)
		return NULL;

	HDC hdc32 = CreateCompatibleDC(GetDC(NULL));
	HGDIOBJ hbmpPrev32 = SelectObject(hdc32, hbmp32);

	HDC hdcImage = CreateCompatibleDC(GetDC(NULL));
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, hbmpImage);

	// bitblt does not work for 32-bit bitmap
	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.SourceConstantAlpha = 255;

	AlphaBlend(hdc32, 0, 0, szImage.cx, szImage.cy, hdcImage, 0, 0, szImage.cx, szImage.cy, bf);

	SelectObject(hdc32, hbmpPrev32);
	DeleteDC(hdc32);

	SelectObject(hdcImage, hbmpPrev);
	DeleteDC(hdcImage);

	return hbmp32;
}

void CTransparentBitmap::OnDrawStretchedImage32(HDC hdcTarget, RECT& rcTarget)
{
	// prepare to draw image
	int wDraw = rcTarget.right - rcTarget.left;
	int hDraw = rcTarget.bottom - rcTarget.top;

	HDC hdcImage = CreateCompatibleDC(hdcTarget);
	HGDIOBJ hbmpPrev = SelectObject(hdcImage, m_hbmpImage);

	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.SourceConstantAlpha = 255;

	AlphaBlend(hdcTarget, rcTarget.left, rcTarget.top, wDraw, hDraw, hdcImage, 0, 0, m_szImage.cx, m_szImage.cy, bf);

	// release resource
	SelectObject(hdcImage, hbmpPrev);
	DeleteDC(hdcImage);
}

bool CTransparentBitmap::IsImage32Bit(HBITMAP hbmpImage)
{
	BITMAP bmImage;
	if (0 == GetObject(hbmpImage, sizeof(BITMAP), (LPVOID)&bmImage))
		return false;


	return bmImage.bmBitsPixel == 32 ? true : false;
}