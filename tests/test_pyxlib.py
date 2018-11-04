import unittest
import pytest
import pyxlib


class PatternTest(unittest.TestCase):
  def test_constructor(self):
    pc = pyxlib.Pattern("fancy", "some_code")
    assert pc.shader_code == "some_code"
    assert pc.name == "fancy"
    assert pc.enabled

class PointTest(unittest.TestCase):
  def test_constructor(self):
    point = pyxlib.Point(1.0, 2.0, 3.0)
    assert point.x == 1.0
    assert point.y == 2.0
    assert point.z == 3.0
    assert point == pyxlib.Point(1.0, 2.0, 3.0)

class LedGeometryTest(unittest.TestCase):
  def test_constructor(self):
    geom = pyxlib.LedGeometry("localhost")
    assert geom.fadecandy_host == "localhost"

class SculptureTest(unittest.TestCase):
  def test_constructor(self):
    geom = pyxlib.Sculpture(["localhost"], 16)

class PerspectiveTest(unittest.TestCase):
  def test_constructor(self):
    perspective = pyxlib.Perspective()
    perspective.id = 1
    perspective.scope = pyxlib.Point(1.0, 2.0, 3.0)
    perspective.yaw = 90
    perspective.pitch = 10
    perspective.zoom = 4

    assert perspective.id == 1
    assert perspective.scope == pyxlib.Point(1.0, 2.0, 3.0)
    assert perspective.yaw == 90
    assert perspective.pitch == 10
    assert perspective.zoom == 4