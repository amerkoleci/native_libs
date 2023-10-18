// Copyright (c) Amer Koleci and Contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include "alimer.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <array>
#include <assert.h>
#include <vector>

std::string GetTexturesPath()
{
    return "assets/textures/";
}

std::vector<uint8_t> LoadTexture(const char* fileName)
{
    std::ifstream is(GetTexturesPath() + "/" + fileName, std::ios::binary | std::ios::in | std::ios::ate);
    
    if (is.is_open())
    {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        char* shaderCode = new char[size];
        is.read(shaderCode, size);
        is.close();

        assert(size > 0);

        std::vector<uint8_t> data(size);
        memcpy(data.data(), shaderCode, size);

        delete[] shaderCode;

        return data;
    }
    else
    {
        std::cerr << "Error: Could not open file \"" << fileName << "\"" << "\n";
        return {};
    }
}

void TestPng()
{
    std::vector<uint8_t> fileData = LoadTexture("10points.png");
    AlimerImage image = Alimer_ImageCreateFromMemory(fileData.data(), fileData.size());
    ImageDimension dimension = Alimer_ImageGetDimension(image);
    PixelFormat format = Alimer_ImageGetFormat(image);

    assert(dimension == ImageDimension_2D);
    assert(format == PixelFormat_RGBA8Unorm);
    Alimer_ImageDestroy(image);
}

void TestHdr()
{
    std::vector<uint8_t> fileData = LoadTexture("environment.hdr");
    AlimerImage image = Alimer_ImageCreateFromMemory(fileData.data(), fileData.size());
    ImageDimension dimension = Alimer_ImageGetDimension(image);
    PixelFormat format = Alimer_ImageGetFormat(image);

    assert(dimension == ImageDimension_2D);
    assert(format == PixelFormat_RGBA32Float);
    Alimer_ImageDestroy(image);
}

int main()
{
    TestPng();
    TestHdr();
    return 0;
}
