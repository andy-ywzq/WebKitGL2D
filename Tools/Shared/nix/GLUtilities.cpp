/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "GLUtilities.h"
#include <iostream>
#include <png.h>

using namespace std;

namespace ToolsNix {

void dumpGLBufferToPng(const char* fileName, int width, int height)
{
    FILE* fp = fopen(fileName, "w+");
    if (!fp) {
        cerr << "Error writing " << fileName << endl;
        return;
    }

    png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop pngInfo = png_create_info_struct(pngPtr);

    if (!pngPtr || !pngInfo) {
        png_destroy_write_struct(&pngPtr, &pngInfo);
        cerr << "Error creating PNG write strucs\n";
        return;
    }

    png_init_io(pngPtr, fp);
    png_set_IHDR(pngPtr, pngInfo, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(pngPtr, pngInfo);

    const size_t imageDataSize = width * height * 4;
    unsigned char* imageData = new unsigned char[imageDataSize];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    // write each row separately.
    png_bytep row = 0;
    for (int y = 0; y < height; ++y) {
        // glReadPixels returns the image upside down, so we write last rows first.
        png_uint_32 q = imageDataSize - (y + 1) * width * 4;
        row = (png_bytep) imageData + q;
        png_write_row(pngPtr, row);
    }
    delete[] imageData;

    png_write_end(pngPtr, 0);
    png_destroy_write_struct(&pngPtr, &pngInfo);

    fclose(fp);
}

void GLOffscreenBuffer::dumpToPng(const char* fileName)
{
    dumpGLBufferToPng(fileName, m_width, m_height);
}

RGBAPixel GLOffscreenBuffer::readPixelAtPoint(unsigned x, unsigned y)
{
    RGBAPixel result = { 0, 0, 0, 0 };
    // Note: the two first arguments indicate the lower left corner of the rectangle being sampled
    // and glReadPixels considers the origin (0, 0) is at bottom-left of the buffer. For example,
    // to get the top-left pixel (a 1x1 rectangle to be sampled), we need to use (0, height - 1).
    glReadPixels(x, m_height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<unsigned char*>(&result));
    return result;
}

} // namespace ToolsNix
