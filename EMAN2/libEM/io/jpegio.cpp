/*
 * Author: Steven Ludtke, 04/10/2003 (sludtke@bcm.edu)
 * Copyright (c) 2000-2006 Baylor College of Medicine
 *
 * This software is issued under a joint BSD/GNU license. You may use the
 * source code in this file under either license. However, note that the
 * complete EMAN2 and SPARX software packages have some GPL dependencies,
 * so you are responsible for compliance with the licenses of these packages
 * if you opt to use BSD licensing. The warranty disclaimer below holds
 * in either instance.
 *
 * This complete copyright notice must be included in any revised version of the
 * source code. Additional authorship citations may be added, but existing
 * author citations must be preserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * */

#ifdef USE_JPEG

#ifdef WIN32
#include <windows.h>
#endif	// WIN32

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "jpegio.h"
#include "geometry.h"
#include "util.h"

using namespace EMAN;

JpegIO::JpegIO(const string & fname, IOMode rw)
:	ImageIO(fname, rw)
{}

JpegIO::~JpegIO()
{
	if (file) {
		fclose(file);
		file = NULL;
	}

}

void JpegIO::init()
{
	ENTERFUNC;

	if (initialized) {
		return;
	}

	initialized = true;

	bool is_new_file = false;
	file = sfopen(filename, rw_mode, &is_new_file, true);

	if (! is_new_file) {
		throw ImageReadException(filename, "JPEG reading not supported");
	}
	else {
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, file);
	}

	rendermin = 0.0;
	rendermax = 0.0;

	jpegqual  = 75;

	EXITFUNC;
}

bool JpegIO::is_valid(const void *)
{
//	ENTERFUNC;

	return false;
}

// int JpegIO::read_header(Dict & dict, int image_index,
// const Region * area, bool)

int JpegIO::read_header(Dict &, int, const Region *, bool)
{
	ENTERFUNC;

	throw ImageReadException(filename, "JPEG reading not supported");

	EXITFUNC;

	return 0;
}

int JpegIO::write_header(const Dict & dict, int image_index,
						const Region* area, EMUtil::EMDataType, bool)
{
	ENTERFUNC;

	// single image format, index can only be zero

	if (image_index == -1) {
		image_index = 0;
	}

	if (image_index != 0) {
		throw ImageWriteException(filename,
			"JPEG file does not support stack.");
	}

	check_write_access(rw_mode, image_index);

	if ((int) dict["nz"] != 1) {
		LOGERR("Only support 2D JPEG file write");

		return 1;
	}

//	rendermin = (float)dict["render_min"];	// float value representing black in the output
//	rendermax = (float)dict["render_max"];	// float value representign white in the output

	EMUtil::getRenderLimits(dict, rendermin, rendermax, renderbits);

	jpegqual = (int)dict["jpeg_quality"];
	if (jpegqual == 0) jpegqual = 75;

	cinfo.image_width  = (int) dict["nx"];	/* image width and height, in pixels */
	cinfo.image_height = (int) dict["ny"];

	if (area) {
		cinfo.image_width  = (int) area->size[0];
		cinfo.image_height = (int) area->size[1];
	}

	cinfo.input_components = 1;     /* # of color components per pixel */
	cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, jpegqual, TRUE);

	EXITFUNC;

	return 0;
}

int JpegIO::read_data(float *, int, const Region *, bool)
{
	ENTERFUNC;

	EXITFUNC;

	return 0;
}

int JpegIO::write_data(float *data, int image_index, const Region* area,
					  EMUtil::EMDataType dt, bool)
{
	ENTERFUNC;

	if (image_index > 0)
		 throw ImageWriteException("N/A", "JPEG files are single-image only");

	if (area && (area->size[0] != cinfo.image_width ||
		 area->size[1] != cinfo.image_height))
		 throw ImageWriteException("N/A", "No region writing for JPEG images");

	int nx = cinfo.image_width, ny = cinfo.image_height;

	// If we didn't get any parameters in 'render_min' or 'render_max',
	// we need to find some good ones.

	if (renderbits==0 || renderbits>8) renderbits=8;
	EMUtil::getRenderMinMax(data, nx, ny, rendermin, rendermax, renderbits);

	auto [rendered_data, count] = getRenderedDataAndRendertrunc<unsigned char>(data, nx*ny);
	unsigned char *cdata = (unsigned char *)malloc(nx+1);

	/* Flip the image vertically, since EMAN use top-left corner as image origin
	   But PNG use bottom-left corner as image origin */
	// convert and write the data 1 scanline at a time

	JSAMPROW rp[1];
	rp[0] = cdata;

	jpeg_start_compress(&cinfo, TRUE);

	for (int i=ny-1; i>=0; i--) {
		for (int j=0; j<nx; j++) {
			cdata[j]= (int)rendered_data[i*nx+j];
		}

		jpeg_write_scanlines(&cinfo, rp, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	free(cdata);

	EXITFUNC;

	return 0;
}

int JpegIO::get_nimg() {
	return 0;
}

void JpegIO::flush()
{

}

bool JpegIO::is_complex_mode()
{
	return false;
}

bool JpegIO::is_image_big_endian()
{
	return true;
}

#endif	//USE_JPEG
