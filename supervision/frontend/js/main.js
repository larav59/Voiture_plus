/************* IMPORTS *************/
import $ from 'jquery';
import 'bootstrap/dist/css/bootstrap.min.css';

import L from 'leaflet';
import 'leaflet/dist/leaflet.css';


/************* DONNÉES GLOBALES (à remplacer par un appel API) *************/

var VEHICLE_DATA = [
    {
        id: 'car1',
        name: 'Voiture 1',
        carIcon: { url: 'images/car/car_red.png', size: [80, 60] },
        pinIcon: { url: 'images/pin/pin_red.png', size: [40, 45] },
        carPosition: [300, 800],
        destinationPosition: [315, 900],
        color: 'red'
    },
    {
        id: 'car2',
        name: 'Voiture 2',
        carIcon: { url: 'images/car/car_yellow.png', size: [100, 60] },
        pinIcon: { url: 'images/pin/pin_yellow.png', size: [40, 45] },
        carPosition: [850, 1100],
        destinationPosition: [700, 250],
        color: 'yellow'
    }
];

var MAP_DATA = [
    {
        id: 'trainStation',
        name: 'Gare',
        icon: { url: 'images/train_station.png', size: [60, 60] },
        position: [190, 750],
    },
    {
        id: 'carPark1',
        name: 'Parking 1',
        icon: { url: 'images/car_park.png', size: [40, 40] },
        position: [590, 240],
    },
    {
        id: 'carPark2',
        name: 'Parking 2',
        icon: { url: 'images/car_park.png', size: [40, 40] },
        position: [925, 1170],
    }
];

$(document).ready(function () {

    const params = {
        imageUrl: "images/plan.png",
        minZoom: -2,
        maxZoom: 2,
        zoomDelta: 0.1,
        zoomSmap: 0.1,
        wheelPxPerZoomLevel: 300
    };

    var img = new Image();
    img.src = params.imageUrl;
    
    img.onload = function() {
        var bounds = [[0, 0], [img.height, img.width]];
        
        var map = L.map('map', {
            crs: L.CRS.Simple,
            minZoom: params.minZoom,
            maxZoom: params.maxZoom,
            zoomDelta: params.zoomDelta,
            zoomSnap: params.zoomSnap,
            wheelPxPerZoomLevel: params.wheelPxPerZoomLevel,
            attributionControl: false
        });
        
        L.imageOverlay(params.imageUrl, bounds).addTo(map);
        map.fitBounds(bounds);
        map.setZoom(-0.25);

        const vehicles = setupVehicles(map, VEHICLE_DATA);
        console.log("Carte initialisée avec", vehicles.length, "véhicules");

        const mapPoints = setupMapPoints(map, MAP_DATA);
        console.log("Points fixes chargés:", mapPoints.length);
    }
});

/************* FONCTIONS UTILITAIRES *************/

function createIcon(iconConfig) {
    return L.icon({
        iconUrl: iconConfig.url,
        iconSize: iconConfig.size,
        iconAnchor: [20, 20],
        popupAnchor: [0, -20]
    });
}

function createMarker(position, icon, popupText, draggable) {
    return L.marker(position, { 
        icon: icon, 
        draggable: draggable || false 
    }).bindPopup(popupText);
}

function setupPinListener(pinMarker, vehicleId) {
    pinMarker.on('moveend', function(e) {
        var pos = e.target.getLatLng();
        console.log("Nouvelle position " + vehicleId + ":", pos.lat, pos.lng);
    });
}

function createVehicleLayer(vehicleConfig) {
    var carIcon = createIcon(vehicleConfig.carIcon);
    var pinIcon = createIcon(vehicleConfig.pinIcon);
    
    var carMarker = createMarker(
        vehicleConfig.carPosition, 
        carIcon, 
        vehicleConfig.name
    );
    
    var pinMarker = createMarker(
        vehicleConfig.destinationPosition, 
        pinIcon, 
        "Destination " + vehicleConfig.name.toLowerCase(),
        true // draggable
    );
    
    setupPinListener(pinMarker, vehicleConfig.id);
    
    return {
        layer: L.layerGroup([carMarker, pinMarker]),
        name: vehicleConfig.name,
        carMarker: carMarker,
        pinMarker: pinMarker
    };
}

/************* SETUP PRINCIPAL *************/

function setupVehicles(map, vehicleData) {
    var layerControl = L.control.layers(null, null, { 
        position: 'topright' 
    }).addTo(map);
    
    var vehicleLayers = [];
    
    vehicleData.forEach(function(vehicleConfig) {
        var vehicleLayer = createVehicleLayer(vehicleConfig);
        vehicleLayer.layer.addTo(map);
        layerControl.addOverlay(vehicleLayer.layer, vehicleLayer.name);
        vehicleLayers.push(vehicleLayer);

        const option = $('<option>').val(vehicleConfig.id).html(vehicleConfig.name);
        $('#vehicleSelect').append(option);
    });
    
    return vehicleLayers;
}

function setupMapPoints(map, mapData) {
    var markers = [];
    
    mapData.forEach(function(point) {
        var icon = createIcon(point.icon);
        var marker = createMarker(point.position, icon, point.name, false);
        marker.addTo(map);
        markers.push(marker);

        const option = $('<option>').val(point.id).html(point.name);
        $('#destinationSelect, .step-select').append(option);
    });
    
    return markers;
}

/************* INTERACTIONS *************/

$('#addStepBtn').on('click', function() {

    const maxStep = 1;
    let stepCount = $('.step-item').length - 1; //step clonable
    if (stepCount >= maxStep) return;

    const step = stepCount+1;
    
    const stepItem= $('#stepClonable').clone().removeAttr('id').removeClass('d-none');
    $('#stepsContainer').append(stepItem);
    
    if (step == maxStep)
        $('#addStepBtn').prop('disabled', true);
});

$(document).on('click', '.remove-step', function() {
    $(this).closest('.step-item').remove();
    $('#addStepBtn').prop('disabled', false);
});