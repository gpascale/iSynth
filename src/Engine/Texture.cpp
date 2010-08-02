/*
 *  Texture.cpp
 *  Phonosynth
 *
 *  Created by Greg on 9/15/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "Texture.h"

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include <ContextManager.h>

#include <GTime.h>
#include <jpeglib.h>


#ifdef IPHONE

#include <GLDrawer.h>
#include <ObjCCommon.h>


#define TEXCOORD_OFFSET_EPS 0.003f


Texture::Texture() :
    m_hRatio(1.0f),
    m_vRatio(1.0f),
    m_hOffset(0.0f),
    m_vOffset(0.0f),
    m_glTexID(0),
	m_matrix(Matrix::IdentityCRef()),
	m_atlas(NULL)
{
}



Texture::~Texture()
{	
    if(m_glTexID != 0){		
		if(m_atlas){
			m_atlas->removeTexture(this);
			if(m_atlas->count() == 0){
				SAFE_DELETE(m_atlas);
				//TextureCounter::dec();
				//cerr << "FREEING GL TEXTURE " << m_glTexID << endl;
				GLDrawer::gregDeleteTextures(1, &m_glTexID);
				TextureMemory::textureDeleted(m_glTexID);
			}
		}
		else{
			//TextureCounter::dec();
			//cerr << "FREEING GL TEXTURE " << m_glTexID << endl;
			GLDrawer::gregDeleteTextures(1, &m_glTexID);
			TextureMemory::textureDeleted(m_glTexID);
		}
    }
}


Texture*
Texture::createTexture(const string& filename,
                       const TexCreateArgs& args)
{
	
	NSString* name = NSSTRING(filename.substr(0, filename.length() - 4).c_str());
	NSString* type = NSSTRING(filename.substr(filename.length() - 3).c_str());
	
	NSString* path = [[NSBundle mainBundle] pathForResource: name
													 ofType: type];
	
	if(path == nil)
		path = [[NSBundle mainBundle] pathForResource: name
											   ofType: type
										  inDirectory: @"buttons"];
		 
	
    UIImage* uiImage = [UIImage imageWithContentsOfFile: path];
    if(!uiImage){
        //cout << "uh oh, texture creation failed (uiImage was nil)" << endl;
        return NULL;
    }
    
    int width, height;
    ByteArray decompressed = decompress(uiImage, IMG_FMT_RAW_8888RGBA, false, width, height);
    Texture* ret = createTexture(decompressed, width, height, IMG_FMT_RAW_8888RGBA, args);
    
	decompressed.release(DESTRUCT_DELETE_ARRAY);
	
    return ret;
	
}


Texture*
Texture::createTexture(const ByteArray& data,
                       const ImageFormat dataFormat, // must be PNG or JPG
                       const TexCreateArgs& args)
{
    ASSERT(dataFormat == IMG_FMT_JPEG || dataFormat == IMG_FMT_PNG);
    
	int width, height;
    ByteArray decompressed = decompress(data, dataFormat, IMG_FMT_RAW_8888RGBA, false, width, height);
    
	Texture* ret;
	if(decompressed.data() != NULL){
		ret = createTexture(decompressed, width, height, IMG_FMT_RAW_8888RGBA, args);
		decompressed.release(DESTRUCT_DELETE_ARRAY);
	}
	else{
		ret = NULL;
	}
	
    return ret;
}



Texture* 
Texture::createTexture(const ByteArray& data, int width, int height,
                       const ImageFormat dataFormat,
                       const TexCreateArgs& args)
{    
    // make sure data being passed in is one of the supported formats
    ASSERT(dataFormat == IMG_FMT_PNG || dataFormat == IMG_FMT_JPEG || dataFormat == IMG_FMT_RAW_8888RGBA);
    
    // if an uncompressed data format is passed in, silently decompress it and call the function again,
    // making sure to clean up the intermediate decompressed buffer
    if(dataFormat == IMG_FMT_JPEG || dataFormat == IMG_FMT_PNG){
        return createTexture(data, dataFormat, args);
    }
    
    unsigned int glTex = createGLTexture(data, width, height, dataFormat, args);
    const int    wp2   = smallestPow2GEQ(width);
    const int    hp2   = smallestPow2GEQ(height);
    
    // whew! texture created successfully
    // create an actual texture object, fill it in and return it
    Texture* ret = new Texture();
    
    ret->m_textureFormat = args.textureFormat;
	
	
	// perturb all texture coordinates into the image a bit. This is a cheap way to deal with
	// the artifacts brought on by texture filtering at edges
    ret->m_hRatio        = (float) width / wp2 * (1 - (2.0f * TEXCOORD_OFFSET_EPS));
    ret->m_vRatio        = (float) height / hp2 * (1 - (2.0f * TEXCOORD_OFFSET_EPS));
    ret->m_hOffset       = TEXCOORD_OFFSET_EPS * width / wp2;
    ret->m_vOffset       = TEXCOORD_OFFSET_EPS * width / wp2;
    ret->m_width         = width;
    ret->m_height        = height;
    ret->m_slabWidth     = wp2;
    ret->m_slabHeight    = hp2;
	ret->m_glTexID       = glTex;
    ret->m_level         = log2(max(smallestPow2GEQ(width), smallestPow2GEQ(height)));
	
	
	// construct the texture matrix
	Matrix scaleMat; Matrix::ScaleMatrix(scaleMat, ret->m_hRatio, ret->m_vRatio, 1.0f);
	Matrix transMat; Matrix::TranslationMatrix(transMat, ret->m_hOffset, ret->m_vOffset, 0.0f);
	ret->m_matrix = scaleMat * transMat;
	 
    return ret;
}


Texture**
Texture::createTextureAtlas(const ByteArray& data,
                            const ImageFormat dataFormat,
                            const TexCreateArgs args,
                            const vector<GRect>& rects)
{
    ASSERT(dataFormat == IMG_FMT_JPEG || dataFormat == IMG_FMT_PNG);
    
    int width, height;
    ByteArray decompressed = decompress(data, dataFormat, IMG_FMT_RAW_8888RGBA, false, width, height);
	
	Texture** ret;
	if(decompressed.data() != NULL){
		ret = createTextureAtlas(decompressed, width, height, IMG_FMT_RAW_8888RGBA, args, rects);
		decompressed.release(DESTRUCT_DELETE_ARRAY);
	}
	else{
		ret = new Texture*[rects.size()];
		memset(ret, 0x00, rects.size() * sizeof(Texture*));
	}
	
				
    return ret;
}


Texture**
Texture::createTextureAtlas(const ByteArray& data, int width, int height,
                            const ImageFormat dataFormat,
                            const TexCreateArgs args,
                            const vector<GRect>& rects)
{
    // make sure data being passed in is one of the supported formats
	ASSERT(dataFormat == IMG_FMT_PNG || dataFormat == IMG_FMT_JPEG || dataFormat == IMG_FMT_RAW_8888RGBA);    
    
    // if an uncompressed data format is passed in, silently decompress it and call the function again,
    // making sure to clean up the intermediate decompressed buffer
    if(dataFormat == IMG_FMT_JPEG || dataFormat == IMG_FMT_PNG){
        return createTextureAtlas(data, dataFormat, args, rects);
    }
    
    unsigned int glTex = createGLTexture(data, width, height, dataFormat, args);    
    const int    wp2   = smallestPow2GEQ(width);
    const int    hp2   = smallestPow2GEQ(height);    
    
    const int nTextures = (const int) rects.size();
    Texture** ret = new Texture*[nTextures];
	SharedAtlas* atlas = new SharedAtlas();
	
    FOR(i,0,nTextures){
        ret[i] = new Texture();
                
		const float xr = ((float) rects[i].w / width);
		const float yr = ((float) rects[i].h / height);
		
		const float hOffset = TEXCOORD_OFFSET_EPS * ((float) width / wp2) * xr;
		const float vOffset = TEXCOORD_OFFSET_EPS * ((float) height / hp2) * yr;
		
		ret[i]->m_textureFormat = args.textureFormat;
        ret[i]->m_hRatio        = xr * ((float) width / wp2)  * (1.0f - (2.0f * TEXCOORD_OFFSET_EPS));
        ret[i]->m_vRatio        = yr * ((float) height / hp2) * (1.0f - (2.0f * TEXCOORD_OFFSET_EPS));
        ret[i]->m_hOffset       = ((float) rects[i].x / wp2)  + hOffset;
        ret[i]->m_vOffset       = ((float) rects[i].y / hp2)  + vOffset;
        ret[i]->m_width         = rects[i].w;
        ret[i]->m_height        = rects[i].h;
        ret[i]->m_slabWidth     = wp2;
        ret[i]->m_slabHeight    = hp2;
        ret[i]->m_glTexID       = glTex;
        ret[i]->m_level         = log2(max(smallestPow2GEQ(width), smallestPow2GEQ(height)));
		ret[i]->m_atlas			= atlas;
		
		atlas->addTexture(ret[i]);
		
		// construct the texture matrix
		Matrix scaleMat; Matrix::ScaleMatrix(scaleMat, ret[i]->m_hRatio, ret[i]->m_vRatio, 1.0f);
		Matrix transMat; Matrix::TranslationMatrix(transMat, ret[i]->m_hOffset, ret[i]->m_vOffset, 0.0f);
		ret[i]->m_matrix = scaleMat * transMat;
    }    	
	
    return ret;    
}



Texture*
Texture::createTextureFromText(const string& text,
							   int w,
							   int h,
							   const Color4F& textColor,
							   const Color4F& backgroundColor,
							   const string& fontName)
{
	Texture* ret = new Texture();
	
	int wp2 = smallestPow2GEQ(w);
	int hp2 = smallestPow2GEQ(h);		
	
	// binary search for the appropriate size for the font. We choose the largest possible size that
	// does not exceed the specified bounds in either dimension	
	NSString* nss = NSSTRING(text.c_str());
	CGSize cur;
	float lo = 0.0f, hi = 1000.0f, best = 0.0f;
	while(hi - lo > 1.0f){		
		float mid = (lo + hi) / 2;
		cur = [nss sizeWithFont: [UIFont fontWithName: NSSTRING(fontName.c_str()) size: mid]];
		if(cur.width >= wp2 || cur.height >= hp2)
			hi = mid;
		else
			best = lo = mid;
	}	
	
	// create a font using the experimentally determined size and compute the actual bounds of the
	// string when drawn with that font
	UIFont* font = [UIFont fontWithName: NSSTRING(fontName.c_str())
								   size: best];
	CGSize actualSz = [nss sizeWithFont: font];
	
	// shrink wp2 or hp2 if the actual size of the text with the font is small enough to allow it.	
	wp2 = smallestPow2GEQ(actualSz.width);
	hp2 = smallestPow2GEQ(actualSz.height);
	
		
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	ByteArray data(new unsigned char[wp2*hp2*4], wp2*hp2*4, DESTRUCT_DELETE_ARRAY, true);
	
	// set the background color before drawing the text
	if(backgroundColor != Color4F::None()){
		const Color4UC backgroundColorUC = convertColor4FToUC(backgroundColor);
		FOR(i,0,wp2*hp2)
			memcpy(&data[4*i], backgroundColorUC, sizeof(Color4UC));		
	}
	
	
	CGContextRef context = CGBitmapContextCreate((void*) data,
												 wp2,
												 hp2,
												 8,
												 wp2 * 4,
												 colorSpace,
												 kCGImageAlphaPremultipliedLast);		
	
	CGContextSetFillColorSpace(context, colorSpace);
	CGContextSetFillColor(context, textColor);
	
	// UIKit and CG are flipped apparently
	CGContextTranslateCTM(context, 0.0, hp2); // (1, 0) --> (0, -1)
	CGContextScaleCTM(context, 1.0, -1.0);	  // (0, -1) --> (0, 1)
						  
	UIGraphicsPushContext(context);

	CGSize sz = [nss drawAtPoint: CGPointMake(0, 0)
						withFont: font];
	
	UIGraphicsPopContext();
	
	CGContextRelease(context);
	CGColorSpaceRelease(colorSpace);
	
	unsigned int glTex = createGLTexture(data,
										 wp2,
										 hp2,
										 IMG_FMT_RAW_8888RGBA,
										 TexCreateArgs(TEX_FORMAT_RGBA_4444)); // texture format should include alpha
			
	ret->m_glTexID	  = glTex;
	ret->m_hRatio	  = sz.width / wp2;
	ret->m_vRatio	  = sz.height / hp2;
	ret->m_hOffset	  = 0.0f;
	ret->m_vOffset	  = 0.0f;
	ret->m_width	  = wp2;
	ret->m_height	  = hp2;
	
	Matrix::ScaleMatrix(ret->m_matrix, ret->m_hRatio, ret->m_vRatio, 1.0f);
	
	return ret;
}


unsigned int 
Texture::createGLTexture(const ByteArray& data, int width, int height,
                         const ImageFormat dataFormat,
                         const TexCreateArgs& args)
{
	//TextureCounter::inc();
	//cerr << "TextureCounter: " << TextureCounter::getCount() << endl;
	
    ASSERT (!((args.width < 0) ^ (args.height < 0)));    	    	
	
	int wp2 = smallestPow2GEQ(width);
	int hp2 = smallestPow2GEQ(height);
	
	if(args.stretchType == STRETCH_PRESERVE_RATIO){
		const float factor = (width > height) ? ((wp2+0.f) / width) : ((hp2+0.f) / height);
		width *= factor;
		height *= factor;
		wp2 = smallestPow2GEQ(width);
		hp2 = smallestPow2GEQ(height);
	}
    else if (args.stretchType == STRETCH_FILL_SLAB){
        width = wp2;
        height = hp2;
    }
	
    
	GLuint texID;	// to store the ID of the generated texture
	if(!GLDrawer::gregGenTextures(1, &texID))
		return 0;
	
	glBindTexture(GL_TEXTURE_2D, texID);
	
    // 512 * 512 is the largest supported texture size
    static GLubyte compressedData[513 * 513 * 4]; // static since only one thread creates textures. careful with this
    
    switch(args.textureFormat){        
            
        case TEX_FORMAT_RGBA_4444:
        {		            
            memset(compressedData, 0x00, sizeof(compressedData));                
            FOR(i,0,wp2 * hp2){                    
                // 4-4-4-4 texture format (RRRR GGGG BBBB AAAA)                    
                const GLubyte*  src =					&(data[i * 4]);
                unsigned short* dst = (unsigned short*) &(compressedData[i * 2]);							
                
                const unsigned short R = (const unsigned short) (*src) >> 4;
                const unsigned short G = (const unsigned short) (*(++src)) >> 4;
                const unsigned short B = (const unsigned short) (*(++src)) >> 4;
                const unsigned short A = (const unsigned short) (*(++src)) >> 4;
                
                (*dst) |= (R << 12);
                (*dst) |= (G << 8);
                (*dst) |= (B << 4);
                (*dst) |= A;                    
            }
			
            glTexImage2D(GL_TEXTURE_2D, 
                         0, 
                         GL_RGBA, 
                         wp2, 
                         hp2, 
                         0, 
                         GL_RGBA, 
                         GL_UNSIGNED_SHORT_4_4_4_4,
                         compressedData);
        
            break;
        }        		
        case TEX_FORMAT_RGB_565:
            {		
				// this is a HUUUGE bottleneck. A very sizeable percentage of CPU time (> 10%) is spent in this 
				// little conversion section. As such, it's hand optimized as much as possible. I
				// could probably do better by inlining assembly but I don't know how to do that
				// right now and there are other higher priority things to take care of.
				// TODO: ARM it up in here
				
				const GTime beg;
				
				GLubyte* src = reinterpret_cast<GLubyte*>(data.data());
				unsigned short* dst = reinterpret_cast<unsigned short*>(compressedData);
				
				const int colsLeft = wp2 - width;
				
				printf("width = %d, height = %d, wp2 = %d, hp2 = %d\n", width, height, wp2, hp2);
				for(int cRow = 0; cRow < height; cRow++){
					for(int cCol = 0; cCol < width; cCol++){
						#define RGB888_TO_565(R,G,B) (unsigned short) (((R)&0xF8)<<8)|(((G)&0xFC)<<3)|(((B)&0xF8)>>3)
						*dst = RGB888_TO_565((*src), (*(src+1)), (*(src+2)));
						src += 4;
						dst++;
						#undef RGB888_TO_565
					}
					if(cCol < wp2)
						*dst = 0; // duplicate the last pixel in the row
					src += colsLeft * 4;
					dst += colsLeft;
                }
				
				// duplicate the last row on the bottom
				if(height < hp2)
					memcpy(dst, dst - wp2, wp2 * 4);
				
				const GTime end;
				////cerr << "conversion took " << end - beg << "seconds" << endl;
								
                glTexImage2D(GL_TEXTURE_2D, 
                             0, 
                             GL_RGB, 
                             wp2, 
                             hp2, 
                             0, 
                             GL_RGB, 
                             GL_UNSIGNED_SHORT_5_6_5,
                             compressedData);
				
				////cerr << "glTexImage2D call took " << GTime::Now() - end << " seconds" << endl;
				
                break;
            }     
        case TEX_FORMAT_RGBA_8888:{
            // 8-8-8-8 texture format, (RRRRRRRR GGGGGGGG BBBBBBBB AAAAAAAA)
            glTexImage2D(GL_TEXTURE_2D, 
                         0, 
                         GL_RGBA, 
                         wp2, 
                         hp2, 
                         0,		
                         GL_RGBA, 
                         GL_UNSIGNED_BYTE,
                         data);
            break;
        }
        case TEX_FORMAT_RGB_888:{
            // 8-8-8 texture format, (RRRRRRRR GGGGGGGG BBBBBBBB XXXXXXXX)
            glTexImage2D(GL_TEXTURE_2D, 
                         0, 
                         GL_RGB, 
                         wp2, 
                         hp2, 
                         0,		
                         GL_RGBA, 
                         GL_UNSIGNED_BYTE,
                         data);
            break;
        }            
    }		


	if(args.textureFormat == TEX_FORMAT_RGBA_8888 || args.textureFormat == TEX_FORMAT_RGB_888)
		TextureMemory::textureCreated(texID, wp2 * hp2 * 4);
	else if(args.textureFormat == TEX_FORMAT_RGBA_4444 || args.textureFormat == TEX_FORMAT_RGB_565)
		TextureMemory::textureCreated(texID, wp2 * hp2 * 2);
	else
		ASSERT(0 && "Unrecognized texture format");	    
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
    return texID;
}



ByteArray
Texture::decompress(const ByteArray& data, 
                    ImageFormat fmtIn, ImageFormat fmtOut, 
                    bool pow2Dimensions,
                    OUT int& width, OUT int& height)
{
    
    ASSERT(fmtIn == IMG_FMT_PNG || fmtIn == IMG_FMT_JPEG);
    ASSERT(fmtOut == IMG_FMT_RAW_8888RGBA);

		
	NSData*		nsData = [[NSData alloc] initWithBytesNoCopy: data
													  length: data.length()
												freeWhenDone: NO];
    // using the above version right now because the Texture thread never releases its
	// autorelease pool at the moment, so this way I can release it manually
//	NSData*     nsData = [NSData dataWithBytes: data length: data.length()];
	UIImage*    uiImage = [[UIImage alloc] initWithData: nsData];
    
	ByteArray ret = NULL;
	
    if(uiImage != nil && nsData != nil){
		ret = decompress(uiImage, fmtOut, pow2Dimensions, width, height);
    }
	else{
		//cout << "uh-oh, image decompression failed (uiImage or nsData was nil)" << endl;
	}

	// free temporrary stuff used
	[uiImage release];
    [nsData release];
	
	return ret;
}


ByteArray
Texture::decompress(const UIImage* uiImage, 
                    ImageFormat fmtOut, 
                    bool pow2Dimensions,
                    OUT int& width, OUT int& height)
{
    
    CGImageRef  cgImage = [uiImage CGImage];
    if(!cgImage){
        //cout << "uh oh, texture creation failed (cgImage was nil)" << endl;
        return NULL;
    }
	
    return decompress(cgImage, fmtOut, pow2Dimensions, width, height);
}


ByteArray
Texture::decompress(const CGImageRef cgImage,
				    ImageFormat fmtOut,
				    bool pow2Dimensions,
					OUT int& width, OUT int& height)
{
	
	
    const int w = CGImageGetWidth(cgImage);
    const int h = CGImageGetHeight(cgImage);
    
	const int wp2 = smallestPow2GEQ(w);
	const int hp2 = smallestPow2GEQ(h);
	
	width = pow2Dimensions ? wp2 : w;
    height = pow2Dimensions ? hp2 : h;

    ByteArray ret(new unsigned char[wp2 * hp2 * 4],	// array to hold the raw image data
				  wp2 * hp2 * 4,					// size of the image
				  DESTRUCT_DO_NOTHING,				// keep this around
				  true);							// zero out the memory on creation
	
	static const CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	
	CGContextRef spriteContext = CGBitmapContextCreate((void*) ret,  
                                                       wp2,
                                                       hp2,
                                                       8,
                                                       wp2 * 4,
                                                       colorSpace,
			
                                                       kCGImageAlphaPremultipliedLast);
    
    if(!spriteContext){
        //cout << "uh-oh, texture creation failed (could not create CGContext)" << endl;
        ret.release(DESTRUCT_DELETE_ARRAY);
        return NULL;
    }
            
    CGContextDrawImage(spriteContext, 
					   CGRectMake(0.0, hp2 - height, width, height), 
					   cgImage);
	
	CGContextRelease(spriteContext);    
	
    return ret;    
            
}


// this ended up not being faster. lame

#if 0

ByteArray
Texture::fastDecompress(const ByteArray& jpegData,
						ImageFormat fmtOut,
						bool pow2Dimensions,
						OUT int& width, OUT int& height)
{

	static unsigned char raw_image[512 * 512 * 4];

	//decompressJpegInMemory(jpegData.data(), jpegData.length(), raw_image);
	
	/* these are standard libjpeg structures for reading(decompression) */
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1];
	
	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &jerr );
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress(&cinfo);
	/* this makes the library read from infile */
	jpeg_inmemory_src(&cinfo, jpegData.data(), jpegData.length());
	/* reading the image header which contains image information */
	
	jpeg_read_header(&cinfo, TRUE);
	/* Uncomment the following to output image information, if needed. */
	
	/*
	printf( "JPEG File Information: \n" );
	printf( "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
	printf( "Color components per pixel: %d.\n", cinfo.num_components );
	printf( "Color space: %d.\n", cinfo.jpeg_color_space );
	 */
	
	/* Start decompression jpeg here */
	jpeg_start_decompress( &cinfo );
	

	const int wp2 = smallestPow2GEQ(cinfo.output_width);
	const int hp2 = smallestPow2GEQ(cinfo.output_height);
	
	width  = pow2Dimensions ? wp2 : cinfo.output_width;
	height = pow2Dimensions ? hp2 : cinfo.output_height;
	
	/* now actually read the jpeg into the raw buffer */	
	/* read one scan line at a time */
		
	const int nComponents = cinfo.num_components;
	const int bytesPerRow = wp2 * 4;
	
	memset(raw_image, 0x00, wp2 * hp2 * 4);
	
	row_pointer[0] = (unsigned char*) malloc(bytesPerRow);
	
	int row = 0, col = 0;
	
	GTime start = GTime::Now();
	
	FOR(nn, 0, cinfo.image_height)
	//while(cinfo.output_scanline < cinfo.image_height)
	{
		col = 0;
		jpeg_read_scanlines(&cinfo, row_pointer, 1);		
		
		unsigned char* cur = raw_image + (row * bytesPerRow);
		for(int i = 0; i < cinfo.output_width * 3; i++){
			*(cur++) = row_pointer[0][i];
			if((i+1) % 3 == 0)
				cur++;
		}
		row++;
	}
	
	GTime end = GTime::Now();
	printf("internal texturization took %f seconds\n", end - start);
	
	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	/* yup, we succeeded! */

	return ByteArray(raw_image, wp2*hp2*nComponents);
}

#endif // 0

#endif IPHONE
