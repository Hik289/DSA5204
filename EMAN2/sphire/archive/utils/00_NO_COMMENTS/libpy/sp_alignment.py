#
from __future__ import print_function
# Author: Markus Stabrin 2019 (markus.stabrin@mpi-dortmund.mpg.de)
# Author: Fabian Schoenfeld 2019 (fabian.schoenfeld@mpi-dortmund.mpg.de)
# Author: Thorsten Wagner 2019 (thorsten.wagner@mpi-dortmund.mpg.de)
# Author: Tapu Shaikh 2019 (tapu.shaikh@mpi-dortmund.mpg.de)
# Author: Adnan Ali 2019 (adnan.ali@mpi-dortmund.mpg.de)
# Author: Luca Lusnig 2019 (luca.lusnig@mpi-dortmund.mpg.de)
# Author: Toshio Moriya 2019 (toshio.moriya@kek.jp)
# Author: Pawel A.Penczek, 09/09/2006 (Pawel.A.Penczek@uth.tmc.edu)
#
# Copyright (c) 2019 Max Planck Institute of Molecular Physiology
# Copyright (c) 2000-2006 The University of Texas - Houston Medical School
#
# This software is issued under a joint BSD/GNU license. You may use the
# source code in this file under either license. However, note that the
# complete EMAN2 and SPARX software packages have some GPL dependencies,
# so you are responsible for compliance with the licenses of these packages
# if you opt to use BSD licensing. The warranty disclaimer below holds
# in either instance.
#
# This complete copyright notice must be included in any revised version of the
# source code. Additional authorship citations may be added, but existing
# author citations must be preserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#


def ali2d_single_iter(data, numr, wr, cs, tavg, cnx, cny, \
						xrng, yrng, step, nomirror = False, mode="F", CTF=False, \
						random_method="", T=1.0, ali_params="xform.align2d", delta = 0.0):
	"""
		single iteration of 2D alignment using ormq
		if CTF = True, apply CTF to data (not to reference!)
	"""
	from sp_utilities import combine_params2, inverse_transform2, get_params2D, set_params2D
	from sp_alignment import ormq, ornq

	if CTF:
		from sp_filter  import filt_ctf

	maxrin = numr[-1]  #  length
	ou = numr[-3]  #  maximum radius
	if random_method == "SCF":
		from sp_fundamentals import fft, scf
		from sp_alignment import multalign2d_scf
		frotim = [fft(tavg)]
		xrng = int(xrng+0.5)
		yrng = int(yrng+0.5)
		cimage = Util.Polar2Dm(scf(tavg), cnx, cny, numr, mode)
		Util.Frngs(cimage, numr)
		Util.Applyws(cimage, numr, wr)
	else:
		# 2D alignment using rotational ccf in polar coords and quadratic interpolation
		cimage = Util.Polar2Dm(tavg, cnx, cny, numr, mode)
		Util.Frngs(cimage, numr)
		Util.Applyws(cimage, numr, wr)

	sx_sum = 0.0
	sy_sum = 0.0
	sxn = 0.
	syn = 0.
	mn = 0
	nope = 0
	mashi = cnx-ou-2
	for im in range(len(data)):
		if CTF:
			#Apply CTF to image
			ctf_params = data[im].get_attr("ctf")
			ima = filt_ctf(data[im], ctf_params, True)
		else:
			ima = data[im]

		if( random_method == "PCP"):
			sxi = data[im][0][0].get_attr('sxi')
			syi = data[im][0][0].get_attr('syi')
			nx = ny = data[im][0][0].get_attr('inx')
		else:
			nx = ima.get_xsize()
			ny = ima.get_ysize()
			alpha, sx, sy, mirror, dummy = get_params2D(data[im], ali_params)
			alpha, sx, sy, dummy         = combine_params2(alpha, sx, sy, mirror, 0.0, -cs[0], -cs[1], 0)
			alphai, sxi, syi, scalei     = inverse_transform2(alpha, sx, sy)
			#  introduce constraints on parameters to accomodate use of cs centering
			sxi = min(max(sxi,-mashi),mashi)
			syi = min(max(syi,-mashi),mashi)

		#  The search range procedure was adjusted for 3D searches, so since in 2D the order of operations is inverted, we have to invert ranges
		txrng = search_range(nx, ou, sxi, xrng, "ali2d_single_iter")
		txrng = [txrng[1],txrng[0]]
		tyrng = search_range(ny, ou, syi, yrng, "ali2d_single_iter")
		tyrng = [tyrng[1],tyrng[0]]
		#print im, "B",cnx,sxi,syi,txrng, tyrng
		# align current image to the reference
		if random_method == "SHC":
			"""Multiline Comment0"""
			#MULTILINEMULTILINEMULTILINE 0
			#MULTILINEMULTILINEMULTILINE 0
			#MULTILINEMULTILINEMULTILINE 0
			#MULTILINEMULTILINEMULTILINE 0
			#MULTILINEMULTILINEMULTILINE 0
			#MULTILINEMULTILINEMULTILINE 0
			#  For shc combining of shifts is problematic as the image may randomly slide away and never come back.
			#  A possibility would be to reject moves that results in too large departure from the center.
			#  On the other hand, one cannot simply do searches around the proper center all the time,
			#    as if xr is decreased, the image cannot be brought back if the established shifts are further than new range
			olo = Util.shc(ima, [cimage], txrng, tyrng, step, -1.0, mode, numr, cnx+sxi, cny+syi, "c1")
			##olo = Util.shc(ima, [cimage], xrng, yrng, step, -1.0, mode, numr, cnx, cny, "c1")
			if(data[im].get_attr("previousmax")<olo[5]):
				#[angt, sxst, syst, mirrort, peakt] = ormq(ima, cimage, xrng, yrng, step, mode, numr, cnx+sxi, cny+syi, delta)
				#print  angt, sxst, syst, mirrort, peakt,olo
				angt = olo[0]
				sxst = olo[1]
				syst = olo[2]
				mirrort = int(olo[3])
				# combine parameters and set them to the header, ignore previous angle and mirror
				[alphan, sxn, syn, mn] = combine_params2(0.0, -sxi, -syi, 0, angt, sxst, syst, mirrort)
				set_params2D(data[im], [alphan, sxn, syn, mn, 1.0], ali_params)
				##set_params2D(data[im], [angt, sxst, syst, mirrort, 1.0], ali_params)
				data[im].set_attr("previousmax",olo[5])
			else:
				# Did not find a better peak, but we have to set shifted parameters, as the average shifted
				set_params2D(data[im], [alpha, sx, sy, mirror, 1.0], ali_params)
				nope += 1
				mn = 0
				sxn = 0.0
				syn = 0.0
		elif random_method == "SCF":
			sxst,syst,iref,angt,mirrort,totpeak = multalign2d_scf(data[im], [cimage], frotim, numr, xrng, yrng, ou = ou)
			[alphan, sxn, syn, mn] = combine_params2(0.0, -sxi, -syi, 0, angt, sxst, syst, mirrort)
			set_params2D(data[im], [alphan, sxn, syn, mn, 1.0], ali_params)
		elif random_method == "PCP":
			[angt, sxst, syst, mirrort, peakt] = ormq_fast(data[im], cimage, txrng, tyrng, step, numr, mode, delta)
			sxst = rings[0][0][0].get_attr("sxi")
			syst = rings[0][0][0].get_attr("syi")
			sxprint(sxst, syst,sx,sy)
			dummy,sxs,sys, dummy = inverse_transform2(-angt,sx+sxst,sy+syst)
			set_params2D(data[im][0][0], [angt, sxs, sys, mirrort, 1.0], ali_params)
		else:
			if nomirror:  [angt, sxst, syst, mirrort, peakt] = ornq(ima, cimage, txrng, tyrng, step, mode, numr, cnx+sxi, cny+syi)
			else:	      [angt, sxst, syst, mirrort, peakt] = ormq(ima, cimage, txrng, tyrng, step, mode, numr, cnx+sxi, cny+syi, delta)
			# combine parameters and set them to the header, ignore previous angle and mirror
			[alphan, sxn, syn, mn] = combine_params2(0.0, -sxi, -syi, 0, angt, sxst, syst, mirrort)
			set_params2D(data[im], [alphan, sxn, syn, mn, 1.0], ali_params)

		if mn == 0: sx_sum += sxn
		else:       sx_sum -= sxn
		sy_sum += syn

	return sx_sum, sy_sum, nope


"""Multiline Comment1"""
#MULTILINEMULTILINEMULTILINE 1
#MULTILINEMULTILINEMULTILINE 1
							#MULTILINEMULTILINEMULTILINE 1
							#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1

	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1

	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
	#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
			#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1

		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1
		#MULTILINEMULTILINEMULTILINE 1

#MULTILINEMULTILINEMULTILINE 1
#MULTILINEMULTILINEMULTILINE 1

def ang_n(tot, mode, maxrin):
	"""
	  Calculate angle based on the position of the peak
	"""
	from math import fmod
	if (mode == 'f' or mode == 'F'): return fmod(((tot-1.0) / maxrin+1.0)*360.0, 360.0)
	else:                            return fmod(((tot-1.0) / maxrin+1.0)*180.0, 180.0)

# Copy of this function is implemented in C++ in Util (Util.Applyws). It works much faster than this one.
"""Multiline Comment2"""
#MULTILINEMULTILINEMULTILINE 2
	#MULTILINEMULTILINEMULTILINE 2
	  #MULTILINEMULTILINEMULTILINE 2
	#MULTILINEMULTILINEMULTILINE 2
	#MULTILINEMULTILINEMULTILINE 2
	#MULTILINEMULTILINEMULTILINE 2
	#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
		#MULTILINEMULTILINEMULTILINE 2
			#MULTILINEMULTILINEMULTILINE 2
			#MULTILINEMULTILINEMULTILINE 2
#MULTILINEMULTILINEMULTILINE 2

def crit2d(args, data):
	#print  " AMOEBA ",args
	#  data: 0 - kb,  1 - mask, 2 - nima,  3 - current ave, 4 - current image in the gridding format
	#from utilities import info
	from sp_fundamentals import rtshgkb
	mn = data[4].get_attr('mirror')
	temp = rtshgkb(data[4], args[0], args[1], args[2], data[0])
	if  mn: temp.process_inplace("xform.mirror", {"axis":'x'})
	#temp2 = data[3] + temp/data[2]
	temp2 = Util.madn_scalar(data[3], temp, 1.0/data[2]) 
	v = temp2.cmp("dot", temp2, {"negative":0, "mask":data[1]})
	#print  " AMOEBA ",args,mn,v
	return v



