from flask import Flask, request, abort, render_template, redirect, url_for
import time

app = Flask(__name__)

devices = {
    '001': {'mode': 0, 'description': 'Heart of Gold Main Array', 'port': 5000, 'last_seen': None, 'ip': None, 'color': '#00FF2A', 'chaos': 50},
    '002': {'mode': 0, 'description': 'Vogon Constructor Aux', 'port': 5001, 'last_seen': None, 'ip': None, 'color': '#00FF2A', 'chaos': 50},
    '003': {'mode': 0, 'description': 'Guide Cockpit Glow', 'port': 5002, 'last_seen': None, 'ip': None, 'color': '#00FF2A', 'chaos': 50},
    '004': {'mode': 0, 'description': 'Improbability Engine', 'port': 5003, 'last_seen': None, 'ip': None, 'color': '#00FF2A', 'chaos': 50},
    '005': {'mode': 0, 'description': 'Zaphod’s Cabin Mood', 'port': 5004, 'last_seen': None, 'ip': None, 'color': '#00FF2A', 'chaos': 50},
    '006': {'mode': 0, 'description': 'Marvin’s Cargo Bay', 'port': 5006, 'last_seen': None, 'ip': None, 'color': '#00FF2A', 'chaos': 50},
}

COMMAND_MAP = {
    0: "RESET",
    1: "COLORPULSE",  # Changed to match Arduino's expected command for DONT_PANIC
    2: "RAINBOW",
    3: "BABELFISH",
    4: "PANGLACTIC",
    5: "IMPROBABILITY",
    6: "VOGONPOETRY",
    7: "GOLDTRAIL",
    8: "BEESWARM",
    9: "LOVEPRESTON",
    10: "COSMICDUST",
    11: "EARTH",
    12: "KRICKET",
    13: "MILLIWAYS",
    14: "CHAOS"  # Added for chaos factor control
}

@app.template_filter('is_active')
def is_active_filter(last_seen):
    return 'Active' if last_seen and (time.time() - last_seen) < 60 else 'Inactive'

@app.route('/', methods=['GET', 'POST'])
def index():
    message = None
    if request.method == 'POST':
        selected_devices = request.form.getlist('devices')
        mode = request.form.get('mode')
        color = request.form.get('color', '').strip().upper()
        chaos = request.form.get('chaos', '').strip()

        if not mode or not mode.isdigit() or int(mode) not in COMMAND_MAP:
            message = "Invalid Guide entry selected"
        else:
            mode = int(mode)
            if 'all' in selected_devices:
                for dev_id in devices:
                    devices[dev_id]['mode'] = mode
                    if color and mode == 1:  # COLORPULSE
                        devices[dev_id]['color'] = color
                    if chaos and mode == 14:  # CHAOS
                        try:
                            chaos_val = int(chaos)
                            if 0 <= chaos_val <= 100:
                                devices[dev_id]['chaos'] = chaos_val
                            else:
                                message = "Chaos value must be between 0 and 100"
                        except ValueError:
                            message = "Invalid chaos value"
                message = message or f"Galaxy updated to {COMMAND_MAP[mode]}"
            else:
                for dev_id in selected_devices:
                    if dev_id in devices:
                        devices[dev_id]['mode'] = mode
                        if color and mode == 1:
                            devices[dev_id]['color'] = color
                        if chaos and mode == 14:
                            try:
                                chaos_val = int(chaos)
                                if 0 <= chaos_val <= 100:
                                    devices[dev_id]['chaos'] = chaos_val
                                else:
                                    message = "Chaos value must be between 0 and 100"
                            except ValueError:
                                message = "Invalid chaos value"
                message = message or f"Selected devices updated to {COMMAND_MAP[mode]}"

    return render_template('index.html', devices=devices, command_map=COMMAND_MAP, message=message)

@app.route('/device/<device_id>', methods=['GET'])
def get_device_command(device_id):
    if device_id not in devices:
        abort(404)
    devices[device_id]['last_seen'] = time.time()
    devices[device_id]['ip'] = request.remote_addr
    mode = devices[device_id]['mode']
    if mode == 1:  # COLORPULSE
        return f"COLORPULSE:{devices[device_id]['color']}\n"
    elif mode == 14:  # CHAOS
        return f"CHAOS:{devices[device_id]['chaos']}\n"
    return f"{COMMAND_MAP[mode]}\n"

@app.template_filter('timestamp_to_time')
def timestamp_to_time_filter(timestamp):
    from datetime import datetime
    return datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S') if timestamp else 'Never'

@app.route('/update_color', methods=['POST'])
def update_color():
    data = request.get_json()
    device_id = data.get('device')
    color = data.get('color')
    if device_id in devices and color:
        devices[device_id]['color'] = color
        if devices[device_id]['mode'] == 1:  # Refresh COLORPULSE
            devices[device_id]['mode'] = 1  # Trigger re-send
        return "Color updated", 200
    return "Invalid request", 400

@app.route('/update_chaos', methods=['POST'])
def update_chaos():
    data = request.get_json()
    device_id = data.get('device')
    chaos = data.get('chaos')
    if device_id in devices and chaos is not None:
        try:
            chaos_val = int(chaos)
            if 0 <= chaos_val <= 100:
                devices[device_id]['chaos'] = chaos_val
                if devices[device_id]['mode'] == 14:  # Refresh CHAOS
                    devices[device_id]['mode'] = 14
                return "Chaos updated", 200
            return "Chaos value must be between 0 and 100", 400
        except ValueError:
            return "Invalid chaos value", 400
    return "Invalid request", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=4999, debug=True)