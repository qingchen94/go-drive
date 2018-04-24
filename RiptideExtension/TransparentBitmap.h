// TransparentBitmap.h: interface for the CTransparentBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPARENTBITMAP_H__70B6B4D7_448F_491A_B043_3B80F3E2538F__INCLUDED_)
#define AFX_TRANSPARENTBITMAP_H__70B6B4D7_448F_491A_B043_3B80F3E2538F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef DWORD HLSCOLOR;
#define HLS(h,l,s) ((HLSCOLOR)(((BYTE)(h)|((WORD)((BYTE)(l))<<8))|(((DWORD)(BYTE)(s))<<16)))

///////////////////////////////////////////////////////////////////////////////
#define HLS_H(hls) ((BYTE)(hls))
#define HLS_L(hls) ((BYTE)(((WORD)(hls)) >> 8))
#define HLS_S(hls) ((BYTE)((hls)>>16))

class CTransparentBitmap  
{
public:
	enum EImageAlignment
	{
		ALIGN_TOPLEFT = 0, // default
		ALIGN_BOTTOMLEFT = 1,
		ALIGN_TOPRIGHT = 2,
		ALIGN_BOTTOMRIGHT = 3,
		ALIGN_CENTER = 4,
		ALIGN_STRETCHTOFIT = 5
	};

	CTransparentBitmap(EImageAlignment nAlignment = ALIGN_TOPLEFT);
	~CTransparentBitmap();

	CTransparentBitmap(const CTransparentBitmap& other);

	CTransparentBitmap& operator = (const CTransparentBitmap& other);

	HBITMAP GetHandle(){return m_hbmpImage;};
	SIZE GetSize(){return m_szImage;};

	void SetImage(HBITMAP hbmpImage, bool bShared = true);
	void OnDraw(HDC hdcTarget, RECT& rcTarget);

	void SetAlignment(EImageAlignment nAlignment);
	EImageAlignment GetAlignment(){return m_nAlignment;};

	void EnableClipImage(bool bEnable = true){m_bClipImage = bEnable;};
	bool IsClipImageEnabled(){return m_bClipImage;};

	HBITMAP CreateBitmap32(HBITMAP hbmpImage);
	HBITMAP CreateBitmap32(HICON hicon);
	HICON CreateIcon(HBITMAP hbmpImage);
	HBITMAP CreateDisabledBitmap32(HICON hIcon);

	class CColorTheme
	{
	public:
		CColorTheme(){};

		// helper functions
		static HLSCOLOR RGB2HLS(COLORREF rgb);
		static BYTE _ToRGB(float rm1, float rm2, float rh);
		static COLORREF HLS2RGB(HLSCOLOR hls);
		static COLORREF HLS_TRANSFORM(COLORREF rgb, int percent_L, int percent_S);

	private:
	};

protected:
	void WriteBitInLine(LPBYTE pbBits, int& nStartBitOffset, bool IsZero);
	DWORD ReadPixelInLine(LPBYTE pbBits, int& nStartBitOffset,  int nBitsPixel);
	void WritePixelInLine(LPBYTE pbBits, int& nStartBitOffset, int nBitsPixel, DWORD dwPixel);
	DWORD GetPixelAt(BITMAP& bm, POINT& ptPixelPos);
	bool GetBitmap(HBITMAP hbmp, BITMAP& bm);
	HBITMAP BuildImageMask(HBITMAP hbmpImage, SIZE* pszImage = NULL);

	HBITMAP CreateBitmap32(const SIZE& size, void** ppBits);

	void SetImage32(HBITMAP hbmpImage, bool bShared = true);
	void OnDraw32(HDC hdcTarget, RECT& rcTarget);
	HBITMAP CopyBitmap32(HBITMAP hbmpImage);
	bool IsImage32Bit(HBITMAP hbmpImage);

	void OnDrawStretchedImage(HDC hdcTarget, RECT& rcTarget);
	void OnDrawStretchedImage32(HDC hdcTarget, RECT& rcTarget);

	bool m_bDeleteObj; // if true, release image at end
	HBITMAP m_hbmpImage; // handle to image
	HBITMAP m_hbmpMask; // handle to image mask for drawing transparent image
	SIZE m_szImage; // image size
	bool m_bClipImage; // if true, clip image outside of drawing rectangle
	EImageAlignment m_nAlignment; // define where/how to draw image
	bool m_bIsTrue32Bit;
};

#endif // !defined(AFX_TRANSPARENTBITMAP_H__70B6B4D7_448F_491A_B043_3B80F3E2538F__INCLUDED_)
