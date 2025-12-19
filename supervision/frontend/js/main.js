/************* IMPORTS *************/
import $ from 'jquery';

import * as bootstrap from 'bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

import L from 'leaflet';
import 'leaflet/dist/leaflet.css';

/************* DONNÉES GLOBALES (à remplacer par un appel API) *************/

var COLORS = ['green', 'blue', 'yellow', 'red', 'orange', 'green'];

var VEHICLE_DATA = [
    {
        id: 1,
        name: 'Voiture 1',
        travel: {
            id: 1,
            createdAt: '2025-10-23T17:45:30',
            status: 'Terminé',
            estimatedDuration: 2,
            nodes : [
                {
                    id: 1,
                    name: 'Parking 1',
                    positionX: 4350,
                    positionY: 3290,
                    pointOfInterest: 1
                },
                {
                    id: 2,
                    name: 'Parking 2',
                    positionX: 610,
                    positionY: 2530,
                    pointOfInterest: 1
                }
            ]
        },
        state: {
            id: 15,
            speed: 35,
            angle: 180,
            positionX: 3000,
            positionY: 2550,
        }
    },
    {
        id: 2,
        name: 'Voiture 2',
        travel: {
            id: 2,
            createdAt: '2025-10-23T17:50:30',
            status: 'En cours',
            estimatedDuration: 0,
            nodes : [
                {
                    id: 2,
                    name: 'Parking 2',
                    positionX: 610,
                    positionY: 2530,
                    pointOfInterest: 1
                },
                {
                    id: 3,
                    name: 'Gare',
                    positionX: 3290,
                    positionY: 590,
                    pointOfInterest: 1
                }
            ]
        },
        state: {
            id: 15,
            speed: 35,
            angle: 0,
            positionX: 2290,
            positionY: 700
        }
    },
];

var MAP_DATA = [
    {
        id: 1,
        name: 'Parking 1',
        positionX: 4350,
        positionY: 3290,
        pointOfInterest: 1
    },
    {
        id: 2,
        name: 'Parking 2',
        positionX: 610,
        positionY: 2530,
        pointOfInterest: 1
    },
    {
        id: 3,
        name: 'Gare',
        positionX: 3290,
        positionY: 590,
        pointOfInterest: 1
    }
];

