<?php

use Illuminate\Support\Facades\Route;

Route::get('/', function () {
    // return view('welcome');
    return view('dashboard', ['vehicles' => \App\Models\Vehicle::all()]);
});
