const apiUrl = 'http://localhost:23500/appointments';

// Navigate Home
document.querySelector('#home-button').addEventListener('click', () => {
    window.location.href = '/';
});

// Search for an appointment
document.querySelector('#search-appointment-form').addEventListener('submit', async (event) => {
    event.preventDefault();

    const searchMethod = document.querySelector('#search-method').value;
    const searchValue = document.querySelector('#search-input').value.trim();

    if (!searchValue) {
        alert('Please enter a search value.');
        return;
    }

    let response;
    try {
        if (searchMethod === 'id') {
            response = await fetch(`${apiUrl}/${searchValue}`);
        } else if (searchMethod === 'last_name') {
            response = await fetch(`${apiUrl}?last_name=${encodeURIComponent(searchValue)}`);
        }

        const responseText = await response.text();
        console.log("Search Response:", response.status, responseText);

        if (response.ok) {
            const data = JSON.parse(responseText);
            
            if (Array.isArray(data) && data.length === 0) {
                alert('No appointments found.');
                return;
            }

            const appointment = Array.isArray(data) ? data[0] : data;
            populateAppointmentDetails(appointment);

            document.querySelector('#appointment-details').style.display = 'block';
            document.querySelector('#update-appointment-form').style.display = 'none';
        } else {
            alert('Appointment not found.');
        }
    } catch (error) {
        console.error('Error fetching appointments:', error);
        alert('An error occurred while fetching appointment details.');
    }
});

// Populate Appointment Details
function populateAppointmentDetails(appointment) {
    document.querySelector('#appointment-id').textContent = appointment.id;
    document.querySelector('#appointment-first-name').textContent = appointment.first_name;
    document.querySelector('#appointment-last-name').textContent = appointment.last_name;
    document.querySelector('#appointment-service').textContent = appointment.service;
    document.querySelector('#appointment-date').textContent = appointment.date;
    document.querySelector('#appointment-time').textContent = appointment.time;
}

// Show update form with pre-filled data
document.querySelector('#update-button').addEventListener('click', () => {
    document.querySelector('#update-service').value = document.querySelector('#appointment-service').textContent;
    document.querySelector('#update-date').value = document.querySelector('#appointment-date').textContent;
    document.querySelector('#update-time').value = document.querySelector('#appointment-time').textContent;

    document.querySelector('#update-appointment-form').style.display = 'block';
});

// Update an appointment
document.querySelector('#update-appointment-form').addEventListener('submit', async (event) => {
    event.preventDefault();
    const appointmentId = document.querySelector('#appointment-id').textContent.trim();

    if (!appointmentId) {
        alert("Error: Appointment ID is missing.");
        return;
    }

    const updateData = {
        service: document.querySelector('#update-service').value.trim(),
        date: document.querySelector('#update-date').value.trim(),
        time: document.querySelector('#update-time').value.trim(),
    };

    console.log("Sending update request:", updateData);

    try {
        const response = await fetch(`${apiUrl}/${appointmentId}`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(updateData),
        });

        const responseText = await response.text();
        console.log("Update Response:", response.status, responseText);

        if (response.ok) {
            alert('Appointment updated successfully!');
            document.querySelector('#update-appointment-form').style.display = 'none';
            document.querySelector('#appointment-details').style.display = 'none';
        } else {
            alert('Failed to update appointment: ' + responseText);
        }
    } catch (error) {
        console.error('Error updating appointment:', error);
        alert('An error occurred while updating the appointment.');
    }
});

// Delete an appointment
document.querySelector('#delete-button').addEventListener('click', async () => {
    const appointmentId = document.querySelector('#appointment-id').textContent.trim();
    if (!appointmentId) {
        alert("Error: Appointment ID is missing.");
        return;
    }

    if (!confirm('Are you sure you want to delete this appointment?')) return;

    try {
        const response = await fetch(`${apiUrl}/${appointmentId}`, {
            method: 'DELETE',
        });

        const responseText = await response.text();
        console.log("Delete Response:", response.status, responseText);

        if (response.ok) {
            alert('Appointment deleted successfully!');
            document.querySelector('#appointment-details').style.display = 'none';
            document.querySelector('#update-appointment-form').style.display = 'none';
        } else {
            alert('Failed to delete appointment: ' + responseText);
        }
    } catch (error) {
        console.error('Error deleting appointment:', error);
        alert('An error occurred while deleting the appointment.');
    }
});
