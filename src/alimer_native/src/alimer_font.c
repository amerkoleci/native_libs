// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_internal.h"

ALIMER_DISABLE_WARNINGS()
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"
ALIMER_ENABLE_WARNINGS()

typedef struct AlimerFontImpl {
    stbtt_fontinfo  info;
    int ascent;
    int descent;
    int lineGap;
} AlimerFontImpl;

AlimerFont Alimer_FontCreateFromMemory(const uint8_t* data, size_t size)
{
    ALIMER_UNUSED(size);

    if (stbtt_GetNumberOfFonts(data) <= 0)
    {
        //Alimer_LogError("Unable to parse Font File");
        return NULL;
    }

    AlimerFontImpl* font = ALIMER_ALLOC(AlimerFontImpl);
    assert(font);

    int offset = stbtt_GetFontOffsetForIndex(data, 0);

    if (stbtt_InitFont(&font->info, data, offset) == 0)
    {
        //Alimer_LogError("Unable to parse Font File");
        ALIMER_FREE(font);
        return NULL;
    }


    stbtt_GetFontVMetrics(&font->info, &font->ascent, &font->descent, &font->lineGap);

    return  font;
}

void Alimer_FontDestroy(AlimerFont font)
{
    ALIMER_FREE(font);
}

void Alimer_FontGetMetrics(AlimerFont font, int* ascent, int* descent, int* linegap)
{
    stbtt_GetFontVMetrics(&font->info, ascent, descent, linegap);
}

int Alimer_FontGetGlyphIndex(AlimerFont font, int codepoint)
{
    return stbtt_FindGlyphIndex(&font->info, codepoint);
}

float Alimer_FontGetScale(AlimerFont font, float size)
{
    return stbtt_ScaleForMappingEmToPixels(&font->info, size);
}

float Alimer_FontGetKerning(AlimerFont font, int glyph1, int glyph2, float scale)
{
    return stbtt_GetGlyphKernAdvance(&font->info, glyph1, glyph2) * scale;
}

void Alimer_FontGetCharacter(AlimerFont font, int glyph, float scale, int* width, int* height, float* advance, float* offsetX, float* offsetY, int* visible)
{
    int adv, ox, x0, y0, x1, y1;

    stbtt_GetGlyphHMetrics(&font->info, glyph, &adv, &ox);
    stbtt_GetGlyphBitmapBox(&font->info, glyph, scale, scale, &x0, &y0, &x1, &y1);

    *width = (x1 - x0);
    *height = (y1 - y0);
    *advance = adv * scale;
    *offsetX = ox * scale;
    *offsetY = (float)y0;
    *visible = *width > 0 && *height > 0 && stbtt_IsGlyphEmpty(&font->info, glyph) == 0;

}

void Alimer_FontGetPixels(AlimerFont font, uint8_t* dest, int glyph, int width, int height, float scale)
{
    // parse it directly into the dest buffer
    stbtt_MakeGlyphBitmap(&font->info, dest, width, height, width, scale, scale, glyph);

    // convert the buffer to RGBA data by working backwards, overwriting data
    int len = width * height;
    for (int a = (len - 1) * 4, b = (len - 1); b >= 0; a -= 4, b -= 1)
    {
        dest[a + 0] = dest[b];
        dest[a + 1] = dest[b];
        dest[a + 2] = dest[b];
        dest[a + 3] = dest[b];
    }
}
