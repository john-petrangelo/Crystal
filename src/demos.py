from luminaria.animations import Pulsate, Rotate
from luminaria.colors import BLUE, RED, BLACK, GREEN, YELLOW
from luminaria.models import Add, Solid, Gradient, Window, Map, Reverse


def make_demo1():
    grad_left = Gradient("grad-right", BLUE, GREEN)
    grad_right = Gradient("grad-left", RED, YELLOW)

    rot_left = Rotate("rotate down", -1/3, grad_left)
    rot_right = Rotate("rotate up", -1, grad_right)

    map_left = Map("map left", 0.0, 1.0, 0.0, 0.3, rot_left)
    map_right = Map("map right", 0.0, 1.0, 0.3, 1.0, rot_right)

    window = Window("window", 0.0, 0.3, map_left, map_right)

    rot_window = Rotate("rotate window", -1/10, window)

    return rot_window


def make_demo2():
    gradient = Gradient("grad", BLUE, RED)
    rot_grad = Rotate("Rotate Gradient", 1/2.5, gradient)
    rev_grad = Reverse("reverse", rot_grad)

    map_left = Map("map left", 0.0, 1.0, 0.0, 0.5, rot_grad)
    map_right = Map("map right", 0.0, 1.0, 0.5, 1.0, rev_grad)

    window = Window("window", 0.0, 0.5, map_left, map_right)
    pulsation = Pulsate("pulsate", 0.2, 1.0, 1000, 1000, window)

    return pulsation


def make_demo3():
    red_gradient = Gradient("grad_red", BLACK, RED)
    blue_gradient = Gradient("grad_red", BLUE, BLACK)
    green_solid = Solid("green solid", GREEN)
    sum3 = Add("Add", [red_gradient, blue_gradient, green_solid])
    pulsation = Pulsate("Pulsation", 0.2, 1.0, 1000, 2000, sum3)

    return pulsation
