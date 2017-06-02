import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
#import numpy as np
import seaborn as sns
import pandas as pd

url = "/tmp/validation_memory.csv"

df_memory = pd.read_csv(url)

print(df_memory)

plt.figure()
sns.pointplot(x="time", y="free", data=df_memory)
plt.savefig("validation_memory_plot-free.png")

plt.figure()
sns.pointplot(x="time", y="buff", data=df_memory)
plt.savefig("validation_memory_plot-buff.png")

plt.figure()
sns.pointplot(x="time", y="cache", data=df_memory)
plt.savefig("validation_memory_plot-cache.png")
