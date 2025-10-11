/************* DONNÉES GLOBALES (à remplacer par un appel API) *************/

var VEHICLE_DATA = [
    {
        id: 'car1',
        name: 'Voiture 1',
        carIcon: { url: 'images/car/car_red.png', size: [80, 60] },
        pinIcon: { url: 'images/pin/pin_red.png', size: [40, 45] },
        carPosition: [500, 800],
        destinationPosition: [600, 900],
        color: 'red'
    },
    {
        id: 'car2',
        name: 'Voiture 2',
        carIcon: { url: 'images/car/car_yellow.png', size: [100, 60] },
        pinIcon: { url: 'images/pin/pin_yellow.png', size: [40, 45] },
        carPosition: [1000, 1200],
        destinationPosition: [1100, 1300],
        color: 'yellow'
    }
];

$(document).ready(function () {

    const params = {
        imageUrl: "images/plan.jpg",
        minZoom: -2
    };

    var img = new Image();
    img.src = params.imageUrl;
    
    img.onload = function() {
        var bounds = [[0, 0], [img.height, img.width]];
        
        var map = L.map('map', {
            crs: L.CRS.Simple,
            minZoom: params.minZoom,
            attributionControl: false
        });
        
        L.imageOverlay(params.imageUrl, bounds).addTo(map);
        map.fitBounds(bounds);

        const vehicles = setupVehicles(map, VEHICLE_DATA);
        console.log("Carte initialisée avec", vehicles.length, "véhicules");
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
    });
    
    return vehicleLayers;
}