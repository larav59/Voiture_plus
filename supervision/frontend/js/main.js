/************* IMPORTS *************/
import $ from 'jquery';

import * as bootstrap from 'bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

import L from 'leaflet';
import 'leaflet/dist/leaflet.css';


/************* DONNÉES GLOBALES (à remplacer par un appel API) *************/

var COLORS = ['green', 'blue', 'yellow', 'red', 'orange'];

var VEHICLE_DATA = [
    {
        id: 1,
        name: 'Voiture 1',
        travel: {
            id: 1,
            created_at: '2025-10-23T17:45:30',
            status: 'Terminé',
            estimated_duration: 2,
            nodes : [
                {
                    id: 1,
                    name: 'Parking 1',
                    position_x: 550,
                    position_y: 240,
                    point_of_interest: 1,
                },
                {
                    id: 2,
                    name: 'Parking 2',
                    position_x: 220,
                    position_y: 700,
                    point_of_interest: 1,
                }
            ]
        },
        state: {
            id: 15,
            speed: 35,
            angle: 50,
            position_x: 1200,
            position_y: 700
        }
    },
    {
        id: 2,
        name: 'Voiture 2',
        travel: {
            id: 2,
            created_at: '2025-10-23T17:50:30',
            status: 'En cours',
            estimated_duration: 0,
            nodes : [
                {
                    id: 2,
                    name: 'Parking 2',
                    position_x: 925,
                    position_y: 800,
                    point_of_interest: 1,
                },
                {
                    id: 3,
                    name: 'Gare',
                    position_x: 970,
                    position_y: 165,
                    point_of_interest: 1,
                }
            ]
        },
        state: {
            id: 15,
            speed: 35,
            angle: 50,
            position_x: 600,
            position_y: 170
        }
    },
];

var MAP_DATA = [
    {
        id: 1,
        name: 'Parking 1',
        position_x: 1400,
        position_y: 885,
        point_of_interest: 1
    },
    {
        id: 2,
        name: 'Parking 2',
        position_x: 220,
        position_y: 685,
        point_of_interest: 1
    },
    {
        id: 3,
        name: 'Gare',
        position_x: 970,
        position_y: 110,
        point_of_interest: 1
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
        const bounds = [[0, 0], [img.height, img.width]];
        
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
        map.setZoom(-0.5);

        const mapNodes = setupMapNodes(map, MAP_DATA);
        console.log("Points fixes chargés:", mapNodes);

        const vehicles = setupVehicles(map, VEHICLE_DATA, mapNodes.circles);
        console.log("Carte initialisée avec", vehicles.length, "véhicules");
    }
});

/************* FONCTIONS UTILITAIRES *************/

var mapUtils = {
    /**
     * Crée une icône Leaflet selon le type demandé
     * @param {number} id - Identifiant de l'objet
     * @param {string} type - Type d'objet ('car', 'pin', 'map' ou autre pour point)
     * @param {number|null} vehicleId - Id du véhicule pour la couleur (optionnel)
     * @returns {L.Icon} Icône Leaflet configurée
     */
    createIcon: function(id, type, vehicleId = null) {

        let iconConfig = {};
        switch (type) {
            case 'car':
                iconConfig.url = 'images/car/car_'+COLORS[id]+'.png';
                iconConfig.size = [70, 40];
                break;

            case 'pin':
                iconConfig.url = 'images/pin/pin_'+COLORS[vehicleId]+'.png';
                iconConfig.size = [40, 45];
                break;

            case 'map':
                iconConfig.url = 'images/destinations/'+id+'.png';
                iconConfig.size = [40, 45];
                break;
        
            default:
                iconConfig.url = 'images/point/point_'+COLORS[vehicleId]+'.png';
                iconConfig.size = [25, 25];
                break;
        }

        return L.icon({
            iconUrl: iconConfig.url,
            iconSize: iconConfig.size,
            iconAnchor: [20, 20],
            popupAnchor: [0, -20]
        });
    },
    /**
     * Crée un marqueur Leaflet avec popup
     * @param {Array<number>} position - Position [latitude, longitude]
     * @param {L.Icon} icon - Icône du marqueur
     * @param {string} popupText - Texte du popup
     * @param {boolean} draggable - Le marqueur est-il déplaçable (défaut: false)
     * @returns {L.Marker} Marqueur Leaflet configuré
     */
    createMarker: function(position, icon, popupText, draggable) {
        return L.marker(position, { 
                    icon: icon, 
                    draggable: draggable || false 
                }).bindPopup(popupText);
    },
    /**
     * Configure un listener pour empêcher un déplacement en dehors des cercles autorisés
     * @param {L.Marker} pinMarker - Le marqueur draggable
     * @param {Array<L.Circle>} circles - Tableau de cercles Leaflet autorisés
     * @param {string} name - Nom affiché pour les logs
     */
    setupNodeListener: function(pinMarker, circles, name) {
        let lastValidPos = pinMarker.getLatLng();

        pinMarker.on('moveend', function(e) {
            const newPos = e.target.getLatLng();
            let isInsideAnyCircle = false;

            // Vérifie la position par rapport à chaque cercle
            for (let circle of circles) {
                const center = circle.getLatLng();
                const radius = circle.getRadius();

                const dx = newPos.lng - center.lng;
                const dy = newPos.lat - center.lat;
                const distance = Math.sqrt(dx * dx + dy * dy);

                if (distance <= radius) {
                    isInsideAnyCircle = true;
                    break;
                }
            }

            if (isInsideAnyCircle) {
                console.log(`${name} position valide :`, newPos.lat, newPos.lng);
                lastValidPos = newPos;
            } else {
                console.warn(`${name} déplacé hors de toutes les zones autorisées, retour à la dernière position.`);
                pinMarker.setLatLng(lastValidPos);
            }
        });
    },
    /**
     * Crée un filtre contenant tous les marqueurs d'un véhicule (voiture + noeuds du trajet)
     * @param {Object} vehicle - véhicule avec propriétés id, name, state, travel
     * @returns {Object} Objet avec layer (L.LayerGroup) et name (string)
     */
    createVehicleLayer: function (vehicle, circles) {

        let markers = [];

        // Voitures
        const carIcon = mapUtils.createIcon(vehicle.id, 'car');
        const carMarker = mapUtils.createMarker(
            [vehicle.state.position_y, vehicle.state.position_x],
            carIcon, 
            vehicle.name
        );
        markers.push(carMarker);

        // Noeuds du trajet
        vehicle.travel.nodes.forEach(function(node) {
            
            const isFinaleDestination = vehicle.travel.nodes.at(-1).id === node.id;
            const type = isFinaleDestination ? 'pin' : 'point';

            const nodeIcon = mapUtils.createIcon(node.id, type, vehicle.id);

            const nodeMarker = mapUtils.createMarker(
                [node.position_y, node.position_x],
                nodeIcon, 
                'Destination '+vehicle.name,
                true // draggable
            );        
            markers.push(nodeMarker);

            mapUtils.setupNodeListener(nodeMarker, circles, node.name);
        });
        
        return {
            layer: L.layerGroup(markers),
            name: vehicle.name
        };
    },
    /**
     * Crée et ajoute un cercle rouge sur la carte autour de la position d’un noeud.
     * @param {L.Map} map - Instance Leaflet de la carte
     * @param {Object} node - Objet représentant le noeud.
     * @returns {L.Circle} Cercle Leaflet ajouté à la carte
     **/
    setupMapNodeArea: function (map, node) {
        
        const circle = L.circle([node.position_y, node.position_x], {
            color: 'red',
            fillColor: '#f03',
            fillOpacity: 0.3,
            radius: 80 // rayon du cercle
        }).addTo(map);

        return circle;
    }
};

