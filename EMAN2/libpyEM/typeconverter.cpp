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

#define NO_IMPORT_ARRAY
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include "typeconverter.h"
#include "emdata.h"

namespace python = boost::python;
namespace np = boost::python::numpy;

using namespace EMAN;

np::ndarray EMNumPy::em2numpy(const EMData *const image)
{
	float * data = image->get_data();
	int nx = image->get_xsize();
	int ny = image->get_ysize();
	int nz = image->get_zsize();

	vector<int> dims;

	if (nz > 1) dims.push_back(nz);
	if (ny > 1) dims.push_back(ny);

	dims.push_back(nx);

	return make_numeric_array(data, dims);
}

EMData* EMNumPy::numpy2em(const np::ndarray& array)
{
	int ndim = array.get_nd();

#if defined (__LP64__) //is it a 64-bit platform?
	long nx=1, ny=1, nz=1;
#else	//for 32 bit platform
	int nx=1, ny=1, nz=1;
#endif // defined (__LP64__)

	if (ndim <= 0 || ndim > 3) {
		LOGERR("%dD numpy array to EMData is not supported.", ndim);
		return 0;
	}

	switch(ndim) {
		case 1:
			nx = array.shape(0);
			break;
		case 2:
			ny = array.shape(0);
			nx = array.shape(1);
			break;
		case 3:
			nz = array.shape(0);
			ny = array.shape(1);
			nx = array.shape(2);
			break;
	}

	auto arr = array.astype(np::dtype::get_builtin<float>());
	float * float_array = (float *)arr.get_data();
	float * temparray = new float[(size_t)nx*ny*nz];
	std::copy(float_array, float_array + nx * ny * nz, temparray);

	EMData* image = new EMData((float*)temparray, nx, ny, nz);

	image->set_attr("apix_x", 1.0);
	image->set_attr("apix_y", 1.0);
	image->set_attr("apix_z", 1.0);

	image->update();
	return image;
}


EMData* EMNumPy::register_numpy_to_emdata(const np::ndarray& array)
{

	if (!PyArray_Check(array.ptr())) {
		PyErr_SetString(PyExc_ValueError, "expected a PyArrayObject");
		return 0;
	}

	PyArrayObject * array_ptr = (PyArrayObject*) array.ptr();
//	Py_INCREF(array_ptr);	//this is for letting EMData take the ownership of the data array
	int ndim = PyArray_NDIM(array_ptr); //->nd;
	//char data_type = PyArray_DESCR(array_ptr)->type; //array_ptr->descr->type;

	npy_intp *dims_ptr = (npy_intp*)PyArray_DIMS(array_ptr);

#if defined (__LP64__) //is it a 64-bit platform?
	//long * dims_ptr = (long*)array_ptr->dimensions;
	long nx=1, ny=1, nz=1;
#else	//for 32 bit platform
	//int * dims_ptr = (int*)array_ptr->dimensions;
	int nx=1, ny=1, nz=1;
#endif // defined (__LP64__)

	if (ndim <= 0 || ndim > 3) {
		LOGERR("%dD numpy array to EMData is not supported.", ndim);
		return 0;
	}

	if (ndim == 1) {
		nx = dims_ptr[0];
	}
	else if (ndim == 2) {
		ny = dims_ptr[0];
		nx = dims_ptr[1];
	}
	else if (ndim == 3) {
		nz = dims_ptr[0];
		ny = dims_ptr[1];
		nx = dims_ptr[2];
	}

	emdata_buffer.register_buffer_data((float*)PyArray_DATA(array_ptr), nx, ny, nz);

	return &emdata_buffer;
}

void EMNumPy::unregister_numpy_from_emdata()
{
	emdata_buffer.unregister_buffer_data();
}


