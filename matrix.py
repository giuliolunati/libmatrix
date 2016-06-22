# vim: set syn=python fdm=indent sw=2 autoindent indk=:
from ctypes import *
import array
A=array.array('f',(9,8,7,6))
B=array.array('f',(1,2,3,4))
C=array.array('f',(0,0,0,0))
_cmatrix=CDLL('/sdcard/B/Prog/cmatrix.so')
matrix_mul= _cmatrix.matrix_mul
matrix_mul.argtypes=(c_void_p, c_void_p, c_void_p, c_int, c_int, c_int)
matrix_mul(C.buffer_info()[0],A.buffer_info()[0],B.buffer_info()[0],2,2,2)
print C
