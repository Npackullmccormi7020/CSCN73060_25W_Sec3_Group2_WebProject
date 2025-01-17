const apiUrl = 'http://localhost:23500/appointments';

// Navigate Home
document.querySelector('#home-button').addEventListener('click', () => {
    window.location.href = '/';
});

// Handle search form submission
document.querySelector('#search-appointment-form').addEventListener('submit', async (event) => {
    event.preventDefault();

    const searchMethod = document.querySelector('#search-method').value;
    const searchValue = document.querySelector('#search-input').value.trim();

    if (!searchValue) {
        alert('Please enter a search value.');
        return;
    }

    try {
        let response;

        if (searchMethod === 'id') {
            response = await fetch(`${apiUrl}/${encodeURIComponent(searchValue)}`);
        } else if (searchMethod === 'last_name') {
            response = await fetch(`${apiUrl}?last_name=${encodeURIComponent(searchValue)}`);
        }

        if (response.ok) {
            const data = await response.json();

            if (Array.isArray(data) && data.length > 0) {
                const appointment = data[0];
                populateAppointmentDetails(appointment);
            } else if (!Array.isArray(data)) {
                populateAppointmentDetails(data);
            } else {
                alert('No appointments found.');
                hideAppointmentDetails();
            }
        } else {
            alert('Appointment not found.');
            hideAppointmentDetails();
        }
    } catch (error) {
        console.error('Error fetching appointment:', error);
        alert('An error occurred while fetching the appointment details.');
        hideAppointmentDetails();
    }
});

// Populate Appointment Details
function populateAppointmentDetails(appointment) {
    document.querySelector('#appointment-id').textContent = appointment.id || 'N/A';
    document.querySelector('#appointment-first-name').textContent = appointment.first_name || 'N/A';
    document.querySelector('#appointment-last-name').textContent = appointment.last_name || 'N/A';
    document.querySelector('#appointment-service').textContent = appointment.service || 'N/A';
    document.querySelector('#appointment-date').textContent = appointment.date || 'N/A';
    document.querySelector('#appointment-time').textContent = appointment.time || 'N/A';

    document.querySelector('#appointment-details').style.display = 'block';
    document.querySelector('#update-appointment-form').style.display = 'none';
}

// Hide Appointment Details
function hideAppointmentDetails() {
    document.querySelector('#appointment-details').style.display = 'none';
    document.querySelector('#update-appointment-form').style.display = 'none';
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
    const appointmentId = document.querySelector('#appointment-id').textContent;

    const updateData = {
        service: document.querySelector('#update-service').value,
        date: document.querySelector('#update-date').value,
        time: document.querySelector('#update-time').value,
    };

    try {
        const response = await fetch(`${apiUrl}/${appointmentId}`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(updateData),
        });

        if (response.ok) {
            alert('Appointment updated successfully!');
            document.querySelector('#update-appointment-form').style.display = 'none';
            document.querySelector('#appointment-details').style.display = 'none';
        } else {
            alert('Failed to update appointment.');
        }
    } catch (error) {
        console.error('Error updating appointment:', error);
        alert('An error occurred while updating the appointment.');
    }
});

// Delete an appointment
document.querySelector('#delete-button').addEventListener('click', async () => {
    const appointmentId = document.querySelector('#appointment-id').textContent;
    if (!confirm('Are you sure you want to delete this appointment?')) return;

    try {
        const response = await fetch(`${apiUrl}/${appointmentId}`, {
            method: 'DELETE',
        });

        if (response.ok) {
            alert('Appointment deleted successfully!');
            document.querySelector('#appointment-details').style.display = 'none';
        } else {
            alert('Failed to delete appointment.');
        }
    } catch (error) {
        console.error('Error deleting appointment:', error);
        alert('An error occurred while deleting the appointment.');
    }
});
