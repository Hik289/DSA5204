#!/usr/bin/env python
# Muyuan Chen 2016-05
from past.utils import old_div
from builtins import range
from EMAN2 import *

def main():
	
	usage="""Run e2proc3d or e2proc2d in parallel. It split an image stack into multiple sub-stacks, process them in parallel and put the results together. Does not support --first --last yet.. 
	procnd_par.py \"e2proc3d.py a.hdf b.hdf --process blabla...\" --threads=1024"""
	parser = EMArgumentParser(usage=usage,version=EMANVERSION)
	parser.add_argument("--threads", type=int,help="number of threads", default=10)
	parser.add_argument("--ppid", type=int, help="Set the PID of the parent process, used for cross platform PPID",default=-1)	
	parser.add_argument("--nomerge", action="store_true", default=False ,help="do not merge results")


	(options, args) = parser.parse_args()
	logid=E2init(sys.argv)
	
	proc=args[0]
	
	### get the input file
	cmds=proc.split()
	infile=cmds[1]
	outfile=cmds[2]
	cmds[2]="{fname}"
	newcmd= " ".join(cmds)
	num=EMUtil.get_image_count(infile)
	print("Total number of images: {}".format(num))
	
	### prepare the threads
	t={}
	nthd=options.threads
	step=old_div(num,nthd)+1
	pt=outfile.rfind('.')
	tmpfname=["{}_tmp_{:02d}{}".format(outfile[:pt], i, outfile[pt:]) for i in range(nthd)]
	
	### run~
	for td in range(nthd):
		if td*step>=num:
			nthd=td
			break
		cmd=newcmd.format(fname=tmpfname[td])
		cmd+= " --first {} --last {} ".format(td*step, min(num-1,td*step+step-1))
		t[td]=threading.Thread(target=run,args=([cmd]))
		t[td].start()
	for td in range(nthd):
		t[td].join()
	
	if "--average" in proc:
		print("Averaging...")
		avgr=Averagers.get("mean")
		
		for i in range(nthd):
			fm=tmpfname[i]
			n=EMUtil.get_image_count(fm)
			for i in range(n):
				e=EMData(fm,i)
				avgr.add_image(e)
			e=None
			try: os.remove(fm)
			except: 
				print("Cannot remove {}".format(fm))
				pass
			
		a=avgr.finish()
		a.write_image(outfile)
		
	elif options.nomerge==False:
		### put outputs together
		print("Merging outputs...")
		for i in range(nthd):
			fm=tmpfname[i]
			n=EMUtil.get_image_count(fm)
			for i in range(n):
				e=EMData(fm,i)
				e.write_image(outfile,-1)
			e=None
			try: os.remove(fm)
			except: 
				print("Cannot remove {}".format(fm))
				pass
			
	print("Done")
	E2end(logid)
	
def run(cmd):
	print(cmd)
	launch_childprocess(cmd)
	
	
if __name__ == '__main__':
	main()
	
