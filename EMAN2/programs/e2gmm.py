#!/usr/bin/env python
#
# Author: Steven Ludtke  9/28/2020 
# Copyright (c) 2020- Baylor College of Medicine
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307 USA
#
# This is a basic GUI for Muyuan's GMM processing

from past.utils import old_div
from future import standard_library
standard_library.install_aliases()
from builtins import range
import sys
import os
import weakref
import threading
import time
from sys import argv
from EMAN2 import *
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt,QTimer
from eman2_gui.emapplication import get_application, EMApp
from eman2_gui.emplot2d import EMPlot2DWidget
from eman2_gui.emimage2d import EMImage2DWidget
from eman2_gui.emscene3d import EMScene3D
from eman2_gui.emshape import EMShape
from eman2_gui.emshapeitem3d import EMScatterPlot3D
from eman2_gui.emdataitem3d import EMDataItem3D,EMIsosurface
from eman2_gui.valslider import *
import queue
from eman2_gui import embrowser
import sklearn.decomposition as skdc
import sklearn.manifold as skmf
from queue import Queue
from matplotlib.patches import Circle

os.environ["CUDA_VISIBLE_DEVICES"]='0' 
os.environ["TF_FORCE_GPU_ALLOW_GROWTH"]='true' 
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' #### reduce log output

import traceback
import tensorflow as tf
import tensorflow.keras.models
import numpy as np

def butval(widg):
	if widg.isChecked(): return 1
	return 0

def butstr(widg):
	if widg.isChecked(): return "1"
	return "0"

def good_num(dct):
	"""when passed a dictionary containing numerical (or string numerical) keys, will return the lowest
	"missing" value>=0"""
	if dct is None or len(dct)==0: return 0
	try: ks=set([int(k) for k in dct.keys()])
	except:
		error_exit("dictionary with non-integer keys")

	full=set(range(max(ks)+2))
	return min(full-ks)

# shortcut, if we had a central GUI file like EMAN2.py, it could go there...
def showerror(msg,parent=None):
	QtWidgets.QMessageBox.warning(parent,"ERROR",msg)

def make3d_thr(que,tskn,fsp,imgns,rparms,latent,currun,rad,mmode,nset,ptclsclip=-1):
	"""Thread for 3-D reconstruction in background
	que - Queue to return result in
	tskn - task number to identify result
	fsp - filename of image file with per particle orientations
	imgns - list of image numbers from fsp to include
	rparms - recosntructor parameters, include size,sym,mode,usessnr,verbose
	latent - coordinates of center of set in latent space (numpy array)
	currun - the current run parameters. pass them in case the user selects a different run during reconstruction
	rad - radius of set, passthrough
	mmode - type of classifcation on this set, passthrough
	nset - set number (not map number) passthrough

	returns in que (tskn,% complete, volume (when 100%)
	"""
	
