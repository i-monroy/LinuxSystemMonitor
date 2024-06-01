# PowerShell script to launch multiple client instances in separate windows

# Define the number of client instances to launch. 
$numberOfClients = 3

# Specify the path to the executable of the client. This path points to the location
# where the client executable is stored. 
$exePath = "path/to/executable"

# Loop through the number of clients specified by $numberOfClients.
for ($i = 0; $i -lt $numberOfClients; $i++) {
    # Start-Process is a PowerShell command used to run an executable or script in a separate process.
    # -FilePath specifies the path to the executable to run.
    # -NoNewWindow:$false allows each process to open in its own window, which helps in observing each client's output independently.
    Start-Process -FilePath $exePath -NoNewWindow:$false
}