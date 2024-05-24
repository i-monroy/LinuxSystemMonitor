from flask import Flask, jsonify

app = Flask(__name__)

# Example software version dictionary including checksum
software_version = {
    "version": "1.0.1",
    "update_url": "http://yourserver.com/updates/latest.zip",
    "checksum": "4b04afbfd0bb3abd4789c932d3322c7d3a0a553d77e0e0fe6dca079ebaf6998a"
}

@app.route('/version-check', methods=['GET'])
def check_version():
    return jsonify(software_version)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)