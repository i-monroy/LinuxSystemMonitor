pipeline {
    agent any

    stages {
        stage('Check Environment') {
            steps {
                echo 'Checking the current directory and listing files...'
                sh 'pwd'
                sh 'ls -la'
            }
        }

        stage('Start OTA Server') {
            steps {
                echo 'Starting OTA Server...'
                sh 'nohup python3 src/ota_server.py &'
                sleep 10 // Ensures the server is up
            }
        }
    
        stage('Build') {
            steps {
                echo 'Building the main project...'
                sh 'mkdir -p build'
                sh 'g++ -o build/system_monitor_server src/system_monitor_server.cpp -Iinclude -std=c++11 -pthread -lcurl -lssl -lcrypto -ldl'
            }
        }

        stage('Start Server') {
            steps {
                echo 'Starting the server...'
                sh 'nohup ./build/system_monitor_server &'
                sleep 10 // Wait for the server to initialize
            }
        }

        stage('Compile and Run Tests') {
            steps {
                echo 'Ensuring test directory exists...'
                sh 'mkdir -p tests'
                echo 'Compiling Connection Tests...'
                sh 'g++ -std=c++17 -isystem /usr/local/include/gtest/ -pthread tests/connection_tests.cpp /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a -o tests/connection_tests'
                echo 'Running Connection Tests...'
                sh './tests/connection_tests'
                echo 'Compiling Data Transmission Tests...'
                sh 'g++ -std=c++17 -isystem /usr/local/include/gtest/ -pthread tests/data_trans_tests.cpp /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a -o tests/data_trans_tests -lssl -lcrypto'
                echo 'Running Data Transmission Tests...'
                sh './tests/data_trans_tests'
            }
        }

        stage('Shutdown Server') {
            steps {
                echo 'Shutting down the server...'
                sh 'pkill -f system_monitor_server'
            }
        }

        stage('Cleanup') {
            steps {
                echo 'Cleaning up...'
                sh 'rm -rf build/*'
                sh 'rm tests/connection_tests'
                sh 'rm tests/data_trans_tests'
            }
        }
    }
}