void parseCommand(char* command) {
  // Split the command into its individual parts
  char* parts[16];
  int numParts = 0;
  char* part = strtok(command, " ");
  while (part != NULL) {
    parts[numParts] = part;
    numParts++;
    part = strtok(NULL, " ");
  }
  
  // Check the command type (G-code or M-code)
  if (parts[0][0] == 'G') {
    // G-code command
    switch (atoi(parts[0])) {
      case 0:  // Rapid move
        // Set the motors to their maximum speed
        // and move them to the specified positions
        // ...
        break;
      case 1:  // Controlled move
        // Set the motors to a specific speed
        // and move them to the specified positions
        // ...
        break;
      case 4:  // Delay
        // Delay for the specified time
        // ...
        break;
      // Add more G-code commands as needed
    }
  } else if (parts[0][0] == 'M') {
    // M-code command
    switch (atoi(parts[0])) {
      case 3:  // Enable spindle
        // Enable the spindle
        // ...
        break;
      case 4:  // Disable spindle
        // Disable the spindle
        // ...
        break;
      // Add more M-code commands as needed
    }
  }
}
