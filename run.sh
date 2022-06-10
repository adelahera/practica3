#!/usr/bin/bash

EXEC=./bin/Parchis

function enfrentar() {
  echo "${1} vs ${2}, ganador: "
  determinar_victoria "$1" "$2"
}

function ejecutar_y_sacar_ganador() {
  (eval $EXEC --p1 "$1" "primero" --p2 "$2" "segundo" --no-gui) | (grep "Ha ganado el jugador") |
    awk -F '(' '{print $1}' | awk -F ' ' '{print $5}'
}

function determinar_victoria() {
  local resultado
  resultado=$(ejecutar_y_sacar_ganador "$1" "$2")
  local AI
  if [[ "$1" =~ AI\ . ]]; then
    AI="0"
  else
    AI="1"
  fi

  if [[ "$resultado" == "$AI" ]]; then
    echo -e "\e[32mHas ganado\e[0m"
  else
    echo -e "\e[31mHas perdido\e[0m"
  fi
}

function run() {
  make > /dev/null;

  enfrentar "AI ${1}" "Ninja 1"
  enfrentar "Ninja 1" "AI ${1}"
  enfrentar "AI ${1}" "Ninja 2"
  enfrentar "Ninja 2" "AI ${1}"
  enfrentar "AI ${1}" "Ninja 3"
  enfrentar "Ninja 3" "AI ${1}"
}

run "$1";