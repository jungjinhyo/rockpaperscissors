
# RockPaperScissors with Auto-Updater

This is a Qt-based application for a Rock-Paper-Scissors game that includes an auto-update feature. The application checks for the latest version using AWS API Gateway and DynamoDB and launches an updater if a new version is available.

Final document update date : 2024.11.26

## Features

### Game Features
- Play Rock-Paper-Scissors with a computer opponent.
- Simple UI for user interaction.

### Update Features
- **Version Comparison**: Compares the current version with the latest version.
- **Version Management**: Uploads and retrieves version information from DynamoDB using AWS API Gateway.
- **Auto-Updater**: Automatically launches an updater to download and install the latest version.

## Project Structure

### Key Files and Classes
1. **`main.cpp`**
   - Entry point of the application.
   - Checks for updates before launching the game.

2. **`MainWindow`**
   - Manages the game UI and gameplay logic.
   - Handles user interactions like choosing Rock, Paper, or Scissors.

3. **`version_manager.h`**
   - Provides functions to compare versions, retrieve the latest version, and upload version information to DynamoDB.

### Components
- **Game Logic**:
  - `computerChoice()`: Generates the computer's move randomly.
  - `determineWinner()`: Determines the winner based on the player's and computer's moves.

- **Version Management**:
  - `compareVersions()`: Compares two version strings to determine which is newer.
  - `getLatestVersionFromDynamoDB()`: Retrieves the latest version of the program from DynamoDB via API Gateway.
  - `uploadVersionToDynamoDB()`: Uploads the current version information to DynamoDB.

- **Auto-Updater**:
  - Checks if a new version is available.
  - Launches `UpdaterLauncher.exe` to handle the update process.

## API Endpoints
- **Get All Latest Versions**:
  - URL: `https://0b3xq5xnkd.execute-api.ap-northeast-2.amazonaws.com/version/getAllLatestVersions`
  - Retrieves the latest version for all programs.
- **Add or Update Version**:
  - URL: `https://0b3xq5xnkd.execute-api.ap-northeast-2.amazonaws.com/version/AddOrUpdateVersion`
  - Updates the version information for a specific program.

## Usage

### Prerequisites
- Qt 5 or higher installed.
- AWS services (API Gateway and DynamoDB) configured for version management.

### Running the Application
1. Compile the project using Qt Creator.
2. Run the application. It will:
   - Check for updates.
   - Launch the game if no update is required.

### Playing the Game
- Choose your move (Rock, Paper, or Scissors) using the buttons.
- See the computer's choice and the result on the screen.
- Click "Play Again" to restart the game.

### Auto-Updater
- If a new version is available:
  - The application launches `UpdaterLauncher.exe` with the new version details.
  - The game exits automatically, and the updater handles the update process.

## Customization
- Modify `CURRENT_VERSION` and `PROGRAM_NAME` in `main.cpp` to set the current program version and name.
- Update API URLs (`getAllLatestVersionsAPI` and `add_or_update_versionAPI`) if the endpoints change.

## Contribution
Feel free to contribute by opening issues or submitting pull requests for new features or bug fixes.

## License
This project is licensed under the MIT License. See `LICENSE` for details.
