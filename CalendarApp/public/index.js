// Clears the text of an element
function clearText(id) {
    $('#' + id).val("");
}

// Adds text to a textarea
function addText(id, message) {
    var textarea = $('#' + id)
    var currentText = textarea.val();
    if (currentText === "") {
        textarea.val(message);
    } else {
        textarea.val(currentText + "\n" + message);
    }
}

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
            addText('statusText', 'On page load, received string "' + JSON.stringify(data) + '" from server');
            //$('#blah').html("On page load, Received string '"+JSON.stringify(data)+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
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

    // Event listener for the <input type='file'...> element
    // The function executes after the user hits 'Browse' and selects a file
    $('#uploadFile').on('change', function() {
        var file = this.files[0];
        // If the file does not end with .ics, it is invalid
        if (!file.name.endsWith('.ics')) {
            alert("File uploads must be iCalendar files, which have the .ics file extension.\n" +
                  "Please choose a different file.");
            $('#uploadForm').trigger('reset'); // resets the entire uploading process
        }
    });

    // Event listener for the 'Upload' button for uploading files
    $('#uploadButton').click(function(e) {
        // Submitting an HTML form has a default action of redirecting to another page.
        // This statement overrides that lets us make an AJAX request and do other things
        // if we want.
        e.preventDefault();

        // AJAX request
        $.ajax({
            url: '/upload',
            type: 'POST',
            data: new FormData($('#uploadForm')[0]),
            cache: false,
            contentType: false,
            processData: false,
            dataType: "json",
            // Simply putting xhr: $.ajaxSettings.xhr() does not work, so I have to do this function nonsense
            xhr: function() {
                var myXhr = $.ajaxSettings.xhr();
                return myXhr;
            },
            success: function(fileJSON) {
                addText('statusText', 'Successfully uploaded file "' + fileJSON.name + '"');
            },
            fail: function(error) {
                addText('statusText', 'Encountered error when attempting to upload a file: ' + error);
            }
        });
    });
});
