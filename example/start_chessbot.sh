#!/bin/bash

# Variabelen
BOT_NAME="chessbot"
BOT_PATH="$(pwd)/$BOT_NAME"  # Pad naar de bot executable
CUTECHESS_GUI="/Applications/cutechess"  # Cute Chess pad
CUTECHESS_CLI="/Applications/cutechess-cli"

# 1️⃣ Sluit Cute Chess als het al draait
pkill -f CuteChess
echo "Closed Cute Chess."

# 2️⃣ Compileer de bot
make
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi
echo "Build successful."

open -a "$CUTECHESS_GUI"

# 3️⃣ Laad de bot in Cute Chess
# if [ -f "$CUTECHESS_CLI" ]; then
#     echo "Starting a game..."
#     "$CUTECHESS_CLI" \
#     -engine name="Stockfish" cmd="/Users/olehendrix/Downloads/stockfish_2/stockfish-macos-x86-64" proto=uci \
#     -engine name="$BOT_NAME" cmd="$BOT_PATH" proto=uci \
#     -each tc=300/40 \
#     -games 1 -rounds 1
# else
#     echo "Cute Chess CLI niet gevonden! Installeer het met: brew install cutechess"
# fi
