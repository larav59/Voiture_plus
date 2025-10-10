$(document).ready(function () {
    var img = new Image();
    img.src = "images/plan.jpg";

    img.onload = function() {
        var imageWidth = img.width;
        var imageHeight = img.height;

        var bounds = [[0, 0], [imageHeight, imageWidth]];

        var map = L.map('map', {
            crs: L.CRS.Simple,
            minZoom: -2,
            attributionControl: false
        });

        L.imageOverlay("images/plan.jpg", bounds).addTo(map);
        map.fitBounds(bounds);

        // Définir une icône voiture
        var car1Icon = L.icon({
            iconUrl: 'images/car_red.png',       // chemin vers ton icône
            iconSize: [80, 60],       // taille de l’icône
            iconAnchor: [20, 20],     // point de l'icône qui correspond à la position [x,y]
            popupAnchor: [0, -20]     // position du popup par rapport à l’icône
        });
        var car2Icon = L.icon({
            iconUrl: 'images/car_yellow.png',       // chemin vers ton icône
            iconSize: [100, 60],       // taille de l’icône
            iconAnchor: [20, 20],     // point de l'icône qui correspond à la position [x,y]
            popupAnchor: [0, -20]     // position du popup par rapport à l’icône
        });

        // Créer les marqueurs
        var car1Marker = L.marker([500, 800], { icon: car1Icon }).bindPopup("Voiture 1");
        var car2Marker = L.marker([1000, 1200], { icon: car2Icon }).bindPopup("Voiture 2");

        // Définir une icône destination
        var pin1Icon = L.icon({
            iconUrl: 'images/pin_red.png',       // chemin vers ton icône
            iconSize: [70, 60],       // taille de l’icône
            iconAnchor: [20, 20],     // point de l'icône qui correspond à la position [x,y]
            popupAnchor: [0, -20]     // position du popup par rapport à l’icône
        });
        var pin2Icon = L.icon({
            iconUrl: 'images/pin_yellow.png',       // chemin vers ton icône
            iconSize: [40, 45],       // taille de l’icône
            iconAnchor: [20, 20],     // point de l'icône qui correspond à la position [x,y]
            popupAnchor: [0, -20]     // position du popup par rapport à l’icône
        });
        
        // Créer les marqueurs
        var pin1Marker = L.marker([500, 800], { icon: pin1Icon, draggable: true }).bindPopup("Destination voiture 1");
        var pin2Marker = L.marker([1000, 1200], { icon: pin2Icon, draggable: true }).bindPopup("Destination voiture 2");

        // Créer des LayerGroups
        var car1Layer = L.layerGroup([car1Marker, pin1Marker]);
        var car2Layer = L.layerGroup([car2Marker, pin2Marker]);

        // Ajouter les deux au départ
        car1Layer.addTo(map);
        car2Layer.addTo(map);

        var layerControl = L.control.layers(null, null, { position: 'topright' }).addTo(map);

        layerControl.addOverlay(car1Layer, "Voiture 1");
        layerControl.addOverlay(car2Layer, "Voiture 2");
    };

});

pin1Marker.on('moveend', function(e) {
    var pos = e.target.getLatLng();
    console.log("Nouvelle position pin1:", pos.lat, pos.lng);
});

pin2Marker.on('moveend', function(e) {
    var pos = e.target.getLatLng();
    console.log("Nouvelle position pin2:", pos.lat, pos.lng);
});