#	print(rparms)
	recone=Reconstructors.get("fourier",rparms)
	recone.setup()
	recono=Reconstructors.get("fourier",rparms)
	recono.setup()
	
	pad=rparms["size"][0]
	
	# read images in chunks of 100 for (maybe) increased efficiency
	for i in range(len(imgns)//100+1):
		que.put((tskn,9500*i//len(imgns),None,None))  # don't want to accidentally return 100, this is for progress display
#		print(fsp,i*100,i*100+100,len(imgns),len(imgns[i*100:i*100+100]))
		imgnsub=imgns[i*100:i*100+100]
		if len(imgnsub)==0 : continue
		imgs=EMData.read_images(fsp,imgnsub)
		for j,im in enumerate(imgs):
			im2=im.get_clip(Region((im["nx"]-pad)//2,(im["ny"]-pad)//2,pad,pad))
			xf=im["xform.projection"]
			trans=xf.get_trans_2d()
			xf.set_trans(-trans[0],-trans[1])
			if j%2==0:
				imf=recone.preprocess_slice(im2,xf)
				recone.insert_slice(imf,xf,1.0)
			else:
				imf=recono.preprocess_slice(im2,xf)
				recono.insert_slice(imf,xf,1.0)
	
	que.put((tskn,96,None,None))  # don't want to accidentally return 100, this is for progress display
	rete=recone.finish(True)
	reto=recono.finish(True)
	rete["apix_x"]=im["apix_x"]
	rete["apix_y"]=im["apix_x"]
	rete["apix_z"]=im["apix_x"]
	reto["apix_x"]=im["apix_x"]
	reto["apix_y"]=im["apix_x"]
	reto["apix_z"]=im["apix_x"]
	que.put((tskn,97,None,None))  # don't want to accidentally return 100, this is for progress display
	fscf=rete.calc_fourier_shell_correlation(reto)
	fsc=fscf[len(fscf)//3:len(fscf)*2//3]
#	print(fsc)
	for r,v in enumerate(fsc[5:]): 
		if v<0.143: break
	r+=5
	que.put((tskn,98,None,None))  # don't want to accidentally return 100, this is for progress display
	ret=rete.copy()
	ret.add(reto)
	ret.process_inplace("filter.lowpass.tophat",{"cutoff_pixels":r})
	if ptclsclip<=0 : ptclsclip=im["nx"]
	ret=ret.get_clip(Region((pad-ptclsclip)//2,(pad-ptclsclip)//2,(pad-ptclsclip)//2,ptclsclip,ptclsclip,ptclsclip))
	rete=rete.get_clip(Region((pad-ptclsclip)//2,(pad-ptclsclip)//2,(pad-ptclsclip)//2,ptclsclip,ptclsclip,ptclsclip))
	reto=reto.get_clip(Region((pad-ptclsclip)//2,(pad-ptclsclip)//2,(pad-ptclsclip)//2,ptclsclip,ptclsclip,ptclsclip))
	que.put((tskn,99,None,None))  # don't want to accidentally return 100, this is for progress display
#	ret=ret.do_ift()
	ret["ptcl_repr"]=len(imgns)
	ret["resolution"]=1.0/fscf[r]
	ret["apix_x"]=im["apix_x"]
	ret["apix_y"]=im["apix_x"]
	ret["apix_z"]=im["apix_x"]
	print(f"Resolution {r} {1.0/fscf[r]}")
			
	que.put((tskn,100,ret,latent,imgns,currun,rad,rete,reto,mmode,nset))

def make3d_thr_fast(que,tskn,fsp,imgns,rparms,latent,currun,rad,mmode,nset,ptclsclip=-1):
	"""Thread for 3-D reconstruction in background
	This version downsamples the data and does some other things to make the reconstruction fast, but of lower quality
	que - Queue to return result in
	tskn - task number to identify result
	fsp - filename of image file with per particle orientations
	imgns - list of image numbers from fsp to include
	rparms - recosntructor parameters, include size,sym,mode,usessnr,verbose
	currun - the current run parameters. pass them in case the user selects a different run during reconstruction
	
	returns in que (tskn,% complete, volume (when 100%)
	"""
	
#	print(rparms)
	rparms["mode"]="nearest_neighbor"
	pad=good_size(rparms["size"][0]//2)
	rparms["size"]=(pad,pad,pad)
	recone=Reconstructors.get("fourier",rparms)
	recone.setup()
	recono=Reconstructors.get("fourier",rparms)
	recono.setup()
	
	
	# read images in chunks of 100 for (maybe) increased efficiency
	for i in range(len(imgns)//100+1):
		que.put((tskn,9900*i//len(imgns),None,None))  # don't want to accidentally return 100, this is for progress display
		imgs=EMData.read_images(fsp,imgns[i*100:i*100+100])
		for j,im in enumerate(imgs):
			im2=im.process("math.fft.resample",{"n":2})
			im2=im2.get_clip(Region((im2["nx"]-pad)//2,(im2["nx"]-pad)//2,pad,pad))
			xf=im["xform.projection"]
			#xfp=xf.get_params("eman")
			#print(f'{xfp["az"]},{xfp["alt"]},{xfp["phi"]},{xfp["tx"]},{xfp["ty"]}')
			trans=xf.get_trans_2d()
			xf.set_trans(-trans[0]//2,-trans[1]//2)
			if j%2==0:
				imf=recone.preprocess_slice(im2,xf)
				recone.insert_slice(imf,xf,1.0)
			else:
				imf=recono.preprocess_slice(im2,xf)
				recono.insert_slice(imf,xf,1.0)
	
	rete=recone.finish(True)
	reto=recono.finish(True)
	rete["apix_x"]=im["apix_x"]
	rete["apix_y"]=im["apix_x"]
	rete["apix_z"]=im["apix_x"]
	reto["apix_x"]=im["apix_x"]
	reto["apix_y"]=im["apix_x"]
	reto["apix_z"]=im["apix_x"]
	fscf=rete.calc_fourier_shell_correlation(reto)
	fsc=fscf[len(fscf)//3:len(fscf)*2//3]
#	print(fsc)
	for r,v in enumerate(fsc[5:]): 
		if v<0.143: break
	r+=5
	ret=rete.copy()
	ret.add(reto)
	ret.process_inplace("filter.lowpass.tophat",{"cutoff_pixels":r})
#	ret=ret.get_clip((pad-im["nx"])//2,(pad-im["nx"])//2,(pad-im["nx"])//2,im["nx"],im["nx"],im["nx"])
#	ret.process_inplace("xform.scale",{"scale":2.0,"clip":im["nx"]})
#	ret=ret.do_ift()
	ret["apix_x"]=im2["apix_x"]
	ret["apix_y"]=im2["apix_x"]
	ret["apix_z"]=im2["apix_x"]
	ret["ptcl_repr"]=len(imgns)
	ret["resolution"]=1.0/fscf[r]
	print(f"Resolution {r} {1.0/fscf[r]}")
			
	que.put((tskn,100,ret,latent,imgns,currun,rad,rete,reto,mmode,nset))

def numrng(f):
	if f==0: return "-"
	f=max(min(1.0,f),-1.0)
	return chr(48+int((f+1.0)/2.01))


def main():
	progname = os.path.basename(sys.argv[0])
	usage = """prog [options]

	WARNING: This program still under development.
	
	This program is designed to work interactively with Gaussian mixture models, automating as much 
	as possible of the tasks associated with studying dynamics using GMMs. It requires either a 
	refine_XX folder or a set of particles with known orientations as a starting point."""

	parser = EMArgumentParser(usage=usage,version=EMANVERSION)

#	parser.add_argument("--path",type=str,default=None,help="Path for the gmm_XX folder to work in, by default a new folder will be created")
	parser.add_argument("--threads", default=-1,type=int,help="Number of alignment threads to run in parallel on a single computer.")
	parser.add_argument("--ppid", type=int, help="Set the PID of the parent process, used for cross platform PPID",default=-1)

	global options
	(options, args) = parser.parse_args()
	
	#if options.path==None:
		#options.path=numbered_path("gmm",True)
#		os.makedirs(options.path)

	if options.threads<1 : options.threads=num_cpus()
		
	#if not os.path.exists(options.path) :
		#os.mkdir(options.path)
		
#	parms=js_open_dict("{}/0_a2d_parms.json".format(options.path))
	

	pid=E2init(argv)
	
	app = EMApp()
	emgmm=EMGMM(app,options)
	emgmm.show()
	emgmm.raise_()
	QtWidgets.QMessageBox.warning(None,"Warning","""This program is still experimental. While functional
many capabilities of the underlying e2gmm_refine program
are not yet available through this interface. Matching number
of gaussians to resolution/volume is key in obtaining good
distributions.""")
	app.execute()
	
	E2end(pid)

class EMGMM(QtWidgets.QMainWindow):
	"""This is the main window for the e2gmm application"""
	
	def __init__(self,application,opt):
		self.options=opt
		"""application is an QApplication instance. path is the path for ouput files""" 
		QtWidgets.QWidget.__init__(self)

		self.particles=None

		self.app=weakref.ref(application)

		self.setWindowTitle("Main Window (e2gmm.py)")
#		self.plotmode=0
		self.ort_slice=Transform()
		self.cur_dyn_vol=None
		self.curmaps={}
		self.curmaps_sel={}

		# Menu Bar
		#self.mfile=self.menuBar().addMenu("File")
		#self.mfileopen=self.mfile.addAction("Select Particles")
		#self.mfileopencls=self.mfile.addAction("Particles from Classes")
		#self.mfileopencls.setEnabled(False)
		#self.mfilequit=self.mfile.addAction("Quit")

		self.setCentralWidget(QtWidgets.QWidget())
		self.gbl = QtWidgets.QGridLayout(self.centralWidget())
		cen=self.centralWidget()
		
		self.gbl.setColumnStretch(0,0)
		self.gbl.setColumnStretch(2,4)
		self.gbl.setColumnStretch(4,5)
		
		# Plot
		self.wplot2d = EMPlot2DWidget()
		self.gbl.addWidget(self.wplot2d,0,2,3,1)
		self.wplot2d.set_mouse_emit(True)
		self.wplot2d.set_annotate(self.plot_annotate)
		self.wplot2d.set_selectpoints(False)
		
		# 3-D View
		self.wview3d = EMScene3D()
		self.gbl.addWidget(self.wview3d,0,4,2,1)
		
		# Left pane for GMM folder and parameters
		self.gbll = QtWidgets.QGridLayout()
		self.gbl.addLayout(self.gbll,0,0,4,1)		# 4 rows tall, 1 column wide
		
		# gmm_XX folder selection
		self.gblfld = QtWidgets.QGridLayout()
		self.gbll.addLayout(self.gblfld,0,0)
		
		self.wlistgmm = QtWidgets.QListWidget()
		self.wlistgmm.setSizePolicy(QtWidgets.QSizePolicy.Preferred,QtWidgets.QSizePolicy.Expanding)
		self.update_gmms()
		self.gblfld.addWidget(self.wlistgmm,0,0,1,2)
		
		self.wbutnewgmm=QtWidgets.QPushButton("New GMM")
		self.gblfld.addWidget(self.wbutnewgmm,1,0)

		self.wlpath = QtWidgets.QLabel("-")
		self.wlpath.setToolTip("Path this GMM is based on")
		self.gblfld.addWidget(self.wlpath,1,1)
		
		#self.wbutrefine = QtWidgets.QPushButton("refine_XX")
		#self.gblfld.addWidget(self.wbutrefine,1,1)
		
		# run selection
		self.gblrun = QtWidgets.QGridLayout()
		self.gbll.addLayout(self.gblrun,1,0)
		
		self.wlistrun = QtWidgets.QListWidget()
		self.wlistrun.setSizePolicy(QtWidgets.QSizePolicy.Preferred,QtWidgets.QSizePolicy.Expanding)
#		self.update_runs()
		self.gblrun.addWidget(self.wlistrun,0,0,1,2)
		
		self.wbutnewrun=QtWidgets.QPushButton("Create Run")
		self.gblrun.addWidget(self.wbutnewrun,1,0)

		self.wbutres = QtWidgets.QPushButton("Resolution (A)")
		self.wbutres.setToolTip("Generate new Gaussian neutral model")
		self.gblrun.addWidget(self.wbutres,2,0)
		
		self.wedres = QtWidgets.QLineEdit("25")
		self.wedres.setToolTip("Resolution for Gaussian generation and refinement FSC")
		self.gblrun.addWidget(self.wedres,2,1)

		self.wedgthr = QtWidgets.QLineEdit("0.3")
		self.wedgthr.setToolTip("Threshold for gaussian generation, smaller => more gaussians (~0.1 - 0.8), start large and reduce")
		self.gblrun.addWidget(self.wedgthr,3,1)

		#self.wbutneutral=QtWidgets.QPushButton("Train Neutral Model")
		#self.gblrun.addWidget(self.wbutneutral,4,0)

		self.wbutneutral2=QtWidgets.QPushButton("Train Neutral")
		self.gblrun.addWidget(self.wbutneutral2,4,1)

		self.wedngauss = QtWidgets.QLabel(" ")		# originally an editor, now output only
		self.wedngauss.setToolTip("Number of Gaussians in the model")
		self.gblrun.addWidget(self.wedngauss,3,0)

		#self.wbutrerun=QtWidgets.QPushButton("Run Dynamics")
		#self.gblrun.addWidget(self.wbutrerun,5,0)
		
		self.wbutrerun2=QtWidgets.QPushButton("Train GMM")
		self.gblrun.addWidget(self.wbutrerun2,5,1)
		
		#### The form with details about the selected gmm_XX folder
		self.gflparm = QtWidgets.QFormLayout()
		self.gbll.addLayout(self.gflparm,2,0)
		
		self.wedbatch = QtWidgets.QLineEdit("1")
		self.wedbatch.setToolTip("Number of batches for network training (reduces memory requirements)")
		self.gflparm.addRow("# Ptcl Batches:",self.wedbatch)

		self.wedrres = QtWidgets.QLineEdit("25")
		self.wedrres.setToolTip("Resolution for individual particle input representation (often lower)")
		self.gflparm.addRow("Input Res (A):",self.wedrres)

		self.wedbox = QtWidgets.QLineEdit("256")
		self.wedbox.setReadOnly(1)
		self.wedbox.setToolTip("Box size of input particles in pixels")
		self.gflparm.addRow("Box Size:",self.wedbox)

		self.wedapix = QtWidgets.QLineEdit("1.0")
		self.wedapix.setToolTip("A/pix of input particles")
		self.gflparm.addRow("A/pix:",self.wedapix)

		self.wedsym = QtWidgets.QLineEdit("c1")
		self.wedsym.setToolTip("Symmetry used during refinement. Cannot reduce here without rerunning refinement!")
		self.gflparm.addRow("Symmetry:",self.wedsym)
		
		self.wedmask = QtWidgets.QLineEdit("")
		self.wedmask.setToolTip("<file>[,<file>...]  3-D volume masks for defining subregions, each with its own latent vector. Latent vec must be divisible by number of masks. Blank for a single region.")
		self.gflparm.addRow("Masks:",self.wedmask)
		
		self.weddim = QtWidgets.QLineEdit("4")
		self.weddim.setToolTip("Number of dimensions in the latent space (middle network layer)")
		self.gflparm.addRow("Latent Dim:",self.weddim)
		
		self.wedtrainiter = QtWidgets.QLineEdit("10")
		self.wedtrainiter.setToolTip("Training iterations (10-20 typical)")
		self.gflparm.addRow("Train iter:",self.wedtrainiter)

		self.wedtrainmodelreg = QtWidgets.QLineEdit("0.0")
		self.wedtrainmodelreg.setToolTip("Model regularlizer, biases Gaussians towards initial model. Larger -> stronger bias ")
		self.gflparm.addRow("Model Reg:",self.wedtrainmodelreg)

		self.wedtrainperturb = QtWidgets.QLineEdit("0.05")
		self.wedtrainperturb.setToolTip("Per-iteration model perturbation during training. Larger -> possibly faster training, but more 'churn'")
		self.gflparm.addRow("Model Perturb:",self.wedtrainperturb)

		# self.wbutconv = QtWidgets.QPushButton("Convolutional")
		# self.wbutconv.setCheckable(True)
		# self.wbutconv.setChecked(False)
		# self.wbutconv.setToolTip("Use a convolutional neural network structure instead of a conventional network structure")
		# self.gflparm.addRow(" ",self.wbutconv)
		
		self.wbutpos = QtWidgets.QPushButton("Position")
		self.wbutpos.setCheckable(True)
		self.wbutpos.setChecked(True)
		self.wbutpos.setToolTip("Include changes of position in the GMM (motion)")
		self.gflparm.addRow("Parameters",self.wbutpos)
		
		self.wbutamp = QtWidgets.QPushButton("Amplitude")
		self.wbutamp.setCheckable(True)
		self.wbutamp.setChecked(True)
		self.wbutamp.setToolTip("Include changes of amplitude in the GMM (ligand binding)")
		self.gflparm.addRow(" ",self.wbutamp)
		
		# self.wbutsig = QtWidgets.QPushButton("Sigma")
		# self.wbutsig.setCheckable(True)
		# self.wbutsig.setChecked(False)
		# self.wbutsig.setToolTip("Include changes of Gaussian Width in the GMM (rarely useful)")
		# self.gflparm.addRow(" ",self.wbutsig)
		
		self.wlabruntime = QtWidgets.QLabel("-")
		self.gflparm.addRow("Run:",self.wlabruntime)
		
		#### Widgets below plot
		self.gblpltctl = QtWidgets.QGridLayout()
		self.gbl.addLayout(self.gblpltctl,3,2)
		
		self.gblpltctl.addWidget(QtWidgets.QLabel("X Col:",self),0,0,Qt.AlignRight)
		self.wsbxcol=QtWidgets.QSpinBox(self)
		self.wsbxcol.setRange(0,10)
		self.gblpltctl.addWidget(self.wsbxcol,0,1,Qt.AlignLeft)
		self.wsbxcol.setValue(0)

		self.gblpltctl.addWidget(QtWidgets.QLabel("Y Col:",self),1,0,Qt.AlignRight)
		self.wsbycol=QtWidgets.QSpinBox(self)
		self.wsbycol.setRange(0,10)
		self.gblpltctl.addWidget(self.wsbycol,1,1,Qt.AlignLeft)
		self.wsbycol.setValue(1)
		

		self.gblpltctl.addWidget(QtWidgets.QLabel("Rad:",self),3,0,Qt.AlignRight)
		self.wedrad=QtWidgets.QLineEdit("0.2")
		self.wedrad.setToolTip("Radius for including points adjacent to selected point or spacing between sets")
		self.gblpltctl.addWidget(self.wedrad,3,1,Qt.AlignRight)

		self.gblpltctl.addWidget(QtWidgets.QLabel("Ptcl:",self),4,0,Qt.AlignRight)
		self.wedcnt=QtWidgets.QLineEdit("10000")
		self.wedcnt.setToolTip("Particles per set in Line of Sets mode")
		self.gblpltctl.addWidget(self.wedcnt,4,1,Qt.AlignRight)

		#self.wbutdrgrp=QtWidgets.QButtonGroup()
		
		#self.wbutdrmid=QtWidgets.QPushButton("Net Mid")
		#self.wbutdrmid.setCheckable(True)
		#self.wbutdrmid.setChecked(True)
		#self.gblpltctl.addWidget(self.wbutdrmid,0,3)
		#self.wbutdrgrp.addButton(self.wbutdrmid,0)
		
		#self.wbutdrpca=QtWidgets.QPushButton("PCA")
		#self.wbutdrpca.setCheckable(True)
		#self.gblpltctl.addWidget(self.wbutdrpca,0,4)
		#self.wbutdrgrp.addButton(self.wbutdrpca,1)
		
		#self.wbutdrpca=QtWidgets.QPushButton("ICA")
		#self.wbutdrpca.setCheckable(True)
		#self.gblpltctl.addWidget(self.wbutdrpca,0,5)
		#self.wbutdrgrp.addButton(self.wbutdrpca,2)
		
		#self.gblpltctl.addWidget(QtWidgets.QLabel("New Dim:",self),1,4,Qt.AlignRight)
		#self.wsbnewdim=QtWidgets.QSpinBox(self)
		#self.wsbnewdim.setRange(2,10)
		#self.gblpltctl.addWidget(self.wsbnewdim,1,5,Qt.AlignLeft)
		
		self.wbutkmeans=QtWidgets.QPushButton("Kmeans")
		self.wbutkmeans.setToolTip("Fast and functional, but produces linear boundaries not clusters")
		self.gblpltctl.addWidget(self.wbutkmeans,0,5)
		self.wbutkmeans.clicked[bool].connect(self.do_kmeans)

		self.wbutwater=QtWidgets.QPushButton("Wshed")
		self.wbutwater.setToolTip("Watershed based, number is a maximum. 2-D and 3-D subspaces only.")
		self.gblpltctl.addWidget(self.wbutwater,1,5)
		self.wbutwater.clicked[bool].connect(self.do_watershed)

		self.wbutdbscan=QtWidgets.QPushButton("OpticsDB")
		self.wbutdbscan.setToolTip("Danger! This may exhaust RAM on large data sets, and may take a long time to run")
		self.gblpltctl.addWidget(self.wbutdbscan,2,5)
		self.wbutdbscan.clicked[bool].connect(self.do_dbscan)

		self.wbutoptics=QtWidgets.QPushButton("OpticsXi")
		self.wbutoptics.setToolTip("Danger! This could take many hours to run")
		self.gblpltctl.addWidget(self.wbutoptics,3,5)
		self.wbutoptics.clicked[bool].connect(self.do_optics)

		self.wbutspectr=QtWidgets.QPushButton("Spectr")
		self.wbutspectr.setToolTip("Danger! May fail on large data sets, and possibly exhaust RAM")
		self.gblpltctl.addWidget(self.wbutspectr,4,5)
		self.wbutspectr.clicked[bool].connect(self.do_spectral)

		self.wvbnsets=ValBox(label="Sets:",value=2)
		self.wvbnsets.setIntonly(True)
		self.gblpltctl.addWidget(self.wvbnsets,0,4)

		self.wstbaxes=StringBox(label="Axes:",value="2-5")
		self.gblpltctl.addWidget(self.wstbaxes,1,4)

		self.wcbpntpln=QtWidgets.QComboBox()
		self.wcbpntpln.addItem("Explore")
		self.wcbpntpln.addItem("Make Set R")
		self.wcbpntpln.addItem("Make Set #")
		self.wcbpntpln.addItem("Line of Sets")

		#self.wcbpntpln.addItem("Plane")
		#self.wcbpntpln.addItem("HSphere")
		#self.wcbpntpln.addItem("HSlab")
		#self.wcbpntpln.addItem("Line")
		#self.wcbpntpln.addItem("Map-HSphere")
		#self.wcbpntpln.addItem("Map-HSlab")
		#self.wcbpntpln.addItem("Map-Line")
		#self.wcbpntpln.addItem("Map-HSlab (fast)")
		#self.wcbpntpln.addItem("Map-Line (fast)")
		#self.wcbpntpln.addItem("Map-Line-Sph (fast)")
		self.gblpltctl.addWidget(self.wcbpntpln,2,0,1,2)

		# These buttons are associated with the map list defined below
		self.wbutmapnorm=QtWidgets.QPushButton("Build Map")
		self.gblpltctl.addWidget(self.wbutmapnorm,0,6)

		self.wbutmapfast=QtWidgets.QPushButton("Quick Map")
		self.gblpltctl.addWidget(self.wbutmapfast,1,6)

		self.wbutmapgauss=QtWidgets.QPushButton("Gauss Map")
		self.gblpltctl.addWidget(self.wbutmapgauss,2,6)

		self.wbutsetdel=QtWidgets.QPushButton("Delete")
		self.gblpltctl.addWidget(self.wbutsetdel,3,6)

		self.wbutsetsave=QtWidgets.QPushButton("Save Set")
		self.gblpltctl.addWidget(self.wbutsetsave,4,6)


		#### Widgets below 3D
		self.gbl3dctl = QtWidgets.QGridLayout()
		self.gbl.addLayout(self.gbl3dctl,2,4,2,1)

		# map list widget
		self.maplist=QtWidgets.QTableWidget()
		self.maplist.setColumnCount(6)
		self.maplist.verticalHeader().hide()
		self.maplist.itemSelectionChanged.connect(self.sel_maptable)
		self.gbl3dctl.addWidget(self.maplist,0,0,6,1)

		# 2-D slice view
		self.wview2d = None
		
		self.wbutvtop=QtWidgets.QPushButton("Top")
		self.gbl3dctl.addWidget(self.wbutvtop,0,1)

		self.wbutvbot=QtWidgets.QPushButton("Bot")
		self.gbl3dctl.addWidget(self.wbutvbot,0,2)

		self.wbutvside1=QtWidgets.QPushButton("Side1")
		self.gbl3dctl.addWidget(self.wbutvside1,0,3)

		self.wbutvside2=QtWidgets.QPushButton("Side2")
		self.gbl3dctl.addWidget(self.wbutvside2,0,4)

		self.wbutvoblq=QtWidgets.QPushButton("Oblique")
		self.gbl3dctl.addWidget(self.wbutvoblq,1,2)

		self.wbutvside3=QtWidgets.QPushButton("Side3")
		self.gbl3dctl.addWidget(self.wbutvside3,1,3)

		self.wbutvside4=QtWidgets.QPushButton("Side4")
		self.gbl3dctl.addWidget(self.wbutvside4,1,4)

		# Sphere size
		self.wvssphsz=ValSlider(self,(1,50),"Sphere Size:",3.0,90)
		self.gbl3dctl.addWidget(self.wvssphsz,2,1,1,4)

		# Thickness
		self.wlthk = QtWidgets.QLabel("Thk:")
		self.wlthk.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignVCenter)
		self.gbl3dctl.addWidget(self.wlthk,3,5)
		self.wsbthk = QtWidgets.QSpinBox()
		self.wsbthk.setMinimum(-1)
		self.wsbthk.setMaximum(256)
		self.wsbthk.setValue(0)
		self.gbl3dctl.addWidget(self.wsbthk,3,6)

		# Center with respect to actual center
		self.wlcen = QtWidgets.QLabel("Cen:")
		self.wlcen.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignVCenter)
		self.gbl3dctl.addWidget(self.wlcen,3,7)
		self.wsbcen = QtWidgets.QSpinBox()
		self.wsbcen.setMinimum(-256)
		self.wsbcen.setMaximum(256)
		self.wsbcen.setValue(0)
		self.gbl3dctl.addWidget(self.wsbcen,3,8)

		self.wbutnmap=QtWidgets.QPushButton("Neutral Map")
		self.wbutnmap.setCheckable(True)
		self.wbutnmap.setChecked(True)
		self.gbl3dctl.addWidget(self.wbutnmap,3,1,1,2)

		self.wbutnmdl=QtWidgets.QPushButton("Neutral Model")
		self.wbutnmdl.setCheckable(True)
		self.wbutnmdl.setChecked(True)
		self.gbl3dctl.addWidget(self.wbutnmdl,3,3,1,2)

		self.wbutdmap=QtWidgets.QPushButton("Dynamic Map")
		self.wbutdmap.setCheckable(True)
		self.wbutdmap.setChecked(True)
		self.gbl3dctl.addWidget(self.wbutdmap,4,1,1,2)

		self.wbutdmdl=QtWidgets.QPushButton("Dynamic Model")
		self.wbutdmdl.setCheckable(True)
		self.wbutdmdl.setChecked(True)
		self.gbl3dctl.addWidget(self.wbutdmdl,4,3,1,2)

		self.wbutdmask=QtWidgets.QPushButton("Mask")
		self.wbutdmask.setCheckable(True)
		self.wbutdmask.setChecked(False)
		self.gbl3dctl.addWidget(self.wbutdmask,5,1,1,2)

		# Connections
		self.wsbthk.valueChanged.connect(self.slice_update)
		self.wsbcen.valueChanged.connect(self.slice_update)
		self.wview3d.sgtransform.connect(self.slice_update)
		self.wlistgmm.currentRowChanged[int].connect(self.sel_gmm)
		self.wbutnmap.clicked[bool].connect(self.new_3d_opt)
		self.wbutnmdl.clicked[bool].connect(self.new_3d_opt)
		self.wbutdmap.clicked[bool].connect(self.new_3d_opt)
		self.wbutdmdl.clicked[bool].connect(self.new_3d_opt)
		self.wbutdmask.clicked[bool].connect(self.new_3d_opt)
		self.wbutvtop.clicked[bool].connect(self.view_top)
		self.wbutvbot.clicked[bool].connect(self.view_bot)
		self.wbutvoblq.clicked[bool].connect(self.view_oblique)
		self.wbutvside1.clicked[bool].connect(self.view_side1)
		self.wbutvside2.clicked[bool].connect(self.view_side2)
		self.wbutvside3.clicked[bool].connect(self.view_side3)
		self.wbutvside4.clicked[bool].connect(self.view_side4)
		self.wvssphsz.valueChanged.connect(self.new_sph_size)
		self.wbutnewgmm.clicked[bool].connect(self.add_gmm)
#		self.wbutrefine.clicked[bool].connect(self.setgmm_refine)
		self.wlistrun.currentRowChanged[int].connect(self.sel_run)
		self.wbutnewrun.clicked[bool].connect(self.new_run)
		#self.wbutrerun.clicked[bool].connect(self.do_run)
		self.wbutrerun2.clicked[bool].connect(self.do_run_new)
		#self.wbutneutral.clicked[bool].connect(self.new_neutral)
		self.wbutneutral2.clicked[bool].connect(self.new_neutral2)
		self.wbutmapnorm.clicked[bool].connect(self.new_map)
		self.wbutmapfast.clicked[bool].connect(self.new_map_fast)
		self.wbutmapgauss.clicked[bool].connect(self.new_map_gauss)
		self.wbutsetdel.clicked[bool].connect(self.set_del)
		self.wbutsetsave.clicked[bool].connect(self.set_save)
#		self.wedres.editingFinished.connect(self.new_res)
		self.wbutres.clicked[bool].connect(self.new_res)
		#self.wbutdrgrp.idClicked[int].connect(self.plot_mode_sel)		# requires pyqt 5.15
		#self.wbutdrgrp.buttonClicked[QtWidgets.QAbstractButton].connect(self.plot_mode_sel)
		self.wsbxcol.valueChanged[int].connect(self.update_axes_x)		
		self.wsbycol.valueChanged[int].connect(self.update_axes_y)		
		#self.wsbxcol.valueChanged[int].connect(self.wplot2d.setXAxisAll) #sequencing issue with this approach
		#self.wsbycol.valueChanged[int].connect(self.wplot2d.setYAxisAll)
		#self.wsbxcol.valueChanged[int].connect(self.update_maps_plot)		# also connect to update map locations when axes change
		#self.wsbycol.valueChanged[int].connect(self.update_maps_plot)
		self.wplot2d.mousedown[QtGui.QMouseEvent,tuple].connect(self.plot_mouse)
		self.wplot2d.mouseup[QtGui.QMouseEvent,tuple].connect(self.plot_mouse_up)
		self.wplot2d.mousedrag[QtGui.QMouseEvent,tuple].connect(self.plot_mouse_drag)
		self.wplot2d.keypress[QtGui.QKeyEvent].connect(self.plot_keyboard)
		E2loadappwin("e2gmm","main",self)

		# map used to generate the neutral map, ie - the seed for the GMM
		self.mapdataitem=EMDataItem3D(None)
		self.mapiso=EMIsosurface(self.mapdataitem)
		self.wview3d.insertNewNode("Neutral Map",self.mapdataitem)
		self.wview3d.insertNewNode("Isosurface",self.mapiso,parentnode=self.mapdataitem)

		# map used to generate the neutral map, ie - the seed for the GMM
		self.maskdataitem=EMDataItem3D(None)
		self.maskiso=EMIsosurface(self.maskdataitem)
		self.wview3d.insertNewNode("Mask",self.maskdataitem)
		self.wview3d.insertNewNode("Isosurface",self.maskiso,parentnode=self.maskdataitem)

		# the current selected dynamic map generated from a subset of particles
		self.dmapdataitem=None
		self.lastbest=None

		# shows the filtered map when appropriate
		self.fmapdataitem=None
		
		# Gaussian dynamic coordinates as spheres
		self.gaussplot=EMScatterPlot3D()
		self.wview3d.insertNewNode("Dynamic Model",self.gaussplot)
		
		# Gaussian neutral model (for comparison with the map and with the dynamic model
		self.neutralplot=EMScatterPlot3D()
		self.wview3d.insertNewNode("Neutral Model",self.neutralplot)

		self.currunkey=None
		self.lastres=0
		
		# Active threads used for 3-D reconstructions, once joined they are removed from the list
		self.threads=[]
		self.threadq=Queue()
		
		# This is used to update reconstruction thread results
		self.timer=QTimer()
		self.timer.timeout.connect(self.timeout)
		self.timer.start(1000)

	def closeEvent(self,event):
		E2saveappwin("e2gmm","main",self)

		# no longer a separate window
		#if not self.maplist is None:
			#E2saveappwin("e2gmm","maps",self.maplist)
			#self.maplist.close()
			#self.maplist = None

		#if self.guiim != None:
			#E2saveappwin("e2ctf","image",self.guiim.qt_parent)
			#self.app().close_specific(self.guiim)
			#self.guiim = None

		event.accept()
		self.app().close_specific(self)
		self.module_closed.emit() # this signal is important when e2ctf is being used by a program running its own event loop\

	def do_events(self,delay=0.1):
		"""process the event loop with a small delay to allow user abort, etc."""
		t=time.time()
		while (time.time()-t<delay): 
			self.app().processEvents()

	def view_top(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":0.0,"alt":0.0,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())


	def view_bot(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":0.0,"alt":180.0,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())

	def view_oblique(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":0.0,"alt":90.0-22.5,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())

	def view_side1(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":0.0,"alt":90.0,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())

	def view_side2(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":90.0,"alt":90.0,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())

	def view_side3(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":22.5,"alt":90.0,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())

	def view_side4(self,tmp=None):
		self.wview3d.getTransform().set_rotation({"type":"eman","az":45.0,"alt":90.0,"phi":0.0})
		self.wview3d.updateGL()
		self.slice_update(b=self.wview3d.getTransform())

	def slice_update(self,a=None,b=None):
		"""Called when any of the slice parameters change, if b is a transform then we use it"""
		if b is not None: self.ort_slice=b
		if self.cur_dyn_vol is None :
			if self.wview2d is not None: 
				self.wview2d.set_data(EMData(256,256,1))
			return
		thk=self.wsbthk.value()		# thickness of layer
		cen=self.wsbcen.value()		# center of layer
		nz=self.cur_dyn_vol["nz"]

		if thk<0 : proj = self.cur_dyn_vol.process("xform",{"transform":self.ort_slice}).process("misc.directional_sum",{"axis":"z"})
		else: proj = self.cur_dyn_vol.process("xform",{"transform":self.ort_slice}).process("misc.directional_sum",{"first":nz//2+cen-thk,"last":nz//2+cen+thk,"axis":"z"})

		if self.wview2d is None:
			self.wview2d = EMImage2DWidget(sizehint=(384,384))
			self.gbl3dctl.addWidget(self.wview2d,1,5,2,4)

		self.wview2d.set_data(proj)

	def timeout(self):
		"""Handles the results of completed threads"""
		
		while not self.threadq.empty():
			ret=self.threadq.get()
			if ret[1]==100:
				print(f"Thread {ret[0]} complete")
				currun=ret[5]
				bs=self.jsparm["boxsize"]
				ps=(ret[2]["nx"]-bs)//2
				vol=ret[2]
				vole=ret[7]
				volo=ret[8]
				latent=ret[3]
				imgns=ret[4]
				rad=ret[6]
				mmode=ret[9]
				nset=ret[10]
				#vol=ret[2].get_clip(Region(ps,ps,ps,bs,bs,bs))
				#vole=ret[7].get_clip(Region(ps,ps,ps,bs,bs,bs))
				#volo=ret[8].get_clip(Region(ps,ps,ps,bs,bs,bs))
				#vol.process_inplace("filter.lowpass.gauss",{"cutoff_freq":1.0/currun["targres"]})	# e/o fsc filter now handled in reconstructor
				vol.process_inplace("normalize")
				vol["latent"]=f"{tuple(latent)} {rad} '{mmode}'"
				
				# store the reconstructed map
				try: nmaps=EMUtil.get_image_count(f"{self.gmm}/set_maps.hdf")
				except: nmaps=0
				vol.write_compressed(f"{self.gmm}/set_maps.hdf",nmaps,8)
				vole.write_compressed(f"{self.gmm}/set_even.hdf",nmaps,8)
				volo.write_compressed(f"{self.gmm}/set_odd.hdf",nmaps,8)
				
				# store metadata to identify maps in dynamic_maps.hdf
				# note that 2
				# set # (key), 0) map #, 1) timestamp, 2) (cols,coordinates of center), 3) map size, 4) est resolution, 5) list of points
				newmap=[nmaps,local_datetime(),latent,vol["nz"],vol["resolution"],imgns]
				self.curmaps[str(nset)]=newmap
				self.sets_changed()

				# display the map,  this seems to trigger segfaults sometimes, so skipping for now
				#self.display_dynamic(vol)

				#self.wplot2d.full_refresh()
				#self.wplot2d.updateGL()

				# clean up the thread
				t=self.threads[ret[0]]
				t.join()
				self.threads[ret[0]]=None		# just gets longer and longer...
			else:
				print(f"Thread {ret[0]} at {ret[1]}%")

	def update_maptable(self,sel=None):
		"""Updates the table containing all of the current computed dynamic maps"""

		self.maplist.clear()
		self.maplist.setHorizontalHeaderLabels(["Set","Ptcl","Map","Size","Res","Datestamp"])
		self.maplist.setRowCount(len(self.curmaps))
		for i,k in enumerate(sorted([int(i) for i in self.curmaps])):
			m=self.curmaps[str(k)]
			#print(m[:-1])
			#cstr="".join([numrng(v) for v in m[2]])

			twi=QtWidgets.QTableWidgetItem(f"{k}")
			twi.setFlags(Qt.ItemIsSelectable|Qt.ItemIsEnabled)
			self.maplist.setItem(i,0,twi)

			twi=QtWidgets.QTableWidgetItem(f"{len(m[5])}")
			twi.setFlags(Qt.ItemIsEnabled)
			self.maplist.setItem(i,1,twi)

			twi=QtWidgets.QTableWidgetItem(f"{m[0]}")
			twi.setFlags(Qt.ItemIsEnabled)
			self.maplist.setItem(i,2,twi)

			twi=QtWidgets.QTableWidgetItem(f"{m[3]}")
			twi.setFlags(Qt.ItemIsEnabled)
			self.maplist.setItem(i,3,twi)

			twi=QtWidgets.QTableWidgetItem(f"{m[4]:1.1f}")
			twi.setFlags(Qt.ItemIsEnabled)
			self.maplist.setItem(i,4,twi)

			twi=QtWidgets.QTableWidgetItem(f"{m[1]}")
			twi.setFlags(Qt.ItemIsEnabled)
#			twi.setFlags(Qt.NoItemFlags)
			self.maplist.setItem(i,5,twi)

		self.maplist.resizeColumnsToContents()
		self.curmaps_sel=[]
		if not sel is None:
			self.maplist.selectRow(sel)

	def sel_maptable(self):
		"""When items are selected in the list of generated maps"""
#		print("*** ", self.data.shape)
#		self.wplot2d.set_data(self.data,"map")
#		self.wplot2d.set_data(None,replace=True,quiet=True)

#		print(self.data.shape[1])
		ss=max(7-int(log10(self.data.shape[1])),1)
		if len(self.maplist.selectedItems())>0:
			try:
				self.wplot2d.set_data(self.data,"map",symsize=ss,replace=True,quiet=True)
				self.curmaps_sel={}
				self.data_sel=[]
				# ss=10
				for i in self.maplist.selectedItems():
					key=i.text()
					# ss-=2
					# if ss<2 : ss=2
					smap=self.curmaps[key]
					self.curmaps_sel[key]=smap
					if not isinstance(smap[5],np.ndarray) : smap[5]=np.array(smap[5])
					self.data_sel.append(self.data[:,np.array(smap[5])])	# smap[5] is a list of points in the class
					#print("S:",self.data.shape,self.data_sel[-1].shape)
	#				self.wplot2d.set_data(self.data_sel[-1],f"set_{key}",symsize=ss,quiet=True)
					self.wplot2d.set_data(self.data_sel[-1],f"set_{int(key):02d}",symsize=ss,quiet=True)
			except:
				print("Error displaying selected points, e2gmm.py:sel_maptable()")
				return
		else:
			self.wplot2d.set_data(self.data,"map",symsize=ss,replace=True,quiet=True)

		self.wplot2d.setXAxisAll(self.wsbxcol.value(),True)
		self.wplot2d.setYAxisAll(self.wsbycol.value(),True)


#		self.wplot2d.setXAxisAll(0,True)
#		self.wplot2d.setYAxisAll(1,True)
#		self.wplot2d.full_refresh()
		self.wplot2d.updateGL()

		# when a single set is selected, we display the corresponding map/model
		if len(self.maplist.selectedItems())==1:
			dim=self.currun.get("dim",4)
			key=self.maplist.selectedItems()[0].text()
			smap=self.curmaps[key]
			ptdist=smap[5]
			if len(ptdist)>1000: ptdist=ptdist[::len(ptdist)//1000]		# We don't really need every point to get a pretty good average position for the set
			latent=np.mean(self.midresult.transpose()[ptdist,2:2+dim],0,keepdims=True)		# the mean latent vector over selected points
			gauss=self.decoder(latent,0).numpy()[0].transpose()
			if latent is not None:
				box=int(self.wedbox.text())
				gauss[:3]*=box
				gauss[2]*=-1.0
				gauss[1]*=-1.0
				if not butval(self.wbutpos): gauss[:3]=self.model[:3]
				if not butval(self.wbutamp): gauss[3]=self.model[3]
				self.gaussplot.setData(gauss,self.wvssphsz.value)

			if smap[0] is not None:
				try:
					vol=EMData(f"{self.gmm}/set_maps.hdf",smap[0])
					self.display_dynamic(vol)
				except:
					print("Error: map missing for ",smap)
			else:
				self.display_dynamic(None) 
#				self.wview3d.updateGL()
		else:
			self.display_dynamic(None)

	def display_dynamic(self,vol):
		"""Displays a new dynamic map, used multiple places so condensed here"""
		#print("Set null vol")
		self.cur_dyn_vol=vol
		if self.dmapdataitem is None:
			self.dmapdataitem=EMDataItem3D(vol)
			self.dmapiso=EMIsosurface(self.dmapdataitem)
			self.wview3d.insertNewNode("Dynamic Map",self.dmapdataitem,parentnode=self.wview3d)
			self.wview3d.insertNewNode("Isosurface",self.dmapiso,parentnode=self.dmapdataitem)
		else:
			self.dmapdataitem.setData(vol)
		
		if vol is not None:
			self.dmapiso.getTransform().set_scale(vol["apix_x"]/self.jsparm["apix"])
		
		self.new_3d_opt()
		self.wview3d.updateGL()
		self.slice_update()

	def saveparm(self,mode=None):
		"""mode is used to decide which parameters to update. neutral, dynamics or None"""
		self.currun={}
		self.currun["targres"]=float(self.wedres.text())
		self.currun["gausthr"]=self.wedgthr.text()
		try: self.currun["ngauss"]=len(self.amps)
		except: self.currun["ngauss"]=0
		#self.currun["boxsize"]=int(self.wedbox.text())
		#self.currun["apix"]=float(self.wedapix.text())
		rres=str(self.wedrres.text())
		if "," in rres :
			self.currun["ptclres"]=float(rres.split(",")[0])
			self.currun["ptclminres"]=float(rres.split(",")[1])
		else:
			self.currun["ptclres"]=float(rres)
			self.currun["ptclminres"]=1000.0
		self.currun["batches"]=str(self.wedbatch.text())
		self.currun["sym"]=str(self.wedsym.text())
		self.currun["mask"]=str(self.wedmask.text())
		self.currun["dim"]=int(self.weddim.text())
		self.currun["trainiter"]=int(self.wedtrainiter.text())
		self.currun["modelreg"]=float(self.wedtrainmodelreg.text())
		self.currun["perturb"]=float(self.wedtrainperturb.text())
		self.currun["conv"]=0	#butval(self.wbutconv)
		self.currun["pas"]=butstr(self.wbutpos)+butstr(self.wbutamp)+"0"  # butstr(self.wbutsig)
		if mode=="neutral": self.currun["time_neutral"]=local_datetime()
		if mode=="dynamics": self.currun["time_dynamics"]=local_datetime()
		self.jsparm["run_"+self.currunkey]=self.currun
	
	def new_sph_size(self,newval=10):
		self.gaussplot.setPointSize(self.wvssphsz.value)
		self.neutralplot.setPointSize(self.wvssphsz.value)
		self.wview3d.update()
	
	def plot_keyboard(self,event):
		"""keyboard events from the 2-D plot"""
		
		if event.key()==Qt.Key_Escape: self.mouseabort=True
	
	#def plot_mouse(self,event,loc):
		#self.mouseabort=False
		#mmode=str(self.wcbpntpln.currentText())
		#dim=self.currun.get("dim",4)
		#latent=np.zeros(dim)		# latent coordinates of the selected point
		#try: rad=float(self.wedrad.text())
		#except:
			#print("invalid radius, using 0.05")
			#rad=0.05
		#xcol=self.wsbxcol.value()
		#ycol=self.wsbycol.value()
		#if self.plotmode==0:
			#latent[xcol]=loc[0]
			#latent[ycol]=loc[1]
		#if self.plotmode==1:
			#newdim=self.wsbnewdim.value()
			#sel=np.zeros(newdim)
			#sel[xcol]=loc[0]
			#sel[ycol]=loc[1]
			#latent=self.decomp.inverse_transform(sel)

		## Check whether we are close enough to a computed dynamic map to display it
		#best=(1.0e6,None)
		#for k in self.curmaps_sel:
			#m=self.curmaps_sel[k]
			#if self.plotmode==1:
				#xm=self.decomp.transform(m[2].reshape(1,len(m[2])))[0]	# expects an array of vectors
				#c=(xm[xcol],xm[ycol])
			#else:
				#c=(m[2][xcol],m[2][ycol])
			#d=hypot(c[0]-loc[0],c[1]-loc[1])
			#if d<0.05 and d<best[0] : best=(d,m)	# fixed max radius of 0.05

		#if best[1] is not None:
			#if self.lastbest!=best[1][0]:
##				print(f"{loc} Best {best}")
				#map=EMData(f"{self.gmm}/set_maps.hdf",best[1][0])
				#self.display_dynamic(map)
				#self.lastbest=best[1][0]

		## Mode dependent update of dynamic model
		#if mmode=="Plane":
			## run the current latent vector through the decoder then pull the result out in a useful 'shape'
			#gauss=np.array(self.decoder(latent[None,...]))[0].transpose()
			#box=int(self.wedbox.text())
			#gauss[:3]*=box
			#gauss[2]*=-1.0
			#gauss[1]*=-1.0
			#if not butval(self.wbutpos): gauss[:3]=self.model[:3]
			#if not butval(self.wbutamp): gauss[3]=self.model[3]
##			print(gauss[:,:6])
##			print("-----")
			#self.gaussplot.setData(gauss,self.wvssphsz.value)
			#self.wview3d.update()

			## if the point is inside a dynamic map cicle, load the map
			##for m in self.curmaps:
				##r=

			#return
		#elif mmode=="HSphere":
			## This will produce a list of indices where the distance in latent space is less than the specified rad
			#ptdist=(np.sum((self.midresult.transpose()-latent)**2,1)<(rad**2)).nonzero()[0]
			#self.wplot2d.add_shape("count",EMShape(["scrlabel",0.1,0.1,0.1,10.,10.,f"{len(ptdist)} ptcls",120.0,-1]))
			#self.wplot2d.add_shape("region",EMShape(["circle",0.1,0.8,0.1,loc[0],loc[1],rad,1]))
			#self.wplot2d.update()
##			print(loc,self.wplot2d.plot2draw(loc[0],loc[1]),event.x(),event.y(),self.wplot2d.scrlim,rad)

			##Limit ourselves to a random subset of ~500 of the points to average
			#if len(ptdist)>500: ptdist=ptdist[::len(ptdist)//500]
			#gauss=np.mean(self.decoder(self.midresult.transpose()[ptdist]),0).transpose()		# run all of the selected latent vectors through the decoder at once
			#box=int(self.wedbox.text())
			#gauss[:3]*=box
			#gauss[2]*=-1.0
			#gauss[1]*=-1.0
			#if not butval(self.wbutpos): gauss[:3]=self.model[:3]
			#if not butval(self.wbutamp): gauss[3]=self.model[3]
			#self.gaussplot.setData(gauss,self.wvssphsz.value)
			#self.wview3d.update()
			#return
		#elif mmode=="HSlab":
			## This will produce a list of indices where the distance in the plane is less than the specified rad
			#ptdist=((self.midresult[xcol]-latent[xcol])**2+(self.midresult[ycol]-latent[ycol])**2<(rad**2)).nonzero()[0]
			#self.wplot2d.add_shape("count",EMShape(["scrlabel",0.1,0.1,0.1,15.,15.,f"{len(ptdist)} ptcls",120.0,-1]))
			#self.wplot2d.add_shape("region",EMShape(["circle",0.1,0.8,0.1,loc[0],loc[1],rad,1]))
			#self.wplot2d.update()

			##gauss=np.mean(self.decoder(self.midresult.transpose()[ptdist]),0).transpose()		# run all of the selected latent vectors through the decoder at once
			##Limit ourselves to a random subset of ~500 of the points to average
			#if len(ptdist)>500: ptdist=ptdist[::len(ptdist)//500]
			#gauss=np.mean(self.decoder(self.midresult.transpose()[ptdist]),0).transpose()		# run all of the selected latent vectors through the decoder at once
			#box=int(self.wedbox.text())
			#gauss[:3]*=box
			#gauss[2]*=-1.0
			#gauss[1]*=-1.0
			#if not butval(self.wbutpos): gauss[:3]=self.model[:3]
			#if not butval(self.wbutamp): gauss[3]=self.model[3]
			#self.gaussplot.setData(gauss,self.wvssphsz.value)
			#self.wview3d.update()
			#return
		#elif mmode in ("Line"):
			#self.line_origin=(loc,latent)
			#return
		#else: print("mode error")


	def plot_mouse(self,event,loc):
		self.mouseabort=False
		self.line_origin=None
		mmode=str(self.wcbpntpln.currentText())
		dim=self.currun.get("dim",4)
		augdim=self.midresult.shape[0]

		try: rad=float(self.wedrad.text())
		except:
			print("invalid radius, using 0.05")
			rad=0.05

		xcol=min(self.wsbxcol.value(),self.data.shape[1]-1)
		ycol=min(self.wsbycol.value(),self.data.shape[1]-1)

		if mmode=="Explore":
			# This will produce a list of indices where the distance in the plane is less than the specified rad
			ptdist=((self.midresult[xcol]-loc[0])**2+(self.midresult[ycol]-loc[1])**2<(rad**2)).nonzero()[0]
			nptcl=len(ptdist)
			self.wplot2d.add_shape("region",EMShape(["circle",0.1,0.8,0.1,loc[0],loc[1],rad,1]))

			# no particles in the circle!
			if nptcl==0 : return

			if len(ptdist)>500: ptdist=ptdist[::len(ptdist)//500]
			#print(ptdist.shape)
			#print(self.midresult.transpose()[ptdist,2:2+dim].shape)
			# This computes the mean latent vector over the points, then runs that single latent vector through the decoder
			latent=np.mean(self.midresult.transpose()[ptdist,2:2+dim],0,keepdims=True)		# the mean latent vector over selected points
			gauss=self.decoder(latent,0).numpy()[0].transpose()
			#print(latent,latent.shape,self.midresult.transpose()[ptdist,2:2+dim].shape)

			#this computes the gaussian coords for all selected points then computes the mean of the gaussian parameters
			# this approach doesn't generate a Gaussian configuration which corresponds exactly to a point in latent space
			#gauss=np.mean(self.decoder(self.midresult.transpose()[ptdist,2:2+dim]),0).transpose()		# run all of the selected latent vectors through the decoder at once

			box=int(self.wedbox.text())
			gauss[:3]*=box
			gauss[2]*=-1.0
			gauss[1]*=-1.0
			if not butval(self.wbutpos): gauss[:3]=self.model[:3]
			if not butval(self.wbutamp): gauss[3]=self.model[3]
			self.gaussplot.setData(gauss,self.wvssphsz.value)
			self.wview3d.update()

			self.wplot2d.add_shape("latent",EMShape(["scrlabel",0.1,0.1,0.1,10.,10.,f"{np.array_str(latent[0], precision=3)} {nptcl} ptcl",120.0,-1]))
			self.wplot2d.update()
			return
		elif mmode in ("Make Set R","Make Set #"):
			return
		elif mmode=="Line of Sets":
			self.line_origin=loc

		else: print("mode error")


	def plot_mouse_drag(self,event,loc):
		mmode=str(self.wcbpntpln.currentText())
		if self.mouseabort:
			print("Abort")
			self.wplot2d.del_shape("genline")
			self.mouseabort=False
			self.line_origin=None
			return

		if mmode=="Line of Sets":
			self.wplot2d.add_shape("genline",EMShape(["line",0.1,0.8,0.1,self.line_origin[0],self.line_origin[1],loc[0],loc[1],1]))
			self.wplot2d.update()
		else:
			self.plot_mouse(event,loc)


		#mmode=str(self.wcbpntpln.currentText())
		#if mmode != "Line":
			#return
		


	def plot_mouse_up(self,event,loc):
		mmode=str(self.wcbpntpln.currentText())
		dim=self.currun.get("dim",4)
		xcol=self.wsbxcol.value()
		ycol=self.wsbycol.value()
		
		if self.mouseabort:
			print("Abort")
			self.wplot2d.del_shape("genline")
			self.mouseabort=False
			self.line_origin=None
			return

		try: rad=float(self.wedrad.text())
		except: 
			print("invalid radius, using 0.05")
			rad=0.05
		try: ptperset=int(self.wedcnt.text())
		except:
			print("invalid particle count, using 4000")
			ptperset=4000

		xcol=self.wsbxcol.value()
		ycol=self.wsbycol.value()
		self.wplot2d.del_shapes(["region","genline"])

		if mmode=="Make Set R":
			# This will produce a list of indices where the distance in latent space is less than the specified rad
#			ptdist=(np.sum((self.midresult.transpose()-latent)**2,1)<(rad**2)).nonzero()[0]
			ptdist=((self.midresult[xcol]-loc[0])**2+(self.midresult[ycol]-loc[1])**2<(rad**2)).nonzero()[0]
			sz=good_size(self.jsparm["boxsize"]*5//4)

			vec=np.zeros(len(self.midresult))
			vec[xcol]=loc[0]
			vec[ycol]=loc[1]
			nset=good_num(self.curmaps)
			newmap=[None,local_datetime(),list(vec),0,0,ptdist]
			self.curmaps[str(nset)]=newmap
			self.sets_changed(nset)

			return
		if mmode=="Make Set #":
			# This will produce a list of indices where the distance in latent space is less than the specified rad
#			ptdist=(np.sum((self.midresult.transpose()-latent)**2,1)<(rad**2)).nonzero()[0]
			ptdist=((self.midresult[xcol]-loc[0])**2+(self.midresult[ycol]-loc[1])**2)
			sel=np.argsort(ptdist)[:ptperset]

			vec=np.zeros(len(self.midresult))
			vec[xcol]=loc[0]
			vec[ycol]=loc[1]
			nset=good_num(self.curmaps)
			newmap=[None,local_datetime(),list(vec),0,0,sel]
			self.curmaps[str(nset)]=newmap
			self.sets_changed(nset)

			return
		elif mmode=="Line of Sets":

			dst=hypot((self.line_origin[0]-loc[0]),(self.line_origin[1]-loc[1]))
			stps=floor(dst/rad)+1		# number of points to generate along the line

			# now we generate a set for each point on the line
			for i in range(stps+1):
				cen=((self.line_origin[0]*(stps-i)+loc[0]*i)/stps,(self.line_origin[1]*(stps-i)+loc[1]*i)/stps)

				ptdist=((self.midresult[xcol]-cen[0])**2+(self.midresult[ycol]-cen[1])**2)
				sel=np.argsort(ptdist)[:ptperset]

				vec=np.zeros(len(self.midresult))
				vec[xcol]=loc[0]
				vec[ycol]=loc[1]
				nset=good_num(self.curmaps)
				newmap=[None,local_datetime(),list(vec),0,0,sel]
				self.curmaps[str(nset)]=newmap
			self.sets_changed(nset)
			return

	def sets_changed(self,nnew=None):
		curmaps={}
		dim=self.currun.get("dim",4)
		for k in self.curmaps:
			curmaps[k]=self.curmaps[k][:5]+[list(self.curmaps[k][5])]
			if isinstance(self.curmaps[k][2][0],int):
				curmaps[k][2]=[list(range(2,2+dim)),self.curmaps[k][2]]
		allmaps=self.jsparm.getdefault("sets",{})
		allmaps[self.currunkey]=curmaps
		self.jsparm["sets"]=allmaps
		self.update_maptable(nnew)

	def new_map(self,ign=None):
		"""Start threads to generate a new full scale map for all selected sets"""

		sz=good_size(self.jsparm["boxsize"]*5//4)
		for k in self.curmaps_sel:
			st=self.curmaps_sel[k]
			rparms={"size":(sz,sz,sz),"sym":self.jsparm["sym"],"mode":"gauss_2","usessnr":0,"verbose":0}
			self.threads.append(threading.Thread(target=make3d_thr,args=(self.threadq,len(self.threads),f"{self.gmm}/particles.lst",st[5],rparms,st[2],self.currun,0,"Full",k,self.jsparm["boxsize"])))
			self.threads[-1].start()
			print(f"Thread {len(self.threads)} started with {len(st[5])} particles")


	def new_map_fast(self,ign=None):
		"""Start threads to generate a new downsampled map for all selected sets"""

		sz=good_size(self.jsparm["boxsize"]*5//4)
		for k in self.curmaps_sel:
			st=self.curmaps_sel[k]
			rparms={"size":(sz,sz,sz),"sym":self.jsparm["sym"],"mode":"gauss_2","usessnr":0,"verbose":0}
			self.threads.append(threading.Thread(target=make3d_thr_fast,args=(self.threadq,len(self.threads),f"{self.gmm}/particles.lst",st[5],rparms,st[2],self.currun,0,"Full",k,self.jsparm["boxsize"])))
			self.threads[-1].start()
			print(f"Thread {len(self.threads)} started with {len(st[5])} particles")

	def new_map_gauss(self,ign=None):
		"""generate gaussian maps from set centers"""

		sz=good_size(self.jsparm["boxsize"]*5//4)
		sym=self.jsparm["sym"]
		for k in self.curmaps_sel:
			st=self.curmaps_sel[k]

# TODO: Implement!

	def set_del(self,ign=None):
		"""Delete an existing set (only if a map hasn't been computed for it)"""

		delerr=False
		for k in self.curmaps_sel:
			st=self.curmaps_sel[k]
			if st[0] is not None:
				delerr=True
				continue
			del self.curmaps[k]

		self.sets_changed()

		if delerr: QtWidgets.QMessageBox.warning(None, "Warning", "Warning: sets with computed maps not deleted!")

	def set_save(self,ign=None):
		"""Save one or more sets to a new LST file for further processing"""
		if len(self.maplist.selectedItems())==0:
			showerror("One or more sets must be selected. Selected sets will be merged into a single .lst file.")
			return

		imgns=[]
		ks=[]
		for k in self.curmaps_sel:
			ks.append(k)
			st=self.curmaps_sel[k]
			imgns.extend(list(st[5]))
		imgns.sort()

		lsin=LSXFile(f"{self.gmm}/particles.lst")

		# check if this is subtomogram data, in which case we need to retrieve 3D particles too
		tmp=lsin[0][2]
		if "ptcl3d_id" in tmp: ptcl3d=set()	# we use this set to aggregate the 3-D particle ids we need to keep
		else: ptcl3d=None

		# This is tricky. If we have subtomogram data, in addition to extracting only the 3-D particles referenced
		# by 2-D particles, we also have to remap the 3-D particle numbers in each 2-D particle!
		if ptcl3d is not None:
			# Construct a set containing all 3-D particle numbers we need to keep
			for n in imgns:
				ptcl3d.add(lsin[n][2]["ptcl3d_id"])

			ptcl3d=sorted(ptcl3d)	# set -> sorted list
			map3d2d={j:i for i,j in enumerate(ptcl3d)}		# maps a 3D particle number from the original file to the new file

			outfsp=f"{self.gmm}/{self.currunkey}_set2d_{'-'.join(ks)}.lst"
			try: os.unlink(outfsp)
			except: pass
			lsout=LSXFile(outfsp)

			# first the 2-D particles with corrections
			for n in imgns:
				pt=lsin[n]
				pt[2]["ptcl3d_id"]=map3d2d[pt[2]["ptcl3d_id"]]		# fix the old 3D particle id
				lsout[-1]=pt		# despite the odd notation, -1 does work this way

			# input and output files for 3D
			rpath=self.jsparm["refinepath"]
			ppth=[f for f in os.listdir(rpath) if "aliptcls3d_" in f]	# find all 3D lsts
			lsin=LSXFile(f"{rpath}/{max(ppth)}")	# find the highest numbered 3D lst
			outfsp2=f"{self.gmm}/{self.currunkey}_set3d_{'-'.join(ks)}.lst"
			try: os.unlink(outfsp2)
			except: pass

			lsout=LSXFile(outfsp2)
			for i,n in enumerate(ptcl3d):
				lsout[i]=lsin[n]

			print(f"{len(imgns)} selected particles saved to {outfsp} and {len(ptcl3d)} to {outfsp2}")

		else:	# SPA particles not SPT partcles
			outfsp=f"{self.gmm}/{self.currunkey}_set_{'-'.join(ks)}.lst"
			try: os.unlink(outfsp)
			except: pass
			lsout=LSXFile(outfsp)

			for n in imgns:
				lsout[-1]=lsin[n]		# despite the odd notation, -1 does work this way

			print(f"{len(imgns)} selected particles saved to {outfsp}")

	def do_kmeans(self):
		print("kmeans ...")
		get_application().setOverrideCursor(Qt.BusyCursor)
		from sklearn.cluster import KMeans
		nseg=int(self.wvbnsets.getValue())
		cols=np.array(parse_range(self.wstbaxes.getValue()))

		try: nset=max([int(k) for k in self.curmaps])+1
		except: nset=0

		try:
			kmseg=KMeans(n_clusters=nseg,init='k-means++')
			classes=kmseg.fit_predict(self.data[cols].transpose())
		except:
			showerror("Problem with K-means parameters")
			traceback.print_exc()
			return

		for i in range(nseg):
			ptdist=np.where(classes==i)[0]
			newmap=[None,local_datetime(),(cols,kmseg.cluster_centers_[i]),0,0,ptdist]
			self.curmaps[str(nset+i)]=newmap

		self.sets_changed()
		print("done")
		get_application().restoreOverrideCursor()

	def do_watershed(self):
		nseg=int(self.wvbnsets.getValue())		# targeted number of segments
		npts=len(self.data[0])					# number of particles/points
		cols=np.array(parse_range(self.wstbaxes.getValue()))
		if len(cols) not in (2,3) :
			showerror(f"Watershed only works on 2 or 3 selected dimensions, you selected {cols}")
			return

		get_application().setOverrideCursor(Qt.BusyCursor)
		print("watershed ...")
		print(nseg,npts,cols)

		# 2-D watershed
		if len(cols)==2:
			nbins=max(int(sqrt(npts//16)),20)
			ctrmap,xe,ye=np.histogram2d(self.data[cols[0]],self.data[cols[1]],bins=nbins)		# make a histogram image
			dens=from_numpy(ctrmap)
			dens.process_inplace("filter.lowpass.gauss",{"cutoff_abs":0.2})		# smooth the histogram a bit to reduce noise, but keep sampling high enough for watershed to work
			seg=dens.process("segment.watershed",{"nseg":nseg,"thr":10.0,"verbose":1})		# apply watershed segmentation to the histogram image

			nseg=int(seg["maximum"]+1)
			ptdist=[[] for i in range(nseg)]

			# x and y coordinates converted to bin numbers (pixel coordinates)
			xs=((self.data[cols[0]]-xe[0])*nbins/(xe[-1]-xe[0])).astype("int32")
			ys=((self.data[cols[1]]-ye[0])*nbins/(ye[-1]-ye[0])).astype("int32")
			print(xs[:10],ys[:10])
			for i in range(npts):
				try: ptdist[int(seg[xs[i],ys[i]])].append(i)
				except: print(f"Err: {i}\t{xs[i]}\t{ys[i]}\t{seg[xs[i],ys[i]]}")

			try: nset=max([int(k) for k in self.curmaps])+1
			except: nset=0
		# 3-D watershed
		else:
			nbins=max(int(pow(npts,0.333)),12)
			ctrmap,edg=np.histogramdd((self.data[cols[0]],self.data[cols[1]],self.data[cols[2]]),bins=nbins)		# make a histogram image
			xe,ye,ze=edg
			dens=from_numpy(ctrmap)
			dens.process_inplace("filter.lowpass.gauss",{"cutoff_abs":0.2})		# smooth the histogram a bit to reduce noise, but keep sampling high enough for watershed to work
			seg=dens.process("segment.watershed",{"nseg":nseg,"thr":5.0,"verbose":1})		# apply watershed segmentation to the histogram image

			nseg=int(seg["maximum"]+1)
			ptdist=[[] for i in range(nseg)]

			# x and y coordinates converted to bin numbers (pixel coordinates)
			xs=((self.data[cols[0]]-xe[0])*nbins/(xe[-1]-xe[0])).astype("int32")
			ys=((self.data[cols[1]]-ye[0])*nbins/(ye[-1]-ye[0])).astype("int32")
			zs=((self.data[cols[2]]-ze[0])*nbins/(ze[-1]-ze[0])).astype("int32")
			for i in range(npts):
				try: ptdist[int(seg[xs[i],ys[i],zs[i]])].append(i)
				except: print(f"Err: {i}\t{xs[i]}\t{ys[i]}\t{seg[xs[i],ys[i]]}")

			try: nset=max([int(k) for k in self.curmaps])+1
			except: nset=0

		# dens.write_image("ws_d.hdf",0)
		# seg.write_image("ws_s.hdf",0)

		for i in range(1,nseg):		# we skip class 0, which will generally wrap around the others
			if len(ptdist[i])>0 :
				self.curmaps[str(nset+i)]=[None,local_datetime(),(cols,None),0,0,np.array(ptdist[i])]

		self.sets_changed()
		print("done")
		get_application().restoreOverrideCursor()

	def do_dbscan(self):
		get_application().setOverrideCursor(Qt.BusyCursor)
		print("OpticsDB ...")
		t0=time.time()
		from sklearn.cluster import OPTICS
		nseg=int(self.wvbnsets.getValue())
		cols=np.array(parse_range(self.wstbaxes.getValue()))

		try: nset=max([int(k) for k in self.curmaps])+1
		except: nset=0

		opseg=OPTICS(n_jobs=-1,cluster_method="dbscan",max_eps=0.25)
		classes=opseg.fit_predict(self.data[cols].transpose())
		nseg=max(classes)+1
		if (nseg>100):
			print(f"{nseg} classes found, aborting")
			get_application().restoreOverrideCursor()
			return

		for i in range(nseg):
			ptdist=np.where(classes==i)[0]
			try:
				sel=self.data[:,ptdist]
				cen=np.mean(sel[cols],1)
				newmap=[None,local_datetime(),[cols,cen],0,0,ptdist]
			except:
				traceback.print_exc()
				print(self.data.shape,cols)
				newmap=[None,local_datetime(),(cols,None),0,0,ptdist]
			self.curmaps[str(nset+i)]=newmap

		self.sets_changed()
		print(f"done ({time.time()-t0}s)")
		get_application().restoreOverrideCursor()

	def do_optics(self):
		get_application().setOverrideCursor(Qt.BusyCursor)
		print("OpticsXi ...")
		t0=time.time()
		from sklearn.cluster import OPTICS
		nseg=int(self.wvbnsets.getValue())
		cols=np.array(parse_range(self.wstbaxes.getValue()))

		try: nset=max([int(k) for k in self.curmaps])+1
		except: nset=0

		opseg=OPTICS(n_jobs=-1,cluster_method="xi",max_eps=0.25)
		classes=opseg.fit_predict(self.data[cols].transpose())
		nseg=max(classes)+1
		if (nseg>100):
			print(f"{nseg} classes found, aborting")
			get_application().restoreOverrideCursor()
			return

		for i in range(nseg):
			ptdist=np.where(classes==i)[0]
			try:
				sel=self.data[:,ptdist]
				cen=np.mean(sel[cols],1)
				newmap=[None,local_datetime(),[cols,cen],0,0,ptdist]
			except:
				traceback.print_exc()
				print(self.data.shape,cols)
				newmap=[None,local_datetime(),(cols,None),0,0,ptdist]
			self.curmaps[str(nset+i)]=newmap

		self.sets_changed()
		print(f"done ({time.time()-t0}s)")
		get_application().restoreOverrideCursor()

	def do_spectral(self):
		get_application().setOverrideCursor(Qt.BusyCursor)
		print("spectral ...")
		t0=time.time()
		from sklearn.cluster import SpectralClustering
		nseg=int(self.wvbnsets.getValue())
		cols=np.array(parse_range(self.wstbaxes.getValue()))

		try: nset=max([int(k) for k in self.curmaps])+1
		except: nset=0

		spseg=SpectralClustering(n_jobs=-1)
		classes=spseg.fit_predict(self.data[cols].transpose())
		nseg=max(classes)+1
		for i in range(nseg):
			ptdist=np.where(classes==i)[0]
			try:
				cen=np.mean(self.data[cols,ptdist],1)
				newmap=[None,local_datetime(),[cols,cen],0,0,ptdist]
			except:
				traceback.print_exc()
				print(self.data[cols,ptdist].shape)
				newmap=[None,local_datetime(),(cols,None),0,0,ptdist]
			self.curmaps[str(nset+i)]=newmap

		self.sets_changed()
		print(f"done ({time.time()-t0}s)")
		get_application().restoreOverrideCursor()

	#def plot_mode_sel(self,but):
		#"""Plot mode selected"""
		
		#self.plotmode=self.wbutdrgrp.id(but)
		#newdim=self.wsbnewdim.value()
		
		#if self.plotmode==0:
##			print("norm")
			#self.data=self.midresult
		#elif self.plotmode==1:
##			print("pca")
			#self.decomp=skdc.PCA(n_components=newdim)
			#self.data=self.decomp.fit_transform(self.midresult.transpose()).transpose()
		#elif self.plotmode==2:
			#pass
			
		#self.wsbxcol.setRange(0,len(self.data)-1)
		#self.wsbxcol.setValue(0)
		#self.wsbycol.setRange(0,len(self.data)-1)
		#self.wsbycol.setValue(1)
		
##		print(self.data.shape)
		#self.wplot2d.set_data(self.data,"map",symsize=1)

	def update_axes_x(self,x):
		self.wplot2d.setXAxisAll(x,True)
#		self.wplot2d.autoscale()
		self.wplot2d.updateGL()
		
	def update_axes_y(self,y):
		self.wplot2d.setYAxisAll(y,True)
#		self.wplot2d.autoscale()
		self.wplot2d.updateGL()
	
	def plot_annotate(self,fig,ax):
		"""This is called as a callback during rendering of the 2-D plot"""
		#xa=self.wsbxcol.value()
		#ya=self.wsbycol.value()
		#for i,k in enumerate(self.curmaps_sel):
			#m=self.curmaps_sel[k]
			#try:
				#if self.plotmode==1:
					#xm=self.decomp.transform(m[2].reshape(1,len(m[2])))[0]	# expects an array of vectors
					#c=Circle((xm[xa],xm[ya]),m[4],edgecolor="gold",facecolor="none")
				#else:
					#c=Circle((m[2][xa],m[2][ya]),m[4],edgecolor="gold",facecolor="none")
				#ax.add_artist(c)
	##			print((m[2][xa],m[2][ya]),m[4])
			#except:
				#print(f"Plot error '{m}' {xa} {ya}")

	def new_3d_opt(self,clk=False):
		"""When the user changes selections for the 3-D display"""
		self.gaussplot.setVisibleItem(butval(self.wbutdmdl))
		self.mapdataitem.setVisibleItem(butval(self.wbutnmap))
		self.neutralplot.setVisibleItem(butval(self.wbutnmdl))
		self.maskdataitem.setVisibleItem(butval(self.wbutdmask))
		if self.dmapdataitem is not None :
			self.dmapdataitem.setVisibleItem(butval(self.wbutdmap))
		#print(self.gaussplot.isVisibleItem(),self.mapdataitem.isVisibleItem())

		try: self.dmapiso.getTransform().set_scale(self.cur_dyn_vol["apix_x"]/self.jsparm["apix"])
		except: pass
		try: self.maskiso.getTransform().set_scale(self.mask["apix_x"]/self.jsparm["apix"])
		except: pass

		self.wview3d.updateGL()

	def set3dvis(self,neumap=1,dynmap=1,filtmap=1,neumdl=1,dynmdl=1,blankplot=0):
		"""sets the visibility of various 3-D display components. 1 enables, 0 disables, -1 leaves unchanged"""
		if neumap>=0 : self.wbutnmap.setChecked(True)
		try: 
			if dynmap>=0: self.wbutdmap.setChecked(True)
		except: pass
		try: 
			if filtmap>=0: self.fmapdataitem.setVisibleItem(filtmap)
		except: pass
		if dynmdl>=0: self.wbutdmdl.setChecked(True)
		if neumdl>=0: self.wbutnmdl.setChecked(True)
		if blankplot: 
			self.wplot2d.set_data(None,replace=True,symsize=1)

	def new_res(self):
		"""Resolution changed. Update the initial points"""
		try: res=float(self.wedres.text())
		except: return
		self.lastres=res
		
		map3d=EMData(f"{self.gmm}/input_map.hdf")

		self.saveparm("neutral")  # updates self.currun with current user input

		# we briefly supported "enhancement of densities in the masked region. detect and remove
		try:
			val=float(str(self.wedmask.text()).split(",")[1])
			showerror("old style mask enhancement detected. Ignoring/removing")
			self.wedmask.setText(self.wedmask.text().split(",")[0])
		except:
			pass

		thrt=self.wedgthr.text()
		try: minpos,minneg=float(thrt.split(",")[0]),float(thrt.split(",")[1])
		except:
			try:
				minpos=float(thrt)
				minneg=minpos*1.25
			except:
				showerror("Invalid threshold, either POS or POS,NEG, typ: 0.3,0.4")
				return
		opt={"minratio":minpos,"width":res,"skipseg":2}
#		if mask!=None: opt["mask"]=mask

		try: sym=self.currun.setdefault("sym","c1")
		except:
			showerror("Unable to get symmetry. Have you created a run yet?")
			return

		print(f"sym {sym}")
		if sym.lower()!="c1" :
			mask=map3d.process("mask.asymunit",{"au":0,"sym":sym})
			map3d.mult(mask)
			mask=None

		# Strong positive peaks
		seg=map3d.process("segment.gauss",opt)
		print("pos:",len(seg["segment_amps"]))
		map3d2=map3d.process("normalize.edgemean")
		map3d2.mult(-1.0)

		# strong negative peaks
		opt["minratio"]=minneg
		segneg=map3d2.process("segment.gauss",opt)
		print("neg:",len(segneg["segment_amps"]))
#		amps=np.array(seg["segment_amps"]+segneg["segment_amps"])
		if self.currun["pas"][1]=="0":
			print("No amplitude modification, so 'negative' peaks set to small positive value")
			amps=np.append(seg["segment_amps"],np.zeros(len(segneg["segment_amps"]))+.05)
			centers=np.array(seg["segment_centers"]+segneg["segment_centers"]).reshape((len(amps),3)).transpose()
		else:
			print("negative peaks set to small negative value")
			amps=np.append(seg["segment_amps"],np.zeros(len(segneg["segment_amps"]))-.05)
			centers=np.array(seg["segment_centers"]+segneg["segment_centers"]).reshape((len(amps),3)).transpose()
		try: amps/=max(amps)
		except:
			print("ERROR: no gaussians at specified threshold")
			return

		if self.fmapdataitem==None:
			self.fmapdataitem=EMDataItem3D(seg)
			self.fmapiso=EMIsosurface(self.fmapdataitem)
			self.wview3d.insertNewNode("Filt or Gauss Map",self.fmapdataitem,parentnode=self.wview3d)
			self.wview3d.insertNewNode("Isosurface",self.fmapiso,parentnode=self.fmapdataitem)
		else:
			self.fmapdataitem.setData(seg)
			
#		self.wedngauss.setText(str(len(amps)*4//3))
		self.wedngauss.setText(f"{str(len(amps))} Gau")

		print(f"Resolution={res} -> Ngauss={len(amps)}  ({self.currunkey})")
		
		nx=map3d["nx"]
		centers[0]-=nx/2
		centers[1]-=nx/2
		centers[2]-=nx/2
		self.centers=centers
		self.amps=amps

		self.group_by_mask()

		if self.cengroups is None: self.neutralplot.setData(np.concatenate((centers,[amps])),self.wvssphsz.value)
		else: self.neutralplot.setData(np.concatenate((centers,[amps],[np.ones(len(amps))],[self.cengroups])),self.wvssphsz.value)
		self.wview3d.update()

		# write the new seg model to disk for use in subsequent runs
		modelseg=f"{self.gmm}/{self.currunkey}_model_seg.txt"
		
		if self.cengroups is None:
			with open(modelseg,"w") as out:
				for i in range(len(self.amps)):
					try: out.write(f"{self.centers[0,i]/nx:1.2f}\t{-self.centers[1,i]/nx:1.2f}\t{-self.centers[2,i]/nx:1.2f}\t{self.amps[i]:1.3f}\n")
					except: print("write error: ",self.centers[:,i],self.amps[i],self.amps.shape,i)
		else:
			with open(modelseg,"w") as out:
				for i in range(len(self.amps)):
					try: out.write(f"{self.centers[0,i]/nx:1.2f}\t{-self.centers[1,i]/nx:1.2f}\t{-self.centers[2,i]/nx:1.2f}\t{self.amps[i]:1.3f}\t1.0\t{self.cengroups[i]:1.0f}\n")
					except:
						if i==0:
							traceback.print_exc()
							print(len(self.cengroups),self.cengroups)
						if i<10: print("write error: ",self.centers[:,i],self.amps[i],self.amps.shape,i)

		self.set3dvis(0,0,1,1,0,1)

	def new_neutral(self):
		"""Makes a new neutral model, which will be used by any subsequent runs"""
		# Compute and save initial centers
		prog=QtWidgets.QProgressDialog("Running neutral model network. Progress updates here are limited. See the Console for detailed output.","Abort",0,4)
		prog.show()

		# Regenerate the initial model by segmentation (writes to modelseg file)
		# this also updates self.jsparm
		self.new_res()

		sym=self.currun["sym"]
		maxbox=(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/self.currun["targres"])//2)*2
		minboxp =(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/self.currun["ptclminres"]))
		modelout=f"{self.gmm}/{self.currunkey}_model_gmm.txt"
		modelseg=f"{self.gmm}/{self.currunkey}_model_seg.txt"
		prog.setValue(1)

		nx=int(self.jsparm["boxsize"])
		ncen=len(self.amps)		# number of centers from original segmentation
		if (ncen==0) :
			showerror("No centers determined at current resolution!")
			return
			
		if int(self.currun["conv"]): conv="--conv"
		else: conv=""
		decoder=f"{self.gmm}/{self.currunkey}_decoder.h5"

		
		# we extract a subset of the input particles targeting ~10k
		try: os.unlink(f"{self.gmm}/particles_subset.lst")
		except: pass
		lsx=LSXFile(f"{self.gmm}/particles.lst",True)
		lsxs=LSXFile(f"{self.gmm}/particles_subset.lst")
		step=max(len(lsx)//5000,1)
		for i in range(0,len(lsx),step):
			lsxs.write(-1,*lsx.read(i))
		print(f"Subset of {len(lsxs)} particles extracted to train neutral model")
		lsxs.close()
		lsx=None
		lsxs=None

		# refine the neutral model against some real data in entropy training mode
		er=run(f"e2gmm_refine.py --projs {self.gmm}/particles_subset.lst  --npt {self.currun['ngauss']} --decoderentropy --npt {self.currun['ngauss']} --sym {sym} --maxboxsz {maxbox} --minressz {minboxp} --model {modelseg} --modelout {modelout} --niter 10  --nmid {self.currun['dim']} --evalmodel {self.gmm}/{self.currunkey}_model_projs.hdf --evalsize {self.jsparm['boxsize']} --decoderout {decoder} {conv} --ampreg 0.1 --sigmareg 1.0")
		if er :
			showerror("Error running e2gmm_refine, see console for details. GPU memory exhaustion is a common issue. Consider reducing the target resolution.")
			return

		# Now we train latent zero to the neutral conformation
		er=run(f"e2gmm_refine.py --projs {self.gmm}/proj_in.hdf --decoderin {decoder} --sym {sym} --maxboxsz {maxbox} --minressz {minboxp} --model {modelseg} --modelout {modelout} --niter 20  --nmid {self.currun['dim']} --evalmodel {self.gmm}/{self.currunkey}_model_projs.hdf --evalsize {self.jsparm['boxsize']} --decoderout {decoder} {conv} --modelreg {self.currun['modelreg']} --ampreg 1.0")
		if er :
			showerror("Error running e2gmm_refine, see console for details. GPU memory exhaustion is a common issue. Consider reducing the target resolution.")
			return
		prog.setValue(2)
		
		pts=np.loadtxt(modelout).transpose()
		pts[1]*=-1.0
		pts[2]*=-1.0
		pts[:3,:]*=nx

		n2c=min(ncen,pts.shape[1])
		pts[:3,:n2c]=self.centers[:,:n2c]
		pts[3:4,:n2c]=self.amps[:n2c]
		pts[4,:n2c]=1.0

		self.centers=pts[:3,:]
		self.amps=pts[3]
		if len(pts)>5: self.cengroups=pts[5]
		else: self.cengroups=None
		
		self.neutralplot.setData(self.pts,self.wvssphsz.value)
		self.wview3d.updateGL()
		self.do_events()
		prog.setValue(3)

		# make3d on gaussian output for comparison
		er=run(f"e2make3dpar.py --input {self.gmm}/{self.currunkey}_model_projs.hdf --output {self.gmm}/{self.currunkey}_model_recon.hdf --pad {good_size(self.jsparm['boxsize']*1.25)} --mode trilinear --keep 1 --threads {self.options.threads}")

		# Display the reconstructed Gaussian map
		seg=EMData(f"{self.gmm}/{self.currunkey}_model_recon.hdf")
		self.fmapdataitem.setData(seg)
		prog.setValue(4)
		self.currun=self.jsparm["run_"+self.currunkey]
		self.currun["time_neutral_end"]=local_datetime()
		self.jsparm["run_"+self.currunkey]=self.currun
		
		self.set3dvis(1,0,0,1,0,1)

	def new_neutral2(self):
		"""Makes a new neutral model, which will be used by any subsequent runs"""
		# Compute and save initial centers
		prog=QtWidgets.QProgressDialog("Running neutral model network. Progress updates here are limited. See the Console for detailed output.","Abort",0,4)
		prog.show()

		# Regenerate the initial model by segmentation (writes to modelseg file)
		self.new_res()

		self.saveparm("neutral")  # updates self.currun with current user input

		sym=self.currun["sym"]
		maxbox=(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/self.currun["targres"])//2)*2
		modelout=f"{self.gmm}/{self.currunkey}_model_gmm.txt"
		modelseg=f"{self.gmm}/{self.currunkey}_model_seg.txt"
		prog.setValue(1)

		nx=int(self.jsparm["boxsize"])
		ncen=len(self.amps)		# number of centers from original segmentation
		if (ncen==0) :
			showerror("No centers determined at current resolution!")
			return

		if int(self.currun["conv"]): conv="--conv"
		else: conv=""
		decoder=f"{self.gmm}/{self.currunkey}_decoder.h5"

		# we extract a subset of the input particles targeting ~10k
		try: os.unlink(f"{self.gmm}/particles_subset.lst")
		except: pass
		lsx=LSXFile(f"{self.gmm}/particles.lst",True)
		lsxs=LSXFile(f"{self.gmm}/particles_subset.lst")
		step=max(len(lsx)//10000,1)		# no more than ~10k particles in initial training
		for i in range(0,len(lsx),step):
			lsxs.write(-1,*lsx.read(i))
		print(f"Subset of {len(lsxs)} particles extracted to train neutral model")
		lsxs.close()
		lsx=None
		lsxs=None

		# refine the neutral model against some real data in entropy training mode
		er=run(f"e2gmm_refine_point.py --projs {self.gmm}/particles_subset.lst --decoderentropy --npt {self.currun['ngauss']} --sym {sym} --maxboxsz {maxbox} --model {modelseg} --modelout {modelout} --niter 10  --nmid {self.currun['dim']} --evalmodel {self.gmm}/{self.currunkey}_model_projs.hdf --evalsize {self.jsparm['boxsize']} --decoderout {decoder} {conv} --ampreg 0.1  --ptclsclip {self.jsparm['boxsize']}")
		if er :
			showerror("Error running e2gmm_refine, see console for details. GPU memory exhaustion is a common issue. Consider reducing the target resolution.")
			return

		# Now we train latent zero to the neutral conformation
		er=run(f"e2gmm_refine_point.py --projs {self.gmm}/proj_in.hdf --decoderin {decoder} --sym {sym} --maxboxsz {maxbox} --model {modelseg} --modelout {modelout} --niter 20  --nmid {self.currun['dim']} --evalmodel {self.gmm}/{self.currunkey}_model_projs.hdf --evalsize {self.jsparm['boxsize']} --decoderout {decoder} {conv} --modelreg {self.currun['modelreg']} --ampreg 1.0  --ptclsclip {self.jsparm['boxsize']}")
		#er=run(f"e2gmm_refine_point.py --projs {self.gmm}/proj_in.hdf  --sym {sym} --maxboxsz {maxbox} --model {modelseg} --modelout {modelout} --niter 20  --nmid {self.currun['dim']} --evalmodel {self.gmm}/{self.currunkey}_model_projs.hdf --evalsize {self.jsparm['boxsize']} --decoderout {decoder} {conv} --modelreg {self.currun['modelreg']} --ampreg 1.0 --ndense -1 --ptclsclip {self.jsparm['boxsize']}")
		if er :
			showerror("Error running e2gmm_refine, see console for details. GPU memory exhaustion is a common issue. Consider reducing the target resolution.")
			return
		prog.setValue(2)

		pts=np.loadtxt(modelout).transpose()
		pts[1]*=-1.0
		pts[2]*=-1.0
		pts[:3,:]*=nx

		# I'm sure there is/was a reason for this, but it isn't obvious to me right now
		if ncen!=pts.shape[1]:
			print(f"Note: number of points from training ({pts.shape[1]}) doesn't match segmenation ({ncen})")
			n2c=min(ncen,pts.shape[1])
			pts[:3,:n2c]=self.centers[:,:n2c]
			pts[3:4,:n2c]=self.amps[:n2c]

		self.centers=pts[:3,:]
		self.amps=pts[3]
		self.group_by_mask()

		# if we did mask based grouping, we need to rewrite the file so the groups are used
		# by the encoder when computing latent vectors during full network training
		if self.cengroups is not None:
			with open(modelout,"w") as out:
				for i in range(len(self.amps)):
					try: out.write(f"{self.centers[0,i]/nx:1.2f}\t{-self.centers[1,i]/nx:1.2f}\t{-self.centers[2,i]/nx:1.2f}\t{self.amps[i]:1.3f}\t1.0\t{self.cengroups[i]:1.0f}\n")
					except:
						if i==0:
							traceback.print_exc()
							print(len(self.cengroups),self.cengroups)
						if i<10: print("write error: ",self.centers[:,i],self.amps[i],self.amps.shape,i)


		if self.cengroups is None: self.neutralplot.setData(pts,self.wvssphsz.value)
		else: self.neutralplot.setData(np.concatenate((pts,[np.ones(len(self.cengroups))],[self.cengroups])),self.wvssphsz.value)
		self.wview3d.updateGL()
		self.do_events()
		prog.setValue(3)

		# make3d on gaussian output for comparison
		er=run(f"e2make3dpar.py --input {self.gmm}/{self.currunkey}_model_projs.hdf --output {self.gmm}/{self.currunkey}_model_recon.hdf --pad {good_size(self.jsparm['boxsize']*1.25)} --mode trilinear --keep 1 --threads {self.options.threads}")

		# Display the reconstructed Gaussian map
		seg=EMData(f"{self.gmm}/{self.currunkey}_model_recon.hdf")
		self.fmapdataitem.setData(seg)
		prog.setValue(4)
		self.currun=self.jsparm["run_"+self.currunkey]
		self.currun["time_neutral_end"]=local_datetime()
		self.jsparm["run_"+self.currunkey]=self.currun

		self.set3dvis(1,0,0,1,0,1)

	def group_by_mask(self):
		"""If masks have been specified, use them to group the gaussians as appropriate"""

		nx=int(self.jsparm["boxsize"])

		# load the masks for later
		try:
			m=str(self.currun["mask"]).split(",")
			if m[0]!="" : masks=[EMData(f) for f in m]
			else: return
		except:
			showerror("Invalid mask. Comma separated list of filenames or blank!")
			return

		# put each Gaussian in the mask-based group it first matches
		# results unspecified for overlapping masks
		# group 0 corresponds to the unmasked region
		groups=np.zeros(len(self.amps))	# float type for compatibility later
		for i in range(len(self.amps)):
			for j in range(len(masks)):
				if masks[j][int(self.centers[0][i])+nx//2,int(self.centers[1][i])+nx//2,int(self.centers[2][i])+nx//2]>0.5:
					groups[i]=j+1
					break

		self.cengroups=groups


	def new_run(self,clk=False):
		"""Create a new run and run() it"""
		nm=QtWidgets.QInputDialog.getText(self,"Run Name","Enter a name for the new run. You will still need to run the subsequent steps.")
		if not nm[1]: return
		name=str(nm[0]).replace(" ","_")
		if "run_"+name not in self.jsparm : self.wlistrun.addItem(name)
		self.currunkey=name
		self.saveparm()		# initialize to avoid messed up defaults later
		self.wlistrun.setCurrentRow(self.wlistrun.count()-1)

	def do_run_new(self,clk=False):
		"""Run the current job with current parameters"""
		self.saveparm("dynamics")  # updates self.currun with current user input

		prog=QtWidgets.QProgressDialog("Running neutral model network. Progress updates here are limited. See the Console for detailed output.","Abort",0,4)
		prog.show()
		
		maxbox =(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/self.currun["targres"])//2)*2
		maxbox25=(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/25.0)//2)*2
		maxboxp =(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/self.currun["ptclres"])//2)*2
		minboxp =(int(self.jsparm["boxsize"]*(2*self.jsparm["apix"])/self.currun["ptclminres"]))
		print(f"Target res {self.currun['targres']} -> max box size {maxbox}")
		modelout=f"{self.gmm}/{self.currunkey}_model_gmm.txt"		# note that this is from the neutral training above, we do not regenerate modelout at the "run" stage
		modelseg=f"{self.gmm}/{self.currunkey}_model_seg.txt"
		
		sym=self.currun["sym"]
		prog=QtWidgets.QProgressDialog("Running networks. Progress updates here are limited. See the Console for detailed output.","Abort",0,2)
		prog.show()
		self.do_events(1)
		
		decoder=f"{self.gmm}/{self.currunkey}_decoder.h5"
		encoder=f"{self.gmm}/{self.currunkey}_encoder.h5"
		if (len(self.currun["mask"])>4) : mask=f"--mask {self.currun['mask'].split(',')[0]}"
		else: mask=""
		# heterogeneity analysis
		if int(self.currun["conv"]): conv="--conv"
		else: conv=""
		
		# if targeting high resolution, we start with 10 iterations at 25 A first  # note 10/22, disabled, may do more harm than good?
		#if maxbox25<maxbox:
			#er=run(f"e2gmm_refine_point.py --model {modelout} --decoderin {decoder} --ptclsin {self.gmm}/particles.lst --heter {conv} --sym {sym} --maxboxsz {maxbox25} --niter 10 {mask} --nmid {self.currun['dim']} --midout {self.gmm}/{self.currunkey}_mid.txt --decoderout {decoder} --modelreg {self.currun['modelreg']} --perturb {self.currun['perturb']} --pas {self.currun['pas']} --ndense -1 --ptclsclip {self.jsparm['boxsize']}")
			#if er :
				#showerror("Error running e2gmm_refine, see console for details. Memory is a common issue. Consider reducing the target resolution.")
				#return
		ptrep=f"{self.gmm}/{self.currunkey}_ptrep_{maxboxp}.hdf"
		#if not os.path.exists(ptrep):

		print("Training network")
		if int(self.currun['batches'])<=1 :
			# We really do need to rerun this each time in case parameters have changed
			print("Pregenerating per-particle Gaussian representation")
			er=run(f"e2gmm_refine_point.py --model {modelout} --ptclsin {self.gmm}/particles.lst --ptclrepout {ptrep} --maxboxsz {maxboxp} --minressz {minboxp} --nmid {self.currun['dim']}")
			print("Training network (single batch)")
			er=run(f"e2gmm_refine_point.py --model {modelout} --decoderin {decoder} --decoderout {decoder} --encoderout {encoder} --ptclsin {self.gmm}/particles.lst --ptclrepin {ptrep} --heter {conv} --sym {sym} --maxboxsz {maxbox} --niter {self.currun['trainiter']} {mask} --nmid {self.currun['dim']} --midout {self.gmm}/{self.currunkey}_mid.txt --modelreg {self.currun['modelreg']} --perturb {self.currun['perturb']} --pas {self.currun['pas']} --ptclsclip {self.jsparm['boxsize']} --minressz {minboxp}")
		else:
			# batched run. Run 10 iterations using each batch of data, and repeat until all requested iterations are complete for all data
			nb=int(self.currun['batches'])
			first=True
			itsize=self.currun['trainiter']//3	# We run 1/3 of the iterations at at a time for all batches
			# We really do need to rerun this each time in case parameters have changed
			print("Pregenerating per-particle Gaussian representation")
			for b in range(nb):
				er=run(f"e2gmm_refine_point.py --model {modelout} --ptclsin {self.gmm}/particles.lst --ptclrepout {ptrep} --maxboxsz {maxboxp} --minressz {minboxp} --chunk {b},{nb} --nmid {self.currun['dim']}")

			print("Training in ",nb," batches")
			for it in range(0,self.currun['trainiter'],itsize):
				nit=min(itsize,self.currun['trainiter']-itsize)
				for b in range(nb):
					if first :
						encin=""
						first=False
					else: encin=f" --encoderin {encoder}"

					er=run(f"e2gmm_refine_point.py --model {modelout} --decoderin {decoder} --decoderout {decoder} --ptclsin {self.gmm}/particles.lst --ptclrepin {ptrep} --heter {conv} --sym {sym} --maxboxsz {maxbox} --niter {nit} {mask} --nmid {self.currun['dim']} --modelreg {self.currun['modelreg']} --perturb {self.currun['perturb']} --pas {self.currun['pas']} --ptclsclip {self.jsparm['boxsize']} --minressz {minboxp} {encin} --encoderout {encoder} --chunk {b},{nb}")

					if er: break

		# generate latent representation for all particles using final trained encoder
		if not er:
			print("Generating latent vectors with final encoder")
			run(f"e2gmm_refine_point.py --encoderin {encoder} --ptclrepin {ptrep} --midout {self.gmm}/{self.currunkey}_mid.txt --model {modelout}")
			self.augment_mid()
		else:
			showerror("Error running e2gmm_refine_point, see console for details. If memory exhausted, increase batches.")
			return

		self.currun=self.jsparm["run_"+self.currunkey]
		self.currun["time_dynamics_end"]=local_datetime()
		self.jsparm["run_"+self.currunkey]=self.currun

		prog.setValue(2)
		self.do_events()
		self.currun=self.jsparm["run_"+self.currunkey]
		self.currun["time_dynamics_end"]=local_datetime()
		self.jsparm["run_"+self.currunkey]=self.currun
		
		self.sel_run(-1)

		self.set3dvis(-1,0,0,0,1,0)


	def update_gmms(self):
		"""Updates the display of gmm_XX folders"""
		#self.gmm=str(self.wlistgmm.currentItem().text())
		self.gmms=[i for i in sorted(os.listdir(".")) if i[:4]=="gmm_" and os.path.isdir(i)]
		self.wlistgmm.clear()
		for i in self.gmms:
			self.wlistgmm.addItem(i)
	
	def sel_gmm(self,line):
		"""Called when the user selects a new GMM from the list. Should not be called directly by the user without updating wlistgmm"""

#		print("sel gmm",line)
		get_application().setOverrideCursor(Qt.BusyCursor)
		if line<0 : 
			#self.wedbox.setText("")
			#self.wedapix.setText("")
			#self.wedsym.setText("c1")
			#self.wlistrun.clear()
			return
		self.gmm=str(self.wlistgmm.item(line).text())
		self.jsparm=js_open_dict(f"{self.gmm}/0_gmm_parms.json")
		# These are associated with the whole GMM
		self.wlpath.setText(f'Path: {self.jsparm.getdefault("refinepath","-")}')
		self.wedbox.setText(f'{self.jsparm.getdefault("boxsize",128)}')
		self.wedapix.setText(f'{self.jsparm.getdefault("apix",0.0):0.5f}')
		self.wedsym.setText(f'{self.jsparm.getdefault("sym","c1")}')
		self.wedmask.setText(f'{self.jsparm.getdefault("mask",f"{self.gmm}/mask.hdf")}')
		
		# these may vary from run to run
		self.wlistrun.clear()
		for k in self.jsparm.keys():
			if k[:4]!="run_": continue
			self.wlistrun.addItem(k[4:])
		self.currunkey=None

		self.sel_run(self.wlistrun.count()-1)
		try: 
			self.map3d=EMData(f'{self.gmm}/input_map.hdf')
			self.mapdataitem.setData(self.map3d)
		except: print("Error: input_map.hdf missing")

		self.set3dvis(1,0,0,0,0,0)
		get_application().restoreOverrideCursor()
			
	def sel_run(self,line):
		"""Called when the user selects a new run from the list. If called with -1, reloads the current run"""
		
#		print("sel_run",line)
		get_application().setOverrideCursor(Qt.BusyCursor)
		if line>=0: 
			self.currunkey=str(self.wlistrun.item(line).text())
			self.currun=self.jsparm.getdefault("run_"+self.currunkey,{"dim":4,"mask":f"{self.gmm}/mask.hdf","trainiter":10,"pas":"100","modelreg":0.5,"perturb":0.1,"time":"-"})
		elif self.currunkey is None or len(self.currunkey)==0 or self.currun is None:
			get_application().restoreOverrideCursor()
			return
	
		self.wedres.setText(f'{self.currun.get("targres",20)}')
		rres=self.currun.get("ptclres",self.currun.get("targres",20))
		rminres=self.currun.get("ptclminres",1000.0)
		if rminres<1000.0 : self.wedrres.setText(f'{rres:1.1f},{rminres:1.0f}')
		else : self.wedrres.setText(f'{rres:1.1f}')
		try:
			lsx=LSXFile(f"{self.gmm}/particles.lst",True)
			if len(lsx)<100000: dbatch=1
			else: dbatch=len(lsx)//50000
		except: dbatch=1
		self.wedbatch.setText(f'{self.jsparm.getdefault("batches",dbatch):d}')
		self.wedapix.setText(f'{self.jsparm.getdefault("apix",0.0):0.5f}')
		self.wedngauss.setText(f'{self.currun.get("ngauss",64)}')
		self.weddim.setText(f'{self.currun.get("dim",4)}')
		self.wedsym.setText(f'{self.currun.get("sym","c1")}')
		self.wedmask.setText(f'{self.currun.get("mask",self.jsparm.getdefault("mask",f"{self.gmm}/mask.hdf"))}')
		self.wedtrainiter.setText(f'{self.currun.get("trainiter",10)}')
		self.wedtrainperturb.setText(f'{self.currun.get("perturb",0.1)}')
		self.wedtrainmodelreg.setText(f'{self.currun.get("modelreg",0.5)}')
		#self.wbutconv.setChecked(int(self.currun.get("conv",1)))
		pas=self.currun.get("pas","100")
		self.wbutpos.setChecked(int(pas[0]))
		self.wbutamp.setChecked(int(pas[1]))
		#self.wbutsig.setChecked(int(pas[2]))
		self.wlabruntime.setText(self.currun.get("time","-"))
		nx=int(self.jsparm.getdefault("boxsize",128))

		# not critical, but display it if we have it
		if self.fmapdataitem!=None:
			try:
				seg=EMData(f"{self.gmm}/{self.currunkey}_model_recon.hdf")
				self.fmapdataitem.setData(seg)
			except:
				self.fmapdataitem.setData(None)

		# Neutral Gaussian model (needed when PAS != 111)
		try: 
			self.model=np.loadtxt(f"{self.gmm}/{self.currunkey}_model_gmm.txt").transpose()
			pts=self.model
			pts[1]*=-1.0
			pts[2]*=-1.0
			pts[:3,:]*=nx
			#n2c=min(ncen,pts.shape[1])
			#pts[:3,:n2c]=self.centers[:,:n2c]
			#pts[3:4,:n2c]=self.amps[:n2c]
			#pts[4,:n2c]=1.0
			self.centers=pts[:3,:]
			self.amps=pts[3]
			if len(pts)>5: self.cengroups=pts[5]
			else: self.cengroups=None

			
			self.neutralplot.setData(pts,self.wvssphsz.value)
			self.wview3d.update()

		except:
			self.neutralplot.setData(None)
			print(f"Neutral gaussian model missing ({self.gmm}/{self.currunkey}_model_gmm.txt)")
			
		# Decoder model for generating Gaussians
		try: 
			self.decoder = tf.keras.models.load_model(f"{self.gmm}/{self.currunkey}_decoder.h5",compile=False)
		except: 
			traceback.print_exc()
			print(f"Run {self.gmm} -> {self.currunkey} results incomplete. No stored decoder found.",self)
			self.set3dvis(1,0,0,0,0,1)
			get_application().restoreOverrideCursor()
			return

		# list of all data subsets for this run, in the old system these were called "maps" but had incomplete info
		allmaps=self.jsparm.getdefault("sets",{})
		try:
				self.curmaps=allmaps[self.currunkey]
				self.curmaps_sel={}
		except: self.curmaps={}
		for k in self.curmaps:
			m=self.curmaps[k]
			# m[2] is now (cols,center), normally a list, doesn't really need to be forced to numpy
			#m[2]=np.array(m[2])		# latent space center
			m[5]=np.array(m[5])		# list of point numbers in set
		
		if not os.path.exists(f"{self.gmm}/{self.currunkey}_aug.txt"): self.augment_mid()

		# Augmented middle layer for every particle, this is 2 columns of PCA, N columns of latent vector, arbitrary columns with dim-reduced representations
		self.wplot2d.del_shapes()
		try: 
			self.midresult=np.loadtxt(f"{self.gmm}/{self.currunkey}_aug.txt").transpose()
			self.data=self.midresult
			self.wplot2d.set_data(self.data,"map",symsize=1,replace=True,quiet=True)
			self.wsbxcol.setRange(0,self.data.shape[1]-1)
			self.wsbycol.setRange(0,self.data.shape[1]-1)
			#self.wbutdrmid.click()
			#self.plot_mode_sel(self.wbutdrmid)		# the previous line will also trigger this, but possibly not before the plot_mouse below

		except:
			traceback.print_exc()
			print(f"Augmented middle layer missing ({self.gmm}/{self.currunkey}_aug.txt)")
			self.set3dvis(1,0,0,0,0,1)
		
#		self.plot_mouse(None,(0,0))
		self.wplot2d.updateGL()
		self.update_maptable()

		try:
			self.mask=EMData(str(self.wedmask.text()).split(",")[0])
			self.maskdataitem.setData(self.mask)
		except: pass

		self.set3dvis(1,0,0,1,1,0)
		get_application().restoreOverrideCursor()

	def augment_mid(self):
		"""This will take the middle layer from the network result and create the "augmented" file with 2-D PCA in the first 2 columns
		The idea is that these columns can be later expanded with other dimensionality reduction methods"""

		try:
			if not os.path.exists(f"{self.gmm}/{self.currunkey}_mid.txt"): raise Exception
			midresult=np.loadtxt(f"{self.gmm}/{self.currunkey}_mid.txt")[:,1:].transpose()

			#prog=QtWidgets.QProgressDialog("Augmenting middle layer with PCA, TSNE and Isomap. May take a while.","Abort",0,4)
			#prog.show()
			#self.do_events()

			print("Compute PCA")
			self.pca=skdc.PCA(n_components=2)
			pcadc=self.pca.fit_transform(midresult.transpose()).transpose()
			print(midresult.shape)
			out=open(f"{self.gmm}/{self.currunkey}_aug.txt","w")
			out.write("# PCA0; PCA1; ")
			out.write("Latent; "*midresult.shape[0])
			out.write("\n")
			for i in range(midresult.shape[1]):
				out.write(f"{pcadc[0][i]:1.4f}\t{pcadc[1][i]:1.4f}\t")
				out.write("\t".join([f"{r:1.4f}" for r in midresult[:,i]]))
				out.write("\n")

			out.close()
			#prog.setValue(4)
			#self.do_events()
		except:
			traceback.print_exc()
			print(f"Middle layer missing ({self.gmm}/{self.currunkey}_mid.txt)")


	def add_gmm(self,clk=False):
		"""Creates a new numbered gmm_XX folder"""
		try: newgmm=num_path_new("gmm")
		except: 
			showerror(f"Cannot create {newgmm}",self)
			return
		self.gmm=newgmm
		ok=self.setgmm_refine(remove=True)
		if ok: self.wlistgmm.addItem(newgmm)
		
	def setgmm_refine(self,clk=False,remove=False):
		"""Allows the user to select the source data for a new gmm_XX folder.
		if remove is set and the user hits cancel, the folder will be removed.""" 
		self.jsparm=js_open_dict(f"{self.gmm}/0_gmm_parms.json")
		
		# double check if the user will be destroying results
		if "refinepath" in self.jsparm:
			ans=QtWidgets.QMessageBox.question(self,"Are you sure?",f"{self.gmm} has already been configured to work on {self.jsparm['refinepath']}. Continuing may invalidate current results. Proceed?")
			if ans==QtWidgets.QMessageBox.No:
				if remove: os.unlink(self.gmm)
				return False
			
		# Get the name of an existing refinement
		try:
			rpath=os.path.relpath(str(QtWidgets.QFileDialog.getExistingDirectory(self,"Please select an existing refine_xx or r3d_xx folder to base the analysis on")))
		except: return
		if not os.path.isdir(rpath) : 
			showerror("Invalid path")
			if remove: os.unlink(self.gmm)
			return False
		self.jsparm["refinepath"]=rpath
		
		if rpath.startswith("refine_"):
			### setup the folder
			try:
				itr=max([int(i.split("_")[1]) for i in os.listdir(rpath) if i[:7]=="threed_" and i.split("_")[1].isdigit()])
			except:
				showerror("No projections in refine folder")
				return

			self.app().setOverrideCursor(Qt.BusyCursor)
			rparm=js_open_dict(f"{rpath}/0_refine_parms.json")
			if rparm["breaksym"] : self.jsparm["sym"]="c1"
			else: self.jsparm["sym"]=rparm["sym"]
			
			# Copy projections from refine folder
			eprj=f"{rpath}/projections_{itr:02d}_even.hdf"
			oprj=f"{rpath}/projections_{itr:02d}_odd.hdf"
			n=EMUtil.get_image_count(eprj)
			for i in range(n):
				a1=EMData(eprj,i)
				a2=EMData(oprj,i)
				a=a1+a2
				a.mult(0.5)
				a.write_compressed(f"{self.gmm}/proj_in.hdf",i,10)
			self.jsparm["boxsize"]=a["nx"]
			self.jsparm["apix"]=a["apix_x"]
	#		self.jsparm["res"]=file_resolution(f"{rpath}/fsc_maskedtight_{itr:02d}.txt")

			# Copy map from refine folder
			a=EMData(f"{rpath}/threed_{itr:02d}.hdf")
			a.write_compressed(f"{self.gmm}/input_map.hdf",0,12)
			self.jsparm["source_map"]=f"{rpath}/threed_{itr:02d}.hdf"
			
			# Copy mask from refine folder
			a=EMData(f"{rpath}/mask_tight.hdf")
			a.write_compressed(f"{self.gmm}/mask.hdf",0,8)
			self.jsparm["mask"]=f"{self.gmm}/mask.hdf"

			# Extract particles from refine folder
			run(f"e2evalrefine.py {rpath} --extractorientptcl {self.gmm}/particles.lst")
			self.app().setOverrideCursor(Qt.ArrowCursor)
		elif rpath.startswith("r3d_"):
			### setup the folder
			try:
				itr=max([int(i.split("_")[1]) for i in os.listdir(rpath) if i[:7]=="threed_" and i.split("_")[1].isdigit()])
			except:
				showerror("No threed_xx files in refine folder")
				return

			self.app().setOverrideCursor(Qt.BusyCursor)
			rparm=js_open_dict(f"{rpath}/0_spa_params.json")
			#if rparm["breaksym"] : self.jsparm["sym"]="c1"
			#else: self.jsparm["sym"]=rparm["sym"]
			
			# Copy map from refine folder
			a=EMData(f"{rpath}/threed_{itr:02d}.hdf")
			a.write_compressed(f"{self.gmm}/input_map.hdf",0,12)
			self.jsparm["source_map"]=f"{rpath}/threed_{itr:02d}.hdf"
			self.jsparm["boxsize"]=a["nx"]
			self.jsparm["apix"]=a["apix_x"]
			try: self.jsparm["sym"]=rparm["sym"]
			except:
				self.jsparm["sym"]="c1"
				print("symmetry missing, assuming C1")
				
			# make projection from threed
			run(f"e2project3d.py {rpath}/threed_{itr:02d}.hdf --outfile {self.gmm}/proj_in.hdf --orientgen rand:n=500:phitoo=1 --sym c1 --parallel thread:5")
			
			# Copy mask from refine folder
			a=EMData(f"{rpath}/mask_tight.hdf")
			a.write_compressed(f"{self.gmm}/mask.hdf",0,8)
			self.jsparm["mask"]=f"{self.gmm}/mask.hdf"

			# Extract particles from refine folder
			if os.path.isfile(f"{rpath}/ptcls_{itr:02d}_even.lst") :
				run(f"e2proclst.py {rpath}/ptcls_{itr:02d}_even.lst {rpath}/ptcls_{itr:02d}_odd.lst --merge {self.gmm}/particles.lst")
			else: run(f"cp {rpath}/ptcls_{itr:02d}.lst {self.gmm}/particles.lst; echo ")
			self.app().setOverrideCursor(Qt.ArrowCursor)
		elif rpath.startswith("spt_"):
			### setup the folder
			try:
				itr=max([int(i.split("_")[1]) for i in os.listdir(rpath) if i[:7]=="threed_" and i.split("_")[1].isdigit()])
			except:
				showerror("No threed_xx files in refine folder")
				return

			self.app().setOverrideCursor(Qt.BusyCursor)
			rparm=js_open_dict(f"{rpath}/0_spa_params.json")
			#if rparm["breaksym"] : self.jsparm["sym"]="c1"
			#else: self.jsparm["sym"]=rparm["sym"]

			# Copy map from refine folder
			a=EMData(f"{rpath}/threed_{itr:02d}.hdf")
			a.write_compressed(f"{self.gmm}/input_map.hdf",0,12)
			self.jsparm["source_map"]=f"{rpath}/threed_{itr:02d}.hdf"
			self.jsparm["boxsize"]=a["nx"]
			self.jsparm["apix"]=a["apix_x"]
			try: self.jsparm["sym"]=rparm["sym"]
			except:
				self.jsparm["sym"]="c1"
				print("symmetry missing, assuming C1")

			# make projection from threed
			run(f"e2project3d.py {rpath}/threed_{itr:02d}.hdf --outfile {self.gmm}/proj_in.hdf --orientgen eman:n=500 --sym c1 --parallel thread:5")

			# Copy mask from refine folder
			a=EMData(f"{rpath}/mask_tight.hdf")
			a.write_compressed(f"{self.gmm}/mask.hdf",0,8)
			self.jsparm["mask"]=f"{self.gmm}/mask.hdf"

			# Copy aligned particles (lst file)
			run(f"cp {rpath}/aliptcls2d_{itr:02d}.lst {self.gmm}/particles.lst; echo ")
			self.app().setOverrideCursor(Qt.ArrowCursor)

		self.wedsym.setText(f'{self.jsparm.getdefault("sym","c1")}')
		self.wedapix.setText(f'{self.jsparm.getdefault("apix",0.0):0.5f}')
		self.wedbox.setText(f'{self.jsparm.getdefault("boxsize",128)}')

		return True
	
	def closeEvent(self,event):
		E2saveappwin("e2gmm","main",self)


if __name__ == "__main__":
	main()
