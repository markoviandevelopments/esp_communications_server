from flask import Flask, request, abort, render_template, redirect, url_for
import time

app = Flask(__name__)

devices = {
    '001': {'mode': 0, 'description': 'Heart of Gold Main Array', 'port': 5000, 'last_seen': None, 'ip': None, 'color': '#00FF2A'},
    '002': {'mode': 0, 'description': 'Vogon Constructor Aux', 'port': 5001, 'last_seen': None, 'ip': None, 'color': '#00FF2A'},
    '003': {'mode': 0, 'description': 'Guide Cockpit Glow', 'port': 5002, 'last_seen': None, 'ip': None, 'color': '#00FF2A'},
    '004': {'mode': 0, 'description': 'Improbability Engine', 'port': 5003, 'last_seen': None, 'ip': None, 'color': '#00FF2A'},
    '005': {'mode': 0, 'description': 'Zaphod’s Cabin Mood', 'port': 5004, 'last_seen': None, 'ip': None, 'color': '#00FF2A'},
    '006': {'mode': 0, 'description': 'Marvin’s Cargo Bay', 'port': 5006, 'last_seen': None, 'ip': None, 'color': '#00FF2A'},
}

COMMAND_MAP = {
    0: "NONE",
    1: "BABELFISH",
    2: "RAINBOW",
    3: "PANGLACTIC",
    4: "IMPROBABILITY",
    5: "VOGONPOETRY",
    6: "GOLDTRAIL",
    7: "TOWELWAVE",
    8: "ZAPHODDUAL",
    9: "GUIDEMATRIX",
    10: "COLORPULSE",
    11: "BEESWARM",
    12: "LOVEPRESTON",
    13: "MARVINMOAN",
    14: "FORDSPARKLE",
    15: "ARTHURTEA",
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

        if not mode or not mode.isdigit() or int(mode) not in COMMAND_MAP:
            message = "Invalid Guide entry selected"
        else:
            mode = int(mode)
            if 'all' in selected_devices:
                for dev_id in devices:
                    devices[dev_id]['mode'] = mode
                    if color and mode == 10:  # Update color for COLORPULSE
                        devices[dev_id]['color'] = color
                message = f"Galaxy updated to {COMMAND_MAP[mode]}"
            else:
                for dev_id in selected_devices:
                    if dev_id in devices:
                        devices[dev_id]['mode'] = mode
                        if color and mode == 10:
                            devices[dev_id]['color'] = color
                message = f"Selected devices updated to {COMMAND_MAP[mode]}"

    return render_template('index.html', devices=devices, command_map=COMMAND_MAP, message=message)

@app.route('/device/<device_id>', methods=['GET'])
def get_device_command(device_id):
    if device_id not in devices:
        abort(404)
    devices[device_id]['last_seen'] = time.time()
    devices[device_id]['ip'] = request.remote_addr
    mode = devices[device_id]['mode']
    return f"COLORPULSE:{devices[device_id]['color']}\n" if mode == 10 else f"{COMMAND_MAP[mode]}\n"

@app.template_filter('timestamp_to_time')
def timestamp_to_time_filter(timestamp):
    from datetime import datetime
    return datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S') if timestamp else 'Never'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=4999, debug=True)