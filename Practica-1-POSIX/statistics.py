import matplotlib
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from IPython.display import HTML, display_html, display

""""""


def plot_results(image_name):
    # 1 import sys
    # config matplotlib
    matplotlib.rcParams['figure.figsize'] = [20, 10]  # for square canvas
    matplotlib.rcParams['figure.subplot.left'] = 0
    matplotlib.rcParams['figure.subplot.bottom'] = 0
    matplotlib.rcParams['figure.subplot.right'] = 1
    matplotlib.rcParams['figure.subplot.top'] = 1

    # 2 arg = str(sys.argv)
    def auto_label(point, ax):
        # """Attach a text label above each bar in *rects*, displaying its height."""
        # https://matplotlib.org/gallery/lines_bars_and_markers/barchart.html#sphx-glr-gallery-lines-bars-and-markers-barchart-py
        print(type(point))
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
    """
    df_example = pd.DataFrame(np.random.rand(7 * 5, 10))
    df_example.to_csv("example.csv")"""

    size_group = 7
    num_of_group = 5
    decimals = 2
    # 3 cereal_df = pd.read_csv("arg[1]")
    cereal_df = pd.read_csv("{}_time.csv".format(image_name))

    print("Image name: {}".format(image_name))
    num = cereal_df.to_numpy()
    avg = []
    deviation = []
    kernelLabel = ["K=3", "K=5", "k=7", "K=9", "K=11", "k=13", "K=15"]
    threadsLabel = ["th=1", "th=2", "th=4", "th=8", "th=16"]

    # obtain central tendency measures
    for val in num:
        avg.append(np.mean(val[1:]))
        deviation.append((np.std(val[1:])))

    data = []
    speedup = []
    # tabular
    for i in range(num_of_group):
        data.append(np.around(avg[i * size_group:(i * size_group) + size_group], decimals=decimals))
        speedup.append((np.around(np.divide(data[0], data[i]), decimals=decimals)))
    data = np.array(data)
    speedup = np.array(speedup)

    df1 = pd.DataFrame(data, index=threadsLabel,
                       columns=kernelLabel)
    df2 = pd.DataFrame(speedup, index=threadsLabel,
                       columns=kernelLabel)
    # =======================================================================
    #                            Graphs
    # =======================================================================

    x = np.arange(len(kernelLabel))
    x1 = np.arange(len(threadsLabel))
    width = 0.15

    fig1, ax1 = plt.subplots()
    fig2, ax2 = plt.subplots()

    # Graphs labels
    ax1.set_ylabel("Tiempo (s)")
    ax1.set_xlabel("Hilos")
    ax1.set_title("Tiempo de respuesta")
    ax1.set_xticks(x)
    ax1.set_xticklabels(threadsLabel)

    ax2.set_ylabel("Speedup")
    ax2.set_xlabel("Hilos")
    ax2.set_title("Speedup por hilos")
    ax2.set_xticks(x1)
    ax2.set_xticklabels(threadsLabel)

    rects = []
    line = []

    colors = ['#ffc100', '#ff9a00', '#ff7400', '#ff4d00', '#ff0000']
    # bars
    for i in range(size_group):
        # r = ax1.bar(x + ((i - (size_group / 4)) * width), data[i], width, label=threadsLabel[i], color=colors[i])
        ax1.plot(data[:, i], marker='o', label=kernelLabel[i], linewidth=2.0)

    for i in range(size_group):
        ax2.plot(speedup[:, i], marker='o', label=kernelLabel[i], linewidth=2.0)

    ax1.legend()
    ax2.legend()
    fig1.tight_layout()
    fig2.tight_layout()

    display(HTML('<h2>Tiempo de respuesta</h2>'))
    display_html(df1)
    display(HTML('<h2>Speedup</h2>'))
    display_html(df2)
    plt.show()


for image in ('720', '1080', '4k'):
    plot_results(image)
