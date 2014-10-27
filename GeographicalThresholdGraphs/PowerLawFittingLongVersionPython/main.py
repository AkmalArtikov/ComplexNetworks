import analyze
import generate
import plot
import time
import datetime
import igraph

def generate_report(dimension, size, threshold, file_name, start_time, power_law, mode, alpha):
    file = open(file_name + ".txt", "w")

    file.write("Network generated with next parameters: \n")
    file.write("   -Time: {0} seconds \n".format(time.time() - start_time))
    file.write("   -Dimension: {0} \n".format(dimension))
    file.write("   -Vertices: {0} \n \n".format(size))

    file.write("   -Threshold: {0} \n".format(threshold))
    file.write("   -Pareto alpha: {0} \n".format(alpha))
    file.write("   -Pareto mode: {0} \n \n".format(mode))

    file.write("   -Power-law alpha: {} \n \n".format(power_law.alpha))

    #file.write("   -Full degree Distribution: \n")
    #file.write(str(distribution))

    file.close()

def get_file_name(size, dimension, threshold):
    file_name = "{0} vertices_{1} dimension_{2} threshold ".format(size, dimension, threshold)
    file_name = "networks/" + file_name
    ts = time.time()
    st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    st = st.replace(":", "_")
    file_name += st

    return file_name

def run_generating(dimension, size, threshold, alpha, mode, visualise):
    start_time = time.time()

    graph, coords, weights = generate.get_graph(dimension=dimension, size=size,
                                                threshold=threshold, alpha=alpha, mode=mode, start_time=start_time)

    file_name = get_file_name(size=size, dimension=dimension, threshold=threshold)

    if dimension == 3 and visualise:
        plot.drawGraph3D(graph, coords, [0, 0, 0], file_name + ".jpg")
    if dimension == 2 and visualise:
        plot.drawGraph2D(graph, coords, file_name + ".jpg")

    print "   -Time: {0} seconds \n".format(time.time() - start_time)

    degree_distribution1 = analyze.get_degree_distribution(graph)

    print "   -Time: {0} seconds \n".format(time.time() - start_time)

    degree_distribution = analyze.get_cut_degree_distribution(graph=graph, threshold=threshold,
                                                             mode=mode, size=size, weights=weights)

    print "   -Time: {0} seconds \n".format(time.time() - start_time)
    #print degree_distribution

    power_law = igraph.power_law_fit(degree_distribution)

    generate_report(dimension=dimension, size=size, threshold=threshold, file_name=file_name, start_time=start_time
                    , power_law=power_law, mode=mode, alpha=alpha)

for i in range(0, 1):
    run_generating(dimension=3, size=10000, threshold=2.0, alpha=3.0, mode=0.0, visualise=False)
#    print str(i) + " 3, 1000, 2.0, 4.0"

# for i in range(0, 10):
#     run_generating(dimension=3, size=1000, threshold=3.0, alpha=2.0, mode=1.0, visualise=False)
#     print str(i) + " 3, 1000, 3.0, 2.0"
#
# for i in range(0, 10):
#     run_generating(dimension=3, size=1000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
#     print str(i) + " 3, 1000, 2.0, 3.0"
#
# for i in range(0, 5):
#     run_generating(dimension=3, size=2000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
#     print str(i) + " 3, 2000, 2.0, 3.0"

# for i in range(0, 3):
#     run_generating(dimension=3, size=5000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
#     print str(i) + " 3, 5000, 2.0, 3.0"
#
# run_generating(dimension=3, size=10000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
# print str(i) + " 3, 10000, 2.0, 3.0"
#
# for i in range(0, 10):
#     run_generating(dimension=2, size=1000, threshold=2.0, alpha=4.0, mode=1.0, visualise=False)
#     print str(i) + " 2, 1000, 2.0, 4.0"
#
# for i in range(0, 10):
#     run_generating(dimension=2, size=1000, threshold=3.0, alpha=2.0, mode=1.0, visualise=False)
#     print str(i) + " 2, 1000, 3.0, 2.0"
#
# for i in range(0, 10):
#     run_generating(dimension=2, size=1000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
#     print str(i) + " 2, 1000, 2.0, 3.0"
#
# for i in range(0, 5):
#     run_generating(dimension=2, size=2000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
#     print str(i) + " 2, 2000, 2.0, 3.0"
#
# for i in range(0, 3):
#     run_generating(dimension=2, size=5000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
#     print str(i) + " 2, 5000, 2.0, 3.0"
#
# run_generating(dimension=2, size=10000, threshold=2.0, alpha=3.0, mode=1.0, visualise=False)
# print str(i) + " 2, 10000, 2.0, 3.0"

#TODO 2D plotting with cairo
#TODO clasterization_coef into report
#TODO large graphs?
#TODO check ideas with different weight distribution