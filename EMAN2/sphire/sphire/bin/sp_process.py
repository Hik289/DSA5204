#!/usr/bin/env python
from __future__ import print_function
from __future__ import division
from past.utils import old_div
#
# Author: Markus Stabrin 2019 (markus.stabrin@mpi-dortmund.mpg.de)
# Author: Fabian Schoenfeld 2019 (fabian.schoenfeld@mpi-dortmund.mpg.de)
# Author: Thorsten Wagner 2019 (thorsten.wagner@mpi-dortmund.mpg.de)
# Author: Toshio Moriya 2019 (toshio.moriya@kek.jp)
# Author: Pawel A. Penczek (pawel.a.penczek@uth.tmc.edu)
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  2111-1307 USA
#
#

import EMAN2
import EMAN2_cppwrap
import EMAN2jsondb
import glob
import matplotlib
import matplotlib.pylab
import numpy
import optparse
import os
import random
from ..libpy import sp_filter
from ..libpy import sp_fundamentals
from ..libpy import sp_global_def
from ..libpy import sp_logger
from ..libpy import sp_morphology
from ..libpy import sp_projection
from ..libpy import sp_statistics
from ..libpy import sp_utilities
import sys
import time
import subprocess
from builtins import range

"""
 rotate_shift_params(paramsin, transf) has been moved to utilities
"""

"""
	Traveling salesman problem solved using Simulated Annealing.
"""
# from scipy import *
# from pylab import *


"""Multiline Comment0"""