def eqproj_cascaded_ccc_fitness_function(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor

	volft   = data[0]
	kb	    = data[1]
	prj	    = data[2]
	mask2D  = data[3]
	refi    = data[4]
	shift   = data[5]
	ts      = data[6]
	#print  "Input shift ",shift
	R = Transform({"type":"spider", "phi":args[0], "theta":args[1], "psi":args[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	refprj = volft.extract_plane(R, kb)
	refprj.fft_shuffle()
	refprj.center_origin_fft()

	if(shift[0]!=0. or shift[1]!=0.):
		filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
				  "x_shift" : shift[0], "y_shift" : shift[1], "z_shift" : 0.0}
		refprj = Processor.EMFourierFilter(refprj, filt_params)

	refprj.do_ift_inplace()
	MM = refprj.get_ysize()
	refprj.set_attr_dict({'npad':2})
	refprj.depad()

	if ts==0.0:
		return ccc(prj, refprj, mask2D), shift

	refprj.process_inplace("normalize.mask", {"mask":mask2D, "no_sigma":1})
	Util.mul_img(refprj, mask2D)

	product = ccf(fpol(refprj, MM, MM, 1, False), data[4])
	nx = product.get_ysize()
	sx = nx//2
	sy = sx
	# This is for debug purpose
	#if ts == -1.0:
	#	return twoD_fine_search([sx, sy], [product, kb, -ts, sx]), shift

	ts2 = 2*ts
	data2 = [product, kb, 1.1*ts2, sx]
	size_of_ccf = 2*int(ts+1.5)+1
	pk = peak_search(Util.window(product, size_of_ccf, size_of_ccf,1,0,0,0))
	# adjust pk to correspond to large ccf
	# print  " pk ",pk
	pk[0][1] = sx + pk[0][4]
	pk[0][2] = sy + pk[0][5]
	#print  " pk ",pk
	# step in amoeba should be vicinity of the peak, within one pixel or even less.
	ps = amoeba([pk[0][1], pk[0][2]], [1.1, 1.1], twoD_fine_search, 1.e-4, 1.e-4, 1, data2)
	#print  " ps ",ps,[sx,sy], data2, shift
	#print  " if ",abs(sx-ps[0][0]),abs(sy-ps[0][1]),ts2
	if(  abs(sx-ps[0][0]) >= ts2 or abs(sy-ps[0][1]) >= ts2 ):
		return  twoD_fine_search([sx,sy], data2), shift
	else:
		s2x = (sx-ps[0][0])/2 + shift[0]
		s2y = (sy-ps[0][1])/2 + shift[1]
		#print  " B ",ps[1], [s2x, s2y]
		return ps[1], [s2x, s2y]

def format_list(l):
	return "["+", ".join(["%10.6f" % x for x in l])+"]"


def objective_function_just_ccc_has_maximum(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor
	from sp_projection import prgl
	from math import sqrt

	# return 1
	import numpy as np
	
	# if type(args).__module__ == np.__name__:
	# 	args = args.tolist()

	data[5] = args[3:5]

	volft   = data[0]
	kb	    = data[1]
	prj	    = data[2]
	mask2D  = data[3]
	refi    = data[4]
	shift   = data[5]
	ts      = data[6]
	
	# 2016-02-08--15-25-45-589 
	# #print  "Input shift ",shift
	# R = Transform({"type":"spider", "phi":args[0], "theta":args[1], "psi":args[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	# refprj = volft.extract_plane(R, kb)
	# refprj.fft_shuffle()
	# refprj.center_origin_fft()
	# 
	# if(shift[0]!=0. or shift[1]!=0.):
	# 	filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
	# 			  "x_shift" : shift[0], "y_shift" : shift[1], "z_shift" : 0.0}
	# 	refprj = Processor.EMFourierFilter(refprj, filt_params)
	# 
	# refprj.do_ift_inplace()
	# refprj.set_attr_dict({'npad':2})
	# refprj.depad()

	reference_projection = prgl(volft, args[0:5], interpolation_method = 1, return_real = True)
	reference_projection.set_attr("is_complex",0)

	# peak = Util.innerproduct(temp, emimage[im])
	# peak /= nrmref

	# norm_of_reference_projection = sqrt(reference_projection.cmp("dot", reference_projection, dict(negative = 0)))
	# rrr = -reference_projection.cmp("dot", prj, dict(negative = 0, mask = mask2D))/ norm_of_reference_projection


	norm_of_reference_projection = sqrt(Util.innerproduct(reference_projection, reference_projection, None))
	rrr =  Util.innerproduct(prj, reference_projection, None) / norm_of_reference_projection

	# print "ccc:", format_list(args[0:5]), rrr
	# with open("test.txt", "a") as myfile:
	# 	myfile.write("%f\n"%rrr)

	return rrr

def objective_function_just_ccc_has_minimum(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor
	from sp_projection import prgl
	from math import sqrt


	# volft   = data[0]
	# prj	    = data[2]
	

	reference_projection = prgl(data[0], args[0:5], interpolation_method = 1, return_real = False)
	reference_projection.set_attr("is_complex",0)

	norm_of_reference_projection = sqrt(Util.innerproduct(reference_projection, reference_projection, None))
	return  -Util.innerproduct(data[2], reference_projection, None) / norm_of_reference_projection
	# rrr =  -Util.innerproduct(prj, reference_projection) / norm_of_reference_projection

	# print "ccc:", format_list(args[0:5]), rrr
	# with open("test.txt", "a") as myfile:
	# 	myfile.write("%f\n"%rrr)
	
	# return rrr



def objective_function_just_ccc_has_minimum_reduced(args, data):
	# in this version, args contains only the angles
	from sp_projection import prgl
	from math import sqrt

	import numpy as np
	
	# volft   = data[0]
	# prj	    = data[2]

	args1 = np.append(args, data[5])
	reference_projection = prgl(data[0], args1 , interpolation_method = 1, return_real = False)
	reference_projection.set_attr("is_complex",0)

	norm_of_reference_projection = sqrt(Util.innerproduct(reference_projection, reference_projection, None))
	return  -Util.innerproduct(data[2], reference_projection, None) / norm_of_reference_projection
	# rrr =  -Util.innerproduct(prj, reference_projection) / norm_of_reference_projection

	# print "ccc:", format_list(args[0:5]), rrr
	# with open("test.txt", "a") as myfile:
	# 	myfile.write("%f\n"%rrr)
	
	# return rrr

def objective_function_just_ccc_has_minimum_reduced_only_shifts(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor
	from sp_projection import prgl
	from math import sqrt

	# return 1
	import numpy as np
	
	# if type(args).__module__ == np.__name__:
	# 	args = args.tolist()

	# data[5] = args[3:5]

	volft   = data[0]
	kb	    = data[1]
	prj	    = data[2]
	mask2D  = data[3]
	refi    = data[4]
	shift   = data[5]
	ts      = data[6]
	
	# 2016-02-08--15-25-45-589 
	# #print  "Input shift ",shift
	# R = Transform({"type":"spider", "phi":args[0], "theta":args[1], "psi":args[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	# refprj = volft.extract_plane(R, kb)
	# refprj.fft_shuffle()
	# refprj.center_origin_fft()
	# 
	# if(shift[0]!=0. or shift[1]!=0.):
	# 	filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
	# 			  "x_shift" : shift[0], "y_shift" : shift[1], "z_shift" : 0.0}
	# 	refprj = Processor.EMFourierFilter(refprj, filt_params)
	# 
	# refprj.do_ift_inplace()
	# refprj.set_attr_dict({'npad':2})
	# refprj.depad()

	# args1 = np.append(args, data[5])
	args1 = np.append(data[5], args)
	
	
	reference_projection = prgl(volft, args1 , interpolation_method = 1, return_real = True)
	reference_projection.set_attr("is_complex",0)

	# peak = Util.innerproduct(temp, emimage[im])
	# peak /= nrmref

	norm_of_reference_projection = sqrt(reference_projection.cmp("dot", reference_projection, dict(negative = 0)))
	rrr =  -reference_projection.cmp("dot", prj, dict(negative = 0, mask = mask2D))/ norm_of_reference_projection


	# norm_of_reference_projection = sqrt(Util.innerproduct(reference_projection, reference_projection))
	# rrr =  -Util.innerproduct(prj, reference_projection) / norm_of_reference_projection

	# print "ccc:", format_list(args[0:5]), rrr
	# with open("test.txt", "a") as myfile:
	# 	myfile.write("%f\n"%rrr)
	
	return rrr

def objective_function_just_ccc_has_minimum2(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor
	from sp_projection import prgl
	from math import sqrt

	# return 1
	import numpy as np
	
	# if type(args).__module__ == np.__name__:
	# 	args = args.tolist()

	# data[5] = args[3:5]

	volft   = data[0]
	kb	    = data[1]
	prj	    = data[2]
	mask2D  = data[3]
	refi    = data[4]
	shift   = data[5]
	ts      = data[6]
	
	# 2016-02-08--15-25-45-589 
	# #print  "Input shift ",shift
	# R = Transform({"type":"spider", "phi":args[0], "theta":args[1], "psi":args[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	# refprj = volft.extract_plane(R, kb)
	# refprj.fft_shuffle()
	# refprj.center_origin_fft()
	# 
	# if(shift[0]!=0. or shift[1]!=0.):
	# 	filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
	# 			  "x_shift" : shift[0], "y_shift" : shift[1], "z_shift" : 0.0}
	# 	refprj = Processor.EMFourierFilter(refprj, filt_params)
	# 
	# refprj.do_ift_inplace()
	# refprj.set_attr_dict({'npad':2})
	# refprj.depad()

	reference_projection = prgl(volft, args[0:5], interpolation_method = 1, return_real = True)
	reference_projection.set_attr("is_complex",0)

	# peak = Util.innerproduct(temp, emimage[im])
	# peak /= nrmref

	norm_of_reference_projection = sqrt(reference_projection.cmp("dot", reference_projection, dict(negative = 0)))
	return -reference_projection.cmp("dot", prj, dict(negative = 0, mask = mask2D))/ norm_of_reference_projection

	# norm_of_reference_projection = sqrt(Util.innerproduct(reference_projection, reference_projection))
	# return -Util.innerproduct(prj, reference_projection) / norm_of_reference_projection


	
	
# def prgl(volft, params, interpolation_method = 0, return_real = True):
# 	"""
# 		Name
# 			prgl - calculate 2-D projection of a 3-D volume
# 		Input
# 			vol: input volume, the volume has to be cubic
# 			params: input parameters given as a list [phi, theta, psi, s2x, s2y], projection in calculated using the three Eulerian angles and then shifted by sx,sy
# 		Output
# 			proj: generated 2-D projection

	# 2016-02-08--15-25-45-589 
	# return -ccc(prj, refprj, mask2D)

def objective_function_just_ccc_has_maximum___old(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor

	# return 1
	import numpy as np
	
	# if type(args).__module__ == np.__name__:
	# 	args = args.tolist()

	data[5] = args[3:5]

	volft   = data[0]
	kb	    = data[1]
	prj	    = data[2]
	mask2D  = data[3]
	refi    = data[4]
	shift   = data[5]
	ts      = data[6]
	#print  "Input shift ",shift
	R = Transform({"type":"spider", "phi":args[0], "theta":args[1], "psi":args[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	refprj = volft.extract_plane(R, kb)
	refprj.fft_shuffle()
	refprj.center_origin_fft()

	if(shift[0]!=0. or shift[1]!=0.):
		filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
				  "x_shift" : shift[0], "y_shift" : shift[1], "z_shift" : 0.0}
		refprj = Processor.EMFourierFilter(refprj, filt_params)

	refprj.do_ift_inplace()
	refprj.set_attr_dict({'npad':2})
	refprj.depad()

	return ccc(prj, refprj, mask2D)
	


def objective_function_just_ccc_rewrite(params, volft, kb, data_im, mask2D):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor
	# import numpy as np
	
	# if type(args).__module__ == np.__name__:
	# 	args = args.tolist()
	
	#print  "Input shift ",shift
	R = Transform({"type":"spider", "phi":params[0], "theta":params[1], "psi":params[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	refprj = volft.extract_plane(R, kb)
	refprj.fft_shuffle()
	refprj.center_origin_fft()

	if(params[3]!=0. or params[4]!=0.):
		filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
				  "x_shift" : params[3], "y_shift" : params[4], "z_shift" : 0.0}
		refprj = Processor.EMFourierFilter(refprj, filt_params)

	refprj.do_ift_inplace()
	refprj.set_attr_dict({'npad':2})
	refprj.depad()

	return -ccc(data_im, refprj, mask2D)


def eqproj_cascaded_ccc(args, data):
	from sp_utilities     import peak_search, amoeba
	from sp_fundamentals  import fft, ccf, fpol
	from sp_alignment     import twoD_fine_search
	from sp_statistics    import ccc
	from EMAN2 import Processor

	volft   = data[0]
	kb	    = data[1]
	prj	    = data[2]
	mask2D  = data[3]
	refi    = data[4]
	shift   = data[5]
	ts      = data[6]
	#print  "Input shift ",shift
	R = Transform({"type":"spider", "phi":args[0], "theta":args[1], "psi":args[2], "tx":0.0, "ty":0.0, "tz":0.0, "mirror":0, "scale":1.0})
	refprj = volft.extract_plane(R, kb)
	refprj.fft_shuffle()
	refprj.center_origin_fft()

	if(shift[0]!=0. or shift[1]!=0.):
		filt_params = {"filter_type" : Processor.fourier_filter_types.SHIFT,
				  "x_shift" : shift[0], "y_shift" : shift[1], "z_shift" : 0.0}
		refprj = Processor.EMFourierFilter(refprj, filt_params)

	refprj.do_ift_inplace()
	MM = refprj.get_ysize()
	refprj.set_attr_dict({'npad':2})
	refprj.depad()

	if ts==0.0:
		return ccc(prj, refprj, mask2D), shift

	refprj.process_inplace("normalize.mask", {"mask":mask2D, "no_sigma":1})
	Util.mul_img(refprj, mask2D)

	product = ccf(fpol(refprj, MM, MM, 1, False), data[4])
	nx = product.get_ysize()
	sx = nx//2
	sy = sx
	# This is for debug purpose
	# if ts == -1.0:
	# 	return twoD_fine_search([sx, sy], [product, kb, -ts, sx]), shift
	
	

	ts2 = 2*ts
	data2 = [product, kb, 1.1*ts2, sx]
	size_of_ccf = 2*int(ts+1.5)+1
	pk = peak_search(Util.window(product, size_of_ccf, size_of_ccf,1,0,0,0))
	# adjust pk to correspond to large ccf
	# print  " pk ",pk
	pk[0][1] = sx + pk[0][4]
	pk[0][2] = sy + pk[0][5]
	#print  " pk ",pk
	# step in amoeba should be vicinity of the peak, within one pixel or even less.
	ps = amoeba([pk[0][1], pk[0][2]], [1.1, 1.1], twoD_fine_search, 1.e-4, 1.e-4, 500, data2)
	#print  " ps ",ps,[sx,sy], data2, shift
	#print  " if ",abs(sx-ps[0][0]),abs(sy-ps[0][1]),ts2
	if(  abs(sx-ps[0][0]) >= ts2 or abs(sy-ps[0][1]) >= ts2 ):
		return  twoD_fine_search([sx,sy], data2), shift
	else:
		s2x = (sx-ps[0][0])/2 + shift[0]
		s2y = (sy-ps[0][1])/2 + shift[1]
		#print  " B ",ps[1], [s2x, s2y]
		return ps[1], [s2x, s2y]

def twoD_fine_search(args, data):
	if(abs(args[0]-data[3]) > data[2] or abs(args[1]-data[3]) > data[2]): return -1.0e22
	return data[0].get_pixel_conv7(args[0], args[1], 0.0, data[1])

def eqproj(args, data):
	from sp_projection import prgs
	#from fundamentals import cyclic_shift
	#from utilities import info
	#print  " AMOEBA ",args
	#  data: 0 - volkb,  1 - kb, 2 - image,  3 - mask,
	#  args: 0 - phi, 1 - theta, 2 - psi, 3 - sx, 4 - sy
	prj = prgs(data[0], data[1], args)

	# the idea is for the mask to "follow" the projection
	#isx = int(args[3]+100000.5)-100000 # this is a strange trick to take care of negative sx
	#isy = int(args[4]+100000.5)-100000
	#shifted_mask = cyclic_shift(data[3], isx, isy)
	#info(proj)
	#info(data[2])
	#info(proj,None,"proj")
	#info(data[2],None,"data[2")
	#info(data[3],None,"data[3")

	#info(shifted_mask,None,"shifted mask")
	#v = -proj.cmp("SqEuclidean", data[2], {"mask":shifted_mask})
	#        CURRENTLY THE DISTANCE IS cross-correlation coefficient
	#v = -prj.cmp("SqEuclidean", data[2], {"mask":data[3]})
	v = prj.cmp("ccc", data[2], {"mask":data[3], "negative":0})
	#v = proj.cmp("ccc", data[2], {"mask":shifted_mask, "negative":0})
	#print  " AMOEBA o", args, v
	return v

def eqprojDot(args, data):
	from sp_projection import project
	from sp_filter import filt_ctf
	phi = args[0]
	tht = args[1]
	psi = args[2]
	vol = data[0]
	img = data[1]
	s2x = data[2]
	s2y = data[3]
	msk = data[4]
	CTF = data[5]
	ou  = data[6]

	tmp = img.process( "normalize.mask", {"mask":msk, "no_sigma":0} )
	ref = project( vol, [phi,tht,psi,-s2x,-s2y], ou )
	if CTF:
		ctf = img.get_attr( "ctf" )
		ref = filt_ctf( ref, ctf )
	return ref.cmp( "dot", tmp, {"mask":msk, "negative":0} )

def eqprojEuler(args, data):
	from sp_projection import prgs
	prj = prgs(data[0], data[1], [args[0], args[1], args[2], data[3], data[4]])
	v = prj.cmp("ccc", data[2], {"mask":data[5], "negative":0})
	return v

def symm_func(args, data):
	from sp_utilities import sym_vol
	from sp_fundamentals  import  rot_shift3D
	sym = sym_vol(rot_shift3D(data[0], args[0], args[1], args[2]), data[2])
	avg = sym.cmp("dot",sym,{"mask":data[1], "negative":0})
	sxprint(avg, args)
	return avg

def find_symm(vol, mask, sym_gp, phi, theta, psi, scale, ftolerance, xtolerance):
	
	from sp_utilities import amoeba, model_circle
	from sp_alignment import symm_func
	args   = [phi, theta, psi]
	data   = [vol, mask, sym_gp]
	result = amoeba(args, scale, symm_func, ftolerance, xtolerance, 500, data)

	return result

#   Implemented in c in utli_sparx
#  Helper functions for ali2d_r
def kbt(nx,npad=2):
	# padd two times
	N=nx*npad
	# support of the window
	K=6
	alpha=1.75
	r=nx/2
	v=K/2.0/N
	return Util.KaiserBessel(alpha, K, r, K/(2.*N), N)
     

#  AP stuff  01/18/06
    
    
def log2(n):
	""" Returns the smallest power by which 2 has to be raised to obtain
	    an integer less equal n
	"""
	m = 1
	k =-1
	while (m <= n):
		i = m
		k +=1
		m = 2*i
	return k
    
def Numrinit(first_ring, last_ring, skip=1, mode="F"):
	"""This function calculates the necessary information for the 2D 
	   polar interpolation. For each ring, three elements are recorded:
	   numr[i*3]:  Radius of this ring
	   numr[i*3+1]: Total number of samples of all inner rings+1
	   		(Or, the beginning point of this ring)
	   numr[i*3+2]: Number of samples of this ring. This number is an 
	   		FFT-friendly power of the 2.
			
	   "F" means a full circle interpolation
	   "H" means a half circle interpolation
	"""
	MAXFFT = 32768
	from math import pi

	if (mode == 'f' or mode == 'F'): dpi = 2*pi
	else:                            dpi = pi
	numr = []
	lcirc = 1
	for k in range(first_ring, last_ring+1, skip):
		numr.append(k)
		jp = int(dpi * k+0.5)
		ip = 2**(log2(jp)+1)  # two times oversample each ring
		if (k+skip <= last_ring and jp > ip+ip//2): ip=min(MAXFFT,2*ip)
		if (k+skip  > last_ring and jp > ip+ip//5): ip=min(MAXFFT,2*ip)

		numr.append(lcirc)
		numr.append(ip)
		lcirc += ip

	return  numr
"""Multiline Comment3"""
#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	   #MULTILINEMULTILINEMULTILINE 3
	   #MULTILINEMULTILINEMULTILINE 3
	   #MULTILINEMULTILINEMULTILINE 3
	   		#MULTILINEMULTILINEMULTILINE 3
	   #MULTILINEMULTILINEMULTILINE 3
	   		#MULTILINEMULTILINEMULTILINE 3

	   #MULTILINEMULTILINEMULTILINE 3
	   #MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3

	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
		#MULTILINEMULTILINEMULTILINE 3
		#MULTILINEMULTILINEMULTILINE 3
		#MULTILINEMULTILINEMULTILINE 3
		#MULTILINEMULTILINEMULTILINE 3
	#MULTILINEMULTILINEMULTILINE 3
#MULTILINEMULTILINEMULTILINE 3

def ringwe(numr, mode="F"):
	"""
	   Calculate ring weights for rotational alignment
	   The weights are r*delta(r)*delta(phi).
	"""
	from math import pi
	if (mode == 'f' or mode == 'F'):
		dpi = 2*pi
	else:
		dpi = pi
	nring = len(numr)/3
	wr=[0.0]*nring
	maxrin = float(numr[len(numr)-1])
	for i in range(0,nring): wr[i] = numr[i*3]*dpi/float(numr[2+i*3])*maxrin/float(numr[2+i*3])
	return wr

def ornq(image, crefim, xrng, yrng, step, mode, numr, cnx, cny, deltapsi = 0.0):
	"""Determine shift and rotation between image and reference image (refim)
	   no mirror
		quadratic interpolation
		cnx, cny in FORTRAN convention
	"""
	from math import pi, cos, sin, radians
	from sp_alignment import ang_n
	#from utilities import info
	#print "ORNQ"
	peak = -1.0E23

	lkx = int(xrng[0]/step)
	rkx = int(xrng[-1]/step)

	lky = int(yrng[0]/step)
	rky = int(yrng[-1]/step)

	for i in range(-lky, rky+1):
		iy = i*step
		for j in range(-lkx, rkx+1):
			ix = j*step
			cimage = Util.Polar2Dm(image, cnx+ix, cny+iy, numr, mode)
			Util.Frngs(cimage, numr)
			retvals = Util.Crosrng_e(crefim, cimage, numr, 0, deltapsi)
			qn = retvals["qn"]
			if qn >= peak:
				sx = -ix
				sy = -iy
				ang = ang_n(retvals["tot"], mode, numr[-1])
				peak = qn
	# mirror is returned as zero for consistency
	mirror = 0
	co =  cos(radians(ang))
	so = -sin(radians(ang))
	sxs = sx*co - sy*so
	sys = sx*so + sy*co
	return  ang, sxs, sys, mirror, peak


def ormq(image, crefim, xrng, yrng, step, mode, numr, cnx, cny, delta = 0.0):
	"""Determine shift and rotation between image and reference image (crefim)
		crefim should be as FT of polar coords with applied weights
	        consider mirror
		quadratic interpolation
		cnx, cny in FORTRAN convention
	"""
	from math import pi, cos, sin, radians
	#print "ORMQ"
	peak = -1.0E23

	lkx = int(xrng[0]/step)
	rkx = int(xrng[-1]/step)

	lky = int(yrng[0]/step)
	rky = int(yrng[-1]/step)

	for i in range(-lky, rky+1):
		iy = i*step
		for j in range(-lkx, rkx+1):
			ix = j*step
			cimage = Util.Polar2Dm(image, cnx+ix, cny+iy, numr, mode)
			Util.Frngs(cimage, numr)
			# The following code it used when mirror is considered
			if delta == 0.0:
				retvals = Util.Crosrng_ms(crefim, cimage, numr, 0.0)
			else:
				retvals = Util.Crosrng_ms_delta(crefim, cimage, numr, 0.0, delta)
			qn = retvals["qn"]
			qm = retvals["qm"]
			if (qn >= peak or qm >= peak):
				sx = -ix
				sy = -iy
				if (qn >= qm):
					ang = ang_n(retvals["tot"], mode, numr[-1])
					peak = qn
					mirror = 0
				else:
					ang = ang_n(retvals["tmt"], mode, numr[-1])
					peak = qm
					mirror = 1
			"""Multiline Comment4"""
			#MULTILINEMULTILINEMULTILINE 4
			#MULTILINEMULTILINEMULTILINE 4
			#MULTILINEMULTILINEMULTILINE 4
			#MULTILINEMULTILINEMULTILINE 4
				#MULTILINEMULTILINEMULTILINE 4
				#MULTILINEMULTILINEMULTILINE 4
				#MULTILINEMULTILINEMULTILINE 4
				#MULTILINEMULTILINEMULTILINE 4
				#MULTILINEMULTILINEMULTILINE 4
			#MULTILINEMULTILINEMULTILINE 4
	co  =  cos(radians(ang))
	so  = -sin(radians(ang))
	sxs = sx*co - sy*so
	sys = sx*so + sy*co
	return  ang, sxs, sys, mirror, peak

def ormq_fast(dimage, crefim, xrng, yrng, step, numr, mode, delta = 0.0):
	"""Determine shift and rotation between image and reference image (crefim)
		crefim should be as FT of polar coords with applied weights
	        consider mirror
		cnx, cny in FORTRAN convention
	"""
	#from math import pi, cos, sin, radians
	#print "ORMQ_FAST"
	maxrange = len(dimage)//2
	#istep = int(2*step)
	istep = int(step)
	"""Multiline Comment5"""
	#MULTILINEMULTILINEMULTILINE 5
	#MULTILINEMULTILINEMULTILINE 5

	#MULTILINEMULTILINEMULTILINE 5
	#MULTILINEMULTILINEMULTILINE 5
	#MULTILINEMULTILINEMULTILINE 5
	lkx = rkx = int(xrng*istep)

	lky = rky = int(yrng*istep)

	peak = -1.0E23
	for j in range(-lky, rky+1, istep):
		for i in range(-lkx, rkx+1, istep):
			if delta == 0.0: retvals = Util.Crosrng_ms(crefim, dimage[i+maxrange][j+maxrange], numr, 0.0)
			else:            retvals = Util.Crosrng_ms_delta(crefim, dimage[i+maxrange][j+maxrange], numr, delta)
			qn = retvals["qn"]
			qm = retvals["qm"]
			if (qn >= peak or qm >= peak):
				sx = i
				sy = j
				if (qn >= qm):
					ang = ang_n(retvals["tot"], mode, numr[-1])
					peak = qn
					mirror = 0
				else:
					ang = ang_n(retvals["tmt"], mode, numr[-1])
					peak = qm
					mirror = 1
	"""Multiline Comment6"""
	#MULTILINEMULTILINEMULTILINE 6
	#MULTILINEMULTILINEMULTILINE 6
	#MULTILINEMULTILINEMULTILINE 6
	#MULTILINEMULTILINEMULTILINE 6
	#MULTILINEMULTILINEMULTILINE 6
	if( peak < -1.0e20): ERROR("ormq_fast","failed, most likely due to search ranges",1)
	#return  ang, sx/2.0, sy/2.0, mirror, peak
	return  ang, sx, sy, mirror, peak
			

def prepref(data, maskfile, cnx, cny, numr, mode, maxrangex, maxrangey, step):
	from sp_utilities import get_params2D, combine_params2
	from EMAN2 import Util
	#step = 1
	mashi = cnx -numr[-3] -2
	nima = len(data)
	istep = int(1.0/step)
	dimage = [[[None for j in range(2*maxrangey*istep+1)] for i in range(2*maxrangex*istep+1)] for im in range(nima) ]
	for im in range(nima):
		sts = Util.infomask(data[im], maskfile, False)
		data[im] -= sts[0]
		data[im] /= sts[1]
		alpha, sx, sy, mirror, dummy = get_params2D(data[im])
		#alpha, sx, sy, dummy         = combine_params2(alpha, sx, sy, mirror, 0.0, -cs[0], -cs[1], 0)
		alphai, sxi, syi, dummy      = combine_params2(0.0, sx, sy, 0, -alpha, 0,0, 0)
		#  introduce constraints on parameters to accomodate use of cs centering
		sxi = min(max(sxi,-mashi),mashi)
		syi = min(max(syi,-mashi),mashi)	
		for j in range(-maxrangey*istep, maxrangey*istep+1):
			iy = j*step
			for i in range(-maxrangex*istep, maxrangex*istep+1):
				ix = i*step
				dimage[im][i+maxrangex][j+maxrangey] = Util.Polar2Dm(data[im], cnx+sxi+ix, cny+syi+iy, numr, mode)
				#print ' prepref  ',j,i,j+maxrangey,i+maxrangex
				Util.Frngs(dimage[im][i+maxrangex][j+maxrangey], numr)
		dimage[im][0][0].set_attr("sxi",sxi)
		dimage[im][0][0].set_attr("syi",syi)

	return dimage

def ormq_peaks(image, crefim, xrng, yrng, step, mode, numr, cnx, cny):
	"""
	Determine shift and rotation between image and reference image (crefim)
	crefim should be as FT of polar coords with applied weights
	consider mirror
	quadratic interpolation
	cnx, cny in FORTRAN convention
	"""
	from sp_utilities import peak_search

	ccfs = EMData()
	ccfm = EMData()
	ou = numr[-3]
	nx = image.get_xsize()
	ny = image.get_ysize()
	#  The search range procedure was adjusted for 3D searches, so since in 2D the order of operations is inverted, we have to invert ranges
	txrng = search_range(nx, ou, 0, xrng)
	txrng = [txrng[1],txrng[0]]
	tyrng = search_range(ny, ou, 0, yrng)
	tyrng = [tyrng[1],tyrng[0]]
	Util.multiref_peaks_ali2d(image, crefim, txrng, tyrng, step, mode, numr, cnx, cny, ccfs, ccfm)

	peaks = peak_search(ccfs, 1000)
	for i in range(len(peaks)):	peaks[i].append(0)

	peakm = peak_search(ccfm, 1000)
	for i in range(len(peakm)):	peakm[i].append(1)
	peaks += peakm

	return peaks


"""Multiline Comment7"""
#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7

#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
			#MULTILINEMULTILINEMULTILINE 7
			#MULTILINEMULTILINEMULTILINE 7
				#MULTILINEMULTILINEMULTILINE 7
					#MULTILINEMULTILINEMULTILINE 7
					#MULTILINEMULTILINEMULTILINE 7
				#MULTILINEMULTILINEMULTILINE 7
					#MULTILINEMULTILINEMULTILINE 7
			#MULTILINEMULTILINEMULTILINE 7
				#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7


#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
			#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
			#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
			#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7


#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7


	#MULTILINEMULTILINEMULTILINE 7


#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7

		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7

		#MULTILINEMULTILINEMULTILINE 7
		#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7
	#MULTILINEMULTILINEMULTILINE 7

	#MULTILINEMULTILINEMULTILINE 7
#MULTILINEMULTILINEMULTILINE 7


def select_k(dJe, T):
	"""
	This routine is used in simulated annealing to select a random path
	based on the weight of the each path and the temperature.
	"""
	from random import random

	K = len(dJe)

	p  = [0.0] * K
	ut = 1.0/T
	for k in range(K): p[k] = dJe[k]**ut

	sumq = float(sum(p))
	for k in range(K): p[k] /= sumq
	#print  p

	for k in range(1, K-1): p[k] += p[k-1]
	# the next line looks strange, but it assures that at least the lst element is selected
	p[K-1] = 2.0

	pb = random()
	select = 0

	while(p[select] < pb):  select += 1
	#select = 0
	return select

def sim_anneal(peaks, T, step, mode, maxrin):
	from random import random
	from math import pi, cos, sin

	peaks.sort(reverse=True)

	if T < 0.0:
		select = int(-T)
		ang = ang_n(peaks[select][1]+1, mode, maxrin)
		sx  = -peaks[select][6]*step
		sy  = -peaks[select][7]*step

		co =  cos(ang*pi/180.0)
		so = -sin(ang*pi/180.0)
		sxs = sx*co - sy*so
		sys = sx*so + sy*co

		mirror = peaks[select][8]
		peak   = peaks[select][0]/peaks[0][0]
	elif T == 0.0:
		select = 0
	
		ang = ang_n(peaks[select][1]+1, mode, maxrin)
		sx  = -peaks[select][6]*step
		sy  = -peaks[select][7]*step

		co =  cos(ang*pi/180.0)
		so = -sin(ang*pi/180.0)
		sxs = sx*co - sy*so
		sys = sx*so + sy*co

		mirror = peaks[select][8]
		peak   = peaks[select][0]/peaks[0][0]
	else:
		K = len(peaks)
		qt = peaks[0][0]
		p  = [0.0] * K
		ut = 1.0/T
		for k in range(K): p[k] = (peaks[k][0]/qt)**ut

		sumq = float(sum(p))
		cp  = [0.0] * K
		for k in range(K):
			p[k] /= sumq
			cp[k] = p[k]
		#print  p

		for k in range(1, K-1): cp[k] += cp[k-1]
		# the next line looks strange, but it assures that at least the lst element is selected
		cp[K-1] = 2.0

		pb = random()
		select = 0
		while(cp[select] < pb):  select += 1

		ang = ang_n(peaks[select][1]+1, mode, maxrin)
		sx  = -peaks[select][6]*step
		sy  = -peaks[select][7]*step

		co =  cos(ang*pi/180.0)
		so = -sin(ang*pi/180.0)
		sxs = sx*co - sy*so
		sys = sx*so + sy*co

		mirror = peaks[select][8]
		peak   = p[select]
		
	return  ang, sxs, sys, mirror, peak, select

def sim_ccf(peaks, T, step, mode, maxrin):
	from random import random
	from math import pi, cos, sin

	if T < 0.0:
		select = int(-T)
		ang = ang_n(peaks[select][1]+1, mode, maxrin)
		sx  = -peaks[select][2]*step
		sy  = -peaks[select][3]*step

		co =  cos(ang*pi/180.0)
		so = -sin(ang*pi/180.0)
		sxs = sx*co - sy*so
		sys = sx*so + sy*co

		mirror = peaks[select][4]
		peak   = peaks[select][0]/peaks[0][0]
	elif T == 0.0:
		select = 0
	
		ang = ang_n(peaks[select][1]+1, mode, maxrin)
		sx  = -peaks[select][2]*step
		sy  = -peaks[select][3]*step

		co =  cos(ang*pi/180.0)
		so = -sin(ang*pi/180.0)
		sxs = sx*co - sy*so
		sys = sx*so + sy*co

		mirror = peaks[select][4]
		peak   = peaks[select][0]/peaks[0][0]
	else:
		select = int(peaks[5])
		ang = ang_n(peaks[1]+1, mode, maxrin)
		sx  = -peaks[2]*step
		sy  = -peaks[3]*step

		co =  cos(ang*pi/180.0)
		so = -sin(ang*pi/180.0)
		sxs = sx*co - sy*so
		sys = sx*so + sy*co

		mirror = int(peaks[4])
		peak   = peaks[0]

	return  ang, sxs, sys, mirror, peak, select


def sim_anneal2(peaks, Iter, T0, F, SA_stop):
	from math import exp, pow
	from random import random

	# Determine the current temperature
	T = T0*pow(F, Iter)	

	K = len(peaks)
	p = [0.0] * K

	if T > 0.0001 and Iter < SA_stop:
	
		dJe = [0.0]*K
		for k in range(K):
			dJe[k] = peaks[k][0]/peaks[0][0]

		# q[k]
		q      = [0.0] * K
		arg    = [0.0] * K
		maxarg = 0
		for k in range(K):
			arg[k] = dJe[k] / T
			if arg[k] > maxarg: maxarg = arg[k]
		limarg = 200
		if maxarg > limarg:
			sumarg = float(sum(arg))
			for k in range(K): q[k] = exp(arg[k] * limarg / sumarg)
		else:
			for k in range(K): q[k] = exp(arg[k])

		sumq = float(sum(q))
		for k in range(K):
			p[k] = q[k] / sumq
	else:
		p[0] = 1.0
	
	return p


def sim_anneal3(peaks, peakm, peaks_major, peakm_major, Iter, T0, F, SA_stop):
	from math import pow, sin, sqrt, pi
	from random import random

	# Determine the current temperature
	T = T0*pow(F, Iter)
	max_peak = 5
	DEG_to_RAD = pi/180.0

	dim = 1

	if T > 0.001 and Iter < SA_stop:

		K = len(peaks_major)
		dJe = [0.0]*K
		for k in range(K):	dJe[k] = peaks_major[k][dim+1]
		
		select_major = select_k(dJe, T)
		
		ang_m = peaks_major[select_major][1]
		#sx_m = peaks_major[select_major][6]
		#sy_m = peaks_major[select_major][7]
		
		neighbor = []
		for i in range(len(peaks)):
			ang = peaks[i][1]
			#sx = peaks[i][6]
			#sy = peaks[i][7]		
			dist = 64*abs(sin((ang-ang_m)/2*DEG_to_RAD))#+sqrt((sx-sx_m)**2+(sy-sy_m)**2)
			neighbor.append([dist, i])
		neighbor.sort()

		dJe = [0.0]*max_peak
		for k in range(max_peak):   dJe[k] = peaks[neighbor[k][1]][4]
		select_s = neighbor[select_k(dJe, T)][1]
			
		#############################################################################################################

		K = len(peakm_major)
		dJe = [0.0]*K
		for k in range(K): 	dJe[k] = peakm_major[k][dim+1]

		select_major = select_k(dJe, T)
				
		ang_m = peakm_major[select_major][1]
		#sx_m = peakm_major[select_major][6]
		#sy_m = peakm_major[select_major][7]
		
		neighbor = []
		for i in range(len(peakm)):
			ang = peakm[i][1]
			#sx = peakm[i][6]
			#sy = peakm[i][7]		
			dist = 64*abs(sin((ang-ang_m)/2*DEG_to_RAD))#+sqrt((sx-sx_m)**2+(sy-sy_m)**2)
			neighbor.append([dist, i])
		neighbor.sort()

		dJe = [0.0]*max_peak
		for k in range(max_peak):   dJe[k] = peakm[neighbor[k][1]][4]
		select_m = neighbor[select_k(dJe, T)][1]

		ps = peaks[select_s][0]
		pm = peakm[select_m][0]
		pk = select_k([1.0, min(ps/pm, pm/ps)], T)
		
		if ps > pm and pk == 0 or ps < pm and pk == 1: use_mirror = 0
		else: use_mirror = 1
	else:
		select_s = 0
		select_m = 0
		ps = peaks[select_s][0]
		pm = peakm[select_m][0]
		if ps > pm:
			use_mirror = 0
		else:
			use_mirror = 1
	
	if use_mirror == 0:
		select = select_s	
		ang = peaks[select][1]
		sx  = peaks[select][6]
		sy  = peaks[select][7]
		mirror = 0
		peak = peaks[select][0]
	else:
		select = select_m
		ang = peakm[select][1]
		sx  = peakm[select][6]
		sy  = peakm[select][7]
		mirror = 1
		peak = peakm[select][0]
		
	return  ang, sx, sy, mirror, peak, select


def prep_vol_kb(vol, kb, npad=2):
	# prepare the volume
	volft = vol.copy()
	volft.divkbsinh(kb)
	volft = volft.norm_pad(False, npad)
	volft.do_fft_inplace()
	volft.center_origin_fft()
	volft.fft_shuffle()
	return  volft

def prepare_refrings( volft, kb, nz = -1, delta = 2.0, ref_a = "P", sym = "c1", numr = None, MPI=False, \
						phiEqpsi = "Zero", kbx = None, kby = None, initial_theta = None, \
						delta_theta = None, initial_phi = None):
	"""
		Generate quasi-evenly distributed reference projections converted to rings
		ref_a can be a list of angles, in which case it is used instead of being generated
	"""
	from sp_projection   import prep_vol, prgs
	from sp_applications import MPI_start_end
	from sp_utilities    import even_angles, getfvec
	from types        import BooleanType

	# mpi communicator can be sent by the MPI parameter
	if type(MPI) is BooleanType:
		if MPI:
			from mpi import MPI_COMM_WORLD
			mpi_comm = MPI_COMM_WORLD
	else:
		mpi_comm = MPI
		MPI = True

	mode = "F"

	from types import ListType
	if(type(ref_a) is ListType):
		# if ref_a is  list, it has to be a list of projection directions, use it
		ref_angles = ref_a
	else:
		# generate list of Eulerian angles for reference projections
		#  phi, theta, psi
		if initial_theta and initial_phi :
			ref_angles = even_angles(delta, theta1 = initial_theta, phi1 = initial_phi, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)
		else:
			if initial_theta is None:
				if(sym[:1] == "c" or sym[:1] == "d"):
					ref_angles = even_angles(delta, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)
				else:
					from sp_fundamentals import symclass
					psp = symclass(sym)
					ref_angles = psp.even_angles(delta)
					del psp
			else:
				if delta_theta is None: delta_theta = 1.0
				ref_angles = even_angles(delta, theta1 = initial_theta, theta2 = delta_theta, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)


	wr_four  = ringwe(numr, mode)
	cnx = nz//2 + 1
	cny = nz//2 + 1
	num_ref = len(ref_angles)

	if MPI:
		from mpi import mpi_comm_rank, mpi_comm_size
		myid = mpi_comm_rank( mpi_comm )
		ncpu = mpi_comm_size( mpi_comm )
	else:
		ncpu = 1
		myid = 0

	if(nz <1):  ERROR("Data size has to be given (nz)", "prepare_refrings", 1, myid)
	
	ref_start, ref_end = MPI_start_end(num_ref, ncpu, myid)

	refrings = []     # list of (image objects) reference projections in Fourier representation

	sizex = numr[len(numr)-2] + numr[len(numr)-1]-1

	for i in range(num_ref):
		prjref = EMData()
		prjref.set_size(sizex, 1, 1)
		refrings.append(prjref)

	if kbx is None:
		for i in range(ref_start, ref_end):
			prjref = prgs(volft, kb, [ref_angles[i][0], ref_angles[i][1], ref_angles[i][2], 0.0, 0.0])
			cimage = Util.Polar2Dm(prjref, cnx, cny, numr, mode)  # currently set to quadratic....
			Util.Normalize_ring(cimage, numr, 0 )
			Util.Frngs(cimage, numr)
			Util.Applyws(cimage, numr, wr_four)
			refrings[i] = cimage
	else:
		for i in range(ref_start, ref_end):
			prjref = prgs(volft, kb, [ref_angles[i][0], ref_angles[i][1], ref_angles[i][2], 0.0, 0.0], kbx, kby)
			cimage = Util.Polar2Dm(prjref, cnx, cny, numr, mode)  # currently set to quadratic....
			Util.Normalize_ring(cimage, numr, 0 )
			Util.Frngs(cimage, numr)
			Util.Applyws(cimage, numr, wr_four)
			refrings[i] = cimage

	if MPI:
		from sp_utilities import bcast_compacted_EMData_all_to_all
		bcast_compacted_EMData_all_to_all(refrings, myid, comm=mpi_comm)

	for i in range(len(ref_angles)):
		n1,n2,n3 = getfvec(ref_angles[i][0], ref_angles[i][1])
		refrings[i].set_attr_dict( {"phi":ref_angles[i][0], "theta":ref_angles[i][1], "psi":ref_angles[i][2], "n1":n1, "n2":n2, "n3":n3} )

	return refrings

def prepare_refrings_projections( volft, kb, nz = -1, delta = 2.0, ref_a = "P", sym = "c1", mode = "H", numr = None, MPI=False, \
						phiEqpsi = "Zero", initial_theta = None, delta_theta = None):
	"""
		Generate quasi-evenly distributed reference FTs projections and the halfrings of their scf's
		nz has to be provided
	"""
	from sp_projection   import prep_vol, prgs
	from sp_applications import MPI_start_end
	from sp_utilities    import even_angles, getfvec, model_circle
	from sp_fundamentals import scf, fft, fftip
	from types        import BooleanType

	# mpi communicator can be sent by the MPI parameter
	if type(MPI) is BooleanType:
		if MPI:
			from mpi import MPI_COMM_WORLD
			mpi_comm = MPI_COMM_WORLD
	else:
		mpi_comm = MPI
		MPI = True

	from types import ListType
	if(type(ref_a) is ListType):
		# if ref_a is  list, it has to be a list of projection directions, use it
		ref_angles = ref_a
	else:
		# generate list of Eulerian angles for reference projections
		#  phi, theta, psi
		if initial_theta is None:
			ref_angles = even_angles(delta, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)
		else:
			if delta_theta is None: delta_theta = 1.0
			ref_angles = even_angles(delta, theta1 = initial_theta, theta2 = delta_theta, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)
	wr_four  = ringwe(numr, mode)
	cnx = nz//2 + 1
	cny = nz//2 + 1
	num_ref = len(ref_angles)

	if MPI:
		from mpi import mpi_comm_rank, mpi_comm_size
		myid = mpi_comm_rank( mpi_comm )
		ncpu = mpi_comm_size( mpi_comm )
	else:
		ncpu = 1
		myid = 0
	
	ref_start, ref_end = MPI_start_end(num_ref, ncpu, myid)

	projections = [None]*num_ref     # list of (image objects) reference projections
	refrings    = [None]*num_ref     # list of (image objects) reference projections in Fourier/polar representation
	sizex = numr[len(numr)-2] + numr[len(numr)-1]-1
	cimage = EMData(nz,nz,1,False)  #  FT blank

	mask2D = model_circle(numr[-3], nz,nz)

	for i in range(num_ref):
		prjref = EMData()
		prjref.set_size(sizex, 1, 1)
		refrings[i] = prjref
		projections[i] = cimage.copy()

	for i in range(ref_start, ref_end):
		prjref = prgs(volft, kb, [ref_angles[i][0], ref_angles[i][1], ref_angles[i][2], 0.0, 0.0])
		
		st = Util.infomask(prjref, None, True)
		prjref -= st[0]
		st = Util.infomask(prjref, mask2D, True)
		prjref /= st[1]
		fftip( prjref )
		cimage = Util.Polar2Dm(scf(prjref), cnx, cny, numr, mode)  # currently set to quadratic....
		Util.Normalize_ring(cimage, numr, 0 )
		Util.Frngs(cimage, numr)
		Util.Applyws(cimage, numr, wr_four)
		refrings[i] = cimage
		projections[i] = prjref

	if MPI:
		from sp_utilities import bcast_compacted_EMData_all_to_all
		from sp_utilities import info
		bcast_compacted_EMData_all_to_all(projections, myid, comm=mpi_comm)
		bcast_compacted_EMData_all_to_all(refrings, myid, comm=mpi_comm)

	#dd = {'is_complex':1, 'is_fftodd':nz%2, 'is_fftpad':1}
	for i in range(num_ref):
		n1,n2,n3 = getfvec(ref_angles[i][0], ref_angles[i][1])
		refrings[i].set_attr_dict( {"phi":ref_angles[i][0], "theta":ref_angles[i][1], "psi":ref_angles[i][2], "n1":n1, "n2":n2, "n3":n3} )
		projections[i].set_attr_dict( {"phi":ref_angles[i][0], "theta":ref_angles[i][1], "psi":ref_angles[i][2], "n1":n1, "n2":n2, "n3":n3} )
		#projections[i].set_attr_dict( dd )

	return refrings, projections


def prepare_refrings2( volft, kb, nz, segmask, delta, ref_a, sym, numr, MPI=False, phiEqpsi = "Minus", kbx = None, kby = None, initial_theta = None, delta_theta = None):

	from sp_projection   import prep_vol, prgs
	from math         import sin, cos, radians
	from sp_applications import MPI_start_end
	from sp_utilities    import even_angles
	from sp_alignment	  import ringwe

	# generate list of Eulerian angles for reference projections
	#  phi, theta, psi
	mode = "F"
	ref_angles = []
	if initial_theta is None:
		#ref_angles = even_angles(delta, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)
		phiphi = 0.0
		while( phiphi < 360.0 ):
			ref_angles.append([phiphi, 90.0, 90.0])
			phiphi += delta
	else:
		if delta_theta is None: delta_theta = 1.0
		#ref_angles = even_angles(delta, theta1 = initial_theta, theta2 = delta_theta, symmetry=sym, method = ref_a, phiEqpsi = phiEqpsi)
		ththt = 90.0
		while(ththt >= initial_theta ):
			phiphi = 0.0
			while( phiphi < 360.0 ):
				ref_angles.append([phiphi, ththt, 90.0])
				if(ththt != 90.0): ref_angles.append([phiphi, 180.0 - ththt, 90.0])
				phiphi += delta
			ththt -= delta_theta
	wr_four  = ringwe(numr, mode)
	cnx = nz//2 + 1
	cny = nz//2 + 1
	num_ref = len(ref_angles)

	if MPI:
		from mpi import mpi_comm_rank, mpi_comm_size, MPI_COMM_WORLD
		myid = mpi_comm_rank( MPI_COMM_WORLD )
		ncpu = mpi_comm_size( MPI_COMM_WORLD )
	else:
		ncpu = 1
		myid = 0
	from sp_applications import MPI_start_end
	ref_start, ref_end = MPI_start_end(num_ref, ncpu, myid)

	refrings = []     # list of (image objects) reference projections in Fourier representation

	sizex = numr[len(numr)-2] + numr[len(numr)-1]-1

	for i in range(num_ref):
		prjref = EMData()
		prjref.set_size(sizex, 1, 1)
		refrings.append(prjref)

	if kbx is None:
		for i in range(ref_start, ref_end):
			prjref = prgs(volft, kb, [ref_angles[i][0], ref_angles[i][1], ref_angles[i][2], 0.0, 0.0])
			Util.mul_img(prjref, segmask )
			cimage = Util.Polar2Dm(prjref, cnx, cny, numr, mode)  # currently set to quadratic....
			Util.Normalize_ring(cimage, numr, 0 )
			Util.Frngs(cimage, numr)
			Util.Applyws(cimage, numr, wr_four)
			refrings[i] = cimage
	else:
		sxprint("do not handle this case")
		sys.exit()
	if MPI:
		from sp_utilities import bcast_EMData_to_all
		for i in range(num_ref):
			for j in range(ncpu):
				ref_start, ref_end = MPI_start_end(num_ref, ncpu, j)
				if i >= ref_start and i < ref_end: rootid = j
			bcast_EMData_to_all(refrings[i], myid, rootid)

	for i in range(num_ref):
		q0 = radians(ref_angles[i][0])
		q1 = radians(ref_angles[i][1])
		sq1 = sin(q1)
		n1 = sq1*cos(q0)
		n2 = sq1*sin(q0)
		n3 = cos(q1)
		refrings[i].set_attr_dict( {"n1":n1, "n2":n2, "n3":n3} )
		refrings[i].set_attr("phi",   ref_angles[i][0])
		refrings[i].set_attr("theta", ref_angles[i][1])
		refrings[i].set_attr("psi",   ref_angles[i][2])

	return refrings

def refprojs( volft, kb, ref_angles, cnx, cny, numr, mode, wr ):
	from sp_projection		import prgs
	from sp_utilities		import getfvec

	ref_proj_rings = []     # list of (image objects) reference projections in Fourier representation
	for i in range(len(ref_angles)):
		#prjref = project(volref, [ref_angles[i][0], ref_angles[i][1], ref_angles[i][2], 0.0, 0.0], last_ring)
		prjref = prgs(volft, kb, [ref_angles[i][0], ref_angles[i][1], ref_angles[i][2], 0.0, 0.0])
		cimage = Util.Polar2Dm(prjref, cnx, cny, numr, mode)  # currently set to quadratic....
		Util.Normalize_ring(cimage, numr, 0 )
		Util.Frngs(cimage, numr)
		Util.Applyws(cimage, numr, wr)
		ref_proj_rings.append(cimage)
		n1,n2,n3 = getfvec(ref_angles[i][0], ref_angles[i][1])
		ref_proj_rings[-1].set_attr_dict( {"phi":ref_angles[i][0], "theta":ref_angles[i][1], "psi":ref_angles[i][2], "n1":n1, "n2":n2, "n3":n3} )

	return ref_proj_rings

def proj_ali_incore(data, refrings, numr, xrng, yrng, step, finfo=None, sym = "c1", delta_psi = 0.0, rshift = 0.0):
	from sp_alignment import search_range
	from EMAN2 import Vec2f

	if finfo:
		from sp_utilities    import get_params_proj
		phi, theta, psi, s2x, s2y = get_params_proj(data)
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, s2x, s2y))
		finfo.flush()

	mode = "F"
	#  center is in SPIDER convention
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	cnx  = nx//2 + 1
	cny  = ny//2 + 1

	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	dp = t1.get_params("spider")
	ou = numr[-3]
	sxi = round(-dp["tx"]+rshift,2)
	syi = round(-dp["ty"]+rshift,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_3d(data, refrings, txrng, tyrng, step, mode, numr, cnx-sxi, cny-syi, delta_psi)
	#print ang, sxs, sys, mirror, iref, peak
	iref = int(iref)
	#  What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
	#  rotation has to be reversed
	if mirror:
		phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
		theta = 180.0-refrings[iref].get_attr("theta")
		psi   = (540.0-refrings[iref].get_attr("psi")-ang)%360.0
	else:
		phi   = refrings[iref].get_attr("phi")
		theta = refrings[iref].get_attr("theta")
		psi   = (360.0+refrings[iref].get_attr("psi")-ang)%360.0
	s2x   = sxs + sxi
	s2y   = sys + syi
	#set_params_proj(data, [phi, theta, psi, s2x, s2y])
	t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
	t2.set_trans(Vec2f(-s2x, -s2y))
	data.set_attr("xform.projection", t2)
	data.set_attr("referencenumber", iref)
	from sp_pixel_error import max_3D_pixel_error
	ts = t2.get_sym_proj(sym)
	if(len(ts) > 1):
		# only do it if it is not c1
		pixel_error = +1.0e23
		for ut in ts:
			# we do not care which position minimizes the error
			pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
	else:
		pixel_error = max_3D_pixel_error(t1, t2, numr[-3])
	

	if finfo:
		finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
		finfo.flush()

	return peak, pixel_error

def proj_ali_incore_zoom(data, refrings, numr, xrng, yrng, step, finfo=None, sym = "c1", delta_psi = 0.0):
	from sp_alignment import search_range
	from EMAN2 import Vec2f

	if finfo:
		from sp_utilities    import get_params_proj
		phi, theta, psi, s2x, s2y = get_params_proj(data)
		finfo.write("Old parameters: %7.2f  %7.2f  %7.2f  %7.2f  %7.2f\n"%(phi, theta, psi, s2x, s2y))
		finfo.flush()

	mode = "F"
	#  center is in SPIDER convention
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	ou = numr[-3]

	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	t2 = t1
	for zi in range(len(xrng)):
		dp = t2.get_params("spider")
		sxi = round(-dp["tx"],2)
		syi = round(-dp["ty"],2)
		txrng = search_range(nx, ou, sxi, xrng[zi])
		tyrng = search_range(ny, ou, syi, yrng[zi])

		[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_3d(data, refrings, txrng, tyrng, step[zi], mode, numr, cnx-sxi, cny-syi, delta_psi)
		#print ang, sxs, sys, mirror, iref, peak
		iref = int(iref)
		#  What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		if mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")-ang)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (360.0+refrings[iref].get_attr("psi")-ang)%360.0
		s2x   = sxs + sxi
		s2y   = sys + syi
		#set_params_proj(data, [phi, theta, psi, s2x, s2y])
		t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
		t2.set_trans(Vec2f(-s2x, -s2y))

	data.set_attr("xform.projection", t2)
	data.set_attr("referencenumber", iref)
	from sp_pixel_error import max_3D_pixel_error
	ts = t2.get_sym_proj(sym)
	if(len(ts) > 1):
		# only do it if it is not c1
		pixel_error = +1.0e23
		for ut in ts:
			# we do not care which position minimizes the error
			pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
	else:
		pixel_error = max_3D_pixel_error(t1, t2, numr[-3])

	if finfo:
		finfo.write( "New parameters: %7.2f  %7.2f  %7.2f  %7.2f  %7.2f  %11.3e  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
		finfo.flush()

	return peak, pixel_error

def proj_ali_incore_local(data, refrings, list_of_reference_angles, numr, xrng, yrng, step, an, finfo=None, sym='c1', delta_psi = 0.0, rshift = 0.0):
	from sp_alignment    import search_range
	#from utilities    import set_params_proj, get_params_proj
	from math         import cos, sin, pi, radians
	from EMAN2        import Vec2f

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1

	ant = cos(radians(an))
	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	dp = t1.get_params("spider")
	ou = numr[-3]
	sxi = round(-dp["tx"] + rshift, 2)
	syi = round(-dp["ty"] + rshift, 2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)
	if finfo:
		finfo.write("Old parameters: %6.2f %6.2f %6.2f %6.2f %6.2f\n"%(dp["phi"], dp["theta"], dp["psi"], -dp["tx"], -dp["ty"]))
		finfo.write("ou, nx, ny, xrng, yrng, cnx, cny, sxi, syi, txrng[0],txrng[1],tyrng[0],tyrng[1] : %3d  %3d  %3d    %4.1f  %4.1f %3d %3d   %4.1f  %4.1f     %4.1f  %4.1f %4.1f %4.1f\n"%(ou, nx, ny, xrng, yrng, cnx, cny, sxi, syi, txrng[0],txrng[1],tyrng[0],tyrng[1]))
		finfo.flush()
	
	[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_3d_local(data, refrings, list_of_reference_angles, txrng, tyrng, step, ant, mode, numr, cnx-sxi, cny-syi, sym, delta_psi)

	iref=int(iref)
	if iref > -1:
		# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		if  mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")-ang)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (360.0+refrings[iref].get_attr("psi")-ang)%360.0
		s2x   = sxs + sxi
		s2y   = sys + syi

		#set_params_proj(data, [phi, theta, psi, s2x, s2y])
		t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
		t2.set_trans(Vec2f(-s2x, -s2y))
		data.set_attr("xform.projection", t2)
		from sp_pixel_error import max_3D_pixel_error
		ts = t2.get_sym_proj(sym)
		if(len(ts) > 1):
			# only do it if it is not c1
			pixel_error = +1.0e23
			for ut in ts:
				# we do not care which position minimizes the error
				pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
		else:
			pixel_error = max_3D_pixel_error(t1, t2, numr[-3])
		#print phi, theta, psi, s2x, s2y, peak, pixel_error
		if finfo:
			from sp_utilities import get_params_proj
			phi, theta, psi, s2x, s2y = get_params_proj(data)
			finfo.write( "New parameters: %6.2f %6.2f %6.2f %6.2f %6.2f   %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
			finfo.flush()
		return peak, pixel_error
	else:
		return -1.0e23, 0.0


def proj_ali_incore_local_zoom(data, refrings, list_of_reference_angles, numr, xrng, yrng, step, an, finfo=None, sym='c1', delta_psi = 0.0):
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2
	#from utilities    import set_params_proj, get_params_proj
	from math         import cos, sin, pi, radians
	from EMAN2        import Vec2f

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	ou = numr[-3]

	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	t2 = t1
	s2x = None
	for zi in range(len(xrng)):
		ant = cos(radians(an[zi]))
		dp = t2.get_params("spider")
		if finfo and zi == 0:
			#finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, sxo, syo))
			finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(dp["phi"], dp["theta"], dp["psi"], -dp["tx"], -dp["ty"]))
			finfo.flush()

		sxi = round(-dp["tx"],2)
		syi = round(-dp["ty"],2)
		txrng = search_range(nx, ou, sxi, xrng[zi])
		tyrng = search_range(ny, ou, syi, yrng[zi])

		[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_3d_local(data, refrings, list_of_reference_angles, txrng, tyrng, step[zi], ant, mode, numr, cnx-sxi, cny-syi, sym, delta_psi)

		iref=int(iref)
		#[ang,sxs,sys,mirror,peak,numref] = apmq_local(projdata[imn], ref_proj_rings, xrng, yrng, step, ant, mode, numr, cnx-sxo, cny-syo)
		#ang = (ang+360.0)%360.0
		#print  ang, sxs, sys, mirror, iref, peak
		if iref > -1:
			# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
			# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
			if  mirror:
				phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
				theta = 180.0-refrings[iref].get_attr("theta")
				psi   = (540.0-refrings[iref].get_attr("psi")-ang)%360.0
			else:			
				phi   = refrings[iref].get_attr("phi")
				theta = refrings[iref].get_attr("theta")
				psi   = (360.0+refrings[iref].get_attr("psi")-ang)%360.0
			s2x   = sxi + sxs
			s2y   = syi + sys

			#set_params_proj(data, [phi, theta, psi, s2x, s2y])
			t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
			t2.set_trans(Vec2f(-s2x, -s2y))

	# This trick assures that transformation is only set when at least one reference is found.
	if( s2x != None ):
		data.set_attr("xform.projection", t2)
		from sp_pixel_error import max_3D_pixel_error
		ts = t2.get_sym_proj(sym)
		if(len(ts) > 1):
			# only do it if it is not c1
			pixel_error = +1.0e23
			for ut in ts:
				# we do not care which position minimizes the error
				pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
		else:
			pixel_error = max_3D_pixel_error(t1, t2, numr[-3])


		#print phi, theta, psi, s2x, s2y, peak, pixel_error
		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
			finfo.flush()
		return peak, pixel_error
	else:
		return -1.0e23, 0.0

#  This function is obsoleted
def proj_ali_incore_delta(data, refrings, numr, xrng, yrng, step, start, delta, finfo=None):
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2
	from EMAN2 import Vec2f
	ERROR("proj_ali_incore_delta","OBSOLETED",1)

	mode = "F"
	#  center is in SPIDER convention
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	cnx  = nx//2 + 1
	cny  = ny//2 + 1

	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	dp = t1.get_params("spider")
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(dp["phi"], dp["theta"], dp["psi"], -dp["tx"], -dp["ty"]))
		finfo.flush()

	ou = numr[-3]
	sxi = round(-dp["tx"],2)
	syi = round(-dp["ty"],2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	#  This function should be modified to work properly for refrings wrapping due to symmetries 01/27/2015
	[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_2d_delta(data, refrings, txrng, tyrng, step, mode, numr, cnx-sxi, cny-syi, start, delta)
	iref = int(iref)
	#[ang,sxs,sys,mirror,peak,numref] = apmq(projdata[imn], ref_proj_rings, xrng, yrng, step, mode, numr, cnx-sxo, cny-syo)
	#ang = (ang+360.0)%360.0
	# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
	#  What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
	angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
	if mirror:
		phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
		theta = 180.0-refrings[iref].get_attr("theta")
		psi   = (540.0-refrings[iref].get_attr("psi")+angb)%360.0
	else:
		phi   = refrings[iref].get_attr("phi")
		theta = refrings[iref].get_attr("theta")
		psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
	s2x   = sxb + sxi
	s2y   = syb + syi
	#set_params_proj(data, [phi, theta, psi, s2x, s2y])
	t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
	t2.set_trans(Vec2f(-s2x, -s2y))
	data.set_attr("xform.projection", t2)
	from sp_pixel_error import max_3D_pixel_error
	ts = t2.get_sym_proj(sym)
	if(len(ts) > 1):
		# only do it if it is not c1
		pixel_error = +1.0e23
		for ut in ts:
			# we do not care which position minimizes the error
			pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
	else:
		pixel_error = max_3D_pixel_error(t1, t2, numr[-3])

	if finfo:
		finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
		finfo.flush()

	return peak, pixel_error

#  This function is obsoleted
def proj_ali_incore_local_psi(data, refrings, numr, xrng, yrng, step, an, dpsi=180.0, finfo=None):
	"""
	  dpsi - how far psi can be from the original value.
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2
	#from utilities   import set_params_proj, get_params_proj
	from EMAN2 import Vec2f
	from math         import cos, sin, pi
	ERROR("proj_ali_incore_local_psi","OBSOLETED",1)
	if finfo:
		phi, theta, psi, s2x, s2y = get_params_proj(data)
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, s2x, s2y))
		finfo.flush()

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1

	ant = cos(an*pi/180.0)
	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	dp = t1.get_params("spider")
	if finfo:
		#finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, sxo, syo))
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(dp["phi"], dp["theta"], dp["psi"], -dp["tx"], -dp["ty"]))
		finfo.flush()
		
	ou = numr[-3]
	sxi = round(-dp["tx"],2)
	syi = round(-dp["ty"],2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_2d_local_psi(data, refrings, txrng, tyrng, step, ant, dpsi, mode, numr, cnx-sxi, cny-syi)
	iref = int(iref)
	#Util.multiref_peaks_ali(data[imn].process("normalize.mask", {"mask":mask2D, "no_sigma":1}), ref_proj_rings, xrng, yrng, step, mode, numr, cnx-sxo, cny-syo, ccfs, ccfm, nphi, ntheta)
	#[ang,sxs,sys,mirror,peak,numref] = apmq_local(projdata[imn], ref_proj_rings, xrng, yrng, step, ant, mode, numr, cnx-sxo, cny-syo)
	#ang = (ang+360.0)%360.0
	if iref > -1:
		# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
		# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		if  mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")+angb)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		#set_params_proj(data, [phi, theta, psi, s2x, s2y])
		t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
		t2.set_trans(Vec2f(-s2x, -s2y))
		data.set_attr("xform.projection", t2)
		from sp_pixel_error import max_3D_pixel_error
		ts = t2.get_sym_proj(sym)
		if(len(ts) > 1):
			# only do it if it is not c1
			pixel_error = +1.0e23
			for ut in ts:
				# we do not care which position minimizes the error
				pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
		else:
			pixel_error = max_3D_pixel_error(t1, t2, numr[-3])
		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
			finfo.flush()
		return peak, pixel_error
	else:
		return -1.0e23, 0.0



def ornq_gridding(image, crefim, shifts, shrink, kb, mode, numr, cnx, cny, deltapsi = 0.0):
	"""Determine shift and rotation between image and reference image (refim)
	   no mirror
	   	input image is preshifted and prepped for gridding
		gridding interpolation
		cnx, cny in FORTRAN convention
	"""
	#from math import pi, cos, sin, radians
	from sp_alignment import ang_n
	#from utilities import info
	peak = -1.0E23
	for ll,iq in enumerate(shifts):
		cimage = Util.Polar2Dmi(image[ll], cnx, cny, numr, mode, kb)
		Util.Frngs(cimage, numr)
		retvals = Util.Crosrng_e(crefim, cimage, numr, 0, deltapsi)
		qn = retvals["qn"]
		###print  qn,  (360.0-ang_n(retvals["tot"], mode, numr[-1]))%360.0, iq[0]*shrink, iq[1*shrink]
		if qn >= peak:
			sx = iq[0]*shrink
			sy = iq[1]*shrink
			ang = ang_n(retvals["tot"], mode, numr[-1])
			peak = qn
	#print  ang, sx, sy, peak
	# mirror is returned as zero for consistency
	mirror = 0
	#  For 3D work no need to invert
	"""Multiline Comment8"""
	#MULTILINEMULTILINEMULTILINE 8
	#MULTILINEMULTILINEMULTILINE 8
	#MULTILINEMULTILINEMULTILINE 8
	#MULTILINEMULTILINEMULTILINE 8
	#MULTILINEMULTILINEMULTILINE 8
	return  (360.0-ang)%360.0, sx, sy, mirror, peak


def ali3D_gridding(data, volprep, refang, delta_psi, shifts, shrink, numr, wr, cnx, myid, main_node, kb3D = None):
	from sp_projection import prgs,prgl
	from sp_alignment import ornq_gridding
	from sp_fundamentals import prep_refim_gridding
	#from utilities import wrap_mpi_gatherv
	from mpi import mpi_barrier, MPI_COMM_WORLD
	###from time import time
	#  Input data has to be CTF-multiplied, preshifted, and prepped for gridding
	###at = time()
	nang = len(refang)
	simis = [-1.0e23]*len(data)
	newpar = [None]*len(data)
	for i in range(nang):
		###if myid == main_node:  print "  Angle :",i,time()-at
		if kb3D:  temp = prgs(volprep, kb3D, [refang[i][0],refang[i][1],0.0, 0.0,0.0])
		else:     temp = prgl(volprep,[ refang[i][0],refang[i][1],0.0, 0.0,0.0], 1, True)
		crefim,kb = prep_refim_gridding(temp, wr, numr)
		for kl,emimage in enumerate(data):
			psi, sxs, sys, mirror, peak = ornq_gridding(emimage, crefim, shifts, shrink, kb, "F", numr, cnx, cnx, deltapsi = delta_psi)
			#print  "%4d     %12.3e     %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(i,peak,refang[i][0],refang[i][1],psi,sxs/shrink,sys/shrink)
			if(peak > simis[kl]):
				#best = i
				simis[kl]  = peak
				newpar[kl] = [refang[i][0],refang[i][1],psi,sxs/shrink,sys/shrink]
			
	#print  " >>>  %4d   %12.3e       %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(best,simis[0],newpar[0][0],newpar[0][1],newpar[0][2],newpar[0][3],newpar[0][4])

	###if myid == main_node:  print "  Finished :",time()-at

	#mpi_barrier(MPI_COMM_WORLD)
	#simis  = wrap_mpi_gatherv(simis, main_node, MPI_COMM_WORLD)
	#newpar = wrap_mpi_gatherv(newpar, main_node, MPI_COMM_WORLD)
	mpi_barrier(MPI_COMM_WORLD)
	return newpar,simis

"""Multiline Comment9"""
#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9

	#MULTILINEMULTILINEMULTILINE 9
		#MULTILINEMULTILINEMULTILINE 9
		#MULTILINEMULTILINEMULTILINE 9
		#MULTILINEMULTILINEMULTILINE 9
			#MULTILINEMULTILINEMULTILINE 9
			#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
				#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
		#MULTILINEMULTILINEMULTILINE 9
	#MULTILINEMULTILINEMULTILINE 9
#MULTILINEMULTILINEMULTILINE 9

def ali3D_direct(data, volprep, refang, delta_psi, shifts, myid, main_node, lentop = 1000, kb3D = None):
	from sp_projection import prgs,prgl
	from sp_fundamentals import fft
	#from utilities import wrap_mpi_gatherv
	from math import sqrt
	from mpi import mpi_barrier, MPI_COMM_WORLD
	from time import time
	#  Input data has to be CTF-multiplied, preshifted
	#  Output - newpar, see structure
	#    newpar = [[i, [worst_similarity, sum_all_similarities], [[-1, -1.0e23] for j in xrange(lentop)]] for i in xrange(len(data))]
	#    newpar = [[params],[],... len(data)]
	#    params = [particleID, [worst_similarity, sum_all_similarities],[imageallparams]]]
	#    imageallparams = [[orientation, similarity],[],...  number of all orientations ]
	#  Coding of orientations:
	#    hash = ang*100000000 + lpsi*1000 + ishift
	#    ishift = hash%1000
	#    ipsi = (hash/1000)%100000
	#    iang  = hash/100000000
	#  To get best matching for particle #kl:
	#     hash_best = newpar[kl][-1][0][0]
	#     best_sim  = newpar[kl][-1][0][1]
	#  To sort:
	from operator import itemgetter#, attrgetter, methodcaller
	#   params.sort(key=itemgetter(2))

	at = time()
	npsi = int(360./delta_psi)
	nang = len(refang)
	ndat = len(data)
	newpar = [[i, [1.0e23,0.0], [] ] for i in range(ndat)]
	#newpar = [[i, [1.0e23,1.0e23], []] for i in xrange(len(data))]
	for i in range(nang):
		#if myid == main_node:  print "  Angle :",i,time()-at
		iang = i*100000000
		for j in range(npsi):
			iangpsi = j*1000 + iang
			psi = j*delta_psi
			if kb3D:  temp = fft(prgs(volprep, kb3D, [refang[i][0],refang[i][1],psi, 0.0,0.0]))
			else:     temp = prgl(volprep,[ refang[i][0],refang[i][1],psi, 0.0,0.0], 1, False)
			temp.set_attr("is_complex",0)
			nrmref = sqrt(Util.innerproduct(temp, temp, None))
			for kl,emimage in enumerate(data):
				for im in range(len(shifts)):
					peak = Util.innerproduct(temp, emimage[im],None)
					peak /= nrmref
					#print  "%4d     %12.3e     %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(i,peak,refang[i][0],refang[i][1],psi,sxs/shrink,sys/shrink)
					newpar[kl][-1].append([im + iangpsi, peak])
					#newpar[kl][-1].sort(key=itemgetter(1),reverse=True)
					#del newpar[kl][-1][-1]
					"""Multiline Comment10"""
					#MULTILINEMULTILINEMULTILINE 10
					#MULTILINEMULTILINEMULTILINE 10
						#MULTILINEMULTILINEMULTILINE 10
							#MULTILINEMULTILINEMULTILINE 10
							#MULTILINEMULTILINEMULTILINE 10
					#MULTILINEMULTILINEMULTILINE 10
					#MULTILINEMULTILINEMULTILINE 10
					#MULTILINEMULTILINEMULTILINE 10
					newpar[kl][1][1] += peak
					#  Store the worst one
					if( peak < newpar[kl][1][0]):  newpar[kl][1][0] = peak
		for kl in range(ndat):
			newpar[kl][-1].sort(key=itemgetter(1),reverse=True)
			newpar[kl][-1] = newpar[kl][-1][:min(lentop, len(newpar[kl][-1]))]

		
	#print  " >>>  %4d   %12.3e       %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(best,simis[0],newpar[0][0],newpar[0][1],newpar[0][2],newpar[0][3],newpar[0][4])
	###if myid == main_node:  print "  Finished :",time()-at
	#mpi_barrier(MPI_COMM_WORLD)
	#simis  = wrap_mpi_gatherv(simis, main_node, MPI_COMM_WORLD)
	#newpar = wrap_mpi_gatherv(newpar, main_node, MPI_COMM_WORLD)
	for kl in range(ndat):
		newpar[kl][-1].sort(key=itemgetter(1),reverse=True)
		newpar[kl][-1] = newpar[kl][-1][:1]
	mpi_barrier(MPI_COMM_WORLD)
	return newpar


def ali3D_direct_preselect(data, volprep, oldcodedparams, refang, delta_psi, shifts, myid, main_node, lentop = 1000, kb3D = None):
	from sp_projection import prgs,prgl
	from sp_fundamentals import fft
	#from utilities import wrap_mpi_gatherv
	from math import sqrt
	from mpi import mpi_barrier, MPI_COMM_WORLD
	from time import time
	#  Input data has to be CTF-multiplied, preshifted
	#  Output - newpar, see structure, shifts are sublist of angles
	#    newpar = [[i, 1.0e23, [[j, -1.0e11,[[q,-1.0e22] for q in xrange(lshift)]] for j in xrange(lentop)]] for i in xrange(ldat)]
	#    ??? newpar = [[params],[],... len(data)]
	#    params = [particleID, worstsimilarity,[imageallparams]]]
	#    imageallparams = [[orientation, similarity],[],...  number of all orientations ]
	#  Coding of orientations:
	#    hash = ang*100000 + lpsi
	#    ipsi = hash%100000
	#    iang  = hash/100000
	#  To get best matching for particle #kl:
	#     hash_best = newpar[kl][-1][0][0]
	#     best_sim  = newpar[kl][-1][0][1]
	#  To delete  shifts:
	#for i in xrange(ldat):
    #for j in xrange(lentop):
    #    del newpar[i][2][j][-1]

	#  To extract list of valid angles:
	#	for i in xrange(ldat):
    # 		newpar[i][2] = [newpar[i][2][j][0] for j in xrange(len(newpar[i][2])) ]
    #  as a result, oldcodedparams[im][2] will contain a list of valid angles for image im


	#  To sort:
	from operator import itemgetter#, attrgetter, methodcaller
	#   params.sort(key=itemgetter(2))

	at = time()
	npsi = int(360./delta_psi)
	nang = len(refang)
	lshift = len(shifts)
	#newpar = [[i, 1.0e23, [[-1, -1.0e23] for j in xrange(lentop)]] for i in xrange(len(data))]
	newpar = [[i, 1.0e23, []] for i in range(len(data))]
	#newpar = [[i, 1.0e23, [ [j, -1.0e11, [[q,-1.0e22] for q in xrange(lshift)]] for j in xrange(lentop)] ] for i in xrange(ldat)]
	for i in range(nang):
		#if myid == main_node:  print "  Angle :",i,time()-at
		iang = i*100000
		for j in range(npsi):
			iangpsi = j + iang
			psi = j*delta_psi
			if kb3D:  temp = fft(prgs(volprep, kb3D, [refang[i][0],refang[i][1],psi, 0.0,0.0]))
			else:     temp = prgl(volprep,[ refang[i][0],refang[i][1],psi, 0.0,0.0], 1, False)
			temp.set_attr("is_complex",0)
			nrmref = sqrt(Util.innerproduct(temp, temp))
			for kl,emimage in enumerate(data):
				try:
					junk = oldcodedparams[kl][2].index(iangpsi)
					newpar[kl][2].append([iangpsi, -1.0, []])
					for im in range(lshift):
						peak = Util.innerproduct(temp, emimage[im])
						peak /= nrmref
						#print  "%4d     %12.3e     %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(i,peak,refang[i][0],refang[i][1],psi,sxs/shrink,sys/shrink)
						newpar[kl][-1][2].append([im, peak])
						#newpar[kl][-1].sort(key=itemgetter(1),reverse=True)
						#del newpar[kl][-1][-1]
						"""Multiline Comment11"""
						#MULTILINEMULTILINEMULTILINE 11
						#MULTILINEMULTILINEMULTILINE 11
							#MULTILINEMULTILINEMULTILINE 11
								#MULTILINEMULTILINEMULTILINE 11
								#MULTILINEMULTILINEMULTILINE 11
						#MULTILINEMULTILINEMULTILINE 11
						#MULTILINEMULTILINEMULTILINE 11
						#MULTILINEMULTILINEMULTILINE 11
						#  Store the worst one
						if( peak < newpar[kl][1]):  newpar[kl][1] = peak

					#  Store sum of shift scores
					newpar[kl][2][-1][1] = sum([newpar[kl][2][-1][2][im][1] for im in range(lshift)])
				except:
					#  No valid angles
					pass
					
		for kl in range(len(data)):
			newpar[kl][2].sort(key=itemgetter(1),reverse=True)
			newpar[kl][2] = newpar[kl][2][:min(lentop, len(newpar[kl][2]))]

		
	#print  " >>>  %4d   %12.3e       %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(best,simis[0],newpar[0][0],newpar[0][1],newpar[0][2],newpar[0][3],newpar[0][4])
	###if myid == main_node:  print "  Finished :",time()-at
	#mpi_barrier(MPI_COMM_WORLD)
	#simis  = wrap_mpi_gatherv(simis, main_node, MPI_COMM_WORLD)
	#newpar = wrap_mpi_gatherv(newpar, main_node, MPI_COMM_WORLD)
	mpi_barrier(MPI_COMM_WORLD)
	return newpar



def ali3D_direct_local(data, volprep, refang, delta_psi, shifts, an, oldangs, myid, main_node, lentop = 1000, kb3D = None):
	from sp_projection import prgs,prgl
	from sp_fundamentals import fft
	from sp_utilities import getfvec
	from math import sqrt
	from mpi import mpi_barrier, MPI_COMM_WORLD
	from time import time
	#  Input data has to be CTF-multiplied, preshifted
	#  Output - newpar, see structure
	#    newpar = [[i, [worst_similarity, sum_all_similarities]], [[-1, -1.0e23] for j in xrange(lentop)]] for i in xrange(len(data))]
	#    newpar = [[params],[],... len(data)]
	#    params = [particleID, [worst_similarity, sum_all_similarities],[imageallparams]]]
	#    imageallparams = [[orientation, similarity],[],...  number of all orientations ]
	#  Coding of orientations:
	#    hash = ang*100000000 + lpsi*1000 + ishift
	#    ishift = hash%1000
	#    ipsi = (hash/1000)%100000
	#    iang  = hash/100000000
	#  To get best matching for particle #kl:
	#     hash_best = newpar[kl][-1][0][0]
	#     best_sim  = newpar[kl][-1][0][1]
	#  To sort:
	from operator import itemgetter#, attrgetter, methodcaller
	#   params.sort(key=itemgetter(2))

	from math import cos, radians
	ac = cos(radians(an))
	dvec = [None]*len(data)
	for kl in range(len(data)):
		dvec[kl] = getfvec(oldangs[kl][0], oldangs[kl][1])
	

	at = time()
	npsi = int(360./delta_psi)
	nang = len(refang)
	newpar = [[i, [1.0e23,0.0], [[-1, -1.0e23] for j in range(lentop)]] for i in range(len(data))]
	#newpar = [[i, 1.0e23, []] for i in xrange(len(data))]
	for i in range(nang):
		#if myid == main_node:  print "  Angle :",i,time()-at
		iang = i*100000000
		rdir = getfvec(refang[i][0],refang[i][1])
		for j in range(npsi):
			iangpsi = j*1000 + iang
			psi = j*delta_psi
			if kb3D:  temp = fft(prgs(volprep, kb3D, [refang[i][0],refang[i][1],psi, 0.0,0.0]))
			else:     temp = prgl(volprep,[ refang[i][0],refang[i][1],psi, 0.0,0.0], 1, False)
			temp.set_attr("is_complex",0)
			nrmref = sqrt(Util.innerproduct(temp, temp))
			for kl,emimage in enumerate(data):
				#  Check psi first which is faster
				qt = (oldangs[kl][2]-psi)%360.0
				if( qt > 180.0 ): qt = 360. - qt
				if(qt < an):
					if( (dvec[kl][0]*rdir[0] + dvec[kl][1]*rdir[1] + dvec[kl][2]*rdir[2]) >= ac ):
						for im in range(len(shifts)):
							peak = Util.innerproduct(temp, emimage[im])
							peak /= nrmref
							#print  "%4d     %12.3e     %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(i,peak,refang[i][0],refang[i][1],psi,sxs/shrink,sys/shrink)
							newpar[kl][-1].append([im + iangpsi, peak])
							#newpar[kl][-1].sort(key=itemgetter(1),reverse=True)
							#del newpar[kl][-1][-1]
							"""Multiline Comment12"""
							#MULTILINEMULTILINEMULTILINE 12
							#MULTILINEMULTILINEMULTILINE 12
								#MULTILINEMULTILINEMULTILINE 12
									#MULTILINEMULTILINEMULTILINE 12
									#MULTILINEMULTILINEMULTILINE 12
							#MULTILINEMULTILINEMULTILINE 12
							#MULTILINEMULTILINEMULTILINE 12
							#MULTILINEMULTILINEMULTILINE 12
							#  Store the worst one
							newpar[kl][1][1] += peak
							if( peak < newpar[kl][1][0]):  newpar[kl][1][0] = peak
		for kl in range(len(data)):
			newpar[kl][-1].sort(key=itemgetter(1),reverse=True)
			newpar[kl][-1] = newpar[kl][-1][:min(lentop, len(newpar[kl][-1]))]

		
	#print  " >>>  %4d   %12.3e       %12.5f     %12.5f     %12.5f     %12.5f     %12.5f"%(best,simis[0],newpar[0][0],newpar[0][1],newpar[0][2],newpar[0][3],newpar[0][4])
	###if myid == main_node:  print "  Finished :",time()-at
	#mpi_barrier(MPI_COMM_WORLD)
	#simis  = wrap_mpi_gatherv(simis, main_node, MPI_COMM_WORLD)
	#newpar = wrap_mpi_gatherv(newpar, main_node, MPI_COMM_WORLD)
	mpi_barrier(MPI_COMM_WORLD)
	return newpar

def proj_ali_incore_direct(data, ref_angs, numr, xrng, yrng, step, finfo=None, sym = "c1", delta_psi = 0.0, rshift = 0.0):
	from sp_alignment import search_range
	from EMAN2 import Vec2f

	if finfo:
		from sp_utilities import get_params_proj
		phi, theta, psi, s2x, s2y = get_params_proj(data)
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, s2x, s2y))
		finfo.flush()

	mode = "F"
	#  center is in SPIDER convention
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	cnx  = nx//2 + 1
	cny  = ny//2 + 1

	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	dp = t1.get_params("spider")
	ou = numr[-3]
	sxi = round(-dp["tx"]+rshift,2)
	syi = round(-dp["ty"]+rshift,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	[ang, sxs, sys, mirror, iref, peak] = Util.multiref_polar_ali_3d(data, refrings, txrng, tyrng, step, mode, numr, cnx-sxi, cny-syi, delta_psi)
	#print ang, sxs, sys, mirror, iref, peak
	iref = int(iref)
	#  What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
	#  rotation has to be reversed
	if mirror:
		phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
		theta = 180.0-refrings[iref].get_attr("theta")
		psi   = (540.0-refrings[iref].get_attr("psi")-ang)%360.0
	else:
		phi   = refrings[iref].get_attr("phi")
		theta = refrings[iref].get_attr("theta")
		psi   = (360.0+refrings[iref].get_attr("psi")-ang)%360.0
	s2x   = sxs + sxi
	s2y   = sys + syi
	#set_params_proj(data, [phi, theta, psi, s2x, s2y])
	t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
	t2.set_trans(Vec2f(-s2x, -s2y))
	data.set_attr("xform.projection", t2)
	data.set_attr("referencenumber", iref)
	from sp_pixel_error import max_3D_pixel_error
	ts = t2.get_sym_proj(sym)
	if(len(ts) > 1):
		# only do it if it is not c1
		pixel_error = +1.0e23
		for ut in ts:
			# we do not care which position minimizes the error
			pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
	else:
		pixel_error = max_3D_pixel_error(t1, t2, numr[-3])
	

	if finfo:
		finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
		finfo.flush()

	return peak, pixel_error



def proj_ali_helical(data, refrings, numr, xrng, yrng, stepx, ynumber, psi_max=180.0, finfo=None):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2, get_params_proj
	from math         import cos, sin, pi

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	phi, theta, psi, sxi, syi = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()

	ou = numr[-3]
	sxi = round(sxi,2)
	syi = round(syi,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	[ang, sxs, sys, mirror, iref, peak] = \
		Util.multiref_polar_ali_helical(data, refrings, txrng, tyrng, stepx, psi_max, mode, numr, cnx-sxi, cny-syi, int(ynumber))
	iref = int(iref)
	#print  " IN ", ang, sxs, sys, mirror, iref, peak
	if iref > -1:
		# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
		# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		if  mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")+angb)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f\n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()
		return peak, phi, theta, psi, s2x, s2y
	else:
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0

def proj_ali_helical_local(data, refrings, numr, xrng, yrng, stepx,ynumber, an, psi_max=180.0, finfo=None, yrnglocal=-1.0):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2, get_params_proj
	from math         import cos, sin, radians

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	ant = cos(radians(an))
	phi, theta, psi, sxi, syi = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()
	
	ou = numr[-3]
	sxi = round(sxi,2)
	syi = round(syi,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	[ang, sxs, sys, mirror, iref, peak] = \
		Util.multiref_polar_ali_helical_local(data, refrings, txrng, tyrng, stepx, ant, psi_max, mode, numr, cnx-sxi, cny-syi, int(ynumber), yrnglocal)

	iref = int(iref)

	if iref > -1:
		# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
		# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		if  mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")+angb)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		if finfo:
			finfo.write("ref phi: %9.4f\n"%(refrings[iref].get_attr("phi")))
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f \n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()

		return peak, phi, theta, psi, s2x, s2y
	else:
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0\

def proj_ali_helical_90(data, refrings, numr, xrng, yrng, stepx, ynumber, psi_max=180.0, finfo=None):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2, get_params_proj

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	phi, theta, psi, sxi, syi = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()

	ou = numr[-3]
	sxi = round(sxi,2)
	syi = round(syi,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)
	
	[ang, sxs, sys, mirror, iref, peak] = \
		Util.multiref_polar_ali_helical_90(data, refrings, txrng, tyrng, stepx, psi_max, mode, numr, cnx-sxi, cny-syi, int(ynumber))
	iref = int(iref)
	#print  " IN ", ang, sxs, sys, mirror, iref, peak
	if iref > -1:
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		phi   = refrings[iref].get_attr("phi")
		theta = refrings[iref].get_attr("theta")
		psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f\n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()
		return peak, phi, theta, psi, s2x, s2y
	else:
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0

def proj_ali_helical_90_local(data, refrings, numr, xrng, yrng, stepx, ynumber, an, psi_max=180.0, finfo=None, yrnglocal=-1.0):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2, get_params_proj
	from math         import cos, sin, radians

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	ant = cos(radians(an))
	phi, theta, psi, sxi, syi = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()

	ou = numr[-3]
	sxi = round(sxi,2)
	syi = round(syi,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)
	
	[ang, sxs, sys, mirror, iref, peak] = \
		Util.multiref_polar_ali_helical_90_local(data, refrings, txrng, tyrng, stepx, ant, psi_max, mode, numr, cnx-sxi, cny-syi, int(ynumber), yrnglocal)
	iref = int(iref)
	if iref > -1:
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		phi   = refrings[iref].get_attr("phi")
		theta = refrings[iref].get_attr("theta")
		psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f\n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()
		return peak, phi, theta, psi, s2x, s2y
	else:
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0

#  HELICON functions
def proj_ali_helicon_local(data, refrings, numr, xrng, yrng, stepx,ynumber, an, psi_max=180.0, finfo=None, yrnglocal=-1.0):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2, get_params_proj
	from math         import cos, sin, radians

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	ant = cos(radians(an))
	phi, theta, psi, sxi, syi = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()

	ou = numr[-3]
	sxi = round(sxi,2)
	syi = round(syi,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)
	
	[ang, sxs, sys, mirror, iref, peak] = \
		Util.multiref_polar_ali_helicon_local(data, refrings, txrng, tyrng, stepx, ant, psi_max, mode, numr, cnx-sxi, cny-syi, int(ynumber), yrnglocal)

	iref = int(iref)

	if iref > -1:
		# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
		# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		if  mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")+angb)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		if finfo:
			finfo.write("ref phi: %9.4f\n"%(refrings[iref].get_attr("phi")))
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f \n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()

		return peak, phi, theta, psi, s2x, s2y
	else:
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0\

def proj_ali_helicon_90_local_direct(data, refrings, xrng, yrng, \
		an, psi_max=180.0, psi_step=1.0, stepx = 1.0, stepy = 1.0, finfo=None, yrnglocal=-1.0):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_utilities    import compose_transform2, get_params_proj
	from sp_alignment    import directaligridding
	from math         import cos, sin, radians

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	#cnx  = nx//2 + 1
	#cny  = ny//2 + 1
	ant = cos(radians(an))
	phi, theta, psi, tx, ty = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()
	#  Determine whether segment is up and down and search for psi in one orientation only.
	if psi < 180.0 :  direction = "up"
	else:             direction = "down"
	peak = -1.0e23
	iref = -1
	imn1 = sin(radians(theta))*cos(radians(phi))
	imn2 = sin(radians(theta))*sin(radians(phi))
	imn3 = cos(radians(theta))
	sxprint('  aaaaaa  ',psi_max, psi_step, xrng, yrng, direction)
	for i in range(len(refrings)):
		if( (refrings[i][0].get_attr("n1")*imn1 + refrings[i][0].get_attr("n2")*imn2 + refrings[i][0].get_attr("n3")*imn3)>=ant ):
			sxprint(" Matching refring  ",i,phi, theta, psi, tx, ty)
			#  directali will do fft of the input image and 180 degs rotation, if necessary.  Eventually, this would have to be pulled up.
			a, tx,ty, tp = directaligridding(data, refrings[i], psi_max, psi_step, xrng, yrng, stepx, stepy, direction)
			if(tp>peak):
				peak = tp
				iref = i
				angb = a
				sxb = tx
				syb = ty
	"""Multiline Comment13"""
	#MULTILINEMULTILINEMULTILINE 13
		#MULTILINEMULTILINEMULTILINE 13
	#MULTILINEMULTILINEMULTILINE 13
	if iref > -1:
		#angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		phi   = refrings[iref][0].get_attr("phi")
		theta = refrings[iref][0].get_attr("theta")
		psi   = (refrings[iref][0].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb #+ tx
		s2y   = syb #+ ty
		sxprint("New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f" %(phi, theta, psi, s2x, s2y, peak))
		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f\n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()
		return peak, phi, theta, psi, s2x, s2y
	else:
		sxprint("  NO PEAK")
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0

def proj_ali_helicon_90_local_direct1(data, refrings, xrng, yrng, \
		psi_max=180.0, psi_step=1.0, stepx = 1.0, stepy = 1.0, finfo=None, yrnglocal=-1.0, direction = "both"):
	"""
	  psi_max - how much psi can differ from either 90 or 270 degrees
	"""
	from sp_utilities    import inverse_transform2, get_params_proj
	from sp_alignment    import directaligridding1
	from math         import cos, sin, radians
	
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	#cnx  = nx//2 + 1
	#cny  = ny//2 + 1

	phi, theta, psi, tx, ty = get_params_proj(data)

	#  directali will do fft of the input image and 180 degs rotation, if necessary.  Eventually, this would have to be pulled up.
	angb, tx,ty, tp = directaligridding1(data, kb, refrings, psi_max, psi_step, xrng, yrng, stepx, stepy, direction)

	if tp > -1.0e23:
		#angb, sxb, syb, ct = inverse_transform2(ang, sxs, sys, 0)
		phi   = refrings[iref][0].get_attr("phi")
		theta = refrings[iref][0].get_attr("theta")
		psi   = (refrings[iref][0].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb #+ tx
		s2y   = syb #+ ty
		return peak, phi, theta, psi, s2x, s2y
	else:
		sxprint("  NO PEAK")
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0

def proj_ali_helicon_90_local(data, refrings, numr, xrng, yrng, stepx, ynumber, an, psi_max=180.0, finfo=None, yrnglocal=-1.0):
	"""
	  psi_max - how much psi can differ from 90 or 270 degrees
	"""
	from sp_alignment import search_range
	from sp_utilities    import compose_transform2, get_params_proj
	from math         import cos, sin, pi

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1
	ant = cos(an*pi/180.0)
	phi, theta, psi, sxi, syi = get_params_proj(data)
	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(phi, theta, psi, tx, ty))
		finfo.flush()

	ou = numr[-3]
	sxi = round(sxi,2)
	syi = round(syi,2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)
	
	[ang, sxs, sys, mirror, iref, peak] = \
		Util.multiref_polar_ali_helicon_90_local(data, refrings, txrng, tyrng, stepx, ant, psi_max, mode, numr, cnx-sxi, cny-syi, int(ynumber), yrnglocal)
	iref = int(iref)
	if iref > -1:
		angb, sxb, syb, ct = compose_transform2(0.0, sxs, sys, 1, -ang, 0.0, 0.0, 1)
		phi   = refrings[iref].get_attr("phi")
		theta = refrings[iref].get_attr("theta")
		psi   = (refrings[iref].get_attr("psi")+angb+360.0)%360.0
		s2x   = sxb + sxi
		s2y   = syb + syi

		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f\n\n" %(phi, theta, psi, s2x, s2y, peak))
			finfo.flush()
		return peak, phi, theta, psi, s2x, s2y
	else:
		return -1.0e23, 0.0, 0.0, 0.0, 0.0, 0.0

def ali_vol_func(params, data):
	from sp_utilities    import model_gauss
	from sp_fundamentals import rot_shift3D, cyclic_shift
	from sp_morphology   import binarize
	#print  params
	#print  data[3]
	#cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][6], params[0], params[1], params[2],params[3], params[4], params[5],1.0)
	#print  cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale
	x = rot_shift3D(data[0], params[0], params[1], params[2], params[3], params[4], params[5], 1.0)

	res = -x.cmp("ccc", data[1], {"mask":data[2]})
	#print  " %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f  %10.5f" %(params[0], params[1], params[2],params[3], params[4], params[5], -res)
	return res

def ali_vol_func_julio(params, data):
	from sp_utilities    import model_gauss
	from sp_fundamentals import rot_shift3D, cyclic_shift
	from sp_morphology   import binarize
	#print  params
	#print  data[3]
	#cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][6], params[0], params[1], params[2],params[3], params[4], params[5],1.0)
	#print  cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale
	x = rot_shift3D(data[0], params[0], params[1], params[2], params[3], params[4], params[5], 1.0)

	if (data[3] == None):
		mask = data[2]
	elif (data[3] > 0.0):
		mask = binarize(x, data[3])
	else:
		mask = cyclic_shift(data[2], int(round(params[3],0)), int(round(params[4],0)), int(round(params[5],0)))

	if (data[5] > 1):
		from EMAN2 import rsconvolution
		gker = model_gauss(1, 7, 7, 7)
		x = rsconvolution(x, gker)
		x = Util.decimate(x, data[5], data[5], data[5])
		mask = Util.decimate(mask, data[5], data[5], data[5])

	#res = -x.cmp("ccc", data[1], {"mask":data[2]})
	res = -x.cmp(data[4], data[1], {"mask":mask, "normalize":0})
	#print  " %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f  %10.5f" %(params[0], params[1], params[2],params[3], params[4], params[5], -res)
	return res

def ali_vol_func_grid(params, data):
	from sp_fundamentals import rot_shift3D_grid, cyclic_shift
	from sp_morphology   import binarize

	# data[0]: image output from prepi3D (segment)
	# data[5]: kb from prepi3D
	# data[2], data[3]: mask-related info
	# data[4]: similarity measure
	# data[1]: target volume, into which data[0] is being fitted
	# data[6]: wraparound option

	# params are assumed to be in the "xyz" convention, so get "spider" ones to do the rot:
	tr = Transform({"type":"xyz","xtilt":params[0],"ytilt":params[1],"ztilt":params[2], "tx":params[3], "ty":params[4], "tz":params[5]})
	qt = tr.get_params("spider")

	x = rot_shift3D_grid(data[0], qt['phi'], qt['theta'], qt['psi'], qt['tx'], qt['ty'], qt['tz'], 1.0, data[5], "background", data[6])

	if (data[3] == None):
		mask = data[2]
	elif (data[3] > 0.0):
		mask = binarize(x, data[3])
	else:
		mask = cyclic_shift(data[2], int(round(params[3],0)), int(round(params[4],0)), int(round(params[5],0)))

	res = -x.cmp(data[4], data[1], {"mask":mask, "normalize":0})
	return res

def ali_vol_func_nopsi(params, data):
	from sp_utilities    import compose_transform3
	from sp_fundamentals import rot_shift3D
	#print  params
	#print  data[3]
	#cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][6], params[0], params[1], params[2],params[3], params[4], params[5],1.0)
	#print  cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale
	x = rot_shift3D(data[0], params[0], params[1], 0.0, params[2], params[3], params[4], 1.0)
	#res = -x.cmp("ccc", data[1], {"mask":data[2]})
	res = -x.cmp(data[4], data[1], {"mask":data[2]})
	#print  " %9.3f %9.3f %9.3f %9.3f %9.3f  %10.5f" %(params[0], params[1], params[2],params[3], params[4], -res)
	return res

def ali_vol_func_rotate(params, data):
	from sp_utilities    import compose_transform3
	from sp_fundamentals import rot_shift3D
	cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][7], params[0], params[1], params[2],0.0,0.0,0.0,1.0)
	x = rot_shift3D(data[0], cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale)
	res = -x.cmp(data[4], data[1], {"mask":data[2]})
	#print  " %9.3f %9.3f %9.3f  %12.3e" %(params[0], params[1], params[2], -res)
	return res

def ali_vol_func_shift(params, data):
	from sp_utilities    import compose_transform3
	from sp_fundamentals import rot_shift3D
	cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][7], 0.0,0.0,0.0, params[0], params[1], params[2],1.0)
	x = rot_shift3D(data[0], cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale)
	res = -x.cmp(data[4], data[1], {"mask":data[2]})
	#print  " %9.3f %9.3f %9.3f  %12.3e" %(params[0], params[1], params[2], -res)
	return res

def ali_vol_func_scale(params, data):
	from sp_utilities    import compose_transform3
	from sp_fundamentals import rot_shift3D
	cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][7], params[0], params[1], params[2], params[3], params[4], params[5], params[6])
	x = rot_shift3D(data[0], cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale)
	res = -x.cmp(data[4], data[1], {"mask":data[2]})
	#print  " %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f  %12.3e" %(params[0], params[1], params[2],params[3], params[4], params[5], params[6], -res)
	return res

def ali_vol_func_only_scale(params, data):
	from sp_utilities    import compose_transform3
	from sp_fundamentals import rot_shift3D
	cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale= compose_transform3(data[3][0], data[3][1], data[3][2], data[3][3], data[3][4], data[3][5], data[3][7], 0.0,0.0,0.0,0.0,0.0,0.0, params[0])
	x = rot_shift3D(data[0], cphi, ctheta, cpsi, cs2x, cs2y, cs2z, cscale)
	res = -x.cmp(data[4], data[1], {"mask":data[2]})
	#print  " %9.3f  %12.3e" %(params[0], -res)
	return res

def helios_func(params, data):
	sm = data[0].helicise(data[2], params[0], params[1], data[3], data[4], data[5])
	#try other sim creteria
	q = sm.cmp("dot", sm, {"negative":0})
	#q = sm.cmp("dot", data[0], {"negative":0})# corelation  with the recon data
	#print  params,q
	return  q

def helios(vol, pixel_size, dp, dphi, section_use = 0.75, radius = 0.0, rmin = 0.0):
	from sp_alignment    import helios_func
	from sp_utilities    import amoeba
	nx = vol.get_xsize()
	ny = vol.get_ysize()
	nz = vol.get_zsize()
	if(radius <= 0.0):    radius = nx//2-1
	params = [dp, dphi]
	#print  " input params ",params
	data=[vol, params, pixel_size, section_use, radius, rmin]
	new_params = [dp, dphi]
	new_params = amoeba(new_params, [0.05*dp, 0.05*abs(dphi)], helios_func, 1.0e-2, 1.0e-2, 50, data)
	#print  " new params ", new_params[0], new_params[1]
	return  vol.helicise(pixel_size, new_params[0][0], new_params[0][1], section_use, radius), new_params[0][0], new_params[0][1]

def helios7(vol, pixel_size, dp, dphi, section_use = 0.75, radius = 0.0, rmin = 0.0):
	from sp_alignment    import helios_func
	nx = vol.get_xsize()
	ny = vol.get_ysize()
	nz = vol.get_zsize()
	if(radius <= 0.0):    radius = nx//2-1
	params = [dp, dphi]
	data=[vol, params, pixel_size, section_use, radius, rmin]
	q = helios_func([dp, dphi], data)
	return q

def sub_favj(ave, data, jtot, mirror, numr):
	'''
		Subtract FT of rings from the average
	'''
	from math import pi,sin,cos
	#from utilities  import print_col
	# trig functions in radians
	pi2 = pi*2
	nring = len(numr)/3
	maxrin = numr[len(numr)-1]
	#print  "update",psi
	#print_col(ave)
	#print_col(data)
	#print numr
	if(mirror):
		# for mirrored data has to be conjugated
		for i in range(nring):
			numr3i = numr[2+i*3]
			np = numr[1+i*3]-1
			ave[np]   -= data[np]
			ave[np+1] -= data[np+1]*cos(pi2*(jtot-1)/2.0*numr3i/maxrin)
			for j in range(2, numr3i, 2):
				arg = pi2*(jtot-1)*int(j/2)/maxrin
				cs = complex(data[np + j],data[np + j +1])*complex(cos(arg),sin(arg))
				ave[np + j]    -= cs.real
				ave[np + j +1] += cs.imag
	else:
		for i in range(nring):
			numr3i = numr[2+i*3]
			np = numr[1+i*3]-1
			ave[np]   -= data[np]
			ave[np+1] -= data[np+1]*cos(pi2*(jtot-1)/2.0*numr3i/maxrin)
			for j in range(2, numr3i, 2):
				arg = pi2*(jtot-1)*int(j/2)/maxrin
				cs = complex(data[np + j],data[np + j +1])*complex(cos(arg),sin(arg))
				ave[np + j]    -= cs.real
				ave[np + j +1] -= cs.imag
	#print_col(ave)

def update_favj(ave, data, jtot, mirror, numr):
	'''
		Add FT of rings to the average
	'''
	from math import pi,sin,cos
	#from utilities  import print_col
	# trig functions in radians
	pi2 = pi*2
	nring = len(numr)/3
	maxrin = numr[len(numr)-1]
	#print  "update",psi
	#print_col(ave)
	#print_col(data)
	#print numr
	if(mirror):
		# for mirrored data has to be conjugated
		for i in range(nring):
			numr3i = numr[2+i*3]
			np = numr[1+i*3]-1
			ave[np]   += data[np]
			ave[np+1] += data[np+1]*cos(pi2*(jtot-1)/2.0*numr3i/maxrin)
			for j in range(2, numr3i, 2):
				arg = pi2*(jtot-1)*int(j/2)/maxrin
				cs = complex(data[np + j],data[np + j +1])*complex(cos(arg),sin(arg))
				ave[np + j]    += cs.real
				ave[np + j +1] -= cs.imag
	else:
		for i in range(nring):
			numr3i = numr[2+i*3]
			np = numr[1+i*3]-1
			ave[np]   += data[np]
			ave[np+1] += data[np+1]*cos(pi2*(jtot-1)/2.0*numr3i/maxrin)
			for j in range(2, numr3i, 2):
				arg = pi2*(jtot-1)*int(j/2)/maxrin
				cs = complex(data[np + j],data[np + j +1])*complex(cos(arg),sin(arg))
				ave[np + j]    += cs.real
				ave[np + j +1] += cs.imag
	#print_col(ave)

def fine_2D_refinement(data, br, mask, tavg, group = -1):
	from sp_utilities import amoeba
	from sp_fundamentals 	import rtshgkb, prepg

	# IMAGES ARE SQUARES!
	nx = data[0].get_xsize()
	#  center is in SPIDER convention
	cnx = int(nx/2)+1
	cny = cnx

	if(group > -1):
		nima = 0
		for im in range(len(data)):
			if(data[im].get_attr('ref_num') == group):  nima += 1
	else:  nima = len(data)

	# prepare KB interpolants
	kb = kbt(nx)
	# load stuff for amoeba
	stuff = []
	stuff.insert(0, kb)
	stuff.insert(1, mask)
	stuff.insert(2, nima)
	#stuff.insert(3,tave)  # current average
	#stuff.insert(4,data)  # current image in the gridding format
	weights = [br]*3 # weights define initial bracketing, so one would have to figure how to set them correctly

	for im in range(len(data)):
		if(group > -1):
			if(data[im].get_attr('ref_num') != group):  continue
		# subtract current image from the average
		alpha  = data[im].get_attr('alpha')
		sx     = data[im].get_attr('sx')
		sy     = data[im].get_attr('sy')
		mirror = data[im].get_attr('mirror')
		ddata  = prepg(data[im], kb)
		ddata.set_attr_dict({'alpha': alpha, 'sx':sx, 'sy':sy, 'mirror': mirror})
		temp   = rtshgkb(ddata, alpha, sx, sy, kb)
		if  mirror: temp.process_inplace("xform.mirror", {"axis":'x'})
		#  Subtract current image from the average
		refim = Util.madn_scalar(tavg, temp, -1.0/float(nima)) 
		stuff.append(refim)  # curent ave-1
		stuff.append(ddata)  # curent image
		# perform amoeba alignment
		params = [alpha, sx, sy]
		outparams =  amoeba(params, weights, crit2d, 1.e-4, 1.e-4, 500, stuff)
		del stuff[3]
		del stuff[3]
		# set parameters to the header
		data[im].set_attr_dict({'alpha':outparams[0][0], 'sx':outparams[0][1], 'sy':outparams[0][2],'mirror': mirror})
		# update the average
		temp = rtshgkb(ddata, outparams[0][0], outparams[0][1], outparams[0][2], kb)
		if  mirror: temp.process_inplace("xform.mirror",{"axis":'x'})
		#check whether the criterion actually increased
		# add current image to the average
		tavg = Util.madn_scalar(refim, temp, 1.0/float(nima))
		#print  im,tave.cmp("dot", tave, {"negative":0,"mask":mask}),params,outparams[0],outparams[2]
		#tave,tvar = ave_var_series_g(data,kb)
		#print  " Criterium on the fly ", tave.cmp("dot", tave, {"negative":0,"mask":mask})


def align2d(image, refim, xrng=[0, 0], yrng=[0, 0], step=1.0, first_ring=1, last_ring=0, rstep=1, mode = "F"):
	"""  Determine shift and rotation between image and reference image
	     quadratic interpolation
	     xrng[k,m] - translation search will be performed in range [-k,m] in steps equal step, which can non-integer
	     Output: ang, sxs, sys, mirror, peak
	"""
	#from utilities import print_col
	from sp_alignment import Numrinit, ringwe
	step = float(step)
	nx = refim.get_xsize()
	ny = refim.get_ysize()
	if(last_ring == 0):  last_ring = nx/2-2-int(max(max(xrng),max(yrng)))
	# center in SPIDER convention
	cnx = nx//2+1
	cny = ny//2+1
	#precalculate rings
	numr = Numrinit(first_ring, last_ring, rstep, mode)
	wr   = ringwe(numr, mode)
	#cimage=Util.Polar2Dmi(refim, cnx, cny, numr, mode, kb)
	crefim = Util.Polar2Dm(refim, cnx, cny, numr, mode)
	#crefim = Util.Polar2D(refim, numr, mode)
	#print_col(crefim)
	Util.Frngs(crefim, numr)
	Util.Applyws(crefim, numr, wr)
	return ormq(image, crefim, xrng, yrng, step, mode, numr, cnx, cny)
	
"""Multiline Comment14"""
#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	     #MULTILINEMULTILINEMULTILINE 14
	     #MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
		#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
		#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
	#MULTILINEMULTILINEMULTILINE 14
#MULTILINEMULTILINEMULTILINE 14

def align2d_scf(image, refim, xrng=-1, yrng=-1, ou = -1):
	from sp_fundamentals import scf, rot_shift2D, ccf, mirror
	from sp_fundamentals import fft
	from sp_utilities import peak_search
	from math import radians, sin, cos
	nx = image.get_xsize()
	ny = image.get_xsize()
	if(ou<0):  ou = min(nx//2-1,ny//2-1)
	if(yrng < 0):  yrng = xrng
	if(ou<2):
		ERROR('Radius of the object (ou) has to be given','align2d_scf',1)
	sci = scf(image)
	scr = scf(refim)
	first_ring = 1

	#alpha1, sxs, sys, mirr, peak1 = align2d_no_mirror(scf(image), scr, last_ring=ou, mode="H")
	#alpha2, sxs, sys, mirr, peak2 = align2d_no_mirror(scf(mirror(image)), scr, last_ring=ou, mode="H")
	#alpha1, sxs, sys, mirr, peak1 = align2d_no_mirror(sci, scr, first_ring = 1, last_ring=ou, mode="H")
	#alpha2, sxs, sys, mirr, peak2 = align2d_no_mirror(mirror(sci), scr,  first_ring = 1, last_ring=ou, mode="H")


	from sp_alignment import Numrinit, ringwe, ornq
	# center in SPIDER convention
	cnx = nx//2+1
	cny = ny//2+1
	#precalculate rings
	numr = Numrinit(first_ring, ou, 1, "H")
	wr   = ringwe(numr, "H")
	crefim = Util.Polar2Dm(scr, cnx, cny, numr, "H")
	Util.Frngs(crefim, numr)
	Util.Applyws(crefim, numr, wr)
	alpha1, sxs, sys, mirr, peak1 = ornq(sci, crefim, [0.0], [0.0], 1.0, "H", numr, cnx, cny)
	alpha2, sxs, sys, mirr, peak2 = ornq(mirror(sci), crefim, [0.0], [0.0], 1.0, "H", numr, cnx, cny)


	if(peak1>peak2):
		mirr = 0
		alpha = alpha1
	else:
		mirr = 1
		alpha = -alpha2
	nrx = min( 2*(xrng+1)+1, (((nx-2)//2)*2+1) )
	nry = min( 2*(yrng+1)+1, (((ny-2)//2)*2+1) )
	frotim = fft( refim )
	ccf1 = Util.window(ccf(rot_shift2D(image, alpha, 0.0, 0.0, mirr), frotim),nrx,nry)
	p1 = peak_search(ccf1)
	
	ccf2 = Util.window(ccf(rot_shift2D(image, alpha+180.0, 0.0, 0.0, mirr), frotim),nrx,nry)
	p2 = peak_search(ccf2)
	#print p1
	#print p2

	peak_val1 = p1[0][0]
	peak_val2 = p2[0][0]
	
	if peak_val1 > peak_val2:
		sxs = -p1[0][4]
		sys = -p1[0][5]
		cx = int(p1[0][1])
		cy = int(p1[0][2])
		peak = peak_val1
	else:
		alpha += 180.0
		sxs = -p2[0][4]
		sys = -p2[0][5]
		peak = peak_val2
		cx = int(p2[0][1])
		cy = int(p2[0][2])
		ccf1 = ccf2
	from sp_utilities import model_blank
	#print cx,cy
	z = model_blank(3,3)
	for i in range(3):
		for j in range(3):
			z[i,j] = ccf1[i+cx-1,j+cy-1]
	#print  ccf1[cx,cy],z[1,1]
	XSH, YSH, PEAKV = parabl(z)
	#print sxs, sys, XSH, YSH, PEAKV, peak
	if(mirr == 1):  	sx = -sxs+XSH
	else:               sx =  sxs-XSH
	return alpha, sx, sys-YSH, mirr, PEAKV



def multalign2dscf(image, refrings, frotim, numr, xrng=-1, yrng=-1, ou = -1):
	from sp_fundamentals import scf, rot_shift2D, ccf, mirror
	from sp_utilities import peak_search, model_blank
	from math import radians, sin, cos
	from sp_alignment import ang_n

	nx = image.get_xsize()
	ny = image.get_xsize()
	if(ou<0):  ou = min(nx//2-1,ny//2-1)
	if(yrng < 0):  yrng = xrng
	if(ou<2):
		ERROR('Radius of the object (ou) has to be given','align2d_scf',1)
	sci = scf(image)
	first_ring = 1
	# center in SPIDER convention
	cnx = nx//2+1
	cny = ny//2+1

	cimage = Util.Polar2Dm(sci, cnx, cny, numr, "H")
	Util.Frngs(cimage, numr)
	mimage = Util.Polar2Dm(mirror(sci), cnx, cny, numr, "H")
	Util.Frngs(mimage, numr)

	nrx = min( 2*(xrng+1)+1, (((nx-2)//2)*2+1) )
	nry = min( 2*(yrng+1)+1, (((ny-2)//2)*2+1) )

	totpeak = -1.0e23

	for iki in range(len(refrings)):
		#print  "TEMPLATE  ",iki
		#  Find angle
		retvals = Util.Crosrng_e(refrings[iki], cimage, numr, 0, 0.0)
		alpha  = ang_n(retvals["tot"], "H", numr[-1])
		peak 	= retvals["qn"]
		#print  alpha1, peak1
		#print  alpha2, peak2

		ccf1 = Util.window(ccf(rot_shift2D(image, alpha, 0.0, 0.0, mirr), frotim[iki]), nrx, nry)
		p1 = peak_search(ccf1)

		sxs = -p1[0][4]
		sys = -p1[0][5]
		cx = int(p1[0][1])
		cy = int(p1[0][2])
		peak = p1[0][0]

		#print cx,cy
		z = model_blank(3,3)
		for i in range(3):
			for j in range(3):
				z[i,j] = ccf1[i+cx-1,j+cy-1]
		#print  ccf1[cx,cy],z[1,1]
		XSH, YSH, PEAKV = parabl(z)
		#print  PEAKV
		if(PEAKV > totpeak):
			totpeak = PEAKV
			iref = iki
			if(mirr == 1):  	sx = -sxs+XSH
			else:               sx =  sxs-XSH
			sy = sys-YSH
			talpha = alpha
			#print "BETTER",sx,sy,iref,talpha,tmirr,totpeak
			#return alpha, sx, sys-YSH, mirr, PEAKV
	return sx,sy,iref,talpha,totpeak

def multalign2d_scf(image, refrings, frotim, numr, xrng=-1, yrng=-1, ou = -1):
	from sp_fundamentals import scf, rot_shift2D, ccf, mirror
	from sp_utilities import peak_search, model_blank
	from math import radians, sin, cos
	from sp_alignment import ang_n

	nx = image.get_xsize()
	ny = image.get_xsize()
	if(ou<0):  ou = min(nx//2-1,ny//2-1)
	if(yrng < 0):  yrng = xrng
	if(ou<2):
		ERROR('Radius of the object (ou) has to be given','align2d_scf',1)
	sci = scf(image)
	first_ring = 1
	# center in SPIDER convention
	cnx = nx//2+1
	cny = ny//2+1

	cimage = Util.Polar2Dm(sci, cnx, cny, numr, "H")
	Util.Frngs(cimage, numr)
	mimage = Util.Polar2Dm(mirror(sci), cnx, cny, numr, "H")
	Util.Frngs(mimage, numr)

	nrx = min( 2*(xrng+1)+1, (((nx-2)//2)*2+1) )
	nry = min( 2*(yrng+1)+1, (((ny-2)//2)*2+1) )

	totpeak = -1.0e23

	for iki in range(len(refrings)):
		#print  "TEMPLATE  ",iki
		#  Find angle
		retvals = Util.Crosrng_e(refrings[iki], cimage, numr, 0, 0.0)
		alpha1  = ang_n(retvals["tot"], "H", numr[-1])
		peak1 	= retvals["qn"]
		retvals = Util.Crosrng_e(refrings[iki], mimage, numr, 0, 0.0)
		alpha2  = ang_n(retvals["tot"], "H", numr[-1])
		peak2 	= retvals["qn"]
		#print  alpha1, peak1
		#print  alpha2, peak2

		if(peak1>peak2):
			mirr = 0
			alpha = alpha1
		else:
			mirr = 1
			alpha = -alpha2

		ccf1 = Util.window(ccf(rot_shift2D(image, alpha, 0.0, 0.0, mirr), frotim[iki]), nrx, nry)
		p1 = peak_search(ccf1)
	
		ccf2 = Util.window(ccf(rot_shift2D(image, alpha+180.0, 0.0, 0.0, mirr), frotim[iki]), nrx, nry)
		p2 = peak_search(ccf2)
		#print p1
		#print p2

		peak_val1 = p1[0][0]
		peak_val2 = p2[0][0]
	
		if peak_val1 > peak_val2:
			sxs = -p1[0][4]
			sys = -p1[0][5]
			cx = int(p1[0][1])
			cy = int(p1[0][2])
			peak = peak_val1
		else:
			alpha += 180.0
			sxs = -p2[0][4]
			sys = -p2[0][5]
			peak = peak_val2
			cx = int(p2[0][1])
			cy = int(p2[0][2])
			ccf1 = ccf2
		#print cx,cy
		z = model_blank(3,3)
		for i in range(3):
			for j in range(3):
				z[i,j] = ccf1[i+cx-1,j+cy-1]
		#print  ccf1[cx,cy],z[1,1]
		XSH, YSH, PEAKV = parabl(z)
		#print  PEAKV
		if(PEAKV > totpeak):
			totpeak = PEAKV
			iref = iki
			if(mirr == 1):  	sx = -sxs+XSH
			else:               sx =  sxs-XSH
			sy = sys-YSH
			talpha = alpha
			tmirr = mirr
			#print "BETTER",sx,sy,iref,talpha,tmirr,totpeak
			#return alpha, sx, sys-YSH, mirr, PEAKV
	return sx,sy,iref,talpha,tmirr,totpeak

def parabl(Z):
	#  parabolic fit to a peak, C indexing
	C1 = (26.*Z[0,0] - Z[0,1] + 2*Z[0,2] - Z[1,0] - 19.*Z[1,1] - 7.*Z[1,2] + 2.*Z[2,0] - 7.*Z[2,1] + 14.*Z[2,2])/9.

	C2 = (8.* Z[0,0] - 8.*Z[0,1] + 5.*Z[1,0] - 8.*Z[1,1] + 3.*Z[1,2] +2.*Z[2,0] - 8.*Z[2,1] + 6.*Z[2,2])/(-6.)

	C3 = (Z[0,0] - 2.*Z[0,1] + Z[0,2] + Z[1,0] -2.*Z[1,1] + Z[1,2] + Z[2,0] - 2.*Z[2,1] + Z[2,2])/6.

	C4 = (8.*Z[0,0] + 5.*Z[0,1] + 2.*Z[0,2] -8.*Z[1,0] -8.*Z[1,1] - 8.*Z[1,2] + 3.*Z[2,1] + 6.*Z[2,2])/(-6.)

	C5 = (Z[0,0] - Z[0,2] - Z[2,0] + Z[2,2])/4.

	C6 = (Z[0,0] + Z[0,1] + Z[0,2] - 2.*Z[1,0] - 2.*Z[1,1] -2.*Z[1,2] + Z[2,0] + Z[2,1] + Z[2,2])/6.

	DENOM = 4. * C3 * C6 - C5 * C5
	if(DENOM == 0.):
		return 0.0, 0.0, 0.0

	YSH   = (C4*C5 - 2.*C2*C6) / DENOM - 2.
	XSH   = (C2*C5 - 2.*C4*C3) / DENOM - 2.

	PEAKV = 4.*C1*C3*C6 - C1*C5*C5 - C2*C2*C6 + C2*C4*C5 - C4*C4*C3
	PEAKV = PEAKV / DENOM
	#print  "  in PARABL  ",XSH,YSH,Z[1,1],PEAKV

	XSH = min(max( XSH, -1.0), 1.0)
	YSH = min(max( YSH, -1.0), 1.0)

	return XSH, YSH, PEAKV
"""Multiline Comment15"""
#MULTILINEMULTILINEMULTILINE 15
	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15
	#MULTILINEMULTILINEMULTILINE 15
		#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15
	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15
	#MULTILINEMULTILINEMULTILINE 15
	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15
	#MULTILINEMULTILINEMULTILINE 15

	#MULTILINEMULTILINEMULTILINE 15
#MULTILINEMULTILINEMULTILINE 15

def align2d_direct2(image, refim, xrng=1, yrng=1, psimax=1, psistep=1, ou = -1):
	from sp_fundamentals import fft, rot_shift2D, ccf, mirror
	from sp_utilities import peak_search, model_circle, model_blank, inverse_transform2
	from math import radians, sin, cos
	
	nx = image.get_xsize()
	if(ou<0):  ou = nx//2-1
	mask = model_circle(ou,nx,nx)
	nk = int(psimax/psistep)
	nm = 2*nk+1
	nc = nk + 1
	refs = [None]*nm*2
	for i in range(nm):
		refs[2*i] = fft(rot_shift2D(refim, (i-nc)*psistep)*mask)
		refs[2*i+1] = fft(rot_shift2D(refim, (i-nc)*psistep+180.0)*mask)
	ims = fft(image)
	ama = -1.e23
	bang = 0.
	bsx = 0.
	bsy = 0.
	for i in range(1,nm*2):
		c = ccf(ims, refs[i])
		#c.write_image('rer.hdf')
		#exit()
		w = Util.window(c,2*xrng+1,2*yrng+1)
		pp =peak_search(w)[0]
		px = int(pp[4])
		py = int(pp[5])
		if( pp[0] == 1.0 and px == 0 and py == 0):
			pass #XSH, YSH, PEAKV = 0.,0.,0.
		else:
			ww = model_blank(3,3)
			ux = int(pp[1])
			uy = int(pp[2])
			for k in range(3):
				for l in range(3):
					ww[k,l] = w[k+ux-1,l+uy-1]
			XSH, YSH, PEAKV = parabl(ww)
			#print i,pp[-1],XSH, YSH,px+XSH, py+YSH, PEAKV
			if(PEAKV >ama):
				ama = PEAKV
				bsx = px+round(XSH,2)
				bsy = py+round(YSH,2)
				bang = i
	# returned parameters have to be inverted
	bang = (bang//2-nc)*psistep + 180.*(bang%2)
	sxprint(bang,bsx,bsy)
	bang, bsx, bsy, i = inverse_transform2(bang, bsx, bsy)
	return bang, bsx, bsy, ama


def align2d_direct3(input_images, refim, xrng=1, yrng=1, psimax=180, psistep=1, ou = -1, CTF = None):
	from sp_fundamentals import fft, rot_shift2D, ccf, mirror
	from sp_filter       import filt_ctf
	from sp_utilities    import peak_search, model_circle, model_blank, inverse_transform2
	from math import radians, sin, cos
	
	nx = input_images[0].get_xsize()
	if(ou<0):  ou = nx//2-1
	mask = model_circle(ou,nx,nx)
	nk = int(psimax/psistep)
	nm = 2*nk+1
	nc = nk + 1
	refs = [None]*nm*2
	for i in range(nm):
		temp = rot_shift2D(refim, (i-nc)*psistep)*mask
		refs[2*i] = [fft(temp), fft(mirror(temp))]
		temp = rot_shift2D(refim, (i-nc)*psistep+180.0)*mask
		refs[2*i+1] = [fft(temp), fft(mirror(temp))]
	del temp

	results = []
	mir = 0
	for image in input_images:
		if CTF:  ims = filt_ctf(fft(image), image.get_attr("ctf"))
		else:    ims = fft(image)
		ama = -1.e23
		bang = 0.
		bsx = 0.
		bsy = 0.
		for i in range(nm*2):
			for mirror_flag in [0, 1]:
				c = ccf(ims, refs[i][mirror_flag])
				w = Util.window(c,2*xrng+1,2*yrng+1)
				pp =peak_search(w)[0]
				px = int(pp[4])
				py = int(pp[5])
				if( pp[0] == 1.0 and px == 0 and py == 0):
					pass #XSH, YSH, PEAKV = 0.,0.,0.
				else:
					ww = model_blank(3,3)
					ux = int(pp[1])
					uy = int(pp[2])
					for k in range(3):
						for l in range(3):
							ww[k,l] = w[k+ux-1,l+uy-1]
					XSH, YSH, PEAKV = parabl(ww)
					#print i,pp[-1],XSH, YSH,px+XSH, py+YSH, PEAKV
					if(PEAKV >ama):
						ama = PEAKV
						bsx = px+round(XSH,2)
						bsy = py+round(YSH,2)
						bang = i
						mir = mirror_flag
		# returned parameters have to be inverted
		bang = (bang//2-nc)*psistep + 180.*(bang%2)
		bang, bsx, bsy, _ = inverse_transform2(bang, (1 - 2*mir)*bsx, bsy, mir)
		results.append([bang, bsx, bsy, mir, ama])
	return results


def align2d_direct(image, refim, xrng=1, yrng=1, psimax=1, psistep=1, ou = -1):
	from sp_fundamentals import fft, rot_shift2D, ccf
	from sp_utilities import model_blank, model_circle, peak_search, compose_transform2, inverse_transform2
	from math import radians, sin, cos

	nx = image.get_xsize()
	if(ou<0):  ou = nx//2-1
	mask = model_circle(ou,nx,nx)
	nk = int(psimax/psistep)
	nm = 2*nk+1
	nc = nk + 1
	refs = [None]*nm
	for i in range(nm):
		refs[i] = fft(rot_shift2D(refim, (i-nc)*psistep)*mask)
	ims = fft(image)
	imr = fft(rot_shift2D(image, 180.0))
	ama  = -1.0e23
	bang = 0.0
	bsx  = 0.0
	bsy  = 0.0
	for i in range(nm):
		c = ccf(ims, refs[i])
		w = Util.window(c,2*xrng+1,2*yrng+1)
		pp =peak_search(w)[0]
		px = int(pp[4])
		py = int(pp[5])
		if( pp[0] == 1.0 and px == 0 and py == 0):
			pass #XSH, YSH, PEAKV = 0.,0.,0.
		else:
			ww = model_blank(3,3)
			ux = int(pp[1])
			uy = int(pp[2])
			for k in range(3):
				for l in range(3):
					ww[k,l] = w[k+ux-1,l+uy-1]
			XSH, YSH, PEAKV = parabl(ww)
			#print i,pp[-1],XSH, YSH,px+XSH, py+YSH, PEAKV
			if(PEAKV >ama):
				ama = PEAKV
				bsx = px+round(XSH,2)
				bsy = py+round(YSH,2)
				bang = i
				rt180 = 0.
		c = ccf(imr, refs[i])
		#c.write_image('imr.hdf')
		#exit()
		c = rot_shift2D(c,180)
		w = Util.window(c,2*xrng+1,2*yrng+1)
		pp =peak_search(w)[0]
		px = int(pp[4])
		py = int(pp[5])
		if( pp[0] == 1.0 and px == 0 and py == 0):
			pass #XSH, YSH, PEAKV = 0.,0.,0.
		else:
			ww = model_blank(3,3)
			ux = int(pp[1])
			uy = int(pp[2])
			for k in range(3):
				for l in range(3):
					ww[k,l] = w[k+ux-1,l+uy-1]
			XSH, YSH, PEAKV = parabl(ww)
			#print i,pp[-1],XSH, YSH,px+XSH, py+YSH, PEAKV
			if(PEAKV >ama):
				ama = PEAKV
				bsx = px+round(XSH,2)
				bsy = py+round(YSH,2)
				bang = i
				rt180 = 180.
	# returned parameters have to be inverted
	bang = 180-(bang//2-nc)*psistep
	sxprint(bang, bsx, bsy,rt180)
	bang, bsx, bsy, i = inverse_transform2(bang, bsx, bsy)
	return bang, bsx, bsy, ama


def align2d_no_mirror(image, refim, xrng=0, yrng=0, step=1, first_ring=1, last_ring=0, rstep=1, mode = "F"):
	"""  Determine shift and rotation between image and reference image
	     no mirror
	     quadratic interpolation
	     Output: ang, sxs, sys, mirror, peak  # Mirror==0
	"""
	#from utilities import print_col
	from sp_alignment import Numrinit, ringwe
	step = float(step)
	nx = refim.get_xsize()
	ny = refim.get_ysize()
	MAX_XRNG = nx/2
	MAX_YRNG=ny/2
	if xrng >= MAX_XRNG:
		ERROR('Translation search range in x is at most %d'%MAX_XRNG, "align2d ", 1)
	if yrng >= MAX_YRNG:
		ERROR('Translation search range in y is at most %d'%MAX_YRNG, "align2d ", 1)
	if(last_ring == 0):  last_ring = nx/2-2-int(max(xrng,yrng))
	# center in SPIDER convention
	cnx = nx//2+1
	cny = ny//2+1
	#precalculate rings
	numr = Numrinit(first_ring, last_ring, rstep, mode)
	wr   = ringwe(numr, mode)
	crefim = Util.Polar2Dm(refim, cnx, cny, numr, mode)
	Util.Frngs(crefim, numr)
	Util.Applyws(crefim, numr, wr)
	return ornq(image, crefim, [xrng], [yrng], step, mode, numr, cnx, cny)


def align2d_peaks(image, refim, xrng=0, yrng=0, step=1, first_ring=1, last_ring=0, rstep=1, mode = "F"):
	"""  Determine shift and rotation between image and reference image
	     quadratic interpolation
	"""
	#from utilities import print_col
	from sp_alignment import Numrinit, ringwe
	step = float(step)
	nx = refim.get_xsize()
	ny = refim.get_ysize()
	if(last_ring == 0):  last_ring = nx/2-2-int(max(xrng,yrng))
	# center in SPIDER convention
	cnx = nx//2+1
	cny = ny//2+1
	#precalculate rings
	numr = Numrinit(first_ring, last_ring, rstep, mode)
	wr   = ringwe(numr, mode)
	#cimage=Util.Polar2Dmi(refim, cnx, cny, numr, mode, kb)
	crefim = Util.Polar2Dm(refim, cnx, cny, numr, mode)
	#crefim = Util.Polar2D(refim, numr, mode)
	#print_col(crefim)
	Util.Frngs(crefim, numr)
	Util.Applyws(crefim, numr, wr)
	return ormq_peaks(image, crefim, [xrng], [yrng], step, mode, numr, cnx, cny)

def align2d_g(image, refim, xrng=0, yrng=0, step=1, first_ring=1, last_ring=0, rstep=1, mode = "F"):
	"""  Determine shift and rotation between image and reference image
	     quadratic interpolation
	"""
	from sp_development import ormy2
	from sp_alignment import Numrinit, ringwe
	from sp_fundamentals import fft
	
	step = float(step)
	nx = refim.get_xsize()
	ny = refim.get_ysize()
	if(last_ring == 0):  last_ring = nx/2-2-int(max(xrng,yrng))
	# center in SPIDER convention
	cnx = int(nx/2)+1
	cny = int(ny/2)+1
	#precalculate rings
	numr = Numrinit(first_ring, last_ring, rstep, mode)
	wr = ringwe(numr, mode)

	N = nx*2
	K = 6
	alpha = 1.75
	r = nx/2
	v = K/2.0/N
	kb = Util.KaiserBessel(alpha, K, r, v, N)
	refi = refim.FourInterpol(N,N,1,0)  
	params = {"filter_type" : Processor.fourier_filter_types.KAISER_SINH_INVERSE,"alpha" : alpha, "K":K,"r":r,"v":v,"N":N}
	q = Processor.EMFourierFilter(refi,params)
	refi = fft(q)
	crefim = Util.Polar2Dmi(refi,cnx,cny,numr,mode,kb)

	Util.Frngs(crefim, numr)
	Util.Applyws(crefim, numr, wr)
	numr = Numrinit(first_ring, last_ring, rstep, mode)

	return ormy2(image,refim,crefim,xrng,yrng,step,mode,numr,cnx,cny,"gridding")


def directali(inima, refs, psimax=1.0, psistep=1.0, xrng=1, yrng=1, updown = "both"):
	"""
	Direct 2D alignment within a predefined angular range.  If the range is large the method will be very slow.
	refs - a stack of reference images.  If a single image, the stack will be created.
	updown - one of three keywords: both, up, down, indicating which angle to consider, 0, 180, or both.
	PAP 12/20/2014
	"""
	from sp_fundamentals import fft, rot_shift2D, ccf
	from sp_utilities    import peak_search, model_blank, inverse_transform2, compose_transform2
	from sp_alignment    import parabl

	nr = int(2*psimax/psistep)+1
	nc = nr//2

	try:
		wn = len(refs)
		if(wn != nr):
			ERROR("Incorrect number of reference images","directali",1)
		ref = refs
	except:
		ref = [None]*nr
		for i in range(nr):  ref[i] = fft(rot_shift2D(refs,(i-nc)*psistep))

	#  Have to add 1 as otherwise maximum on the edge of the window will not be found
	wnx = 2*(xrng+1) + 1
	wny = 2*(yrng+1) + 1

	if updown == "both" or updown == "up" :    ima = fft(inima)
	if updown == "both" or updown == "down" :  imm = fft(rot_shift2D(inima,180.0, interpolation_method = 'linear'))

	sxprint(" in ali  ", psimax, psistep, xrng, yrng, wnx, wny, nr,updown) 
	ma1  = -1.e23
	ma2  = -1.e23
	ma3  = -1.e23
	ma4  = -1.e23
	oma2 = [-1.e23, -1.e23, -1.e23]
	oma4 = [-1.e23, -1.e23, -1.e23]
	"""Multiline Comment16"""
	#MULTILINEMULTILINEMULTILINE 16
	#MULTILINEMULTILINEMULTILINE 16
	#MULTILINEMULTILINEMULTILINE 16
	#MULTILINEMULTILINEMULTILINE 16
	#MULTILINEMULTILINEMULTILINE 16
	for i in range(nr):
		if updown == "both" or updown == "up" :
			c = ccf(ima,ref[i])
			w = Util.window(c, wnx, wny)
			pp = peak_search(w)[0]
			px = int(pp[4])
			py = int(pp[5])
			sxprint('  peak   ',i,pp)
			#  did not find a peak, find a maximum location instead
			if( pp[0] == 1.0 and px == 0 and py == 0):
				loc = w.calc_max_location()
				PEAKV = w.get_value_at(loc[0],loc[1])
				if(PEAKV>ma2):
					ma2  = PEAKV
					oma2 = pp+[loc[0], loc[1], loc[0], loc[1], PEAKV,(i-nc)*psistep]
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["S %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment17"""
				#MULTILINEMULTILINEMULTILINE 17
					#MULTILINEMULTILINEMULTILINE 17
					#MULTILINEMULTILINEMULTILINE 17
				#MULTILINEMULTILINEMULTILINE 17
				if(PEAKV>ma2):
					ma2  = PEAKV
					oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]
		if updown == "both" or updown == "down" :
			c = ccf(imm,ref[i])
			w = Util.window(c, wnx, wny)
			pp = peak_search(w)[0]
			px = int(pp[4])
			py = int(pp[5])
			if( pp[0] == 1.0 and px == 0 and py == 0):
				loc = w.calc_max_location()
				PEAKV = w.get_value_at(loc[0],loc[1])
				if(PEAKV>ma4):
					ma4  = PEAKV
					oma4 = pp+[loc[0], loc[1], loc[0], loc[1], PEAKV,(i-nc)*psistep]
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["R %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment18"""
				#MULTILINEMULTILINEMULTILINE 18
					#MULTILINEMULTILINEMULTILINE 18
					#MULTILINEMULTILINEMULTILINE 18
				#MULTILINEMULTILINEMULTILINE 18
				if(PEAKV>ma4):
					ma4 = PEAKV
					oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]

	sxprint("  hoho ",updown)
	sxprint("  oma2 ",oma2)
	sxprint("  oma4 ",oma4)
	if( oma2[-2] > oma4[-2] ):
		"""Multiline Comment19"""
		#MULTILINEMULTILINEMULTILINE 19
		#MULTILINEMULTILINEMULTILINE 19
		#MULTILINEMULTILINEMULTILINE 19
		#MULTILINEMULTILINEMULTILINE 19
		nalpha, ntx, nty, mirror = inverse_transform2(oma2[-1],oma2[-4],oma2[-3],0)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		peak = oma2[-2]
	else:
		"""Multiline Comment20"""
		#MULTILINEMULTILINEMULTILINE 20
		#MULTILINEMULTILINEMULTILINE 20
		#MULTILINEMULTILINEMULTILINE 20
		nalpha, ntx, nty, junk = compose_transform2(oma4[-1],oma4[-4],oma4[-3],1.0,180.,0,0,1)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		nalpha, ntx, nty, mirror = inverse_transform2(nalpha, ntx, nty,0)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		peak = oma4[-2]
	return  nalpha, ntx, nty, peak


def preparerefsgrid(refs, psimax=1.0, psistep=1.0):
	from sp_fundamentals import prepi, fft
	from EMAN2 import Processor

	M = refs.get_xsize()
	alpha = 1.75
	K = 6
	N = M*2  # npad*image size
	r = M/2
	v = K/2.0/N
	params = {"filter_type" : Processor.fourier_filter_types.KAISER_SINH_INVERSE,
	          "alpha" : alpha, "K":K,"r":r,"v":v,"N":N}
	kb = Util.KaiserBessel(alpha, K, r, v, N)

	nr = int(2*psimax/psistep)+1
	nc = nr//2

	ref = [None]*nr
	ima,kb = prepi(refs)
	from math import radians
	psisteprad = radians(psistep)
	for i in range(nr):
		# gridding rotation
		ref[i] = fft(ima.rot_scale_conv_new_background_twice((i-nc)*psisteprad, 0.,0., kb, 1.))

	return  ref


def preparerefsgrid1(refs, psimax=1.0, psistep=1.0):
	from sp_fundamentals import prepi, fft
	from EMAN2 import Processor

	M = refs.get_xsize()
	alpha = 1.75
	K = 6
	N = M*2  # npad*image size
	r = M/2
	v = K/2.0/N
	params = {"filter_type" : Processor.fourier_filter_types.KAISER_SINH_INVERSE,
	          "alpha" : alpha, "K":K,"r":r,"v":v,"N":N}
	kb = Util.KaiserBessel(alpha, K, r, v, N)

	nr = int(2*psimax/psistep)+1
	nc = nr//2
# 	if updown == "up" :  reduced_psiref = psiref -  90.0
# 	else:                reduced_psiref = psiref - 270.0


	
	ref = [None]*nr
	ima,kb = prepi(refs)
	from math import radians
	psisteprad = radians(psistep)
# if psimax > 0:
# 		bnr = int(round(reduced_psiref/psistep)) - nc
# 		enr = nr + bnr
	for i in range(0,nr):
		# gridding rotation
		ref[i] = fft(ima.rot_scale_conv_new_background_twice((i-nc)*psisteprad, 0.,0., kb, 1.))

# 	if psimax == 0:
# 		ref[0] = fft(ima.rot_scale_conv_new_background_twice(radians(reduced_psiref), 0.,0., kb, 1.))

		
		
	return  ref
	
	
def directaligridding(inima, refs, psimax=1.0, psistep=1.0, xrng=1, yrng=1, stepx = 1.0, stepy = 1.0, updown = "both"):
	"""
	Direct 2D alignment within a predefined angular range.  If the range is large the method will be very slow.
	refs - a stack of reference images.  If a single image, the stack will be created.
	updown - one of three keywords: both, up, down, indicating which angle to consider, 0, 180, or both.
	PAP 12/27/2014
	"""
	#  Eventually will have to pass kb here
	from sp_fundamentals import fft, rot_shift2D, ccf, prepi
	from sp_utilities    import peak_search, model_blank, inverse_transform2, compose_transform2
	from sp_alignment    import parabl
	from EMAN2 import Processor
	sxprint("  directaligridding  ",psimax, psistep, xrng, yrng, stepx, stepy, updown)
	M = inima.get_xsize()
	alpha = 1.75
	K = 6
	N = M*2  # npad*image size
	r = M/2
	v = K/2.0/N
	params = {"filter_type" : Processor.fourier_filter_types.KAISER_SINH_INVERSE,
	          "alpha" : alpha, "K":K,"r":r,"v":v,"N":N}
	kb = Util.KaiserBessel(alpha, K, r, v, N)



	nr = int(2*psimax/psistep)+1
	nc = nr//2

	try:
		wn = len(refs)
		if(wn != nr):
			ERROR("Incorrect number of reference images","directali",1)
		"""Multiline Comment21"""
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		#MULTILINEMULTILINEMULTILINE 21
		ref = refs
	except:
		ref = [None]*nr
		ima,kb = prepi(refs)
		from math import radians
		psisteprad = radians(psistep)
		for i in range(nr):
			# gridding rotation
			ref[i] = fft(ima.rot_scale_conv_new_background_twice((i-nc)*psisteprad, 0.,0., kb, 1.))
			"""Multiline Comment22"""
			#MULTILINEMULTILINEMULTILINE 22
			#MULTILINEMULTILINEMULTILINE 22
			#MULTILINEMULTILINEMULTILINE 22
			#MULTILINEMULTILINEMULTILINE 22

	#  Window for ccf sampled by gridding
	rnx   = int((xrng/stepx+0.5))
	rny   = int((yrng/stepy+0.5))
	wnx = 2*rnx + 1
	wny = 2*rny + 1
	w = model_blank( wnx, wny)
	stepxx = 2*stepx
	stepyy = 2*stepy
	nic = N//2
	wxc = wnx//2
	wyc = wny//2

	if updown == "both" or updown == "up" :
		ima = inima.FourInterpol(N, N, 1,0)
		ima = Processor.EMFourierFilter(ima,params)

	if updown == "both" or updown == "down" :
		imm = rot_shift2D(inima,180.0, interpolation_method = 'linear')
		imm = imm.FourInterpol(N, N, 1,0)
		imm = Processor.EMFourierFilter(imm, params)

	#fft(ima).write_image('imap.hdf')
	from sp_utilities import get_params_proj
	e1 = ref[0]['phi']
	f1,e2,e3,e4,e5 = get_params_proj(inima)
	sxprint(" in ali  ", e1,f1,psimax, psistep, xrng, yrng, wnx, wny, rnx, rny, stepxx, stepyy, nr,updown) 
	ma1  = -1.e23
	ma2  = -1.e23
	ma3  = -1.e23
	ma4  = -1.e23
	oma2 = [-1.e23, -1.e23, -1.e23]
	oma4 = [-1.e23, -1.e23, -1.e23]
	"""Multiline Comment23"""
	#MULTILINEMULTILINEMULTILINE 23
	#MULTILINEMULTILINEMULTILINE 23
	#MULTILINEMULTILINEMULTILINE 23
	#MULTILINEMULTILINEMULTILINE 23
	#MULTILINEMULTILINEMULTILINE 23
	for i in range(nr):
		if updown == "both" or updown == "up" :
			c = ccf(ima,ref[i])
			#c.write_image('gcc.hdf')
			#p = peak_search(window2d(c,4*xrng+1,4*yrng+1),5)
			#for q in p: print q
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nic, iy*stepyy+nic, 0.0, kb)

			pp = peak_search(w)[0]
			#print '  peak   ',i,pp
			#from sys import exit
			#exit()

			px = int(pp[4])
			py = int(pp[5])
			#print '  peak   ',i,pp,px*stepx,py*stepy
			#  did not find a peak, find a maximum location instead
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				return  0., 0., 0., -1.0e23
				"""Multiline Comment24"""
				#MULTILINEMULTILINEMULTILINE 24
				#MULTILINEMULTILINEMULTILINE 24
				#MULTILINEMULTILINEMULTILINE 24
				#MULTILINEMULTILINEMULTILINE 24
						#MULTILINEMULTILINEMULTILINE 24
						#MULTILINEMULTILINEMULTILINE 24
				#MULTILINEMULTILINEMULTILINE 24
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				sxprint(["S %10.1f"%pp[k] for k in range(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep))
				"""Multiline Comment25"""
				#MULTILINEMULTILINEMULTILINE 25
					#MULTILINEMULTILINEMULTILINE 25
					#MULTILINEMULTILINEMULTILINE 25
				#MULTILINEMULTILINEMULTILINE 25
				if(PEAKV>ma2):
					ma2  = PEAKV
					oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]
		if updown == "both" or updown == "down" :
			c = ccf(imm,ref[i])
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nic, iy*stepyy+nic, 0.0, kb)
			pp = peak_search(w)[0]
			px = int(pp[4])
			py = int(pp[5])
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				return  0., 0., 0., -1.0e23
				"""Multiline Comment26"""
				#MULTILINEMULTILINEMULTILINE 26
				#MULTILINEMULTILINEMULTILINE 26
				#MULTILINEMULTILINEMULTILINE 26
					#MULTILINEMULTILINEMULTILINE 26
					#MULTILINEMULTILINEMULTILINE 26
				#MULTILINEMULTILINEMULTILINE 26
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				sxprint(["R %10.1f"%pp[k] for k in range(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep))
				"""Multiline Comment27"""
				#MULTILINEMULTILINEMULTILINE 27
					#MULTILINEMULTILINEMULTILINE 27
					#MULTILINEMULTILINEMULTILINE 27
				#MULTILINEMULTILINEMULTILINE 27
				if(PEAKV>ma4):
					ma4 = PEAKV
					oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]

	#print "  hoho ",updown
	#print "  oma2 ",oma2
	#print "  oma4 ",oma4
	if( oma2[-2] > oma4[-2] ):
		"""Multiline Comment28"""
		#MULTILINEMULTILINEMULTILINE 28
		#MULTILINEMULTILINEMULTILINE 28
		#MULTILINEMULTILINEMULTILINE 28
		#MULTILINEMULTILINEMULTILINE 28
		nalpha, ntx, nty, mirror = inverse_transform2(oma2[-1],oma2[-4]*stepx,oma2[-3]*stepy,0)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		peak = oma2[-2]
	else:
		"""Multiline Comment29"""
		#MULTILINEMULTILINEMULTILINE 29
		#MULTILINEMULTILINEMULTILINE 29
		#MULTILINEMULTILINEMULTILINE 29
		nalpha, ntx, nty, junk = compose_transform2(oma4[-1],oma4[-4]*stepx,oma4[-3]*stepy,1.0,180.,0,0,1)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		nalpha, ntx, nty, mirror = inverse_transform2(nalpha, ntx, nty,0)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		peak = oma4[-2]
	return  nalpha, ntx, nty, peak


def directaligridding1(inima, kb, ref, psimax=1.0, psistep=1.0, xrng=1, yrng=1, stepx = 1.0, stepy = 1.0, updown = "both"):
	"""
	Direct 2D alignment within a predefined angular range.  If the range is large the method will be very slow.
	ref - a stack of reference images. 
	updown - one of three keywords: both, up, down, indicating which angle to consider, 0, 180, or both.
	PAP 01/01/2015
	"""

	from sp_fundamentals import fft, rot_shift2D, ccf, prepi
	from sp_utilities    import peak_search, model_blank, inverse_transform2, compose_transform2
	from sp_alignment    import parabl
	from EMAN2 import Processor
	#print  "  directaligridding1  ",psimax, psistep, xrng, yrng, stepx, stepy, updown

	"""Multiline Comment30"""
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	          #MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30
	#MULTILINEMULTILINEMULTILINE 30


	nr = int(2*psimax/psistep)+1
	nc = nr//2

	N = inima.get_ysize()  # assumed image is square, but because it is FT take y.
	#  Window for ccf sampled by gridding
	rnx   = int((xrng/stepx+0.5))
	rny   = int((yrng/stepy+0.5))
	wnx = 2*rnx + 1
	wny = 2*rny + 1
	w = model_blank( wnx, wny)
	stepxx = 2*stepx
	stepyy = 2*stepy
	nic = N//2
	wxc = wnx//2
	wyc = wny//2

	if updown == "both" or updown == "up" :
		ima = inima
		#ima = inima.FourInterpol(N, N, 1,0)
		#ima = Processor.EMFourierFilter(ima,params)

	if updown == "both" or updown == "down" :
		#  This yields rotation by 180 degrees.  There is no extra shift as the image was padded 2x, so it is even-sized, but two rows are incorrect
		imm = inima.conjg()
		#imm = rot_shift2D(inima,180.0, interpolation_method = 'linear')
		#imm = imm.FourInterpol(N, N, 1,0)
		#imm = Processor.EMFourierFilter(imm,params)

	#fft(ima).write_image('imap.hdf')

	ma1  = -1.e23
	ma2  = -1.e23
	ma3  = -1.e23
	ma4  = -1.e23
	oma2 = [-1.e23, -1.e23, -1.e23]
	oma4 = [-1.e23, -1.e23, -1.e23]
	"""Multiline Comment31"""
	#MULTILINEMULTILINEMULTILINE 31
	#MULTILINEMULTILINEMULTILINE 31
	#MULTILINEMULTILINEMULTILINE 31
	#MULTILINEMULTILINEMULTILINE 31
	#MULTILINEMULTILINEMULTILINE 31
	for i in range(nr):
		if updown == "both" or updown == "up" :
			c = ccf(ima,ref[i])
			#c.write_image('gcc.hdf')
			#p = peak_search(window2d(c,4*xrng+1,4*yrng+1),5)
			#for q in p: print q
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nic, iy*stepyy+nic, 0.0, kb)

			pp = peak_search(w)[0]
			#print '  peak   ',i,pp
			#from sys import exit
			#exit()

			px = int(pp[4])
			py = int(pp[5])
			#print '  peak   ',i,pp,px*stepx,py*stepy
			#  did not find a peak, find a maximum location instead
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				pass
				"""Multiline Comment32"""
				#MULTILINEMULTILINEMULTILINE 32
				#MULTILINEMULTILINEMULTILINE 32
				#MULTILINEMULTILINEMULTILINE 32
				#MULTILINEMULTILINEMULTILINE 32
						#MULTILINEMULTILINEMULTILINE 32
						#MULTILINEMULTILINEMULTILINE 32
				#MULTILINEMULTILINEMULTILINE 32
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["S %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment33"""
				#MULTILINEMULTILINEMULTILINE 33
					#MULTILINEMULTILINEMULTILINE 33
					#MULTILINEMULTILINEMULTILINE 33
				#MULTILINEMULTILINEMULTILINE 33
				if(PEAKV>ma2):
					ma2  = PEAKV
					oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]
		if updown == "both" or updown == "down" :
			c = ccf(imm,ref[i])
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nic, iy*stepyy+nic, 0.0, kb)
			pp = peak_search(w)[0]
			px = int(pp[4])
			py = int(pp[5])
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				pass
				"""Multiline Comment34"""
				#MULTILINEMULTILINEMULTILINE 34
				#MULTILINEMULTILINEMULTILINE 34
				#MULTILINEMULTILINEMULTILINE 34
					#MULTILINEMULTILINEMULTILINE 34
					#MULTILINEMULTILINEMULTILINE 34
				#MULTILINEMULTILINEMULTILINE 34
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["R %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment35"""
				#MULTILINEMULTILINEMULTILINE 35
					#MULTILINEMULTILINEMULTILINE 35
					#MULTILINEMULTILINEMULTILINE 35
				#MULTILINEMULTILINEMULTILINE 35
				if(PEAKV>ma4):
					ma4 = PEAKV
					oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]

	if( oma2[-2] > oma4[-2] ):
		peak = oma2[-2]
		if( peak == -1.0e23 ):  return  0.0, 0.0, 0.0, peak
	
		"""Multiline Comment36"""
		#MULTILINEMULTILINEMULTILINE 36
		#MULTILINEMULTILINEMULTILINE 36
		#MULTILINEMULTILINEMULTILINE 36
		#MULTILINEMULTILINEMULTILINE 36
		#  The inversion would be needed for 2D alignment.  For 3D, the proper way is to return straight results.
		#nalpha, ntx, nty, mirror = inverse_transform2(oma2[-1], oma2[-4]*stepx, oma2[-3]*stepy, 0)
		nalpha = oma2[-1]
		ntx    = oma2[-4]*stepx
		nty    = oma2[-3]*stepy
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	else:
		peak = oma4[-2]
		if( peak == -1.0e23 ):  return  0.0, 0.0, 0.0, peak
		#  This is still strange as why I would have to invert here but not for 90 degs.  PAP  01/09/2014
		#print oma3
		#print oma4

		nalpha, ntx, nty, junk = compose_transform2(-oma4[-1],oma4[-4]*stepx,oma4[-3]*stepy,1.0,180.,0,0,1)
		#nalpha = oma4[-1] + 180.0
		#ntx    = oma4[-4]*stepx
		#nty    = oma4[-3]*stepy
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		nalpha, ntx, nty, mirror = inverse_transform2(nalpha, ntx, nty, 0)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	return  nalpha, ntx, nty, peak


def directaligriddingconstrained(inima, kb, ref, psimax=1.0, psistep=1.0, xrng=1, yrng=1, \
			stepx = 1.0, stepy = 1.0, psiref = 0., txref = 0., tyref = 0., updown = "up"):
	"""
	Direct 2D alignment within a predefined angular range.  If the range is large the method will be very slow.
	ref - a stack of reference images. 
	updown - one of two keywords:  up, down, indicating which angle to consider, 0, or 180.
	
	Usage of constrains:  Search is around the previous parameters (psiref, txref, tyref), 
	                      but only within ranges specified by (psimax, xrng, yrng).
	
	PAP 01/16/2015
	"""

	from sp_fundamentals import fft, rot_shift2D, ccf, prepi
	from sp_utilities    import peak_search, model_blank, inverse_transform2, compose_transform2
	from sp_alignment    import parabl
	from EMAN2 import Processor
	#from time  import time
	#print  "  directaligridding1  ",psimax, psistep, xrng, yrng, stepx, stepy, updown
	#print  "IN         %6.2f %6.2f  %6.2f"%(psiref, txref, tyref)

	"""Multiline Comment37"""
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	          #MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37
	#MULTILINEMULTILINEMULTILINE 37


	nr = int(2*psimax/psistep)+1
	nc = nr//2
	if updown == "up" :  reduced_psiref = psiref -  90.0
	else:                reduced_psiref = psiref - 270.0
	

#  Limit psi search to within psimax range
#  It makes no sense, as it still searches within the entire range of psi_max
# 	bnr = int(round(reduced_psiref/psistep)) - nc
# 	enr = nr + bnr
	
	bnr = min(max(int(round(reduced_psiref/psistep)) - nc, -nc), nr-nc-1)
	enr = max(min(int(round(reduced_psiref/psistep))+nr-nc,nr-nc),-nc)

	if enr <= bnr: return 0.0, 0.0, 0.0, peak
	N = inima.get_ysize()  # assumed image is square, but because it is FT take y.
	#  Window for ccf sampled by gridding
	#   We quietly assume the search range for translations is always much less than the ccf size,
	#     so instead of restricting anything, we will just window out ccf around previous shift locations
	rnx   = int(round(xrng/stepx))
	rny   = int(round(yrng/stepy))
	wnx = 2*rnx + 1
	wny = 2*rny + 1

	w = model_blank( wnx, wny)
	stepxx = 2*stepx
	stepyy = 2*stepy
	
	nicx = N//2 + 2*txref #  here one would have to add or subtract the old value.
	nicy = N//2 + 2*tyref
	wxc = wnx//2
	wyc = wny//2

	if updown == "up" :
		ima = inima
		nicx = N//2 - 2*txref #  here one would have to add or subtract the old value.
		nicy = N//2 - 2*tyref
		#ima = inima.FourInterpol(N, N, 1,0)
		#ima = Processor.EMFourierFilter(ima,params)

	if updown == "down" :
		#  This yields rotation by 180 degrees.  There is no extra shift as the image was padded 2x, so it is even-sized, but two rows are incorrect
		imm = inima.conjg()

		#imm = rot_shift2D(inima,180.0, interpolation_method = 'linear')
		#imm = imm.FourInterpol(N, N, 1,0)
		#imm = Processor.EMFourierFilter(imm,params)

	#fft(ima).write_image('imap.hdf')

	ma1  = -1.e23
	ma2  = -1.e23
	ma3  = -1.e23
	ma4  = -1.e23
	oma2 = [-1.e23, -1.e23, -1.e23]
	oma4 = [-1.e23, -1.e23, -1.e23]
	"""Multiline Comment38"""
	#MULTILINEMULTILINEMULTILINE 38
	#MULTILINEMULTILINEMULTILINE 38
	#MULTILINEMULTILINEMULTILINE 38
	#MULTILINEMULTILINEMULTILINE 38
	#MULTILINEMULTILINEMULTILINE 38
	#if ( rny == 0 ) : return  0.0, 0.0, 0.0, -1.e23     ## do nothing for rny=0 @ming
	
	for i in range(bnr, enr):
		if updown == "up" :
			c = ccf(ima,ref[nc+i])
			#print "compute ccf time", time() - startc
			#c.write_image('gcc.hdf')
			#p = peak_search(window2d(c,4*xrng+1,4*yrng+1),5)
			#for q in p: print q
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nicx, iy*stepyy+nicy, 0.0, kb)
	
			pp = peak_search(w)[0]
			
			#print "find peak time", time()-startpp
			#print '  peak   ',i,pp
			#from sys import exit
			#exit()

			px = int(pp[4])
			py = int(pp[5])
			#print '  peak   ',i,pp,px*stepx,py*stepy
			#  did not find a peak, find a maximum location instead
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				pass
				"""Multiline Comment39"""
				#MULTILINEMULTILINEMULTILINE 39
				#MULTILINEMULTILINEMULTILINE 39
				#MULTILINEMULTILINEMULTILINE 39
				#MULTILINEMULTILINEMULTILINE 39
						#MULTILINEMULTILINEMULTILINE 39
						#MULTILINEMULTILINEMULTILINE 39
				#MULTILINEMULTILINEMULTILINE 39
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["S %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment40"""
				#MULTILINEMULTILINEMULTILINE 40
					#MULTILINEMULTILINEMULTILINE 40
					#MULTILINEMULTILINEMULTILINE 40
				#MULTILINEMULTILINEMULTILINE 40
				if(PEAKV>ma2):
					ma2  = PEAKV
					#if psimax > 0:
					oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,i*psistep]
					# if psimax == 0: 
# 						oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,reduced_psiref]
		if updown == "down" :
			c = ccf(imm,ref[nc+i])
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nicx, iy*stepyy+nicy, 0.0, kb)
			pp = peak_search(w)[0]
			px = int(pp[4])
			py = int(pp[5])
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				pass
				"""Multiline Comment41"""
				#MULTILINEMULTILINEMULTILINE 41
				#MULTILINEMULTILINEMULTILINE 41
				#MULTILINEMULTILINEMULTILINE 41
					#MULTILINEMULTILINEMULTILINE 41
					#MULTILINEMULTILINEMULTILINE 41
				#MULTILINEMULTILINEMULTILINE 41
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["R %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment42"""
				#MULTILINEMULTILINEMULTILINE 42
					#MULTILINEMULTILINEMULTILINE 42
					#MULTILINEMULTILINEMULTILINE 42
				#MULTILINEMULTILINEMULTILINE 42
				if(PEAKV>ma4):
					ma4 = PEAKV
					#if psimax > 0:
					oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,i*psistep]
				# 	if psimax == 0:
# 						oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,reduced_psiref]

	if( oma2[-2] > oma4[-2] ):
		peak = oma2[-2]
		if( peak == -1.0e23 ):  return  0.0, 0.0, 0.0, peak
	
		"""Multiline Comment43"""
		#MULTILINEMULTILINEMULTILINE 43
		#MULTILINEMULTILINEMULTILINE 43
		#MULTILINEMULTILINEMULTILINE 43
		#MULTILINEMULTILINEMULTILINE 43
		#  The inversion would be needed for 2D alignment.  For 3D, the proper way is to return straight results.
		#nalpha, ntx, nty, mirror = inverse_transform2(oma2[-1], oma2[-4]*stepx, oma2[-3]*stepy, 0)
		nalpha = oma2[-1]
		ntx    = oma2[-4]*stepx - txref
		nty    = oma2[-3]*stepy - tyref
		#print "oma2[-4]*stepx - txref",oma2[-4]*stepx,  txref
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	else:
		peak = oma4[-2]
		if( peak == -1.0e23 ):  return  0.0, 0.0, 0.0, peak
		#  This is still strange as why I would have to invert here but not for 90 degs.  PAP  01/09/2014
		#print oma3
		#print oma4

		nalpha, ntx, nty, junk = compose_transform2(-oma4[-1], oma4[-4]*stepx + txref,oma4[-3]*stepy + tyref,1.0,180.,0,0,1)
		#nalpha = oma4[-1] + 180.0
		#ntx    = oma4[-4]*stepx
		#nty    = oma4[-3]*stepy
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
		nalpha, ntx, nty, mirror = inverse_transform2(nalpha, ntx, nty, 0)
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	#print  "OUT        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	return  nalpha, ntx, nty, peak
	
def directaligriddingconstrained3dccf(inima, kb, ref, psimax=1.0, psistep=1.0, xrng=1, yrng=1, \
			stepx = 1.0, stepy = 1.0, psiref = 0., txref = 0., tyref = 0., updown = "up"):
	"""
	Direct 2D alignment within a predefined angular range.  If the range is large the method will be very slow.
	ref - a stack of reference images. 
	updown - one of two keywords:  up, down, indicating which angle to consider, 0, or 180.
	
	Usage of constrains:  Search is around the previous parameters (psiref, txref, tyref), 
	                      but only within ranges specified by (psimax, xrng, yrng).
	
	PAP 01/16/2015
	"""

	from sp_fundamentals import fft, rot_shift2D, ccf, prepi
	from sp_utilities    import peak_search, model_blank, inverse_transform2, compose_transform2
	from sp_alignment    import parabl
	from EMAN2 import Processor
	#print  "  directaligridding1  ",psimax, psistep, xrng, yrng, stepx, stepy, updown
	#print  "IN         %6.2f %6.2f  %6.2f"%(psiref, txref, tyref)

	"""Multiline Comment44"""
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	          #MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44
	#MULTILINEMULTILINEMULTILINE 44

	nr = int(2*psimax/psistep)+1
	nc = nr//2
	if updown == "up" :  reduced_psiref = psiref -  90.0
	else:                reduced_psiref = psiref - 180.0
	#  Limit psi search to within psimax range
	#bnr = max(int(round(reduced_psiref/psistep)),0)
	#enr = min(int(round(reduced_psiref/psistep))+nr,nr)
	bnr = max(int(round(reduced_psiref/psistep)),0)
	enr = min(int(round(reduced_psiref/psistep))+nr,nr)
	bnr = 0
	enr = nr
	
	N = inima.get_ysize()  # assumed image is square, but because it is FT take y.
	#  Window for ccf sampled by gridding
	#   We quietly assume the search range for translations is always much less than the ccf size,
	#     so instead of restricting anything, we will just window out ccf around previous shift locations
	rnx   = int(round(xrng/stepx))
	rny   = int(round(yrng/stepy))
	wnx = 2*rnx + 1
	wny = 2*rny + 1
	w = model_blank( wnx, wny)
	stepxx = 2*stepx
	stepyy = 2*stepy
	nicx = N//2 - 2*txref #  here one would have to add or subtract the old value.
	nicy = N//2 - 2*tyref
	wxc = wnx//2
	wyc = wny//2

	if updown == "up" :
		ima = inima
		#ima = inima.FourInterpol(N, N, 1,0)
		#ima = Processor.EMFourierFilter(ima,params)

	if updown == "down" :
		#  This yields rotation by 180 degrees.  There is no extra shift as the image was padded 2x, so it is even-sized, but two rows are incorrect
		imm = inima.conjg()
		#imm = rot_shift2D(inima,180.0, interpolation_method = 'linear')
		#imm = imm.FourInterpol(N, N, 1,0)
		#imm = Processor.EMFourierFilter(imm,params)

	#fft(ima).write_image('imap.hdf')

	ma1  = -1.e23
	ma2  = -1.e23
	ma3  = -1.e23
	ma4  = -1.e23
	oma2 = [-1.e23, -1.e23, -1.e23]
	oma4 = [-1.e23, -1.e23, -1.e23]
	"""Multiline Comment45"""
	#MULTILINEMULTILINEMULTILINE 45
	#MULTILINEMULTILINEMULTILINE 45
	#MULTILINEMULTILINEMULTILINE 45
	#MULTILINEMULTILINEMULTILINE 45
	#MULTILINEMULTILINEMULTILINE 45
	#print "wnx wny enr-bnr", wnx,wny,enr-bnr
	ccf3dimg = model_blank(wnx, wny, enr-bnr)
	if ( rny == 0 ) : 
		sxprint("rny = 0 return---------------")
		return  0.0, 0.0, 0.0, -1.e23, ccf3dimg     ## do nothing for rny=0 @ming
	for i in range(bnr, enr, 1):
		if updown == "up" :
			c = ccf(ima,ref[i])
			#c.write_image('gcc.hdf')
			#p = peak_search(window2d(c,4*xrng+1,4*yrng+1),5)
			#for q in p: print q
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nicx, iy*stepyy+nicy, 0.0, kb)

			for j in range(wnx):
				for k in range(wny):
					ccf3dimg.set_value_at(j,k,i-bnr,w[j,k])

			pp = peak_search(w)[0]
			#print '  peak   ',i,pp
			#from sys import exit
			#exit()

			px = int(pp[4])
			py = int(pp[5])
			#print '  peak   ',i,pp,px*stepx,py*stepy
			#  did not find a peak, find a maximum location instead
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				#pass
				
				loc = w.calc_max_location()
				PEAKV = w.get_value_at(loc[0],loc[1])
				#print "  Did not find a peak  :",i,wxc, wyc, loc[0]-wxc, loc[1]-wyc, PEAKV
				if(PEAKV>ma2):
					ma2  = PEAKV
					#oma2 = pp+[loc[0]-wxc, loc[1]-wyc, loc[0]-wxc, loc[1]-wyc, PEAKV,(i-nc)*psistep]
					oma2 = pp+[loc[0]-wxc, loc[1]-wyc, loc[0]-wxc, loc[1]-wyc, PEAKV,(i-nc)]
				
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["S %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment46"""
				#MULTILINEMULTILINEMULTILINE 46
					#MULTILINEMULTILINEMULTILINE 46
					#MULTILINEMULTILINEMULTILINE 46
				#MULTILINEMULTILINEMULTILINE 46
				if(PEAKV>ma2):
					ma2  = PEAKV
					#oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]
					oma2 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,i-nc]
					
				#print "wnx, wny, pp, oma2", wnx, wny, pp, oma2	
		if updown == "down" :
			c = ccf(imm,ref[i])
			for iy in range(-rny, rny + 1):
				for ix in range(-rnx, rnx + 1):
					w[ix+rnx,iy+rny] = c.get_pixel_conv7(ix*stepxx+nicx, iy*stepyy+nicy, 0.0, kb)

			for j in range(wnx):
				for k in range(wny):
					 ccf3dimg.set_value_at(j,k,i-bnr,w[j,k])

			pp = peak_search(w)[0]
			px = int(pp[4])
			py = int(pp[5])
			if( pp[0] == 1.0 and px == 0 and py == 0):
				#  No peak!
				#pass
				
				loc = w.calc_max_location()
				PEAKV = w.get_value_at(loc[0],loc[1])
				if(PEAKV>ma4):
					ma4  = PEAKV
					#oma4 = pp+[loc[0], loc[1], loc[0], loc[1], PEAKV,(i-nc)*psistep]
					oma4 = pp+[loc[0]-wxc, loc[1]-wyc, loc[0]-wxc, loc[1]-wyc, PEAKV,(i-nc)]
				
			else:
				ww = model_blank(3,3)
				px = int(pp[1])
				py = int(pp[2])
				for k in range(3):
					for l in range(3):
						ww[k,l] = w[k+px-1,l+py-1]
				XSH, YSH, PEAKV = parabl(ww)
				#print ["R %10.1f"%pp[k] for k in xrange(len(pp))]," %6.2f %6.2f  %6.2f %6.2f %12.2f  %4.1f"%(XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep)
				"""Multiline Comment47"""
				#MULTILINEMULTILINEMULTILINE 47
					#MULTILINEMULTILINEMULTILINE 47
					#MULTILINEMULTILINEMULTILINE 47
				#MULTILINEMULTILINEMULTILINE 47
				if(PEAKV>ma4):
					ma4 = PEAKV
					#oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)*psistep]
					oma4 = pp+[XSH, YSH,int(pp[4])+XSH, int(pp[5])+YSH, PEAKV,(i-nc)]

	if( oma2[-2] > oma4[-2] ):
		peak = oma2[-2]
		if( peak == -1.0e23 ):  return  0.0, 0.0, 0.0, peak, ccf3dimg
	
		"""Multiline Comment48"""
		#MULTILINEMULTILINEMULTILINE 48
		#MULTILINEMULTILINEMULTILINE 48
		#MULTILINEMULTILINEMULTILINE 48
		#MULTILINEMULTILINEMULTILINE 48
		#  The inversion would be needed for 2D alignment.  For 3D, the proper way is to return straight results.
		#nalpha, ntx, nty, mirror = inverse_transform2(oma2[-1], oma2[-4]*stepx, oma2[-3]*stepy, 0)
		nalpha = oma2[-1]
		# ntx    = oma2[-4]*stepx - txref
# 		nty    = oma2[-3]*stepy - tyref
		ntx    = oma2[-4]
		nty    = oma2[-3]
		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	else:
		peak = oma4[-2]
		if( peak == -1.0e23 ):  return  0.0, 0.0, 0.0, peak, ccf3dimg
		#  This is still strange as why I would have to invert here but not for 90 degs.  PAP  01/09/2014
		#print oma3
		#print oma4

		nalpha = oma4[-1]
		ntx    = oma4[-4]
		nty    = oma4[-3]
		
# 		nalpha, ntx, nty, junk = compose_transform2(-oma4[-1], oma4[-4]*stepx - txref,oma4[-3]*stepy - tyref,1.0,180.,0,0,1)
# 		#nalpha = oma4[-1] + 180.0
# 		#ntx    = oma4[-4]*stepx
# 		#nty    = oma4[-3]*stepy
# 		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
# 		nalpha, ntx, nty, mirror = inverse_transform2(nalpha, ntx, nty, 0)
# 		#print  "        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
# 	#print  "OUT        %6.2f %6.2f  %6.2f"%(nalpha, ntx, nty)
	
	return  nalpha, ntx, nty, peak, ccf3dimg

def alignment3Dsnake(partition, snakeknots, nsegs, initialori, ctx, psistep, stepx, stepy, txref, tyref, nc, rnx, rny, updown = "up"):
	from scipy import interpolate
	from sp_utilities    import inverse_transform2, compose_transform2
	#1. setting basis parameters for b-spline
	#patitions[ivol] = 2        ##only for test. should be removed later.@ming
	pt = partition
	nknots=[0]*pt
	for ipt in range(pt):
		if snakeknots>1: nknots[ipt]  =  min(snakeknots-1,nsegs//2+1)   ##does not include the right end knots.
		else: nknots[ipt]  = nsegs//2+1
		#nknots1 = 4
	#mknots = 1	
	sxprint("begin snake refine....")
	##2. get initial b-splines coefficients for initial alignment parameters(snake).
	###2.1 for b-spline fitting of tttt.
	#nsegs = seg_end-seg_start
	TCK=[] 
	for repd in range(3):
		T=[0.0]      #b-spline knots.
		U=[]	  #sampling points.	
		AT=[]	  #values at U.	
		W=[]				
		for ipt in range(pt):
			T +=[0.0]*nknots[ipt]
		U=[0.0]*nsegs
		AT=[0.0]*nsegs
		W=[0.0]*nsegs
		
		
		for i in range(0,len(T)):
			T[i] = i*(nsegs-1)*1.0/(len(T)-1)
			
		for i in range(0,nsegs):
			U[i] = i
			AT[i]= initialori[i][repd]
			W[i] = 1.0

		out_file = open("T%d.txt"%repd, "w")
		out_file1 = open("AT%d.txt"%repd, "w")
		out_file2 = open("W%d.txt"%repd, "w")
		for i in range(len(T)):
			out_file.write( "%f\n" % (T[i]) )
		for i in range(len(AT)):	
			out_file1.write( "%f\n" % (AT[i]) )
			out_file2.write( "%f\n" % (W[i]) )
		out_file.close()
		out_file1.close()
		out_file2.close()	 

		tck=interpolate.splrep(U,AT,W, t=T[1:len(T)-1], k=3,s=0)
		TCK.append(tck)
		#print tck	

	angrot0=list(TCK[0][1]) 	
	sx0    =list(TCK[1][1])
	sy0    =list(TCK[2][1])
	
	angrot=angrot0
	sx    =sx0
	sy    =sy0
	
	##3. refine snake's b-spline coefficients using amoeba. added@ming
	from sp_utilities import amoeba
	params0 = angrot0+sx0+sy0
	params  = angrot+sx+sy
	ftol = 1.e-8
	xtol = 1.e-8
	maxi = 5000
	scale = [nc*1.0]*len(TCK[0][1])+[rnx*1.0]*len(TCK[1][1])+[rny*1.0]*len(TCK[2][1])
	sxprint("begin amoeba refine... number of segments=%d, number of amoeba parameters for x-shift =%d"%(nsegs, len(sx))) #, params0
	params,fval, numit=amoeba(params, scale, flexhelicalali, ftol, xtol, maxi, [ctx,params0, 0.0, TCK, nsegs])
	sxprint("after amoeba refine, iter_num=%d"%numit)#, params
	##4. get alignment parameters from refined b-spline coefficients.
	import numpy as np        
	pang = np.array(params[0:len(TCK[0][1])])
	px   = np.array(params[len(TCK[0][1]):len(TCK[0][1])+len(TCK[1][1])])
	py   = np.array(params[len(TCK[0][1])+len(TCK[1][1]):len(TCK[0][1])+len(TCK[1][1])+len(TCK[2][1])])
       
	tckang = (TCK[0][0], pang,TCK[0][2])
	tckx   = (TCK[1][0], px,TCK[1][2]) 
	tcky   = (TCK[2][0], py,TCK[2][2])
	
	from scipy import interpolate
	#print "lambw", lambw
	sx_sum=0.0
	
	u=[i for i in range(nsegs)]
	valang = interpolate.splev(u, tckang, der=0, ext=0)
	valx = interpolate.splev(u, tckx, der=0, ext=0)
	valy = interpolate.splev(u, tcky, der=0, ext=0)	
	neworient = [[0.0, 0.0, 0.0] for i in range(nsegs)]
	for im in range(nsegs):	
		if updown == "up" :				
			neworient[im][0] = valang[im]*psistep
			neworient[im][1] =  valx[im]*stepx - txref[im]
			neworient[im][2] =  valy[im]*stepy - tyref[im]	
		if updown == "down" :
			nalpha, ntx, nty, junk = compose_transform2(-valang[im]*psistep, valx[im]*stepx - txref[im],valy[im]*stepy - tyref[im],1.0,180.,0,0,1)
			nalpha, ntx, nty, mirror = inverse_transform2(nalpha, ntx, nty, 0)
			neworient[im][0] = nalpha
			neworient[im][1] = ntx
			neworient[im][2] = nty			
	return neworient

def flexhelicalali(params,data):
	sccf    = data[0]
	params0 = data[1]
	lambw   = data[2]
	TCK     = data[3]
	nsegs   = data[4]

	import numpy as np  
	pang = np.array(params[0:len(TCK[0][1])])
	px   = np.array(params[len(TCK[0][1]):len(TCK[0][1])+len(TCK[1][1])])
	py   = np.array(params[len(TCK[0][1])+len(TCK[1][1]):len(TCK[0][1])+len(TCK[1][1])+len(TCK[2][1])])
	      
	    
	tckang = (TCK[0][0], pang,TCK[0][2])
	tckx   = (TCK[1][0], px,TCK[1][2]) 
	tcky   = (TCK[2][0], py,TCK[2][2])
	

	
	from scipy import interpolate
	#print "lambw", lambw
	sx_sum=0.0
		
	u=[i for i in range(nsegs)]
	valang = interpolate.splev(u, tckang, der=0, ext=0)
	valx = interpolate.splev(u, tckx, der=0, ext=0)
	valy = interpolate.splev(u, tcky, der=0, ext=0)
	for id in range(nsegs):
		nx = sccf[id].get_xsize()
		ny = sccf[id].get_ysize()
		na = sccf[id].get_zsize()
	
		#print "nx ny nz, size(sccf), type(sccf[0]) get_value_at", nx, ny,na, len(sccf), type(sccf[1]), sccf[1].get_value_at(0,0,0)
		nxc=nx//2
		nyc=ny//2
		nac=na//2
	
		al = valang[id]  #interpolate.splev([id-nperiod], tck, der=0, ext=0)
		xl = valx[id]
		yl = valy[id]
		
		#print "xl=%f"%xl
		al = al+nac
		ial = int(al)
		dal = al - ial
		
		xl = xl+nxc
		ixl = int(xl)
		dxl = xl - ixl

		yl = yl+nyc
		iyl = int(yl)
		dyl = yl - iyl
				
		#print "sx_sum, xl, ixl, dxl", sx_sum, xl,ixl,dxl
		# if ixl < 0:
# 			print "ixl=%d xl=%f params[id]=%f"%(ixl,xl,params[id])
		#print "ix iy ia", ixl, iyl, ial
		if ial < 0 or ial >= na-1 or ixl < 0 or ixl >= nx-1 or iyl < 0 or iyl >= ny-1:
			return -1.0e23
			
		# if ial < 0:
# 			ial = 0
# 			dal = 0
# 		if ial >= na-1:
# 			ial = na-2
# 			dal = 0
# 		if ixl < 0:
# 			ixl = 0
# 			dxl = 0
# 		if ixl >= nx-1:
# 			ixl = nx-2
# 			dxl = 0
# 		if iyl < 0:
# 			iyl = 0
# 			dyl = 0	
# 		if iyl >= ny-1:
# 			iyl = ny-2
# 			dyl = 0		
		c00 = (1.0-dxl)*sccf[id].get_value_at(ixl,iyl,ial)+dxl*sccf[id].get_value_at(ixl+1,iyl,ial)
		c10 = (1.0-dxl)*sccf[id].get_value_at(ixl,iyl+1,ial)+dxl*sccf[id].get_value_at(ixl+1,iyl+1,ial)
		c01 = (1.0-dxl)*sccf[id].get_value_at(ixl,iyl,ial+1)+dxl*sccf[id].get_value_at(ixl+1,iyl,ial+1)
		c11 = (1.0-dxl)*sccf[id].get_value_at(ixl,iyl+1,ial+1)+dxl*sccf[id].get_value_at(ixl+1,iyl+1,ial+1)
		
		c0 = (1-dyl)*c00 + dyl*c10
		c1 = (1-dyl)*c01 + dyl*c11
		
		c  = (1-dal)*c0 + dal*c1
			
		sx_sum += c
	#print "part 1", sx_sum
	# part2_sum=0
# 	for id in xrange(sccfn):
# 		part2_sum += lambw*(params0[id]-params[id])**2
# 	#print "part 2", part2_sum
# 	sx_sum -= part2_sum
	return sx_sum
	
def ali_nvol(v, mask):
	from sp_alignment    import alivol_mask_getref, alivol_mask
	from sp_statistics   import ave_var
	from sp_utilities    import set_params3D, get_params3D ,compose_transform3

	from sp_fundamentals import rot_shift3D
	ocrit = 1.0e20
	gogo = True
	niter = 0
	for l in range(len(v)):  set_params3D( v[l],   (0.0,0.0,0.0,0.0,0.0,0.0,0,1.0))
	while(gogo):
		ave,var = ave_var(v)
		p = Util.infomask(var, mask, True)
		crit = p[1]
		if((crit-ocrit)/(crit+ocrit)/2.0 > -1.0e-2 or niter > 10):  gogo = False
		niter += 1
		ocrit = crit
		ref = alivol_mask_getref(ave, mask)
		for l in range(len(v)):
			ophi,otht,opsi,os3x,os3y,os3z,dum, dum = get_params3D(v[l])
			vor = rot_shift3D(v[l], ophi,otht,opsi,os3x,os3y,os3z )
			phi,tht,psi,s3x,s3y,s3z = alivol_mask(vor, ref, mask)
			phi,tht,psi,s3x,s3y,s3z,scale = compose_transform3(phi,tht,psi,s3x,s3y,s3z,1.0,ophi,otht,opsi,os3x,os3y,os3z,1.0)
			set_params3D(v[l],  (phi,tht,psi,s3x,s3y,s3z,0,1.0))
			#print "final align3d params: %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f" % (phi,tht,psi,s3x,s3y,s3z)
	for l in range(len(v)):
		ophi,otht,opsi,os3x,os3y,os3z,dum,dum = get_params3D(v[l])
		sxprint(l,ophi,otht,opsi,os3x,os3y,os3z)
		v[l] = rot_shift3D( v[l], ophi,otht,opsi,os3x,os3y,os3z )
		v[l].del_attr("xform.align3d")
	return v

def alivol_mask_getref( v, mask ):
	from sp_utilities import set_params3D
	v50S_ref = v.copy()
	v50S_ref *= mask
	cnt = v50S_ref.phase_cog()
	set_params3D( v50S_ref, (0.0,0.0,0.0,-cnt[0],-cnt[1],-cnt[2],0,1.0) )
	return v50S_ref

def alivol_mask( v, vref, mask ):
	from sp_utilities    import set_params3D, get_params3D,compose_transform3
	from sp_applications import ali_vol_shift, ali_vol_rotate
	v50S_i = v.copy()
	v50S_i *= mask
	cnt = v50S_i.phase_cog()
	set_params3D( v50S_i,   (0.0,0.0,0.0,-cnt[0],-cnt[1],-cnt[2],0,1.0) )

	v50S_i = ali_vol_shift( v50S_i, vref, 1.0 )
	v50S_i = ali_vol_rotate(v50S_i, vref, 5.0 )
	v50S_i = ali_vol_shift( v50S_i, vref, 0.5 )
	v50S_i = ali_vol_rotate(v50S_i, vref, 1.0 )
	phi,tht,psi,s3x,s3y,s3z,mirror,scale = get_params3D( v50S_i )
	dun,dum,dum,cnx,cny,cnz,mirror,scale = get_params3D( vref )
	phi,tht,psi,s3x,s3y,s3z,scale = compose_transform3(phi,tht,psi,s3x,s3y,s3z,1.0,0.0,0.0,0.0,-cnx,-cny,-cnz,1.0)
	return phi,tht,psi,s3x,s3y,s3z

def ali_mvol(v, mask):
	from sp_alignment    import alivol_m
	from sp_statistics   import ave_var
	from sp_utilities    import set_params3D, get_params3D ,compose_transform3

	from sp_fundamentals import rot_shift3D
	ocrit = 1.0e20
	gogo = True
	niter = 0
	for l in range(len(v)):  set_params3D( v[l],   (0.0,0.0,0.0,0.0,0.0,0.0,0,1.0))
	while(gogo):
		ave,var = ave_var(v)
		set_params3D( ave,   (0.0,0.0,0.0,0.0,0.0,0.0,0,1.0))
		p = Util.infomask(var, mask, True)
		crit = p[1]
		if((crit-ocrit)/(crit+ocrit)/2.0 > -1.0e-2 or niter > 10):  gogo = False
		niter += 1
		ocrit = crit
		ave *= mask
		for l in range(len(v)):
			ophi,otht,opsi,os3x,os3y,os3z,dum, dum = get_params3D(v[l])
			vor = rot_shift3D(v[l], ophi,otht,opsi,os3x,os3y,os3z )
			phi,tht,psi,s3x,s3y,s3z = alivol_m(vor, ave, mask)
			phi,tht,psi,s3x,s3y,s3z,scale = compose_transform3(phi,tht,psi,s3x,s3y,s3z,1.0,ophi,otht,opsi,os3x,os3y,os3z,1.0)
			set_params3D(v[l],  (phi,tht,psi,s3x,s3y,s3z,0,1.0))
			#print "final align3d params: %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f" % (phi,tht,psi,s3x,s3y,s3z)
	for l in range(len(v)):
		ophi,otht,opsi,os3x,os3y,os3z,dum,dum = get_params3D(v[l])
		sxprint(i,ophi,otht,opsi,os3x,os3y,os3z)
		v[l] = rot_shift3D( v[l], ophi,otht,opsi,os3x,os3y,os3z )
		v[l].del_attr("xform.align3d")
	return v

def alivol_m( v, vref, mask ):
	from sp_utilities    import set_params3D, get_params3D,compose_transform3
	from sp_applications import ali_vol_shift, ali_vol_rotate
	vola = v.copy()
	vola *= mask
	set_params3D( vola,   (0.0,0.0,0.0,0.0,0.0,0.0,0,1.0) )

	vola = ali_vol_shift( vola, vref, 1.0 )
	vola = ali_vol_rotate(vola, vref, 5.0 )
	vola = ali_vol_shift( vola, vref, 0.5 )
	vola = ali_vol_rotate(vola, vref, 1.0 )
	phi,tht,psi,s3x,s3y,s3z,mirror,scale = get_params3D( vola )
	return phi,tht,psi,s3x,s3y,s3z


# =================== SHC
"""Multiline Comment49"""
#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49

	#MULTILINEMULTILINEMULTILINE 49

	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49

	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49

	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49

	#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
				#MULTILINEMULTILINEMULTILINE 49
				#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
	#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49

		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
				#MULTILINEMULTILINEMULTILINE 49
				#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
			#MULTILINEMULTILINEMULTILINE 49
		#MULTILINEMULTILINEMULTILINE 49
#MULTILINEMULTILINEMULTILINE 49

def shc(data, refrings, list_of_reference_angles, numr, xrng, yrng, step, an = -1.0, sym = "c1", finfo=None):
	from sp_alignment import search_range
	from math         import cos, sin, degrees, radians
	from EMAN2 import Vec2f

	number_of_checked_refs = 0

	mode = "F"
	nx   = data.get_xsize()
	ny   = data.get_ysize()
	#  center is in SPIDER convention
	cnx  = nx//2 + 1
	cny  = ny//2 + 1

	if( an>= 0.0):  ant = cos(radians(an))
	else:           ant = -1.0
	#phi, theta, psi, sxo, syo = get_params_proj(data)
	t1 = data.get_attr("xform.projection")
	dp = t1.get_params("spider")
	ou = numr[-3]
	sxi = round(-dp["tx"],2)
	syi = round(-dp["ty"],2)
	txrng = search_range(nx, ou, sxi, xrng)
	tyrng = search_range(ny, ou, syi, yrng)

	if finfo:
		finfo.write("Old parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(dp["phi"], dp["theta"], dp["psi"], -dp["tx"], -dp["ty"]))
		finfo.flush()
		from sp_utilities import get_params_proj
		z1,z2,z3,z4,z5 = get_params_proj(data, "xform.anchor")
		finfo.write("Anc parameters: %9.4f %9.4f %9.4f %9.4f %9.4f\n"%(z1,z2,z3,-z4,-z5))
		finfo.flush()

	previousmax = data.get_attr("previousmax")
	[ang, sxs, sys, mirror, iref, peak, checked_refs] = Util.shc(data, refrings, list_of_reference_angles, txrng, tyrng, step, ant, mode, numr, cnx-sxi, cny-syi, sym)
	iref=int(iref)
	number_of_checked_refs += int(checked_refs)
	if peak <= previousmax:
		return -1.0e23, 0.0, number_of_checked_refs, -1
		"""Multiline Comment50"""
		#MULTILINEMULTILINEMULTILINE 50
		#MULTILINEMULTILINEMULTILINE 50
		#MULTILINEMULTILINEMULTILINE 50
		#MULTILINEMULTILINEMULTILINE 50
		#MULTILINEMULTILINEMULTILINE 50
			#MULTILINEMULTILINEMULTILINE 50
			#MULTILINEMULTILINEMULTILINE 50
				#MULTILINEMULTILINEMULTILINE 50
				#MULTILINEMULTILINEMULTILINE 50
		#MULTILINEMULTILINEMULTILINE 50
			#MULTILINEMULTILINEMULTILINE 50
		#MULTILINEMULTILINEMULTILINE 50
	else:
		# The ormqip returns parameters such that the transformation is applied first, the mirror operation second.
		# What that means is that one has to change the the Eulerian angles so they point into mirrored direction: phi+180, 180-theta, 180-psi
		if  mirror:
			phi   = (refrings[iref].get_attr("phi")+540.0)%360.0
			theta = 180.0-refrings[iref].get_attr("theta")
			psi   = (540.0-refrings[iref].get_attr("psi")-ang)%360.0
		else:
			phi   = refrings[iref].get_attr("phi")
			theta = refrings[iref].get_attr("theta")
			psi   = (360.0+refrings[iref].get_attr("psi")-ang)%360.0
		s2x   = sxs + sxi
		s2y   = sys + syi

		#set_params_proj(data, [phi, theta, psi, s2x, s2y])
		t2 = Transform({"type":"spider","phi":phi,"theta":theta,"psi":psi})
		t2.set_trans(Vec2f(-s2x, -s2y))
		data.set_attr("xform.projection", t2)
		data.set_attr("previousmax", peak)
		#  Find the pixel error that is minimum over symmetry transformations
		from sp_pixel_error import max_3D_pixel_error
		if(sym == "nomirror" or sym == "c1"):
			pixel_error = max_3D_pixel_error(t1, t2, numr[-3])
		else:		
			ts = t2.get_sym_proj(sym)
			# only do it if it is not c1
			pixel_error = +1.0e23
			for ut in ts:
				# we do not care which position minimizes the error
				pixel_error = min(max_3D_pixel_error(t1, ut, numr[-3]), pixel_error)
		if finfo:
			finfo.write( "New parameters: %9.4f %9.4f %9.4f %9.4f %9.4f %10.5f  %11.3e\n\n" %(phi, theta, psi, s2x, s2y, peak, pixel_error))
			finfo.flush()
		return peak, pixel_error, number_of_checked_refs, iref


# parameters: list of (all) projections | reference volume is optional, if provided might be shrank| ...
#  This functions centers projections using an self-correlation-based exhaustive search
#  It only returns shifts
#  Data is assumed to be shrunk and CTF-applied
#  The input volume is assumed to be shrunk but not filtered, if not provided, it will be reconstructed and shrunk
#  We apply ali3d_options.fl
def center_projections_3D(data, ref_vol = None, ali3d_options = None, onx = -1, shrinkage = 1.0, \
							mpi_comm = None, myid = 0, main_node = 0, log = None ):

	from sp_alignment       import Numrinit, prepare_refrings, proj_ali_incore,  proj_ali_incore_local, shc
	from sp_utilities       import bcast_number_to_all, bcast_EMData_to_all, model_blank
	from sp_utilities       import get_im, file_type, model_circle, get_input_from_string, get_params_proj, set_params_proj
	from mpi             import mpi_bcast, mpi_comm_size, mpi_comm_rank, MPI_FLOAT, MPI_COMM_WORLD, mpi_barrier, mpi_reduce, MPI_INT, MPI_SUM
	from sp_projection      import prep_vol
	from sp_statistics      import hist_list
	from sp_utilities		 import params_2D_3D
	from sp_applications    import MPI_start_end
	from sp_filter          import filt_ctf
	from sp_global_def      import Util
	from sp_fundamentals    import resample, fshift
	from sp_multi_shc       import do_volume, shc_multi
	from EMAN2           import EMUtil, EMData
	import types
	from time            import time

	ir     = ali3d_options.ir
	rs     = ali3d_options.rs
	ou     = ali3d_options.ou
	xr     = ali3d_options.xr
	yr     = ali3d_options.yr
	ts     = ali3d_options.ts
	#an     = ali3d_options.an
	sym    = ali3d_options.sym
	sym    = sym[0].lower() + sym[1:]
	delta  = ali3d_options.delta
	#center = ali3d_options.center
	CTF    = ali3d_options.CTF
	ref_a  = ali3d_options.ref_a
	#maskfile = ali3d_options.mask3D

	if mpi_comm == None:
		mpi_comm = MPI_COMM_WORLD

	if log == None:
		from sp_logger import Logger
		log = Logger()

	if myid == main_node:
		log.add("Start 3D centering")

	xrng        = int(get_input_from_string(xr)[0])
	if  yr == "-1":  yrng = xrng
	else          :  yrng = int(get_input_from_string(yr)[0])
	step        = get_input_from_string(ts)
	delta       = get_input_from_string(delta)
	lstp = 1 #min(len(xrng), len(yrng), len(step), len(delta))
	"""Multiline Comment51"""
	#MULTILINEMULTILINEMULTILINE 51
		#MULTILINEMULTILINEMULTILINE 51
	#MULTILINEMULTILINEMULTILINE 51
		#MULTILINEMULTILINEMULTILINE 51
	#MULTILINEMULTILINEMULTILINE 51
	first_ring  = int(ir)
	rstep       = int(rs)
	last_ring   = int(ou)

	if myid == 0:
		finfo = None
		"""Multiline Comment52"""
		#MULTILINEMULTILINEMULTILINE 52
		#MULTILINEMULTILINEMULTILINE 52
		#MULTILINEMULTILINEMULTILINE 52
		#MULTILINEMULTILINEMULTILINE 52
		#MULTILINEMULTILINEMULTILINE 52
	else:
		finfo = None

	#  Data is already shrank
	nx = data[0].get_xsize()

	if last_ring < 0:	last_ring = nx//2 - 1
	if(shrinkage < 1.0):
		#  redefines parameters
		ali3d_options.ou = last_ring
		ali3d_options.ir = first_ring
	numr	= Numrinit(first_ring, last_ring, rstep, "H")
	if(xrng == -1): xrng = nx//2 - last_ring - 1
	#else:  xrng = int(xrng*shrinkage + 0.5)  #  XRNG HAS TO REFER TO SHRANK DATA
	yrng = xrng

	if myid == main_node:
		start_time = time()

	#  Read	template volume if provided or reconstruct it
	if ref_vol:
		if type(ref_vol) is bytes:
			if myid == main_node:
				vol = get_im(ref_vol)
				i = vol.get_xsize()
				if( shrinkage < 1.0 ):
					if( i != nx ):
						vol = resample(vol, shrinkage)
			else:
				vol = model_blank(nx, nx, nx)
		else:
			if myid == main_node:
				i = ref_vol.get_xsize()
				if( shrinkage < 1.0 ):
					if( i != nx ):
						vol = resample(ref_vol, shrinkage)
				else:
					vol = ref_vol.copy()
			else:
				vol = model_blank(nx, nx, nx)
		bcast_EMData_to_all(vol, myid, main_node)
		del ref_vol
		vol = do_volume(vol, ali3d_options, 0, mpi_comm)
	else:
		vol = do_volume(data, ali3d_options, 0, mpi_comm)

	N_step = 0
	# log
	if myid == main_node:
		log.add("Dimensions used (nx, onx, first_ring, last_ring, shrinkage)  %5d     %5d     %5d     %5d     %6.3f\n"%(nx, onx, first_ring, last_ring, shrinkage))
		log.add("Reference 3D reconstruction time = %f\n"%(time()-start_time))
		log.add("Delta = %5.2f, xrange = %5.2f, yrange = %5.2f, step = %5.2f"%(delta[N_step], xrng, yrng, step[N_step]))
		start_time = time()


		log.add("Inner radius                : %i"%(first_ring))
		log.add("Outer radius                : %i"%(last_ring))
		log.add("Ring step                   : %i"%(rstep))
		log.add("X search range              : %s"%(xrng))
		log.add("Y search range              : %s"%(yrng))
		log.add("Translational step          : %s"%(step))
		log.add("Angular step                : %s\n"%(delta))

	#=========================================================================
	# build references
	volft, kb = prep_vol(vol)
	refrings, ftprojections = prepare_refrings_projections(volft, kb, nx, delta[N_step], ref_a, sym, "H", numr, MPI=mpi_comm, phiEqpsi = "Zero")
	#from fundamentals import fft
	#for i in xrange(len(ftprojections)):  fft(ftprojections[i]).write_image("template%03d.hdf"%myid, i)
	#MPI=mpi_comm, phiEqpsi = "Zero")
	#MPI=False, phiEqpsi = "Zero")
	del volft, kb
	#=========================================================================

	if myid == main_node:
		log.add("Time to prepare rings: %f\n" % (time()-start_time))
		start_time = time()
	# alignment
	nima = len(data)
	params = [None]*nima
	for im in range(nima):
		newsx,newsy,iref,talpha,tmirr,totpeak = multalign2d_scf(data[im], refrings, ftprojections, numr, xrng, yrng, last_ring)
		dummy, dummy, talpha, newsx, newsy = params_2D_3D(talpha, newsx, newsy, tmirr)
		params[im] = [talpha, newsx/shrinkage, newsy/shrinkage, iref]

	#=========================================================================
	mpi_barrier(mpi_comm)
	if myid == main_node:
		#print  data[0].get_attr_dict()
		log.add("Time of alignment = %f\n"%(time()-start_time))
		start_time = time()
		log.add("End 3D centering")
	return params



def search_range(n, radius, shift, range, location = ""):
	"""
		Find permissible ranges for translational searches by resampling into polar coordinates
		n - image size; radius - particle radius, the circle has to fit into the square image;
		shift - current particle shift; range - desired maximum range search
		Output: a list of two elements:
		  left range (positive)
		  right range
		NOTE - ranges are with respect to the point n//2+1-shift within image (in 3D)
	"""
	cn = n//2 +1
	ql = cn+shift-radius -2   # lower end is positive
	qe = n - cn-shift-radius    # upper end
	if( ql < 0 or qe < 0 ):
		ERROR("Shift of particle too large, results may be incorrect:  %4d   %3d   %f  %f  %f  %f  %f"%(n, cn, radius, shift, range, ql, qe),"search_range  "+location,0)
		ql = max(ql,0)
		qe = max(qe,0)
	# ???for mysterious reasons it has to be this way as C code changes the order of searches.
	return  [ min( qe, range), min(ql, range) ]


def generate_list_of_reference_angles_for_search(input_angles, sym):
	"""
	  Generate full set of reference angles, including mirror and symmetry related
	  from a unique subrange generated by even_angles and stored in refrings.
	  Input - input_angles [[angles],[angles]]
	  Output - [[angles], [angles]] (no shifts)
			Blocks - [[basic][mirrored basic]] [[basic sym1][mirrored basic sym1]] ...
	"""
	from EMAN2 import Transform
	t2   = Transform()
	nsym = t2.get_nsym(sym)

	original_number_of_angles = len(input_angles)
	# original_number_of_angles is the same as the number of refrings
	
	list_of_reference_angles = [None]*original_number_of_angles
	for i in range(original_number_of_angles): 
		list_of_reference_angles[i] = [input_angles[i][0],input_angles[i][1], 0]

	#  add mirror related
	list_of_reference_angles += [[0.0,0.0,0.0] for i in range(original_number_of_angles)]
	for i in range(original_number_of_angles):
		list_of_reference_angles[i+original_number_of_angles][0] = (list_of_reference_angles[i][0]+180.0)%360.0
		list_of_reference_angles[i+original_number_of_angles][1] = 180.0-list_of_reference_angles[i][1]
		list_of_reference_angles[i+original_number_of_angles][2] =  list_of_reference_angles[i][2]

	#  add symmetry related
	if(nsym>1):	
		number_of_angles_original_and_mirror = len(list_of_reference_angles)
		for l in range(1,nsym):
			list_of_reference_angles += [[0.0,0.0,0.0] for i in range(number_of_angles_original_and_mirror)]

		for i in range(number_of_angles_original_and_mirror):
			t2 = Transform({"type":"spider","phi":list_of_reference_angles[i][0],"theta":list_of_reference_angles[i][1]})
			ts = t2.get_sym_proj(sym)
			for ll in range(1,nsym,1):
				d = ts[ll].get_params("spider")
				list_of_reference_angles[i+ll*number_of_angles_original_and_mirror][0] = round(d["phi"],5)
				list_of_reference_angles[i+ll*number_of_angles_original_and_mirror][1] = round(d["theta"],5)
				list_of_reference_angles[i+ll*number_of_angles_original_and_mirror][2] = round(d["psi"],5)  #  Not needed?

	return list_of_reference_angles


def reduce_indices_so_that_angles_map_only_to_asymmetrix_unit_and_keep_mirror_info(all_refs_angles, angle_index__to__all_refs_angles_within_asymmetric_unit_plus_mirror_and_symmetries):

	index_of_base_refangles_reduced_to_asymetric_unit_with_mirror_info = \
		list(set((x%len(all_refs_angles), (x/len(all_refs_angles))%2) for x in angle_index__to__all_refs_angles_within_asymmetric_unit_plus_mirror_and_symmetries))

	#need to eliminate duplicates, but keep the mirror information, so sort by index and then cummulate multiple indices into only one that has the mirror info from all, sorted and filtered through a set
	
	index_of_base_refangles_reduced_to_asymetric_unit_with_mirror_info = sorted(index_of_base_refangles_reduced_to_asymetric_unit_with_mirror_info, key = lambda x: x[0])
	
	previous_angle_index = -1
	filtered_refrings_index_angles_with_mirror_info = []
	all_refs_angles_reduced = []
	counter = -1
	for index_of_angle_and_mirror_info in index_of_base_refangles_reduced_to_asymetric_unit_with_mirror_info:
		if previous_angle_index == index_of_angle_and_mirror_info[0]:
			new_mirror_info = sorted(list(set(filtered_refrings_index_angles_with_mirror_info[counter][1:] + [index_of_angle_and_mirror_info[1]])))
			filtered_refrings_index_angles_with_mirror_info[counter]= [index_of_angle_and_mirror_info[0]]
			filtered_refrings_index_angles_with_mirror_info[counter].extend(new_mirror_info)
		else:
			counter += 1
			all_refs_angles_reduced.append(all_refs_angles[index_of_angle_and_mirror_info[0]])
			filtered_refrings_index_angles_with_mirror_info.append([index_of_angle_and_mirror_info[0], index_of_angle_and_mirror_info[1]])
		previous_angle_index = index_of_angle_and_mirror_info[0]

	for info in filtered_refrings_index_angles_with_mirror_info:
		all_refs_angles_reduced[counter].extend(info[1:])

	return all_refs_angles_reduced
	# filtered_refrings_index_angles_with_mirror_info


########################################################################################################################
### start: code that supports cone implementation 
"""Multiline Comment53"""
#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53


#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53


	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53


	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53


#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53


		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53


		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53


			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53

		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53

			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53

					#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53
						#MULTILINEMULTILINEMULTILINE 53
						#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53
						#MULTILINEMULTILINEMULTILINE 53

					#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53

					#MULTILINEMULTILINEMULTILINE 53

					#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53

					#MULTILINEMULTILINEMULTILINE 53

					#MULTILINEMULTILINEMULTILINE 53

				#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53




#MULTILINEMULTILINEMULTILINE 53
#MULTILINEMULTILINEMULTILINE 53

#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
				#MULTILINEMULTILINEMULTILINE 53
					#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53

		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53


		#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53
			#MULTILINEMULTILINEMULTILINE 53

	#MULTILINEMULTILINEMULTILINE 53
		#MULTILINEMULTILINEMULTILINE 53
#MULTILINEMULTILINEMULTILINE 53
#  06-12-14 code lifted
"""Multiline Comment54"""
#MULTILINEMULTILINEMULTILINE 54
						#MULTILINEMULTILINEMULTILINE 54
						#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54

	#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54

	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54

	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
	#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54

		#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54
			#MULTILINEMULTILINEMULTILINE 54

		#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54
		#MULTILINEMULTILINEMULTILINE 54

	#MULTILINEMULTILINEMULTILINE 54
#MULTILINEMULTILINEMULTILINE 54


from builtins import range
from sp_global_def import *
