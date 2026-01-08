/************* IMPORTS *************/
import $ from 'jquery';

import * as bootstrap from 'bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

/************* DONNÉES GLOBALES (à remplacer par un appel API) *************/

var ALARMS = [
    {
        id: 1,
        createdAt: '2025-09-25 17:45:30',
        description: 'Connexion véhicule 2 perdue',
        alarmType: {
            id: 2,
            label: 'Vehicle connexion lost',
            criticity: 'FATAL'
        },
        origin: {
            id: 1,
            label: 'Heartbeat'
        }
    },
    {
        id: 2,
        createdAt: '2025-09-25 17:45:30',
        description: 'Gare not found',
        alarmType: {
            id: 1,
            label: 'No such route',
            criticity: 'WARNING'
        },
        origin: {
            id: 2,
            label: 'RoutePlanner'
        }
    },
    {
        id: 3,
        createdAt: '2025-09-25 17:45:30',
        description: 'Gare not found',
        alarmType: {
            id: 3,
            label: 'No such route',
            criticity: 'INFO'
        },
        origin: {
            id: 2,
            label: 'RoutePlanner'
        }
    },
    {
        id: 4,
        createdAt: '2025-09-25 17:45:30',
        description: 'Gare not found',
        alarmType: {
            id: 4,
            label: 'No such route',
            criticity: 'DEBUG'
        },
        origin: {
            id: 2,
            label: 'RoutePlanner'
        }
    },
    {
        id: 4,
        createdAt: '2025-09-25 17:45:30',
        description: 'Gare not found',
        alarmType: {
            id: 5,
            label: 'No such route',
            criticity: 'ERROR'
        },
        origin: {
            id: 2,
            label: 'RoutePlanner'
        }
    },
    {
        id: 4,
        createdAt: '2025-09-25 17:45:30',
        description: 'Gare not found',
        alarmType: {
            id: 5,
            label: 'No such route',
            criticity: 'ERROR'
        },
        origin: {
            id: 2,
            label: 'RoutePlanner'
        }
    }
];

$(document).ready(function () {

    /*** Refresh des alertes ***/

    const refreshTime = 1000; //1000 ms = 1 seconde

    let previousAlarms, alarms = null;

    setInterval(function() {
        alarms = getAlarms();

        if (JSON.stringify(alarms) !== JSON.stringify(previousAlarms)) {
            fillAlarms(alarms);
            previousAlarms = alarms;
        }
    }, refreshTime);
});

function getAlarms() {

    let alarms = null;

    $.ajax({
        type: 'GET',
        url: window.API_URL+'/alarms',
        async : false,
        headers: {
            'Authorization': 'Bearer '+sessionStorage.getItem('API_KEY'),
        },
        success: function(data){
            alarms = data;
        },
        error: function(e) {
            console.error(e);
        }
    });

    //alarms = ALARMS;

    return alarms;
}

function fillAlarms(alarms) {

    $('#alarmsContainer').empty();
    console.log(alarms)

    for(const alarm of alarms) {

        let varAlarm = $('#alarmToClone').clone().removeClass('d-none');

        let criticityColor, alertColor = '';
        switch (alarm.alarmType.criticity) {
            case 'ERROR':
            case 'FATAL':
                criticityColor = '#f03030';
                alertColor = 'danger';
                break;
            case 'WARNING':
                criticityColor = '#e7a63fff';
                alertColor = 'warning';
                break;
            case 'INFO':
                criticityColor = '#51b2dcff';
                alertColor = 'info';
                break;
            case 'DEBUG':
                criticityColor = '#7bae39ff';
                alertColor = 'success'
                break;
            default:
                criticityColor = '#777777';
                alertColor = 'secondary';
                break;
        }

        varAlarm.addClass('alert-'+alertColor);
        varAlarm.find('.criticity').html(alarm.alarmType.criticity).css('background-color', criticityColor);

        varAlarm.find('.type').html(alarm.alarmType.label);
        varAlarm.find('.description').html(alarm.description);
        varAlarm.find('.origin').html(alarm.origin.label);
        varAlarm.find('.createdAt').html(alarm.createdAt);

        $('#alarmsContainer').append(varAlarm);
    }
}

/************* INTERACTIONS *************/

/*
* Logout de l'utilisateur
*/
$(document).on('click', '#logout', function() {
    sessionStorage.removeItem('API_KEY');
    window.location.href = 'login.html';
});