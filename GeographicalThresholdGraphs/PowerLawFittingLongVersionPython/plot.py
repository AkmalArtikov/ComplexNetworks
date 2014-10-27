import cairo
import igraph
import numpy

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

    for i in range(0, len(coords)):
        layout[i] = coords[i]

    plot = igraph.plot(graph, bbox=(0, 0, 1280, 1280), layout=layout)
    plot.save(fileName)

def drawGraph3D(graph, coords, angle, fileName):
    '''
    Draw a graph in 3D with the given layout, angle, and filename.
    '''

    # Setup some vertex attributes and calculate the projection
    layout = graph.layout("sphere")

    for i in range(0, len(coords)):
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