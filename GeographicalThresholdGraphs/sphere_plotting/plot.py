import cairo
import codecs
import dateutil.parser
import igraph
import numpy
import re
import random
import math
import time
import datetime

def project2D(layout, alpha, beta):
    '''
    This method will project a set of points in 3D to 2D based on the given
    angles alpha and beta.
    '''

    # Calculate the rotation matrices based on the given angles.
    c = numpy.matrix([[1, 0, 0], [0, numpy.cos(alpha), numpy.sin(alpha)], [0, -numpy.sin(alpha), numpy.cos(alpha)]])
    c = c * numpy.matrix([[numpy.cos(beta), 0, -numpy.sin(beta)], [0, 1, 0], [numpy.sin(beta), 0, numpy.cos(beta)]])
    b = numpy.matrix([[1, 0, 0], [0, 1, 0], [0, 0, 1]])

    # Hit the layout, rotate, and kill a dimension
    layout = numpy.matrix(layout)
    X = (b * (c * layout.transpose())).transpose()
    return [[X[i,0],X[i,1],X[i,2]] for i in range(X.shape[0])]

def drawGraph2D(graph, coords, fileName):
    layout = graph.layout("drl")

    for i in range(0, size):
        layout[i] = coords[i]

    plot = igraph.plot(graph, bbox=(0, 0, 1280, 1280), layout=layout)
    plot.save(fileName)

def drawGraph3D(graph, coords, angle, fileName):
    '''
    Draw a graph in 3D with the given layout, angle, and filename.
    '''

    # Setup some vertex attributes and calculate the projection
    layout = graph.layout("sphere")

    for i in range(0, size):
        layout[i] = coords[i]

    graph.vs['degree'] = graph.degree()
    vertexRadius = 0.1 * (0.9 * 0.9) / numpy.sqrt(graph.vcount())
    graph.vs['x3'], graph.vs['y3'], graph.vs['z3'] = zip(*layout)

    layout2D = project2D(layout, angle[0], angle[1])
    graph.vs['x2'], graph.vs['y2'], graph.vs['z2'] = zip(*layout2D)
    minX, maxX = min(graph.vs['x2']), max(graph.vs['x2'])
    minY, maxY = min(graph.vs['y2']), max(graph.vs['y2'])
    minZ, maxZ = min(graph.vs['z2']), max(graph.vs['z2'])

    # Calculate the draw order.  This is important if we want this to look
    # realistically 3D.
    zVal, zOrder = zip(*sorted(zip(graph.vs['z3'], range(graph.vcount()))))

    # Setup the cairo surface
    surf = cairo.ImageSurface(cairo.FORMAT_ARGB32, 1280, 1280)
    con = cairo.Context(surf)
    con.scale(1280.0, 1280.0)

    # Draw the background
    con.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    con.rectangle(0.0, 0.0, 1.0, 1.0)
    con.fill()

    # Draw the edges without respect to z-order but set their alpha along
    # a linear gradient to represent depth.
    for e in graph.get_edgelist():
        # Get the first vertex info
        v0 = graph.vs[e[0]]
        x0 = (v0['x2'] - minX) / (maxX - minX)
        y0 = (v0['y2'] - minY) / (maxY - minY)
        alpha0 = (v0['z2'] - minZ) / (maxZ - minZ)
        alpha0 = max(0.1, alpha0)

        # Get the second vertex info
        v1 = graph.vs[e[1]]
        x1 = (v1['x2'] - minX) / (maxX - minX)
        y1 = (v1['y2'] - minY) / (maxY - minY)
        alpha1 = (v1['z2'] - minZ) / (maxZ - minZ)
        alpha1 = max(0.1, alpha1)

        # Setup the pattern info
        pat = cairo.LinearGradient(x0, y0, x1, y1)
        pat.add_color_stop_rgba(0, 1, 1.0, 1.0,  alpha0 / 6.0)
        pat.add_color_stop_rgba(1, 1, 1.0, 1.0,  alpha1 / 6.0)
        con.set_source(pat)

        # Draw the line
        con.set_line_width(vertexRadius / 4.0)
        con.move_to(x0, y0)
        con.line_to(x1, y1)
        con.stroke()

    # Draw vertices in z-order
    for i in zOrder:
        v = graph.vs[i]
        alpha = (v['z2'] - minZ) / (maxZ - minZ)
        alpha = max(0.1, alpha)
        radius = vertexRadius
        x = (v['x2'] - minX) / (maxX - minX)
        y = (v['y2'] - minY) / (maxY - minY)

        # Setup the radial pattern for 3D lighting effect
        pat = cairo.RadialGradient(x, y, radius / 4.0, x, y, radius)
        pat.add_color_stop_rgba(0, alpha, 0, 0, 1)
        pat.add_color_stop_rgba(1, 0, 0, 0, 1)
        con.set_source(pat)


        # Draw the vertex sphere
        con.move_to(x, y)
        con.arc(x, y, radius, 0, 2 * numpy.pi)
        con.fill()

    # Output the surface
    surf.write_to_png(fileName)

def generate_sphere(dimension, size):
    coords = []

    for i in range(0, size):
        radius = 0.0
        vector = []

        for j in range(0, dimension):
            vector.append(random.normalvariate(0, 1))
            radius += vector[j]**2

        radius = math.sqrt(radius)
        coords.append([x / radius for x in vector])

    return coords

def get_edges(graph, coords, threshold):
    for i in range(0, len(coords)):
        for j in range (0, len(coords)):
            if i != j:
                path = 0.0
                for k in range(0, len(coords[0])):
                    path += (coords[i][k] - coords[j][k])**2
                path = math.sqrt(path)

                if path < threshold:
                    graph.add_edge(i, j)
    return graph

def get_graph(dimension, size, threshold):
    g = igraph.Graph()
    g.add_vertices(size)

    coords = generate_sphere(dimension=dimension, size=size)
    g = get_edges(graph=g, coords=coords, threshold=threshold)

    return g, coords

def generate_report(dimension, size, threshold, file_name, start_time):
    file = open(file_name + ".txt", "w")

    file.write("Network generated with next parameters: \n")
    file.write("   -Dimension: {0} \n".format(dimension))
    file.write("   -Vertices: {0} \n".format(size))
    file.write("   -Threshold: {0} \n".format(threshold))
    file.write("   -Time: {0} seconds \n".format(time.time() - start_time))

    file.close()

dimension = 3
size = 100
threshold = 0.25
visualise = False

start_time = time.time()

graph, coords = get_graph(dimension=dimension, size=size, threshold=threshold)

file_name = "{0} vertices_{1} dimension_{2} threshold ".format(size, dimension, threshold)
file_name = "networks/" + file_name
ts = time.time()

st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
st = st.replace(":", "_")
file_name += st

print file_name

generate_report(dimension=dimension, size=size, threshold=threshold, file_name=file_name, start_time=start_time)

if dimension == 3 and visualise:
    drawGraph3D(graph, coords, [0, 0, 0], file_name + ".jpg")
if dimension == 2 and visualise:
    drawGraph2D(graph, coords, file_name + ".jpg")

#TODO 2D plotting with cairo
#TODO degree_distribution, clasterization_coef into report
#TODO large graphs??? visualise? how much time need to generate? square-time complexity
#TODO check ideas with different weight distribution