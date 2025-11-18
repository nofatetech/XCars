<?php

namespace App\Console\Commands;

use App\Models\Vehicle;
use Illuminate\Console\Command;
use PhpMqtt\Client\MqttClient;
use PhpMqtt\Client\ConnectionSettings;

class VehicleLive extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'vehicle:live';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Run the main vehicle process to manage state and publish MQTT updates.';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        $server   = config('mqtt.host');
        $port     = config('mqtt.port');
        $clientId = config('mqtt.client_id') . '_vehicle_' . uniqid();
        $username = config('mqtt.username');
        $password = config('mqtt.password');

        $mqtt = new MqttClient($server, $port, $clientId);
        
        $connectionSettings = (new ConnectionSettings)
            ->setUsername($username)
            ->setPassword($password);

        $mqtt->connect($connectionSettings, true);
        $this->info("MQTT client connected.");

        // On a vehicle, we assume there's only one vehicle record, which represents itself.
        // We'll use firstOrCreate to initialize it if it doesn't exist.
        $vehicle = Vehicle::firstOrCreate([], [
            'name' => 'My Vehicle',
            'energy' => 100,
            'happiness' => 100,
            'last_interaction_at' => now(),
        ]);

        $this->info("Vehicle '{$vehicle->name}' initialized. Starting live loop.");

        while (true) {
            // Degrade state over time
            $vehicle->energy = max(0, $vehicle->energy - 1);
            $vehicle->happiness = max(0, $vehicle->happiness - 1);
            $vehicle->last_interaction_at = now();
            $vehicle->save();

            $this->info("Energy: {$vehicle->energy}, Happiness: {$vehicle->happiness}");

            // Publish status to MQTT
            $topic = "vehicles/{$vehicle->id}/status";
            $payload = $vehicle->toJson();
            $mqtt->publish($topic, $payload, 0);
            
            $this->info("Published status to topic: {$topic}");

            sleep(5);
        }
    }
}
