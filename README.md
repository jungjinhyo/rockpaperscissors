
# Rock Paper Scissors Game with Auto-Updater

### Overview

이 프로젝트는 Qt로 개발된 간단한 가위바위보 게임으로, 자동 업데이트 기능을 포함하고 있습니다. `MainWindow` 클래스는 가위바위보 게임의 UI 및 기능을 제공하고, `version_manager` 모듈은 GitHub API를 통해 최신 버전을 확인하고 업데이트 런처를 실행하는 기능을 수행합니다.

---

## Requirements

- **Qt 5 또는 6** (필요 모듈: `QtCore`, `QtGui`, `QtWidgets`, `QtNetwork`, `QtJson`)
- **GitHub Personal Access Token** (자동 버전 업로드 시 필요)
- Windows OS 호환

---

## Project Structure

- **main.cpp**: 프로그램의 진입점으로, 현재 버전을 설정하고 업데이트를 확인한 후 최신 버전일 경우 게임을 실행합니다.
- **mainwindow.h / mainwindow.cpp**: `MainWindow` 클래스는 가위바위보 게임의 UI와 게임 로직을 구현합니다.
- **version_manager.h / version_manager.cpp**: GitHub API와 상호작용하여 버전을 비교하고, 새로운 버전이 있을 경우 업데이트 런처를 실행합니다.

---

## How to Build

1. **Qt 설치**: 최신 Qt 버전을 설치합니다.
2. **저장소 클론**:
   ```bash
   git clone https://github.com/username/rock-paper-scissors-auto-updater.git
   ```
3. **Qt Creator에서 열기**:
   - **Qt Creator**에서 `rock-paper-scissors-auto-updater.pro` 파일을 엽니다.
4. **빌드 및 실행**:
   - 배포용 **Release** 구성 또는 테스트용 **Debug** 구성을 사용하여 빌드합니다.
   - **Qt Creator**에서 프로젝트를 실행할 수 있습니다.

---

## Usage

### 애플리케이션 실행

애플리케이션을 실행하면, 현재 버전과 GitHub에 저장된 최신 버전을 비교하여 최신 버전일 경우 게임이 실행되고, 그렇지 않을 경우 업데이트 런처가 실행됩니다.

1. **가위바위보 게임**: 사용자가 가위, 바위, 보 버튼 중 하나를 클릭하면 컴퓨터의 선택과 결과가 표시됩니다.
2. **업데이트 확인**: `version_manager` 모듈이 GitHub API를 통해 `version.json` 파일의 최신 버전을 확인하고, 새로운 버전이 있는 경우 `UpdaterLauncher`를 실행하여 업데이트합니다.

### 주요 함수

- **checkForUpdate()**: GitHub에서 최신 버전을 확인하고 업데이트가 필요할 경우 런처를 실행합니다.
- **compareVersions()**: 현재 버전과 최신 버전을 비교하여 업데이트 여부를 판단합니다.
- **uploadVersionJson()**: GitHub에 새로운 버전을 업로드합니다.

### 예제

1. 현재 버전: `v1.2.0`
2. 최신 버전: `v1.3.0`

최신 버전이 존재할 경우, `UpdaterLauncher.exe`가 실행되어 새로운 버전으로 업데이트됩니다.

---

## Known Issues

- **GitHub API Rate Limit**: GitHub API 사용 시 비공개 리포지토리 접근을 위해 Personal Access Token이 필요하며, Rate Limit을 초과할 수 있습니다.
- **UpdaterLauncher 실행 실패**: `UpdaterLauncher` 경로가 잘못되었거나 파일이 존재하지 않을 경우 업데이트가 실행되지 않습니다.

---

## License

이 프로젝트는 RGBLAB 라이선스 하에 제공됩니다.

---

### Author

개발자: Jung jinhyo

--- 