/************* SETUP PRINCIPAL *************/

function setupVehicles(map, vehicleData, circles) {

    // Ajout d'un filtre pour le véhicule sur la carte
    const layerControl = L.control.layers(null, null, { 
        position: 'topright' 
    }).addTo(map);
    
    let vehicleLayers = [];
    
    vehicleData.forEach(function(vehicle) {
        // Map
        const vehicleLayer = mapUtils.createVehicleLayer(vehicle, circles);
        vehicleLayer.layer.addTo(map);
        layerControl.addOverlay(vehicleLayer.layer, vehicleLayer.name);
        vehicleLayers.push(vehicleLayer);

        // Formulaire
        let statusColor = '';
        switch (vehicle.travel.status) {
            case 'En attente':
                statusColor = '#e7dd55';
                break;
            case 'En cours':
                statusColor = '#7cd1f5';
                break;
            case 'Terminé':
                statusColor = '#9ae249';
                break;
            case 'Annulé':
                statusColor = '#f03030';
                break;
            default:
                statusColor = vehicle.status;
                break;
        }

        const option = '<li class="dropdown-item" value="'+ vehicle.id +'">'+
                            vehicle.name +'<span class="ms-3 badge rounded-pill" style="background-color:'+statusColor+'">'+ vehicle.travel.status +'</span>' +
                        '</li>';

        $('#vehicleSelect').append(option);
    });
    $('#vehicleSelect').find('.dropdown-item:last').trigger('click');
    
    return vehicleLayers;
}

function setupMapNodes(map, mapData) {
    let markers = [];
    let circles = [];
    
    mapData.forEach(function(node) {
        const icon = mapUtils.createIcon(node.id, 'map');
        const marker = mapUtils.createMarker(
            [node.position_y, node.position_x],
            icon,
            node.name,
            false
        );
        marker.addTo(map);
        markers.push(marker);

        const circle = mapUtils.setupMapNodeArea(map, node);
        circles.push(circle);

        const option = '<li class="dropdown-item" value="'+ node.id +'">'+
                            '<img class="me-2" src="images/destinations/'+node.id+'.png" width="20" height="20"/>' + node.name +
                        '</li>';

        $('#destinationSelect, #stepClonable .step-select').append(option);
    });
    $('#destinationSelect, #stepClonable .step-select').find('.dropdown-item:last').trigger('click');
    
    return {'markers':markers, 'circles': circles};
}

/************* INTERACTIONS *************/

/*
* Mise à jour du bouton de sélection de véhicule au click d'une option
*/
$(document).on('click', '.dropdown-item', function() {
    const selectBtn =  $(this).closest('.dropdown').find('.dropdown-toggle');
    selectBtn.html($(this).html());
});

/*
* Ajout d'une étape dans un trajet
*/
$(document).on('click', '#addStepBtn', function() {

    const maxStep = 1;
    let stepCount = $('.step-item').length - 1; //step clonable
    if (stepCount >= maxStep) return;

    const step = stepCount+1;
    
    const stepItem= $('#stepClonable').clone().removeAttr('id').removeClass('d-none');
    $('#stepsContainer').append(stepItem);
    
    if (step == maxStep)
        $('#addStepBtn').prop('disabled', true);
});

/*
* Suppression d'une étape d'un trajet
*/
$(document).on('click', '.remove-step', function() {
    $(this).closest('.step-item').remove();
    $('#addStepBtn').prop('disabled', false);
});
