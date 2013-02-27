import numpy as np
import random
from mayavi.mlab import *
from mayavi.modules.grid_plane import GridPlane

from pyface.api import GUI
# The core Engine.
from mayavi.core.api import Engine
from mayavi.core.ui.engine_view import EngineView
import json



def renderLearningData(filename):
    f = open(filename, 'r')
    data = json.loads(f.read())

    sf = 4
    refp = data['referencePoint']
    scounter = 2
    stypes = {}

    pointsx = []
    pointsy = []
    pointsz = []
    pointss = []
    qx = []
    qy = []
    qz = []
    qu = []
    qv = []
    qw = []

    def addPoint(p, s):
        pointsx.append(p['x'] * sf)
        pointsy.append(p['y'] * sf)
        pointsz.append(p['z'] * sf)
        pointss.append(s)

    def addArrow(p, pv):
        qx.append(p['x'] * sf)
        qy.append(p['y'] * sf)
        qz.append(p['z'] * sf)
        qu.append(pv['x'] * sf)
        qv.append(pv['y'] * sf)
        qw.append(pv['z'] * sf)

    addPoint(refp, 1)

    for o in data['objects']:
        p = o['pose']['point']
        pv = o['pose']['poseVector']

        if o['type'] not in stypes:
            print 'add type', o['type']
            stypes[o['type']] = scounter
            scounter = scounter + 1

        addPoint(p, stypes[o['type']])
        addArrow(p, pv)


    nodes = points3d(pointsx, pointsy, pointsz, pointss, vmin=0, vmax=5, colormap='blue-red', opacity=0.5, scale_mode='none', scale_factor=0.06)

    # Connect point 1 and 2, as well as 3 and 4
    connections = []
    for i in range(1, len(pointsx)):
        connections.append((0, i))
    nodes.mlab_source.dataset.lines = np.array(connections)
    nodes.mlab_source.update()

    lines = pipeline.surface(nodes, opacity=0.4)
    outline(nodes)

    quiver3d(qx, qy, qz, qu, qv, qw, scale_factor=0.05)

class ExampleRenderer:
    sf = 10
    pcounter = 0

    def addPoint(self, p, s, label=''):
        px = p['x'] * self.sf
        py = p['y'] * self.sf
        pz = p['z'] * self.sf
        if label != '':
            self.texts.append((px, py, pz, label));
        self.pointsx.append(px)
        self.pointsy.append(py)
        self.pointsz.append(pz)
        self.pointss.append(s)
        old = self.pcounter
        self.pcounter +=1
        return old

    def addArrow(self, p, pv):
        self.qx.append(p['x'] * self.sf)
        self.qy.append(p['y'] * self.sf)
        self.qz.append(p['z'] * self.sf)
        self.qu.append(pv['x'] * self.sf)
        self.qv.append(pv['y'] * self.sf)
        self.qw.append(pv['z'] * self.sf)

    def __init__(self, filename):
        f = open(filename, 'r')
        data = json.loads(f.read())

        refp = data['referencePose']
        ocounter = 2
        otypes = {}

        self.texts = []
        self.pointsx = []
        self.pointsy = []
        self.pointsz = []
        self.pointss = []
        self.qx = []
        self.qy = []
        self.qz = []
        self.qu = []
        self.qv = []
        self.qw = []
        gridsize = 20
        contour = np.zeros((gridsize, gridsize))

        connections = []

        self.addPoint(refp['point'], 1)
        self.addArrow(refp['point'], refp['poseVector'])

        for i in data['objects']:
            o = i['object']
            p = o['pose']['point']
            pv = o['pose']['poseVector']

            if o['type'] not in otypes:
                print 'add type', o['type']
                otypes[o['type']] = ocounter
                ocounter = ocounter + 1

            idx = self.addPoint(p, otypes[o['type']], label=o['type'])
            for v in i['votes']:
                #id = self.addPoint(v, ocounter + 20)
                #connections.append((idx, id))
                self.addArrow(p, pv)


        nodes = points3d(
                self.pointsx,
                self.pointsy,
                self.pointsz,
                self.pointss,
                vmin=0,
                colormap='blue-red',
                opacity=0.5,
                scale_mode='none',
                scale_factor=0.5)

        nodes.mlab_source.dataset.lines = np.array(connections)
        nodes.mlab_source.update()

        lines = pipeline.surface(nodes, opacity=0.6)
        outline(nodes)

        quiver3d(self.qx, self.qy, self.qz, self.qu, self.qv, self.qw)

        for (px, py, pz, label) in self.texts:
            text3d(px, py, pz, label, scale=0.4)

        minx = np.array(self.pointsx).min()
        maxx = np.array(self.pointsx).max()
        miny = np.array(self.pointsy).min()
        maxy = np.array(self.pointsy).max()
        minz = np.array(self.pointsz).min()
        maxz = np.array(self.pointsz).max()
        print minx, maxx, miny, maxy, minz, maxz
        ex = maxx - minx
        ey = maxy - miny
        ez = maxz - minz

        for i in range(0, len(self.pointsx)):
            px = self.pointsx[i]
            py = self.pointsy[i]
            rpx = round(((px - minx) / (ex)) * gridsize)
            rpy = round(((py - miny) / (ey)) * gridsize)
            print px, py, rpx, rpy
            contour[rpx - 1][rpy - 1] += 0.2


        surf(contour, extent=[minx, maxx, miny, maxy, minz - ez, maxz - ez], opacity=1, colormap='jet')


def main():
    # Create the MayaVi engine and start it.
    e = Engine()
    # Starting the engine registers the engine with the registry and
    # notifies others that the engine is ready.
    e.start()

    # Do this if you need to see the MayaVi tree view UI.
    ev = EngineView(engine=e)
    ui = ev.edit_traits()

    # Create a new scene.
    #scene = e.new_scene()
    #renderLearningData('pattern.json')
    #scene = e.new_scene()
    #renderLearningData('pattern2.json')
    #scene = e.new_scene()
    #renderLearningData('pattern3.json')
    scene = e.new_scene()
    ExampleRenderer('voted.json')
    show()
    return e, ui

if __name__ == '__main__':
    # When main returns the ui to go out of scope and be gc'd causing the view
    # to disappear with qt4.
    e, ui = main()
    # Create a GUI instance and start the event loop.  We do this here so that
    # main can be run from IPython -wthread if needed.
    gui = GUI()
    gui.start_event_loop()

