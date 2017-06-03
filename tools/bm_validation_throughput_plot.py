import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
#import numpy as np
import seaborn as sns
import pandas as pd

url = "/tmp/validation_throughput.csv"

table = pd.read_csv(url)
#print(table)

sns.violinplot(x="duration", y="txsize", data=table)
plt.savefig("validation_throughput_plot.png")
