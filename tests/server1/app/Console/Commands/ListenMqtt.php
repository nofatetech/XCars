<?php

namespace App\Console\Commands;

use App\Models\Vehicle;
use Illuminate\Console\Command;
use PhpMqtt\Client\MqttClient;
use PhpMqtt\Client\ConnectionSettings;

class ListenMqtt extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'mqtt:listen';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Listen for MQTT messages from vehicles';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        $server   = config('mqtt.host');
        $port     = config('mqtt.port');
        $clientId = config('mqtt.client_id') . '_server_' . uniqid();
        $username = config('mqtt.username');
        $password = config('mqtt.password');

        $mqtt = new MqttClient($server, $port, $clientId);

        $connectionSettings = (new ConnectionSettings)
            ->setUsername($username)
            ->setPassword($password);

        $mqtt->connect($connectionSettings, true);
        $this->info("MQTT client connected and listening for messages...");

        // Subscribe to the topic where all vehicles publish their status
        $mqtt->subscribe('vehicles/+/status', function ($topic, $message) {
            $this->info("Received message on topic [{$topic}]: {$message}");

            $data = json_decode($message, true);

            // Use the vehicle's ID from the payload to update its record
            // in the server's database.
            if (isset($data['id'])) {
                Vehicle::updateOrCreate(
                    ['id' => $data['id']], // Find vehicle by its unique ID
                    $data                  // Update with the new data from the payload
                );
                $this->info("Updated vehicle #{$data['id']}");
            }
        }, 0);

        // Subscribe to the topic where control commands are published
        $mqtt->subscribe('vehicle/+/control', function ($topic, $message) {
            $this->info("Received control command on topic [{$topic}]: {$message}");
            // At this point, the command has been published.
            // The actual vehicle is expected to subscribe to this topic directly.
            // We are just logging it here for visibility within the Laravel app.
            $data = json_decode($message, true);
            $vehicleId = explode('/', $topic)[1]; // Extract vehicle ID from topic
            if (isset($data['action'])) {
                $this->info("Vehicle #{$vehicleId} - Action: {$data['action']}, Speed: {$data['speed'] ?? 'N/A'}");
                // Further actions can be added here, e.g., logging to a database,
                // updating UI in real-time if a WebSocket server is running.
            }
        }, 0);

        $mqtt->loop(true);
    }
}
