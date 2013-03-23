import numpy as np
import math
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

    def addArrow(self, p, pv, s):
        self.qx.append(p['x'] * self.sf)
        self.qy.append(p['y'] * self.sf)
        self.qz.append(p['z'] * self.sf)
        self.qu.append(pv['x'] * self.sf)
        self.qv.append(pv['y'] * self.sf)
        self.qw.append(pv['z'] * self.sf)
        self.qs.append(s)

    def resetArrows(self):
        self.qx = []
        self.qy = []
        self.qz = []
        self.qu = []
        self.qv = []
        self.qw = []
        self.qs = []

    def considerPoint(self, p):
        self.mimax.append(p['x'])
        self.mimay.append(p['y'])
        self.mimaz.append(p['z'])

    def voteTo(self, p):
        px = p['x'] * self.sf
        py = p['y'] * self.sf
        self.votesx.append(px)
        self.votesy.append(py)

    def __init__(self, filename, scene):
        f = open(filename, 'r')
        data = json.loads(f.read())

        refp = data['referencePose']
        ocounter = 2
        otypes = {}

        self.texts = []
        self.mimax = []
        self.mimay = []
        self.mimaz = []
        self.pointsx = []
        self.pointsy = []
        self.pointsz = []
        self.pointss = []
        self.votesx = []
        self.votesy = []
        self.votesz = []
        self.poseArrows = []
        self.voteArrows = []
        self.resetArrows()
        gridsize = 30
        contour = np.ones((gridsize, gridsize))

        connections = []

        self.addPoint(refp['point'], 1, 'reference point')
        self.considerPoint(refp['point'])
        self.poseArrows.append((refp['point'], refp['poseVector'], ocounter))

        for i in data['objects']:
            o = i['object']
            p = o['pose']['point']
            pv = o['pose']['poseVector']

            ocounter += 1

            idx = self.addPoint(p, ocounter, label=o['type'])
            self.considerPoint(p)
            for v in i['votes']:
                self.considerPoint(v)
                #id = self.addPoint(v, ocounter + 20)
                #connections.append((idx, id))
                pv = {}
                pv['x'] = v['x'] - p['x']
                pv['y'] = v['y'] - p['y']
                pv['z'] = v['z'] - p['z']
                self.voteTo(v)
                self.voteArrows.append((p, pv, ocounter))

        nodes = points3d(
                self.pointsx,
                self.pointsy,
                self.pointsz,
                self.pointss,
                vmin=0,
                colormap='blue-red',
                opacity=0.5,
                scale_mode='none',
                scale_factor=0.3)

        nodes.mlab_source.dataset.lines = np.array(connections)
        nodes.mlab_source.update()

        lines = pipeline.surface(nodes, opacity=0.6)

        for (p, pv, s) in self.voteArrows:
            self.addArrow(p, pv, s)
        n = quiver3d(self.qx,
                self.qy,
                self.qz,
                self.qu,
                self.qv,
                self.qw,
                scalars=self.qs,
                scale_factor=1,
                opacity=0.5)
        n.glyph.glyph_source.glyph_source = n.glyph.glyph_source.glyph_dict['arrow_source']
        n.glyph.glyph_source.glyph_source.tip_radius = 0.01
        n.glyph.glyph_source.glyph_source.progress = 1.0
        n.glyph.glyph_source.glyph_source.shaft_radius = 0.002
        n.glyph.glyph_source.glyph_source.tip_length = 0.0492
        n.glyph.glyph.range = [ 1.,  1.]
        n.glyph.glyph.color_mode = 'color_by_scalar'
        self.resetArrows()
        outline()

        for (p, pv, s) in self.poseArrows:
            self.addArrow(p, pv, s)
        quiver3d(
                self.qx,
                self.qy,
                self.qz,
                self.qu,
                self.qv,
                self.qw,
                scalars=self.qs,
                mode='arrow',
                scale_mode='none',
                scale_factor=0.03,
                opacity=0.5)

        for (px, py, pz, label) in self.texts:
            text3d(px, py, pz, label, scale=0.2)

        self.minx = np.array(self.mimax).min() * self.sf
        self.maxx = np.array(self.mimax).max() * self.sf
        self.miny = np.array(self.mimay).min() * self.sf
        self.maxy = np.array(self.mimay).max() * self.sf
        self.minz = np.array(self.mimaz).min() * self.sf
        self.maxz = np.array(self.mimaz).max() * self.sf
        ex = self.maxx - self.minx
        ey = self.maxy - self.miny
        ez = self.maxz - self.minz
        print self.minx, self.maxx, self.miny, self.maxy, self.minz, self.maxz, ex, ey, ez

        for i in range(0, len(self.votesx)):
            px = self.votesx[i]
            py = self.votesy[i]
            rpx = round(((px - self.minx) / (ex)) * gridsize)
            rpy = round(((py - self.miny) / (ey)) * gridsize)
            contour[rpx - 1][rpy - 1] += 1
            print px, py, rpx, rpy, ex, ey, contour[rpx - 1][rpy - 1]

        for x in range(0, gridsize):
            for y in range(0, gridsize):
                contour[x][y] = math.log(contour[x][y])

        s = surf(contour,
            extent=[self.minx, self.maxx, self.miny, self.maxy, self.minz - ez / 2, self.maxz - ez / 2],
            opacity=1,
            colormap='jet')

        scene.scene.camera.position = [-5.7103637050230756, -10.888659180922089, 24.539917749501686]
        scene.scene.camera.focal_point = [6.5652658044879457, -0.91543314381867202, 17.35787000664325]
        scene.scene.camera.view_angle = 30.0
        scene.scene.camera.view_up = [0.35218711444961392, 0.22137596584026409, 0.90937171616668644]
        scene.scene.camera.clipping_range = [8.746184970243073, 29.223277267993776]
        scene.scene.camera.compute_view_plane_normal()
        scene.scene.render()


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
    ExampleRenderer('voted.json', scene)
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

