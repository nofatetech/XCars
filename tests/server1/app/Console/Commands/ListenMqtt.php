<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use PhpMqtt\Client\MqttClient;

class ListenMqtt extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    // protected $signature = 'app:listen-mqtt';
    protected $signature = 'listen:mqtt';

    /**
     * The console command description.
     *
     * @var string
     */
    // protected $description = 'Command description';
    protected $description = 'Listen for MQTT messages from vehicles';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        
        $client = new MqttClient('localhost', 1883, 'laravel-client');
        $client->connect();
        $client->subscribe('vehicle/#', function ($topic, $message) {
            \Log::info("Received: $topic => $message");
        }, 0);
        $client->loop(true);

    }
}
