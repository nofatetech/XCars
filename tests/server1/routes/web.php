<?php

use Illuminate\Support\Facades\Route;
use App\Models\Vehicle;
use Illuminate\Http\Request;
use PhpMqtt\Client\MqttClient;

// The web routes are only available if the application is in server mode.
if (config('app.server_mode_enabled')) {
    Route::get('/', function () {
        $localIp = getHostByName(getHostName());
        return view('dashboard', [
            'vehicles' => \App\Models\Vehicle::all(), 
            'localIp' => $localIp
        ]);
    });

    Route::post('/vehicle/{id}/command', function (Request $request, $id) {
        $vehicle = Vehicle::findOrFail($id);
        $action = $request->input('action');
        $speed = $request->input('speed', 0.5); // Default speed: 0.5

        // Validate inputs
        if (!in_array($action, ['forward', 'stop', 'turn_left', 'turn_right'])) {
            return response()->json(['error' => 'Invalid action'], 400);
        }

        // Publish MQTT command using centralized config
        $server   = config('mqtt.host');
        $port     = config('mqtt.port');
        $clientId = config('mqtt.client_id') . '_command_' . uniqid();
        $client = new MqttClient($server, $port, $clientId);
        
        $client->connect();
        $payload = json_encode(['action' => $action, 'speed' => (float)$speed]);
        $client->publish("vehicle/$id/control", $payload, 0);
        $client->disconnect();

        return response()->json(['message' => "Command sent to vehicle $id: $payload"]);
    })
    ->withoutMiddleware([Illuminate\Foundation\Http\Middleware\VerifyCsrfToken::class]);
}