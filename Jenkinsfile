pipeline {
    agent any

    stages {
        stage('Start OTA Server') {
            steps {
                echo 'Starting OTA Server...'
                sh 'nohup python3 src/ota_server.py &'
                sleep 10 // Ensures the server is up
            }
        }

        stage('Build') {
            steps {
                echo 'Building the project...'
                sh 'mkdir -p build'
                sh 'g++ -o build/system_monitor_server src/system_monitor_server.cpp -Iinclude -std=c++11 -pthread -lcurl -lssl -lcrypto -ldl'
            }
        }

        stage('Test') {
            steps {
                echo 'Running tests...'
                // Commands to execute your tests
            }
        }

        stage('Archive Artifacts') {
            steps {
                echo 'Archiving executables...'
                archiveArtifacts artifacts: 'build/system_monitor_server', onlyIfSuccessful: true
            }
        }

        stage('Cleanup') {
            steps {
                echo 'Cleaning up temporary files...'
                sh 'find build/ -type f ! -name "system_monitor_server" -delete'
            }
        }
    }
}
