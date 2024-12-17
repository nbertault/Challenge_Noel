$(document).ready(function () {
    if (window.WebSocket) { // l'API WebSocket est-elle installee ?
        var socket;
        $('#connect').on('click', function () {
            socket = new WebSocket("ws://localhost:12359");
            socket.onopen = function () {
                console.log("Connecté au serveur WebSocket.");
                $('#connect').prop('disabled', true);
                $('#disconnect, #send').prop('disabled', false);
            };
            socket.onmessage = function (event) {
                console.log("Message reçu :", event.data);
                $('#receivedMessages').append("Message reçu : " + event.data + "\n");
            };
            socket.onerror = function (error) {
                console.error("Erreur WebSocket :", error);
            };
            socket.onclose = function () {
                console.log("Déconnecté du serveur WebSocket.");
                $('#connect').prop('disabled', false);
                $('#disconnect, #send').prop('disabled', true);
            };
        });
        $('#disconnect').on('click', function () {
            if (socket) {
                socket.close();
            }
        });
        $('#send').on('click', function () {
            const message = $('#messageToSend').val();
            if (socket && socket.readyState === WebSocket.OPEN) {
                socket.send(message);
                console.log("Message envoyé :", message);
            } else {
                alert("La connexion WebSocket n'est pas ouverte.");
            }
        });
    } else
        console.log('L\'API WebSocket n\'est pas supportée par ce navigateur.');
});