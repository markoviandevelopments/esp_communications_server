# Blink Mode Controller 🛸

A Hitchhiker's Guide to the Galaxy-inspired LED blink mode controller! 🌌 Control digital LEDs with a web-based interface and customize modes with a toad from the stars.

## Features 🚀
- **Blink Modes**: Choose between None (0), Water Flow with Colors (1), and Rainbow Flow (2).
- **Responsive Design**: Styled with an animated starry background.
- **Interactive Web Interface**: Intuitive UI for setting LED modes.
- **Hitchhiker’s Guide Easter Egg**: Includes a whimsical touch for fans of the series.

## Setup 🛠️

### Prerequisites
- Python 3.7+
- `Flask` and `requests` libraries
- A web browser

### Installation
1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/blink-mode-controller.git
    cd blink-mode-controller
    ```
2. Install dependencies:
    ```bash
    pip install flask requests
    ```
3. Run the main server:
    ```bash
    python3 esp_comm_server.py
    ```
4. Launch the web interface:
    ```bash
    python3 server_5001.py
    ```

5. Open the application in your browser:
    ```
    http://localhost:5001
    ```

## Usage 🎮

1. Open the web interface.
2. Enter the desired blink mode:
   - `0`: No blinking.
   - `1`: Water flow with colors.
   - `2`: Rainbow flow.
3. Submit the form to send the mode to the LED controller.

## Killing the Process 🔪
To free the ports if necessary:
```bash
bash kill_hog.sh
