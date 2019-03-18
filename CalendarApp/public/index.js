/*********************
 * Utility Functions *
 *********************/

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

// Returns a string that formats the 'date' part of the given DateTime JSON object
// so it can be properly displayed in a table
function formatDate(dt) {
    var toReturn = "";

    toReturn += dt.date.slice(0, 4);
    toReturn += '/';
    toReturn += dt.date.slice(4, 6);
    toReturn += '/';
    toReturn += dt.date.slice(6);

    return toReturn;
}

// Returns a string that formats the 'time' part of the given DateTime JSON object
// so it can be properly displayed in a table
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
    var markup = "<tr><td><input type='radio' name='eventSelect' data-obj='" + JSON.stringify(evt) + "'></td><td>" + formatDate(evt.startDT) + "</td><td>"
                 + formatTime(evt.startDT) + "</td><td>" + evt.summary + "</td><td><b>" + evt.numProps + " (total)</b><br>" + (evt.numProps-3) + " (optional)</td><td>"
                 + evt.numAlarms + "</td></tr>";

    $('#eventTable').append(markup);
}

// Adds a new row to the File Log Panel table, given a Calendar JSON object and a file name string.
// If a row with the same file name exists already in the File Log Panel, it is
// replaced with the new Calendar passed into the function.
function addCalendarToTable(filename, calendar) {
    // First, determine if the first row of the table body contains the 'No files in system' text
    var firstRowElement = $('#fileLogBody tr:eq(0) td').filter(function() {
        return $(this).text() == 'No files in system';
    });

    // If the first row was the special 'table empty' row, remove it.
    if (firstRowElement.length !== 0) {
        firstRowElement.parent().remove();
    }


    // The markup for the new row in the table
    var markup = "<tr><td><a href='/uploads/" + filename + "'>" + filename + "</a></td><td>"
                 + calendar.version + "</td><td>" + calendar.prodID + "</td><td>"
                 + calendar.numProps + "</td><td>" + calendar.numEvents + "</td></tr>";


    // Check for duplicate file re-uploading
    var prevRow = $('#fileLogBody td').filter(function() {
        return $(this).text() == filename;
    }).closest('tr');

    // If a row with the fame filename was found, then it is a re-upload
    if (prevRow.length !== 0) {
        statusMsg('Re-uploading the file "' + filename + '"');

        // Replace row in File Log Panel
        prevRow.replaceWith(markup);
    } else {
        $('#fileLogBody').append(markup);
    }
}

// Adds a new option to the Calendar File Selector, given a Calendar JSON object and a file name string.
// If a row with the same file name exists already in the File Log Panel, it is
// replaced with the new Calendar passed into the function.
function addCalendarToFileSelector(filename, calendar) {
    // The option object that will be added to the file selector
    var option = new Option(filename, filename);
    $(option).data('obj', calendar);

    // Check for duplicate file re-uploading
    var prevOption = $('#fileSelector').filter(function() {
        return $(this).text() == filename;
    });

    // If an option with the same filename was found, then it is a re-upload
    if (prevOption.length !== 0) {
        // Replace entry in the 'Calendar File to View:' <select> element
        prevOption.replaceWith(option);
    } else {
        // Brand new Calendar upload: add it to the file selector
        $('#fileSelector').append(option);
    }

    // Select the new file
    $('#fileSelector option[value="' + filename + '"]').attr('selected', 'selected');
    $('#fileSelector').change();
}

function addAlarmToTable(alarm) {
    // Markup the entire table row, which itself contains a table of properties
    var markup = "<tr><td>" + alarm.action + "</td><td>" + alarm.trigger + "</td><td>"
                 + "<table width='100%'><thead><tr><th width='25%'>Prop Name</th><th>Prop Descr</th></tr></thead><tbody>";

    // Add alarm properties to the inner table
    if (alarm.properties.length === 0) {
        // Alarm contains no additional properties
        markup += "<tr><td></td><td>No properties</td></tr>"
    } else {
        for (var prop of alarm.properties) {
            markup += "<tr><td>" + prop.propName + "</td><td>" + prop.propDescr + "</td></tr>";
        }
    }
    // finishing closing tags for the property table (and ending the row)
    markup += "</tbody></table></tr>";

    $('#eventAlarmBody').append(markup);
}

function addPropertyToTable(prop) {
    // Markup for the row to add
    var markup = "<tr><td>" + prop.propName + "</td><td>" + prop.propDescr + "</tr>"

    $('#eventPropertyBody').append(markup);
}

function getFormData(form) {
    var formDataArray = form.serializeArray();
    var formData = {};

    $.map(formDataArray, function(key, val) {
        formData[key['name']] = key['value'];
    });

    return formData;
}

function loadFile(file) {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getCal/' + file.name,
        success: function(cal) {
            // In this case, 'success' just means the callback itself didn't encounter
            // an error; the function itself could have still failed.
            // Error code JSON's have the format, for example {"error":"Invalid Alarm","filename":"..."}


            // All of this is good, and is staying
            if (cal.error != undefined) {
                statusMsg('Error when trying to create calendar from "' + cal.filename + '": ' + cal.error);

                // On an error, only the File name and Product ID columns are filled,
                // and the Product Id column only says 'invalid file'
                addCalendarToTable(cal.filename, {
                    'version': '',
                    'prodID': 'invalid file',
                    'numProps': '',
                    'numEvents': ''
                });
            } else {
                statusMsg('Loaded "' + cal.filename + '" successfully');
                addCalendarToTable(cal.filename, cal.obj);
                addCalendarToFileSelector(cal.filename, cal.obj);
            }
        },
        fail: function(error) {
            statusMsg(error);
        }
    });
}

