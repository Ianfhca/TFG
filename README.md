# SNN Simulator using LIF neuron model

This project implements a **Spiking Neural Network (SNN) simulator** written in C++, with evaluation and visualization tools in Python. The goal is to offer a biologically inspired simulation for event-based classification tasks using spike data, based on the **Leaky Integrate-and-Fire (LIF)** model and **STDP** learning rules.

## Key Features

- Fully implemented in C++ from scratch
- Leaky Integrate-and-Fire (LIF) neuron model
- Multiplicative **STDP** learning rule
- Input processing from the **IBM DVS128 Gesture Dataset**
- Flexible configuration using `.conf` files
- Support for convolutional and dense layers
- Automatic evaluation and visualization using Python (`matplotlib`, `seaborn`)

## 🗂 Project Structure


## Requirements

Project/\
│\
├── src/\
│   ├── SNN.cpp\
│   ├── Layer.cpp\
│   ├── Neuron.cpp\
│   └── ...\
│\
├── include/\
│   ├── SNN.hpp\
│   ├── Layer.hpp\
│   └── ...\
│\
├── output/\
│   └── [topology]/\
│\
├── evaluation/\
│   ├── evaluate_train.py\
│   └── generate_all.py\
│\
├── topology/\
│\
├── README.md # This file\
└── Makefile # For compiling the simulator\

### C++
- `g++` or any C++17-compatible compiler
- `make` (optional, for compiling using the Makefile)

### Python (for evaluation)

```bash
pip install pandas matplotlib seaborn
