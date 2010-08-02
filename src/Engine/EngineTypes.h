/*
 *  EngineTypes.h
 *  Phonosynth
 *
 *  Created by Greg on 11/18/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _ENGINETYPES_H_
#define _ENGINETYPES_H_

enum VertexFormat
{
    FMT_FLOAT2,
    FMT_FLOAT3,
    FMT_FLOAT4,
    FMT_UCHAR2,
    FMT_UCHAR3,
    FMT_UCHAR4,
    FMT_USHORT
};


enum DrawMode
{
    DRAWMODE_POINTS,
    DRAWMODE_LINES,
	DRAWMODE_LINE_LOOP,
    DRAWMODE_TRIANGLES,
    DRAWMODE_TRIANGLESTRIP,
    DRAWMODE_TRIANGLEFAN
};


enum VBType
{
	VB_STATIC,
	VB_DYNAMIC
};


enum ImageFormat
{
    IMG_FMT_RAW_565RGB,
    IMG_FMT_RAW_4444RGBA,
    IMG_FMT_RAW_888RGB,
    IMG_FMT_RAW_8888RGBA,
    IMG_FMT_PNG,
    IMG_FMT_JPEG
};


enum TextureFormat
{
    TEX_FORMAT_RGB_565,
	TEX_FORMAT_BGR_565,
    TEX_FORMAT_RGBA_4444,
	TEX_FORMAT_BGRA_4444,
    TEX_FORMAT_RGB_888,
	TEX_FORMAT_BGR_888,
    TEX_FORMAT_RGBA_8888,
	TEX_FORMAT_BGRA_8888
};


enum TexStretchType
{
    STRETCH_NONE,
    STRETCH_PRESERVE_RATIO,
    STRETCH_FILL_SLAB
};


// there's only one kind of multitexturing I use but I could potentially add more
enum MultitextureType
{
	MULTITEXTURE_INTERPOLATE
};

#endif // _ENGINETYPES_H_