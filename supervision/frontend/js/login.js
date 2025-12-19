/************* IMPORTS *************/
import $ from 'jquery';

import * as bootstrap from 'bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';


/*
* Connexion de l'utilisateur
*/
$(document).on('click', '#connexionBtn', function() {

    $('#loginIncorrect').hide();

    const username = $('#username').val();
    const password = $('#password').val();

    $.ajax({
        type: 'POST',
        url: window.API_URL+'/login',
        dataType: 'json',
        contentType: 'application/json',
        data: JSON.stringify({
            'username' : username,
            'password' : password
        }),
        success: function(data){

            if(data.token) {
                sessionStorage.setItem('API_KEY', data.token);
                window.location.href = 'main.html';
            }
            else {
                $('#loginIncorrect').show();
            }
        },
        error: function(e) {
            console.error(e);
            $('#loginIncorrect').show();
        }
    });
});