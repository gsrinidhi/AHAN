from csnake import CodeWriter, Variable,FormattedLiteral
import pandas as pd
import numpy as np

#Take file name as input. File should be in csv format
fname = input("Enter file name")

#Open file
try:
    packet_def_file = open(fname)
except Exception as e:
    print("Cannot open " + str(fname))
    print("Exception: " + str(e))

packet_def_df = pd.read_csv(fname)
print("Headers are:")
print(packet_def_df.head) 
