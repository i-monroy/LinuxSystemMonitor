from flask import Flask, jsonify

app = Flask(__name__)

# Example of a software version dictionary
software_version = {
    "version": "1.0.1",
    "update_url": "http://yourserver.com/updates/latest.zip"
}

@app.route('/version-check', methods=['GET'])
def check_version():
    return jsonify(software_version)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)