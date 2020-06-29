import ROOT as rt

def get_poission_error(N):
    alpha = 1 - 0.6827
    L = 0.0
    if N > 0: L = rt.Math.gamma_quantile(alpha/2,N,1.)
    U =  rt.Math.gamma_quantile_c(alpha/2,N+1,1)
    return N - L, U - N

for i in range(5):
    U,L = get_poission_error(i)
    print "error down, up for %d are %.4f, %.4f" % (i, U, L)
