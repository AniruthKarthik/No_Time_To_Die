# No Time to Die  

**Built with:**  
- **Language:** C++  
- **Library:** Raylib  

## About the Game  
**No Time to Die** is a fast-paced survival game where time speeds up as you move. Your objective is to navigate through a chaotic environment, avoid falling balls, and survive as long as possible while racking up the highest score.

Key Features:  
- **Dynamic Time Mechanics**: Movement accelerates the passage of time.  
- **Interactive Gameplay**: Avoid obstacles and stay alive.  
- **Click-to-Destroy Yellow Balls**: Unlock bonus points by destroying special yellow balls.  
- **Real-Time Feedback**: Visual and auditory effects enhance the gameplay experience.  

## Gameplay Instructions  
1. **Controls**:  
   - **Move**: Use `W`, `A`, `S`, `D` keys.  
   - **Click**: Left-click to destroy yellow balls (when available).  

2. **Objective**:  
   - Survive for as long as you can.  
   - Avoid collisions with falling balls.  
   - Score points by staying alive and destroying special yellow balls.

3. **Scoring**:  
   - **Movement**: Score increases when you move.  
   - **Yellow Balls**: Bonus points are awarded for destroying yellow balls.

## Prerequisites  
You only need to install **Make**. The build script will attempt to install the C++ compiler (g++) and libraries automatically if they are missing.

### Windows  
- **via Chocolatey** (Recommended):  
  Run PowerShell as Administrator and type:  
  ```powershell
  choco install make
  ```

### Linux  
- **Arch Linux**: `sudo pacman -S make`  
- **Debian/Ubuntu**: `sudo apt update && sudo apt install make`  

### macOS  
- **via Homebrew**: `brew install make`  
- *Note: You may be prompted to install Xcode Command Line Tools automatically.*

## How to Run  
1. Clone the repository:  
   ```bash
   git clone https://github.com/AniruthKarthik/No_Time_To_Die.git
   cd No_Time_To_Die
   ```
2. Build the game (this will also attempt to install dependencies if missing on Linux/macOS):  
   ```bash
   make game
   ```
3. Run the game:  
   ```bash
   make run
   ```

## Assets  
- **Background**: Custom visual assets to create an immersive experience.  
- **Hero Sprite**: `assets/scarfy.png`  
- **Music**: Dynamic soundtrack included (`assets/mdmp3.mp3`).  

## Future Enhancements  
- Add levels with increasing difficulty.  
- Introduce power-ups and new obstacles.  
- Include a leaderboard system to track high scores.  

## Feedback  
Found a bug or have suggestions? Feel free to open an issue or contribute to the project!  

---

Let me know if you'd like to customize it further!
