scp pagerank 192.168.0.33:~/parallelization_lab4/
scp data_input 192.168.0.33:~/parallelization_lab4/
scp pagerank 192.168.0.36:~/parallelization_lab4/
scp data_input 192.168.0.36:~/parallelization_lab4/
scp pagerank 192.168.0.92:~/parallelization_lab4/
scp data_input 192.168.0.92:~/parallelization_lab4/
mpirun -np 16 -f hosts ./pagerank
