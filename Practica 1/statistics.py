import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


def auto_label(rects, ax):
    # """Attach a text label above each bar in *rects*, displaying its height."""
    # https://matplotlib.org/gallery/lines_bars_and_markers/barchart.html#sphx-glr-gallery-lines-bars-and-markers-barchart-py
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


#
# sec, 2, 4, 8, 16
dataSec = np.array([2, 3, 5])
dataTh2 = np.array([1, 2, 4])
dataTh4 = np.array([0.8, 1, 3])
dataTh8 = np.array([0.5, 0.2, 2])
dataTh16 = np.array([0.2, 0.1, 1])
# =======================================================================
# Todo enter data, cal median and mod dataSec dataTh#
#
#
cereal_df = pd.read_csv("example.csv")
print(cereal_df)
num = cereal_df.to_numpy()
median = []
for val in num:
    median.append(np.median(val))
dataSec = median[0:3]
dataTh2 = median[3:6]
dataTh4 = median[6:9]
dataTh8 = median[9:12]
dataTh16 = median[12:15] 
# =======================================================================
decimals = 1
speedupTh2 = np.around(np.divide(dataSec, dataTh2), decimals=decimals)
speedupTh4 = np.around(np.divide(dataSec, dataTh4), decimals=decimals)
speedupTh8 = np.around(np.divide(dataSec, dataTh8), decimals=decimals)
speedupTh16 = np.around(np.divide(dataSec, dataTh16), decimals=decimals)

resolutionLabel = ["720p", "1080p", "4K"]

x = np.arange(len(resolutionLabel))
width = 0.1

fig1, ax1 = plt.subplots()
fig2, ax2 = plt.subplots()

ax1.set_ylabel("Tiempo (s)")
ax1.set_xlabel("Resolución")
ax1.set_title("Tiempo de respuesta")
ax1.set_xticks(x)
ax1.set_xticklabels(resolutionLabel)

ax2.set_ylabel("Speedup")
ax2.set_xlabel("Resolución")
ax2.set_title("Speedup")
ax2.set_xticks(x)
ax2.set_xticklabels(resolutionLabel)

t1 = ax1.bar(x - 2 * width, dataSec, width, label="sec", color='purple')
t2 = ax1.bar(x - width, dataTh2, width, label="th = 2", color='orange')
t3 = ax1.bar(x, dataTh4, width, label="th = 4", color='green')
t4 = ax1.bar(x + width, dataTh8, width, label="th = 8", color='red')
t5 = ax1.bar(x + 2 * width, dataTh16, width, label="th = 16", color='c')
t6 = ax2.bar(x - width, speedupTh2, width, label="th = 2", color='orange')
t7 = ax2.bar(x, speedupTh4, width, label="th = 4", color='green')
t8 = ax2.bar(x + width, speedupTh8, width, label="th = 8", color='red')
t9 = ax2.bar(x + 2 * width, speedupTh16, width, label="th = 16", color='c')

ax1.legend()
ax2.legend()
fig1.tight_layout()
fig2.tight_layout()
auto_label(t1, ax1)
auto_label(t2, ax1)
auto_label(t3, ax1)
auto_label(t4, ax1)
auto_label(t5, ax1)
auto_label(t6, ax2)
auto_label(t7, ax2)
auto_label(t8, ax2)
auto_label(t9, ax2)
plt.show()