$(document).ready(function () {

    const params = {
        imageUrl: "images/plan.png",
        minZoom: -2,
        maxZoom: 2,
        zoomDelta: 0.1,
        zoomSnap: 0.1,
        wheelPxPerZoomLevel: 300,
        refreshTime : 1000  // 1000 ms = 1 seconde
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

        const mapData = getMapData();
        const mapNodes = setupMapNodes(map, mapData);

        /*** Refresh de la map avec les véhicules ***/

        let previousData, vehiclesData = null;
        let isFirstRefresh = true;

        setInterval(function() {
            vehiclesData = getVehiclesData();

            if (JSON.stringify(vehiclesData) !== JSON.stringify(previousData)) {

                setupVehicles(map, vehiclesData, mapNodes.circles);
                previousData = vehiclesData;

                if(isFirstRefresh) {
                    $('#vehicleSelect').find('.dropdown-item:last').trigger('click');
                    isFirstRefresh = false;
                }
            }
        }, params.refreshTime);
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
     * * @param {boolean} popupPermanent - La popup est elle affihcée en permanence (défaut: false)
     * @returns {L.Marker} Marqueur Leaflet configuré
     */
    createMarker: function(position, icon, popupText, draggable = false, popupPermanent = false) {
        var marker = L.marker(position, { 
            icon: icon, 
            draggable: draggable
        });
        
        if (popupPermanent) {
            marker.bindTooltip(popupText, {
                permanent: true,
                direction: 'top',
                className: 'custom-tooltip',
                offset: [10, -20]
            });
        }
        else marker.bindPopup(popupText);
        
        return marker;
    },
    /**
     * Génère le contenu HTML du tooltip d'un véhicule
     * @param {Object} vehicle - Objet véhicule
     * @returns {string} HTML du tooltip
     */
    generateVehicleTooltip :function(vehicle) {

        let statusColor = '#777777';
        let travel = vehicle.travel;

        if(travel) {
            switch (vehicle.travel.status) {
                case 'En attente':
                    statusColor = '#e7dd55';
                    break;
                case 'En cours':
                    statusColor = '#7cd1f5';
                    break;
                case 'Terminé':
                    statusColor = '#a0e249ff';
                    break;
                case 'Annulé':
                    statusColor = '#f03030';
                    break;
                default:
                    statusColor = '#777777';
                    break;
            }
        }

        return `
            <div class="card text-center">
                <div class="card-header py-1 px-2">` +
                    vehicle.name + `</span><span class="ms-2 pt-1 badge rounded-pill" style="background-color:`+statusColor+`">`+ vehicle.travel?.status +`</span>
                </div>
                <div class="card-body px-2 py-1">
                    <div>
                        <img class="me-1" src="images/speed.png" width="20" height="13"/>` + 
                        vehicle.state.speed +` cm/s
                    </div>
                    <div class="mt-1">
                        <img class="me-1" src="images/angle.png" width="15" height="13"/>` +
                        vehicle.state.angle +`°
                    </div>
                </div>
            </div>`;
    },
    /**
     * Crée un filtre contenant tous les marqueurs d'un véhicule (voiture + noeuds du trajet)
     * @param {Object} vehicle - véhicule avec propriétés id, name, state, travel
     * @param {L.Map} map - Instance Leaflet de la carte
     * @returns {Object} Objet avec layer (L.LayerGroup) et name (string)
     */
    createVehicleLayer: function (vehicle, map, circles) {

        let markers = [];
        if(!vehicle.state) return;

        const position = mapUtils.realToLeaflet(vehicle.state.positionX, vehicle.state.positionY);

        // Flèche (direction et angle du véhicule)
        var arrowIcon = L.divIcon({
            html: '<div style="transform: rotate(' + vehicle.state.angle + 'deg);"> → </div>',
            className: 'arrow-marker',
            iconSize: [30, 30],
            iconAnchor: [0, 10]
        });
        var arrowMarker = L.marker(
            position,
            {icon: arrowIcon}
        );
        markers.push(arrowMarker);

        // Véhicule (petite icone de voiture)
        const carIcon = mapUtils.createIcon(vehicle.id, 'car');
        const carMarker = mapUtils.createMarker(
            position,
            carIcon, 
            mapUtils.generateVehicleTooltip(vehicle),
            false,
            true // permanent
        );
        markers.push(carMarker);
        mapUtils.setupClickListener(carMarker, vehicle.id);

        // Noeuds du trajet
        if(vehicle.travel) {
            vehicle.travel.nodes.forEach(function(node) {
                
                const isFinaleDestination = vehicle.travel.nodes.at(-1).id === node.id;
                const type = isFinaleDestination ? 'pin' : 'point';

                const position = mapUtils.realToLeaflet(node.positionX, node.positionY);
                const nodeIcon = mapUtils.createIcon(node.id, type, vehicle.id);

                let nodeMarker = mapUtils.createMarker(
                    position,
                    nodeIcon, 
                    'Destination '+vehicle.name,
                    true // draggable
                );
                nodeMarker.type = type;
                nodeMarker.id = node.id;
                nodeMarker.vehicleId = vehicle.id;
        
                markers.push(nodeMarker);

                mapUtils.setupNodeDropListener(nodeMarker, map, circles, node.name);
                mapUtils.setupClickListener(nodeMarker, vehicle.id);
            });
        }
        
        return {
            layer: L.layerGroup(markers),
            name: vehicle.name
        };
    },
    /**
     * Crée et ajoute un cercle rouge sur la carte autour de la position d’un noeud.
     * @param {Object} node - Objet représentant le noeud.
     * @returns {L.Circle} Cercle Leaflet ajouté à la carte
     **/
    setupMapNodeArea: function (node) {

        const position = mapUtils.realToLeaflet(node.positionX, node.positionY);
        
        const circle = L.circle(position, {
            color: 'red',
            fillColor: '#f03',
            fillOpacity: 0.3,
            radius: 80 // rayon du cercle
        });
        circle.id = node.id;

        return circle;
    },
    /**
     * Configure un listener pour mettre à jour le formulaire au click d'un véhicule
     * @param {L.Marker} carMarker - Le marqueur de voiture
     * @param {number} id - véhicule id
     */
    setupClickListener: function(carMarker, id) {

        carMarker.on('click', function() {
            selectVehicle(id);
        });
    },
    /**
     * Configure un listener pour empêcher un déplacement en dehors des cercles autorisés et mettre à jour le trajet
     * @param {L.Marker} pinMarker - Le marqueur draggable
     * @param {L.Map} map - Instance Leaflet de la carte
     * @param {Array<L.Circle>} circles - Tableau de cercles Leaflet autorisés
     * @param {string} name - Nom affiché pour les logs
     */
    setupNodeDropListener: function(pinMarker, map, circles, name) {
        let lastValidPos = pinMarker.getLatLng();

        // Drag d'un noeud
        pinMarker.on('movestart', function() {
            circles.forEach(circle => circle.addTo(map)); // Afficher les cercles au début du drag
            selectVehicle(pinMarker.vehicleId);
        });

        // Drop d'un noeud
        pinMarker.on('moveend', function(e) {
            circles.forEach(circle => circle.remove()); // Cacher les cercles au drop 

            const newPos = e.target.getLatLng();
            let destination = null;

            // Vérifie la position par rapport à chaque cercle
            for (let circle of circles) {
                const center = circle.getLatLng();
                const radius = circle.getRadius();

                const dx = newPos.lng - center.lng;
                const dy = newPos.lat - center.lat;
                const distance = Math.sqrt(dx * dx + dy * dy);

                if (distance <= radius) {
                    destination = circle;
                    break;
                }
            }

            //Modification du trajet
            if (destination != null) {
                console.log(`${name} position valide :`, newPos.lat, newPos.lng);
                lastValidPos = newPos;

                selectVehicle(pinMarker.vehicleId);

                if(pinMarker.type == 'pin') {
                    selectOption('destinationSelect', destination.id);
                }
                else {
                    const stepToUpdate = $('#stepsContainer .step-item').filter(function() {
                        return $(this).data('id') == pinMarker.id;
                    });

                    stepToUpdate.find('.dropdown-item[value='+destination.id+']').trigger('click');
                }
            } else {
                console.warn(`${name} déplacé hors de toutes les zones autorisées, retour à la dernière position.`);
                pinMarker.setLatLng(lastValidPos);
            }
        });
    },
    /**
     * Convertit des coordonnées réelles (mm) en coordonnées Leaflet (pixels)
     * @param {number} realX - Position x réelle en mm
     * @param {number} realY - Position y réelle en mm
     * @returns {Array} [y, x]
     */
    realToLeaflet: function(realX, realY) {

        const scale = {
            base : {  // Point de référence (coin inférieur gauche parking 2)
                real :{ x: 420, y: 2310 },
                leaflet : { x: 158.58967, y: 638.19456 }
            },
            ratio : {  // pixels par mm
                x: 0.3151964908675799,
                y: 0.2866093559322034
            },
            offset : { // marvelmind décalées par rapport à la maquette + marges du plan
                x: 100,
                y: -190
            }
        }

        const x = (realX + scale.offset.x) * scale.ratio.x;
        const y = (realY + scale.offset.y) * scale.ratio.y;

        return [y, x];
    }
};

/************* SETUP PRINCIPAL *************/

function setupVehicles(map, vehicleData, circles) {

    // Ajout d'un filtre pour le véhicule sur la carte
    const layerControl = L.control.layers(null, null, { 
        position: 'topright' 
    }).addTo(map);
    
    $('#vehicleSelect').empty();
    let vehicleLayers = [];
    
    vehicleData.forEach(function(vehicle) {
        // Map
        const vehicleLayer = mapUtils.createVehicleLayer(vehicle, map, circles);
        if(!vehicleLayer) return;

        vehicleLayer.layer.addTo(map);
        layerControl.addOverlay(vehicleLayer.layer, vehicleLayer.name);
        vehicleLayers.push(vehicleLayer);

        // Formulaire
        let statusColor = '';
        let travel = vehicle.travel;

        if(travel) {
            switch (vehicle.travel.status) {
                case 'En attente':
                    statusColor = '#e7dd55';
                    break;
                case 'En cours':
                    statusColor = '#7cd1f5';
                    break;
                case 'Terminé':
                    statusColor = '#a0e249ff';
                    break;
                case 'Annulé':
                    statusColor = '#f03030';
                    break;
                default:
                    statusColor = '#777777';
                    break;
            }
        }

        const option = $('<li>').addClass('dropdown-item')
                                .val(vehicle.id)
                                .html(vehicle.name +'<span class="ms-3 badge rounded-pill" style="background-color:'+statusColor+'">'+ vehicle.travel?.status +'</span>')
                                .data('vehicle', vehicle);

        $('#vehicleSelect').append(option);
    });
    
    return vehicleLayers;
}

function setupMapNodes(map, mapData) {
    let markers = [];
    let circles = [];
    
    mapData.forEach(function(node) {
        const position = mapUtils.realToLeaflet(node.positionX, node.positionY);
        const icon = mapUtils.createIcon(node.id, 'map');

        const marker = mapUtils.createMarker(
            position,
            icon,
            node.name
        );
        marker.addTo(map);
        markers.push(marker);

        const circle = mapUtils.setupMapNodeArea(node);
        circles.push(circle);

        const option = '<li class="dropdown-item" value="'+ node.id +'">'+
                            '<img class="me-2" src="images/destinations/'+node.id+'.png" width="20" height="20"/>' + node.name +
                        '</li>';

        $('#destinationSelect, #stepClonable .step-select').append(option);
    });
    $('#destinationSelect, #stepClonable .step-select').find('.dropdown-item:last').trigger('click');
    
    return {'markers': markers, 'circles': circles};
}


function getMapData() {

    let mapData = null;

    $.ajax({
        type: 'GET',
        url: window.API_URL+'/nodes?isPointOfInterest=true',
        async : false,
        headers: {
            'Authorization': 'Bearer '+sessionStorage.getItem('API_KEY'),
        },
        success: function(data){
            mapData = data;
        },
        error: function(e) {
            console.error(e);
        }
    });

    //mapData = MAP_DATA;
    return mapData;
}

function getVehiclesData() {

    let vehiclesData = null;

    $.ajax({
        type: 'GET',
        url: window.API_URL+'/vehicles',
        async : false,
        headers: {
            'Authorization': 'Bearer '+sessionStorage.getItem('API_KEY'),
        },
        success: function(data){
            vehiclesData = data;
        },
        error: function(e) {
            console.error(e);
        }
    });

    //vehiclesData = VEHICLE_DATA;
    console.log(vehiclesData);
    return vehiclesData;
}

/************* INTERACTIONS *************/

/*
* Mise à jour du formulaire au click d'un véhicule
*/
$(document).on('click', '#vehicleSelect .dropdown-item', function() {

    const vehicle = $(this).data('vehicle');
    if(!vehicle || !vehicle.travel) return;

    const destinations = vehicle.travel.nodes.filter(function(node) { return node.pointOfInterest; });

    $('#stepsContainer').empty();
    $('#addStepBtn').prop('disabled', false);

    for(const node of destinations){
        if(destinations.at(-1).id === node.id) { // Destination finale
            selectOption('destinationSelect', node.id);
        }
        else { // Etape
            $('#addStepBtn').trigger('click');
            $('.step-select:last').find('.dropdown-item[value='+node.id+']').trigger('click');
        }
    }
    $('#travel').data('id', vehicle.travel.id);

    if(vehicle.travel.status == 'Terminé' || vehicle.travel.status == 'Annulé'){
        $('#calculTravelBtn').show();
        $('#cancelTravelBtn').hide();
    }
    else {
        $('#calculTravelBtn').hide();
        $('#cancelTravelBtn').show();
    }
});

/*
* Click sur une option d'un select (dropdown)
*/
function selectOption(select, value){
    $('#'+select+' .dropdown-item[value='+value+']').trigger('click');
}

/*
* Mise à jour du formulaire en sélectionnant un véhicule en particulier
*/
function selectVehicle(id) {
    selectOption('vehicleSelect', id);
}

/*
* Mise à jour du bouton de sélection (dropdown) au click d'une option
*/
$(document).on('click', '.dropdown-item', function() {
    const selectBtn =  $(this).closest('.dropdown').find('.dropdown-toggle');
    selectBtn.html($(this).html());
    selectBtn.data('id', $(this).val());

    $(this).closest('.step-item').data('id', $(this).val());
});

/*
* Ajout d'une étape dans un trajet
*/
$(document).on('click', '#addStepBtn', function() {

    const maxStep = 2;
    let stepCount = $('#stepsContainer').find('.step-item').length;
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

/*
* Sauvegarde d'un trajet
*/
$(document).on('click', "#calculTravelBtn", function() {
    //envoie post trajet

    const vehicleId = $('#vehicleSelectBtn').data('id');
    if (!vehicleId) {
        alert('Veuillez sélectionner un véhicule');
        return;
    }

    const nodes = [];
    let order = 1;

    // Étapes intermédiaires
    $('#stepsContainer .step-item').each(function () {
        const nodeId = $(this).data('id');
        if (!nodeId) return;

        nodes.push({
            id: nodeId,
            order: order++
        });
    });

    // Destination finale
    const destinationId = $('#destinationSelectBtn').data('id');

    if (destinationId) {
        nodes.push({
            id: destinationId,
            order: order
        });
    } else {
        alert('Veuillez sélectionner une destination finale');
        return;
    }

    const payload = {
        vehicle: vehicleId,
        nodes: nodes
    };

    console.log(payload);

    // Envoi API
    $.ajax({
        type: 'POST',
        url: window.API_URL + '/travels',
        contentType: 'application/json',
        data: JSON.stringify(payload),
        headers: {
            'Authorization': 'Bearer ' + sessionStorage.getItem('API_KEY')
        },
        success: function (res) {
            console.log('trajet créé avec succès');
            location.reload();
        },
        error: function (e) {
            console.error(e);
        }
    });
});

/*
* Annulation d'un trajet
*/
$(document).on('click', "#cancelTravelBtn", function() {

    const travelId = $('#travel').data('id');

    $.ajax({
        type: 'PUT',
        url: window.API_URL + '/travels/' + travelId,
        contentType: 'application/json',
        data: JSON.stringify({
            status: 'Annulé'
        }),
        headers: {
            'Authorization': 'Bearer ' + sessionStorage.getItem('API_KEY')
        },
        success: function (res) {
            console.log('trajet annulé avec succès');
            location.reload();
        },
        error: function (e) {
            console.error(e);
        }
    });
});

/*
* Logout de l'utilisateur
*/
$(document).on('click', '#logout', function() {
    sessionStorage.removeItem('API_KEY');
    window.location.href = 'login.html';
});