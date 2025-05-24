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
    // Sanitize data in one place - create a clean object with default values
    const cleanData = {
        water_level: isValidNumber(data.water_level) ? data.water_level.toFixed(2) + ' m' : 'N/A',
        water_percent: isValidNumber(data.water_percent) ? (data.water_percent * 100).toFixed(1) + '%' : 'N/A',
        measured_distance: isValidNumber(data.measured_distance) ? data.measured_distance.toFixed(2) + ' m' : 'N/A',
        sensor_connected: data.sensor_connected !== undefined ? data.sensor_connected : false,
        mqtt_connected: data.mqtt_connected !== undefined ? data.mqtt_connected : false,
        wifi_network: data.wifi_network || 'Unknown',
        wifi_signal: data.wifi_signal ? data.wifi_signal + ' dBm' : 'N/A',
        ip_address: data.ip_address || 'Unknown',
        uptime: data.uptime || 'N/A',
        water_percent_value: isValidNumber(data.water_percent) ? data.water_percent : 0
    };
    
    // Update all UI elements with clean data
    document.getElementById('water-level').textContent = cleanData.water_level;
    document.getElementById('water-percent').textContent = cleanData.water_percent;
    document.getElementById('raw-measurement').textContent = cleanData.measured_distance;
    
    // Handle connection status indicators
    document.getElementById('sensor-status').textContent = cleanData.sensor_connected ? 'Connected' : 'Disconnected';
    document.getElementById('sensor-status').className = cleanData.sensor_connected ? 'stat-value connected' : 'stat-value disconnected';
    
    document.getElementById('mqtt-status').textContent = cleanData.mqtt_connected ? 'Connected' : 'Disconnected';
    document.getElementById('mqtt-status').className = cleanData.mqtt_connected ? 'stat-value connected' : 'stat-value disconnected';
    
    // Update network information
    document.getElementById('wifi-network').textContent = cleanData.wifi_network;
    document.getElementById('wifi-signal').textContent = cleanData.wifi_signal;
    document.getElementById('ip-address').textContent = cleanData.ip_address;
    document.getElementById('uptime').textContent = cleanData.uptime;
    
    // Update water fill visualization
    const fillElement = document.getElementById('water-fill');
    if (fillElement) {
        fillElement.style.height = `${cleanData.water_percent_value * 100}%`;
        
        fillElement.classList.remove('low', 'medium', 'high');
        if (cleanData.water_percent_value < 0.25) {
            fillElement.classList.add('low');
        } else if (cleanData.water_percent_value < 0.75) {
            fillElement.classList.add('medium');
        } else {
            fillElement.classList.add('high');
        }
    }
}

// Helper function to check if a value is a valid number
function isValidNumber(value) {
    return value !== undefined && value !== null && !isNaN(value);
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
