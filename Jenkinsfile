pipeline {
    agent any

    stages {
        stage('Start OTA Server') {
            steps {
                echo 'Starting OTA Server...'
                sh 'nohup python3 src/ota_server.py &'
                sleep 10 // Add a delay to ensure the server has started, adjust as necessary
            }
        }

        stage('Build') {
            steps {
                echo 'Building the project...'
                sh 'mkdir -p build'
                sh 'g++ -o build/system_monitor_server src/system_monitor_server.cpp -Iinclude -std=c++11 -pthread -lcurl -lssl -lcrypto -ldl'
            }
        }

        stage('Archive Artifacts') {
            steps {
                echo 'Archiving artifacts...'
                archiveArtifacts artifacts: '/home/pi/Desktop/LinuxSystemMonitor/build/system_monitor_server', onlyIfSuccessful: true
            }
        }

        stage('Set Permissions') {
            steps {
                echo 'Setting executable permissions...'
                sh 'chmod +x /home/pi/Desktop/LinuxSystemMonitor/build/system_monitor_server'
            }
        }
      
        stage('Test') {
            steps {
                echo 'Running tests...'
                // Add commands to execute your tests here
            }
        }

    }
}
