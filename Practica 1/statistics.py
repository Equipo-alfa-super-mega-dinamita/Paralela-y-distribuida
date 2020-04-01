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
# =======================================================================
#                               DATA
# ========================================================================
cereal_df = pd.read_csv("example.csv")
num = cereal_df.to_numpy()
median = []
deviation = []
resolutionLabel = ["720p", "1080p", "4K"]
threadsLabel = ["th=1", "th=2", "th=4", "th=8", "th=16"]

for val in num:
    median.append(np.median(val))
    deviation.append((np.std(val)))
decimals = 1
dataSec = np.around(median[0:3], decimals=decimals)
dataTh2 = np.around(median[3:6], decimals=decimals)
dataTh4 = np.around(median[6:9], decimals=decimals)
dataTh8 = np.around(median[9:12], decimals=decimals)
dataTh16 = np.around(median[12:15], decimals=decimals)

speedupSec = np.around(np.divide(dataSec, dataSec), decimals=decimals)
speedupTh2 = np.around(np.divide(dataSec, dataTh2), decimals=decimals)
speedupTh4 = np.around(np.divide(dataSec, dataTh4), decimals=decimals)
speedupTh8 = np.around(np.divide(dataSec, dataTh8), decimals=decimals)
speedupTh16 = np.around(np.divide(dataSec, dataTh16), decimals=decimals)

df1 = pd.DataFrame(np.array([dataSec, dataTh2, dataTh4, dataTh8, dataTh16]), index=threadsLabel,
                   columns=resolutionLabel)
df2 = pd.DataFrame(np.array([speedupSec, speedupTh2, speedupTh4, speedupTh8, speedupTh16]), index=threadsLabel,
                   columns=resolutionLabel)
speedupData = df2.to_numpy()

# =======================================================================
#                            Graphs
# =======================================================================


x = np.arange(len(resolutionLabel))
x1 = np.arange(len(threadsLabel))
width = 0.1

fig1, ax1 = plt.subplots()
fig2, ax2 = plt.subplots()
fig3, ax3 = plt.subplots()

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

ax3.set_ylabel("Speedup")
ax3.set_xlabel("Hilos")
ax3.set_title("Speedup por hilos")
ax3.set_xticks(x1)
ax3.set_xticklabels(threadsLabel)

t1 = ax1.bar(x - 2 * width, dataSec, width, label=threadsLabel[0], color='purple')
t2 = ax1.bar(x - width, dataTh2, width, label=threadsLabel[1], color='orange')
t3 = ax1.bar(x, dataTh4, width, label=threadsLabel[2], color='green')
t4 = ax1.bar(x + width, dataTh8, width, label=threadsLabel[3], color='red')
t5 = ax1.bar(x + 2 * width, dataTh16, width, label=threadsLabel[4], color='c')

t6 = ax2.bar(x - width, speedupTh2, width, label=threadsLabel[1], color='orange')
t7 = ax2.bar(x, speedupTh4, width, label=threadsLabel[2], color='green')
t8 = ax2.bar(x + width, speedupTh8, width, label=threadsLabel[3], color='red')
t9 = ax2.bar(x + 2 * width, speedupTh16, width, label=threadsLabel[4], color='c')

t10 = ax3.plot(speedupData[:, 0], label=resolutionLabel[0])
t11 = ax3.plot(speedupData[:, 1], label=resolutionLabel[1])
t12 = ax3.plot(speedupData[:, 2], label=resolutionLabel[2])

ax1.legend()
ax2.legend()
ax3.legend()

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

# display(df1)
# display(df2)
plt.show()
