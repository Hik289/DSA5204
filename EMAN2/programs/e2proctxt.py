#!/usr/bin/env python
# This program performs simple processing of .LST files

# Author: Steven Ludtke, 5/19/2016 (sludtke@bcm.edu)
# Copyright (c) 2014- Baylor College of Medicine
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

from builtins import range
from EMAN2 import *
from math import *
import numpy as np
import sklearn.decomposition as skdc
import sklearn.manifold as skmf
import os
import sys
import re
from time import time

def readfile(filename,verbose=0):
	"""reads a multicolumn numerical file, including optional header comment line
	returns data[row][col],label[col]"""

	# : specification, n0=first line #, n1=last line #+1, ns=step
	# :::2 = all even lines
	# :1::2 = all odd lines
	# :1000 = first 1000 lines
	# :0:1000:2 = even lines 0-998
	if ":" in filename:
		rng=filename.split(":")
		filename=rng[0]
		if len(rng)==2: n0,n1,ns=0,int(rng[1]),1
		if len(rng)==3: n0,n1,ns=int(rng[1]),int(rng[2]),1
		if len(rng)==4: 
			try: n0=int(rng[1])
			except: n0=0
			try: n1=int(rng[2])
			except: n1=-1
			try: ns=int(rng[3])
			except: ns=1
		print(f"Covering range {n0}:{n1} with step {ns}")
	else: n0,n1,ns=0,-1,1
			
	# loadtxt is really slow
	fin=open(filename,"r")
	nr=0
	nt=0
	lbls=[]
	for i,lin in enumerate(fin):
		l=lin.strip()
		if lin[0]!="#" and len(l)!=0:
			ll=l
			nt+=1
			if (n1<0 or nt<n1) and nt>=n0 and (nt-n0)%ns==0: nr+=1
		else:
			# look for a comment line with ; separator which may contain column labels
			if lin[0]=="#":
				lbls2=lin[1:].strip().split(";")
				lbls2=[lbl for lbl in lbls2 if len(lbl)>0]
				if len(lbls2)>len(lbls):
					lbls=lbls2
					lbln=i

	nc=len(re.split("[\s,;]+",ll))		# last non-comment
	if verbose>0 :
		print(f"{filename} : ({nc},{nr})")
		if len(lbls)>1 : print("  ".join(lbls))
	data=np.zeros((nr,nc))

	fin.seek(0)
	# second pass, read the data, seems dumb, but actually faster
	r=0	# included lines
	nl=0	# total lines of data
	for lin in fin:
		l=lin.strip()
		if lin[0]!="#" and len(l)!=0:
			if (n1<0 or nl<n1) and nl>=n0 and (nl-n0)%ns==0:
				v=[float(x) for x in re.split("[\s,;]+",l)]
				data[r]=v
				r+=1
			nl+=1

	if r!=nr : print(f"ERROR: inconsistent read {r} lines read with {nr} rows expected")
	return data,lbls

def writefile(filename,data,lbls,fmt="1.4f"):
	"""writes a multicolumn numerical file
	filename
	data[row][col]
	label[col] or [] or None
	fmt format string, def "1.4f" """

	# Overwrite input!
	if os.path.exists(filename):
		try: os.unlink(filename+".bak")
		except: pass
		os.rename(filename,filename+".bak")

	fmt="%"+fmt
	out=open(filename,"w")
	if lbls is not None and len(lbls)>0:
		out.write("# "+";".join(lbls))
		out.write("\n")
	for r,d in enumerate(data):
		line=[fmt%v for v in d]
		out.write("\t".join(line))
		out.write("\n")
	out.close()


