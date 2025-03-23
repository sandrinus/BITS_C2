from flask import Flask, send_from_directory, request
import os
import time
from threading import Thread

app = Flask(__name__)

# Directory to host files
UPDATE_DIRECTORY = 'updates'

# Ensure the directory exists
os.makedirs(UPDATE_DIRECTORY, exist_ok=True)

def modify_file_after_delay(filename):
    """Modify the file after a delay."""
    time.sleep(20)  # Simulate waiting for the download to complete
    file_path = os.path.join(UPDATE_DIRECTORY, filename)
    if os.path.exists(file_path):
        with open(file_path, 'w') as file:
            file.write("wait")  # Modify the file (write 'wait' in it)

@app.route('/updates/<filename>', methods=['GET'])
def download_file(filename):
    """Send the requested file from the updates directory."""
    return send_from_directory(directory=UPDATE_DIRECTORY, path=filename, as_attachment=True)

@app.after_request
def after_request(response):
    """This function runs after the request is processed."""
    # Check if the response status code is 200 (indicating file has been sent)
    if response.status_code == 200:
        filename = request.view_args.get('filename')
        if filename:
            print(f"File requested: {filename}")
            
            # Run the file modification asynchronously to avoid blocking the response
            thread = Thread(target=modify_file_after_delay, args=(filename,))
            thread.start()

    return response  # Return the response as it is, no need to modify it

if __name__ == '__main__':
    # Run the Flask app
    app.run(host="0.0.0.0", port=8080)
