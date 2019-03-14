// Clears the text of an element
function clearText(id) {
    $('#' + id).val("");
}

// Adds text to a textarea
function addText(id, message) {
    var textarea = $('#' + id)
    var currentText = textarea.val();

    // Determine how text should be appended (note: .append() method from jquery doesn't work for this)
    // https://stackoverflow.com/a/4723017
    if (currentText === "") {
        textarea.val(message);
    } else {
        // Append text
        textarea.val(currentText + "\n" + message);

        // Scroll to the bottom
        var height1 = textarea[0].scrollHeight;
        var height2 = textarea.height();
        textarea.scrollTop(height1 - height2);
    }
}

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
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
            // Simply putting xhr: $.ajaxSettings.xhr() does not work, so I have to do this function nonsense
            xhr: function() {
                return $.ajaxSettings.xhr();
            },
            success: function(file) {
                addText('statusText', 'Successfully uploaded file "' + file.name + '"');
                // TODO add file to file log panel
            },
            fail: function(error) {
                addText('statusText', 'Encountered error when attempting to upload a file: ' + error);
            }
        });
    });


    /*******************************************
     * AJAX Callbacks for Calendar data JSON's *
     *******************************************/

    // AJAX Stub : Fake DateTime
    $('#fakeDTbutton').click(function(e) {
        e.preventDefault();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getFakeDT',
            success: function(data) {
                addText('statusText', 'Received DT-string: ' + JSON.stringify(data));
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    // AJAX Stub : Fake Event
    $('#showPropertiesButton').click(function(e) {
        e.preventDefault();

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getFakeEvent',   //The server endpoint we are connecting to
            success: function (data) {
                addText('statusText', 'Received event-string "' + JSON.stringify(data) + '"');
                //We write the object to the console to show that the request was successful
                console.log(data); 
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });
    });

    // AJAX Stub : Fake Alarm
    $('#showAlarmsButton').click(function(e) {
        e.preventDefault();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getFakeAlarm',
            success: function(data) {
                addText('statusText', 'Received alarm-string: ' + JSON.stringify(data));
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    // AJAX Stub : Fake Calendar
    $('#fakeCalButton').click(function(e) {
        e.preventDefault();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getFakeCal',
            success: function(data) {
                addText('statusText', 'Received Cal-string: ' + JSON.stringify(data));
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
});