PyObject* EMObject_to_python::convert(EMObject const& emobj)
{

	EMObject::ObjectType t = emobj.get_type();
	PyObject * result = 0;

	if (t == EMObject::BOOL) {
#ifdef IS_PY3K
		result = PyLong_FromLong((bool)emobj);
#else
		result = PyInt_FromLong((bool)emobj);
#endif	//IS_PY3K
	}
	if(t == EMObject::SHORT) {
#ifdef IS_PY3K
		result = PyLong_FromLong((short)emobj);
#else
		result = PyInt_FromLong((short)emobj);
#endif	//IS_PY3K
	}
	if (t == EMObject::INT) {
#ifdef IS_PY3K
		result = PyLong_FromLong((int)emobj);
#else
		result = PyInt_FromLong((int)emobj);
#endif	//IS_PY3K
	}
	else if (t == EMObject::FLOAT) {
		result = PyFloat_FromDouble((float) emobj);
	}
	else if (t == EMObject::DOUBLE) {
		result = PyFloat_FromDouble((double) emobj);
	}
	else if (t == EMObject::STRING) {
#ifdef IS_PY3K
		result = PyUnicode_FromString((const char*) emobj);
#else
		result = PyString_FromString((const char*) emobj);
#endif	//IS_PY3K
	}
	else if (t == EMObject::EMDATA) {
		EMData * img = (EMData*) emobj;
		result = python::incref(python::object(img).ptr());
	}
	else if (t == EMObject::XYDATA) {
		XYData * xyd = (XYData*) emobj;
		result = python::incref(python::object(xyd).ptr());
	}
	else if (t == EMObject::TRANSFORM ) {
		Transform * trans = (Transform*) emobj;
		result = python::incref(python::object(trans).ptr());
	}
	else if (t == EMObject::CTF ) {
		Ctf * ctf_ = (Ctf*) emobj;
		string str = ctf_->to_string();

		if(str.at(0) == 'O') {
			EMAN1Ctf* c = dynamic_cast<EMAN1Ctf*>(ctf_);
			result = python::incref(python::object(c).ptr());
		}
		else if(str.at(0) == 'E') {
			EMAN2Ctf* c = dynamic_cast<EMAN2Ctf*>(ctf_);
			result = python::incref(python::object(c).ptr());
		}
		else {
			printf("Ctf object wrong...\n");
		}
	}
	else if (t == EMObject::FLOATARRAY) {
		vector<float> farray = emobj;
		python::list flist;

		for (size_t i = 0; i < farray.size(); i++) {
			flist.append(farray[i]);
		}

		result = python::incref(python::list(flist).ptr());
	}
	else if (t == EMObject::INTARRAY) {
		vector<int> iarray = emobj;
		python::list ilist;

		for (size_t i = 0; i < iarray.size(); i++) {
			ilist.append(iarray[i]);
		}

		result = python::incref(python::list(ilist).ptr());
	}
	else if (t == EMObject::STRINGARRAY) {
		vector<string> strarray = emobj;
		python::list flist;

		for (size_t i = 0; i < strarray.size(); i++) {
			flist.append(strarray[i]);
		}

		result = python::incref(python::list(flist).ptr());
	}
	else if (t == EMObject::TRANSFORMARRAY) {
		vector<Transform> transformarray = emobj;
		python::list tlist;

		for (size_t i = 0; i < transformarray.size(); i++) {
			tlist.append(transformarray[i]);
		}

		result = python::incref(python::list(tlist).ptr());
	}
	else if (t == EMObject::FLOAT_POINTER) {
		float* fp = (float*) emobj;
		result = python::incref(python::object(fp).ptr());
	}
	else if (t == EMObject::INT_POINTER) {
		int* ip = (int*) emobj;
		result = python::incref(python::object(ip).ptr());
	}
	else if (t == EMObject::UNKNOWN) {
		result = python::incref(Py_None);
	}

	return result;
}

EMNumPy::~EMNumPy()
{
	// Setting rdata data member of EMData to 0 (Null)
	// avoids that the destructor of EMData Buffer deletes the valid memory,
	// which allocated and owned by the other object (e.g. NumPy)
	emdata_buffer.unregister_buffer_data();
}