def main():
	progname = os.path.basename(sys.argv[0])
	usage = """Usage:\nproctxt.py [options] <txt 1> <txt 2> ... 
Manipulations of text files conatining multi-column data (as would be used with plotting programs, like e2display --plot).

--merge  combines several files, all with N rows, into a single file with N rows but additional columns

--dimreduce  a variety of dimensional reduction algorithms are available. This will add additional dimensionally reduced columns to an existing file
	Note that tsne is the only dimensional reduction algorithm suitable for large numbers (>~50k) of rows.
	
--hist2d <bins>

--hist3d <bins>
	""" 

	parser = EMArgumentParser(usage=usage,version=EMANVERSION)
	####################
	parser.add_argument("--copy",type=str,help="Copy input file to output specified here. Will follow ':' convention to limit input lines. ",default=None)
	parser.add_argument("--sortcomment",action="store_true",default=False,help="Sorts rows based on per-row comment (after #) before merging")
	parser.add_argument("--merge",type=str,help="Merge several files into a single output by appending columns. All inputs must have the same number of rows. Row comments stripped.",default=None)
	parser.add_argument("--dimreduce",type=str,help="tsne, mds, isomap, lle, spectral. output=input with added columns. Multiple files are independent.",default=None)
	parser.add_argument("--hist2d",type=int,help="[bins]. Generate a 2d histogram as an image of any 2 specified columns. output=input.hdf",default=0)
	parser.add_argument("--hist3d",type=int,help="[bins]. Generate a 3d histogram as a 3D volume of any 3 specified columns. output=input.hdf",default=0)
	parser.add_argument("--dimout",type=int,help="number of output dimensions for dimreduce. default=2",default=2)
	parser.add_argument("--columns",type=str,help="which columns to use for the analysis (eg, 2-4). First column is 0. End is inclusive. default = all columns",default=None)
	parser.add_argument("--normalize",action="store_true",default=False,help="Applies normal EMAN normalization to specified columns (mean->0, std->1)")
	parser.add_argument("--precout",type=str,help="specify precision and format for writing output, '1.4f' - 4 digits of precision, '1.4g' 4 digits sci notation. default=1.4f",default="1.4f")
	parser.add_argument("--verbose", "-v", dest="verbose", action="store", metavar="n", type=int, help="verbose level [0-9], higher number means higher level of verboseness",default=1)
	parser.add_argument("--ppid", type=int, help="Set the PID of the parent process, used for cross platform PPID",default=-1)


	(options, args) = parser.parse_args()
	
	if len(args)<1 : 
		parser.error("At least one lst file required")
		sys.exit(1)

	logid=E2init(sys.argv,options.ppid)

	if options.copy is not None:
		if len(args)>1 : print("Copy uses first specified input only!")
		data,lbls=readfile(args[0],options.verbose)
		writefile(options.copy,data,lbls,options.precout)
		print("Copied ",args[0]," to ",options.copy)
		sys.exit(0)

	if options.dimreduce is not None:
		for filename in args:
			data,lbls=readfile(filename,options.verbose)

			if options.columns is not None :
				cols=parse_range(options.columns,data.shape[1]-1)
				if options.verbose>0: print("using columns: ",cols)
				v2a=data[:,cols]
			else: v2a=data

			# requested dimensional reduction, done using scikit.learn
			if options.dimreduce.lower()=="tsne":
				if options.verbose>0: print("Begin TSNE for",filename)
				stime=time()
				tsne=skmf.TSNE(n_components=options.dimout,init="pca",learning_rate="auto",n_jobs=-1,verbose=options.verbose)
				vdc=tsne.fit_transform(v2a)
				vdc/=100.0
			elif options.dimreduce.lower()=="mds":
				if options.verbose>0:print("Begin MDS for",filename)
				stime=time()
				mds=skmf.MDS(n_components=options.dimout,verbose=options.verbose)
				vdc=mds.fit_transform(v2a)
			elif options.dimreduce.lower()=="isomap":
				if options.verbose>0:print("Begin Isomap for",filename)
				stime=time()
				isomap=skmf.Isomap(n_components=options.dimout)
				vdc=isomap.fit_transform(v2a)
			elif options.dimreduce.lower()=="lle":
				if options.verbose>0:print("Begin LLE for",filename)
				stime=time()
				lle=skmf.LocallyLinearEmbedding(n_components=options.dimout,n_jobs=-1)
				vdc=lle.fit_transform(v2a)
			elif options.dimreduce.lower()=="spectral":
				if options.verbose>0:print("Begin Spectral Embedding for",filename)
				stime=time()
				sem=skmf.SpectralEmbedding(n_components=options.dimout,n_jobs=-1)
				vdc=sem.fit_transform(v2a)
			else:
				error_exit("Unknown dimensionality reduction algorithm")

			if options.verbose>0: print(f"Complete in {time()-stime:1.1f}s")

			# Overwrite input!
			out=open(filename,"w")
			if len(lbls)>0:
				out.write("# "+";".join(lbls))
				out.write((";"+options.dimreduce)*options.dimout)
				out.write("\n")
			for r,d in enumerate(data):
				for v in d: out.write(f"{v:1.4f}\t")
				for v in vdc[r]: out.write(f"{v:1.4f}\t")
				out.write("\n")
			out.close()
			print("Additional columns added to ",filename)
			sys.exit(0)
	if options.normalize :

		for filename in args:
			data,lbls=readfile(filename,options.verbose)

			data2=data.transpose()
			if options.columns is not None :
				cols=parse_range(options.columns,data.shape[1]-1)
				if options.verbose>0: print("using columns: ",cols)
				for c in cols:
					data2[c]-=np.mean(data2[c])
					data2[c]/=np.std(data2[c])
			else:
				for c in range(data2.shape[0]):
					data2[c]-=np.mean(data2[c])
					data2[c]/=np.std(data2[c])
			data=data2.transpose()
			writefile(filename,data,lbls,options.precout)
		print("file(s) normalized")
		sys.exit(0)

	if options.hist2d>1:
		for filename in args:
			data,lbls=readfile(filename,options.verbose)

			data2=data.transpose()
			cols=parse_range(options.columns,data.shape[1]-1)
			if options.columns is not None and len(cols)==2 :
				cols=parse_range(options.columns,data.shape[1]-1)
				if options.verbose>0: print("using columns: ",cols)
				ctrmap,xe,ye=np.histogram2d(data2[cols[0]],data2[cols[1]],bins=options.hist2d)
				outname=f'{filename.rsplit(".",1)[0]}_{cols[0]}_{cols[1]}.hdf'
				from_numpy(ctrmap).write_image(outname)
				print("Wrote: ",outname)
			else:
				print("Error: please specify 2 columns")
				sys.exit(1)
		sys.exit(0)

	elif options.hist3d>1:
		for filename in args:
			data,lbls=readfile(filename,options.verbose)

			data2=data.transpose()
			cols=parse_range(options.columns,data.shape[1]-1)
			if options.columns is not None and len(cols)==3:
				cols=parse_range(options.columns,data.shape[1]-1)
				if options.verbose>0: print("using columns: ",cols)
				data2=data2[np.array((cols[0],cols[1],cols[2]))]
				print(data2.shape)
				ctrmap,edges=np.histogramdd(data2.transpose(),bins=options.hist3d)
				outname=f'{filename.rsplit(".",1)[0]}_{cols[0]}_{cols[1]}_{cols[2]}.hdf'
				from_numpy(ctrmap).write_image(outname)
				print("Wrote: ",outname)
			else:
				print("Error: please specify 3 columns")
				sys.exit(1)
		sys.exit(0)


	if options.merge!=None:
		# read all files. data_sets is a list of files. each file is a list of rows. each row is a list of comma, semicolon or space/tab separated values
		data_sets=[]
		for filename in args:
			fin=open(filename,"r")
			data=[]
			for line in fin:
				if "#" in line : 
					comment=line.split("#")[1].strip()
					line=line.split("#")[0].strip()
				if len(line)==0 : continue	# pure comment line
				if "," in line : line=line.split(",")
				elif ";" in line : line=line.split(";")
				else : line=line.split()
				if options.sortcomment : line.insert(0,comment)
				data.append(line)
			
			if options.sortcomment:
				data.sort()
				data=[i[1:] for i in data]
				
			data_sets.append(data)
			
		# merge all of the columns into data_sets[0]
		for i in range(len(args)-1):
			if len(data_sets[i])!=len(data_sets[i+1]) :
				print("Error: {} has {} rows and {} has {}".format(args[i],len(data_sets[i]),args[i+1],len(data_sets[i])))
				sys.exit(1)
			
			for row in range(len(data_sets[i+1])): 
				data_sets[0][row].extend(data_sets[i+1][row])

		out=open(options.merge,"w")
		for row in data_sets[0]:
			out.write("\t".join(row))
			out.write("\n")

		print("merged data written to ",options.merge)
		sys.exit(0)

	
			

	E2end(logid)

if __name__ == "__main__":
	main()
