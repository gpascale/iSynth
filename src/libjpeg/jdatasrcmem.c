/*
 *  jdatasrcmem.c
 *  Phonosynth
 *
 *  Created by Greg on 1/20/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */


// libjpeg comes only with the ability to decompress a jpeg read in from a FILE*. This is an
// implementation of a data source which allows decompression of a jpeg stored in memory



#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"


/* Expanded data source object for stdio input */

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */
	
	unsigned char*		 inbuffer;		/* source buffer containing the JPEG data in memory */
	int					 nBytesLeft;		/* number of bytes unread in inbuffer */
	JOCTET*				 buffer;			/* start of buffer */
	boolean				 start_of_file;	/* have we gotten any data yet? */
} mem_source_mgr;

typedef mem_source_mgr* mem_src_ptr;

#define INPUT_BUF_SIZE  64000	/* choose an efficiently fread'able size */


/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF(void)
init_source_mem (j_decompress_ptr cinfo)
{
	mem_src_ptr src = (mem_src_ptr) cinfo->src;
	
	/* We reset the empty-input-file flag for each image,
	 * but we don't clear the input buffer.
	 * This is correct behavior for reading a series of images from one source.
	 */
	src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume5n6
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer_mem (j_decompress_ptr cinfo)
{
	printf("fill_input_buffer_mem\n");
	
	mem_src_ptr src = (mem_src_ptr) cinfo->src;	
	
	//nbytes = JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);
	// read in up to INPUT_BUF_SIZE bytes	
	int nbytes = (src->nBytesLeft < INPUT_BUF_SIZE) ? (src->nBytesLeft) : (INPUT_BUF_SIZE);	
	
	src->buffer = src->inbuffer;
	
	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}
	
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;
	
	src->inbuffer += nbytes;
	src->nBytesLeft -= nbytes;
	
	return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data_mem (j_decompress_ptr cinfo, long num_bytes)
{
	mem_src_ptr src = (mem_src_ptr) cinfo->src;
	
	/* Just a dumb implementation for now.  Could use fseek() except
	 * it doesn't work on pipes.  Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	 */
	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer_mem(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source_mem (j_decompress_ptr cinfo)
{
	/* no work necessary here */
}


GLOBAL(void)
jpeg_inmemory_src (j_decompress_ptr cinfo, unsigned char* buffer, int len)
{
	//printf("&cinfo = %p\n", cinfo);
	
	cinfo->src = (struct jpeg_source_mgr *)
	(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
								SIZEOF(mem_source_mgr));
	
    mem_src_ptr src = (mem_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
	(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
								INPUT_BUF_SIZE * SIZEOF(JOCTET));
	
	src->pub.init_source = init_source_mem;
	src->pub.fill_input_buffer = fill_input_buffer_mem;
	src->pub.skip_input_data = skip_input_data_mem;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source_mem;

	// in-memory specific stuff
	src->inbuffer = buffer;
	src->nBytesLeft = len;		
	
	//printf("nBytesLeft, inbuffer = %d, %p\n", src->nBytesLeft, src->inbuffer);
		
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}


GLOBAL(void)
decompressJpegInMemory()
{
	
	
}


