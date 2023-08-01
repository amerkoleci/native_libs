// Copyright © Amer Koleci and Contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include "alimer_image.h"
#include <stdio.h>
#include <stdlib.h>
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
    Image image = alimerImageCreateFromMemory(fileData.data(), fileData.size());
    ImageDimension dimension = alimerImageGetDimension(image);
    ImageFormat format = alimerImageGetFormat(image);

    assert(dimension == ImageDimension_2D);
    assert(format == ImageFormat_RGBA8Unorm);
    alimerImageDestroy(image);
}

void TestHdr()
{
    std::vector<uint8_t> fileData = LoadTexture("environment.hdr");
    Image image = alimerImageCreateFromMemory(fileData.data(), fileData.size());
    ImageDimension dimension = alimerImageGetDimension(image);
    ImageFormat format = alimerImageGetFormat(image);

    assert(dimension == ImageDimension_2D);
    assert(format == ImageFormat_RGBA32Float);
    alimerImageDestroy(image);
}

int main()
{
    TestPng();
    TestHdr();
    return 0;
}
