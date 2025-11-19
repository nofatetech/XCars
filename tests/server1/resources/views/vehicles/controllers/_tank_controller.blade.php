<div style="background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 10px; padding: 20px; width: 600px; margin: auto; box-shadow: 0 4px 8px rgba(0,0,0,0.1);">
    <table style="width: 100%;">
        <tr>
            <td colspan="2">
                <div style="border: 1px solid #ccc; background-color: #fff; min-height: 240px; text-align: center; padding: 10px;">
                    Camera Feed
                </div>
            </td>
        </tr>
        <tr>
            <td style="width: 50%; vertical-align: top;">
                <table style="width: 100%;">
                    <caption>Motor Controls</caption>
                    <tr>
                        <td>Left Motor</td>
                        <td><input type="range" min="-100" max="100" value="0" class="motor-slider" data-motor="left"></td>
                    </tr>
                    <tr>
                        <td>Right Motor</td>
                        <td><input type="range" min="-100" max="100" value="0" class="motor-slider" data-motor="right"></td>
                    </tr>
                </table>
                <table style="width: 100%; margin-top: 10px;">
                    <caption>Light Controls</caption>
                    <tr>
                        <td><button class="light-button" data-light="headlights">Headlights</button></td>
                        <td><button class="light-button" data-light="high_beams">High Beams</button></td>
                    </tr>
                    <tr>
                        <td><button class="light-button" data-light="fog_lights">Fog Lights</button></td>
                        <td></td>
                    </tr>
                    <tr>
                        <td><button class="light-button" data-light="left_blinker">Left Blinker</button></td>
                        <td><button class="light-button" data-light="right_blinker">Right Blinker</button></td>
                    </tr>
                </table>
            </td>
            <td style="width: 50%; vertical-align: top; padding-left: 20px;">
                <div style="border: 1px solid #ccc; background-color: #fff; padding: 10px; border-radius: 5px;">
                    <h4>Status</h4>
                    <ul style="list-style-type: none; padding: 0;">
                        <li><strong>Battery:</strong> <span id="battery-level-{{ $vehicle->id }}">N/A</span></li>
                        <li><strong>Status:</strong> <span id="status-{{ $vehicle->id }}">{{ $vehicle->status }}</span></li>
                        <li><strong>Wi-Fi:</strong> <span id="wifi-level-{{ $vehicle->id }}">N/A</span></li>
                        <!-- Add other vehicle data here -->
                    </ul>
                    <button class="settings-button" data-vehicle-id="{{ $vehicle->id }}">Settings</button>
                </div>
            </td>
        </tr>
        <tr>
            <td style="vertical-align: top; padding-top: 10px;">
                <div style="border: 1px solid #ccc; background-color: #fff; padding: 10px; border-radius: 5px;">
                    <h4>AI Camera</h4>
                    <ul id="ai-detected-objects-{{ $vehicle->id }}" style="list-style-type: none; padding: 0;">
                        <li>-</li>
                    </ul>
                </div>
            </td>
            <td style="vertical-align: top; padding-left: 20px; padding-top: 10px;">
                <div style="border: 1px solid #ccc; background-color: #fff; padding: 10px; border-radius: 5px;">
                    <h4>Personality</h4>
                    <ul style="list-style-type: none; padding: 0;">
                        <li><strong>Energy:</strong> <span id="energy-{{ $vehicle->id }}">{{ $vehicle->energy ?? 'N/A' }}</span></li>
                        <li><strong>Happiness:</strong> <span id="happiness-{{ $vehicle->id }}">{{ $vehicle->happiness ?? 'N/A' }}</span></li>
                    </ul>
                </div>
            </td>
        </tr>
    </table>
</div>
