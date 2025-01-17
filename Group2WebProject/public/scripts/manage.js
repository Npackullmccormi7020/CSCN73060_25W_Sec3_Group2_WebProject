const apiUrl = 'http://localhost:23500/appointments';

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

        // Fetch data based on search method
        if (searchMethod === 'id') {
            response = await fetch(`${apiUrl}/${encodeURIComponent(searchValue)}`);
        } else if (searchMethod === 'last_name') {
            response = await fetch(`${apiUrl}?last_name=${encodeURIComponent(searchValue)}`);
        }

        // Check if the response is OK
        if (response.ok) {
            const data = await response.json();

            if (Array.isArray(data) && data.length > 0) {
                // Handle multiple results (take the first one for simplicity)
                const appointment = data[0];
                updateAppointmentDetails(appointment);
            } else if (!Array.isArray(data)) {
                // Handle a single result
                updateAppointmentDetails(data);
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

// Function to update the DOM with appointment details
function updateAppointmentDetails(appointment) {
    document.querySelector('#appointment-id').textContent = appointment.id || 'N/A';
    document.querySelector('#appointment-first-name').textContent = appointment.first_name || 'N/A';
    document.querySelector('#appointment-last-name').textContent = appointment.last_name || 'N/A';
    document.querySelector('#appointment-service').textContent = appointment.service || 'N/A';
    document.querySelector('#appointment-date').textContent = appointment.date || 'N/A';
    document.querySelector('#appointment-time').textContent = appointment.time || 'N/A';

    document.querySelector('#appointment-details').style.display = 'block';
}

// Function to hide the appointment details
function hideAppointmentDetails() {
    document.querySelector('#appointment-details').style.display = 'none';
}
