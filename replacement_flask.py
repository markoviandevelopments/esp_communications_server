from flask import Flask, request, jsonify, redirect, url_for
from flask_socketio import SocketIO, emit
from flask_cors import CORS
import os
import requests
import re
import json
import datetime
import ast
import mysql.connector
import threading
import logging

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

app = Flask(__name__, static_url_path="/leds/static")
app.config["APPLICATION_ROOT"] = "/leds"
socketio = SocketIO(app, cors_allowed_origins="https://markovianchats.duckdns.org")
CORS(app)

API_URL = "http://50.188.120.138:5049/api/deepseek"
LAST_RESULT = {"status": "No request yet", "timestamp": None, "raw_output": "", "think_content": ""}
ANIMATION_DATA = {"frames": [], "frame_rate": 0.1, "type": "static"}
animation_lock = threading.Lock()

db_config = {
    "host": os.getenv("DB_HOST", "localhost"),
    "user": os.getenv("DB_USER", "log_user"),
    "password": os.getenv("DB_PASSWORD", "Not$erp011"),
    "database": os.getenv("DB_NAME", "client_logs_db")
}

def get_db_connection():
    try:
        return mysql.connector.connect(**db_config)
    except mysql.connector.Error as e:
        logger.error(f"Database Connection Error: {e}")
        return None

def query_api(user_prompt, temperature=0.7):
    params = {"prompt": user_prompt, "temperature": temperature}
    try:
        response = requests.get(API_URL, params=params, timeout=150)
        response.raise_for_status()
        return response.json().get("response", "").strip() or "No response received."
    except requests.RequestException as e:
        logger.error(f"API Request Error: {e}")
        return f"Error: {e}"

def broadcast_pattern(pattern_data):
    """Send pattern directly to connected ESP-01 clients."""
    socketio.emit('pattern_update', pattern_data, namespace='/leds')

