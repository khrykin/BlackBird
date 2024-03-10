"""
Applying the antiderivative antialiasing idea to tanh(x) function.

Stefan Bilbao, Fabián Esqueda, Julian D. Parker, Vesa Välimäki.
Antiderivative antialiasing for memoryless nonlinearities.
IEEE Signal Processing Letters, Vol. 24, No. 7, pp. 1049-1053, July 2017.
DOI: 10.1109/LSP.2017.2675541
"""

import numpy as np
import mpmath


def order1(X):
    assert X.shape[0] >= 2

    def D(z0, z1):
        return (_F1(z0) - _F1(z1)) / (z0 - z1)

    # X[k, :] contains a signal delayed by k taps
    return D(X[0,:], X[1,:])


def order2(X):
    assert X.shape[0] >= 3

    def D(z0, z1):
        return (_F2(z0) - _F2(z1)) / (z0 - z1)

    # X[k, :] contains a signal delayed by k taps
    A = 2 / (X[0,:] - X[2,:])
    D_01 = D(X[0,:], X[1,:])
    D_12 = D(X[1,:], X[2,:])

    return A * (D_01 - D_12)


def order3(X):
    assert X.shape[0] >= 4

    def D(z0, z1):
        return (_F3(z0) - _F3(z1)) / (z0 - z1)

    # X[k, :] contains a signal delayed by k taps
    A = 1 / (X[1,:] - X[2,:])
    B = 2 / (X[0,:] - X[2,:])
    C = 2 / (X[1,:] - X[3,:])

    D_01 = D(X[0,:], X[1,:])
    D_12 = D(X[1,:], X[2,:])
    D_23 = D(X[2,:], X[3,:])

    return A * (B * (D_01 - D_12) - C * (D_12 - D_23))

def _Li_n(x, n):
    np_polylog = np.frompyfunc(lambda z: mpmath.polylog(n, z), 1, 1)
    return np_polylog(x)


def _F1(x):
    return np.log(np.cosh(x))


def _F2(x):
    """ Calculated with Wolfram Alpha """
    exp = np.exp(-2*x)
    C1 = 0  # This won't matter as C1 coeffs cancel each other in 'order2' equation
    return C1*x + 0.5*_Li_n(-exp, 2) - 0.5*x**2 - x*np.log(1 + exp) + x*np.log(np.cosh(x))


def _F3(x):
    """ Calculated with Wolfram Alpha """
    exp = np.exp(-2*x)
    C1 = 0
    C2 = 0
    return (1/6)*x*(3*C1*x + 6*C2 - 2*x**2 - 3*x*np.log(exp+1) + 3*x*np.log(np.cosh(x))) - (1/4)*_Li_n(-exp, 3)
