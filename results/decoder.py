import numpy as np

def gen_pop(X, PSIZE): # Gera população inicial
    n, d = X.shape
    p = []
    for _ in range(PSIZE):
        A = []
        k = int(np.random.uniform(2, n/2))
        A.append(k)
        for __ in range(d):
            A.append(np.random.binomial(n=1, p=0.5))
        p.append(A)

    return p

def encoder(pop, kmax):
    new_p = []
    for ind in pop:
        new_ind = []
        k = ind[0]
        k = (k-2)/max(1, kmax - 3) # kmax - 2 - 1
        new_ind.append(k)
        for i in range(1, len(ind)):
            if ind[i] == 0:
                rand = np.random.uniform(0, 0.49999)
            else:
                rand = np.random.uniform(0.5, 1)
            new_ind.append(rand)
        new_p.append(new_ind)
            
    return new_p

def decode(X, chromosome):
    flags = [1 if gene >= 0.5 else 0 for gene in chromosome[1:]]
    X_sel = X[:, [i for i,f in enumerate(flags) if f == 1]]
    unique_samples = len(np.unique(X_sel, axis=0)) - 1
    k_real = chromosome[0]
    kmax = int(unique_samples/2)
    k = int(2 + (kmax - 2 + 1) * k_real)

    return k, flags