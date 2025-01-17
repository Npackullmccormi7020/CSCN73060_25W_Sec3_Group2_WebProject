document.querySelector('#home-button').addEventListener('click', () => {
    window.location.href = '/';
});

document.querySelector('#book-appointment-form').addEventListener('submit', async (event) => {
    event.preventDefault();
    console.log("Form submitted!");

    // Generate a random appointment ID
    const appointmentId = `APPT-${Math.floor(Math.random() * 1000000)}`;
    console.log("Generated Appointment ID:", appointmentId);

    // Gather form data
    const formData = new FormData(event.target);
    const appointment = {
        id: appointmentId,
        first_name: formData.get('first_name'),
        last_name: formData.get('last_name'),
        service: formData.get('service'),
        date: formData.get('date'),
        time: formData.get('time'),
    };

    console.log("Appointment Data:", appointment);

    // Send data to the server
    try {
        const apiUrl = 'http://localhost:23500/appointments';
        const response = await fetch(apiUrl, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(appointment),
        });

        if (response.ok) {
            alert(`Appointment booked successfully! Appointment ID: ${appointmentId}`);
            event.target.reset();
        } else {
            console.error("Failed to book appointment:", await response.text());
            alert('Failed to book appointment.');
        }
    } catch (error) {
        console.error("Network Error:", error);
        alert('Network error occurred.');
    }
});
