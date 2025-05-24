document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();    
    loadConfig();
    
    document.getElementById('config-form').addEventListener('submit', (e) => {
        e.preventDefault();
        saveConfig();
    });
    
    document.getElementById('restart-btn').addEventListener('click', restartDevice);
    document.getElementById('reset-btn').addEventListener('click', resetDevice);
    
    setupConfigToggle();
    requestStatus();
});

function setupConfigToggle() {
    const toggleBtn = document.getElementById('config-toggle');
    const configPanel = document.querySelector('.config-panel');
    const toggleIcon = document.querySelector('.toggle-icon');
    
    if (toggleBtn && configPanel) {
        toggleBtn.addEventListener('click', () => {
            configPanel.classList.toggle('expanded');
            
            if (configPanel.classList.contains('expanded')) {
                toggleIcon.textContent = '-';
            } else {
                toggleIcon.textContent = '+';
            }
        });
    }
}

function updateUI(data) {
    if (data.water_level !== undefined) {
        document.getElementById('water-level').textContent = `${data.water_level.toFixed(2)} m`;
    }
    
    if (data.water_percent !== undefined) {
        document.getElementById('water-percent').textContent = `${(data.water_percent * 100).toFixed(1)}%`;
    }
    
    if (data.measured_distance !== undefined) {
        document.getElementById('raw-measurement').textContent = `${data.measured_distance.toFixed(2)} m`;
    }
    
    if (data.measured_distance !== undefined) {
        const sensorConnected = data.measured_distance > 0;
        document.getElementById('sensor-status').textContent = sensorConnected ? 'Connected' : 'Disconnected';
        document.getElementById('sensor-status').className = sensorConnected ? 'stat-value connected' : 'stat-value disconnected';
    }
    
    if (data.mqtt_connected !== undefined) {
        document.getElementById('mqtt-status').textContent = data.mqtt_connected ? 'Connected' : 'Disconnected';
        document.getElementById('mqtt-status').className = data.mqtt_connected ? 'stat-value connected' : 'stat-value disconnected';
    }
    
    if (data.wifi_network !== undefined) {
        document.getElementById('wifi-network').textContent = data.wifi_network;
    }
    
    if (data.wifi_signal !== undefined) {
        document.getElementById('wifi-signal').textContent = `${data.wifi_signal} dBm`;
    }
    
    if (data.ip_address !== undefined) {
        document.getElementById('ip-address').textContent = data.ip_address;
    }
    
    if (data.uptime !== undefined) {
        document.getElementById('uptime').textContent = data.uptime;
    }
    
    if (data.water_percent !== undefined) {
        const fillElement = document.getElementById('water-fill');
        fillElement.style.height = `${data.water_percent * 100}%`;
        
        fillElement.classList.remove('low', 'medium', 'high');
        if (data.water_percent < 0.25) {
            fillElement.classList.add('low');
        } else if (data.water_percent < 0.75) {
            fillElement.classList.add('medium');
        } else {
            fillElement.classList.add('high');
        }
    }
}

function addLogMessage(message, autoScroll = false) {
    const logContainer = document.getElementById('log-container');
    const logEntry = document.createElement('div');
    logEntry.className = 'log-entry';
    
    if (!message.includes('[20') && !message.includes('[System]')) {
        const timestamp = new Date().toLocaleTimeString();
        logEntry.textContent = `[${timestamp}] ${message}`;
    } else {
        logEntry.textContent = message;
    }
    
    logContainer.appendChild(logEntry);
    
    if (autoScroll) {
        logContainer.scrollTop = logContainer.scrollHeight;
    }
    
    if (logContainer.children.length > 100) {
        logContainer.removeChild(logContainer.children[0]);
    }
}