// Returns true if all required input fields have been filled, and false otherwise.
// Highlights the border of the input field red if it is both required and empty.
function formHasAllRequired(formID) {
    var allRequired = true;

    $('#' + formID + ' input').each(function() {
        if ($(this).prop('required') && $(this).val() === '') {
            $(this).css('border-color', 'red');
            allRequired = false;
        } else {
            $(this).css('border-color', '');
        }
    });

    return allRequired;
}




/********************************************
 * Onload AJAX calls, Event Listeners, etc. *
 ********************************************/
$(document).ready(function() {

    /******************************
     * Load all files in /uploads *
     ******************************/
    $.ajax({
        type: 'get',
        url: '/uploadsContents',
        dataType: 'json',
        success: function(files) {
            for (var file of files) {
                // Get a JSON of the calendar, and add it to the necessary HTML elements
                loadFile({'name': file});
            }
        }, fail: function(error) {
            statusMsg('Encountered an error while loading saved .ics files: ' + error);
        }
    }); 





    /**************************************************
     * Event Listeners Relating to the File Log Panel *
     **************************************************/

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
                // Errors with the file are handled inside the loadFile() function,
                // so they do not need to be handled here.
                loadFile(file);
            },
            fail: function(error) {
                statusMsg('Encountered error when attempting to upload a file: ' + error);
            }
        });
    });





    /*******************************************************
     * Event Listeners Relating to the Calendar View Panel *
     *******************************************************/

    // Event listener for the <select..> tag where users can choose which Calendar in the
    // File Log Panel to view the Events of
    $('#fileSelector').change(function() {
        // First, empty the Event Table body
        $('#eventTable tbody').empty();

        // Now add all the Events from the currently selected Calendar to the (now empty) Event Table tbody
        var selected = $(this).find(':selected');

        if (selected.data('obj').events.length === 0) {
            var markup = '<tr><td></td><td></td><td></td><td>No Events in this Calendar! Calendars must have at least 1 Event in them</td><td></td><td></td></tr>';
            return;
        }

        for (var i = 0; i < selected.data('obj').events.length; i++) {
            var ev = selected.data('obj').events[i];
            addEventToTable(ev);
        }
    });

    // ========== Modals ==========
    //  - Add Event to existing Calendar
    //  - Create new Calendar
    //  - Show Alarms for selected Event
    //  - Show Properties for selected Event
    
    // Create New Calendar
    $('#createCalendarButton').click(function() {
        $('#createCalendarModal').css("display", "block");
    });
    $('#closeModalCalendar').click(function() {
        $('#createCalendarModal').css("display", "none");
        $('#calendarForm').trigger('reset');
    });

    // Submit the Calendar created in the Create New Calendar modal
    $('#submitCalendar').click(function(e) {
        e.preventDefault();

        if (!formHasAllRequired('calendarForm')) {
            return;
        }

        var formData = getFormData($('#calendarForm'));
        statusMsg('From calendarForm, got form data: "' + JSON.stringify(formData) + '"');

        // TODO construct Calendar JSON and write it to /uploads with an AJAX call
        // to the C backend

        $('#closeModalCalendar').click();
    });


    // Add Event to Existing Calendar
    $('#addEventButton').click(function() {
        $('#addEventModal').css("display", "block");
    });
    $('#closeModalEvent').click(function() {
        $('#addEventModal').css("display", "none");
        $('#eventForm').trigger('reset');
    });

    // Submit the Event created in the Add Event To Calendar modal
    $('#submitEvent').click(function(e) {
        e.preventDefault();

        if (!formHasAllRequired('eventForm')) {
            return;
        }

        var formData = getFormData($('#eventForm'));
        statusMsg('From eventForm, got form data: "' + JSON.stringify(formData) + '"');

        // TODO construct Event JSON and use addEvent() from the C backend to add it
        // to the Calendar that was selected in the Calendar View Panel

        $('#closeModalEvent').click();
    }); 


    // Show Alarms For Selected Event
    $('#showAlarmsButton').click(function() {
        var selectedEvent;

        $('#eventBody tr td input:radio').each(function() {
            if ($(this).prop('checked')) {
                selectedEvent = $(this).data('obj');
            }
        });

        // Add all the alarms to the table in the modal
        if (selectedEvent.alarms.length === 0) {
            var markup = "<tr><td></td><td></td><td>This event doesn't have any alarms</td><tr>";
            $('#eventAlarmBody').append(markup);
        } else {
            for (var alarm of selectedEvent.alarms) {
                addAlarmToTable(alarm);
            }
        }
        $('#viewAlarmsModal').css('display', 'block');
    });
    $('#closeModalAlarms').click(function() {
        $('#viewAlarmsModal').css('display', 'none');
        $('#eventAlarmBody').empty();
    });


    // Show Properties For Selected Event
    $('#showPropertiesButton').click(function() {
        var selectedEvent;

        $('#eventBody tr td input:radio').each(function() {
            if ($(this).prop('checked')) {
                selectedEvent = $(this).data('obj');
            }
        });

        // Add all the properties to the table in the modal
        if (selectedEvent.properties.length === 0) {
            var markup = "<tr><td></td><td>This event doesn't have any optional properties</td><tr>";
            $('#eventPropertyBody').append(markup);
        } else {
            for (var prop of selectedEvent.properties) {
                addPropertyToTable(prop);
            }
        }
        $('#viewPropertiesModal').css('display', 'block');
    });
    $('#closeModalProperties').click(function() {
        $('#viewPropertiesModal').css('display', 'none');
        $('#eventPropertyBody').empty();
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
