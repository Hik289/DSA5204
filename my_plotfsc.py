'''Plot FSC txtfile'''

import numpy as np

import matplotlib.pyplot as plt


colors = ['C0','C1']

plt.figure(figsize=(5,4))
labels = ['Deep EMAN2', 'EMAN2']
drgn = np.loadtxt('/Users/manifect/10076/gmm_01/fsc_map_model.txt')
plt.plot(drgn[:,0],drgn[:,1], label=labels[0], color=colors[0])
w = np.where(drgn[:,1]<0.5)
print(w)
print(drgn[:,0][w])
print(1/drgn[:,0][w])

eman = np.loadtxt('/Users/manifect/10076/gmm_00/fsc_map_model.txt')
plt.plot(eman[:,0],eman[:,1], label=labels[1], color=colors[1])
w = np.where(eman[:,1]<0.5)
print(w)
print(eman[:,0][w])
print(1/eman[:,0][w])

f = np.arange(1,6)*.1
r = 1/f*1.6375
print(r)
res_text = ['1/{:.1f}'.format(i) for i in r]

plt.plot([0,.5],[.143,.143],'--',color='grey')
plt.plot([0,.5],[.5,.5],'--',color='grey')

plt.ylim((0,1))
plt.xlim((0,.5))
plt.xticks(f,res_text)
plt.ylabel('FSC')
plt.xlabel('Resolution')
plt.legend()
plt.savefig('/Users/manifect/10076/10076.png')
plt.show()