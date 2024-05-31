pipeline {
    agent any  // This tells Jenkins to run this pipeline on any available agent

    stages {
        stage('Build') {
            steps {
                sh 'mkdir -p build'
                sh 'g++ -o build/system_monitor_server src/system_monitor_server.cpp -Iinclude -std=c++11 -pthread -lcurl -lssl -lcrypto -ldl'
            }
        }
        stage('Test') {
            steps {
                sh 'echo "Running tests..."'
                // Add your test scripts here
            }
        }
        stage('Cleanup') {
            steps {
                sh 'echo "Clean up build artifacts"'
                sh 'rm -rf build/*'
            }
        }
    }
}