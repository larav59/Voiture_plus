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
        dataType: 'json',
        url: window.API_URL+'/login',
        data: {
            'username' : username,
            'password' : password
        },
        success: function(data){

            if(data.hash) {
                sessionStorage.setItem('API_KEY', hash);
                window.location.href = 'main.html';
            }
            else {
                $('#loginIncorrect').show();
            }
        },
        error: function(e) {
            console.error(e);
        }
    });
});