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

// Wrapper for addText() to display to the Status Panel
function statusMsg(text) {
    addText('statusText', text);
}

function formatDate(dt) {
    var toReturn = "";

    toReturn += dt.date.slice(0, 4);
    toReturn += '/';
    toReturn += dt.date.slice(4, 6);
    toReturn += '/';
    toReturn += dt.date.slice(6);

    return toReturn;
}

function formatTime(dt) {
    var toReturn = "";

    toReturn += dt.time.slice(0, 2);
    toReturn += ':';
    toReturn += dt.time.slice(2, 4);
    toReturn += ':';
    toReturn += dt.time.slice(4);

    if (dt.isUTC) {
        toReturn += ' (UTC)';
    }

    return toReturn;
}

// Adds a new row to the Event List table, given an Event object retrieved from a JSON
function addEventToTable(evt) {
    var markup = "<tr><td><input type='radio' name='eventSelect'></td><td>" + formatDate(evt.startDT) + "</td><td>"
                 + formatTime(evt.startDT) + "</td><td>" + evt.summary + "</td><td>" + evt.numProps + "</td><td>"
                 + evt.numAlarms + "</td></tr>";

    $('#eventTable').append(markup);
}

// Adds a new row to the File Log Panel table, given a Calendar JSON
function addCalendarToTable(cal) {
    var markup = "<tr><td><a href='/uploads/" + file.name + "'>" + file.name + "</a></td><td>"
                 + calendar.version + "</td><td>" + calendar.prodID + "</td><td>"
                 + calendar.numProps + "</td><td>" + calendar.numEvents + "</td></tr>";

    $('#fileLogBody').append(markup);
}

