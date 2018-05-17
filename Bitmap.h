// 17/12/2013
// This encoder implemented by David Castells-Rufas (david.castells@uab.cat)
// to do research in parallel implementation of the JPEG encoding algorithm
// The code is implemented in C++ and derived from a Java version, see previous
// author licences below
//
/*
 * Windows Bitmap File Loader
 * Version based on 1.2.1 (20070430)
 *
 * Supported Formats: 24 Bit Images
 * Supported compression types: none
 *
 * Created by: Benjamin Kalytta, 2006 - 2007
 * Modified by: Vladimir Antonenko 2009
 * Modified by: David Castells 2013 to support Image like interface
 *
 * Licence: Free to use
 * Source can be found at http://www.kalytta.com/bitmap.h
 */


#ifndef BITMAP_H
#define	BITMAP_H


#include <stdlib.h>
#include <stdio.h>
//#include <memory.h>
#include <string.h>

#include "Image_i2c.h"

typedef unsigned char color;

#pragma once
#pragma pack(push)
#pragma pack(1)

#define BITMAP_SIGNATURE ('M'<<8 | 'B')     // 'MB' 

typedef __attribute__((aligned(1))) struct {
	unsigned short int Signature;
	unsigned int Size;
	unsigned int Reserved;
	unsigned int BitsOffset;
} BITMAP_FILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

typedef __attribute__((aligned(1))) struct {
	unsigned int HeaderSize;
	int Width;
	int Height;
	unsigned short int Planes;
	unsigned short int BitCount;
	unsigned int Compression;
	unsigned int SizeImage;
	int PelsPerMeterX;
	int PelsPerMeterY;
	unsigned int ClrUsed;
	unsigned int ClrImportant;
	unsigned int RedMask;
	unsigned int GreenMask;
	unsigned int BlueMask;
	unsigned int AlphaMask;
	unsigned int CsType;
	unsigned int Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
	unsigned int GammaRed;
	unsigned int GammaGreen;
	unsigned int GammaBlue;
} BITMAP_HEADER;

typedef struct __attribute__((aligned(1))) {
	color Red;
	color Green;
	color Blue;
	color Alpha;
} RGBA;

typedef struct __attribute__((aligned(1))) {
	color Blue;
	color Green;
	color Red;
	color Alpha;
} BGRA;

typedef struct __attribute__((aligned(1))) {
	color Blue;
	color Green;
	color Red;
} BGR;

typedef struct __attribute__((aligned(1))) {
	unsigned short int Blue:5;
	unsigned short int Green:5;
	unsigned short int Red:5;
	unsigned short int Reserved:1;
} BGR16;

#if 0

#define RIFF_SIGNATURE	0x46464952
#define RIFF_TYPE		0x204c4150
#define PAL_SIGNATURE	0x61746164
#define PAL_UNKNOWN		0x01000300

typedef struct {
	unsigned int Signature;
	unsigned int FileLength;
	unsigned int Type;
	unsigned int PalSignature;
	unsigned int ChunkSize;
	unsigned int Unkown;
} PAL;

#endif

#pragma pack(pop)


class CBitmap : public Image_I2C
{
private:
	BITMAP_FILEHEADER m_BitmapFileHeader;
	BITMAP_HEADER m_BitmapHeader;
	BGR *m_BitmapData;
	unsigned m_BitmapSize;
	unsigned m_Width;
	unsigned m_Height;

public:
	
	CBitmap()
        {
            m_Width = 320;
            m_Height = 240;
            m_BitmapSize = m_Width*m_Height;
            m_BitmapData = new BGR[m_BitmapSize];
        
	}
	
	
	
	
	
	bool Save(char* Filename)
	{
		FILE *file = fopen(Filename, "wb");

		if (file == 0) return false;
		
		BITMAP_FILEHEADER bfh = {0};
		BITMAP_HEADER bh = {0};

		bh.HeaderSize = sizeof(BITMAP_HEADER);
		bh.BitCount = 24;
		bh.Compression = 0; // RGB
		bh.Planes = 1;
		bh.Height = getHeight();
		bh.Width = getWidth();
		bh.SizeImage = bh.Width * bh.Height * bh.BitCount/8;
		bh.PelsPerMeterX = 3780;
		bh.PelsPerMeterY = 3780;

		bfh.Signature = BITMAP_SIGNATURE;
		bfh.BitsOffset = sizeof(BITMAP_HEADER) + sizeof(BITMAP_FILEHEADER);
		bfh.Size = bh.Width * bh.Height * bh.BitCount/8 + bfh.BitsOffset;
		
		unsigned char* Bitmap = (unsigned char*) m_BitmapData;

		fwrite(&bfh, sizeof(BITMAP_FILEHEADER), 1, file);
		fwrite(&bh, sizeof(BITMAP_HEADER), 1, file);
		fwrite(Bitmap, bh.SizeImage, 1, file);

		fclose(file);
		return true;
	}

	 int getWidth()  {
		return m_Width;
	}
	
	int getHeight()  {
		return m_Height;
	}
	
	unsigned int GetBitCount() const {
		return m_BitmapHeader.BitCount;
	}
	
	unsigned int GetBitmapSize() const {
		return m_BitmapSize;
	}

