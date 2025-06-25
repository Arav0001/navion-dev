import serial
import struct
import time
import numpy as np
from vpython import *
from scipy.spatial.transform import Rotation as R

PORT = 'COM3'
BAUD = 115200
PACKET_SIZE = 16

# -------------------------
# VPython Scene Setup
# -------------------------
scene.title = "NAVION Orientation Viewer"
scene.background = vector(0.15, 0.15, 0.15)
scene.width = 1920
scene.height = 1080
scene.forward = vector(-1, -1, -2)
scene.range = 3
scene.userspin = True
scene.userzoom = False
scene.autoscale = False

# Store initial camera parameters
initial_center = scene.center
initial_forward = scene.forward
initial_up = scene.up
initial_range = scene.range

running = True

def key_input(evt):
    global running
    s = evt.key.lower()
    if s == 'q':
        print("Quit requested")
        running = False  # tell main loop to stop
    elif s == 'r':
        print("Resetting view")
        scene.center = initial_center
        scene.forward = initial_forward
        scene.up = initial_up
        scene.range = initial_range

scene.bind('keydown', key_input)

# -------------------------
# Ground Grid (X-Z plane)
# -------------------------
GRID_SIZE = 10      # total width/length of grid
GRID_SPACING = 1.0  # distance between lines

for x in np.arange(-GRID_SIZE/2, GRID_SIZE/2 + 0.1, GRID_SPACING):
    curve(pos=[vector(x, 0, -GRID_SIZE/2), vector(x, 0, GRID_SIZE/2)],
          color=color.gray(0.5), radius=0.005, opacity=0.3)

for z in np.arange(-GRID_SIZE/2, GRID_SIZE/2 + 0.1, GRID_SPACING):
    curve(pos=[vector(-GRID_SIZE/2, 0, z), vector(GRID_SIZE/2, 0, z)],
          color=color.gray(0.5), radius=0.005, opacity=0.3)

# -------------------------
# Global Axes
# -------------------------
AXIS_LENGTH = 5
global_x = cylinder(pos=vector(-AXIS_LENGTH, 0, 0), axis=vector(2 * AXIS_LENGTH, 0, 0), radius=0.01, color=color.red, opacity=0.2)  # East
global_y = cylinder(pos=vector(0, 0, AXIS_LENGTH), axis=vector(0, 0, -2 * AXIS_LENGTH), radius=0.01, color=color.green, opacity=0.2)  # North
global_z = cylinder(pos=vector(0, -AXIS_LENGTH, 0), axis=vector(0, 2 * AXIS_LENGTH, 0), radius=0.01, color=color.blue, opacity=0.2)  # Up

# -------------------------
# Rocket / Box Object
# -------------------------
rocket = box(length=1, height=1, width=1, color=vector(0.8, 0.8, 0.8), opacity=0.8)
rocket.pos = vector(0, 0, 0)

# Body-frame axes (rotate with rocket)
axis_x = arrow(pos=rocket.pos, axis=vector(1, 0, 0), color=color.red, shaftwidth=0.03)   # East
axis_y = arrow(pos=rocket.pos, axis=vector(0, 0, -1), color=color.green, shaftwidth=0.03) # North
axis_z = arrow(pos=rocket.pos, axis=vector(0, 1, 0), color=color.blue, shaftwidth=0.03)  # Up

# DISCONNECTED label
disconnect_label = label(
    text='',
    xoffset=0,
    yoffset=scene.height//2 - 40,  # near top of screen
    space='screen',
    height=24,
    color=color.yellow,
    box=False,
    opacity=0,
    align='center',
    line=False
)

# -------------------------
# Serial Handling
# -------------------------
def open_serial():
    while running:
        try:
            ser = serial.Serial(PORT, BAUD, timeout=1)
            print(f"[INFO] Connected to {PORT}")
            disconnect_label.text = ''
            return ser
        except serial.SerialException:
            print("[WARN] Waiting for device...")
            disconnect_label.text = 'DISCONNECTED'
            time.sleep(1)

def read_quaternion(ser):
    try:
        data = ser.read(PACKET_SIZE)
        if len(data) == PACKET_SIZE:
            return struct.unpack('<ffff', data)
    except serial.SerialException:
        print("[ERROR] Serial lost. Reconnecting...")
        ser.close()
    return None

def normalize(q):
    norm = np.linalg.norm(q)
    return q if norm == 0 else q / norm

# -------------------------
# Main Loop
# -------------------------
ser = open_serial()

while running:
    rate(60)

    quat = read_quaternion(ser)
    if quat is None:
        disconnect_label.text = 'DISCONNECTED'
        ser = open_serial()
        continue

    q = normalize(np.array(quat))
    rot = R.from_quat([q[1], q[2], q[3], q[0]])  # [x, y, z, w]
    mat = rot.as_matrix()

    # Apply rotation with x=East, y=North, z=Up
    rocket.axis = vector(*mat[:, 1])  # Up = forward
    rocket.up = vector(*(-mat[:, 2]))  # North = up

    # Body-frame axes
    offset_x = vector(*mat[:, 0]) * 0.5
    offset_y = vector(*(-mat[:, 2])) * 0.5
    offset_z = vector(*mat[:, 1]) * 0.5

    axis_x.pos = rocket.pos + offset_x
    axis_y.pos = rocket.pos + offset_y
    axis_z.pos = rocket.pos + offset_z

    axis_x.axis = vector(*mat[:, 0])  # East
    axis_y.axis = vector(*(-mat[:, 2]))  # North
    axis_z.axis = vector(*mat[:, 1])  # Up