@app.route("/leds", methods=["GET", "POST"])
def index():
    global LAST_RESULT, ANIMATION_DATA

    if request.method == "POST":
        request_data = request.get_json() if request.is_json else request.form
        theme = request_data.get("theme")
        temp = float(request_data.get("temperature", 0.7))
        pattern_type = request_data.get("pattern_type", "static")

        if not theme:
            return jsonify({"error": "Theme is required"}), 400

        prompt_templates = {
            "static": (
                f'Generate exactly 10 RGB tuples as a Python list that fully embodies the theme "{theme}". '
                f'Each color must be highly representative of this theme, avoiding unnecessary variety. '
                f'If the theme is known for specific colors (e.g., camouflage = shades of green/brown), ensure the output maintains that consistency. '
                f'Do NOT introduce unrelated colors—every color must strongly reinforce the theme. '
                f'Repeat colors when appropriate rather than adding extra variety. '
                f'Return ONLY the list, with NO extra text, formatting, explanations, or symbols. '
                f'The response must strictly be formatted as: @[[R, G, B], [R, G, B], ..., [R, G, B]].'
            ),
            "animated": (
                f'Generate an LED animation sequence for the theme "{theme}". '
                f'Return a Python dict with: '
                f'- "frames": a list of at least 5 frames, each frame being a list of exactly 10 RGB tuples ([R, G, B]), '
                f'  where colors strongly embody the theme with smooth transitions or dynamic patterns (e.g., waves, pulses). '
                f'- "frame_rate": a float (seconds per frame, between 0.05 and 1.0) to control animation speed. '
                f'Ensure all colors reinforce the theme consistently. '
                f'Return ONLY the dict, formatted as: @{{"frames": [[[...], [...]], ...], "frame_rate": float}}.'
            )
        }
        user_prompt = prompt_templates.get(pattern_type, "static")

        ip_address = request.remote_addr
        result = query_api(user_prompt, temperature=temp)
        raw_result = result

        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        led_data, status = None, "Fail"

        think_match = re.search(r'<think>.*?</think>', raw_result, re.DOTALL)
        think_content = think_match.group(0) if think_match else "No thinking process provided."
        cleaned_result = re.sub(r'<think>.*?</think>', '', raw_result, flags=re.DOTALL).strip()

        if pattern_type == "static":
            match = re.search(r"\[\s*\[\s*\d+\s*,\s*\d+\s*,\s*\d+\s*\](?:\s*,\s*\[\s*\d+\s*,\s*\d+\s*,\s*\d+\s*\]){9}\s*\]", cleaned_result)
            if match:
                try:
                    raw_list = match.group(0)
                    parsed_data = ast.literal_eval(raw_list)
                    if (isinstance(parsed_data, list) and len(parsed_data) == 10 and 
                        all(isinstance(t, list) and len(t) == 3 and all(0 <= v <= 255 for v in t) for t in parsed_data)):
                        led_data = parsed_data
                        status = "Pass"
                        with animation_lock:
                            ANIMATION_DATA = {"frames": [led_data], "frame_rate": 0.1, "type": "static"}
                        broadcast_pattern({"pattern": led_data})
                except Exception as e:
                    led_data = f"Parsing Error: {e}"
                    logger.error(led_data)
            else:
                led_data = "Format Error: No valid static pattern found"

        elif pattern_type == "animated":
            match = re.search(r'(?:@)?\{.*"frames":.*"frame_rate":.*\}', cleaned_result, re.DOTALL)
            if not match:
                match = re.search(r'```python\s*(\{.*"frames":.*"frame_rate":.*\})\s*```', cleaned_result, re.DOTALL)
            if match:
                try:
                    dict_str = match.group(1) if match.lastindex else match.group(0).lstrip('@')
                    led_data = ast.literal_eval(dict_str)
                    frames = led_data["frames"]
                    frame_rate = led_data["frame_rate"]
                    if (len(frames) >= 5 and 
                        all(len(f) == 10 and all(len(c) == 3 and all(0 <= v <= 255 for v in c) for c in f) for f in frames) and 
                        0.05 <= frame_rate <= 1.0):
                        status = "Pass"
                        with animation_lock:
                            ANIMATION_DATA = {"frames": frames, "frame_rate": frame_rate, "type": "animated"}
                        broadcast_pattern({"frames": frames, "frame_rate": frame_rate})
                    else:
                        led_data = f"Validation Error: Frames: {len(frames)}, Frame Rate: {frame_rate}"
                except Exception as e:
                    led_data = f"Parsing Error: {e}"
            else:
                led_data = "Format Error: No valid animation data"

        LAST_RESULT = {
            "status": status,
            "timestamp": timestamp,
            "raw_output": raw_result,
            "pattern_type": pattern_type,
            "think_content": think_content if think_content != "No thinking process provided." else None,
            "data": led_data if status == "Pass" else None
        }

        conn = get_db_connection()
        if conn:
            try:
                with conn.cursor() as cursor:
                    cursor.execute(
                        """
                        INSERT INTO led_history (timestamp, prompt, theme, temperature, ip_address, pattern_generated, 
                                                pattern_type, api_response_length, status, raw_output, think_content)
                        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
                        """,
                        (timestamp, user_prompt, theme, temp, ip_address,
                         json.dumps(led_data) if led_data else None, pattern_type,
                         len(raw_result), status, raw_result,
                         think_content if think_content != "No thinking process provided." else None)
                    )
                conn.commit()
            except mysql.connector.Error as e:
                logger.error(f"Database Insert Error: {e}")
            finally:
                conn.close()

        return jsonify(LAST_RESULT)

    return jsonify(LAST_RESULT)

@app.route('/')
def redirect_to_leds():
    try:
        return app.send_static_file('led_app.html')
    except FileNotFoundError:
        return jsonify({"error": "Frontend file led_app.html not found in static folder"}), 404

@socketio.on('connect', namespace='/leds')
def handle_connect():
    logger.info("ESP-01 connected to WebSocket")
    emit('connection_ok', {'status': 'connected'})

@socketio.on('disconnect', namespace='/leds')
def handle_disconnect():
    logger.info("ESP-01 disconnected from WebSocket")

if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=5047, debug=True)