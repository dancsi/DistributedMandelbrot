import json
import subprocess

import numpy as np
import pyglet
from pyglet.gl import *
from pyglet.window import mouse

window_size = np.r_[640, 480]
window_pos = np.r_[0, 0]
window = pyglet.window.Window(*window_size)


def blit_texture_fullscreen(texture):
    x = 0
    y = 0
    z = 0
    t = texture.tex_coords
    x1 = x - texture.anchor_x
    y1 = y - texture.anchor_y
    x2 = x1 + window_size[0]
    y2 = y1 + window_size[1]
    array = (GLfloat * 32)(
        t[0], t[1], t[2], 1.,
        x1, y1, z, 1.,
        t[3], t[4], t[5], 1.,
        x2, y1, z, 1.,
        t[6], t[7], t[8], 1.,
        x2, y2, z, 1.,
        t[9], t[10], t[11], 1.,
        x1, y2, z, 1.)

    glPushAttrib(GL_ENABLE_BIT)
    glEnable(texture.target)
    glBindTexture(texture.target, texture.id)
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT)
    glInterleavedArrays(GL_T4F_V4F, 0, array)
    glDrawArrays(GL_QUADS, 0, 4)
    glPopClientAttrib()
    glPopAttrib()


def get_transform():
    return mandelbrot_to_img @ screen_to_mandelbrot


def check_valid_transform(transform_):
    transform = mandelbrot_to_img @ transform_
    lower_left = transform @ np.r_[0, 0, 1]
    upper_right = transform @ np.r_[window_size, 1]
    lower_bound = np.r_[0, 0, 1]
    upper_bound = np.r_[texture.width, texture.height, 1]
    return (lower_bound <= lower_left).all() and (lower_left <= upper_bound).all() and (
        lower_bound <= upper_right).all() and (upper_right <= upper_bound).all()


def get_texture_region():
    transform = get_transform()
    corner = transform @ np.r_[0, 0, 1]
    dims = (transform @ np.r_[window_size, 1]) - corner
    return corner[:2], dims[:2]


def update_transform(transform, check):
    global screen_to_mandelbrot

    valid = True
    if check:
        valid = check_valid_transform(transform)

    screen_to_mandelbrot = transform

    _, dims = get_texture_region()
    zoom_level = (window_size / dims).max()
    if zoom_level > 1 or not valid:
        render_new()

    return True


def update_transform_checked(transform):
    return update_transform(transform, check=True)


def update_transform_unchecked(transform):
    return update_transform(transform, check=False)


def set_identity_transform():
    update_transform_unchecked(screen_to_mandelbrot_identity)


@window.event
def on_key_release(symbol, modifiers):
    if symbol == pyglet.window.key.R:
        set_identity_transform()


@window.event
def on_draw():
    window.clear()
    corner, dims = get_texture_region()
    part = texture.get_region(x=corner[0], y=corner[1], width=dims[0], height=dims[1])
    blit_texture_fullscreen(part)


@window.event
def on_mouse_drag(x, y, dx, dy, buttons, modifiers):
    print("Mouse drag {}, {}".format(dx, dy))
    if buttons & mouse.LEFT:
        update_transform_checked(screen_to_mandelbrot @ translate(-dx, -dy))


@window.event
def on_mouse_scroll(x, y, scroll_x, scroll_y):
    global screen_to_mandelbrot
    zoom_factor = 1.05 ** scroll_y
    update_transform_checked(screen_to_mandelbrot @ scale_around_point(zoom_factor, x, y))


def translate(x, y):
    ret = np.eye(3)
    ret[0][2] = x
    ret[1][2] = y
    return ret


def scale(f):
    return np.diag([1.0 / f, 1.0 / f, 1])


def scale_around_point(zoom_factor, x, y):
    return translate(x, y) @ scale(zoom_factor) @ translate(-x, -y)


def render(pos, viewport_size, viewport_size_px, outfile="temp.ppm"):
    global img, texture
    json_ = "\n".join(
        [
            "{",
            "\t\"pos\": [{}, {}],".format(*(pos[:2])),
            "\t\"viewport_size\": [{}, {}],".format(*(viewport_size[:2])),
            "\t\"viewport_size_px\": [{}, {}]".format(*(viewport_size_px[:2])),
            "}"
        ])
    print(json_)
    with open("temp.json", "w") as fout:
        fout.write(json_)
    subprocess.run(["../msvc/Release/mandelbrot.exe", "--jobspec=temp.json", "--standalone", "--processor=worker-mt",
                    "--outfile={}".format(outfile)], check=True)
    img = pyglet.image.load(outfile)
    texture = img.get_texture()


def render_new():
    global mandelbrot_to_img
    global screen_to_mandelbrot
    global screen_to_mandelbrot_identity
    print("Rendering")
    lower_left = screen_to_mandelbrot @ np.r_[0, 0, 1]
    upper_right = screen_to_mandelbrot @ np.r_[window_size, 1]
    dims = upper_right - lower_left
    lower_left_margin = lower_left - dims
    upper_right_margin = upper_right + dims
    viewport_size = upper_right_margin - lower_left_margin
    texture_size = 3 * window_size
    render(lower_left_margin, viewport_size, texture_size)

    mandelbrot_to_img = np.diag([texture_size[0] / viewport_size[0], texture_size[1] / viewport_size[1], 1])
    mandelbrot_to_img = mandelbrot_to_img @ translate(*(-lower_left_margin[:2]))

    screen_to_mandelbrot = np.diag(np.r_[dims[:2] / window_size, 1])
    screen_to_mandelbrot = translate(*(lower_left[:2])) @ screen_to_mandelbrot
    screen_to_mandelbrot_identity = screen_to_mandelbrot.copy()


img = None
texture = None

pos = np.r_[-2.5, -1.5]
viewport = np.r_[4, 3]

render(pos, viewport, window_size)

screen_to_mandelbrot = translate(*pos) @ np.diag(np.r_[viewport / window_size, 1])
mandelbrot_to_img = np.diag([img.width / viewport[0], img.height / viewport[1], 1]) @ translate(*(-pos))
screen_to_mandelbrot_identity = screen_to_mandelbrot.copy()

render_new()

pyglet.app.run()
