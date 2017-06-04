import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
#import numpy as np
import seaborn as sns
import pandas as pd
import sys

url = sys.argv[1]

table = pd.read_csv(url)

sns.violinplot(x="duration", y="txsize", data=table)
plt.savefig("validation_throughput_plot.png")
