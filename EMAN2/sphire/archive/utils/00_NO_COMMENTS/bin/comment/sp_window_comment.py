






























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































"""0
		sxprint("\n Creating bdb:%s/data\n"%root_out_dir)
		for proc_i in range(n_mpi_procs):
			mic_start, mic_end = MPI_start_end(len(unsliced_valid_serial_id_list), n_mpi_procs, proc_i)
			for mic_id_substr in unsliced_valid_serial_id_list[mic_start:mic_end]:
				e2bdb_command = "e2bdb.py "
				mic_baseroot = mic_baseroot_pattern.replace("*", mic_id_substr)
				if RUNNING_UNDER_MPI:
					e2bdb_command += "bdb:" + os.path.join(root_out_dir,"%03d/"%proc_i) + mic_baseroot + "_ptcls "
				else:
					e2bdb_command += "bdb:" + os.path.join(root_out_dir, mic_baseroot + "_ptcls ") 
				
				e2bdb_command += " --appendvstack=bdb:%s/data  1>/dev/null"%root_out_dir
		"""






















































