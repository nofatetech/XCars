

hostname -i

sudo ufw status
sudo ufw allow 1883

mosquitto_pub -t "vehicle/0/control" -m '{"action":"forward"}'

mosquitto_sub -h 192.168.1.100 -t "vehicle/1/control" &
mosquitto_pub -h 192.168.1.100 -t "vehicle/1/control" -m '{"action":"forward"}'

php artisan listen:mqtt -v

sudo systemctl status mosquitto
sudo systemctl start mosquitto  # If not running

