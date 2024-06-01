from flask import Flask, jsonify

# Create a Flask application instance
app = Flask(__name__)

# This dictionary simulates a database or configuration file
# where software version details are stored.
software_version = {
    "version": "1.0.1",
    "update_url": "http://yourserver.com/updates/latest.zip",
    "checksum": "checksum value"
}

# Define a route for the version check. This route is accessible
# via HTTP GET requests and will return the software version information.
@app.route('/version-check', methods=['GET'])
def check_version():
    # Return the software_version dictionary as a JSON response.
    return jsonify(software_version)

if __name__ == '__main__':
    # Start the Flask application on all available IP addresses
    # of this machine (`0.0.0.0`) and on port 5000.
    app.run(host='0.0.0.0', port=5000)
