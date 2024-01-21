from luminaria.animations import Pulsate
from luminaria.models import Triangle, Add


def hex_to_color(s: str):
    """
    Converts a given hexadecimal color code string (e.g., "ABCDEF") to an RGB tuple containing three
    integer values representing the red, green, and blue components of that color in that order. If the
    provided input is not a valid hexadecimal color code string, returns None instead.

    :param s: String representation of a hexadecimal color code (e.g., "ABCDEF")

    :returns: Tuple containing three integer values representing the red, green, and blue components of an RGB color
    """
    return (int(s[0:2], 16),
            int(s[2:4], 16),
            int(s[4:6], 16),
            )


def int24_to_color(value: int):
    """
    Create a color tuple from a 24-bit integer.

    :param value: The 24-bit integer to extract bytes from.

    :return: An RGB color tuple.
    """
    red = (value >> 16) & 255
    green = (value >> 8) & 255
    blue = value & 255
    color = (red, green, blue)
    return color


def map_value(value, from_min, from_max, to_min, to_max):
    """
    Map a value from one range to another.

    :param float value: The input value to be mapped.
    :param float from_min: The minimum value of the range we are mapping from.
    :param float from_max: The maximum value of the range we are mapping from.
    :param float to_min: The minimum value of the range we are mapping to.
    :param float to_max: The maximum value of the range we are mapping to.

    :return: The mapped value in the output range.
    """
    return (value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min


def make_crystal_model(
        upper_color: tuple, upper_period_sec: float,
        middle_color: tuple, middle_period_sec: float,
        lower_color: tuple, lower_period_sec: float):
    """
    Create a crystal model consisting of pulsating triangles in three ranges.

    :param tuple upper_color: The RGB color tuple for the upper range of the crystal.
    :param float upper_period_sec: The pulsation period in seconds for the upper range.
    :param tuple middle_color: The RGB color tuple for the middle range of the crystal.
    :param float middle_period_sec: The pulsation period in seconds for the middle range.
    :param tuple lower_color: The RGB color tuple for the lower range of the crystal.
    :param float lower_period_sec: The pulsation period in seconds for the lower range.

    :return: A pulsating model representing the crystal with combined upper, middle, and lower ranges.
    """
    # Create upper range of crystal
    upper_triangle = Triangle("crystal upper triangle", 0.6, 1.0, upper_color)
    upper_pulsate = upper_triangle
    if upper_period_sec <= 10.0:
        upper_pulsate = Pulsate("crystal upper pulsate", 0.3, 1.0,
                                1000 * upper_period_sec / 2.0, 1000 * upper_period_sec / 2.0, upper_triangle)

    # Create middle range of crystal
    middle_triangle = Triangle("crystal middle triangle", 0.3, 0.7, middle_color)
    middle_pulsate = middle_triangle
    if middle_period_sec <= 10.0:
        middle_pulsate = Pulsate("crystal middle pulsate", 0.4, 1.0,
                                 1000 * middle_period_sec / 2.0, 1000 * middle_period_sec / 2.0, middle_triangle)

    # Create lower range of crystal
    lower_triangle = Triangle("crystal lower triangle", 0.0, 0.4, lower_color)
    lower_pulsate = lower_triangle
    if lower_period_sec <= 10.0:
        lower_pulsate = Pulsate("crystal lower pulsate", 0.3, 1.0,
                                1000 * lower_period_sec / 2.0, 1000 * lower_period_sec / 2.0, lower_triangle)

    sum_pulsates = Add("sum", [upper_pulsate, middle_pulsate, lower_pulsate])

    return sum_pulsates
