import pygame
import paho.mqtt.client as mqtt
import time
import json
import os

# MQTT Configuration
MQTT_BROKER = os.getenv('MQTT_HOST', '127.0.0.1')
MQTT_PORT = int(os.getenv('MQTT_PORT', 1883))
MQTT_TOPIC = f"vehicle/{VEHICLE_ID}/control" # Topic for sending control commands
MQTT_USERNAME = os.getenv('MQTT_USERNAME')
MQTT_PASSWORD = os.getenv('MQTT_PASSWORD')
CLIENT_ID = f"python_gamepad_client_{os.getpid()}"

# Vehicle ID to control (can be made dynamic later)
# For now, let's assume we are controlling a specific vehicle.
# This could be selected via a UI or command line argument.
VEHICLE_ID = 1 # !!! IMPORTANT: CHANGE THIS TO THE ACTUAL VEHICLE ID YOU WANT TO CONTROL !!!

# Gamepad State
gamepad_connected = False
current_speed = 0.0 # From 0.0 to 1.0
current_action = "stop" # "forward", "backward", "left", "right", "stop"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
    else:
        print(f"Failed to connect, return code {rc}\n")

def publish_command(client, vehicle_id, action, speed):
    payload = {
        "vehicle_id": vehicle_id,
        "action": action,
        "speed": speed
    }
    client.publish(MQTT_TOPIC, json.dumps(payload))
    print(f"Published: {payload}")

def init_pygame():
    pygame.init()
    pygame.joystick.init()

    if pygame.joystick.get_count() == 0:
        print("No joystick found. Please connect a joystick and restart.")
        pygame.quit()
        return None
    else:
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print(f"Detected joystick: {joystick.get_name()}")
        return joystick

def main():
    global gamepad_connected, current_speed, current_action

    # MQTT Client setup
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, CLIENT_ID)
    client.on_connect = on_connect

    if MQTT_USERNAME and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start()
    except Exception as e:
        print(f"Could not connect to MQTT broker: {e}")
        return

    joystick = init_pygame()
    if not joystick:
        return

    gamepad_connected = True

    try:
        while True:
            for event in pygame.event.get():
                if event.type == pygame.JOYAXISMOTION:
                    # Axis 0: Left Stick X (left/right)
                    # Axis 1: Left Stick Y (up/down)
                    # Axis 2: Right Stick X (left/right)
                    # Axis 3: Right Stick Y (up/down)

                    # Example for a common controller (e.g., Xbox/PlayStation style)
                    # Left stick Y-axis for forward/backward
                    if event.axis == 1: # Y-axis of left stick
                        if event.value < -0.1: # Moving forward
                            new_speed = abs(event.value) # Speed from 0 to 1
                            if current_action != "forward" or new_speed != current_speed:
                                current_action = "forward"
                                current_speed = new_speed
                                publish_command(client, VEHICLE_ID, current_action, current_speed)
                        elif event.value > 0.1: # Moving backward
                            new_speed = abs(event.value)
                            if current_action != "backward" or new_speed != current_speed:
                                current_action = "backward"
                                current_speed = new_speed
                                publish_command(client, VEHICLE_ID, current_action, current_speed)
                        else: # Stick released to center
                            if current_action != "stop":
                                current_action = "stop"
                                current_speed = 0.0
                                publish_command(client, VEHICLE_ID, current_action, current_speed)

                    # Left stick X-axis for left/right (steering)
                    elif event.axis == 0: # X-axis of left stick
                        if event.value < -0.1: # Steering left
                            if current_action != "left":
                                current_action = "left"
                                publish_command(client, VEHICLE_ID, current_action, current_speed)
                        elif event.value > 0.1: # Steering right
                            if current_action != "right":
                                current_action = "right"
                                publish_command(client, VEHICLE_ID, current_action, current_speed)
                        else: # Stick released to center, return to previous forward/backward/stop state
                            # This needs more sophisticated logic if we want to hold direction.
                            # For now, if moving forward/backward, releasing X axis doesn't change that.
                            # If stopped, it stays stopped.
                            pass # Do nothing, main movement (Y-axis) dictates action if not turning

                elif event.type == pygame.JOYBUTTONDOWN:
                    # Example button mapping (adjust for your specific gamepad)
                    if event.button == 0: # A button (Xbox) or X button (PlayStation)
                        print("Button 0 pressed - Emergency Stop")
                        if current_action != "stop":
                            current_action = "stop"
                            current_speed = 0.0
                            publish_command(client, VEHICLE_ID, current_action, current_speed)
                    # Add more button mappings as needed
                    # if event.button == X: publish_command(...)

                elif event.type == pygame.JOYBUTTONUP:
                    pass # Or handle specific button release actions

                elif event.type == pygame.JOYDEVICEREMOVED:
                    print("Joystick disconnected.")
                    gamepad_connected = False
                    # Potentially send a "stop" command if the vehicle was moving
                    if current_action != "stop":
                        current_action = "stop"
                        current_speed = 0.0
                        publish_command(client, VEHICLE_ID, current_action, current_speed)
                    break # Exit polling loop to re-initialize or wait

            if not gamepad_connected:
                # Attempt to re-initialize joystick if disconnected
                pygame.joystick.quit()
                pygame.joystick.init()
                joystick = init_pygame()
                if joystick:
                    gamepad_connected = True
                    print("Joystick reconnected.")
                else:
                    time.sleep(1) # Wait before trying again

            time.sleep(0.05) # Small delay to prevent busy-waiting

    except KeyboardInterrupt:
        print("Exiting gamepad controller.")
    finally:
        client.loop_stop()
        client.disconnect()
        pygame.quit()

if __name__ == "__main__":
    main()