	BGR* getBytes() {
		return m_BitmapData;
	}

        void setGrey(int x, int y, unsigned char v)
        {
            unsigned offset = (m_Height - (y+1))*m_Width + x;
            BGR* bgr = &m_BitmapData[offset];
            
            bgr->Blue = v;
            bgr->Green = v;
            bgr->Red = v;
        }
        
        int getRGB(int x, int y)
        {
		unsigned offset = (m_Height - (y+1))*m_Width + x;

                BGR bgr = m_BitmapData[offset];

                int ARGB = (bgr.Red << 16) | ((bgr.Green << 8)) | ((bgr.Blue));
                return ARGB;

        }

        void getRGB(int x, int y, int* r, int* g, int* b)
        {
            unsigned offset = (m_Height - (y+1))*m_Width + x;

            BGR bgr = m_BitmapData[offset];

            *r = bgr.Red;
            *g = bgr.Green;
            *b = bgr.Blue;
        }

	/* Copies internal RGBA buffer to user specified buffer and convertes into destination bit format.
	 * Supported Bit depths are: 24
	 */
	bool GetBlock(unsigned x, unsigned y, unsigned sx, unsigned sy, BGR* buf)
	{
		//FUNC_ENTER();
		
		if ((y + sy) > m_Height || (x + sx) > m_Width) {
			//FUNC_EXIT();
			return false;
		}

		for (unsigned r = 0; r < sy; r++)
                {
			unsigned offset = (m_Height - (y+r+1))*m_Width + x;

			for (unsigned c = 0; c < sx; c++)
                        {
				unsigned i = offset + c;
				buf[sy*r + c] = m_BitmapData[i];
			}
		}

		//FUNC_EXIT();
		return true;
	}

	bool GetBlock16x16(unsigned x, unsigned y, BGRA buf[16][16])
	{
		if ((y + 16) > m_Height || (x + 16) > m_Width) {
			return false;
		}

		for (unsigned r = 0; r < 16; r++)
		{
			unsigned offset = (m_Height - (y+r+1))*m_Width + x;

			for (unsigned c = 0; c < 16; c++)
			{
				unsigned i = offset + c;

				buf[r][c].Blue  = m_BitmapData[i].Blue;
				buf[r][c].Green = m_BitmapData[i].Green;
				buf[r][c].Red   = m_BitmapData[i].Red;
				buf[r][c].Alpha = 1;
			}
		}

		return true;
	}

	bool GetBlock(unsigned x, unsigned y, unsigned sx, unsigned sy, color *R, color *G, color *B)
	{
		//FUNC_ENTER();
		if ((y + sy) > m_Height || (x + sx) > m_Width) {
			//FUNC_EXIT();
			return false;
		}

		for (unsigned r = 0; r < sy; r++) {
			unsigned offset = (m_Height - (y+r+1))*m_Width + x;

			for (unsigned c = 0; c < sx; c++) {
				unsigned i = offset + c;

				R[sy*r + c] = m_BitmapData[i].Red;
				G[sy*r + c] = m_BitmapData[i].Green;
				B[sy*r + c] = m_BitmapData[i].Blue;
			}
		}

		//FUNC_EXIT();
		return true;
	}


	bool GetBlock16x16(unsigned x, unsigned y, color R[16][16], color G[16][16], color B[16][16])
	{
		if ((y + 16) > m_Height || (x + 16) > m_Width) {
			return false;
		}

		for (unsigned r = 0; r < 16; r++)
		{
			unsigned offset = (m_Height - (y+r+1))*m_Width + x;
			unsigned *ptr = (unsigned*)(m_BitmapData + offset);

			// optimization for BGR color format
			for (unsigned c = 0, i = 0; c < 16; c += 4, i += 3)
			{
				unsigned n1 = ptr[i+0];
				unsigned n2 = ptr[i+1];
				unsigned n3 = ptr[i+2];

				*(unsigned*)&B[r][c] = ((n1 >>  0) & 0xff) | ((n1 >> 16) & 0xff00) | ((n2 >>  0) & 0xff0000) | ((n3 << 16) & 0xff000000);
				*(unsigned*)&G[r][c] = ((n1 >>  8) & 0xff) | ((n2 <<  8) & 0xff00) | ((n2 >>  8) & 0xff0000) | ((n3 <<  8) & 0xff000000);
				*(unsigned*)&R[r][c] = ((n1 >> 16) & 0xff) | ((n2 >>  0) & 0xff00) | ((n3 << 16) & 0xff0000) | ((n3 >>  0) & 0xff000000);
			}
		}

		return true;
	}


	bool SetBlock(unsigned x, unsigned y, unsigned sx, unsigned sy, BGR* buf)
	{
		if ((y + sy) > m_Height || (x + sx) > m_Width) {
			return false;
		}

		for (unsigned r = 0; r < sy; r++) {
			unsigned offset = (m_Height - (y+r+1))*m_Width + x;

			for (unsigned c = 0; c < sx; c++) {
				unsigned i = offset + c;
				m_BitmapData[i] = buf[sy*r + c];
			}
		}

		return true;
	}

};


#endif	/* BITMAP_H */

