const API_BASE = '/api/v1';

async function loadConfig() {
    try {
        const response = await fetch(`${API_BASE}/config`);
        if (!response.ok) throw new Error('Failed to fetch configuration');
        
        const config = await response.json();
        
        document.getElementById('sensor-range').value = config.sensor_range || '';
        document.getElementById('distance-empty').value = config.distance_empty || '';
        document.getElementById('distance-full').value = config.distance_full || '';
        document.getElementById('avg-sample-count').value = config.avg_sample_count || '';
        document.getElementById('sampling-interval').value = config.sampling_interval || '';
        document.getElementById('max-distance-delta').value = config.max_distance_delta || '';
        document.getElementById('mqtt-host').value = config.mqtt_host || '';
        document.getElementById('mqtt-port').value = config.mqtt_port || '';
        document.getElementById('mqtt-user').value = config.mqtt_user || '';
        document.getElementById('mqtt-pass').value = config.mqtt_pass || '';
        document.getElementById('mqtt-device').value = config.mqtt_device || '';
        
        updateMqttTopic(config.mqtt_device || '');
        
        addLogMessage('Configuration loaded successfully');
    } catch (error) {
        addLogMessage(`Error loading configuration: ${error.message}`);
    }
}

function updateMqttTopic(deviceName) {
    const topicElement = document.getElementById('mqtt-topic');
    if (deviceName) {
        topicElement.value = `${deviceName}/stat/distance`;
    } else {
        topicElement.value = '';
    }
}

document.addEventListener('DOMContentLoaded', () => {
    const deviceInput = document.getElementById('mqtt-device');
    if (deviceInput) {
        deviceInput.addEventListener('input', (e) => {
            updateMqttTopic(e.target.value);
        });
    }
});

async function saveConfig() {
    try {
        const form = document.getElementById('config-form');
        const formData = new FormData(form);
        const config = {};
        
        for (const [key, value] of formData.entries()) {
            config[key] = value;
        }
        
        const response = await fetch(`${API_BASE}/config`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(config)
        });
        
        if (!response.ok) throw new Error('Failed to save configuration');
        
        addLogMessage('Configuration saved successfully');
        
        if (confirm('Configuration saved. Restart device to apply changes?')) {
            restartDevice();
        }
    } catch (error) {
        addLogMessage(`Error saving configuration: ${error.message}`);
    }
}

async function requestStatus() {
    try {
        const response = await fetch(`${API_BASE}/status`);
        if (!response.ok) throw new Error('Failed to fetch status');
        
        const status = await response.json();
        updateUI(status);
    } catch (error) {
        console.error(`Error updating status: ${error.message}`);
    }
}

async function restartDevice() {
    if (confirm('Are you sure you want to restart the device?')) {
        try {
            const response = await fetch(`${API_BASE}/restart`, {
                method: 'POST'
            });
            
            if (!response.ok) throw new Error('Failed to restart device');
            
            addLogMessage('Device is restarting...');
            
            setTimeout(() => {
                window.location.reload();
            }, 10000);
        } catch (error) {
            addLogMessage(`Error restarting device: ${error.message}`);
        }
    }
}

async function resetDevice() {
    if (confirm('WARNING: This will reset all settings to defaults. Continue?')) {
        try {
            const response = await fetch(`${API_BASE}/reset`, {
                method: 'POST'
            });
            
            if (!response.ok) throw new Error('Failed to reset device');
            
            addLogMessage('Device has been reset and is restarting...');
            
            setTimeout(() => {
                window.location.href = "http://192.168.4.1";
            }, 5000);
        } catch (error) {
            addLogMessage(`Error resetting device: ${error.message}`);
        }
    }
}
