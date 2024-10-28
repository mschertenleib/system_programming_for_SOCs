import numpy as np

def custom_range(mbits:int=23,ebits:int=8,offset:int=0):
    maxvalue = (1 + (2**(mbits+1)-1)/(2**(mbits+1)))*(2**((2**ebits -1)-offset))
    minvalue = (1 + (1/(2**(mbits+1))))*(2**(-offset))
    print("maxvalue: ",maxvalue," minvalue: ",minvalue)

custom_range(23,8,127)
custom_range(28,3,3)
