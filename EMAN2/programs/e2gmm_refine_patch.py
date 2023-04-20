#!/usr/bin/env python
# Muyuan Chen 2023-02
from EMAN2 import *
import numpy as np
from sklearn.cluster import KMeans

	
def main():
	
	usage="""
	Patch-by-patch refinement using GMM as references. Require GPU and CUDA. For a simple run, use:
	e2gmm_refine_patch.py gmm_00/threed_05.hdf --startres 3.5 
	
	The input needs to be from an existing gmm_xx folder and the program will take information from corresponding files in that folder. The program will segment the GMM from the previous refinement into N patches (default 8), focus refine each one, then combine results together. The first digit in iteration number corresponds to the index of patches. e.g., gmm_xx/threed_23.hdf is the refinement result of the 3rd iteration using the 2nd focus mask. The final result is labeled iteration 99 in the gmm_xx folder.
	
	Note the segmentation of GMM does not support symmetry. So for symmetrical structures, always specify the symmetry in --expandsym and the output will be in c1.
	"""
	
	parser = EMArgumentParser(usage=usage,version=EMANVERSION)
	parser.add_argument("--path", type=str,help="path for refinement. default is the next gmm_xx", default=None)
	parser.add_argument("--niter", type=int,help="number of iteration. default is 3", default=3)
	parser.add_argument("--startres", type=float,help="starting resolution", default=3)
	parser.add_argument("--npatch", type=int,help="number of patches. default is 8", default=8)
	parser.add_argument("--maxres", type=float,help="max resolution to consider in refinement. i.e. the alignment will not use information beyond this even if FSC goes further.", default=-1)
	parser.add_argument("--expandsym", type=str,help="symmetry expansion. i.e. start from an input refinement with the given symmetry and perform the new refinement with c1 by making copies of each particle at symmetrical equivalent positions. ", default=None)
	parser.add_argument("--masktight", action="store_true", default=False ,help="Use tight patch mask instead of spherical ones. seems safe.")
	parser.add_argument("--batchsize", type=int,help="Number of particles in each batch for alignment. Increase will make the alignment faster, but also increases GPU memory use. ", default=-1)
	parser.add_argument("--chunksize", type=int,help="Number of particles in each e2gmm_batch process. Increase will make the alignment slightly faster, but also increases CPU memory use. ", default=-1)


	(options, args) = parser.parse_args()
	
	logid=E2init(sys.argv)
	
	fname=args[0]
	oldpath=os.path.dirname(fname)
	
	c=[i for i in fname if i.isdigit()]
	olditer=int(''.join(c[-2:]))
	print(f"refinement path {oldpath}, iteration {olditer}.")
	p00=np.loadtxt(f"{oldpath}/model_{olditer-1:02d}_even.txt")
	
	
	if options.path==None: 
		path=options.path=num_path_new("gmm_")
	
	p=p00
	pn=options.npatch
	km=KMeans(pn,max_iter=30)
	km.fit(p[:,:3])
	pc=km.cluster_centers_
	lb=km.labels_
	path=options.path
	
	msk0=EMData(f"{oldpath}/mask.hdf")
	msk0.write_image(f"{path}/mask_00.hdf")
	msk=msk0.copy()
	
	etc=""
	if options.maxres>0:
		etc+=f" --maxres {options.maxres}"
	if options.batchsize>0:
		etc+=f" --batchsize {options.batchsize}"
	if options.chunksize>0:
		etc+=f" --chunksize {options.chunksize}"
		
		
	if options.masktight:
		for ci in range(pn):
			ps=p[lb==ci,:].copy()
			print(ps.shape)
			np.savetxt(f"{path}/model_tmp.txt", ps)
			run(f"e2gmm_refine_new.py --ptclsin {oldpath}/projections_even.hdf  --model {path}/model_tmp.txt --maxres -1 --modelout {path}/model_tmp.txt --niter 0 --trainmodel --evalmodel {path}/model_projs.hdf")
			run(f"e2spa_make3d.py --input {path}/model_projs.hdf --output {path}/model_avg.hdf --thread 32")
			e=EMData(f"{path}/model_avg.hdf")
			e.process_inplace("filter.lowpass.gauss",{"cutoff_abs":.25})
			e.process_inplace("normalize.edgemean")
			e.process_inplace("mask.auto3d.thresh",{"nshells":3,"nshellsgauss":5,"threshold1":3,"threshold2":1,"return_mask":True})
			e.write_image(f"{path}/mask_patch_{ci:02d}.hdf")
		
	else:
		for ci in range(pn):
			c=pc[ci].copy()
			r=np.linalg.norm(p[lb==ci,:3]-c, axis=1)
			r=np.max(r)
			
			msk.to_one()
			c=(c*msk["nx"]).tolist()
			msk.process_inplace("mask.soft",{"dx":c[0], "dy":-c[1], "dz":-c[2], "outer_radius":r*msk["nx"],"width":5})
			msk.write_image(f"{path}/mask_patch_{ci:02d}.hdf")
		
	for ci in range(pn):
		for eo in ["even", "odd"]:
			run(f"e2proc3d.py {oldpath}/threed_{olditer:02d}_{eo}.hdf {path}/threed_{ci*10:02d}_{eo}.hdf")
			run(f"e2proclst.py {oldpath}/ptcls_{olditer:02d}_{eo}.lst --create {path}/ptcls_{ci*10:02d}_{eo}.lst")
			if options.expandsym:
				run(f"e2proclst.py {path}/ptcls_{ci*10:02d}_{eo}.lst --sym {options.expandsym}")
			p=np.loadtxt(f"{oldpath}/model_{olditer-1:02d}_{eo}.txt")
			np.savetxt(f"{path}/model_{ci*10:02d}_{eo}.txt", p)
			
		run(f"e2gmm_refine_iter.py {oldpath}/threed_{olditer:02d}.hdf --startres {options.startres} --initpts {oldpath}/model_{olditer-1:02d}.txt --mask {path}/mask_patch_{ci:02d}.hdf --masksigma --path {path} --niter {options.niter} --maskpp {path}/mask_00.hdf --startiter {ci*10+1} {etc}")
		
		for eo in ["even", "odd"]:
			run(f"e2proc3d.py {path}/threed_raw_{eo}.hdf {path}/threed_patch_{ci:02d}_raw_{eo}.hdf")
	
	run(f"e2gmm_merge_patch.py {path}")
	E2end(logid)
	
	
if __name__ == '__main__':
	main()
	
		
