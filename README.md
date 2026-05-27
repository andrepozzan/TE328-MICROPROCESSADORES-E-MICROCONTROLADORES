# 🛠️ TE328/DB — Microprocessadores e Microcontroladores (2026/1)

<div align="center">
  <img src="https://img.shields.io/badge/Platform-STM32Cube-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white" alt="STM32 Platform">
  <img src="https://img.shields.io/badge/Hardware-Nucleo--G491RE-007ACC?style=for-the-badge" alt="Nucleo-G491RE">
  <img src="https://img.shields.io/badge/Environment-VS%20Code%20%2B%20Zsh-1E1E2E?style=for-the-badge&logo=visual-studio-code&logoColor=79C0FF" alt="VS Code">
</div>

---

## 📌 Sobre o Repositório

Este repositório contém os códigos-fonte, laboratórios e projetos desenvolvidos durante o semestre de **2026/1** na disciplina **TE328/DB — Microprocessadores e Microcontroladores**. 

O objetivo principal das atividades é o aprendizado prático de arquitetura de computadores, periféricos de hardware e sistemas embarcados utilizando programação em **C (Bare-Metal e HAL)** na placa de desenvolvimento **STM32 NUCLEO-G491RE** (arquitetura ARM Cortex-M4).

---

## 🎛️ Especificações do Hardware

* **Placa:** NUCLEO-G491RE (STMicroelectronics)
* **Microcontrolador:** STM32G491RETx
* **Core:** ARM® 32-bit Cortex®-M4 com FPU
* **Frequência Máxima:** 170 MHz
* **Memória:** 512 KB Flash / 112 KB SRAM
* **Gravador Integrado:** ST-LINK V3E

---

## 📂 Estrutura de Pastas sugerida

```text
├── .vscode/               # Configurações de Debug locais (Cortex-Debug/Launch)
├── Core/                  # Código principal do projeto
│   ├── Inc/               # Arquivos de cabeçalho (.h)
│   └── Src/               # Código fonte (.c) (main.c, stm32g4xx_it.c, etc)
├── Drivers/               # Drivers CMSIS e STM32G4xx_HAL_Driver
├── .gitignore             # Filtro de arquivos de build e binários
├── CMakeLists.txt         # Arquivo de configuração de build do CMake
└── README.md              # Documentação do repositório