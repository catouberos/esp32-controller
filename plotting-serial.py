import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Serial port configuration
PORT = "/dev/ttyUSB1"  # Replace with your serial port
BAUDRATE = 115200
TIMEOUT = 1

# Initialize serial connection
ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)

# Initialize variables for plotting
time_data = list(range(10))  # Time axis (fixed size for real-time scrolling)
inputs = [[0.0] * 10 for _ in range(4)]  # Input data for 4 wheels (last 10 points)
outputs = [[0.0] * 10 for _ in range(4)]  # Output data for 4 wheels (last 10 points)
setpoints = [
    [0.0] * 10 for _ in range(4)
]  # Setpoint data for 4 wheels (last 10 points)

# Create subplots for each wheel
fig, axes = plt.subplots(2, 2, figsize=(10, 8))
wheels = ["Wheel 1", "Wheel 2", "Wheel 3", "Wheel 4"]
lines = []  # To hold line objects

for i, ax in enumerate(axes.flatten()):
    (line_input,) = ax.plot(time_data, inputs[i], label="Input", linestyle="-")
    (line_output,) = ax.plot(time_data, outputs[i], label="Output", linestyle=":")
    (line_setpoint,) = ax.plot(
        time_data, setpoints[i], label="Setpoint", linestyle="--"
    )
    ax.set_title(wheels[i])
    ax.set_xlim(0, 9)  # Fixed x-axis range for real-time scrolling
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Value")
    ax.legend()
    lines.append((line_input, line_output, line_setpoint))


# Update function for real-time plotting
def update(frame):
    global inputs, outputs, setpoints

    # Read data from serial port
    try:
        raw_data = ser.readline().decode("utf-8").strip()
        if raw_data:
            values = [float(v) for v in raw_data.split(",")]
            if len(values) == 12:  # Ensure the data length matches expectations
                # Update input, output, and setpoint for each wheel
                for i in range(4):
                    inputs[i].append(values[i * 3])  # Append new input
                    inputs[i].pop(0)  # Remove the oldest input
                    outputs[i].append(values[i * 3 + 1])  # Append new output
                    outputs[i].pop(0)  # Remove the oldest output
                    setpoints[i].append(values[i * 3 + 2])  # Append new setpoint
                    setpoints[i].pop(0)  # Remove the oldest setpoint

    except (ValueError, UnicodeDecodeError):
        print("Error: Invalid data received. Skipping...")

    # Update plots for each wheel
    for i, (line_input, line_output, line_setpoint) in enumerate(lines):
        line_input.set_ydata(inputs[i])
        line_output.set_ydata(outputs[i])
        line_setpoint.set_ydata(setpoints[i])

        # Dynamically adjust y-axis limits based on data
        all_data = inputs[i] + outputs[i] + setpoints[i]
        min_val = min(all_data)
        max_val = max(all_data)
        axes.flatten()[i].set_ylim(min_val - 10, max_val + 10)  # Add padding to y-axis

    return [line for triple in lines for line in triple]


# Animation
ani = animation.FuncAnimation(fig, update, interval=100)

# Show the plot
plt.tight_layout()
plt.show()

# Close the serial connection when the script ends
ser.close()
