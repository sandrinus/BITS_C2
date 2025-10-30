#!/usr/bin/python3

from flask import Flask, send_from_directory, request, abort
import os
import time
from threading import Thread
import requests

app = Flask(__name__)

# Directory to host files
UPDATE_DIRECTORY = 'updates'
BASE_DIRECTORY = 'clients'

# Ensure the directory exists
os.makedirs(UPDATE_DIRECTORY, exist_ok=True)

@app.route('/updates/<filename>', methods=['GET', 'HEAD'])
def download_update(filename):
    """Send the requested file from the updates directory."""
    return send_from_directory(directory=UPDATE_DIRECTORY, path=filename, as_attachment=True)

@app.route('/clients/<path:requested_path>', methods=['GET', 'HEAD'])
def download_file(requested_path):
    """Serve a requested file from the /clients directory securely."""
    requested_path = requested_path.replace("\\", "/")  # Convert backslashes to forward slashes
    # print("Requested path", requested_path)
    
    full_path = os.path.join(BASE_DIRECTORY, requested_path)
    # print("Full Path", full_path)

    client_ip = '10.20.1.1' # request.remote_addr
    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] File requested: {requested_path} from {client_ip}")

    # Pwnboard notification
    # url = "https://www.pwnboard.win/pwn"
    # payload = {"ip": client_ip, "application": "BITSC2", "access_type": "http c2"}
    # resp = requests.post(url, json=payload, headers={"Content-Type": "application/json"})
    # assert resp.text.strip() == "valid"

    # Ensure the file exists
    if not os.path.isfile(full_path):
        abort(404)  # File not found

    return send_from_directory(directory=BASE_DIRECTORY, path=requested_path, as_attachment=True)

if __name__ == '__main__':
    # Run the Flask app
    app.run(host="0.0.0.0", port=8080)


