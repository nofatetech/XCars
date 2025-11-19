

hostname -i

sudo ufw status
sudo ufw allow 1883


{"left": 0.7, "right": 0.7}   → forward
{"left": -0.5, "right": -0.5} → backward
{"left": -0.6, "right": 0.6}  → spin left
{"left": 0.8, "right": 0.3}   → gentle right turn
{"left": 0, "right": 0}       → stop


{"lights":1, "fog":1, "highbeam":1, "honk":1}

blinkers

# Main lights on
mosquitto_pub -h 192.168.0.13 -t vehicle/1/cmd -m '{"lights":1}'

# High beams + fog + honk
mosquitto_pub -h 192.168.0.13 -t vehicle/1/cmd -m '{"highbeam":1,"fog":1,"honk":1}'

# Hazard lights (both blinkers flashing)
mosquitto_pub -h 192.168.0.13 -t vehicle/1/cmd -m '{"blinkers":2}'

# Everything off
mosquitto_pub -h 192.168.0.13 -t vehicle/1/cmd -m '{"lights":0,"fog":0,"highbeam":0,"blinkers":0}'


Payload,Effect
lights_on,Main lights ON
lights_off,Main lights OFF
fog_on / fog_off,Fog lamps
highbeam_on / highbeam_off,High beams
honk,Single horn beep






<!-- mosquitto_pub -t "vehicle/0/control" -m '{"action":"forward"}'

mosquitto_sub -h 192.168.1.100 -t "vehicle/1/control" &
mosquitto_pub -h 192.168.1.100 -t "vehicle/1/control" -m '{"action":"forward"}' -->

php artisan listen:mqtt -v

sudo systemctl status mosquitto
sudo systemctl start mosquitto  # If not running



Rasp Pi

sudo apt update
sudo apt install python3-gpiozero python3-lgpio python3-pigpio gpiod libgpiod-dev pigpio
sudo systemctl enable --now pigpiod   # starts the pigpio daemon



TODOs:
- vehicle dashboard
- battery monitoring
- AI stuff
    - personality
    - gossip protocol??
- cameras
- sounds, other than horn
- SLAM, autonomous



FPV camera + streaming
Cheap option: OV2640/ESP32-CAM module as a separate MQTT video broker (mjpeg2sd or esp32-cam-webserver)
Luxury option: ESP32-S3 with PSI-RAM + CameraWebServer example, stream directly to the same web dashboard (WebSocket MJPEG)



