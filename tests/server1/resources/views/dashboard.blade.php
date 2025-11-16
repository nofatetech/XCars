<ul>
    @foreach($vehicles as $vehicle)
        <li>{{ $vehicle->name }}: {{ $vehicle->status }}</li>
    @endforeach
</ul>