def run():
    progname = os.path.basename(sys.argv[0])
    usage = (
        progname
        + """ [options] <inputfile> <outputfile>

	Generic 2-D image processing programs.

	Functionality:

	1.  Phase flip a stack of images and write output to new file:
		sxprocess.py input_stack.hdf output_stack.hdf --phase_flip

	2.  Resample (decimate or interpolate up) images (2D or 3D) in a stack to change the pixel size.
	    The window size will change accordingly.
		sxprocess input.hdf output.hdf  --changesize --ratio=0.5

	3.  Compute average power spectrum of a stack of 2D images with optional mask and/or padding (option wn) with zeroes or a 3-D volume.
		sxprocess.py input_stack.hdf powerspectrum.hdf  <mask.hdf> --pw [--wn=1024]

	4.  Generate a stack of projections bdb:data and micrographs with prefix mic (i.e., mic0.hdf, mic1.hdf etc) from structure input_structure.hdf, with CTF applied to both projections and micrographs:
		sxprocess.py input_structure.hdf data mic --generate_projections format="bdb":apix=5.2:CTF=True:boxsize=64

    5.  Retrieve original image numbers in the selected ISAC group (here group 12 from generation 3):
    	sxprocess.py  bdb:test3 class_averages_generation_3.hdf  list3_12.txt --isacgroup=12 --params=originalid

    6.  Retrieve original image numbers of images listed in ISAC output stack of averages:
    	sxprocess.py  select1.hdf  ohk.txt

    7.  Adjust rotationally averaged power spectrum of an image to that of a reference image or a reference 1D power spectrum stored in an ASCII file.
    	Optionally use a tangent low-pass filter.  Also works for a stack of images, in which case the output is also a stack.
    	sxprocess.py  vol.hdf ref.hdf  avol.hdf < 0.25 0.2> --adjpw
   	 	sxprocess.py  vol.hdf pw.txt   avol.hdf < 0.25 0.2> --adjpw

    8.  Generate a 1D rotationally averaged power spectrum of an image.
		sxprocess.py  vol.hdf --rotpw=rotpw.txt
    	# Output will contain three columns:
       (1) rotationally averaged power spectrum
       (2) logarithm of the rotationally averaged power spectrum
       (3) integer line number (from zero to approximately to half the image size)

    9.  Apply 3D transformation (rotation and/or shift) to a set of orientation parameters associated with projection data.
    	sxprocess.py  --transformparams=phi,theta,psi,tx,ty,tz      input.txt  output.txt
    	The output file is then imported and 3D transformed volume computed:
    	sxheader.py  bdb:p  --params=xform.projection  --import=output.txt
    	mpirun -np 2 sxrecons3d_n.py  bdb:p tvol.hdf --MPI
    	The reconstructed volume is in the position of the volume computed using the input.txt parameters and then
    	transformed with rot_shift3D(vol, phi,theta,psi,tx,ty,tz)

   10.  Import ctf parameters from the output of sxcter into windowed particle headers.
	    There are three possible input files formats:  (1) all particles are in one stack, (2 aor 3) particles are in stacks, each stack corresponds to a single micrograph.
	    In each case the particles should contain a name of the micrograph of origin stores using attribute name 'ptcl_source_image'.
        Normally this is done by e2boxer.py during windowing.
	    Particles whose defocus or astigmatism error exceed set thresholds will be skipped, otherwise, virtual stacks with the original way preceded by G will be created.
		sxprocess.py  --input=bdb:data  --importctf=outdir/partres  --defocuserror=10.0  --astigmatismerror=5.0
		#  Output will be a vritual stack bdb:Gdata
		sxprocess.py  --input="bdb:directory/stacks*"  --importctf=outdir/partres  --defocuserror=10.0  --astigmatismerror=5.0
		To concatenate output files:
		cd directory
		e2bdb.py . --makevstack=bdb:allparticles  --filt=G
		IMPORTANT:  Please do not move (or remove!) any input/intermediate EMAN2DB files as the information is linked between them.

   11. Scale 3D shifts.  The shifts in the input five columns text file with 3D orientation parameters will be DIVIDED by the scale factor
		sxprocess.py  orientationparams.txt  scaledparams.txt  scale=0.5

   12. Generate soft-edged 3D mask from input 3D volume automatically or using the user-provided threshold.
        Automatically compute the threshold to intially obtain the largest density cluster.
        sxprocess.py  vol3d.hdf  mask3d.hdf  --adaptive_mask  --fl=12.0 --aa=0.02 --pixel_size=3.1 --nsigma=3.0  --ndilation=1  --edge_width=3 --edge_type="G" --mol_mass=1500.0

        Use the user-provided threshold to intially obtain the largest density cluster.
        sxprocess.py  vol3d.hdf  mask3d.hdf  --adaptive_mask --threshold=0.05  --ndilation=0  --edge_width=5

   13. Generate binary 3D mask from input 3D volume using the user-provided threshold.
        sxprocess.py  vol3d.hdf  mask3d.hdf  --binary_mask  --threshold=0.05  --ndilation=0 --nerosion=0

   14. Sharpen of volumes or images by enhancing the power spectrum:
       (1) Halfset Volumes Mode
           For a pair of unfiltered odd & even halfset volumes produced by MERIDIEN, this command executes the following processes:
           a. Calculate FSC with provided mask and adjust the FSC;
           b. Sum two volume; 
           c. Apply mask
           d. Apply MTF correction (optional);
           e. Adjust power spectrum by 2*FSC/(1+FSC) (optional);  
           f. Estimate B-factor from 10 Angstrom (default) to the resolution (optional); 
           g. Apply negative B-factor to enhance the volume (optional);
           h. Apply low-pass filter to the volume (optional)
           options are independent of each others.
           --do_adaptive_mask : =True when it is restored, the program adaptively creates adaptive mask file using summed two volumes. This takes a couple of minutes. For map with dimension of 384*384*384, it takes 6 minutes.
           --mtf              : For those high resolution maps, mtf correction would significantly enhance structural features.
           --fsc_adj          : FSC adjustment of power spectrum is inclined to increase the slope of power spectrum of the summed volume.
           --B_enhance        : =0.0, program estimates B-factor from B_start (usually 10 Angstrom) to the resolution determined by FSC 0.143; >0.0, program uses the given value to enhance map; =-1.0, B-factor is not applied.
           --fl               : =0.0, low-pass filter to resolution; >=0.5, low-pass filter to the given Angstrom; >0.0 AND <=0.5, low-pass filter to the given absolute frequency; =-1.0, no low-pass filter.

           sxprocess.py  --combinemaps  vol_0_unfil.hdf vol_1_unfil.hdf  --output_dir=outdir_sharpen_haflset  --output=spharpen_fullset_vol3d.hdf  --pixel_size=1.12  --mask=mask3d.hdf   --mtf=mtf.txt  --fl=-1   --fsc_adj
           sxprocess.py  --combinemaps  vol_0_unfil.hdf vol_1_unfil.hdf  --output_dir=outdir_sharpen  --pixel_size=1.12  --mask=mask3d.hdf   --mtf=aa.txt  --fl=4.7  --aa=0.02 --fsc_adj
           sxprocess.py  --combinemaps  vol_0_unfil.hdf vol_1_unfil.hdf  --output_dir=outdir_sharpen  --output=spharpen_fullset_vol3d.hdf  --pixel_size=1.12  --do_adaptive_mask  --mtf=mtf.txt  --fl=3.9  --aa=0.01  --B_enhance=280

       (2) Cluster Volumes Mode
           For cluster volumes produced by SORT3D_DEPTH, this command executes the following processes:
           a. Apply mask
           b. Apply MTF correction (optional);
           c. Apply negative B-factor to enhance the volume using user-provided ad-hoc value (optional);
           d. Apply low-pass filter to the volume using user-provided ad-hoc value (optional)
           options are independent of each others.
           --do_adaptive_mask : =True when it is restored, the program adaptively creates adaptive mask file using each cluster volume. This takes a couple of minutes. For map with dimension of 384*384*384, it takes 6 minutes.
           --mtf              : For those high resolution maps, mtf correction would significantly enhance structural features.
           --B_enhance        : >0.0, program uses the given value to enhance map; =-1.0, B-factor is not applied.
           --fl               : >=0.5, low-pass filter to the given Angstrom; >0.0 AND <=0.5, low-pass filter to the given absolute frequency; =-1.0, no low-pass filter.

           sxprocess.py  --combinemaps  vol_cluster*.hdf    --output_dir=outdir_sharpen_cluster  --output=spharpen_cluster_vol3d.hdf  --pixel_size=1.12  --do_adaptive_mask  --mtf=mtf.txt  --fl=3.9  --aa=0.01  --B_enhance=280
           sxprocess.py  --combinemaps  vol_cluster000.hdf  --output_dir=outdir_sharpen_cluster  --output=spharpen_cluster_vol3d.hdf  --pixel_size=1.12  --do_adaptive_mask  --mtf=mtf.txt  --fl=3.9  --aa=0.01  --B_enhance=280

       (3) Images Mode - for 2D images
           Calculate B-factor and apply negative B-factor to 2D images.

   15. Window stack file --window out central area of images.
        sxprocess.py input.hdf output.hdf --box=new_box_size

   16. Pad stack file --pad images to a larger size and set surround background to request value (default 0.0).
        sxprocess.py input.hdf output.hdf --box=new_box_size --background=3.0

   17. Create angular distribution .build file
        sxprocess.py --angular_distribution  inputfile=example/path/params.txt --pixel_size=1.0  --round_digit=5  --box_size=500  --particle_radius=175  --cylinder_width=1  --cylinder_length=10000

   18. Subtract from images in the first stack images in the second stack and write results to the third stack.
       If the name of the output stack is the same as the second stack, the results will be written to the second
       stack (it will be overwritten).

   		sxprocess.py bdb:orgstack bdb:proj/data  bdb:proj/sdata bdb:proj/odata --subtract_stack

   19. Balance angular distribution. Input ASCII file with 3D orientation parameters, compute a histogram
       of ditribution of angles using user-provided angular step, retain a subset of randomly select
       projection direction per angular bin using user-provided threshold, write the list of the all
       retained projection directions.  (In order to create a substack with retained images, use e2bdb.py
       with options makevstack and list).

   		sxprocess.py --balance_angular_distribution  params.txt select.txt --max_occupy=100 --angstep=15 --symmetry=d3

"""
    )

    parser = optparse.OptionParser(usage, version=sp_global_def.SPARXVERSION)
    parser.add_option(
        "--order",
        action="store_true",
        default=False,
        help="Two arguments are required: name of input stack and desired name of output stack. The output stack is the input stack sorted by similarity in terms of cross-correlation coefficent.",
    )
    parser.add_option(
        "--order_lookup", action="store_true", default=False, help="Test/Debug."
    )
    parser.add_option(
        "--order_metropolis", action="store_true", default=False, help="Test/Debug."
    )
    parser.add_option(
        "--order_pca", action="store_true", default=False, help="Test/Debug."
    )
    parser.add_option(
        "--initial",
        type="int",
        default=-1,
        help="Specifies which image will be used as an initial seed to form the chain. (default -1; means the first image)",
    )
    parser.add_option(
        "--circular",
        action="store_true",
        default=False,
        help="Select circular ordering (fisr image has to be similar to the last",
    )
    parser.add_option(
        "--radius",
        type="int",
        default=-1,
        help="Radius of a circular mask for similarity based ordering",
    )
    parser.add_option(
        "--ratio",
        type="float",
        default=1.0,
        help="The ratio of new to old image size (if <1 the pixel size will increase and image size decrease, if>1, the other way round",
    )
    parser.add_option(
        "--changesize",
        action="store_true",
        default=False,
        help="resample (decimate or interpolate up) images (2D or 3D) in a stack to change the pixel size.",
    )
    parser.add_option(
        "--pw",
        action="store_true",
        default=False,
        help="compute average power spectrum of a stack of 2-D images with optional padding (option wn) with zeroes",
    )
    parser.add_option(
        "--wn",
        type="int",
        default=-1,
        help="Size of window to use (should be larger/equal than particle box size, default padding to max(nx,ny))",
    )
    parser.add_option(
        "--phase_flip",
        action="store_true",
        default=False,
        help="Phase flip the input stack",
    )
    parser.add_option(
        "--makedb",
        metavar="param1=value1:param2=value2",
        type="string",
        action="append",
        help="One argument is required: name of key with which the database will be created. Fill in database with parameters specified as follows: --makedb param1=value1:param2=value2, e.g. 'gauss_width'=1.0:'pixel_input'=5.2:'pixel_output'=5.2:'thr_low'=1.0",
    )
    parser.add_option(
        "--generate_projections",
        metavar="param1=value1:param2=value2",
        type="string",
        action="append",
        help="Three arguments are required: name of input structure from which to generate projections, desired name of output projection stack, and desired prefix for micrographs (e.g. if prefix is 'mic', then micrographs mic0.hdf, mic1.hdf etc will be generated). Optional arguments specifying format, apix, box size and whether to add CTF effects can be entered as follows after --generate_projections: format='bdb':apix=5.2:CTF=True:boxsize=100, or format='hdf', etc., where format is bdb or hdf, apix (pixel size) is a float, CTF is True or False, and boxsize denotes the dimension of the box (assumed to be a square). If an optional parameter is not specified, it will default as follows: format='bdb', apix=2.5, CTF=False, boxsize=64.",
    )
    parser.add_option(
        "--isacgroup",
        type="int",
        default=-1,
        help="Retrieve original image numbers in the selected ISAC group. See ISAC documentation for details.",
    )
    parser.add_option(
        "--isacselect",
        action="store_true",
        default=False,
        help="Retrieve original image numbers of images listed in ISAC output stack of averages. See ISAC documentation for details.",
    )
    parser.add_option(
        "--params",
        type="string",
        default=None,
        help="Name of header of parameter, which one depends on specific option",
    )
    parser.add_option(
        "--adjpw",
        action="store_true",
        default=False,
        help="Adjust rotationally averaged power spectrum of an image",
    )
    parser.add_option(
        "--rotpw",
        type="string",
        default=None,
        help="Name of the text file to contain rotationally averaged power spectrum of the input image.",
    )
    parser.add_option(
        "--transformparams",
        type="string",
        default=None,
        help="Transform 3D projection orientation parameters using six 3D parameters (phi, theta,psi,sx,sy,sz).  Input: --transformparams=45.,66.,12.,-2,3,-5.5 desired six transformation of the reconstructed structure. Output: file with modified orientation parameters.",
    )

    # import ctf estimates done using cter
    parser.add_option("--input", type="string", default=None, help="Input particles.")
    parser.add_option(
        "--importctf",
        type="string",
        default=None,
        help="Name of the file containing CTF parameters produced by sxcter.",
    )
    parser.add_option(
        "--defocuserror",
        type="float",
        default=1000000.0,
        help="Exclude micrographs whose relative defocus error as estimated by sxcter is larger than defocuserror percent.  The error is computed as (std dev defocus)/defocus*100%",
    )
    parser.add_option(
        "--astigmatismerror",
        type="float",
        default=360.0,
        help="Set to zero astigmatism for micrographs whose astigmatism angular error as estimated by sxcter is larger than astigmatismerror degrees.",
    )

    # import ctf estimates done using cter
    parser.add_option(
        "--scale",
        type="float",
        default=-1.0,
        help="Divide shifts in the input 3D orientation parameters text file by the scale factor.",
    )

    # Generate soft-edged 3D mask from input 3D volume and Generate binarized version of input 3D volume
    parser.add_option(
        "--adaptive_mask",
        action="store_true",
        default=False,
        help="generate soft-edged 3D mask from input 3D volume",
    )
    parser.add_option(
        "--nsigma",
        type="float",
        default=1.0,
        help="number of times of sigma of the input volume to intially obtain the largest density cluster",
    )
    parser.add_option(
        "--threshold",
        type="float",
        default=-9999,
        help="threshold provided by user to intially obtain the largest density cluster (default: -9999, threshold will be determined automatically",
    )
    parser.add_option(
        "--ndilation",
        type="int",
        default=3,
        help="number of times of dilation applied to the largest cluster of density",
    )
    parser.add_option(
        "--edge_width",
        type="int",
        default=5,
        help="width of the cosine edge of the mask",
    )
    parser.add_option(
        "--edge_type",
        type=str,
        default="cosine",
        help="Soft-edge type: The type of soft-edge for moon-eliminator 3D mask and a moon-eliminated soft-edged 3D mask. Available methods are (1) 'cosine' for cosine soft-edged (used in PostRefiner) and (2) 'gauss' for gaussian soft-edge. (default cosine)",
    )
    parser.add_option(
        "--mol_mass",
        type=float,
        default=-1.0,
        help="Molecular mass [kDa]: The estimated molecular mass of the target particle in kilodalton. (default Not used)",
    )

    # Generate soft-edged 3D mask from input 3D volume and Generate binarized version of input 3D volume
    parser.add_option(
        "--binary_mask",
        action="store_true",
        default=False,
        help="generate binary 3D mask from input 3D volume",
    )
    parser.add_option(
        "--nerosion",
        type="int",
        default=0,
        help="number of times to erode binarized volume",
    )

    # Postprocess 3-D
    parser.add_option(
        "--combinemaps",
        action="store_true",
        default=False,
        help="flag to enhance the power spectrum of volumes or images. Available modes are (1) Halfset Volumes Mode, (2) Cluster Volumes Mode,and (3) Images Mode",
    )
    parser.add_option(
        "--mtf", type="string", default=None, help="entry for mtf text file of camera"
    )
    parser.add_option(
        "--fsc_adj",
        action="store_true",
        default=False,
        help="flag to turn on power spectrum adjustment of summed volume by their FSC (effective only in Halfset Volumes Mode)",
    )
    parser.add_option(
        "--B_enhance",
        type="float",
        default=0.0,
        help="=0.0, apply Bfactor to enhance map with automatic estimation (valid only in Halfset Volumes Mode); >=0.0, with ad-hoc value; =-1.0, not enhance at all",
    )
    parser.add_option(
        "--fl",
        type="float",
        default=0.0,
        help="=0.0, low-pass filter to resolution limit; =some value, low-pass filter to some valume; =-1, not low-pass filter applied",
    )
    parser.add_option(
        "--aa",
        type="float",
        default=0.01,
        help="=0.01,low pass filter falloff (effective only when --fl option is not -1.0)",
    )
    parser.add_option(
        "--mask", type="string", default=None, help="path for input mask file"
    )
    parser.add_option(
        "--output", type="string", default="vol_combined.hdf", help="output file name"
    )
    parser.add_option(
        "--output_dir", type="string", default="./", help="output directory name"
    )
    parser.add_option(
        "--pixel_size", type="float", default=0.0, help="pixel size of the data"
    )
    parser.add_option(
        "--B_start",
        type="float",
        default=10.0,
        help="=10.0 Angstrom, starting frequency in Angstrom for B-factor estimation (effective only in Halfset Volumes Mode with --B_enhance=0.0)",
    )
    parser.add_option(
        "--B_stop",
        type="float",
        default=0.0,
        help="=0.0, cutoff frequency in Angstrom for B-factor estimation. recommended to set cutoff to the frequency where fsc < 0.0. by default, the program uses Nyquist frequency. (effective only in Halfset Volumes Mode with --B_enhance=0.0)",
    )
    parser.add_option(
        "--do_adaptive_mask",
        action="store_true",
        default=False,
        help="generate adaptive mask with the given threshold",
    )
    parser.add_option(
        "--do_approx",
        action="store_true",
        default=False,
        help="Approximate the soft edge values instead of using the exact ones.",
    )
    # parser.add_option("--randomphasesafter",   type="float",         default=0.8,                   help=" set Fourier pixels random phases after FSC value ")
    # window
    parser.add_option(
        "--window_stack",
        action="store_true",
        default=False,
        help="window stack images using a smaller window size",
    )
    parser.add_option("--box", type="int", default=0, help="the new window size ")

    # pad
    parser.add_option(
        "--pad",
        action="store_true",
        default=False,
        help="pad stack images to a larger window size and set the surrounding background (by default to 0.0)",
    )
    parser.add_option(
        "--background",
        type="float",
        default=0.0,
        help="value the surrounding area will be set to",
    )

    # Options for angular distribution
    parser.add_option(
        "--angular_distribution",
        action="store_true",
        default=False,
        help="create an angular distribution file based on a project3d.txt",
    )
    parser.add_option(
        "--symmetry", type="string", default="c1", help="point group symmetry"
    )
    parser.add_option(
        "--round_digit",
        type="int",
        default=5,
        help="accuracy of the loaded angle (default 5)",
    )
    parser.add_option(
        "--box_size",
        type="int",
        default=500,
        help="box size in pixel used for calculating the center of the particle [px] (default 500)",
    )
    parser.add_option(
        "--particle_radius",
        type="int",
        default=175,
        help="particle radius [Pixels] (default 175)",
    )
    parser.add_option(
        "--cylinder_width",
        type="int",
        default=1,
        help="width of the cylinder (default 1)",
    )
    parser.add_option(
        "--cylinder_length",
        type="int",
        default=10000,
        help="length of the cylinder (default 10000)",
    )

    parser.add_option(
        "--subtract_stack",
        action="store_true",
        default=False,
        help="Subtract from images in the first stack images in the second stack",
    )
    parser.add_option(
        "--normalize", action="store_true", default=False, help="Flag to normalize data"
    )

    # Options for balance_angular distribution
    parser.add_option(
        "--balance_angular_distribution",
        action="store_true",
        default=False,
        help="balance an angular distribution",
    )
    parser.add_option(
        "--max_occupy",
        type="int",
        default=1,
        help="maximum number of angular orientations per reference angle (default 1)",
    )
    parser.add_option(
        "--angstep",
        type="float",
        default=3.75,
        help="angular step of reference angles, i.e., number of bins of angular histogram) (default 3.75)",
    )

    (options, args) = parser.parse_args()

    sp_global_def.BATCH = True

    if options.phase_flip:
        nargs = len(args)

        if nargs != 2:
            sp_global_def.ERROR("Must provide name of input and output file!")
            return

        instack = args[0]
        outstack = args[1]
        nima = EMAN2_cppwrap.EMUtil.get_image_count(instack)
        for i in range(nima):
            img = EMAN2_cppwrap.EMData()
            img.read_image(instack, i)
            try:
                ctf = img.get_attr("ctf")
            except:
                sp_global_def.ERROR("No ctf information in input stack!")
                return

            dopad = True
            sign = 1
            binary = 1  # phase flip

            assert img.get_ysize() > 1
            dict = ctf.to_dict()
            dz = dict["defocus"]
            cs = dict["cs"]
            voltage = dict["voltage"]
            pixel_size = dict["apix"]
            b_factor = dict["bfactor"]
            ampcont = dict["ampcont"]
            dza = dict["dfdiff"]
            azz = dict["dfang"]

            if dopad and not img.is_complex():
                ip = 1
            else:
                ip = 0

            params = {
                "filter_type": EMAN2_cppwrap.Processor.fourier_filter_types.CTF_,
                "defocus": dz,
                "Cs": cs,
                "voltage": voltage,
                "Pixel_size": pixel_size,
                "B_factor": b_factor,
                "amp_contrast": ampcont,
                "dopad": ip,
                "binary": binary,
                "sign": sign,
                "dza": dza,
                "azz": azz,
            }

            tmp = EMAN2_cppwrap.Processor.EMFourierFilter(img, params)
            tmp.set_attr_dict({"ctf": ctf})

            tmp.write_image(outstack, i)

    elif options.changesize:
        nargs = len(args)
        if nargs != 2:
            sp_global_def.ERROR("Must provide name of input and output file!")
            return

        instack = args[0]
        outstack = args[1]
        sub_rate = float(options.ratio)

        nima = EMAN2_cppwrap.EMUtil.get_image_count(instack)
        for i in range(nima):
            sp_fundamentals.resample(
                sp_utilities.get_im(instack, i), sub_rate
            ).write_image(outstack, i)

    elif options.isacgroup > -1:
        nargs = len(args)
        if nargs != 3:
            sp_global_def.ERROR("Three files needed on input!")
            return

        instack = args[0]
        m = sp_utilities.get_im(args[1], int(options.isacgroup)).get_attr("members")
        l = []
        for k in m:
            l.append(int(sp_utilities.get_im(args[0], k).get_attr(options.params)))
        sp_utilities.write_text_file(l, args[2])

    elif options.isacselect:
        nargs = len(args)
        if nargs != 2:
            sp_global_def.ERROR("Two files needed on input!")
            return

        nima = EMAN2_cppwrap.EMUtil.get_image_count(args[0])
        m = []
        for k in range(nima):
            m += sp_utilities.get_im(args[0], k).get_attr("members")
        m.sort()
        sp_utilities.write_text_file(m, args[1])

    elif options.pw:
        nargs = len(args)
        if nargs < 2:
            sp_global_def.ERROR("Must provide name of input and output file!")
            return

        d = sp_utilities.get_im(args[0])
        ndim = d.get_ndim()
        if ndim == 3:
            pw = sp_fundamentals.rops_table(d)
            sp_utilities.write_text_file(pw, args[1])
        else:
            nx = d.get_xsize()
            ny = d.get_ysize()
            if nargs == 3:
                mask = sp_utilities.get_im(args[2])
            wn = int(options.wn)
            if wn == -1:
                wn = max(nx, ny)
            else:
                if (wn < nx) or (wn < ny):
                    sp_global_def.ERROR(
                        "window size cannot be smaller than the image size"
                    )
                    return

            n = EMAN2_cppwrap.EMUtil.get_image_count(args[0])
            p = sp_utilities.model_blank(wn, wn)

            for i in range(n):
                d = sp_utilities.get_im(args[0], i)
                st = EMAN2_cppwrap.Util.infomask(d, None, True)
                d -= st[0]
                if nargs == 3:
                    d *= mask
                p += EMAN2_cppwrap.periodogram(sp_utilities.pad(d, wn, wn, 1, 0.0))
            p = old_div(p, n)
            p.write_image(args[1])

    elif options.adjpw:

        if len(args) < 3:
            sp_global_def.ERROR(
                "Filt_by_rops input target output fl aa (the last two are optional parameters of a low-pass filter)"
            )
            return

        img_stack = args[0]
        if args[1][-3:] == "txt":
            rops_dst = sp_utilities.read_text_file(args[1])
        else:
            rops_dst = sp_fundamentals.rops_table(sp_utilities.get_im(args[1]))

        out_stack = args[2]
        if len(args) > 4:
            fl = float(args[3])
            aa = float(args[4])
        else:
            fl = -1.0
            aa = 0.0

        nimage = EMAN2_cppwrap.EMUtil.get_image_count(img_stack)

        for i in range(nimage):
            img = sp_fundamentals.fft(sp_utilities.get_im(img_stack, i))
            rops_src = sp_fundamentals.rops_table(img)

            assert len(rops_dst) == len(rops_src)

            table = [0.0] * len(rops_dst)
            for j in range(len(rops_dst)):
                table[j] = numpy.sqrt(old_div(rops_dst[j], rops_src[j]))

            if fl > 0.0:
                img = sp_filter.filt_tanl(img, fl, aa)
            img = sp_fundamentals.fft(sp_filter.filt_table(img, table))
            img.write_image(out_stack, i)

    elif options.rotpw != None:

        if len(args) != 1:
            sp_global_def.ERROR("Only one input permitted")
            return

        im = sp_utilities.get_im(args[0])
        ndim = im.get_ndim()
        if ndim == 1:
            t = EMAN2_cppwrap.periodogram(im)
            t = [t[i] for i in range(t.get_xsize())]
        else:
            t = sp_fundamentals.rops_table(im)
        x = list(range(len(t)))
        r = [numpy.log10(abs(q)) for q in t]
        sp_utilities.write_text_file([t, r, x], options.rotpw)

    elif options.transformparams != None:
        if len(args) != 2:
            sp_global_def.ERROR(
                "Please provide names of input and output files with orientation parameters"
            )
            return

        transf = [0.0] * 6
        spl = options.transformparams.split(",")
        for i in range(len(spl)):
            transf[i] = float(spl[i])

        sp_utilities.write_text_row(
            sp_utilities.rotate_shift_params(
                sp_utilities.read_text_row(args[0]), transf
            ),
            args[1],
        )

    elif options.makedb != None:
        nargs = len(args)
        if nargs != 1:
            sp_global_def.ERROR(
                "Must provide exactly one argument denoting database key under which the input params will be stored"
            )
            return

        dbkey = args[0]
        print("database key under which params will be stored: ", dbkey)
        gbdb = EMAN2jsondb.js_open_dict("e2boxercache/gauss_box_DB.json")

        parmstr = "dummy:" + options.makedb[0]
        (processorname, param_dict) = EMAN2.parsemodopt(parmstr)
        dbdict = {}
        for pkey in param_dict:
            if (pkey == "invert_contrast") or (pkey == "use_variance"):
                if param_dict[pkey] == 1:
                    dbdict[pkey] = True
                else:
                    dbdict[pkey] = False
            else:
                dbdict[pkey] = param_dict[pkey]
        gbdb[dbkey] = dbdict

    elif options.generate_projections:
        nargs = len(args)
        if nargs != 3:
            sp_global_def.ERROR(
                "Must provide name of input structure(s) from which to generate projections, name of output projection stack, and prefix for output micrographs."
            )
            return

        inpstr = args[0]
        outstk = args[1]
        micpref = args[2]

        parmstr = "dummy:" + options.generate_projections[0]
        (processorname, param_dict) = EMAN2.parsemodopt(parmstr)

        parm_CTF = False
        parm_format = "bdb"
        parm_apix = 2.5

        if "CTF" in param_dict:
            if param_dict["CTF"] == "True":
                parm_CTF = True

        if "format" in param_dict:
            parm_format = param_dict["format"]

        if "apix" in param_dict:
            parm_apix = float(param_dict["apix"])

        boxsize = 64
        if "boxsize" in param_dict:
            boxsize = int(param_dict["boxsize"])

        print(
            "pixel size: ",
            parm_apix,
            " format: ",
            parm_format,
            " add CTF: ",
            parm_CTF,
            " box size: ",
            boxsize,
        )

        scale_mult = 2500
        sigma_add = 1.5
        sigma_proj = 30.0
        sigma2_proj = 17.5
        sigma_gauss = 0.3
        sigma_mic = 30.0
        sigma2_mic = 17.5
        sigma_gauss_mic = 0.3

        if "scale_mult" in param_dict:
            scale_mult = float(param_dict["scale_mult"])
        if "sigma_add" in param_dict:
            sigma_add = float(param_dict["sigma_add"])
        if "sigma_proj" in param_dict:
            sigma_proj = float(param_dict["sigma_proj"])
        if "sigma2_proj" in param_dict:
            sigma2_proj = float(param_dict["sigma2_proj"])
        if "sigma_gauss" in param_dict:
            sigma_gauss = float(param_dict["sigma_gauss"])
        if "sigma_mic" in param_dict:
            sigma_mic = float(param_dict["sigma_mic"])
        if "sigma2_mic" in param_dict:
            sigma2_mic = float(param_dict["sigma2_mic"])
        if "sigma_gauss_mic" in param_dict:
            sigma_gauss_mic = float(param_dict["sigma_gauss_mic"])

        random.seed(int(time.time()))
        delta = 29
        angles = sp_utilities.even_angles(delta, 0.0, 89.9, 0.0, 359.9, "S")
        nangle = len(angles)

        modelvol = []
        nvlms = EMAN2_cppwrap.EMUtil.get_image_count(inpstr)
        for k in range(nvlms):
            modelvol.append(sp_utilities.get_im(inpstr, k))

        nx = modelvol[0].get_xsize()

        if nx != boxsize:
            sp_global_def.ERROR(
                "Requested box dimension does not match dimension of the input model."
            )
            return

        nvol = 10
        volfts = [[] for k in range(nvlms)]
        for k in range(nvlms):
            for i in range(nvol):
                sigma = sigma_add + random.random()  # 1.5-2.5
                addon = sp_utilities.model_gauss(
                    sigma, boxsize, boxsize, boxsize, sigma, sigma, 38, 38, 40
                )
                scale = scale_mult * (0.5 + random.random())
                vf, kb = sp_projection.prep_vol(modelvol[k] + scale * addon)
                volfts[k].append(vf)
        del vf, modelvol

        if parm_format == "bdb":
            stack_data = "bdb:" + outstk
            sp_utilities.delete_bdb(stack_data)
        else:
            stack_data = outstk + ".hdf"
        Cs = 2.0
        pixel = parm_apix
        voltage = 120.0
        ampcont = 10.0
        ibd = old_div(4096, 2) - boxsize
        iprj = 0

        width = 240
        xstart = 8 + old_div(boxsize, 2)
        ystart = 8 + old_div(boxsize, 2)
        rowlen = 17
        params = []
        for idef in range(3, 8):

            irow = 0
            icol = 0

            mic = sp_utilities.model_blank(4096, 4096)
            defocus = idef * 0.5  # 0.2
            if parm_CTF:
                astampl = defocus * 0.15
                astangl = 50.0
                ctf = sp_utilities.generate_ctf(
                    [defocus, Cs, voltage, pixel, 0.0, ampcont, astampl, astangl]
                )

            for i in range(nangle):
                for k in range(12):
                    dphi = 8.0 * (random.random() - 0.5)
                    dtht = 8.0 * (random.random() - 0.5)
                    psi = 360.0 * random.random()

                    phi = angles[i][0] + dphi
                    tht = angles[i][1] + dtht

                    s2x = 4.0 * (random.random() - 0.5)
                    s2y = 4.0 * (random.random() - 0.5)

                    params.append([phi, tht, psi, s2x, s2y])

                    ivol = iprj % nvol
                    # imgsrc = randint(0,nvlms-1)
                    imgsrc = iprj % nvlms
                    proj = sp_projection.prgs(
                        volfts[imgsrc][ivol], kb, [phi, tht, psi, -s2x, -s2y]
                    )

                    x = xstart + irow * width
                    y = ystart + icol * width

                    mic += sp_utilities.pad(
                        proj, 4096, 4096, 1, 0.0, x - 2048, y - 2048, 0
                    )

                    proj = proj + sp_utilities.model_gauss_noise(sigma_proj, nx, nx)
                    if parm_CTF:
                        proj = sp_filter.filt_ctf(proj, ctf)
                        proj.set_attr_dict({"ctf": ctf, "ctf_applied": 0})

                    proj = proj + sp_filter.filt_gaussl(
                        sp_utilities.model_gauss_noise(sigma2_proj, nx, nx), sigma_gauss
                    )
                    proj.set_attr("origimgsrc", imgsrc)
                    proj.set_attr("test_id", iprj)
                    proj.set_attr("ptcl_source_image", micpref + "%1d.hdf" % (idef - 3))
                    # flags describing the status of the image (1 = true, 0 = false)
                    sp_utilities.set_params2D(proj, [0.0, 0.0, 0.0, 0, 1.0])
                    sp_utilities.set_params_proj(proj, [phi, tht, psi, s2x, s2y])

                    proj.write_image(stack_data, iprj)

                    icol += 1
                    if icol == rowlen:
                        icol = 0
                        irow += 1

                    iprj += 1

            mic += sp_utilities.model_gauss_noise(sigma_mic, 4096, 4096)
            if parm_CTF:
                # apply CTF
                mic = sp_filter.filt_ctf(mic, ctf)
            mic += sp_filter.filt_gaussl(
                sp_utilities.model_gauss_noise(sigma2_mic, 4096, 4096), sigma_gauss_mic
            )

            mic.write_image(micpref + "%1d.hdf" % (idef - 3), 0)

        sp_utilities.drop_spider_doc("params.txt", params)

    elif options.importctf != None:
        print(" IMPORTCTF  ")
        grpfile = "groupid%04d" % random.randint(1000, 9999)
        ctfpfile = "ctfpfile%04d" % random.randint(1000, 9999)
        cterr = [old_div(options.defocuserror, 100.0), options.astigmatismerror]
        ctfs = sp_utilities.read_text_row(options.importctf)
        for kk in range(len(ctfs)):
            root, name = os.path.split(ctfs[kk][-1])
            ctfs[kk][-1] = name[:-4]

        if options.input[:4] != "bdb:":
            sp_global_def.ERROR("Sorry, only bdb files implemented")
            return

        d = options.input[4:]
        # try:     str = d.index('*')
        # except:  str = -1
        uu = os.path.split(d)
        uu = os.path.join(uu[0], "EMAN2DB", uu[1] + ".bdb")
        flist = glob.glob(uu)
        for i in range(len(flist)):
            root, name = os.path.split(flist[i])
            root = root[:-7]
            name = name[:-4]
            fil = "bdb:" + os.path.join(root, name)
            sourcemic = EMAN2_cppwrap.EMUtil.get_all_attributes(
                fil, "ptcl_source_image"
            )
            nn = len(sourcemic)
            gctfp = []
            groupid = []
            for kk in range(nn):
                junk, name2 = os.path.split(sourcemic[kk])
                name2 = name2[:-4]
                ctfp = [-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
                for ll in range(len(ctfs)):
                    if name2 == ctfs[ll][-1]:
                        #  found correct
                        if old_div(ctfs[ll][8], ctfs[ll][0]) <= cterr[0]:
                            #  acceptable defocus error
                            ctfp = ctfs[ll][:8]
                            if ctfs[ll][10] > cterr[1]:
                                # error of astigmatism exceed the threshold, set astigmatism to zero.
                                ctfp[6] = 0.0
                                ctfp[7] = 0.0
                            gctfp.append(ctfp)
                            groupid.append(kk)
                        break
            if len(groupid) > 0:
                sp_utilities.write_text_row(groupid, grpfile)
                sp_utilities.write_text_row(gctfp, ctfpfile)
                cmd = "{} {} {} {}".format(
                    "e2bdb.py",
                    fil,
                    "--makevstack=bdb:" + root + "G" + name,
                    "--list=" + grpfile,
                )
                # print cmd
                subprocess.call(cmd, shell=True)
                cmd = "{} {} {} {}".format(
                    "sxheader.py",
                    "bdb:" + root + "G" + name,
                    "--params=ctf",
                    "--import=" + ctfpfile,
                )
                # print cmd
                subprocess.call(cmd, shell=True)
            else:
                print(" >>>  Group ", name, "  skipped.")

        cmd = "{} {} {}".format("rm -f", grpfile, ctfpfile)
        subprocess.call(cmd, shell=True)

    elif options.scale > 0.0:
        scale = options.scale
        nargs = len(args)

        if nargs != 2:
            sp_global_def.ERROR("Please provide names of input and output file!")
            return

        p = sp_utilities.read_text_row(args[0])
        for i in range(len(p)):
            p[i][3] = old_div(p[i][3], scale)
            p[i][4] = old_div(p[i][4], scale)
        sp_utilities.write_text_row(p, args[1])

    elif options.adaptive_mask:
        print(
            "DEPRECATION WARNING! This function is deprecated and no longer maintained. Please use sxmask.py instead"
        )
        nargs = len(args)

        if nargs == 0:
            sp_global_def.ERROR(
                "Generate soft-edged 3D mask from input 3D volume automatically or using the user provided threshold."
            )
            return

        elif nargs > 2:
            sp_global_def.ERROR("Too many arguments")
            return

        inputvol = sp_utilities.get_im(args[0])  # args[0]: input 3D volume file path
        input_path, input_file_name = os.path.split(args[0])
        input_file_name_root, ext = os.path.splitext(input_file_name)

        if nargs == 2:
            mask_file_name = args[1]  # args[1]: output 3D mask file path
        else:
            mask_file_name = (
                "adaptive_mask_for_" + input_file_name_root + ".hdf"
            )  # Only hdf file is output.

        if options.fl > 0.0:
            inputvol = sp_filter.filt_tanl(
                inputvol, old_div(options.pixel_size, options.fl), options.aa
            )

        if options.mol_mass > 0.0:
            density_threshold = inputvol.find_3d_threshold(
                options.mol_mass, options.pixel_size
            )
        else:
            density_threshold = options.threshold

        if options.edge_type == "cosine":
            mode = "C"
        else:
            mode = "G"

        sp_morphology.adaptive_mask(
            inputvol,
            options.nsigma,
            density_threshold,
            options.ndilation,
            options.edge_width,
            mode,
        ).write_image(mask_file_name)

    elif options.binary_mask:
        print(
            "DEPRECATION WARNING! This function is deprecated and no longer maintained. Please use sxmask.py instead"
        )
        nargs = len(args)

        if nargs == 0:
            sp_global_def.ERROR(
                "Generate binary 3D mask from input 3D volume using the user-provided threshold."
            )
            return

        elif nargs > 2:
            sp_global_def.ERROR("Too many arguments are given, try again!")
            return

        inputvol = sp_utilities.get_im(args[0])
        input_path, input_file_name = os.path.split(args[0])
        input_file_name_root, ext = os.path.splitext(input_file_name)

        if nargs == 2:
            mask_file_name = args[1]
        else:
            mask_file_name = (
                "binary_mask_for_" + input_file_name_root + ".hdf"
            )  # Only hdf file is output.

        mask3d = sp_morphology.binarize(inputvol, options.threshold)

        for i in range(options.nerosion):
            mask3d = sp_morphology.erosion(mask3d)
        for i in range(options.ndilation):
            mask3d = sp_morphology.dilation(mask3d)

        mask3d.write_image(mask_file_name)

    elif options.combinemaps:
        if options.output_dir != "./":
            if not os.path.exists(options.output_dir):
                os.makedirs(options.output_dir)
                sp_global_def.write_command(options.output_dir)
        if os.path.exists(os.path.join(options.output_dir, "log.txt")):
            os.remove(os.path.join(options.output_dir, "log.txt"))
        log_main = sp_logger.Logger(sp_logger.BaseLogger_Files())
        log_main.prefix = os.path.join(options.output_dir, "./")
        # line = strftime("%Y-%m-%d_%H:%M:%S", localtime()) + " =>"
        log_main.add("--------------------------------------------")
        log_main.add("------->>> SPHIRE combinemaps <<<-------")

        nargs = len(args)

        if nargs < 1:
            sp_global_def.ERROR("Too few inputs (--combinemaps option)")
            return

        if options.pixel_size <= 0.0:
            sp_global_def.ERROR(
                "Set a valid value to pixel_size first! There is no default value for pixel_size (--combinemaps option)"
            )
            return

        input_path_list = []
        suffix_patten = None
        map1_basename_tokens = None
        cluster_id_substr_head_idx = None
        if nargs == 1:  # 2D case, 3D single map case, or 3D single maps case
            if args[0].find("*") != -1:  # 3D single maps case
                input_path_list = glob.glob(args[0])
                # Check error condition of input file path list
                if len(input_path_list) == 0:
                    sp_global_def.ERROR(
                        "No input files are found with the provided path pattern %s (--combinemaps option for 3-D)"
                        % (args[0])
                    )
                    return
                # Prepare variables for the loop section below
                # Get prefix and suffix in cluster volume basename pattern
                # to find the head/tail indices of cluster id substring
                map1_basename_tokens = args[0].split("*")
                # assert (len(map1_basename_tokens) == 2)
                # Find head index of cluster id substring
                cluster_id_substr_head_idx = len(map1_basename_tokens[0])
                suffix_patten = "_cluster*"
            else:  # 2D case or 3D single map case
                input_path_list.append(args[0])
        else:  # 3D two maps case
            input_path_list.append(args[0])

        try:
            e1 = sp_utilities.get_im(input_path_list[0], 0)
        except:
            sp_global_def.ERROR(
                input_path_list[0] + " does not exist (--combinemaps option)"
            )
            return

        nx = e1.get_xsize()
        ny = e1.get_ysize()
        nz = e1.get_zsize()
        log_main.add("----------->>> shell line command <<<-----------------")
        line = " "
        for a in sys.argv:
            line += " " + a
        log_main.add(line)
        if e1.get_zsize() == 1:  # 2D case
            log_main.add("-------->>> Settings given by all options <<<-------")
            log_main.add("pixel_size        :" + str(options.pixel_size))
            log_main.add("mask              :" + str(options.mask))
            log_main.add("B_enhance         :" + str(options.B_enhance))
            log_main.add("low_pass_filter   :" + str(options.fl))
            log_main.add("B_start           :" + str(options.B_start))
            log_main.add("B_stop            :" + str(options.B_stop))
            # log_main.add("randomphasesafter "+str(options.randomphasesafter))
            log_main.add("------------>>> processing <<<-----------------------")
            log_main.add("2-D combinemaps for ISAC averaged images")

            if nargs > 1:
                sp_global_def.ERROR("Too many inputs! (--combinemaps option for 2-D)")
                return
            else:  # NOTE: This should be unreachable
                sp_global_def.ERROR(
                    "Incorrected number of inputs (--combinemaps option for 2-D)"
                )
                return

            nimage = EMAN2_cppwrap.EMUtil.get_image_count(input_path_list[0])
            if options.mask != None:
                try:
                    m = sp_utilities.get_im(options.mask)
                    log_main.add("user provided mask is %s" % options.mask)
                except:
                    sp_global_def.ERROR(
                        "Mask image %s does not exists (--combinemaps for 2-D)"
                        % options.mask
                    )
                    return
            else:
                m = None
                log_main.add("Mask is not used")
            log_main.add("Total number of average images is %d" % nimage)
            for i in range(nimage):
                e1 = sp_utilities.get_im(input_path_list[0], i)
                if m:
                    e1 *= m
                if options.B_enhance == 0.0 or options.B_enhance == -1.0:
                    guinierline = sp_fundamentals.rot_avg_table(
                        sp_morphology.power(EMAN2_cppwrap.periodogram(e1), 0.5)
                    )
                    if options.B_stop == 0.0:
                        freq_max = old_div(1.0, (2.0 * options.pixel_size))
                    else:
                        freq_max = old_div(1.0, options.B_stop)
                    freq_min = old_div(1.0, options.B_start)
                    log_main.add(
                        "B-factor exp(-B*s^2) is estimated from %f[A] to %f[A]"
                        % (options.B_start, 2 * options.pixel_size)
                    )
                    b, junk, ifreqmin, ifreqmax = sp_morphology.compute_bfactor(
                        guinierline, freq_min, freq_max, options.pixel_size
                    )
                    global_b = b * 4
                    log_main.add(
                        "The estimated slope of rotationally averaged Fourier factors  of the summed volumes is %f"
                        % round(-b, 2)
                    )
                else:
                    global_b = options.B_enhance
                    log_main.add("User provided B_factor is %f" % global_b)
                sigma_of_inverse = numpy.sqrt(old_div(2.0, global_b))
                e1 = sp_filter.filt_gaussinv(e1, sigma_of_inverse)
                if options.fl > 0.0 and options.fl < 0.5:
                    log_main.add(
                        "Low-pass filter ff %   aa  %f" % (options.fl, options.aa)
                    )
                    e1 = sp_filter.filt_tanl(e1, options.fl, options.aa)
                elif options.fl > 0.5:
                    e1 = sp_filter.filt_tanl(
                        e1, old_div(options.pixel_size, options.fl), options.aa
                    )
                e1.write_image(options.output)

        else:  # 3D case High pass filter should always come along with low-pass filter.
            log_main.add("-------->>>Settings given by all options <<<-------")
            log_main.add("Pixel_size        :" + str(options.pixel_size))
            log_main.add("Mask              :" + str(options.mask))
            log_main.add("Fsc_adj           :" + str(options.fsc_adj))
            log_main.add("B_enhance         :" + str(options.B_enhance))
            log_main.add("Low_pass_filter   :" + str(options.fl))
            log_main.add("Aa                :" + str(options.aa))
            log_main.add("B_start           :" + str(options.B_start))
            log_main.add("B_stop            :" + str(options.B_stop))
            log_main.add("Mtf               :" + str(options.mtf))
            log_main.add("Output            :" + str(options.output))
            log_main.add("Do_adaptive_mask  :" + str(options.do_adaptive_mask))
            log_main.add("threshold    :" + str(options.threshold))
            log_main.add("nsigma    :" + str(options.nsigma))
            log_main.add("mol_mass    :" + str(options.mol_mass))
            log_main.add("Edge width        :" + str(options.edge_width))
            log_main.add("Ndilation         :" + str(options.ndilation))
            log_main.add("Do approximation         :" + str(options.do_approx))
            # log_main.add("randomphasesafter :"+str(options.randomphasesafter))
            log_main.add("------------->>> processing <<<-----------------------")
            log_main.add("3-D refinement combinemaps")
            single_map = True
            map2_path = None
            if nargs == 1:  # 3D single map case or 3D single maps case
                log_main.add("Combinemaps has single input map")
                # Check error condition of input 3D density map file path list
                if options.fl == 0.0:
                    sp_global_def.ERROR(
                        "Low-pass filter to resolution (--fl=0.0) cannot be used with cluster volumes mode (--combinemaps option for 3-D)"
                    )
                    return
                if options.B_enhance == 0.0:
                    sp_global_def.ERROR(
                        "Automatic B-factor estimation (--B_enhance=0.0) cannot be used with cluster volumes mode (--combinemaps option for 3-D)"
                    )
                    return

                if len(input_path_list) > 1:
                    log_main.add(
                        "Using 3D density map path pattern (found %d files in %s)"
                        % (len(input_path_list), os.path.dirname(args[0]))
                    )
                else:
                    log_main.add("Using a single file path")
            elif nargs == 2:  # 3D two maps case
                log_main.add("Combinemaps has two input maps")
                single_map = False
                map2_path = args[1]
            elif nargs >= 3:
                sp_global_def.ERROR(
                    "Too many input maps! (--combinemaps option for 3-D)"
                )
                return
            else:  # This should be unreachable
                sp_global_def.ERROR(
                    "Incorrected number of inputs (--combinemaps option for 3-D)"
                )
                return

            for map1_path in input_path_list:
                log_main.add("-------------------------------------------------------")
                log_main.add(
                    "------------->>> %s <<<-----------------------" % map1_path
                )
                log_main.add("The first input volume: %s" % map1_path)
                try:
                    map1 = sp_utilities.get_im(map1_path)
                except:
                    sp_global_def.ERROR(
                        "Sphire combinemaps fails to read the first map "
                        + map1_path
                        + "(--combinemaps option for 3-D)"
                    )
                    return

                if single_map:
                    log_main.add("No second input volume")
                else:
                    log_main.add("The second input volume: %s" % map2_path)
                    try:
                        map2 = sp_utilities.get_im(map2_path)
                    except:
                        sp_global_def.ERROR(
                            "Sphire combinemaps fails to read the second map "
                            + map2_path
                            + "(--combinemaps option for 3-D)"
                        )
                        return

                    if (
                        (map2.get_xsize() != map1.get_xsize())
                        or (map2.get_ysize() != map1.get_ysize())
                        or (map2.get_zsize() != map1.get_zsize())
                    ):
                        sp_global_def.ERROR(
                            "Two input maps have different image size (--combinemaps option for 3-D)"
                        )
                        return

                suffix = ""
                if suffix_patten is not None:
                    # Find tail index of micrograph id substring and extract the substring from the micrograph name
                    cluster_id_substr_tail_idx = map1_path.index(
                        map1_basename_tokens[1]
                    )
                    cluster_id_substr = map1_path[
                        cluster_id_substr_head_idx:cluster_id_substr_tail_idx
                    ]
                    suffix = suffix_patten.replace("*", cluster_id_substr)

                ### # NOTE: Toshio Moriya 2018/01/11
                ### # Remove "enforce low-pass filter"
                ### filter_to_resolution =  False
                ### ### enforce low-pass filter
                ### if options.B_enhance !=-1:
                ### 	if not options.fsc_adj:
                ### 		if options.fl == -1.0:
                ### 			filter_to_resolution = True
                ### 			msg = "low-pass filter is enforeced to turn on"
                ### 		else:
                ### 			msg = "user chooses low-pass filter  %f"%options.fl
                ### 	else:
                ### 		msg = "fsc_adj option works as a low-pass filter"
                ### 	log_main.add(msg)

                ## prepare mask
                if options.mask != None and options.do_adaptive_mask:
                    sp_global_def.ERROR(
                        "Wrong options, use either adaptive_mask or supply a mask (options.mask and options.do_adaptive_mask)"
                    )
                    return

                if options.mask != None:
                    log_main.add("User provided mask: %s" % options.mask)
                    try:
                        m = sp_utilities.get_im(options.mask)
                    except:
                        sp_global_def.ERROR(
                            "Sphire combinemaps fails to read mask file "
                            + options.mask
                            + " (--combinemaps option for 3-D)"
                        )
                        return

                    if (
                        (m.get_xsize() != map1.get_xsize())
                        or (m.get_ysize() != map1.get_ysize())
                        or (m.get_zsize() != map1.get_zsize())
                    ):
                        sp_global_def.ERROR(
                            "Mask file "
                            + options.mask
                            + " has different size than the input image (--combinemaps for mask)"
                        )
                        return

                elif options.do_adaptive_mask:
                    log_main.add("Create an adaptive mask, let's wait...")
                    if single_map:
                        input_vol_mask = map1
                    else:
                        input_vol_mask = old_div((map1 + map2), 2.0)
                    if options.mol_mass > 0.0:
                        density_threshold = input_vol_mask.find_3d_threshold(
                            options.mol_mass, options.pixel_size
                        )
                    else:
                        density_threshold = options.threshold
                    log_main.add(
                        "density_threshold, options.ndilation, options.edge_width %f %5.2f %5.2f"
                        % (density_threshold, options.ndilation, options.edge_width)
                    )
                    if options.edge_type == "cosine":
                        mode = "C"
                    else:
                        mode = "G"
                    m = sp_morphology.adaptive_mask_scipy(
                        input_vol_mask,
                        options.nsigma,
                        density_threshold,
                        options.ndilation,
                        options.edge_width,
                        mode,
                        do_approx=options.do_approx,
                    )
                    m.write_image(
                        os.path.join(
                            options.output_dir, "vol_adaptive_mask%s.hdf" % suffix
                        )
                    )
                else:
                    m = None
                    log_main.add("No mask is applied")
                ## prepare FSC
                resolution_FSC143 = (
                    0.5
                )  # for single volume, this is the default resolution
                resolution_FSChalf = 0.5

                def filter_product(B_factor, pixel_size, cutoff, aa, image_size):
                    def gauss_inverse(x, sigma):
                        omega = old_div(0.5, (sigma * sigma))
                        return numpy.exp(x * omega)

                    def tanhfl(x, cutoff, aa):
                        omega = cutoff
                        cnst = old_div(numpy.pi, (2.0 * omega * aa))
                        v1 = cnst * (x + omega)
                        v2 = cnst * (x - omega)
                        return 0.5 * (numpy.tanh(v1) - numpy.tanh(v2))

                    N = old_div(image_size, 2)
                    sigma_of_inverse = numpy.sqrt(
                        old_div(2.0, (old_div(B_factor, pixel_size ** 2)))
                    )
                    values = []
                    if cutoff > 0.5:
                        cutoff = old_div(
                            pixel_size, cutoff
                        )  # always uses absolute frequencies
                    for i in range(N):
                        x = old_div(float(i), float(N * 2.0))
                        values.append(
                            tanhfl(x, cutoff, aa) * gauss_inverse(x, sigma_of_inverse)
                        )
                    index_zero = N + 1
                    for i in range(N):
                        if values[i] == 0.0:
                            index_zero = i
                            break
                    # print("current fall off", (index_zero - cutoff*N*2))
                    return (
                        values,
                        values.index(max(values)),
                        max(values),
                        index_zero,
                        int(index_zero - values.index(max(values))),
                    )

                def calculate_fsc_criterion(fsc, criterion):
                    """
					Calculate fsc for the specified criterion
					"""
                    resolution_left = fsc[0][len(fsc[1]) - 1]
                    idx_crit_left = len(fsc[1]) - 1
                    for ifreq in range(1, len(fsc[1])):
                        if fsc[1][ifreq] < criterion:
                            resolution_left = fsc[0][ifreq - 1]
                            idx_crit_left = ifreq - 1
                            break
                    resolution_right = fsc[0][1]
                    idx_crit_right = 1
                    for ifreq in reversed(list(range(1, len(fsc[1])))):
                        if fsc[1][ifreq] >= criterion:
                            resolution_right = fsc[0][ifreq]
                            idx_crit_right = ifreq
                            break
                    return (
                        resolution_left,
                        resolution_right,
                        idx_crit_left,
                        idx_crit_right,
                    )

                def scale_fsc(x):
                    """
					Scale function to adjust the FSC to the full dataset
					"""
                    return old_div(2.0 * x, (1 + x))

                def create_xml(output_dir, fsc, resolution, name):
                    data = []
                    for ifreq, value in enumerate(fsc[1]):
                        data.append([1/resolution[ifreq], value])

                    fsc_values = ['<fsc title="SPHIRE masked-corrected {} FSC" xaxis="Resolution (A-1)" yaxis="Correlation Coefficient">'.format(name)]
                    for x, y in data:
                        fsc_values.append('\n  <coordinate>\n    <x>{0}</x>\n    <y>{1}</y>\n  </coordinate>'.format(x, y))
                    fsc_values.append('\n</fsc>')
                    with open(os.path.join(output_dir, '{0}.xml'.format(name)), 'w') as write:
                        write.write(''.join(fsc_values))

                def create_fsc_txt(output_dir, fsc, resolution, name):
                    """
					Create a text file based on the fsc
					"""
                    fsc_out = []
                    for ifreq, value in enumerate(fsc[1]):
                        fsc_out.append(
                            "%5d   %7.2f   %7.3f" % (ifreq, resolution[ifreq], value)
                        )
                    sp_utilities.write_text_file(
                        fsc_out, os.path.join(output_dir, "{0}.txt".format(name))
                    )

                def freq_to_angstrom(values, pixel_size):
                    """
					Convert spatial frequency to angstrom
					"""
                    if type(values) != list:
                        values = [values]
                    angstrom = [999.0] * len(values)
                    for i, q in enumerate(values):
                        if q > 0.0:
                            angstrom[i] = old_div(pixel_size, q)
                    return angstrom

                ### for two maps
                dip_at_fsc = False
                if not single_map:
                    # Plot FSC curves and write output fsc files
                    matplotlib.pylab.matplotlib.use("Agg")
                    matplotlib.pylab.plt.rcParams["font.family"] = "monospace"
                    title = []

                    # Output curves lists
                    plot_curves = []
                    plot_names = []
                    plot_title = []

                    # Output curves
                    fsc_true = sp_statistics.fsc(map1, map2, 1)
                    fsc_true[1][0] = 1.0  # always reset fsc of zero frequency as 1.0
                    plot_curves.append(fsc_true)
                    plot_names.append(r"halves")
                    # map fsc obtained from halves to full maps
                    plot_curves.append([fsc_true[0], list(map(scale_fsc, fsc_true[1]))])
                    plot_names.append(r"full")
                    if m is not None:
                        fsc_mask = sp_statistics.fsc(map1 * m, map2 * m, 1)
                        fsc_mask[1][
                            0
                        ] = 1.0  # always reset fsc of zero frequency to 1.0
                        plot_curves.append(fsc_mask)
                        plot_names.append(r"masked halves")
                        # map fsc obtained from masked two halves to full maps
                        plot_curves.append(
                            [fsc_mask[0], list(map(scale_fsc, fsc_mask[1]))]
                        )
                        plot_names.append(r"masked full")

                    resolution_in_angstrom = freq_to_angstrom(
                        pixel_size=options.pixel_size, values=fsc_true[0]
                    )

                    # Create plot and write output file
                    minimum_fsc = 0
                    color = ["blue", "grey", "orange", "lightgrey"]
                    idx = -1
                    thicknesslist = [2.5, 0.7, 2.5, 0.7]
                    for fsc, name in zip(plot_curves, plot_names):
                        idx += 1
                        fsc[1][0] = 1
                        label = r"FSC {0:14s}:  $0.5$: ${1: 5.2f}\AA$  |  $0.143$: ${2: 5.2f}\AA$".format(
                            name,
                            round(
                                freq_to_angstrom(
                                    pixel_size=options.pixel_size,
                                    values=calculate_fsc_criterion(fsc, criterion=0.5)[
                                        0
                                    ],
                                )[0],
                                2,
                            ),
                            round(
                                freq_to_angstrom(
                                    pixel_size=options.pixel_size,
                                    values=calculate_fsc_criterion(
                                        fsc, criterion=0.143
                                    )[0],
                                )[0],
                                2,
                            ),
                        )
                        matplotlib.pylab.plt.plot(
                            fsc[0],
                            fsc[1],
                            label=label,
                            color=color[idx],
                            linewidth=thicknesslist[idx],
                        )
                        create_fsc_txt(
                            output_dir=options.output_dir,
                            fsc=fsc,
                            resolution=resolution_in_angstrom,
                            name=name.replace(" ", "_").lower(),
                        )
                        create_xml(
                            output_dir=options.output_dir,
                            fsc=fsc,
                            resolution=resolution_in_angstrom,
                            name=name.replace(" ", "_").lower(),
                        )
                        log_main.add(
                            "Resolution: {0}".format(
                                label.replace("$", "").replace("\A", "")
                            )
                        )
                        if min(fsc[1]) < minimum_fsc:
                            minimum_fsc = min(fsc[1])
                    matplotlib.pylab.plt.axhline(0.143, 0, 1, color="k", alpha=0.3)
                    matplotlib.pylab.plt.axhline(0.5, 0, 1, color="k", alpha=0.3)

                    # Ticks
                    nyquist_resolution = resolution_in_angstrom[-1]
                    raw_x_ticks_ang = [
                        int(round(old_div(options.pixel_size, float(entry)), 0))
                        for entry in [0.1, 0.2, 0.3, 0.4, 0.5]
                    ]
                    x_ticks_ang = [
                        r"$\frac{{1}}{{{0}}}$".format(tick)
                        for tick in raw_x_ticks_ang if tick > nyquist_resolution * 1.03
                    ]
                    x_ticks_freq = [
                        old_div(options.pixel_size, float(tick))
                        for tick in raw_x_ticks_ang if tick > nyquist_resolution * 1.03
                    ]
                    x_ticks_ang.insert(0, r"$0$")
                    x_ticks_freq.insert(0, 0)
                    x_ticks_ang.append(
                        r"$\frac{{1}}{{{0}}}$".format(round(nyquist_resolution, 2))
                    )
                    x_ticks_freq.append(
                        old_div(options.pixel_size, round(nyquist_resolution, 2))
                    )
                    matplotlib.pylab.plt.xticks(
                        x_ticks_freq, x_ticks_ang, size="xx-large"
                    )
                    y_ticks = [-0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0]
                    matplotlib.pylab.plt.yticks(
                        y_ticks,
                        [r"${0}$".format(tick) for tick in y_ticks],
                        size="large",
                    )

                    # Plot related settings
                    matplotlib.pylab.plt.legend(
                        loc="lower left",
                        bbox_to_anchor=(0, 1, 1, 0.2),
                        mode="expand",
                        frameon=False,
                    )
                    matplotlib.pylab.plt.text(
                        0.005, 0.153, r"$0.143$", color="k", alpha=0.4
                    )
                    matplotlib.pylab.plt.text(
                        0.005, 0.51, r"$0.5$", color="k", alpha=0.4
                    )
                    matplotlib.pylab.plt.xlabel(r"Spatial frequency / $\frac{1}{\AA}$")
                    matplotlib.pylab.plt.ylabel(r"FSC")
                    matplotlib.pylab.plt.ylim([minimum_fsc - 0.05, 1.05])
                    matplotlib.pylab.plt.grid()
                    matplotlib.pylab.plt.tight_layout()
                    matplotlib.pylab.plt.savefig(
                        os.path.join(options.output_dir, "fsc.png"), bbox_inches="tight"
                    )
                    matplotlib.pylab.plt.clf()

                    if m is not None:
                        fsc_true = fsc_mask
                    """Multiline Comment1"""
                    # log_main.add("Adjust FSC to the full dataset by: 2.*FSC/(FSC+1.)")
                    # fsc_true[1] = list(map(scale_fsc, fsc_true[1]))

                    ## Determine 05/143 resolution from corrected FSC, RH correction of FSC from masked volumes
                    resolution_FSC143_right = 0.0
                    resolution_FSC143_left = 0.0
                    # dip_at_fsc = False
                    nfreq0 = 1

                    # for ifreq in range(1, len(fsc_true[1])):
                    # 	if fsc_true[1][ifreq] < 0.0:
                    # 		nfreq0  = ifreq - 1
                    # 		break
                    # if nfreq0 ==1: nfreq0= len(fsc_true[1]) - 1
                    #
                    # nfreq05 = len(fsc_true[1])-1
                    # for ifreq in range(1, len(fsc_true[1])):
                    # 	if fsc_true[1][ifreq] < 0.5:
                    # 		resolution_FSChalf = fsc_true[0][ifreq-1]
                    # 		nfreq05 = ifreq-1
                    # 		break
                    #
                    # resolution_FSC143_left = fsc_true[0][len(fsc_true[1])-1]
                    # for ifreq in range(nfreq05, len(fsc_true[1])):
                    # 	if fsc_true[1][ifreq] < 0.143:
                    # 		resolution_FSC143_left = fsc_true[0][ifreq-1]
                    # 		nfreq143 = ifreq - 1
                    # 		break
                    #
                    # resolution_FSC143_right = fsc_true[0][nfreq05]
                    # nfreq143_right = nfreq05
                    # for ifreq in range(nfreq0, nfreq05, -1):
                    # 	if fsc_true[1][ifreq] >= 0.143:
                    # 		resolution_FSC143_right = fsc_true[0][ifreq]
                    # 		nfreq143_right = ifreq
                    # 		break
                    resolution_FSChalf, _, nfreq05, _ = calculate_fsc_criterion(
                        fsc_true, 0.5
                    )
                    resolution_FSC143_left, resolution_FSC143_right, nfreq143, nfreq143_right = calculate_fsc_criterion(
                        fsc_true, 0.143
                    )

                    ## output resolution
                    if resolution_FSC143_left != resolution_FSC143_right:
                        log_main.add(
                            "there is a dip between 0.5 to 0.143 in FSC! Please check the FSC curve and your mask carefully."
                        )
                    else:
                        log_main.add("Fsc smoothly falls from 0.5 to 0.143 !")

                    resolution_FSC143 = resolution_FSC143_left
                    nfreq143 = nfreq143

                    for ifreq in range(len(fsc_true[0])):
                        fsc_true[1][ifreq] = max(fsc_true[1][ifreq], 0.0)
                    ## smooth FSC after FSC143 and set other values to zero
                    for ifreq in range(nfreq143 + 1, len(fsc_true[1])):
                        if ifreq == nfreq143 + 1:
                            fsc_true[1][ifreq] = old_div(
                                fsc_true[1][nfreq143 - 2] + fsc_true[1][nfreq143 - 1],
                                5.0,
                            )
                        elif ifreq == nfreq143 + 2:
                            fsc_true[1][ifreq] = old_div(
                                fsc_true[1][nfreq143 - 1], 5.0
                            )
                        else:
                            fsc_true[1][ifreq] = 0.0
                    EMAN2_cppwrap.Util.add_img(map1, map2)
                    del map2
                    EMAN2_cppwrap.Util.mul_scalar(map1, 0.5)

                outtext = [["Squaredfreq"], ["LogOrig"]]
                guinierline = sp_fundamentals.rot_avg_table(
                    sp_morphology.power(EMAN2_cppwrap.periodogram(map1), 0.5)
                )
                for ig in range(len(guinierline)):
                    x = old_div(
                        old_div(ig * 0.5, float(len(guinierline))), options.pixel_size
                    )
                    outtext[0].append("%10.6f" % (x * x))
                    outtext[1].append("%10.6f" % numpy.log(guinierline[ig]))

                # starts adjustment of powerspectrum
                if options.mtf:  # MTF division #1
                    log_main.add("MTF correction is applied")
                    log_main.add("MTF file is %s" % options.mtf)
                    try:
                        mtf_core = sp_utilities.read_text_file(options.mtf, -1)
                    except:
                        sp_global_def.ERROR(
                            "Sphire combinemaps fails to read MTF file "
                            + options.mtf
                            + " (--combinemaps option for 3-D)"
                        )
                    map1 = sp_fundamentals.fft(
                        EMAN2_cppwrap.Util.divide_mtf(
                            sp_fundamentals.fft(map1), mtf_core[1], mtf_core[0]
                        )
                    )
                    outtext.append(["LogMTFdiv"])
                    guinierline = sp_fundamentals.rot_avg_table(
                        sp_morphology.power(EMAN2_cppwrap.periodogram(map1), 0.5)
                    )
                    for ig in range(len(guinierline)):
                        outtext[-1].append("%10.6f" % numpy.log(guinierline[ig]))
                else:
                    log_main.add("MTF is not applied")

                if options.fsc_adj and not single_map:  # limit resolution #2
                    log_main.add(
                        "Sqrt(FSC) is multiplied to adjust power spectrum of the summed volumes"
                    )
                    # log_main.add("Notice: FSC adjustment of powerspectrum will increase B-factor 2-3 times than not!")
                    #### FSC adjustment ((2.*fsc)/(1+fsc)) to the powerspectrum;
                    fil = len(fsc_true[1]) * [None]
                    for i in range(len(fil)):
                        fil[i] = numpy.sqrt(
                            fsc_true[1][i]
                        )  # fsc already matched to full dataset
                    map1 = sp_filter.filt_table(map1, fil)
                    guinierline = sp_fundamentals.rot_avg_table(
                        sp_morphology.power(EMAN2_cppwrap.periodogram(map1), 0.5)
                    )
                    outtext.append(["LogFSCadj"])
                    for ig in range(len(guinierline)):
                        outtext[-1].append("%10.6f" % numpy.log(guinierline[ig]))
                else:
                    log_main.add("Fsc_adj is not applied")

                map1 = sp_fundamentals.fft(map1)

                if (
                    options.B_enhance != -1
                ):  # 3 One specifies and then apply B-factor sharpen
                    if options.B_enhance == 0.0:  # auto mode

                        cutoff_by_fsc = 0
                        for ifreq in range(len(fsc_true[1])):
                            if fsc_true[1][ifreq] < 0.143:
                                break
                        cutoff_by_fsc = float(ifreq - 1)
                        freq_max = old_div(
                            old_div(cutoff_by_fsc, (2.0 * len(fsc_true[0]))),
                            options.pixel_size,
                        )
                        guinierline = sp_fundamentals.rot_avg_table(
                            sp_morphology.power(EMAN2_cppwrap.periodogram(map1), 0.5)
                        )
                        logguinierline = []
                        spfreq = []
                        for ig in range(len(guinierline)):
                            logguinierline.append(numpy.log(guinierline[ig]))
                            spfreq.append(
                                old_div(
                                    float(ig),
                                    (2 * (len(guinierline) - 1) * options.pixel_size),
                                )
                            )
                        freq_min = old_div(
                            1.0, options.B_start
                        )  # given frequencies in Angstrom unit, say, B_start is 10 Angstrom, or 15  Angstrom
                        if options.B_stop != 0.0:
                            freq_max = old_div(1.0, options.B_stop)
                        if freq_min >= freq_max:
                            log_main.add(
                                "B_start is too high! Decrease it and rerun the program!"
                            )
                            sp_global_def.ERROR(
                                "B_start is too high! Decrease it and re-run the program! (--combinemaps option)"
                            )
                            return

                        # Write Guinier plot
                        guinierzip = [spfreq, guinierline, logguinierline]
                        sp_utilities.write_text_file(
                            guinierline,
                            os.path.join(options.output_dir, "guinierplot.txt"),
                        )  # write power and log
                        matplotlib.pylab.plt.title("Guinier plot")
                        matplotlib.pylab.plt.plot(spfreq, logguinierline)
                        raw_x_ticks_ang = [
                            int(round(old_div(options.pixel_size, float(entry)), 0))
                            for entry in [0.1, 0.2, 0.3, 0.4, 0.5]
                        ]
                        nyquist_resolution = options.pixel_size * 2
                        x_ticks_ang = [
                            r"$\frac{{1}}{{{0}}}$".format(tick)
                            for tick in raw_x_ticks_ang
                            if tick > nyquist_resolution * 1.03
                        ]
                        x_ticks_freq = [
                            old_div(options.pixel_size, float(tick))
                            for tick in raw_x_ticks_ang
                            if tick > nyquist_resolution * 1.03
                        ]
                        x_ticks_ang.insert(0, r"$0$")
                        x_ticks_freq.insert(0, 0)
                        x_ticks_ang.append(
                            r"$\frac{{1}}{{{0}}}$".format(round(nyquist_resolution, 2))
                        )
                        x_ticks_freq.append(
                            old_div(options.pixel_size, round(nyquist_resolution, 2))
                        )
                        matplotlib.pylab.plt.xticks(
                            x_ticks_freq, x_ticks_ang, size="xx-large"
                        )
                        matplotlib.pylab.plt.legend(
                            loc="lower left",
                            bbox_to_anchor=(0, 1, 1, 0.2),
                            mode="expand",
                            frameon=False,
                        )
                        matplotlib.pylab.plt.xlabel(
                            r"Spatial frequency / $\frac{1}{\AA}$"
                        )
                        matplotlib.pylab.plt.ylabel(r"log(I)")
                        matplotlib.pylab.plt.grid()
                        matplotlib.pylab.plt.tight_layout()
                        matplotlib.pylab.plt.savefig(
                            os.path.join(options.output_dir, "guinierplot.png"),
                            bbox_inches="tight",
                        )

                        b, junk, ifreqmin, ifreqmax = sp_morphology.compute_bfactor(
                            guinierline, freq_min, freq_max, options.pixel_size
                        )
                        global_b = 4.0 * b  # Just a convention!
                        cc = sp_statistics.pearson(junk[1], logguinierline)
                        log_main.add(
                            "Similarity between the fitted line and 1-D rotationally average power spectrum within [%d, %d] is %5.3f"
                            % (
                                ifreqmin,
                                ifreqmax,
                                sp_statistics.pearson(
                                    junk[1][ifreqmin:ifreqmax],
                                    logguinierline[ifreqmin:ifreqmax],
                                ),
                            )
                        )
                        if b < 0:
                            log_main.add(
                                "WARNING! B-factor slope %6.2f[A^2] is positive. Inverse B-factor will NOT be applied."
                                % (round(-b, 2))
                            )
                            log_main.add(
                                "WARNING! Check the Guinier plot, and try to diagnose the problem."
                            )
                            return
                        else:
                            log_main.add("The slope is %6.2f[A^2]" % (round(-b, 2)))

                        sigma_of_inverse = numpy.sqrt(
                            old_div(2.0, (old_div(global_b, options.pixel_size**2) ))
                        )

                    else:  # User provided value
                        # log_main.add( " apply user provided B-factor to enhance map!")
                        log_main.add(
                            "User-provided B-factor is %6.2f[A^2]" % options.B_enhance
                        )
                        sigma_of_inverse = numpy.sqrt(
                            old_div(
                                2.0,
                                (
                                    old_div(
                                        abs(options.B_enhance), options.pixel_size**2
                                    )

                                ),
                            )
                        )
                        global_b = options.B_enhance

                    map1 = (sp_filter.filt_gaussinv(map1, sigma_of_inverse))
                    guinierline = sp_fundamentals.rot_avg_table(
                        sp_morphology.power(EMAN2_cppwrap.periodogram(map1), 0.5)
                    )
                    outtext.append([" LogBfacapplied"])
                    last_non_zero = -999.0
                    for ig in range(len(guinierline)):
                        if guinierline[ig] > 0:
                            outtext[-1].append("%10.6f" % numpy.log(guinierline[ig]))
                            last_non_zero = numpy.log(guinierline[ig])
                        else:
                            outtext[-1].append("%10.6f" % last_non_zero)
                else:
                    log_main.add("B-factor enhancement is not applied to map!")

                cutoff = 0.0
                if not single_map:
                    if options.fl != -1.0:  # User provided low-pass filter #4.
                        if options.fl > 0.5:  # Input is in Angstrom
                            map1 = sp_filter.filt_tanl(
                                map1,
                                old_div(options.pixel_size, options.fl),
                                min(options.aa, 0.1),
                            )
                            cutoff = options.fl
                            log_main.add(
                                "low-pass filter to user-provided %f[A]" % cutoff
                            )
                        elif (
                            options.fl > 0.0 and options.fl < 0.5
                        ):  # input is in absolution frequency
                            map1 = sp_filter.filt_tanl(
                                map1, options.fl, min(options.aa, 0.1)
                            )
                            cutoff = old_div(options.pixel_size, options.fl)
                            log_main.add(
                                "Low-pass filter to user-provided %f[A]" % cutoff
                            )
                        else:  # low-pass filter to resolution determined by FSC0.143
                            map1 = sp_filter.filt_tanl(
                                map1, resolution_FSC143, options.aa
                            )
                            cutoff = old_div(options.pixel_size, resolution_FSC143)
                            log_main.add(
                                "Low-pass filter to FSC0.143 resolution (%f[A])!"
                                % cutoff
                            )
                    else:
                        ### # NOTE: Toshio Moriya 2018/01/11
                        ### # Remove "enforce low-pass filter"
                        ### if filter_to_resolution:
                        ### 	map1   = filt_tanl(map1,resolution_FSC143, options.aa)
                        ### 	cutoff = options.pixel_size/resolution_FSC143
                        ### 	log_main.add("low-pass filter to FSC0.143 resolution (%f[A])!"%cutoff)
                        ### else:
                        ### 	cutoff = 0.0
                        ### 	log_main.add("low-pass filter is not applied to map!")
                        log_main.add("Low-pass filter is not applied to map!")
                else:
                    if options.fl == -1.0:
                        log_main.add(
                            "There is no low-pass filtration in single map enhancement"
                        )
                    else:
                        if options.fl > 0.5:  # Input is in Angstrom
                            map1 = sp_filter.filt_tanl(
                                map1,
                                old_div(options.pixel_size, options.fl),
                                min(options.aa, 0.1),
                            )
                            cutoff = options.fl
                        else:
                            sp_global_def.ERROR(
                                "Incorrect low-pass filter value, it should be in Angstroms (combinemaps)"
                            )
                            return

                        log_main.add("Low-pass filter to user provided %f[A]" % cutoff)

                map1 = sp_fundamentals.fft(map1)
                file_name, file_ext = os.path.splitext(options.output)
                if file_ext == "":
                    file_ext = ".hdf"
                file_path_nomask = os.path.join(
                    options.output_dir, file_name + suffix + "_nomask" + file_ext
                )
                map1.write_image(file_path_nomask)
                log_main.add(
                    "The enhanced map without masking is saved as %s"
                    % (file_path_nomask)
                )

                if m:
                    map1 *= m
                else:
                    log_main.add("The final map is not masked!")

                file_path_final = os.path.join(
                    options.output_dir, file_name + suffix + file_ext
                )
                map1.write_image(file_path_final)
                log_main.add("---------- >>> Summary <<<------------")
                if not single_map:
                    log_main.add(
                        "Final resolution 0.5/0.143 is %5.2f/%5.2f[A]"
                        % (
                            round((old_div(options.pixel_size, resolution_FSChalf)), 3),
                            round((old_div(options.pixel_size, resolution_FSC143)), 3),
                        )
                    )
                    if dip_at_fsc:
                        log_main.add(
                            "There is a dip in the fsc curve in the region between 0.5 and 0.143. Please check the FSC curve and mask."
                        )

                if options.B_enhance != -1:
                    log_main.add("B-factor is %6.2f[A^2]" % (round((-global_b), 2)))
                else:
                    log_main.add("B-factor is not applied")

                if not single_map:
                    output_names = [
                        plot_name.replace(" ", "_") for plot_name in plot_names
                    ]
                    log_main.add(
                        "FSC curves are saved in {0}.txt ".format(
                            ".txt, ".join(output_names).lower()
                        )
                    )

                log_main.add("The final volume is " + file_path_final)
                file_path_guinierlines = os.path.join(
                    options.output_dir, "guinierlines" + suffix + ".txt"
                )
                log_main.add(
                    "Guinierlines in logscale are saved in " + file_path_guinierlines
                )

                if options.fl != -1:
                    log_main.add(
                        "Tanl low-pass filter is applied using cutoff frequency 1/%5.2f[1/A]"
                        % round(cutoff, 2)
                    )
                else:
                    log_main.add("The final volume is not low-pass filtered. ")

                sp_utilities.write_text_file(outtext, file_path_guinierlines)

                # evaluation of enhancement: values, values.index(max(values)), max(values), index_zero, int(index_zero - cutoff*N*2)
                if cutoff != 0.0:
                    pvalues, mindex, mavlue, index_zero, pfall_off = filter_product(
                        global_b,
                        options.pixel_size,
                        cutoff,
                        options.aa,
                        map1.get_xsize(),
                    )
                    log_main.add("---->>> Analysis of enhancement <<<-----")
                    ### log_main.add("B_factor:  %f   cutoff:   %f[A]  (%f[absolute]) aa: [absolute]:  %f  Maximum enhancement ocurs in %d pixels. Maximum enhancement ratio is %f. After %d pixel, power spectrum is set to zero. Falloff width is %d pixels"%\
                    ###    (global_b, cutoff, options.pixel_size/cutoff, options.aa, mindex, mavlue, index_zero, pfall_off))
                    log_main.add("B_factor                     :  %f" % (global_b))
                    log_main.add(
                        "Low-pass filter cutoff       :  %f[A] (%f[absolute])"
                        % (cutoff, old_div(options.pixel_size, cutoff))
                    )
                    log_main.add(
                        "Low-pass filter falloff      :  %f[absolute]" % (options.aa)
                    )
                    log_main.add(
                        "Max enhancement point        :  %d[pixels]" % (mindex)
                    )
                    log_main.add("Max enhancement ratio        :  %f" % (mavlue))
                    log_main.add(
                        "First zero pw spectrum point :  %d[pixels]" % (index_zero)
                    )
                    log_main.add(
                        "Falloff width                :  %d[pixels]" % (pfall_off)
                    )
                    if mindex == 0:
                        msg = "Enhancement has no maximum value. Inspect the mask, reduce aa, or decrease fl to a lower frequency, and then rerun the command "
                        log_main.add(msg)
                    if index_zero > old_div(map1.get_xsize(), 2):
                        msg = "Enhancement exceeds Nyquist frequency. Inspect the mask, decrease fl to a lower frequency, or reduce aa, and then rerun the command "
                        log_main.add(msg)
            log_main.add("-------------------------------------------------------")
            log_main.add("----------             >>> DONE <<<        ------------")
            log_main.add("-------------------------------------------------------")

    elif options.window_stack:
        nargs = len(args)
        if nargs == 0:
            print("  window images in a stack")
            return
        else:
            output_stack_name = None
            inputstack = args[0]
            if nargs == 2:
                output_stack_name = args[1]
            input_path, input_file_name = os.path.split(inputstack)
            input_file_name_root, ext = os.path.splitext(input_file_name)
            if input_file_name_root[0:3] == "bdb":
                stack_is_bdb = True
            else:
                stack_is_bdb = False
            if output_stack_name is None:
                if stack_is_bdb:
                    output_stack_name = "bdb:window_" + input_file_name_root[4:]
                else:
                    output_stack_name = (
                        "window_" + input_file_name_root + ".hdf"
                    )  # Only hdf file is output.
            nimage = EMAN2_cppwrap.EMUtil.get_image_count(inputstack)
            for i in range(nimage):
                im = sp_utilities.get_im(inputstack, i)
                if i == 0:
                    if im.get_xsize() < options.box:
                        sp_global_def.ERROR(
                            "New image size has to be smaller than the original image size",
                            "sxprocess.main",
                            1,
                        )
                    newz = im.get_zsize()
                    if newz > 1:
                        newz = options.box
                im = EMAN2_cppwrap.Util.window(
                    im, options.box, options.box, newz, 0, 0, 0
                )
                im.write_image(output_stack_name, i)

    elif options.pad:
        nargs = len(args)
        if nargs == 0:
            print("  pad images in a stack")
            return
        else:
            output_stack_name = None
            inputstack = args[0]
            if nargs == 2:
                output_stack_name = args[1]
            input_path, input_file_name = os.path.split(inputstack)
            input_file_name_root, ext = os.path.splitext(input_file_name)
            if input_file_name_root[0:3] == "bdb":
                stack_is_bdb = True
            else:
                stack_is_bdb = False
            if output_stack_name is None:
                if stack_is_bdb:
                    output_stack_name = "bdb:pad_" + input_file_name_root[4:]
                else:
                    output_stack_name = (
                        "pad_" + input_file_name_root + ".hdf"
                    )  # Only hdf file is output.
            nimage = EMAN2_cppwrap.EMUtil.get_image_count(inputstack)
            for i in range(nimage):
                im = sp_utilities.get_im(inputstack, i)
                if i == 0:
                    if im.get_xsize() > options.box:
                        sp_global_def.ERROR(
                            "New image size has to be larger than the original image size"
                        )
                        return

                    newz = im.get_zsize()

                    if newz > 1:
                        newz = options.box
                sp_utilities.pad(
                    im, options.box, options.box, newz, float(options.background)
                ).write_image(output_stack_name, i)

    elif options.angular_distribution:
        nargs = len(args)
        if nargs > 1:
            sp_global_def.ERROR(
                "Too many inputs are given, see usage and restart the program!"
            )
            return
        else:
            if not os.path.exists(args[0]):
                sp_global_def.ERROR(
                    "Params file does not exists! Please rename and restart the program."
                )
                return
            strInput = args[0]
            strOutput = strInput[: -len(strInput.split("/")[-1])] + "distribution.bild"
            if options.pixel_size == 0:
                options.pixel_size = 1
            sp_utilities.angular_distribution(
                inputfile=strInput, options=options, output=strOutput
            )

    elif options.subtract_stack:
        nargs = len(args)

        if nargs < 2 or nargs > 4:
            sp_global_def.ERROR(
                "Three stack names required, see usage and restart the program! (options.subtract_stack)"
            )
            return

        else:
            minuend_stack = args[0]
            subtrahend_stack = args[1]
            result_stack = args[2]

            nimages = EMAN2_cppwrap.EMUtil.get_image_count(minuend_stack)
            mimages = EMAN2_cppwrap.EMUtil.get_image_count(subtrahend_stack)

            if nimages != mimages:
                sp_global_def.ERROR(
                    "Two input stacks have different number of images (options.subtract_stack)"
                )
                return

            else:
                for im in range(nimages):
                    image = sp_utilities.get_im(minuend_stack, im)
                    simage = sp_utilities.get_im(subtrahend_stack, im)
                    if options.normalize:
                        if im == 0:
                            radius = old_div(image.get_xsize(), 2) - 1
                            mask = sp_utilities.model_circle(
                                radius, image.get_xsize(), image.get_ysize()
                            )
                        st = EMAN2_cppwrap.Util.infomask(image, mask, False)
                        image -= st[0]
                        image = old_div(image, st[1])

                    ssimage = EMAN2_cppwrap.Util.subn_img(image, simage)

                    try:
                        ctf = image.get_attr("ctf")
                        ssimage.set_attr("ctf_applied", 0)
                        ssimage.set_attr("ctf", ctf)
                    except:
                        pass
                    try:
                        ctf = image.get_attr("xform.projection")
                        ssimage.set_attr("xform.projection", ctf)
                    except:
                        pass

                    ssimage.write_image(result_stack, im)

    elif options.balance_angular_distribution:
        resulting_index, resulting_params = sp_utilities.balance_angular_distribution(
            sp_utilities.read_text_row(args[0]),
            options.max_occupy,
            options.angstep,
            options.symmetry,
        )
        sp_utilities.write_text_file(resulting_index, args[1])
        sp_utilities.write_text_row(resulting_params.tolist(), args[1] + "_params.txt")
    else:
        sp_global_def.ERROR("Please provide option name")
        return


def main():
    sp_global_def.print_timestamp("Start")
    run()
    sp_global_def.print_timestamp("Finish")

if __name__ == "__main__":
    main()
