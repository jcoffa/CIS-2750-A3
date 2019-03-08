// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/someendpoint',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, Received string '"+JSON.stringify(data)+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#someform').submit(function(e){
        // Dummy output, to show that the form callback is working
        $('#blah').html("Callback from the form");
        e.preventDefault();
        $.ajax({});
    }); 

    // Event handler for modal Add Event
    $('#addEventButton').click(function() {
        $('#addEventModal').css("display", "block");
    });

    // Close modal Add Event
    $('#closeModalEvent').click(function() {
        $('#addEventModal').css("display", "none");
    });

    // Event handler for modal Create Calendar
    $('#createCalendarButton').click(function() {
        $('#createCalendarModal').css("display", "block");
    });

    // Close modal Create Calendar
    $('#closeModalCalendar').click(function() {
        $('#createCalendarModal').css("display", "none");
    });
});

// Clears the text of an element
function clearText(id) {
    $('#' + id).val("");
}

// Adds text to a textarea
function addText(textarea, message) {
    var currentText = textarea.val();
    if (currentText === "") {
        textarea.val(message);
    } else {
        textarea.val(currentText + "\n" + message);
    }
}
