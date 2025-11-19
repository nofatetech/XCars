@extends('layout1')

@section('content')

<div>
    Server IP: 
    <div>{{ $localIp }}</div>
</div>

<ul>
    @foreach($vehicles as $vehicle)
        <li data-vehicle-id="{{ $vehicle->id }}">
            {{ $vehicle->name }}: {{ $vehicle->status }}
            {{-- @include('vehicles.controllers._nes_controller', ['vehicle' => $vehicle]) --}}
            @include('vehicles.controllers._tank_controller', ['vehicle' => $vehicle])

        </li>
    @endforeach
</ul>
<script>
    $(document).ready(function() {
        $('.control-button').on('click', function() {
            const vehicleId = $(this).closest('li').data('vehicle-id');
            const action = $(this).data('action');
            const speed = 0.5; // Default speed, can be made dynamic later

            if (vehicleId && action) {
                $.ajax({
                    url: `/vehicle/${vehicleId}/command`,
                    type: 'POST',
                    data: {
                        action: action,
                        speed: speed,
                        _token: '{{ csrf_token() }}' // Laravel CSRF token
                    },
                    success: function(response) {
                        console.log('Command sent:', response);
                        alert('Command sent: ' + response.message);
                    },
                    error: function(xhr, status, error) {
                        console.error('Error sending command:', error);
                        alert('Error sending command: ' + (xhr.responseJSON ? xhr.responseJSON.error : error));
                    }
                });
            } else {
                console.error('Missing vehicle ID or action.');
            }
        });
    });
</script>
@endsection