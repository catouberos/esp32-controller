import matplotlib.pyplot as plt
import matplotlib.animation as animation
from websockets.sync.client import connect
import time
import math

# WebSocket server URL
WS_URL = "ws://192.168.4.250/websocket"

# Function to establish a WebSocket connection
def establish_websocket():
    while True:
        try:
            ws = connect(WS_URL)
            print("WebSocket connection established")
            return ws
        except Exception as e:
            print(f"WebSocket connection failed: {e}. Retrying in 5 seconds...")
            time.sleep(5)

# Connect to WebSocket
ws = establish_websocket()

# Initialize variables for plotting
time_data = []  # Time axis (growing dynamically)
positions = [[] for _ in range(4)]  # Angular positions for 4 wheels

# Conversion constants
cpr = 330  # Counts per revolution
sampling_interval = 0.1  # Sampling interval in seconds

# Function to convert encoder counts to rad/s
def encoder_to_rads(encoder_counts, cpr, sampling_interval):
    revolutions = encoder_counts / cpr
    rpm = revolutions * (60 / sampling_interval)
    rad_per_sec = rpm * (2 * math.pi / 60)
    return rad_per_sec

# Create subplots for each wheel
fig, axes = plt.subplots(2, 2, figsize=(16, 10))  # Adjusted figure size
wheels = ["Wheel 1", "Wheel 2", "Wheel 3", "Wheel 4"]
lines = []  # To hold line objects

for i, ax in enumerate(axes.flatten()):
    (line_position,) = ax.plot([], [], label="Angular Position (rad)", linestyle="-")
    ax.set_title(wheels[i])
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Position (rad)")
    ax.legend()
    lines.append(line_position)

# Update function for real-time plotting
def update(frame):
    global time_data, positions, ws

    while True:
        try:
            raw_data = str(ws.recv())
            if raw_data:
                print(raw_data)
                values = [float(v) for v in raw_data.split(",")]
                if len(values) == 12:  # Ensure the data length matches expectations
                    # Update time
                    current_time = len(time_data) * sampling_interval
                    time_data.append(current_time)

                    # Update positions for each wheel
                    for i in range(4):
                        # Get the angular velocity (rad/s)
                        angular_velocity = encoder_to_rads(values[i * 3 + 1], cpr, sampling_interval)

                        # Integrate angular velocity to get angular position
                        if len(positions[i]) == 0:
                            new_position = angular_velocity * sampling_interval
                        else:
                            new_position = positions[i][-1] + angular_velocity * sampling_interval

                        # Append the new position
                        positions[i].append(new_position)
                break
        except (ValueError, UnicodeDecodeError):
            print("Error: Invalid data received. Skipping...")
            break
        except Exception as e:
            print(f"WebSocket connection lost: {e}. Reconnecting...")
            ws = establish_websocket()
            break

    # Update plots for each wheel
    for i, line_position in enumerate(lines):
        line_position.set_data(time_data, positions[i])
        axes.flatten()[i].set_xlim(0, max(time_data))  # Adjust x-axis to match current time
        min_val = min(positions[i]) if positions[i] else 0
        max_val = max(positions[i]) if positions[i] else 1
        axes.flatten()[i].set_ylim(min_val - 10, max_val + 10)  # Adjust y-axis dynamically

    return lines

# Animation
ani = animation.FuncAnimation(fig, update, interval=100)

# Show the plot
plt.tight_layout()
plt.show()