function getFormData(form) {
    var formDataArray = form.serializeArray();
    var formData = {};

    $.map(formDataArray, function(key, val) {
        formData[key['name']] = key['value'];
    });

    return formData;
}

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() { 
    /******************************
     * Load all files in /uploads *
     ******************************/
    $.ajax({
        type: 'get',
        url: '/uploadsContents',
        success: function(files) {
            for (var filepath of files) {
                // TODO read in each file
                statusMsg(filepath);
            }
        }, fail: function(error) {
            statusMsg('Encountered an error while loading saved .ics files: ' + error);
        }
    });


    /***********************************
     * Open/Close Modal Event Handlers *
     ***********************************/
    
    // Add Event to Calendar
    $('#addEventButton').click(function() {
        $('#addEventModal').css("display", "block");
    });
    $('#closeModalEvent').click(function() {
        $('#addEventModal').css("display", "none");
        $('#eventForm').trigger('reset');
    });

    // Create New Calendar
    $('#createCalendarButton').click(function() {
        $('#createCalendarModal').css("display", "block");
    });
    $('#closeModalCalendar').click(function() {
        $('#createCalendarModal').css("display", "none");
        $('#calendarForm').trigger('reset');
    });

    // Show Alarms For Selected Event
    $('#showAlarmsButton').click(function() {
        $('#viewAlarmsModal').css('display', 'block');
    });
    $('#closeModalAlarms').click(function() {
        $('#viewAlarmsModal').css('display', 'none');
    });

    // FOR TESTING ONLY: Add random Alarm
    $('#testAlarmTable').click(function() {
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getFakeAlarm',
            success: function(alarm) {
                // Add alarm to table
                var markup = "<tr><td>" + alarm.action + "</td><td>" + alarm.trigger + "</td><td>"
                             + "<table width='100%'><thead><tr><th width='25%'>Prop Name</th><th>Prop Descr</th></tr></thead><tbody>";

                for (var i = 0; i < alarm.properties.length; i++) {
                    var prop = alarm.properties[i];
                    markup += "<tr><td>" + prop.propName + "</td><td>" + prop.propDescr + "</td></tr>";
                }
                markup += "</tbody></table></tr>";

                $('#eventAlarmBody').append(markup);

                statusMsg('Added "' + JSON.stringify(alarm) + '" to Alarm Event modal');
            },
            fail: function(error) {
                console.log(error);
            }
        });

    });


    // Show Properties For Selected Event
    $('#showPropertiesButton').click(function() {
        $('#viewPropertiesModal').css('display', 'block');
    });
    $('#closeModalProperties').click(function() {
        $('#viewPropertiesModal').css('display', 'none');
    });

    // FOR TESTING ONLY: Add random Property
    $('#testPropertyTable').click(function() {
        $.ajax({
            url: '/getFakeProperty',
            type: 'get',
            dataType: 'json',
            success: function(property) {
                // Add Property to table
                var markup = "<tr><td>" + property.propName + "</td><td>" + property.propDescr + "</td></tr>"

                $('#eventPropertyBody').append(markup);
                statusMsg('Added "' + JSON.stringify(property) + '" to the Property Event table');
            },
            fail: function(error) {
                console.log(error);
            }
        });
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
            type: 'post',
            data: new FormData($('#uploadForm')[0]),
            cache: false,
            contentType: false,
            processData: false,
            // Simply putting xhr: $.ajaxSettings.xhr() does not work, so I have to do this function nonsense
            xhr: function() {
                return $.ajaxSettings.xhr();
            },
            success: function(file) {
                // Removed due to the need to check if it is a re-upload of a file
                // already in the system. This is now printed in the nested AJAX call below.
                //statusMsg('Successfully uploaded file "' + file.name + '"');


                // Add the newly uploaded file to the File Log Panel after getting its JSON,
                // and to the Calendar File list to see its Events (and be able to add an Event to it)
                $.ajax({
                    type: 'get',
                    dataType: 'json',
                    url: '/getFakeCal',  // TODO replace with the real "get calendar JSON" endpoint
                    success: function(calendar) {
                        // First, determine if this is a re-upload of a file with the same name
                        var prevRow = $('#fileLogBody td').filter(function() {
                            return $(this).text() == file.name;
                        }).closest('tr');

                        // Used to add the file to a table in a new row
                        var markup = "<tr><td><a href='/uploads/" + file.name + "'>" + file.name + "</a></td><td>"
                                     + calendar.version + "</td><td>" + calendar.prodID + "</td><td>"
                                     + calendar.numProps + "</td><td>" + calendar.numEvents + "</td></tr>";

                        // Used to add the file to the 'Calendar File to View:' <select> element
                        var option = new Option(file.name, file.name);
                        $(option).data('obj', calendar);


                        if (prevRow.length !== 0) {
                            // This is a re-upload, which must be overwritten in both the File Log Panel and the Calendar View Panel
                            statusMsg('Re-uploading the file "' + file.name + '"');

                            // Replace row in File Log Panel
                            prevRow.replaceWith(markup);

                            // Replace entry in the 'Calendar File to View:' <select> element
                            var prevOption = $('#fileSelector option').filter(function() {
                                return $(this).text() == file.name;
                            });
                            prevOption.replaceWith(option);
                        } else {
                            // The file is not yet in the system, so it can be added normally
                            statusMsg('Successfully uploaded the file "' + file.name + '"');

                            // Add calendar to the File Log Panel
                            $('#fileLogBody').append(markup);

                            // Add calendar to the Calendar File List
                            /*
                            $('#fileSelector').append($('<option>', {
                                value: file.name,
                                text: file.name
                            }).data('obj', calendar));
                            */
                            $('#fileSelector').append(option);
                        }
 
                        // Select the newly uploaded file in the 'Calendar File to View:' <select> element
                        $('#fileSelector option[value="' + file.name + '"]').attr('selected', 'selected');
                        $('#fileSelector').change();
                    },
                    fail: function(error) {
                        statusMsg('Encountered error when retrieving Calendar JSON: ' + error);
                    }
                });
            },
            fail: function(error) {
                statusMsg('Encountered error when attempting to upload a file: ' + error);
            }
        });
    });


    // Event listener for the <select..> tag where users can choose which Calendar in the
    // File Log Panel to view the Events of
    $('#fileSelector').change(function() {
        // First, empty the Event Table body
        $('#eventTable tbody').empty();

        // Now add all the Events from the currently selected Calendar to the (now empty) Event Table tbody
        var selected = $(this).find(':selected');
        for (var i = 0; i < selected.data('obj').events.length; i++) {
            var ev = selected.data('obj').events[i];
            addEventToTable(ev);
        }
    });

    /*****************************************************************
     * AJAX Callbacks for Creating New Calendar and Adding New Event *
     *****************************************************************/

    // Submit the Calendar created in the Create New Calendar modal
    $('#submitCalendar').click(function(e) {
        e.preventDefault();

        var formData = getFormData($('#calendarForm'));
        statusMsg('From calendarForm, got form data: "' + JSON.stringify(formData) + '"');

        // TODO construct Calendar JSON and write it to /uploads with an AJAX call
        // to the C backend

        $('#closeModalCalendar').click();
    });

    // Submit the Event created in the Add Event To Calendar modal
    $('#submitEvent').click(function(e) {
        e.preventDefault();

        var formData = getFormData($('#eventForm'));
        statusMsg('From eventForm, got form data: "' + JSON.stringify(formData) + '"');

        // TODO construct Event JSON and use addEvent() from the C backend to add it
        // to the Calendar that was selected in the Calendar View Panel

        $('#closeModalEvent').click();
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
                statusMsg('Received DT-string JSON: ' + JSON.stringify(data));
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    // AJAX Stub : Fake Event
    /*
    $('#showPropertiesButton').click(function(e) {
        e.preventDefault();

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getFakeEvent',   //The server endpoint we are connecting to
            success: function (data) {
                statusMsg('Received event-string JSON: "' + JSON.stringify(data) + '"');
                //We write the object to the console to show that the request was successful
                console.log(data);

                addEventToTable(data);
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });
    });
    */ 

    // AJAX Stub : Fake Alarm
    /*
    $('#showAlarmsButton').click(function(e) {
        e.preventDefault();

            });
    */ 

    // AJAX Stub : Fake Calendar
    $('#fakeCalButton').click(function(e) {
        e.preventDefault();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getFakeCal',
            success: function(data) {
                statusMsg('Received Cal-string JSON: ' + JSON.stringify(data));
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
});
