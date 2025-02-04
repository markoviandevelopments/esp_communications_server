from flask import Flask, request, abort

app = Flask(__name__)

# Global variable for blink mode:
#   0: Do not send any command (client may then do occasional short blinks)
#   1: Send "SLOW" (client should do 10 slow blinks)
#   2: Send "FAST" (client should do 10 fast blinks)
BLINK_MODE = 1  # Set your initial mode here

@app.route('/', methods=['GET'])
def serve_blink_mode():
    global BLINK_MODE

    # Optional: allow updating BLINK_MODE via a URL parameter, e.g.:
    # http://your_ip:5000/?mode=2
    mode_param = request.args.get('mode')
    if mode_param is not None:
        try:
            new_mode = int(mode_param)
            if new_mode in (0, 1, 2):
                BLINK_MODE = new_mode
                return f"Blink mode updated to {BLINK_MODE}\n", 200
            else:
                abort(400, "Invalid mode. Use 0 (none), 1 (slow), or 2 (fast).")
        except ValueError:
            abort(400, "Mode must be an integer (0, 1, or 2).")

    # Respond with the command based on the current blink mode.
    if BLINK_MODE == 2:
        return "FAST\n", 200
    elif BLINK_MODE == 1:
        return "SLOW\n", 200
    else:
        # For BLINK_MODE == 0, we return an empty response.
        # You could also return a specific message or a 204 status if preferred.
        return "", 200

if __name__ == '__main__':
    # Run the server on all interfaces so that it is accessible externally.
    app.run(host='0.0.0.0', port=5000)
