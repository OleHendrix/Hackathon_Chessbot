#!/bin/bash

# Variabelen
BOT_NAME="chessbot"
BOT_PATH="$(pwd)/$BOT_NAME"  # Pad naar de bot executable
CUTECHESS_GUI="/Applications/cutechess"  # Cute Chess GUI pad

# 1️⃣ Controleer of Cute Chess geïnstalleerd is
if [ ! -f "$CUTECHESS_GUI" ]; then
    echo "Cute Chess niet gevonden! Installeer het met: brew install cutechess"
    exit 1
fi

# 2️⃣ Compileer de bot
make
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi
echo "Build successful."

# 3️⃣ Controleer of de bot uitvoerbaar is
if [ ! -x "$BOT_PATH" ]; then
    echo "Bot is niet uitvoerbaar! Maak het uitvoerbaar met: chmod +x $BOT_PATH"
    exit 1
fi

# 4️⃣ Controleer of de bot UCI-output geeft
echo "uci" | "$BOT_PATH"
if [ $? -ne 0 ]; then
    echo "Fout: Bot geeft geen geldige UCI-output. Controleer je implementatie."
    exit 1
fi

# 5️⃣ Open Cute Chess GUI met jouw bot
echo "Starting Cute Chess GUI..."
open -a "$CUTECHESS_GUI" --args -engine name="$BOT_NAME" cmd="$BOT_PATH" proto=uci
